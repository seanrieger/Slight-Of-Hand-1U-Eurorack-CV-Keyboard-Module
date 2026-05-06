# Slight Of Hand - Changelog

All notable changes to the Slight Of Hand firmware will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.5.4] - 2025-01-19

### Added

- Two-zone musical potentiometer mapping for intuitive portamento control
  - Fast zone (70% of pot): 30-500ms with exponential curve
  - Slow zone (30% of pot): 550-2000ms with linear curve
  - Dead zone at minimum for instant notes
- Enhanced code documentation and inline comments
- Comprehensive developer documentation

### Changed

- **Major optimization:** Refactored slew/portamento system to use proportional rate approach
  - Reduced from 7 function calls per loop to 1 (85% reduction)
  - Simplified state management from 7 variables to 4 (43% reduction)
  - Changed from step-based linear to proportional rate exponential decay
- Updated voltage calculations from DAC units to voltage-based (0-4.93V)
- Improved code organization and readability

### Fixed

- **Critical:** Fixed calibration mode bug where `octaveShift` was not reset on entry
  - Calibration now always starts at octave 0 as intended
  - Prevents calibration values from being stored in wrong array indices
- **Critical:** Fixed double-increment bug in calibration mode octave buttons
  - Main loop no longer handles octave buttons during calibration
  - `handleCalibrationMode()` now has exclusive control over octave buttons
  - Pressing octave up/down once now correctly increments by 1 instead of 2

### Performance

- 85% reduction in slew-related function calls per loop iteration
- More efficient CPU usage with single `updateSlew()` call
- Smoother voltage transitions with proportional rate algorithm

---

## [1.5.3a] - 2024

### Added

- Modular code structure with shared calibration and EEPROM files
- `CalibrationMode.h` and `CalibrationMode.cpp` for shared calibration logic
- `EEPROMHandling.h` and `EEPROMHandling.cpp` for shared EEPROM operations

### Changed

- Refactored from single-file (547 lines) to modular structure (~260 lines main file)
- Extracted 50 lines of EEPROM code to `handleEEPROM()` function
- Extracted 70 lines of calibration code to `handleCalibrationMode()` function
- Aligned architecture with other Nocturne Alchemy platform modules

### Fixed

- Variable definition conflicts resolved with proper extern declarations
- EEPROM handling now consistent across all Nocturne Alchemy firmwares

---

## [1.5.3] - 2024

### Added

- Complete calibration mode with per-note voltage adjustment
- EEPROM persistence for calibration data (49 notes)
- Integer format for calibration storage (scaled by 1000)
- Calibration reset function (8-second hold)
- Calibration mode toggle (2-second hold of both octave buttons)

### Changed

- Stabilized all core functionality
- Finalized user interface and control scheme
- Optimized EEPROM usage and addressing

### Fixed

- Various stability improvements
- Debouncing refinements
- EEPROM data integrity checks

---

## [1.5.2] - 2024

### Fixed

- Bug fixes and stability improvements
- Calibration mode refinements

---

## [1.5.1] - 2024

### Added

- Initial public release
- Basic CV keyboard functionality
- 4 octaves of range (C0-C4)
- Adjustable portamento/slew
- Gate output
- Octave up/down buttons
- High C button

---

## Version Numbering

This project uses [Semantic Versioning](https://semver.org/):

- **MAJOR** version: Incompatible API/hardware changes
- **MINOR** version: New features, backward compatible
- **PATCH** version: Bug fixes, backward compatible



---

## Known Issues

### Fixed in v1.5.4

- Calibration mode octave shift not resetting (fixed)
- Double-increment of octave in calibration mode (fixed)

### Current Limitations

- Minimum slew time is 30ms (by design - rates >1.0 cause oscillation)
- Maximum slew time is ~2 seconds (hardware DAC limitation)
- Pot effective range is 0-800 (last 22% maintains maximum slew)

---

## Migration Guide

### From Step-Based Slew (v1.5.3a) to Proportional Rate (v1.5.4)

**Code Changes:**

**Old system (v1.5.3a):**

```cpp
// 7 global variables
int lastVoltage;
bool isSlewing;
unsigned long slewStartTime;
int slewStepTime;
unsigned long totalSlewTime;
int targetVoltageRaw;
int stepDirection;

// startSlew() called when note changes
// updateSlew() called 7 times per loop
```

**New system (v1.5.4):**

```cpp
// 4 global variables
float currentVoltage;
float targetVoltage;
float portamentoRate;
unsigned long lastPortamentoUpdate;

// outputNoteVoltage() called when note changes
// updateSlew() called once per loop
```

**Benefits:**

- Simpler state management
- More efficient CPU usage
- Natural exponential decay curve
- Easier to understand and modify

---

## Breaking Changes

### None in v1.5.4

This version maintains full backward compatibility with v1.5.3a. EEPROM format is unchanged, and all features work identically from the user's perspective.

---

## Deprecations

### None

All features from previous versions remain functional.

---

## Security

### EEPROM Data Integrity

All versions maintain protected EEPROM regions:

- Addresses 0-103: Calibration data (protected)
- Addresses 200-201: Reserved (protected)

The firmware includes checks to prevent accidental corruption of calibration data.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:

- Reporting bugs
- Suggesting enhancements
- Submitting pull requests
- Code style guidelines

---

**Last Updated:** 2025-01-19  
**Maintainer:** Sean Rieger / FlatSix Modular
