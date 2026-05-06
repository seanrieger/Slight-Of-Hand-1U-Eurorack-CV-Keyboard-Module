#include "CalibrationMode.h"

// Define all extern variables
int buttonState[3][4];
bool lastButtonState[3][4]; 
int selectedNoteIndex;
int octaveShift;
unsigned long lastCalibrationButtonPressTime;
unsigned long downOctaveButtonPressTime;
bool inCalibrationMode;
bool calibrationModeToggleAcknowledged;
float calibrationValues[CALIBRATION_VALUES_SIZE];
float defaultCalibrationValues[CALIBRATION_VALUES_SIZE] = {
    0.000, 0.083, 0.167, 0.250, 0.333, 0.417, 0.500, 0.583, 0.667, 0.750, 0.833, 0.917,
    1.000, 1.083, 1.167, 1.250, 1.333, 1.417, 1.500, 1.583, 1.667, 1.750, 1.833, 1.917,
    2.000, 2.083, 2.167, 2.250, 2.333, 2.417, 2.500, 2.583, 2.667, 2.750, 2.833, 2.917,
    3.000, 3.083, 3.167, 3.250, 3.333, 3.417, 3.500, 3.583, 3.667, 3.750, 3.833, 3.917,
    4.000
};
bool testMode = false;

void handleCalibrationMode() {
    static unsigned long lastOctaveChangeTime = 0;
    unsigned long currentMillis = millis();

    if (!digitalRead(octaveUpPin) && !digitalRead(octaveDownPin)) {
        if (currentMillis - lastCalibrationButtonPressTime > 2000 && !calibrationModeToggleAcknowledged) {
            inCalibrationMode = !inCalibrationMode;
            calibrationModeToggleAcknowledged = true;
            digitalWrite(triggerPin, inCalibrationMode ? HIGH : LOW);
            lastCalibrationButtonPressTime = currentMillis;
        }
    } else {
        lastCalibrationButtonPressTime = currentMillis;
        calibrationModeToggleAcknowledged = false;
    }

    if (!digitalRead(octaveUpPin) && digitalRead(octaveDownPin)) {
        if (currentMillis - lastOctaveChangeTime > 200) {
            if (octaveShift < 4) octaveShift++;
            lastOctaveChangeTime = currentMillis;
        }
    }

    if (!digitalRead(octaveDownPin) && digitalRead(octaveUpPin)) {
        if (currentMillis - lastOctaveChangeTime > 200) {
            if (octaveShift > 0) octaveShift--;
            lastOctaveChangeTime = currentMillis;
        }
        if (downOctaveButtonPressTime == 0) {
            downOctaveButtonPressTime = currentMillis;
        } else if (currentMillis - downOctaveButtonPressTime > 8000) {
            memcpy(calibrationValues, defaultCalibrationValues, CALIBRATION_VALUES_SIZE * sizeof(float));
            for (int i = 0; i < CALIBRATION_VALUES_SIZE; i++) {
                if (!testMode) {
                    EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(int), (int)(defaultCalibrationValues[i] * 1000));
                }
            }
            downOctaveButtonPressTime = 0;
        }
    } else {
        downOctaveButtonPressTime = 0;
    }

    for (int c = 0; c < cols; c++) {
        digitalWrite(colPins[c], LOW);
        for (int r = 0; r < rows; r++) {
            bool currentButtonState = !digitalRead(rowPins[r]);
            if (currentButtonState && !buttonState[r][c]) {
                selectedNoteIndex = r * cols + c + octaveShift * 12;
                if (octaveShift == 4 && r == 0 && c == 0) selectedNoteIndex = 48;
                if (selectedNoteIndex >= 0 && selectedNoteIndex < CALIBRATION_VALUES_SIZE) {
                    float currentVoltage = calibrationValues[selectedNoteIndex];
                    dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
                }
            }
            buttonState[r][c] = currentButtonState;
        }
        digitalWrite(colPins[c], HIGH);
    }

    bool highCButtonState = !digitalRead(highCButton);
    if (highCButtonState && selectedNoteIndex != -1) {
        float defaultVoltage = defaultCalibrationValues[selectedNoteIndex];
        float adjustmentRange = 1.0 / 4.0;
        float potAdjustment = (analogRead(potentiometerPin) / 1023.0) * adjustmentRange - (adjustmentRange / 2.0);
        float currentVoltage = defaultVoltage + potAdjustment;
        if (selectedNoteIndex == 0) currentVoltage = max(currentVoltage, 0.0f);
        else if (selectedNoteIndex == 48) currentVoltage = min(currentVoltage, 4.0f);
        calibrationValues[selectedNoteIndex] = currentVoltage;
        dac.setVoltage((int)(currentVoltage * (4095 / 4.93)), false);
        if (!testMode) {
            EEPROM.put(EEPROM_CALIBRATION_START_ADDR + selectedNoteIndex * sizeof(int), (int)(currentVoltage * 1000));
        }
        selectedNoteIndex = -1;
    }
}
