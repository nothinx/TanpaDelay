// Uji logika TanpaDelay di PC:
//   g++ -std=c++11 -Wall -Wextra -I. -I../../src uji.cpp ../../src/TanpaDelay.cpp -o uji && ./uji
#include <assert.h>
#include <stdio.h>
#include "TanpaDelay.h"

uint32_t waktuPalsu = 0;

// Panggil waktunya() setiap langkah ms selama ms, kembalikan jumlah true.
static int jalan(TanpaDelay &t, uint32_t ms, uint32_t langkah = 1) {
  int n = 0;
  for (uint32_t i = 0; i < ms; i += langkah) {
    n += t.waktunya();
    waktuPalsu += langkah;
  }
  return n;
}

int main() {
  assert(sizeof(TanpaDelay) <= 12);

  { // berkala sejak board menyala: true di 500, 1000, ...
    waktuPalsu = 0;
    TanpaDelay t(500);
    assert(jalan(t, 500) == 0);       // 0..499
    assert(t.waktunya());             // tepat 500
    assert(!t.waktunya());            // hanya sekali
    assert(t.sisaWaktu() == 500);
  }
  { // tanpa drift: loop lambat 7 ms tetap 100 kali dalam 50 detik
    waktuPalsu = 0;
    TanpaDelay t(500);
    assert(jalan(t, 50003, 7) == 100);
  }
  { // loop macet 2600 ms: hanya sekali true, irama kembali ke kelipatan 500
    waktuPalsu = 0;
    TanpaDelay t(500);
    jalan(t, 600);                    // true di 500
    waktuPalsu = 3100;
    assert(t.waktunya());
    assert(!t.waktunya());            // tidak mengejar 1500, 2000, ...
    assert(t.sisaWaktu() == 400);     // berikutnya di 3500
    waktuPalsu = 3499;
    assert(!t.waktunya());
    waktuPalsu = 3500;
    assert(t.waktunya());
  }
  { // tanpa interval: belum jalan sampai setelah()/setiap()
    waktuPalsu = 0;
    TanpaDelay t;
    assert(!t.sedangJalan());
    assert(jalan(t, 5000) == 0);
  }
  { // mulai() menghitung dari sekarang
    waktuPalsu = 1234;
    TanpaDelay t(100);
    t.mulai();
    assert(jalan(t, 100) == 0);
    assert(t.waktunya());
  }
  { // sekali jalan: true sekali lalu berhenti
    waktuPalsu = 5000;
    TanpaDelay t;
    t.setelah(10000);
    assert(t.sedangJalan());
    assert(jalan(t, 10000, 10) == 0);
    assert(t.selesai());
    assert(!t.sedangJalan());
    assert(jalan(t, 30000, 10) == 0);
    assert(t.sisaWaktu() == 0);
    t.mulai();                        // ulang, tetap sekali jalan
    assert(jalan(t, 20000, 10) == 1);
    t.setelah(100);                   // diperpanjang sebelum habis
    waktuPalsu += 50;
    t.setelah(100);
    assert(jalan(t, 100) == 0);
    assert(t.selesai());
  }
  { // berhenti() dan setiap()
    waktuPalsu = 0;
    TanpaDelay t(100);
    t.berhenti();
    assert(!t.sedangJalan() && t.sisaWaktu() == 0);
    assert(jalan(t, 1000) == 0);
    t.setiap(250);
    assert(jalan(t, 1001) == 4);     // 250, 500, 750, 1000 ms setelah setiap()
  }
  { // ubahInterval() tidak mengulang hitungan
    waktuPalsu = 0;
    TanpaDelay t(1000);
    waktuPalsu = 300;
    t.ubahInterval(200);
    assert(t.waktunya());             // sudah lewat 200
    assert(t.sisaWaktu() == 100);     // berikutnya di 400
  }
  { // interval 0: true di setiap panggilan
    waktuPalsu = 0;
    TanpaDelay t(0);
    assert(jalan(t, 10) == 10);
    assert(t.waktunya() && t.waktunya());
  }
  { // millis() meluap: tetap tepat 10 kali per detik
    waktuPalsu = 0xFFFFFF00u;
    TanpaDelay t(100);
    t.mulai();
    assert(jalan(t, 1001) == 10);
    assert(waktuPalsu < 1000);        // memang sudah meluap
    t.setelah(500);
    assert(jalan(t, 500) == 0 && t.selesai());
  }
  { // interval sangat panjang (1 hari)
    waktuPalsu = 0xF0000000u;
    TanpaDelay t;
    t.setelah(86400000UL);
    waktuPalsu += 86399999UL;
    assert(!t.selesai() && t.sisaWaktu() == 1);
    waktuPalsu++;
    assert(t.selesai());
  }
  printf("Semua uji lolos (sizeof = %u byte)\n", (unsigned)sizeof(TanpaDelay));
  return 0;
}
