// Simulasi TanpaDelay di PC memakai kode library asli (../../src).
// Dijalankan oleh gambar.py; mencetak CSV ke stdout.
//   ./sim drift  : loop() lambat 7 ms ± 2 ms, interval 500 ms, 10 menit
//   ./sim macet  : loop() macet 1,7 detik sekali, TanpaDelay vs millisDelay
//   ./sim led    : dua LED (500 ms & 300 ms) dengan delay() vs TanpaDelay
#include <stdio.h>
#include <string.h>
#include "TanpaDelay.h"

uint32_t waktuPalsu = 0;

// Acak deterministik (LCG Numerical Recipes), seed tetap.
static uint32_t acak = 12345;
static uint32_t lamaLoop() { // 5..9 ms, rata-rata 7 ms
  acak = acak * 1664525u + 1013904223u;
  return 5 + (acak >> 16) % 5;
}

// Reproduksi setia millisDelay dari library SafeString (Forward Computing and
// Control), file millisDelay.cpp revisi "V1.1.0 fixed repeat if stopped /
// finished early" (c)2018-2025. Hanya fungsi yang dipakai di sini.
class millisDelay {
public:
  void start(unsigned long delay) { ms_delay = delay; startTime = millis(); running = true; finishNow = false; }
  void stop() { running = false; finishNow = false; }
  void repeat() {
    unsigned long ms = millis();
    if ((ms - startTime) < ms_delay) { start(ms_delay); return; }
    startTime = startTime + ms_delay;
    running = true;
    finishNow = false;
  }
  bool justFinished() {
    if (running && (finishNow || ((millis() - startTime) >= ms_delay))) { stop(); return true; }
    return false;
  }
private:
  bool running = false, finishNow = false;
  unsigned long startTime = 0, ms_delay = 0;
};

// Pola yang biasa ditulis pemula: if (millis() - terakhir >= 500) { terakhir = millis(); ... }
struct PolaLama {
  uint32_t terakhir = 0, interval;
  explicit PolaLama(uint32_t ms) : interval(ms) {}
  bool waktunya() {
    if (millis() - terakhir < interval) return false;
    terakhir = millis();
    return true;
  }
};

static void drift() {
  TanpaDelay td(500);
  PolaLama lama(500);
  int nTd = 0, nLama = 0;
  printf("jenis,n,waktu\n");
  while (waktuPalsu <= 600000UL) {
    if (td.waktunya()) printf("tanpadelay,%d,%u\n", ++nTd, (unsigned)waktuPalsu);
    if (lama.waktunya()) printf("lama,%d,%u\n", ++nLama, (unsigned)waktuPalsu);
    waktuPalsu += lamaLoop();
  }
}

static void macet() {
  TanpaDelay td(500);
  millisDelay md;
  md.start(500); // di setup()
  bool sudahMacet = false;
  printf("jenis,waktu\n");
  while (waktuPalsu <= 4500) {
    if (td.waktunya()) printf("tanpadelay,%u\n", (unsigned)waktuPalsu);
    if (md.justFinished()) { md.repeat(); printf("millisdelay,%u\n", (unsigned)waktuPalsu); }
    if (!sudahMacet && waktuPalsu >= 1200) { // loop() tertahan 1,7 detik
      printf("macet,%u\n", (unsigned)waktuPalsu);
      waktuPalsu += 1700;
      printf("lanjut,%u\n", (unsigned)waktuPalsu);
      sudahMacet = true;
    } else {
      waktuPalsu += lamaLoop();
    }
  }
}

static void catat(const char *mode, const char *led, int nyala) {
  printf("%s,%s,%u,%d\n", mode, led, (unsigned)waktuPalsu, nyala);
}

static void led() {
  const uint32_t AKHIR = 6000;
  printf("mode,led,waktu,nyala\n");
  // delay(): cara pemula, satu LED menunggu yang lain.
  waktuPalsu = 0;
  while (waktuPalsu < AKHIR) {
    catat("delay", "500", 1); waktuPalsu += 500; // digitalWrite(A, HIGH); delay(500);
    catat("delay", "500", 0); waktuPalsu += 500; // digitalWrite(A, LOW);  delay(500);
    catat("delay", "300", 1); waktuPalsu += 300; // digitalWrite(B, HIGH); delay(300);
    catat("delay", "300", 0); waktuPalsu += 300; // digitalWrite(B, LOW);  delay(300);
  }
  // TanpaDelay: contoh DuaLEDBedaKecepatan dengan interval 500 & 300 ms.
  waktuPalsu = 0;
  TanpaDelay a(500), b(300);
  bool nyalaA = false, nyalaB = false;
  catat("tanpadelay", "500", 0);
  catat("tanpadelay", "300", 0);
  for (; waktuPalsu < AKHIR; waktuPalsu++) {
    if (a.waktunya()) catat("tanpadelay", "500", nyalaA = !nyalaA);
    if (b.waktunya()) catat("tanpadelay", "300", nyalaB = !nyalaB);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) return 1;
  if (!strcmp(argv[1], "drift")) drift();
  else if (!strcmp(argv[1], "macet")) macet();
  else if (!strcmp(argv[1], "led")) led();
  else return 1;
  return 0;
}
