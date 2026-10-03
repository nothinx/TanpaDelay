# Compile + jalankan simulasi.cpp (kode library asli), lalu render grafik ke ../gambar/.
# Jalankan dari folder ini: python gambar.py   (butuh g++ dan matplotlib)
import csv, glob, io, os, subprocess, tempfile
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

plt.rcParams.update({
    "figure.figsize": (8, 3.6), "figure.dpi": 100, "savefig.bbox": "tight", "savefig.pad_inches": 0.15,
    "figure.facecolor": "white", "axes.facecolor": "white", "savefig.facecolor": "white",
    "font.size": 10, "axes.titlesize": 11, "axes.titleweight": "bold", "axes.titlelocation": "left",
    "axes.spines.top": False, "axes.spines.right": False, "axes.edgecolor": "#9ca3af",
    "axes.grid": True, "grid.color": "#e5e7eb", "grid.linewidth": 0.8,
    "legend.frameon": False, "svg.fonttype": "path", "svg.hashsalt": "nothinx",
    "lines.linewidth": 1.8,
})
WARNA = {"utama": "#2563eb", "pembanding": "#dc2626", "ketiga": "#16a34a", "keempat": "#9333ea",
         "kelima": "#ea580c", "mentah": "#9ca3af", "target": "#111827"}
KELUAR = os.path.join("..", "gambar")


def koma(x, d=1):
    return f"{x:.{d}f}".replace(".", ",")


def simpan(fig, nama):
    fig.savefig(os.path.join(KELUAR, nama), format="svg", metadata={"Date": None})
    plt.close(fig)


def jalankan(sim, skenario):
    keluar = subprocess.run([sim, skenario], capture_output=True, text=True, check=True).stdout
    return list(csv.DictReader(io.StringIO(keluar)))


def grafik_drift(baris):
    fig, ax = plt.subplots()
    hasil = {}
    for jenis, warna, label in [("lama", WARNA["pembanding"], "terakhir = millis()"),
                                ("tanpadelay", WARNA["utama"], "TanpaDelay")]:
        b = [r for r in baris if r["jenis"] == jenis]
        t = [int(r["waktu"]) / 60000 for r in b]
        sel = [(int(r["waktu"]) - int(r["n"]) * 500) / 1000 for r in b]
        ax.plot(t, sel, color=warna)
        ax.annotate(label, (t[-1], sel[-1]), xytext=(-4, 6), textcoords="offset points",
                    ha="right", va="bottom", color=warna)
        hasil[jenis] = (sel[-1], max(sel) * 1000, len(b))
    menit = round(int(baris[-1]["waktu"]) / 60000)
    ax.set_title(f"Setelah {menit} menit, pola terakhir = millis() tertinggal {koma(hasil['lama'][0])} detik;\n"
                 f"TanpaDelay tetap di jadwal (selisih ≤ {hasil['tanpadelay'][1]:.0f} ms)")
    ax.set_xlabel("waktu (menit)")
    ax.set_ylabel("selisih dari jadwal n × 500 ms (detik)")
    ax.set_xlim(0, menit)
    simpan(fig, "drift.svg")
    return hasil


def grafik_macet(baris):
    macet = next(int(r["waktu"]) for r in baris if r["jenis"] == "macet")
    lanjut = next(int(r["waktu"]) for r in baris if r["jenis"] == "lanjut")
    fig, ax = plt.subplots(figsize=(8, 2.8))
    ax.axvspan(macet / 1000, lanjut / 1000, color=WARNA["mentah"], alpha=0.18, lw=0)
    ax.text((macet + lanjut) / 2000, 2.55, f"loop() macet {koma((lanjut - macet) / 1000)} detik",
            ha="center", va="center", color="#4b5563")
    for y, jenis, warna in [(1, "tanpadelay", WARNA["utama"]), (0, "millisdelay", WARNA["pembanding"])]:
        t = [int(r["waktu"]) / 1000 for r in baris if r["jenis"] == jenis]
        ax.vlines(t, y - 0.3, y + 0.3, color=warna, lw=2)
    # kejadian antara akhir macet dan jadwal kelipatan 500 ms berikutnya
    batas = (lanjut // 500 + 1) * 500
    beruntun = [int(r["waktu"]) for r in baris if r["jenis"] == "millisdelay" and lanjut <= int(r["waktu"]) < batas]
    td = [int(r["waktu"]) for r in baris if r["jenis"] == "tanpadelay" and lanjut <= int(r["waktu"]) < batas]
    ax.annotate(f"{len(beruntun)}× beruntun\ndalam {beruntun[-1] - beruntun[0]} ms",
                (lanjut / 1000, -0.3), xytext=(-8, -2), textcoords="offset points",
                ha="right", va="top", color=WARNA["pembanding"])
    ax.set_yticks([0, 1], ["millisDelay", "TanpaDelay"])
    ax.set_ylim(-0.9, 2.9)
    ax.grid(axis="y", visible=False)
    ax.set_xlim(0, 4.5)
    ax.set_xlabel("waktu (detik), garis = waktunya() / justFinished() true")
    ax.set_title(f"Setelah loop() macet, millisDelay mengejar {len(beruntun)} kali beruntun;\n"
                 f"TanpaDelay true {len(td)} kali lalu kembali ke kelipatan 500 ms")
    simpan(fig, "loop-macet.svg")
    return len(beruntun), beruntun[-1] - beruntun[0], len(td)


def grafik_led(baris):
    fig, ax = plt.subplots()
    akhir = 6.0
    baris_y = [("tanpadelay", "300", "TanpaDelay\nLED 300 ms", WARNA["utama"]),
               ("tanpadelay", "500", "TanpaDelay\nLED 500 ms", WARNA["utama"]),
               ("delay", "300", "delay()\nLED 300 ms", WARNA["pembanding"]),
               ("delay", "500", "delay()\nLED 500 ms", WARNA["pembanding"])]
    siklus = {}
    for y, (mode, led, _, warna) in enumerate(baris_y):
        ev = [(int(r["waktu"]) / 1000, int(r["nyala"])) for r in baris if r["mode"] == mode and r["led"] == led]
        nyala = [t for t, n in ev if n]
        siklus[(mode, led)] = nyala[1] - nyala[0]
        ev.append((akhir, 0))
        bar = [(t, ev[i + 1][0] - t) for i, (t, n) in enumerate(ev[:-1]) if n]
        ax.broken_barh(bar, (y - 0.3, 0.6), color=warna, lw=0)
    ax.set_yticks(range(4), [b[2] for b in baris_y])
    ax.set_ylim(-0.6, 3.6)
    ax.set_xlim(0, akhir)
    ax.grid(axis="y", visible=False)
    ax.set_xlabel("waktu (detik), kotak = LED menyala")
    ax.set_title(f"Dengan delay(), kedua LED saling menunggu: siklus jadi {koma(siklus[('delay', '500')])} detik;\n"
                 f"TanpaDelay tepat {koma(siklus[('tanpadelay', '500')])} detik dan "
                 f"{koma(siklus[('tanpadelay', '300')])} detik")
    simpan(fig, "dua-led.svg")
    return siklus


if __name__ == "__main__":
    os.makedirs(KELUAR, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        sim = os.path.join(tmp, "sim")
        subprocess.run(["g++", "-std=c++11", "-O2", "-I../test", "-I../../src", "simulasi.cpp",
                        *sorted(glob.glob("../../src/*.cpp")), "-o", sim], check=True)
        print("drift:", grafik_drift(jalankan(sim, "drift")))
        print("macet:", grafik_macet(jalankan(sim, "macet")))
        print("led:", grafik_led(jalankan(sim, "led")))
