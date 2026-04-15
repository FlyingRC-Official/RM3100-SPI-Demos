# Arduino Demo

This folder contains an Arduino-native RM3100 library and a simple example sketch.

## Layout

- `src/RM3100.h`
- `src/RM3100.cpp`
- `examples/BasicReadDemo/BasicReadDemo.ino`

## What The Example Does

- Starts SPI in mode `0`
- Reads the RM3100 revision register as a bring-up check
- Triggers single-shot 3-axis measurements
- Prints signed raw counts
- Prints converted field values in `uT`

## What You Need To Adapt

- Select the correct Arduino SPI bus if your board has more than one
- Set the correct chip-select pin in the sketch
- Make sure the RM3100 stays at or below `1 MHz` SPI clock
- Keep the RM3100 wired for SPI mode with `I2CEN` pulled low

## Run Flow

1. Open `examples/BasicReadDemo/BasicReadDemo.ino` in the Arduino IDE.
2. Confirm the `RM3100_CS_PIN` constant matches your board wiring.
3. Upload the sketch.
4. Open the serial monitor at `115200`.

The sketch is intentionally thin. All RM3100 register handling and post-processing lives in the library files under `src/`.
