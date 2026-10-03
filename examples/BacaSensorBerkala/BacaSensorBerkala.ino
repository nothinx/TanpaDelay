// Baca sensor cahaya setiap 2 detik dan kirim ke Serial Monitor (115200).
// Sambil itu LED berkedip, makin terang makin cepat. Kecepatannya diubah
// dengan ubahInterval() tanpa mengganggu irama kedip.
//
// Sambungan: LDR dari 5V ke A0, resistor 10k dari A0 ke GND.
#include <TanpaDelay.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DevKit
#endif

TanpaDelay bacaSensor(2000);
TanpaDelay kedip(500);
bool nyala = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (bacaSensor.waktunya()) {
    int cahaya = analogRead(A0);
    Serial.print("Cahaya: ");
    Serial.println(cahaya);
    kedip.ubahInterval(cahaya > 500 ? 100 : 500);
  }

  if (kedip.waktunya()) digitalWrite(LED_BUILTIN, nyala = !nyala);
}
