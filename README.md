# Slight Of Hand v1.5.4 - 1U Eurorack CV Keyboard

![License](https://img.shields.io/badge/license-CC%20BY--NC%204.0-blue)
![Version](https://img.shields.io/badge/version-1.5.4-green)
![Platform](https://img.shields.io/badge/platform-Arduino%20Nano-red)

## Overview

Slight Of Hand is a compact 1U (Intellijel format) Eurorack CV keyboard module that provides monophonic CV control with adjustable portamento/slew. Designed for the Nocturne Alchemy Platform by FlatSix Modular, it features responsive light-touch switches, four octaves of range, and per-note voltage calibration.

The hardware is designed to be extremely playable with responsive, light touch, tactile switches for the keys, a wide portamento/slew range which is adjustable in real-time, as well as octave up and down buttons.

This firmware is optimized for performance and musical expression, with a carefully tuned two-zone potentiometer mapping for intuitive portamento control.

## Features

- **12-note keyboard** (C to C) with responsive tactile switches
- **4 octaves** of range (C0 to C4, 49 notes total)
- **Musical portamento/slew** with two-zone mapping:
  - 70% of pot: Fast range (30-500ms) for everyday playing
  - 30% of pot: Slow range (550-2000ms) for dramatic effects
- **1V/octave CV output** (0-4V range)
- **Gate output** (5V trigger)
- **Per-note calibration** system (49 notes individually tunable)
- **EEPROM storage** for persistent calibration data
- **Optimized performance** (85% reduction in function calls vs. previous versions)

## ![](https://private-user-images.githubusercontent.com/8561196/299335099-5dabd7d8-bc2f-4699-bec2-8ccc39dbeeba.jpg?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NjkyNjg2MTMsIm5iZiI6MTc2OTI2ODMxMywicGF0aCI6Ii84NTYxMTk2LzI5OTMzNTA5OS01ZGFiZDdkOC1iYzJmLTQ2OTktYmVjMi04Y2NjMzlkYmVlYmEuanBnP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI2MDEyNCUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNjAxMjRUMTUyNTEzWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9YjNkMGEzNTRmYzc2NzBhOTVjMzk1ZTExNTAwYTEyM2QxNjI0MzZkYTkxNTQ0MGE0YjJmYmRlOGI1ZjNlZDJlOSZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.LC-WCQyFnTcG10hrV_KGjXIMlAiS1lD9KE_l_L_Utno)

## Hardware Specifications

| Specification       | Value                        |
| ------------------- | ---------------------------- |
| **Format**          | 1U Intellijel                |
| **Width**           | 26HP                         |
| **Depth**           | 40mm (skiff-friendly)        |
| **Power**           | 40mA +12V, 0mA -12V, 0mA +5V |
| **Microcontroller** | Arduino Nano (ATmega328P)    |
| **DAC**             | MCP4725 (12-bit, I2C)        |
| **CV Range**        | 0-4V (1V/octave standard)    |
| **Gate**            | 5V digital output            |

For complete documentation, see the other files in this directory:

- **QUICK_REFERENCE.md** - One-page user guide
- **DEVELOPER.md** - Technical deep dive
- **CONTRIBUTING.md** - How to contribute
- **CHANGELOG.md** - Version history

## Quick Start

### Installation

1. Download the latest firmware from the releases page or use the online installer to avoid having to use Arduino IDE
2. Open the `.ino` file in Arduino IDE
3. Install required libraries:
   - Adafruit_MCP4725
   - Wire (built-in)
   - EEPROM (built-in)
4. Select **Arduino Nano** as board
5. Select correct processor (ATmega328P - check bootloader type)
6. Upload to your module

### Basic Operation

**Playing Notes:**

- Press any of the 12 keyboard buttons to play notes
- Use **Octave Up/Down** buttons to shift octave range (0-3)
- Press **High C** button to play the C in the next octave

**Adjusting Portamento:**

- Turn the potentiometer to adjust slew/glide time
- Fully CCW (7 o'clock): Instant notes
- 8-1 o'clock: Fast musical range (30-500ms)
- 1-5 o'clock: Slow dramatic range (550-2000ms)

### Calibration Mode

Note: Each module comes fully calibrated, so you should not have to calibrate unless that calibration was reset, or you magically cleared it using your mind. There's a full [walkthrough video for calibration here](https://youtu.be/dOwi5h5uOKQ), should you need it. 

**Entering Calibration:**

1. Power off the module
2. Hold **Octave Up + Octave Down** simultaneously
3. Power on while holding both buttons and continue holding until the gate out goes high
4. Gate output goes HIGH to confirm

**Calibrating Notes:**

1. Press a keyboard button to select a note
2. Hold the **High C** button (shift key)
3. Turn the potentiometer to adjust voltage
4. Release **High C** to save to EEPROM
5. Repeat for all 49 notes (recommended)

**Checking Calibration:**

- Without holding High C, press keyboard buttons to see current voltage
- Use octave buttons to navigate through octaves

**Exiting Calibration:**

- Hold **Octave Up + Octave Down** for 8 seconds (Gate will go low)

**Reset to Defaults:**

- In calibration mode, hold **Octave Down** for 8 seconds
- Gate output blinks 6 times to confirm

## License

I am still working on new firmwares and modules for the Nocture Alchemy Platform, so while I am releasing this as a starter template for those who would like to develop their own firmware, I am unable to provide support for those individual projects.

This firmware is licensed under **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)**.

**You are free to:**

- Share — copy and redistribute the material
- Adapt — remix, transform, and build upon the material

**Under these terms:**

- **Attribution** — Give appropriate credit to FlatSix Modular
- **NonCommercial** — Not for commercial use without permission

Full license: http://creativecommons.org/licenses/by-nc/4.0/

## Credits

**Hardware Design:** FlatSix Modular  
**Firmware Development:** Sean Rieger & Claude AI (Anthropic)  
**Platform:** Nocturne Alchemy  
**Inspiration:** Seventh Summoner proportional rate system

## Support & Community

- **Website:** https://www.flatsixmodular.com
- **Firmware Updates:** https://www.flatsixmodular.com/firmware
- **Instagram:** @flatsix.modular

---

**🎹 Enjoy your Slight Of Hand! 🎹**

*Made with ❤️ for the modular synthesis community*




Check the Wiki for More Info: https://github.com/seanrieger/Slight-Of-Hand-1U-Eurorack-CV-Keyboard-Module/wiki
