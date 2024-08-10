// Slight Of Hand V1.5.3 - Firmware for the Nocturne Alchemy Platform, a 1U Eurorack keyboard module by FlatSix Modular
// Copyright (c) 2024 FlatSix Modular. All rights reserved.
//
// This software is licensed under the Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0).
// You are free to share, copy, and redistribute the material in any medium or format, and adapt, remix, transform, 
// and build upon the material under the following terms:
// - Attribution: You must give appropriate credit, provide a link to the license, and indicate if changes were made.
// - NonCommercial: You may not use the material for commercial purposes.
//
// To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/4.0/
// Unauthorized use or distribution of this software outside the terms of this license may result in civil and criminal penalties.
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <EEPROM.h>

Adafruit_MCP4725 dac;
const int rows = 3;
const int cols = 4;

// Button Matrix Handling - Notes C to B
byte rowPins[rows] = { 2, 3, 4 };     // Connect these to your button rows
byte colPins[cols] = { 5, 6, 7, 8 };  // Connect these to your button columns

const int octaveUpPin = 9;        // Connect to "Octave Up" button
const int octaveDownPin = 10;     // Connect to "Octave Down" button
const int triggerPin = 11;        // Connect to trigger output
const int highCButton = 12;       // Connect to the high C button
const int potentiometerPin = A0;  // Connect to the potentiometer

int buttonState[rows][cols];
int lastVoltage = 0;
int octaveShift = 0;
int selectedNoteIndex = -1;  // Used for note selection in calibration mode

const unsigned long debounceDelayOctaveButtons = 250;
const unsigned long debounceDelayNoteButtons = 60;
unsigned long lastDebounceTimeOctave = 0;
unsigned long lastDebounceTimeNotes = 0;
unsigned long lastDebounceTimeMatrix[rows][cols];
unsigned long lastCalibrationButtonPressTime = 0;
unsigned long downOctaveButtonPressTime = 0;

// Globals for NON BLOCKING SLEW
bool isSlewing = false;       // Flag to indicate if we're currently slewing
unsigned long slewStartTime;  // When the slew started
int slewStepTime;             // Time per step in milliseconds
int totalSlewTime = 0;        // Added here as a global variable
int targetVoltageRaw;         // Target voltage in DAC raw value
int stepDirection;            // 1 for increasing voltage, -1 for decreasing

bool lastHighCButtonState = false;
bool inCalibrationMode = false;
bool calibrationModeToggleAcknowledged = false;

// Create a 2D array to hold the last state of each button
bool lastButtonState[rows][cols];
float calibrationValues[49];

const int EEPROM_SIGNATURE_ADDR = 0;
const int EEPROM_CALIBRATION_START_ADDR = 4;
const int EEPROM_SIGNATURE_VALUE = 12345;
const int EEPROM_INTEGER_FLAG_ADDR = EEPROM_CALIBRATION_START_ADDR + 49 * sizeof(float); // Address for integer flag
const int EEPROM_INTEGER_FLAG_VALUE = 67890;

float defaultCalibrationValues[49] = {
  // Default Eurorack Values
  0.000, 0.083, 0.167, 0.250, 0.333, 0.417, 0.500, 0.583, 0.667, 0.750, 0.833, 0.917,
  1.000, 1.083, 1.167, 1.250, 1.333, 1.417, 1.500, 1.583, 1.667, 1.750, 1.833, 1.917,
  2.000, 2.083, 2.167, 2.250, 2.333, 2.417, 2.500, 2.583, 2.667, 2.750, 2.833, 2.917,
  3.000, 3.083, 3.167, 3.250, 3.333, 3.417, 3.500, 3.583, 3.667, 3.750, 3.833, 3.917,
  4.000
};

bool testMode = false; // Set this to true to enable test mode

void startSlew(float targetVoltage, unsigned long totalSlewTime) {
    targetVoltageRaw = (int)(targetVoltage * (4095 / 4.93));
    int totalVoltageDiff = abs(lastVoltage - targetVoltageRaw);

    if (totalVoltageDiff > 0 && totalSlewTime > 0) {
        isSlewing = true;
        slewStartTime = micros(); // Use micros() for timing
        slewStepTime = totalVoltageDiff > 0 ? totalSlewTime / totalVoltageDiff : totalSlewTime;
        stepDirection = (targetVoltageRaw > lastVoltage) ? 1 : -1;  // Determine direction of change
    } else {
        lastVoltage = targetVoltageRaw;
        dac.setVoltage(lastVoltage, false);
        isSlewing = false;  // Ensure slew flag is reset if no slew action is needed
    }
}

void updateSlew() {
    if (isSlewing) {
        unsigned long currentTime = micros(); // Use micros() for timing
        if (currentTime - slewStartTime >= slewStepTime) {
            lastVoltage += stepDirection;
            dac.setVoltage(lastVoltage, false);
            slewStartTime = currentTime;  // Reset timing for the next step

            if (lastVoltage == targetVoltageRaw) {
                isSlewing = false;  // Slew complete
            }
        }
    }
}

void blinkTriggerPulse(int count, int duration) {
  for (int i = 0; i < count; i++) {
    digitalWrite(triggerPin, HIGH);
    delay(duration);
    digitalWrite(triggerPin, LOW);
    delay(duration);
  }
}

void setup() {
  Wire.begin();
  dac.begin(0x60);
  Serial.begin(9600); // Initialize Serial communication

  for (int r = 0; r < rows; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
    for (int c = 0; c < cols; c++) {
      pinMode(colPins[c], OUTPUT);
      lastDebounceTimeMatrix[r][c] = 0;
    }
  }

  // Initialize all elements of lastButtonState to false
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      lastButtonState[r][c] = false;
    }
  }

  pinMode(octaveUpPin, INPUT_PULLUP);
  pinMode(octaveDownPin, INPUT_PULLUP);
  pinMode(triggerPin, OUTPUT);
  pinMode(highCButton, INPUT_PULLUP);
  pinMode(potentiometerPin, INPUT);

  digitalWrite(triggerPin, LOW);

  // EEPROM Handling so calibration data adjustments will be persistent
  int eepromSignature;
  EEPROM.get(EEPROM_SIGNATURE_ADDR, eepromSignature);

  if (eepromSignature != EEPROM_SIGNATURE_VALUE) {
    Serial.println("EEPROM signature not found. Initializing with default values.");
    if (!testMode) {
      EEPROM.put(EEPROM_SIGNATURE_ADDR, EEPROM_SIGNATURE_VALUE);
      for (int i = 0; i < 49; i++) {
        EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), defaultCalibrationValues[i]);
      }
      EEPROM.put(EEPROM_INTEGER_FLAG_ADDR, 0); // Set the integer flag to 0 (float format)
    } else {
      Serial.println("Test mode: EEPROM not written.");
    }
  }

  int integerFlag;
  EEPROM.get(EEPROM_INTEGER_FLAG_ADDR, integerFlag);

  if (integerFlag != EEPROM_INTEGER_FLAG_VALUE) {
    Serial.println("Float format detected. Converting to integer format.");
    for (int i = 0; i < 49; i++) {
      EEPROM.get(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), calibrationValues[i]);
      int scaledValue = (int)(calibrationValues[i] * 1000);
      if (!testMode) {
        EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(int), scaledValue);
      } else {
        Serial.print("Test mode: Would convert float ");
        Serial.print(calibrationValues[i], 4);
        Serial.print(" to int ");
        Serial.println(scaledValue);
      }
    }
    if (!testMode) {
      EEPROM.put(EEPROM_INTEGER_FLAG_ADDR, EEPROM_INTEGER_FLAG_VALUE);
    } else {
      Serial.println("Test mode: Integer flag not set.");
    }
  }

  for (int i = 0; i < 49; i++) {
    int scaledValue;
    EEPROM.get(EEPROM_CALIBRATION_START_ADDR + i * sizeof(int), scaledValue);
    calibrationValues[i] = scaledValue / 1000.0;
  }

  printEEPROMUsage();

  // Check if both octave buttons are pressed during power-up to enter calibration mode
  if (!digitalRead(octaveUpPin) && !digitalRead(octaveDownPin)) {
    inCalibrationMode = true;
    digitalWrite(triggerPin, HIGH); // Indicate that calibration mode is active
  }
}

void printEEPROMUsage() {
  int startAddress = EEPROM_CALIBRATION_START_ADDR;
  int endAddress = EEPROM_CALIBRATION_START_ADDR + 49 * sizeof(int) - 1;
  int bytesUsed = (endAddress - startAddress + 1) + 1; // Include flag byte

  Serial.println();
  Serial.print("EEPROM Usage Information:");
  Serial.print("\nTotal bytes used: ");
  Serial.println(bytesUsed);
  Serial.print("Data stored between addresses: ");
  Serial.print(startAddress);
  Serial.print(" and ");
  Serial.println(endAddress);
}

void loop() {
  // Call updateSlew to continuously update the voltage without blocking
  updateSlew();

  // Handle octave buttons
  if ((millis() - lastDebounceTimeOctave) > debounceDelayOctaveButtons) {
    if (!digitalRead(octaveUpPin)) {
      octaveShift = inCalibrationMode ? min(octaveShift + 1, 4) : min(octaveShift + 1, 3);
      lastDebounceTimeOctave = millis();
    }
    if (!digitalRead(octaveDownPin)) {
      octaveShift = max(octaveShift - 1, 0);
      lastDebounceTimeOctave = millis();
    }
  }

  // Calibration mode handling
  if (inCalibrationMode) {
    // Toggle out of calibration mode
    if (!digitalRead(octaveUpPin) && !digitalRead(octaveDownPin)) {
      if (millis() - lastCalibrationButtonPressTime > 2000 && !calibrationModeToggleAcknowledged) {
        inCalibrationMode = false;
        calibrationModeToggleAcknowledged = true;
        digitalWrite(triggerPin, LOW);
        lastCalibrationButtonPressTime = millis();
      }
    } else {
      lastCalibrationButtonPressTime = millis();
      calibrationModeToggleAcknowledged = false;
    }

    // Enter calibration mode
    for (int c = 0; c < cols; c++) {
      digitalWrite(colPins[c], LOW);

      for (int r = 0; r < rows; r++) {
        bool currentButtonState = !digitalRead(rowPins[r]);
        if (currentButtonState && !buttonState[r][c]) {
          selectedNoteIndex = r * cols + c + octaveShift * 12;
          if (octaveShift == 4 && r == 0 && c == 0) {  // Special case for High C4
            selectedNoteIndex = 48;
          }

          float currentVoltage = calibrationValues[selectedNoteIndex];
          dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
        }
        buttonState[r][c] = currentButtonState;
      }

      digitalWrite(colPins[c], HIGH);
    }

    // High C button serves as the "enter key" for calibration
    bool highCButtonState = !digitalRead(highCButton);
    if (highCButtonState && selectedNoteIndex != -1) {
      float defaultVoltage = defaultCalibrationValues[selectedNoteIndex];

      // Whole plus Half Step Adjustment Range
      float adjustmentRange = 1.0 / 4.0; // Updated to reflect a larger adjustment range
      float potAdjustment = (analogRead(potentiometerPin) / 1023.0) * adjustmentRange - (adjustmentRange / 2.0);
      float currentVoltage = defaultVoltage + potAdjustment;

      if (selectedNoteIndex == 0) {
        currentVoltage = max(currentVoltage, 0.0f);
      } else if (selectedNoteIndex == 48) {
        currentVoltage = min(currentVoltage, 4.0f);
      }

      calibrationValues[selectedNoteIndex] = currentVoltage;
      dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
      EEPROM.put(EEPROM_CALIBRATION_START_ADDR + selectedNoteIndex * sizeof(int), (int)(currentVoltage * 1000));

      selectedNoteIndex = -1;
    }

    // Check if the down octave button is held down for 8 seconds to reset calibration
    if (!digitalRead(octaveDownPin) && digitalRead(octaveUpPin)) {
      if (downOctaveButtonPressTime == 0) {                      // If the timer is not already running
        downOctaveButtonPressTime = millis();                    // Start the timer
      } else if (millis() - downOctaveButtonPressTime > 8000) {  // Check if 8 seconds have passed
        // Reset calibration values to defaults
        memcpy(calibrationValues, defaultCalibrationValues, sizeof(defaultCalibrationValues));
        for (int i = 0; i < sizeof(calibrationValues) / sizeof(calibrationValues[0]); i++) {
          EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(int), (int)(defaultCalibrationValues[i] * 1000));
        }
        blinkTriggerPulse(6, 250);      // Blink the trigger pulse for confirmation
        downOctaveButtonPressTime = 0;  // Reset the timer
      }
    } else {
      downOctaveButtonPressTime = 0;  // Reset the timer if the button is released
    }
  } else {
    updateSlew();
    // Normal operation logic ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    // Read the potentiometer value
    int potValue = analogRead(potentiometerPin);

    // Normalize the potentiometer value to a 0-1 range
    float normalizedValue = potValue / 1023.0;

    // Apply an exponential curve (e.g., using the square of the normalized value)
    // Adjusting the exponent if needed based on desired response curve
    float exponentialValue = pow(normalizedValue, 2); // This example squares the value

    // Scale the exponential value back to the desired range for totalSlewTime in microseconds
    // Adjust the range as needed for your application
    unsigned long totalSlewTime = (potValue < 2) ? 0 : map(exponentialValue * 1023, 0, 1023, 1, 2000000);

    updateSlew();

    bool isAnyButtonPressed = false;  // Track if any note button is pressed

    for (int c = 0; c < cols; c++) {
      digitalWrite(colPins[c], LOW);

      for (int r = 0; r < rows; r++) {
        bool currentButtonState = !digitalRead(rowPins[r]);
        unsigned long currentMillis = millis();

        if (currentButtonState != lastButtonState[r][c]) {
          if (currentMillis - lastDebounceTimeMatrix[r][c] > debounceDelayNoteButtons) {
            // Button state has changed and debounced
            lastButtonState[r][c] = currentButtonState;  // Update the button state

            updateSlew();

            if (currentButtonState) {
              // Button has been pressed
              isAnyButtonPressed = true;
              digitalWrite(triggerPin, HIGH);

              int noteIndex = r * cols + c + octaveShift * 12;  // Calculate note index
              float targetVoltage = applyCalibration(noteIndex);
              startSlew(targetVoltage, totalSlewTime);
            }
            lastDebounceTimeMatrix[r][c] = currentMillis;  // Update last debounce time
          }
        } else if (currentButtonState) {
          // If the button is held down, mark it as being pressed without retoggling the note
          isAnyButtonPressed = true;
        }
      }

      digitalWrite(colPins[c], HIGH);
    }

    updateSlew();

    // Debounce logic for High C button
    bool highCButtonState = !digitalRead(highCButton);
    unsigned long currentMillis = millis();

    if (highCButtonState != lastHighCButtonState && (currentMillis - lastDebounceTimeNotes >= debounceDelayNoteButtons)) {
      // High C button state has changed and debounced
      lastHighCButtonState = highCButtonState;  // Update last button state
      lastDebounceTimeNotes = currentMillis;    // Reset the debounce timer

      updateSlew();

      if (highCButtonState) {
        // High C button has been pressed
        digitalWrite(triggerPin, HIGH);
        int noteIndex = 48;  // Assume High C corresponds to C4 by default
        switch (octaveShift) {
          case 0: noteIndex = 12; break;  // C1
          case 1: noteIndex = 24; break;  // C2
          case 2: noteIndex = 36; break;  // C3
          case 3: noteIndex = 48; break;  // C4
        }
        float targetVoltage = applyCalibration(noteIndex);
        startSlew(targetVoltage, totalSlewTime);
      } else {
        // High C button has been released
        if (!isAnyButtonPressed) {
          digitalWrite(triggerPin, LOW);
        }
      }
    }

    updateSlew();

    // Unified Gate control logic
    if (isAnyButtonPressed || highCButtonState) {
      digitalWrite(triggerPin, HIGH);
    } else {
      digitalWrite(triggerPin, LOW);
    }
  }
  updateSlew();
}

float applyCalibration(int note) {
  if (note >= 0 && note < sizeof(calibrationValues) / sizeof(calibrationValues[0])) {
    return calibrationValues[note];
  }
  return 0;
}
