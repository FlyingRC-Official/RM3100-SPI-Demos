# RM3100 SPI Demos

This repository shows how to read and post-process data from the PNI RM3100 module over SPI on three platforms:

- Arduino
- STM32 using HAL
- MicroPython

Each platform keeps the RM3100 sensor logic in a separate reusable driver/library file, while the runnable demo entrypoint only performs platform setup, calls the driver, and prints results.

## What You Will Learn

- How to bring up the RM3100 over SPI
- How to trigger a single 3-axis measurement
- How to read the 9 raw measurement bytes
- How to convert RM3100 24-bit 2's-complement data into signed `X`, `Y`, and `Z`
- How to convert raw counts into magnetic field values in `uT`

## Repository Layout

```text
.
|-- PLAN.md
|-- Datasheet/
|-- arduino/
|   |-- README.md
|   |-- library.properties
|   |-- src/
|   `-- examples/
|-- stm32-hal/
|   |-- README.md
|   `-- Core/
|       |-- Inc/
|       `-- Src/
`-- micropython/
    |-- README.md
    |-- rm3100.py
    `-- main.py
```

## Platform Demos

- [Arduino](arduino/README.md): Arduino-style library in `src/` with a runnable example sketch in `examples/`
- [STM32 HAL](stm32-hal/README.md): RM3100 driver split into `rm3100.h` and `rm3100.c`, plus a thin `main.c` demo flow
- [MicroPython](micropython/README.md): Reusable `rm3100.py` module with a simple `main.py` example

## Sensor Notes

- This repo is SPI-only in its current form.
- The examples use single-shot measurements for clarity.
- The default cycle count is `200`, which maps to `75 LSB/uT` from the local datasheet notes.
- Continuous mode, DRDY interrupt handling, and calibration-oriented workflows are intentionally left for future extensions.

## Reference Material

The `Datasheet/` folder contains local RM3100 reference material that the demos are based on. It is not treated as source code.
