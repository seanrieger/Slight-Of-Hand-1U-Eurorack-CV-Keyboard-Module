# Hardware Notice

## The Nocturne Alchemy Platform Hardware Is Proprietary

The **Nocturne Alchemy Platform** hardware — including schematics, PCB layout, panel design, and all associated hardware design files — is the proprietary property of FlatSix Modular and is **not covered by the CC BY-NC 4.0 license that applies to this firmware**.

This repository contains firmware source code only. You cannot manufacture or replicate the Nocturne Alchemy Platform hardware from the contents of this repository.

---

## What This Means for You

**If you own a Slight of Hand module:**
You are free to flash this firmware (and any modifications you make) to your own hardware under the terms of the CC BY-NC 4.0 license.

**If you want to port this firmware to different hardware:**
The firmware architecture is documented in `docs/DEVELOPER.md`. Note that the shared library files (`CalibrationMode`, `EEPROMHandling`) are tightly coupled to the Nocturne Alchemy Platform's hardware and EEPROM layout and would require significant rework for other platforms.

---

## Contact

For hardware-related enquiries, visit [flatsixmodular.com](https://flatsixmodular.com) or contact FlatSix Modular directly.

---

*FlatSix Modular — All hardware design rights reserved.*
