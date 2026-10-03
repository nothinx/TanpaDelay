// Benchmark TanpaDelay di ATmega328P 16 MHz (simavr). Cara menjalankan dan
// angka hasilnya: README bagian "Kecepatan & memori".
// Siklus.h: Timer1 tanpa prescaler, UKUR(nama, ulang, kode) mencetak
// "BENCH nama siklus_per_panggilan". millis() berhenti selama UKUR, jadi
// jalur "kejadian" memajukan millis() 10 ms sendiri di setiap panggilan.
#include <TanpaDelay.h>
#include "Siklus.h"

extern volatile unsigned long timer0_millis; // penghitung millis() di core AVR
TanpaDelay tugas(10);
volatile bool hasil;

void setup() {
  Serial.begin(115200);
  tugas.mulai();
  Serial.print(F("BENCH sizeof "));
  Serial.println(sizeof(TanpaDelay));
  UKUR("waktunya_belum", 1000, hasil = tugas.waktunya());
  UKUR("millis_maju", 1000, timer0_millis += 10);
  UKUR("waktunya_kejadian", 1000, { timer0_millis += 10; hasil = tugas.waktunya(); });
  UKUR("waktunya_macet", 1000, { timer0_millis += 35; hasil = tugas.waktunya(); });
  selesai();
}

void loop() {}
