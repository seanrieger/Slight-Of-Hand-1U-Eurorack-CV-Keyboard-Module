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
int voltageStepPerNote = (4095 / 5) / 12; // Voltage step per note

bool lastHighCButtonState = false; // Variable to store the last state of the high C button
const unsigned long debounceDelay = 200; // debounce time in milliseconds
unsigned long lastDebounceTimeMatrix[rows][cols];
unsigned long lastDebounceTimeOctaveUp = 0;
unsigned long lastDebounceTimeOctaveDown = 0;
unsigned long lastDebounceTimeHighC = 0;

void setup() {
    Wire.begin();
    dac.begin(0x60); // Initialize the DAC

    // Initialize row and column pins
    for (int r = 0; r < rows; r++) {
        pinMode(rowPins[r], INPUT_PULLUP);
        for (int c = 0; c < cols; c++) {
            pinMode(colPins[c], OUTPUT);
            lastDebounceTimeMatrix[r][c] = 0;
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
    unsigned long currentMillis = millis();

    // Debounce for octaveUpPin
    bool readingOctaveUp = !digitalRead(octaveUpPin);
    if (readingOctaveUp && (currentMillis - lastDebounceTimeOctaveUp) > debounceDelay) {
        octaveShift = min(octaveShift + 1, 4);
        lastDebounceTimeOctaveUp = currentMillis;
    }

    // Debounce for octaveDownPin
    bool readingOctaveDown = !digitalRead(octaveDownPin);
    if (readingOctaveDown && (currentMillis - lastDebounceTimeOctaveDown) > debounceDelay) {
        octaveShift = max(octaveShift - 1, -4);
        lastDebounceTimeOctaveDown = currentMillis;
    }

    // Read the potentiometer value and set the slew rate
    int potValue = analogRead(potentiometerPin);
    int slewRate = (potValue < 5) ? 0 : map(potValue, 5, 1023, 2, 80);

    // Scan each column of the button matrix
    for (int c = 0; c < cols; c++) {
        digitalWrite(colPins[c], LOW);

        for (int r = 0; r < rows; r++) {
            bool currentButtonState = !digitalRead(rowPins[r]);
            if (currentButtonState && !buttonState[r][c] && (currentMillis - lastDebounceTimeMatrix[r][c]) > debounceDelay) {
                // Button logic here...
                digitalWrite(triggerPin, HIGH);
                delay(5); // Pulse width, adjust as needed
                digitalWrite(triggerPin, LOW);

                int note = r * cols + c;
                int targetVoltage = note * voltageStepPerNote;
                targetVoltage += (4095 / 5) * octaveShift;
                targetVoltage = constrain(targetVoltage, 0, 4095);

                // Implementing the slew
                if (slewRate > 0) {
                    int timePerStep = slewRate / 12;
                    while (lastVoltage != targetVoltage) {
                        if (lastVoltage < targetVoltage) lastVoltage++;
                        else if (lastVoltage > targetVoltage) lastVoltage--;
                        dac.setVoltage(lastVoltage, false);
                        delay(timePerStep); // Delay based on calculated time per step
                    }
                } else {
                    lastVoltage = targetVoltage;
                    dac.setVoltage(lastVoltage, false);
                }

                lastDebounceTimeMatrix[r][c] = currentMillis;
            }
            buttonState[r][c] = currentButtonState;
        }

        digitalWrite(colPins[c], HIGH);
    }

    // Debounce logic for highCButton
    bool highCButtonState = !digitalRead(highCButton);
    if (highCButtonState && !lastHighCButtonState && (currentMillis - lastDebounceTimeHighC) > debounceDelay) {
        // High C button logic here...
        digitalWrite(triggerPin, HIGH);
        delay(5); // Pulse width, adjust as needed
        digitalWrite(triggerPin, LOW);

        int note = 12; // 13th note (high C)
        int targetVoltage = note * voltageStepPerNote;
        targetVoltage += (4095 / 5) * octaveShift;
        targetVoltage = constrain(targetVoltage, 0, 4095);

        // Implementing the slew for high C
        if (slewRate > 0) {
            int timePerStep = slewRate / 12;
            while (lastVoltage != targetVoltage) {
                if (lastVoltage < targetVoltage) lastVoltage++;
                else if (lastVoltage > targetVoltage) lastVoltage--;
                dac.setVoltage(lastVoltage, false);
                delay(timePerStep);
            }
        } else {
            lastVoltage = targetVoltage;
            dac.setVoltage(lastVoltage, false);
        }

        lastDebounceTimeHighC = currentMillis;
    }
    lastHighCButtonState = highCButtonState;
}
