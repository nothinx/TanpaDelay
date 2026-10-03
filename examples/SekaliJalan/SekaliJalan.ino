// Lampu tangga: tekan tombol, lampu menyala, lalu mati otomatis
// 10 detik setelah tekanan terakhir.
//
// Sambungan: tombol antara pin 2 dan GND, LED (atau modul relay) di pin 13.
#include <TanpaDelay.h>

const uint8_t TOMBOL = 2;
const uint8_t LAMPU = 13;

TanpaDelay matikan;

void setup() {
  pinMode(TOMBOL, INPUT_PULLUP);
  pinMode(LAMPU, OUTPUT);
}

void loop() {
  if (digitalRead(TOMBOL) == LOW) {
    digitalWrite(LAMPU, HIGH);
    matikan.setelah(10000); // ditekan lagi = hitungan diulang dari awal
  }

  if (matikan.selesai()) digitalWrite(LAMPU, LOW);
}
