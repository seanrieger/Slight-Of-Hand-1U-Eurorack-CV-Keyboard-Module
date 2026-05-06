#ifndef EEPROMHANDLING_H
#define EEPROMHANDLING_H

#include <Arduino.h>
#include <EEPROM.h>

const int EEPROM_SIGNATURE_ADDR = 0;
const int EEPROM_CALIBRATION_START_ADDR = 4;
const int EEPROM_SIGNATURE_VALUE = 12345;
const int EEPROM_INTEGER_FLAG_ADDR = EEPROM_CALIBRATION_START_ADDR + 49 * sizeof(float);
const int EEPROM_INTEGER_FLAG_VALUE = 67890;

extern bool testMode;
extern float calibrationValues[49];
extern float defaultCalibrationValues[49];

void handleEEPROM();
void printEEPROMUsage();

#endif // EEPROMHANDLING_H
