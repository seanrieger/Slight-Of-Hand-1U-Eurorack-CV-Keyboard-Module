#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <EEPROM.h>

Adafruit_MCP4725 dac;
const int rows = 3;
const int cols = 4;
byte rowPins[rows] = {2, 3, 4}; // Connect these to your button rows
byte colPins[cols] = {5, 6, 7, 8}; // Connect these to your button columns

const int octaveUpPin = 9; // Connect to "Octave Up" button
const int octaveDownPin = 10; // Connect to "Octave Down" button
const int triggerPin = 11; // Connect to trigger output
const int highCButton = 12; // Connect to the high C button
const int potentiometerPin = A0; // Connect to the potentiometer

int buttonState[rows][cols];
int lastVoltage = 0;
int octaveShift = 0;
int selectedNoteIndex = -1; // This is for the High C being an enter button in calibration mode

const unsigned long debounceDelayOctaveButtons = 250;
const unsigned long debounceDelayNoteButtons = 70;
unsigned long lastDebounceTimeOctave = 0;
unsigned long lastDebounceTimeNotes = 0;
unsigned long lastDebounceTimeMatrix[rows][cols];
unsigned long lastCalibrationButtonPressTime = 0; // Add this at the global level
unsigned long downOctaveButtonPressTime = 0; // For resetting to defaults in the calibration mode

bool lastHighCButtonState = false;
bool inCalibrationMode = false;
bool calibrationModeToggleAcknowledged = false; // Add this at the global level

float calibrationValues[48];

// EEPROM Initialization
const int EEPROM_SIGNATURE_ADDR = 0;
const int EEPROM_CALIBRATION_START_ADDR = 4;
const int EEPROM_SIGNATURE_VALUE = 12345;

// Default calibration values
float defaultCalibrationValues[48] = {
  // Your default calibration Real World Values, Load in perfect world values, and then measure real world voltages then add here and activate
  //  0.001, 0.083, 0.167, 0.250, 0.335, 0.418, 0.500, 0.584, 0.668, 0.751, 0.834, 0.916, // C0 to B0
  //  0.998, 1.080, 1.163, 1.246, 1.329, 1.412, 1.494, 1.576, 1.668, 1.742, 1.826, 1.909, // C1 to B1
  //  1.998, 2.076, 2.159, 2.243, 2.327, 2.409, 2.492, 2.575, 2.660, 2.743, 2.826, 2.909, // C2 to B2
  //  2.992, 3.077, 3.158, 3.241, 3.325, 3.407, 3.489, 3.572, 3.656, 3.741, 3.824, 3.907  // C3 to B3
   
  //Default Eurorack Values - Perfect world
    0.000, 0.083, 0.167, 0.250, 0.333, 0.417, 0.500, 0.583, 0.667, 0.750, 0.833, 0.917, // C0 to B0
    1.000, 1.083, 1.167, 1.250, 1.333, 1.417, 1.500, 1.583, 1.667, 1.750, 1.833, 1.917, // C1 to B1
    2.000, 2.083, 2.167, 2.250, 2.333, 2.417, 2.500, 2.583, 2.667, 2.750, 2.833, 2.917, // C2 to B2
    3.000, 3.083, 3.167, 3.250, 3.333, 3.417, 3.500, 3.583, 3.667, 3.750, 3.833, 3.917,  // C3 to B3
};

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
    Serial.begin(9600);

    for (int r = 0; r < rows; r++) {
        pinMode(rowPins[r], INPUT_PULLUP);
        for (int c = 0; c < cols; c++) {
            pinMode(colPins[c], OUTPUT);
            lastDebounceTimeMatrix[r][c] = 0;
        }
    }

    pinMode(octaveUpPin, INPUT_PULLUP);
    pinMode(octaveDownPin, INPUT_PULLUP);
    pinMode(triggerPin, OUTPUT);
    pinMode(highCButton, INPUT_PULLUP);
    pinMode(potentiometerPin, INPUT);

    digitalWrite(triggerPin, LOW);

    // Check if EEPROM is initialized
    int eepromSignature;
    EEPROM.get(EEPROM_SIGNATURE_ADDR, eepromSignature);

    if (eepromSignature != EEPROM_SIGNATURE_VALUE) {
        // EEPROM not initialized, write default calibration data
        EEPROM.put(EEPROM_SIGNATURE_ADDR, EEPROM_SIGNATURE_VALUE);

        for (int i = 0; i < sizeof(defaultCalibrationValues) / sizeof(defaultCalibrationValues[0]); i++) {
            EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), defaultCalibrationValues[i]);
        }
    }

    // Read calibration data from EEPROM
    for (int i = 0; i < sizeof(calibrationValues) / sizeof(calibrationValues[0]); i++) {
        EEPROM.get(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), calibrationValues[i]);
    }
}

void loop() {
    // Check for long hold on both octave buttons to toggle calibration mode
    if (!digitalRead(octaveUpPin) && !digitalRead(octaveDownPin)) {
        if (millis() - lastCalibrationButtonPressTime > 2000 && !calibrationModeToggleAcknowledged) {
            inCalibrationMode = !inCalibrationMode;
            calibrationModeToggleAcknowledged = true; // Acknowledge that mode change has occurred
            digitalWrite(triggerPin, inCalibrationMode ? HIGH : LOW); // Example
            Serial.println(inCalibrationMode ? "Entering Calibration Mode" : "Exiting Calibration Mode");
            lastCalibrationButtonPressTime = millis();
        }
    } else {
        lastCalibrationButtonPressTime = millis(); // Reset the timer when buttons are not pressed
        calibrationModeToggleAcknowledged = false; // Reset acknowledgement when buttons are released
    }

    // Handle octave shifting in both calibration mode and normal operation
    if ((millis() - lastDebounceTimeOctave) > debounceDelayOctaveButtons) {
        if (!digitalRead(octaveUpPin)) {
            octaveShift = min(octaveShift + 1, 3); // Limit to 3 for the highest octave (C3)
            lastDebounceTimeOctave = millis();
        }
        if (!digitalRead(octaveDownPin)) {
            octaveShift = max(octaveShift - 1, 0); // Limit to 0 for the lowest octave (C0)
            lastDebounceTimeOctave = millis();
        }
    }

// BEGIN CALIBRATION MODE - User enters mode by holding both octave buttons for 2 seconds

if (inCalibrationMode) {
    // Enter calibration mode
    for (int c = 0; c < cols; c++) {
        digitalWrite(colPins[c], LOW);

        for (int r = 0; r < rows; r++) {
            bool currentButtonState = !digitalRead(rowPins[r]);
            if (currentButtonState && !buttonState[r][c]) {
                // Note selection logic
                selectedNoteIndex = r * cols + c + octaveShift * 12;
                Serial.print("Note selected for calibration: ");
                Serial.println(selectedNoteIndex);
                Serial.println("Press High C to confirm and adjust.");
            }
            buttonState[r][c] = currentButtonState;
        }

        digitalWrite(colPins[c], HIGH);
    }

    // High C button serves as the "enter key" for calibration
    bool highCButtonState = !digitalRead(highCButton);
    if (highCButtonState && selectedNoteIndex != -1 && selectedNoteIndex != 48) {
        // Calibration logic for the selected note
        float defaultVoltage = defaultCalibrationValues[selectedNoteIndex]; // Load default voltage
        float adjustmentRange = 1.0 / 12.0; // One half-step range
        float potAdjustment = (analogRead(potentiometerPin) / 1023.0) * adjustmentRange - (adjustmentRange / 2.0);
        float currentVoltage = defaultVoltage + potAdjustment;

        // Apply voltage range limits for C0 and High C3
        if (selectedNoteIndex == 0) { // For C0, limit to 0V at the lowest
            currentVoltage = max(currentVoltage, 0.0f);
        } else if (selectedNoteIndex == 48) { // For High C3, limit to 4V at the highest
            currentVoltage = min(currentVoltage, 4.0f);
        }

        Serial.print("Note Index: ");
        Serial.print(selectedNoteIndex);
        Serial.print(", Adjusted Voltage: ");
        Serial.println(currentVoltage);

        calibrationValues[selectedNoteIndex] = currentVoltage; // Update calibration value
        dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false); // Output adjusted voltage to DAC
        EEPROM.put(EEPROM_CALIBRATION_START_ADDR + selectedNoteIndex * sizeof(float), calibrationValues[selectedNoteIndex]);

        selectedNoteIndex = -1; // Reset the selected note index
    }

    // Check if the down octave button is held down for 8 seconds to reset calibration
    if (!digitalRead(octaveDownPin) && digitalRead(octaveUpPin)) {
        if (downOctaveButtonPressTime == 0) { // If the timer is not already running
            downOctaveButtonPressTime = millis(); // Start the timer
        } else if (millis() - downOctaveButtonPressTime > 8000) { // Check if 8 seconds have passed
            // Reset calibration values to defaults
            memcpy(calibrationValues, defaultCalibrationValues, sizeof(defaultCalibrationValues));
            for (int i = 0; i < sizeof(calibrationValues) / sizeof(calibrationValues[0]); i++) {
                EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), calibrationValues[i]);
            }
            Serial.println("Calibration values reset to defaults.");
            blinkTriggerPulse(6, 250); // Blink the trigger pulse for confirmation
            downOctaveButtonPressTime = 0; // Reset the timer
        }
    } else {
        downOctaveButtonPressTime = 0; // Reset the timer if the button is released
    }


    } else {
       // ... Normal operation logic ...
        int potValue = analogRead(potentiometerPin);
        int slewRate = (potValue < 5) ? 0 : map(potValue, 5, 1023, 1, 30);

        // Scan each column of the button matrix
        for (int c = 0; c < cols; c++) {
            digitalWrite(colPins[c], LOW);

            for (int r = 0; r < rows; r++) {
                bool currentButtonState = !digitalRead(rowPins[r]);
                if (currentButtonState != buttonState[r][c]) {
                    if ((millis() - lastDebounceTimeMatrix[r][c]) > debounceDelayNoteButtons) {
                        buttonState[r][c] = currentButtonState;

                        if (currentButtonState) {
                            digitalWrite(triggerPin, HIGH);
                            delay(5); // Pulse width, adjust as needed
                            digitalWrite(triggerPin, LOW);

                            int note = r * cols + c + octaveShift * 12;
                            float targetVoltage = applyCalibration(note);

                            if (slewRate > 0) {
                                int timePerStep = slewRate / 12;
                                while (lastVoltage != (int)(targetVoltage * (4095 / 4.93))) {
                                    if (lastVoltage < (int)(targetVoltage * (4095 / 4.93))) lastVoltage++;
                                    else if (lastVoltage > (int)(targetVoltage * (4095 / 4.93))) lastVoltage--;
                                    dac.setVoltage(lastVoltage, false);
                                    delay(timePerStep);
                                }
                            } else {
                                lastVoltage = (int)(targetVoltage * (4095 / 4.93));
                                dac.setVoltage(lastVoltage, false);
                            }
                        }
                        lastDebounceTimeMatrix[r][c] = millis();
                    }
                }
            }

            digitalWrite(colPins[c], HIGH);
        }

    // High C button logic
    if ((millis() - lastDebounceTimeNotes) > debounceDelayNoteButtons) {
        bool highCButtonState = !digitalRead(highCButton);
        if (highCButtonState && !lastHighCButtonState) {
            digitalWrite(triggerPin, HIGH);
            delay(5); // Pulse width, adjust as needed
            digitalWrite(triggerPin, LOW);
            float targetVoltage = (octaveShift == 3) ? 3.97 : applyCalibration(12 + octaveShift * 12);
    if (slewRate > 0) {
    int timePerStep = slewRate / 12;
    while (lastVoltage != (int)(targetVoltage * (4095 / 4.93))) {
    if (lastVoltage < (int)(targetVoltage * (4095 / 4.93))) lastVoltage++;
    else if (lastVoltage > (int)(targetVoltage * (4095 / 4.93))) lastVoltage--;
    dac.setVoltage(lastVoltage, false);
    delay(timePerStep);
    }
    } else {
    lastVoltage = (int)(targetVoltage * (4095 / 4.93));
    dac.setVoltage(lastVoltage, false);
    }
            lastDebounceTimeNotes = millis();
        }
        lastHighCButtonState = highCButtonState;
    }
}
}

float applyCalibration(int note) {
if (note >= 0 && note < sizeof(calibrationValues) / sizeof(calibrationValues[0])) {
return calibrationValues[note];
}
return 0; // Default to 0 if note is out of range
}
