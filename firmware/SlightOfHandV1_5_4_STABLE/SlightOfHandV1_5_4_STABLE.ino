// Slight Of Hand V1.5.4 - Optimized Slew Implementation
// Based on v1.5.3a with improved portamento/slew system inspired by Seventh Summoner
// Firmware for the Nocturne Alchemy Platform, a 1U Eurorack keyboard module by FlatSix Modular
// Copyright (c) 2024 FlatSix Modular. All rights reserved.
//
// CHANGELOG v1.5.4:
// - Refactored slew/portamento system to use proportional rate approach
// - Reduced updateSlew() calls from 7 per loop to 1 per loop
// - Simplified slew state management
// - Improved performance and code clarity
// - Maintained identical functionality to v1.5.3a
// - Fixed calibration mode bug: Reset octaveShift to 0 when entering calibration mode
// - Fixed calibration mode octave button handling: Don't handle octave buttons in main loop during calibration

/*******************************
      Include Libraries
*******************************/
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <EEPROM.h>
#include "CalibrationMode.h"
#include "EEPROMHandling.h"

/*******************************
      Hardware & Constants
*******************************/
Adafruit_MCP4725 dac;
const int rows = 3;
const int cols = 4;

byte rowPins[rows] = { 2, 3, 4 };
byte colPins[cols] = { 5, 6, 7, 8 };

const int octaveUpPin = 9;
const int octaveDownPin = 10;
const int triggerPin = 11;
const int highCButton = 12;
const int potentiometerPin = A0;

/*******************************
      Global Variables
*******************************/
// Variables defined in CalibrationMode.cpp (declared extern in CalibrationMode.h):
// - buttonState, lastButtonState, selectedNoteIndex, octaveShift
// - lastCalibrationButtonPressTime, downOctaveButtonPressTime
// - inCalibrationMode, calibrationModeToggleAcknowledged
// - calibrationValues, defaultCalibrationValues, testMode

// OPTIMIZED SLEW VARIABLES (inspired by Seventh Summoner)
float currentVoltage = 0.0;      // Tracks current output voltage
float targetVoltage = 0.0;       // Voltage we're gliding towards
float portamentoRate = 0.0;      // 0 = instant, 0.1 = slowest (range: 0.0 - 0.1)
unsigned long lastPortamentoUpdate = 0;

// Debouncing
const unsigned long debounceDelayOctaveButtons = 250;
const unsigned long debounceDelayNoteButtons = 60;
unsigned long lastDebounceTimeOctave = 0;
unsigned long lastDebounceTimeNotes = 0;
unsigned long lastDebounceTimeMatrix[rows][cols];

bool lastHighCButtonState = false;

/*******************************
      Helper Functions
*******************************/

// NEW OPTIMIZED SLEW FUNCTION - Called once per loop
void updateSlew() {
    if (portamentoRate > 0 && currentVoltage != targetVoltage) {
        unsigned long currentTime = micros();
        if (currentTime - lastPortamentoUpdate >= 500) {  // Update every 500 microseconds
            float difference = targetVoltage - currentVoltage;
            if (abs(difference) > 0.001) {  // Only update if difference is significant
                // Move current voltage toward target voltage proportionally
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

// NEW SIMPLIFIED NOTE OUTPUT FUNCTION
void outputNoteVoltage(int note) {
    targetVoltage = applyCalibration(note);
    if (portamentoRate == 0) {
        // No portamento, instant voltage change
        currentVoltage = targetVoltage;
        dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
    }
    // If portamento > 0, voltage will glide in updateSlew()
}

void blinkTriggerPulse(int count, int duration) {
    for (int i = 0; i < count; i++) {
        digitalWrite(triggerPin, HIGH);
        delay(duration);
        digitalWrite(triggerPin, LOW);
        delay(duration);
    }
}

float applyCalibration(int note) {
    if (note >= 0 && note < sizeof(calibrationValues) / sizeof(calibrationValues[0])) {
        return calibrationValues[note];
    }
    return 0;
}

/*******************************
      Setup Function
*******************************/
void setup() {
    Wire.begin();
    dac.begin(0x60);
    Serial.begin(9600);

    // Initialize button matrix
    for (int r = 0; r < rows; r++) {
        pinMode(rowPins[r], INPUT_PULLUP);
        for (int c = 0; c < cols; c++) {
            pinMode(colPins[c], OUTPUT);
            lastDebounceTimeMatrix[r][c] = 0;
        }
    }

    // Initialize lastButtonState array
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            lastButtonState[r][c] = false;
        }
    }

    // Initialize hardware pins
    pinMode(octaveUpPin, INPUT_PULLUP);
    pinMode(octaveDownPin, INPUT_PULLUP);
    pinMode(triggerPin, OUTPUT);
    pinMode(highCButton, INPUT_PULLUP);
    pinMode(potentiometerPin, INPUT);

    digitalWrite(triggerPin, LOW);

    // Load calibration data from EEPROM
    handleEEPROM();

    // Check if both octave buttons are pressed during power-up to enter calibration mode
    if (!digitalRead(octaveUpPin) && !digitalRead(octaveDownPin)) {
        inCalibrationMode = true;
        octaveShift = 0;  // Reset octave shift to start calibration at octave 0
        digitalWrite(triggerPin, HIGH);
    }
}

/*******************************
      Main Loop
*******************************/
void loop() {
    // Handle octave buttons ONLY in normal mode (not in calibration mode)
    // Calibration mode handles its own octave buttons in handleCalibrationMode()
    if (!inCalibrationMode) {
        if ((millis() - lastDebounceTimeOctave) > debounceDelayOctaveButtons) {
            if (!digitalRead(octaveUpPin)) {
                octaveShift = min(octaveShift + 1, 3);
                lastDebounceTimeOctave = millis();
            }
            if (!digitalRead(octaveDownPin)) {
                octaveShift = max(octaveShift - 1, 0);
                lastDebounceTimeOctave = millis();
            }
        }
    }

    if (inCalibrationMode) {
        handleCalibrationMode();
    } else {
        // NORMAL OPERATION

        // Read potentiometer and calculate portamento rate
        // Musical two-zone mapping: 70% for useful fast range (30-500ms), 30% for slow range (550-2000ms)
        int potValue = analogRead(potentiometerPin);
        int effectiveRange = constrain(potValue, 0, 800);  // Limit to 0-800
        
        if (effectiveRange < 16) {
            // Dead zone: instant notes (accounts for pot variance)
            portamentoRate = 0;
        }
        else if (effectiveRange < 560) {
            // First 70% of pot (16-560): Musical fast range (30ms to 500ms)
            // Using exponential curve for finer control at the fast end
            float normalized = (effectiveRange - 16) / 544.0;  // 0.0 to 1.0
            float exponential = pow(normalized, 2);  // Square for exponential feel
            
            // Map exponentially from rate 0.07675 (30ms) to 0.004605 (500ms)
            portamentoRate = 0.07675 - (exponential * (0.07675 - 0.004605));
            portamentoRate = constrain(portamentoRate, 0.004605, 0.07675);
        }
        else {
            // Last 30% of pot (560-800): Dramatic slow range (550ms to 2000ms)
            // Linear mapping for the slow range
            float normalized = (effectiveRange - 560) / 240.0;  // 0.0 to 1.0
            
            // Map linearly from rate 0.004186 (550ms) to 0.001151 (2000ms)
            portamentoRate = 0.004186 - (normalized * (0.004186 - 0.001151));
            portamentoRate = constrain(portamentoRate, 0.001151, 0.004186);
        }

        bool isAnyButtonPressed = false;

        // Scan button matrix for note presses
        for (int c = 0; c < cols; c++) {
            digitalWrite(colPins[c], LOW);

            for (int r = 0; r < rows; r++) {
                bool currentButtonState = !digitalRead(rowPins[r]);
                unsigned long currentMillis = millis();

                if (currentButtonState != lastButtonState[r][c]) {
                    if (currentMillis - lastDebounceTimeMatrix[r][c] > debounceDelayNoteButtons) {
                        lastButtonState[r][c] = currentButtonState;

                        if (currentButtonState) {
                            // Button pressed
                            isAnyButtonPressed = true;
                            digitalWrite(triggerPin, HIGH);

                            int noteIndex = r * cols + c + octaveShift * 12;
                            outputNoteVoltage(noteIndex);
                        }
                        lastDebounceTimeMatrix[r][c] = currentMillis;
                    }
                } else if (currentButtonState) {
                    // Button held down
                    isAnyButtonPressed = true;
                }
            }

            digitalWrite(colPins[c], HIGH);
        }

        // Handle High C button
        bool highCButtonState = !digitalRead(highCButton);
        unsigned long currentMillis = millis();

        if (highCButtonState != lastHighCButtonState && 
            (currentMillis - lastDebounceTimeNotes >= debounceDelayNoteButtons)) {
            lastHighCButtonState = highCButtonState;
            lastDebounceTimeNotes = currentMillis;

            if (highCButtonState) {
                // High C pressed
                digitalWrite(triggerPin, HIGH);
                int noteIndex = 48;  // Default C4
                switch (octaveShift) {
                    case 0: noteIndex = 12; break;  // C1
                    case 1: noteIndex = 24; break;  // C2
                    case 2: noteIndex = 36; break;  // C3
                    case 3: noteIndex = 48; break;  // C4
                }
                outputNoteVoltage(noteIndex);
            } else {
                // High C released
                if (!isAnyButtonPressed) {
                    digitalWrite(triggerPin, LOW);
                }
            }
        }

        // Unified gate control
        if (isAnyButtonPressed || highCButtonState) {
            digitalWrite(triggerPin, HIGH);
        } else {
            digitalWrite(triggerPin, LOW);
        }

        // SINGLE SLEW UPDATE - Called once per loop iteration
        updateSlew();
    }
}
