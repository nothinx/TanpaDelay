// Dua LED berkedip dengan kecepatan berbeda. Dengan delay() ini sulit,
// karena delay() milik LED pertama ikut menahan LED kedua.
//
// Sambungan: LED + resistor 220 ohm di pin 8 dan pin 9, ke GND.
#include <TanpaDelay.h>

const uint8_t LED_CEPAT = 8;
const uint8_t LED_LAMBAT = 9;

TanpaDelay cepat(150);
TanpaDelay lambat(1000);
bool nyalaCepat = false, nyalaLambat = false;

void setup() {
  pinMode(LED_CEPAT, OUTPUT);
  pinMode(LED_LAMBAT, OUTPUT);
}

void loop() {
  if (cepat.waktunya()) digitalWrite(LED_CEPAT, nyalaCepat = !nyalaCepat);
  if (lambat.waktunya()) digitalWrite(LED_LAMBAT, nyalaLambat = !nyalaLambat);
}
