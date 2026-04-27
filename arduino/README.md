# Arduino / ESP32 Demo

This folder contains an Arduino-native RM3100 library and a simple ESP32-oriented example sketch.

## Layout

- `src/RM3100.h`
- `src/RM3100.cpp`
- `examples/BasicReadDemo/BasicReadDemo.ino`

## Why The Example Defaults To ESP32

The RM3100 module uses `3.3 V` power and `3.3 V` IO. A classic Arduino UNO/Nano uses `5 V` IO, so directly wiring it to the RM3100 SPI pins can cause invalid readings and may damage the module. Use an ESP32 or another 3.3 V host by default. If you must use a 5 V Arduino board, add proper level shifting on `SCK`, `MOSI`, and `CS`, and make sure `MISO` is safe for the host.

## Default ESP32 Wiring

The example uses common ESP32 VSPI-style pins:

| RM3100 signal | ESP32 GPIO |
| --- | --- |
| `SCK` | `GPIO18` |
| `MISO` | `GPIO19` |
| `MOSI` | `GPIO23` |
| `CS` | `GPIO5` |
| `VCC` | `3V3` |
| `GND` | `GND` |
| `I2CEN` | `GND` / LOW |

Change the pin constants at the top of `BasicReadDemo.ino` if your ESP32 board uses different GPIOs.

## What The Example Does

- Starts SPI in mode `0`
- Uses explicit ESP32 SPI pins instead of relying on board defaults
- Reads the RM3100 revision register as a bring-up check
- Warns if `REVID` looks invalid, such as `0x00` or `0xFF`
- Triggers single-shot 3-axis measurements
- Prints signed raw counts
- Prints converted field values in `uT`

## What You Need To Adapt

- Set the correct `RM3100_SCK_PIN`, `RM3100_MISO_PIN`, `RM3100_MOSI_PIN`, and `RM3100_CS_PIN` constants in the sketch
- Make sure the RM3100 stays at or below `1 MHz` SPI clock
- Keep the RM3100 wired for SPI mode with `I2CEN` pulled low
- Keep the whole SPI interface at `3.3 V` logic level

## Run Flow

1. Open `examples/BasicReadDemo/BasicReadDemo.ino` in the Arduino IDE.
2. Select your ESP32 board.
3. Confirm the four SPI pin constants match your board wiring.
4. Upload the sketch.
5. Open the serial monitor at `115200`.
6. Confirm `REVID` is not `0x00` or `0xFF`; values around `0x21`, `0x22`, or newer revisions are expected.

The sketch is intentionally thin. All RM3100 register handling and post-processing lives in the library files under `src/`.
