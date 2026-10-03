// TanpaDelay - pengganti delay() yang tidak menghentikan program.
// Menjalankan beberapa hal "bersamaan" dengan pola if (x.waktunya()).
// 9 byte RAM per objek di AVR. Copyright (c) 2026 Amadeo Wisesa. Lisensi MIT.
//
// - Berkala tanpa drift: jadwal maju tepat kelipatan interval, tidak ikut
//   bergeser walau loop() lambat.
// - Jika loop() macet lebih dari satu interval, waktunya() hanya true sekali
//   lalu jadwal melompat ke kelipatan berikutnya (tidak "mengejar" berondongan).
// - Aman saat millis() meluap (setelah ±49 hari menyala).
#pragma once
#include <Arduino.h>

class TanpaDelay {
public:
  // Berkala setiap ms. Hitungan dimulai sejak board menyala;
  // panggil mulai() untuk menghitung dari sekarang.
  explicit TanpaDelay(uint32_t ms) : _interval(ms), _status(JALAN) {}
  // Belum jalan, untuk dimulai nanti dengan setelah() atau setiap().
  TanpaDelay() {}

  // true sekali setiap interval (berkala), atau sekali saat waktu habis
  // (sekali jalan, lalu berhenti). Panggil di setiap loop().
  bool waktunya();
  // Sama dengan waktunya(), lebih enak dibaca untuk sekali jalan.
  bool selesai() { return waktunya(); }

  void setiap(uint32_t ms) { _interval = ms; _status = JALAN; _mulai = millis(); }          // berkala, mulai sekarang
  void setelah(uint32_t ms) { _interval = ms; _status = JALAN | SEKALI; _mulai = millis(); } // sekali jalan, mulai sekarang
  void mulai() { _status |= JALAN; _mulai = millis(); } // ulang hitungan dari sekarang, mode tetap
  void berhenti() { _status &= ~JALAN; }                // waktunya() selalu false sampai dimulai lagi

  // Ganti interval tanpa mengulang hitungan, aman dipanggil di setiap loop().
  void ubahInterval(uint32_t ms) { _interval = ms; }

  bool sedangJalan() const { return _status & JALAN; }
  uint32_t sisaWaktu() const; // ms sampai waktunya() true, 0 jika sudah waktunya atau berhenti

private:
  enum : uint8_t { JALAN = 0x01, SEKALI = 0x02 };

  uint32_t _mulai = 0;    // awal hitungan saat ini
  uint32_t _interval = 0;
  uint8_t _status = 0;
};
