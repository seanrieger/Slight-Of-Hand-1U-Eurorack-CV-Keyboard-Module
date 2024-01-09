#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;
const int rows = 3;
const int cols = 4;
byte rowPins[rows] = {2, 3, 4}; // Row pins connected to buttons
byte colPins[cols] = {5, 6, 7, 8}; // Column pins connected to buttons

const int octaveUpPin = 9; // Pin for "Octave Up" button
const int octaveDownPin = 10; // Pin for "Octave Down" button
const int triggerPin = 11; // Pin for trigger output
const int highCButton = 12; // Pin for the high C button
const int potentiometerPin = A0; // Potentiometer connected to A0

int buttonState[rows][cols]; // Array to hold the state of each button
int lastVoltage = 0; // Variable to store the last voltage
int octaveShift = 0; // Variable to track octave shifts
int voltageStepPerNote = (int)(4095 * (4.0 / 4.93) / 48); // Voltage step per note for 4 octaves

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers
unsigned long lastDebounceTimeMatrix[rows][cols]; // Array to store the last debounce time for each matrix button

bool lastHighCButtonState = false; // Variable to track the last state of the high C button

// Calibration array with your measured values
float calibrationValues[] = {
    // ... include all your measured values here ...
    0.001, 0.083, 0.167, 0.250, 0.335, 0.418, 0.500, 0.584, 0.668, 0.751, 0.834, 0.916, // C0 to B0
    0.998, 1.080, 1.163, 1.246, 1.329, 1.412, 1.494, 1.576, 1.668, 1.742, 1.826, 1.909, // C1 to B1
    1.993, 2.076, 2.159, 2.243, 2.327, 2.409, 2.492, 2.575, 2.660, 2.743, 2.826, 2.909, // C2 to B2
    2.992, 3.077, 3.158, 3.241, 3.325, 3.407, 3.489, 3.572, 3.656, 3.741, 3.824, 3.907  // C3 to B3
};

float applyCalibration(int note) {
    if (note >= 0 && note < sizeof(calibrationValues) / sizeof(calibrationValues[0])) {
        return calibrationValues[note];
    }
    return 0; // Default to 0 if note is out of range
}

void setup() {
    Wire.begin();
    dac.begin(0x60); // Initialize the DAC

    // Initialize row and column pins
    for (int r = 0; r < rows; r++) {
        pinMode(rowPins[r], INPUT_PULLUP);
        for (int c = 0; c < cols; c++) {
            pinMode(colPins[c], OUTPUT);
            lastDebounceTimeMatrix[r][c] = 0; // Initialize debounce time for each matrix button
        }
    }

    // Initialize other buttons and trigger pin
    pinMode(octaveUpPin, INPUT_PULLUP);
    pinMode(octaveDownPin, INPUT_PULLUP);
    pinMode(triggerPin, OUTPUT);
    pinMode(highCButton, INPUT_PULLUP);
    pinMode(potentiometerPin, INPUT); // Initialize potentiometer pin

    digitalWrite(triggerPin, LOW); // Ensure trigger pin starts LOW
}
void loop() {
    // Check for octave shifts with debounce
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (!digitalRead(octaveUpPin)) {
            octaveShift = min(octaveShift + 1, 3); // Limit to 3 for the highest octave (C3)
            lastDebounceTime = millis();
        }
        if (!digitalRead(octaveDownPin)) {
            octaveShift = max(octaveShift - 1, 0); // Limit to 0 for the lowest octave (C0)
            lastDebounceTime = millis();
        }
    }

    // Read the potentiometer value and set the slew rate
    int potValue = analogRead(potentiometerPin);
    int slewRate = (potValue < 5) ? 0 : map(potValue, 5, 1023, 1, 30);

    // Scan each column of the button matrix
    for (int c = 0; c < cols; c++) {
        digitalWrite(colPins[c], LOW);

        for (int r = 0; r < rows; r++) {
            bool currentButtonState = !digitalRead(rowPins[r]);
            if (currentButtonState != buttonState[r][c]) {
                if ((millis() - lastDebounceTimeMatrix[r][c]) > debounceDelay) {
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

    // High C button logic with debounce
    if ((millis() - lastDebounceTime) > debounceDelay) {
        bool highCButtonState = !digitalRead(highCButton);
        if (highCButtonState && !lastHighCButtonState) {
            digitalWrite(triggerPin, HIGH);
            delay(5); // Pulse width, adjust as needed
            digitalWrite(triggerPin, LOW);

            float targetVoltage;
            if (octaveShift == 3) {
                targetVoltage = 4.0; // C4 (4 volts) in the top octave
            } else {
                int note = 12 + octaveShift * 12; // High C note for the current octave
                targetVoltage = applyCalibration(note);
            }

            // Implementing the slew for high C
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

            lastDebounceTime = millis();
        }
        lastHighCButtonState = highCButtonState;
    }
}
