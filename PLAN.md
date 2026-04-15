# RM3100 SPI Demos Implementation Plan

## Goal

Build this repository as an educational, SPI-focused RM3100 reference for programmers using:

1. Arduino
2. STM32 with HAL
3. MicroPython

The implementation must separate reusable RM3100 sensor logic from the runnable demo application on every platform.

## Deliverables

- A top-level repository README that explains the purpose of the repo and links to all demos
- One Arduino demo with Arduino-native library structure
- One STM32 HAL demo with a separate RM3100 driver and application entrypoint
- One MicroPython demo with a reusable module and a thin `main.py`
- Platform READMEs that explain setup, SPI adaptation points, and how to run each demo

## Required Structure

```text
arduino/
  src/                  # reusable RM3100 Arduino library code
  examples/             # runnable Arduino sketches

stm32-hal/
  Core/Inc/             # reusable RM3100 HAL driver headers
  Core/Src/             # reusable RM3100 HAL driver source and demo main

micropython/
  rm3100.py             # reusable RM3100 module
  main.py               # runnable MicroPython demo entrypoint
```

## Functional Scope

Each platform implementation must include:

- SPI register read and write helpers
- An RM3100 bring-up step such as reading `REVID`
- A single-shot 3-axis measurement flow using `POLL = 0x70`
- A burst read of 9 bytes starting at `0x24`
- 24-bit signed conversion for `X`, `Y`, and `Z`
- Raw-count to `uT` conversion
- A simple platform-native return type for raw and scaled readings

## Public API Expectations

Each platform should expose equivalent concepts even if naming follows the native ecosystem:

- `begin`, `init`, or constructor-based setup
- `read_raw_xyz`
- `read_ut_xyz`
- Small internal helpers for sign extension and gain/scaling

## Demo Expectations

Each demo entrypoint should:

- Show where to configure SPI and chip-select wiring
- Perform one communication or revision check
- Read the sensor periodically
- Print both signed raw counts and scaled `uT` values
- Keep RM3100 protocol details inside the reusable driver/library layer

## Post-Processing Policy

- Required post-processing stops at signed conversion plus `uT` output
- Heading computation is out of scope for v1
- Continuous mode, DRDY interrupt demos, and calibration routines are future extensions

## Acceptance Criteria

- Library code is separate from the runnable demo entrypoint on all three platforms
- The repo layout is native to each ecosystem rather than artificially identical
- The raw `X/Y/Z` conversion is correct for RM3100 24-bit 2's-complement data
- The demo output includes scaled `uT` values
- The root README and platform READMEs are consistent in terminology and setup guidance
- SPI adaptation points are clearly documented for users moving the code to their own hardware

## Defaults And Assumptions

- SPI only
- Generic compatible targets instead of one fixed board per platform
- STM32 examples use HAL
- Default cycle count is `200`
- Default gain for the provided demos is `75 LSB/uT`
- Readability and reuse take priority over feature completeness
