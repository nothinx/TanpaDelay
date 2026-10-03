# TanpaDelay (English)

[Bahasa Indonesia](README.md)

An Arduino **non-blocking `delay()` replacement**. Run several things at once with a single `if`, no callbacks and no hand-written `millis()` math. **9 bytes of RAM per object.** The API and examples are in Indonesian ("tanpa delay" = "without delay"). This page maps every function to English.

```cpp
#include <TanpaDelay.h>

TanpaDelay blink(500);    // every 500 ms
TanpaDelay lightOff;      // one-shot, started later
bool on = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

void loop() {
  if (blink.waktunya()) {               // isTime()
    on = !on;
    digitalWrite(LED_BUILTIN, on);
  }
  if (digitalRead(2) == LOW) {
    digitalWrite(13, HIGH);
    lightOff.setelah(10000);            // after(10 s), restarts on every press
  }
  if (lightOff.selesai()) digitalWrite(13, LOW);  // done()
}
```

## Why

- Reads like a sentence and keeps the code inside `loop()`: no callbacks.
- **No drift**: the schedule advances by exact multiples of the interval, not by `millis()` at the time of the check.
- **No catch-up bursts**: if `loop()` stalls for several intervals, `waktunya()` fires once and the original rhythm resumes.
- One-shot mode, remaining time, and a stop state in the same 9-byte object. No dynamic allocation.
- Safe across the `millis()` overflow.

From the source of popular English libraries: arduino-timer, Ticker (sstaub), TimerEvent, and TaskScheduler are callback-based. arduino-timer, Ticker, TimerEvent, NoDelay, and Neotimer reschedule from the current `millis()`, so the period drifts by the loop latency. millisDelay has no drift with a manual `repeat()` call, but fires in a burst after a stall. kiryanenko's SimpleTimer compares `_start + _interval <= millis()`, which stops working after `millis()` wraps. Measured on an Uno: NoDelay 16 B and Neotimer 13 B per object, the default arduino-timer 256 B.

## Simulation results

![Event time minus the ideal n × 500 ms schedule over 10 minutes](extras/gambar/drift.svg)

Simulated `loop()` taking 5–9 ms (7 ms average) with a 500 ms interval. The `last = millis()` pattern falls 3.8 s behind after 10 minutes; TanpaDelay stays within 8 ms of the schedule.

![Events before and after a 1.7 s loop() stall](extras/gambar/loop-macet.svg)

After a single 1.7 s stall, millisDelay (SafeString, with `repeat()`) catches up with 3 back-to-back events within 16 ms. TanpaDelay fires once and returns to multiples of 500 ms.

![Two LEDs, 500 ms and 300 ms, with delay() vs TanpaDelay](extras/gambar/dua-led.svg)

With `delay()` the two LEDs wait for each other and both cycle every 1.6 s. With TanpaDelay they cycle at exactly 1.0 s and 0.6 s.

The plots come from a PC simulation that runs this library's code (`extras/simulasi`): `cd extras/simulasi && python gambar.py` (needs g++ and matplotlib).

## Speed & memory

Measured with simavr (cycle-accurate ATmega328P simulator), Arduino Uno 16 MHz, 10 ms interval, including the `millis()` call.

| | TanpaDelay 1.0.1 | 1.0.0 | NoDelay 2.2.0 | Neotimer 1.1.6 | arduino-timer 3.0.1 |
|---|---|---|---|---|---|
| Not yet due | 98 cycles (6 µs) | 98 | 108 | 84 | 246 |
| Due | 149 (9 µs) | 725 | 139 | 130 | 329 |
| RAM per object | 9 B | 9 B | 16 B | 13 B | 16 B |
| Extra flash | 302 B | 254 B | 180 B | 254 B | 430 B |

`waktunya()` is O(1). Since 1.0.1 the due path avoids a 32-bit modulo unless `loop()` fell more than one interval behind (5× faster). NoDelay and Neotimer are 10–20 cycles faster because they simply set `start = millis()`, which is exactly what makes them drift. Benchmark sketch: `extras/benchmark/TanpaDelayBenchmark`.

## Function reference

| Indonesian | English | Notes |
|---|---|---|
| `TanpaDelay(ms)` | constructor | periodic, counted from power-on |
| `TanpaDelay()` | constructor | idle until `setelah()` or `setiap()` |
| `waktunya()` | is it time | `true` once per interval, or once when a one-shot expires |
| `selesai()` | done | same as `waktunya()`, reads better for one-shots |
| `setiap(ms)` | every | periodic, counted from now |
| `setelah(ms)` | after | one-shot, counted from now; calling again restarts it |
| `mulai()` | start / restart | restart counting from now, keep the mode |
| `berhenti()` | stop | |
| `ubahInterval(ms)` | change interval | does not restart counting; safe to call every loop |
| `sedangJalan()` | is running | |
| `sisaWaktu()` | remaining time | ms; 0 if due or stopped |

An interval of `0` makes `waktunya()` return `true` on every call.

## Examples

`KedipTanpaDelay` (blink), `DuaLEDBedaKecepatan` (two LEDs, two speeds), `BacaSensorBerkala` (periodic sensor read), `SekaliJalan` (staircase light, one-shot), `PerbandinganDelay` (loop() rate with `delay()` vs TanpaDelay).

## Status

Version 1.0.1 passes automated logic tests and compiles on Uno, Mega, ESP32, ESP32-C3, ESP32-S3, STM32 Blackpill F411, and Bluepill F103. It is pure software and only uses `millis()`.

## License

MIT © 2026 Amadeo Wisesa.
