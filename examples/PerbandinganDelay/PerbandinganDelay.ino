// Perbandingan delay() dan TanpaDelay. LED berkedip tiap detik, dan
// Serial Monitor (115200) menampilkan berapa kali loop() berputar per detik.
//
// PAKAI_DELAY = true : loop() hanya ±1 kali per detik. Tombol di pin 2
//                      sering tidak terbaca karena program sedang "tidur".
// PAKAI_DELAY = false: loop() ribuan kali per detik. Tombol langsung terbaca.
//
// Sambungan: tombol antara pin 2 dan GND.
#include <TanpaDelay.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DevKit
#endif

const bool PAKAI_DELAY = false;
const uint8_t TOMBOL = 2;

TanpaDelay kedip(500);
bool nyala = false;
TanpaDelay lapor(1000);
uint32_t putaran = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TOMBOL, INPUT_PULLUP);
}

void loop() {
  putaran++;

  if (PAKAI_DELAY) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else if (kedip.waktunya()) {
    digitalWrite(LED_BUILTIN, nyala = !nyala);
  }

  if (digitalRead(TOMBOL) == LOW) Serial.println("Tombol terbaca");

  if (lapor.waktunya()) {
    Serial.print("loop() per detik: ");
    Serial.println(putaran);
    putaran = 0;
  }
}
