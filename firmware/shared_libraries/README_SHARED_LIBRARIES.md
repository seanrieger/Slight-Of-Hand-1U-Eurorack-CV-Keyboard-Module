# Shared Platform Libraries — DO NOT MODIFY

This folder contains two library files that are **shared across all Nocturne Alchemy Platform firmwares**:

```
CalibrationMode.h / CalibrationMode.cpp
EEPROMHandling.h  / EEPROMHandling.cpp
```

They are included here solely so the Slight of Hand firmware compiles correctly. **They must be treated as read-only.**

---

## Why You Must Not Modify These Files

Every Nocturne Alchemy Platform module is individually hand-calibrated. That calibration data — unique to your specific hardware — is stored in EEPROM addresses **0–103**:

```
EEPROM Address Range   Contents
────────────────────   ────────────────────────────────────────
0–3                    Calibration signature (validates data)
4–101                  Calibration values for 49 notes (floats)
102–103                Integer format flag
```

**If you modify these files and upload broken code, you risk overwriting or corrupting your calibration data.**

---

## EEPROM Map for Slight of Hand

```
EEPROM Address Range   Contents                          Status
────────────────────   ───────────────────────────────   ──────────────
0–103                  Calibration data                  🔒 PROTECTED
200–201                Platform flag (reserved)          🔒 PROTECTED
104–199, 202–1023      Available for future use          Safe to use
```

---

## Summary

| File                     | Modify? | Why                              |
| ------------------------ | ------- | -------------------------------- |
| `CalibrationMode.h/.cpp` | Never   | Manages factory calibration data |
| `EEPROMHandling.h/.cpp`  | Never   | Reads/writes calibration EEPROM  |

---

*Slight of Hand v1.5.4 — FlatSix Modular*
