#include "TanpaDelay.h"

bool TanpaDelay::waktunya() {
  if (!(_status & JALAN)) return false;
  uint32_t lewat = millis() - _mulai;
  if (lewat < _interval) return false;
  if (_status & SEKALI) {
    _status &= ~JALAN;
  } else {
    // Maju kelipatan interval, bukan _mulai = millis(): irama tidak bergeser,
    // dan interval yang terlewat saat loop() macet dilompati, bukan dikejar.
    _mulai += _interval ? lewat - lewat % _interval : lewat;
  }
  return true;
}

uint32_t TanpaDelay::sisaWaktu() const {
  if (!(_status & JALAN)) return 0;
  uint32_t lewat = millis() - _mulai;
  return lewat >= _interval ? 0 : _interval - lewat;
}
