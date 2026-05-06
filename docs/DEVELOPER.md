# Slight Of Hand v1.5.4 - Developer Documentation

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Code Organization](#code-organization)
3. [Key Algorithms](#key-algorithms)
4. [Hardware Interface](#hardware-interface)
5. [Development Guidelines](#development-guidelines)
6. [Testing Procedures](#testing-procedures)
7. [Common Pitfalls](#common-pitfalls)

---

## Architecture Overview

### Platform: Nocturne Alchemy

Slight Of Hand is part of the Nocturne Alchemy Platform, which means:

- **Shared hardware** across multiple firmware variants
- **Shared calibration system** (CalibrationMode.h/.cpp)
- **Shared EEPROM handling** (EEPROMHandling.h/.cpp)
- **Swappable firmware** via web uploader or Arduino IDE

### Design Philosophy

**Separation of Concerns:**

- Main .ino file: Hardware-specific and feature-specific logic
- Shared files: Platform-wide calibration and EEPROM operations
- Modular functions: Single responsibility, easy to test

**Performance First:**

- Minimize function calls in loop
- Use efficient algorithms (proportional rate vs. step-based)
- Avoid blocking operations (except in calibration mode)

---

## Code Organization

### File Structure

```
SlightOfHand_v1-5-4-Stable/
├── SlightOfHand_v1-5-4-Stable.ino    # Main firmware (260 lines)
│   ├── Hardware definitions
│   ├── Global variables (firmware-specific)
│   ├── Helper functions (slew, calibration, blinking)
│   ├── setup() - Initialization
│   └── loop() - Main execution
│
├── CalibrationMode.h                  # Shared calibration declarations
│   ├── extern variable declarations
│   ├── Function prototypes
│   └── Constants (CALIBRATION_VALUES_SIZE)
│
├── CalibrationMode.cpp                # Shared calibration implementation
│   ├── Variable definitions
│   ├── handleCalibrationMode() - Main calibration logic
│   └── Calibration UI handling
│
├── EEPROMHandling.h                   # Shared EEPROM constants
│   ├── EEPROM addresses
│   ├── Signature values
│   └── Function prototypes
│
└── EEPROMHandling.cpp                 # Shared EEPROM functions
    ├── handleEEPROM() - Initialize and load
    ├── printEEPROMUsage() - Debug output
    └── EEPROM read/write logic
```

### Variable Ownership

**CRITICAL: Understanding extern vs. Definition**

Variables can only be **DEFINED** once across all files. They can be **DECLARED** (extern) many times.

**Defined in CalibrationMode.cpp (shared variables):**

```cpp
int buttonState[3][4];
bool lastButtonState[3][4];
int selectedNoteIndex;
int octaveShift;  // ← Shared between calibration and normal mode!
unsigned long lastCalibrationButtonPressTime;
unsigned long downOctaveButtonPressTime;
bool inCalibrationMode;
bool calibrationModeToggleAcknowledged;
float calibrationValues[49];
float defaultCalibrationValues[49];
bool testMode;
```

**Defined in main .ino (firmware-specific):**

```cpp
Adafruit_MCP4725 dac;
byte rowPins[3];
byte colPins[4];
// ... pin definitions
float currentVoltage;  // Slew system
float targetVoltage;
float portamentoRate;
unsigned long lastPortamentoUpdate;
// ... debouncing variables
```

**Used via extern in main .ino:**

```cpp
#include "CalibrationMode.h"  // Brings in extern declarations
// Now you can use: octaveShift, calibrationValues, inCalibrationMode, etc.
```

---

## Key Algorithms

### 1. Proportional Rate Slew System

**Concept:** Move a percentage of the remaining distance each update.

**Update Function:**

```cpp
void updateSlew() {
    if (portamentoRate > 0 && currentVoltage != targetVoltage) {
        unsigned long currentTime = micros();
        if (currentTime - lastPortamentoUpdate >= 500) {  // 2000 Hz update rate
            float difference = targetVoltage - currentVoltage;
            if (abs(difference) > 0.001) {
                // Move proportionally toward target
                currentVoltage += difference * portamentoRate;
                dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
            } else {
                // Snap to final value when very close
                currentVoltage = targetVoltage;
                dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
            }
            lastPortamentoUpdate = currentTime;
        }
    }
}
```

**Mathematics:**

Time to reach 99% of target:

```
time = -ln(0.01) / (rate × 2000 Hz)
time = 4.605 / (rate × 2000)
```

Solving for rate:

```
rate = 4.605 / (time × 2000)
```

**Example calculations:**
| Desired Time | Calculation | Rate |
|--------------|-------------|------|
| 30ms = 0.03s | 4.605 / (0.03 × 2000) | 0.07675 |
| 500ms = 0.5s | 4.605 / (0.5 × 2000) | 0.004605 |
| 2000ms = 2s | 4.605 / (2 × 2000) | 0.001151 |

**Why rates must be < 1.0:**

If rate ≥ 1.0, the voltage would overshoot the target and oscillate:

```cpp
// Example with rate = 1.5
currentVoltage = 1.0;
targetVoltage = 2.0;
difference = 2.0 - 1.0 = 1.0;
currentVoltage += 1.0 * 1.5 = 1.0 + 1.5 = 2.5  // Overshot!
// Next iteration: difference = 2.0 - 2.5 = -0.5
currentVoltage += -0.5 * 1.5 = 2.5 - 0.75 = 1.75  // Undershot!
// Oscillates forever
```

### 2. Two-Zone Potentiometer Mapping

**Design Goals:**

- 70% of pot for most-used fast range (30-500ms)
- 30% of pot for dramatic slow range (550-2000ms)
- Dead zone at minimum for instant notes
- Exponential in fast zone for fine control
- Linear in slow zone for predictability

**Implementation:**

```cpp
int potValue = analogRead(potentiometerPin);
int effectiveRange = constrain(potValue, 0, 800);  // Limit to 0-800

if (effectiveRange < 16) {
    // Dead zone (0-2%)
    portamentoRate = 0;
}
else if (effectiveRange < 560) {
    // Fast zone (2-70%): Exponential mapping
    float normalized = (effectiveRange - 16) / 544.0;  // 0.0 to 1.0
    float exponential = pow(normalized, 2);  // Square for exponential feel
    portamentoRate = 0.07675 - (exponential * (0.07675 - 0.004605));
    portamentoRate = constrain(portamentoRate, 0.004605, 0.07675);
}
else {
    // Slow zone (70-100%): Linear mapping
    float normalized = (effectiveRange - 560) / 240.0;  // 0.0 to 1.0
    portamentoRate = 0.004186 - (normalized * (0.004186 - 0.001151));
    portamentoRate = constrain(portamentoRate, 0.001151, 0.004186);
}
```

**Visual representation:**

```
Pot:  0────16───────────560──────────800───(1023)
      │    │             │           │
      Dead Fast (70%)    Slow (30%)  │
      │    Exponential   Linear      │
      │    30-500ms      550-2000ms  │
      └────────── Musical Range ──────┘
```

### 3. Calibration System

**Per-Note Voltage Storage:**

49 notes (C0 to C4) each have a calibration value stored in EEPROM.

**Storage format:**

- Integer scaled by 1000 (e.g., 1.083V stored as 1083)
- 2 bytes per note (int)
- Total: 98 bytes for all 49 notes

**Calibration adjustment:**

```cpp
float defaultVoltage = defaultCalibrationValues[selectedNoteIndex];
float adjustmentRange = 1.0 / 4.0;  // ±3 semitones (1/4 of an octave)
float potAdjustment = (analogRead(potentiometerPin) / 1023.0) * adjustmentRange - (adjustmentRange / 2.0);
float currentVoltage = defaultVoltage + potAdjustment;
```

**Example for C1 (note index 12):**

- Default: 1.000V
- Adjustment range: 0.875V to 1.125V
- Pot at 0: 1.000 - 0.125 = 0.875V
- Pot at 512: 1.000 + 0.000 = 1.000V
- Pot at 1023: 1.000 + 0.125 = 1.125V

---

## Hardware Interface

### Button Matrix Scanning

**Configuration:**

- 3 rows (digital pins 2, 3, 4)
- 4 columns (digital pins 5, 6, 7, 8)
- INPUT_PULLUP for rows
- OUTPUT for columns

**Scanning algorithm:**

```cpp
for (int c = 0; c < cols; c++) {
    digitalWrite(colPins[c], LOW);  // Activate column

    for (int r = 0; r < rows; r++) {
        bool currentState = !digitalRead(rowPins[r]);  // Active low
        // Process button state...
    }

    digitalWrite(colPins[c], HIGH);  // Deactivate column
}
```

**Note calculation:**

```cpp
int noteIndex = r * cols + c + octaveShift * 12;
```

**Matrix layout:**

```
        Col 0   Col 1   Col 2   Col 3
Row 0:  C (0)   C# (1)  D (2)   D# (3)
Row 1:  E (4)   F (5)   F# (6)  G (7)
Row 2:  G# (8)  A (9)   A# (10) B (11)
```

### DAC (MCP4725)

**I2C Communication:**

- Address: 0x60
- 12-bit resolution (0-4095)
- Output range: 0V to VCC (~4.93V)

**Voltage calculation:**

```cpp
int dacValue = (int)(voltage * (4095 / 4.93));
dac.setVoltage(dacValue, false);  // false = don't write to EEPROM
```

**Why 4.93V?**

- Arduino VCC is nominally 5V
- Actual VCC varies: typically 4.7V to 5.1V
- 4.93V is a measured average for better accuracy
- Can be fine-tuned during calibration

### Debouncing

**Strategy:** Time-based debouncing with per-button tracking

**Implementation:**

```cpp
const unsigned long debounceDelayNoteButtons = 60;  // 60ms
unsigned long lastDebounceTimeMatrix[rows][cols];

if (currentButtonState != lastButtonState[r][c]) {
    if (currentMillis - lastDebounceTimeMatrix[r][c] > debounceDelayNoteButtons) {
        lastButtonState[r][c] = currentButtonState;
        // Process button change
        lastDebounceTimeMatrix[r][c] = currentMillis;
    }
}
```

**Timing constants:**

- Note buttons: 60ms (responsive for playing)
- Octave buttons: 250ms (prevent accidental double-presses)

---

## Development Guidelines

### 1. Modifying the Firmware

**DO:**

- Add new features in the main .ino file
- Create new helper functions for new features
- Use existing shared variables via extern
- Document your changes with comments
- Test in both normal and calibration modes

**DON'T:**

- Modify CalibrationMode.cpp or EEPROMHandling.cpp
- Redefine variables that exist in shared files
- Change EEPROM addresses 0-103 or 200-201
- Add blocking delays in the main loop
- Break calibration mode functionality

### 2. Adding New Features

**Example: Adding a new mode**

```cpp
// In main .ino, add new global variable
enum PlayMode { NORMAL, ARPEGGIO, SEQUENCE };
PlayMode currentPlayMode = NORMAL;

// Add new function
void handleArpeggioMode() {
    // Your arpeggio logic here
}

// Modify loop() to call your function
void loop() {
    // ... octave button handling

    if (inCalibrationMode) {
        handleCalibrationMode();
    } else {
        switch (currentPlayMode) {
            case NORMAL:
                // Existing normal operation code
                break;
            case ARPEGGIO:
                handleArpeggioMode();
                break;
            // ... etc
        }

        updateSlew();  // Always call this at the end
    }
}
```

### 3. Memory Management

**Arduino Nano ATmega328P:**

- Flash: 32KB (30KB available after bootloader)
- SRAM: 2KB
- EEPROM: 1KB

**Tips:**

- Use `const` for constants (stores in Flash instead of RAM)
- Use `F()` macro for strings: `Serial.println(F("Text"));`
- Minimize global variables
- Be careful with large arrays

**Check memory usage:**

```cpp
void printMemoryUsage() {
    extern int __heap_start, *__brkval;
    int v;
    int freeMemory = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
    Serial.print(F("Free memory: "));
    Serial.println(freeMemory);
}
```

### 4. EEPROM Best Practices

**Writing to EEPROM:**

```cpp
// Always check bounds
if (address >= 0 && address < EEPROM.length()) {
    EEPROM.put(address, value);
}

// Avoid protected regions
if (address >= 0 && address <= 103) {
    // Protected! Don't write here unless you're handling calibration
    return;
}
if (address == 200 || address == 201) {
    // Skip these addresses
    return;
}
```

**EEPROM wear leveling:**

- EEPROM has ~100,000 write cycles per cell
- Don't write on every loop iteration
- Only write when values actually change
- Use flags to track changes

---

## Testing Procedures

### Pre-Compilation Tests

** Code Review Checklist:**

- [ ] No variables defined in both .ino and .cpp files
- [ ] All includes present at top of file
- [ ] No syntax errors (IDE shows green checkmark)
- [ ] Comments explain non-obvious logic
- [ ] Version number updated in comments

### Compilation Tests

**Compile Checklist:**

```bash
# Arduino IDE:
# 1. Click "Verify" button
# 2. Check for 0 errors, 0 warnings
# 3. Note sketch size (should be < 30KB)

# Arduino CLI:
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328 SlightOfHand_v1-5-4-Stable.ino
```

**Memory usage:**

- Flash: Should be < 28KB (leaves margin)
- RAM: Should be < 1.5KB (leaves margin)

### Post-Upload Tests

**Basic Functionality:**

1. Power on normally (not in calibration mode)
2. Press each keyboard button (C to B)
3. Verify gate output HIGH when pressed
4. Verify CV output changes with each note
5. Test octave up/down buttons
6. Test High C button
7. Turn potentiometer and verify slew changes

**Calibration Mode:**

1. Power off, hold octave up + down, power on
2. Verify gate output HIGH (confirms entry)
3. Press keyboard buttons without holding High C
4. Verify voltages display correctly
5. Test octave buttons (should increment by 1)
6. Hold High C and turn pot
7. Verify voltage adjusts smoothly
8. Release High C and verify value persists
9. Exit calibration mode (hold both octave 2 seconds)
10. Re-enter and verify saved values persist

**Edge Cases:**

1. Test pot at minimum (instant slew)
2. Test pot at maximum (2-second slew)
3. Test rapid button presses (no missed notes)
4. Test holding multiple buttons simultaneously
5. Test octave 0, 1, 2, 3 (all should work)
6. Test octave buttons in calibration (should allow 0-4)

### Performance Tests

**Timing Analysis:**

```cpp
void loop() {
    unsigned long startTime = micros();

    // ... main loop code ...

    unsigned long loopTime = micros() - startTime;
    if (loopTime > 1000) {  // More than 1ms
        Serial.print(F("Slow loop: "));
        Serial.print(loopTime);
        Serial.println(F(" us"));
    }
}
```

**Target:** Loop should complete in < 500 microseconds typically

---

## Common Pitfalls

### 1. The "Multiple Definition" Error

**Problem:**

```
error: multiple definition of 'octaveShift'
```

**Cause:**
Variable defined in both main .ino and CalibrationMode.cpp

**Solution:**

```cpp
// WRONG - in main .ino:
int octaveShift = 0;  //  Already defined in CalibrationMode.cpp!

// RIGHT - in main .ino:
#include "CalibrationMode.h"  // Use via extern
// Now you can use octaveShift directly
```

### 2. Calibration Mode Octave Bugs

**Problem:**
Calibration skips octaves or starts at wrong octave

**Cause:**
`octaveShift` not reset when entering calibration mode

**Solution:**

```cpp
if (!digitalRead(octaveUpPin) && !digitalRead(octaveDownPin)) {
    inCalibrationMode = true;
    octaveShift = 0;  //  Always reset!
    digitalWrite(triggerPin, HIGH);
}
```

### 3. Double-Increment Bug

**Problem:**
Pressing octave button once increments by 2 in calibration mode

**Cause:**
Both main loop and handleCalibrationMode() handle octave buttons

**Solution:**

```cpp
void loop() {
    // Only handle octave buttons in normal mode
    if (!inCalibrationMode) {  //  Check mode first!
        if ((millis() - lastDebounceTimeOctave) > debounceDelayOctaveButtons) {
            // ... handle octave buttons
        }
    }

    if (inCalibrationMode) {
        handleCalibrationMode();  // This handles its own octave buttons
    }
}
```

### 4. DAC Voltage Range

**Problem:**
Highest notes don't reach 4.0V

**Cause:**
MCP4725 DAC output limited by VCC (~4.93V typical, but varies)

**Solution:**

```cpp
// Use measured VCC in calculation
float vcc = 4.93;  // Measure your actual VCC
int dacValue = (int)(voltage * (4095 / vcc));
```

### 5. Portamento Oscillation

**Problem:**
Very fast slew rates cause voltage to oscillate/glitch

**Cause:**
Proportional rate > 1.0 causes overshoot

**Solution:**

```cpp
// Minimum slew time is 30ms (rate 0.07675)
// Don't allow faster rates
portamentoRate = constrain(portamentoRate, 0.0, 0.1);  //  Cap at 0.1
```

### 6. EEPROM Corruption

**Problem:**
Calibration data gets corrupted or lost

**Cause:**
Writing to protected EEPROM addresses

**Solution:**

```cpp
// Always skip protected regions
if (address >= 0 && address <= 103) return;  //  Calibration data
if (address == 200 || address == 201) return;  //  Reserved
```

---

## Debugging Tips

### Serial Debugging

**Enable debug output:**

```cpp
void setup() {
    Serial.begin(9600);
    while (!Serial) { ; }  // Wait for serial port
    Serial.println(F("Slight Of Hand v1.5.4"));
}

void loop() {
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 'd') {  // Debug dump
            Serial.print(F("octaveShift: ")); Serial.println(octaveShift);
            Serial.print(F("inCalibrationMode: ")); Serial.println(inCalibrationMode);
            Serial.print(F("portamentoRate: ")); Serial.println(portamentoRate, 6);
            Serial.print(F("currentVoltage: ")); Serial.println(currentVoltage, 4);
        }
    }
}
```

### Visual Debugging (LED)

**Use built-in LED for status:**

```cpp
// Blink pattern indicates state
void blinkStatus(int pattern) {
    switch (pattern) {
        case 0: // Normal operation
            digitalWrite(LED_BUILTIN, LOW);
            break;
        case 1: // Calibration mode
            digitalWrite(LED_BUILTIN, HIGH);
            break;
        case 2: // Error state
            for (int i = 0; i < 10; i++) {
                digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
                delay(50);
            }
            break;
    }
}
```

### Logic Analyzer

For precise timing analysis:

- Use cheap 8-channel logic analyzer ($10-20)
- Connect to gate output and key pins
- Analyze debouncing and timing

---

## Performance Optimization

### Loop Iteration Time

**Measured in v1.5.4:**

- Typical: 200-400 microseconds
- Maximum: < 1 millisecond
- Target: < 500 microseconds average

**Optimization strategies:**

1. Minimize function calls in loop
2. Use local variables when possible
3. Avoid floating-point math in hot paths
4. Cache frequently-used values

### Function Call Reduction

**v1.5.3a:** 7 calls to `updateSlew()` per loop  
**v1.5.4:** 1 call to `updateSlew()` per loop  
**Improvement:** 85% reduction

**How:**

- Moved all slew updates to single end-of-loop call
- Removed scattered `updateSlew()` calls throughout code

---

## Version Control Best Practices

### Branch Strategy

```
main
├── develop
│   ├── feature/new-mode
│   ├── feature/midi-output
│   └── bugfix/calibration-issue
└── release/v1.5.4
```

### Commit Messages

**Good:**

```
Fix: Reset octaveShift when entering calibration mode

Calibration was starting at wrong octave if octaveShift
was non-zero from previous normal operation. Now explicitly
reset to 0 when entering calibration mode.

Fixes #42
```

**Bad:**

```
fixed bug
```

### Versioning

Increment version when:

- **Patch (1.5.X)**: Bug fixes, no new features
- **Minor (1.X.0)**: New features, backward compatible
- **Major (X.0.0)**: Breaking changes, incompatible

---

## Resources

### Documentation

- [Arduino Reference](https://www.arduino.cc/reference/en/)
- [MCP4725 Datasheet](https://www.adafruit.com/product/935)
- [Eurorack Standards](http://www.doepfer.de/a100_man/a100m_e.htm)

### Tools

- [Arduino IDE](https://www.arduino.cc/en/software)
- [Arduino CLI](https://arduino.github.io/arduino-cli/)
- [PlatformIO](https://platformio.org/) (alternative)

### Community

- [FlatSix Modular Website](https://www.flatsixmodular.com)
- [Lines Forum](https://llllllll.co/)
- [ModWiggler Forum](https://www.modwiggler.com/)

---

**Last Updated:** 2025-01-19  
**Document Version:** 1.0  
**Author:** Sean Rieger / FlatSix Modular
