# MicroPython Demo

This folder keeps the RM3100 protocol logic inside `rm3100.py` and the runnable example in `main.py`.

## Layout

- `rm3100.py`
- `main.py`

## What The Module Handles

- SPI register access
- RM3100 cycle-count programming
- Revision-register bring-up check
- Single-shot 3-axis reads
- 24-bit signed conversion
- Raw-count to `uT` conversion

## What You Need To Adapt

- Replace the example SPI bus and pin assignments in `main.py`
- Make sure your board can run SPI mode `0` at `1 MHz` or slower
- Confirm the chip-select pin and `I2CEN` wiring for SPI mode

## Run Flow

1. Copy `rm3100.py` and `main.py` to your board.
2. Edit the SPI and chip-select setup in `main.py`.
3. Reset the board or run `main.py`.
4. Watch the REPL output for raw and `uT` readings.

The example uses polling on the RM3100 status register so no `DRDY` pin is required.
