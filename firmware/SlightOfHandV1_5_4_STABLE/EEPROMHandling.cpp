#include "EEPROMHandling.h"

void handleEEPROM() {
    // Critical code for EEPROM Signature
    int eepromSignature;
    EEPROM.get(EEPROM_SIGNATURE_ADDR, eepromSignature);

    if (eepromSignature != EEPROM_SIGNATURE_VALUE) {
        //Serial.println("EEPROM signature not found. Initializing with default values.");
        if (!testMode) {
            EEPROM.put(EEPROM_SIGNATURE_ADDR, EEPROM_SIGNATURE_VALUE);
            for (int i = 0; i < 49; i++) {
                EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), defaultCalibrationValues[i]);
            }
            EEPROM.put(EEPROM_INTEGER_FLAG_ADDR, 0); // Set the integer flag to 0 (float format)
        } else {
            //Serial.println("Test mode: EEPROM not written.");
        }
    }

    int integerFlag;
    EEPROM.get(EEPROM_INTEGER_FLAG_ADDR, integerFlag);

    if (integerFlag != EEPROM_INTEGER_FLAG_VALUE) {
        //Serial.println("Float format detected. Converting to integer format.");
        for (int i = 0; i < 49; i++) {
            EEPROM.get(EEPROM_CALIBRATION_START_ADDR + i * sizeof(float), calibrationValues[i]);
            int scaledValue = (int)(calibrationValues[i] * 1000);
            if (!testMode) {
                EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(int), scaledValue);
            } else {
                //Serial.print("Test mode: Would convert float ");
                //Serial.print(calibrationValues[i], 4);
                ///Serial.print(" to int ");
                //Serial.println(scaledValue);
            }
        }
        if (!testMode) {
            EEPROM.put(EEPROM_INTEGER_FLAG_ADDR, EEPROM_INTEGER_FLAG_VALUE);
        } else {
            //Serial.println("Test mode: Integer flag not set.");
        }
    }

    for (int i = 0; i < 49; i++) {
        int scaledValue;
        EEPROM.get(EEPROM_CALIBRATION_START_ADDR + i * sizeof(int), scaledValue);
        calibrationValues[i] = scaledValue / 1000.0;
    }

    printEEPROMUsage();
}

void printEEPROMUsage() {
    int startAddress = EEPROM_CALIBRATION_START_ADDR;
    int endAddress = EEPROM_CALIBRATION_START_ADDR + 49 * sizeof(int) - 1;
    int bytesUsed = (endAddress - startAddress + 1) + 1; // Include flag byte

    //Serial.println();
    //Serial.print("EEPROM Usage Information:");
    //Serial.print("\nTotal bytes used: ");
    //Serial.println(bytesUsed);
    //Serial.print("Data stored between addresses: ");
    //Serial.print(startAddress);
    //Serial.print(" and ");
    //Serial.println(endAddress);
    //Serial.println("Firmware: Seventh Summoner v1.0.0");
}