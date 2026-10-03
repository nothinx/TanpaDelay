// LED bawaan berkedip setiap 500 ms tanpa delay(), jadi loop() tetap bebas
// mengerjakan hal lain.
#include <TanpaDelay.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DevKit
#endif

TanpaDelay kedip(500);
bool nyala = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (kedip.waktunya()) {
    nyala = !nyala;
    digitalWrite(LED_BUILTIN, nyala);
  }
  // Kode lain di sini tetap jalan terus, tidak menunggu LED.
}
