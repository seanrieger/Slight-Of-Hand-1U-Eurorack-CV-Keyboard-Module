#ifndef EEPROMHANDLING_H
#define EEPROMHANDLING_H

#include <Arduino.h>
#include <EEPROM.h>

// Define the constants related to EEPROM handling
const int EEPROM_SIGNATURE_ADDR = 0;
const int EEPROM_CALIBRATION_START_ADDR = 4;
const int EEPROM_SIGNATURE_VALUE = 12345;
const int EEPROM_INTEGER_FLAG_ADDR = EEPROM_CALIBRATION_START_ADDR + 49 * sizeof(float); // Address for integer flag
const int EEPROM_INTEGER_FLAG_VALUE = 67890;

// Declare the external variables
extern bool testMode;
extern float calibrationValues[49];       // EEPROM calibration values array
extern float defaultCalibrationValues[49]; // Default calibration values array

// Function declarations
void handleEEPROM();        // Function to handle EEPROM operations
void printEEPROMUsage();    // Function to print EEPROM usage

#endif // EEPROMHANDLING_H
