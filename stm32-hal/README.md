# STM32 HAL Demo

This folder shows a native STM32 HAL organization with a reusable RM3100 driver and a thin application entrypoint.

## Layout

- `Core/Inc/rm3100.h`
- `Core/Src/rm3100.c`
- `Core/Src/main.c`

## What The Driver Handles

- SPI register reads and writes
- RM3100 cycle-count programming
- Revision-register bring-up check
- Single-shot 3-axis measurement
- 24-bit sign extension
- Raw-count to `uT` conversion

## What You Need To Adapt

- Generate or provide your usual CubeMX or CubeIDE project shell
- Bind the demo to the correct `SPI_HandleTypeDef`
- Set the correct chip-select GPIO port and pin
- Confirm SPI mode `0`, MSB-first, and `1 MHz` or slower
- Retarget `printf` or replace the logging calls with your board's UART output function

## Run Flow

1. Bring your STM32 board up with HAL and a working SPI peripheral.
2. Copy or merge the files in `Core/Inc` and `Core/Src` into your STM32 project if needed.
3. Replace the placeholder handle and pin values in `main.c`.
4. Build and flash.

The provided `main.c` is intentionally small so the RM3100 protocol logic remains reusable inside `rm3100.c`.
