#ifndef CALIBRATIONMODE_H
#define CALIBRATIONMODE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_MCP4725.h>
#include "EEPROMHandling.h"  // Include EEPROMHandling to access EEPROM constants

// Define the size of calibrationValues array
#define CALIBRATION_VALUES_SIZE 49

// Declare the variables as extern
extern const int rows;
extern const int cols;
extern byte rowPins[];  // Extern declaration for row pins
extern byte colPins[];  // Extern declaration for column pins
extern const int octaveUpPin;
extern const int octaveDownPin;
extern const int triggerPin;
extern const int highCButton;
extern const int potentiometerPin;

extern int buttonState[3][4];                  // Button state matrix
extern bool lastButtonState[3][4];             // Last button state matrix
extern int selectedNoteIndex;                  // Selected note for calibration
extern int octaveShift;                        // Octave shift for calibration
extern unsigned long lastCalibrationButtonPressTime;  // Last time a calibration button was pressed
extern unsigned long downOctaveButtonPressTime;       // Last time octave down button was pressed
extern bool inCalibrationMode;                 // Flag to indicate if in calibration mode
extern bool calibrationModeToggleAcknowledged; // Flag to acknowledge calibration mode toggle
extern float calibrationValues[CALIBRATION_VALUES_SIZE]; // Calibration values
extern float defaultCalibrationValues[CALIBRATION_VALUES_SIZE]; // Default calibration values
extern bool testMode;

extern Adafruit_MCP4725 dac;  // Declare dac as extern

// Function prototype for handling calibration mode
void handleCalibrationMode();

#endif // CALIBRATIONMODE_H
