#ifndef CALIBRATIONMODE_H
#define CALIBRATIONMODE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_MCP4725.h>
#include "EEPROMHandling.h"

#define CALIBRATION_VALUES_SIZE 49

extern const int rows;
extern const int cols;
extern byte rowPins[];
extern byte colPins[];
extern const int octaveUpPin;
extern const int octaveDownPin;
extern const int triggerPin;
extern const int highCButton;
extern const int potentiometerPin;

extern int buttonState[3][4];
extern bool lastButtonState[3][4];
extern int selectedNoteIndex;
extern int octaveShift;
extern unsigned long lastCalibrationButtonPressTime;
extern unsigned long downOctaveButtonPressTime;
extern bool inCalibrationMode;
extern bool calibrationModeToggleAcknowledged;
extern float calibrationValues[CALIBRATION_VALUES_SIZE];
extern float defaultCalibrationValues[CALIBRATION_VALUES_SIZE];
extern bool testMode;

extern Adafruit_MCP4725 dac;

void handleCalibrationMode();

#endif // CALIBRATIONMODE_H
