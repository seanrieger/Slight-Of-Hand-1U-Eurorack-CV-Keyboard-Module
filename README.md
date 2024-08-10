<H1>Slight Of Hand </H1>
<P>This is a 1u (Intellijel format) Eurorack module that functions as a monophonic CV keyboard. <p>It's a play on words of the phrase "Sleight of hand" due to its "slight" footprint.</p><p>It's designed to be extremely playable with responsive, light touch, tactile switches for the keys, a wide portamento/slew range which is adjustable in real-time, as well as octave up and down buttons.</p><p>The Slight Of Hand outputs 1 volt per octave out of its CV out jack and a 5-volt gate out of the trigger jack, so it can easily be connected to an ADSR envelope.</P>
<h2>Features</H2>
  <ul>
  <li>Light touch, tactile switch keyboard that covers one octave + one note (Low C & high C)</li>
  <li>4 octaves of control from C0 to C4</li>
  <li>Portamento / Slew Control Potentiometer</li>
  <li>5v Gate Out</li>
  <li>CV Out - 1V/0</li>
  <li>Octave up and down buttons</li>
  <li> User-friendly calibration mode with tuning for each note across 4 octaves & EEPROM memory</li>
  </ul>

![IMG_1540](https://github.com/seanrieger/Slight-Of-Hand-1U-Eurorack-CV-Keyboard-Module/assets/8561196/5dabd7d8-bc2f-4699-bec2-8ccc39dbeeba)

<H2>Calibration Mode</H2>
<h3>Overview</h3>
<p>Note: All modules come pre-calibrated (every note across 4 octaves)  to the closest 100th of a volt. Most users will never need to calibrate it, but should you choose to, the following procedure is how it is done:</p>

<p>The Eurorack CV Keyboard Module features a Calibration Mode that allows users to fine-tune the voltage output for each note, ensuring precise pitch control. This mode is particularly useful for adjusting the module to align with specific musical tuning requirements or to compensate for hardware variances.</p>

<h3>Entering Calibration Mode</h3>
<P>Initiate Calibration Mode: Press and hold both the "Octave Up" and "Octave Down" buttons simultaneously whie powering on the module.
Confirmation: The module indicates entry into Calibration Mode by setting the trigger/gate output to HIGH. </P>

<H3>Calibration Process</H3>
<p>Selecting a Note: Press any note button (except High C) to select it for calibration.</p>

<em>Confirming Selection:</em> Press the High C button to confirm the selection. This action activates the potentiometer for calibration adjustment.

<em>Adjusting Calibration:</em> Turn the potentiometer to adjust the voltage for the selected note. The adjustment range is limited to one full step above and below the default voltage for finer control.

<em>Saving Calibration:</em> Release the High C button to save the adjusted voltage. The new calibration value is stored in EEPROM, ensuring persistence across power cycles.

<h3>Special Note Handling</h3>
High C (C4): Because the calibration mode uses the high C button on the keyboard as a "shift" button to confirm editing a pitch, the calibration of the final high C4 note is handled differently.  When in calibration mode, the octave buttons will allow the keyboard to shift up one additional octave, forcing C4 into the low C key for tuning.  No other pitches in that octave will be calibrated, but this allows for calibration of the final high C4.

<h3>Resetting to Default Calibration Values</h3>
Initiate Reset: While in Calibration Mode, press and hold the "Octave Down" button for 8 seconds without pressing the "Octave Up" button.

<em>Confirmation:</em> The module confirms the reset by blinking the trigger output 6 times. All notes are reset to their default calibration values.

<h3>Exiting Calibration Mode</h3>
<em></em>Exit:</em> Press and hold both the "Octave Up" and "Octave Down" buttons simultaneously for 2 seconds. The module exits Calibration Mode, indicated by the trigger output set to LOW. Now go make some music. 

<h3>Notes</h3>
<p>Calibration Mode is designed for precision tuning. It is recommended to use a reliable voltage reference or tuner for accurate calibration.</p>
<p>Be cautious while adjusting calibration values, especially for the lower and upper voltage limits.
Always confirm your adjustments before exiting Calibration Mode to ensure the
desired tuning is achieved.</p>

<h2>Developer Documentation</h2>
<p></p>I am still working on new firmwares and modules for the Nocture Alchemy Platform, so while I am releasing this as a starter template for those who would like to develop their own firmware, I am unable to provide support for those individual projects.</p 

<h3>License</h3>

### Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)

You are free to:
- Share — copy and redistribute the material in any medium or format
- Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms.

Under the following terms:
- Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- NonCommercial — You may not use the material for commercial purposes.

No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

Notices:
- You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.
- No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.

For the full legal text of the license, visit: https://creativecommons.org/licenses/by-nc/4.0/legalcode
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||





**Author:** FlatSix Modular  
**Date:** 2024  
**Version:** 1.5.3  
**License:** Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)

## Overview

This firmware is designed for the Nocturne Alchemy Platform, a 1U Eurorack keyboard module by FlatSix Modular controlled by the Arduino Nano Microcontroller. It manages the interaction between a 3x4 button matrix, a digital-to-analog converter (DAC), and various control buttons (e.g., octave shift, calibration mode). The firmware also provides voltage slewing for smooth transitions and supports a calibration mode for fine-tuning the output voltages associated with each note.

### Main Features
- **Button Matrix Handling**: Detects and processes key presses to generate corresponding voltages.
- **Octave Shifting**: Allows the user to shift the active note range up or down.
- **Voltage Slewing**: Provides smooth transitions between voltage levels.
- **Calibration Mode**: Enables the user to fine-tune the output voltages.
- **EEPROM Management**: Stores and retrieves calibration data to ensure persistence across power cycles.

## Hardware Configuration

### Arduino Nano Pin Assignments
- **Button Matrix:**
  - Rows: Pins 2, 3, 4
  - Columns: Pins 5, 6, 7, 8
- **Octave Shift Buttons:**
  - Octave Up: Pin 9
  - Octave Down: Pin 10
- **Trigger Output:**
  - Pin 11
- **High C Button:**
  - Pin 12
- **Potentiometer Input:**
  - Analog Pin A0
- **DAC (MCP4725):**
  - I2C Address: 0x60

### DAC
- **MCP4725**: A 12-bit DAC used to output precise voltages that correspond to specific notes based on the button matrix input and calibration data.

## Global Variables and Constants

- **`buttonState[rows][cols]`**: Stores the current state of each button in the matrix.
- **`lastVoltage`**: Stores the last voltage value sent to the DAC.
- **`octaveShift`**: Tracks the current octave shift, allowing the note range to be adjusted.
- **`selectedNoteIndex`**: Stores the index of the selected note in calibration mode.
- **`isSlewing`**: A flag indicating whether a voltage slew (smooth transition) is in progress.
- **`slewStartTime`**: The start time of the current slew operation.
- **`slewStepTime`**: The time between each step of the slew.
- **`totalSlewTime`**: The total time allocated for the slew operation.
- **`targetVoltageRaw`**: The target voltage (in DAC raw value) that the slew is moving towards.
- **`stepDirection`**: Indicates the direction of the voltage change (increasing or decreasing).
- **`calibrationValues[49]`**: Stores the calibration values for each note.
- **`defaultCalibrationValues[49]`**: Default calibration values for a standard Eurorack range.
- **`testMode`**: A flag to enable or disable test mode, which prevents actual EEPROM writing.

### EEPROM Addresses
- **`EEPROM_SIGNATURE_ADDR`**: The EEPROM address where the signature is stored to check if calibration data is valid.
- **`EEPROM_CALIBRATION_START_ADDR`**: The starting address for calibration data in EEPROM.
- **`EEPROM_SIGNATURE_VALUE`**: A predefined value used to verify the integrity of the EEPROM data.
- **`EEPROM_INTEGER_FLAG_ADDR`**: The EEPROM address for storing a flag indicating whether calibration values are stored as integers.
- **`EEPROM_INTEGER_FLAG_VALUE`**: The value that indicates calibration values are stored as integers.

## Function Descriptions

### `void setup()`
The `setup()` function initializes the hardware components (I2C, DAC, Serial, pins) and manages the retrieval and validation of calibration data from EEPROM. It also checks if the module should enter calibration mode based on the state of the octave shift buttons during startup.

- **EEPROM Handling**: The EEPROM is checked for a valid signature. If the signature is not found, the EEPROM is initialized with default calibration values. If a legacy float format is detected, it is converted to an integer format.
- **Calibration Mode Check**: If both octave shift buttons are pressed during power-up, the module enters calibration mode.

### `void loop()`
The `loop()` function is the main execution loop of the firmware. It continuously updates the voltage slewing, handles button inputs, manages octave shifting, and controls the transition between normal operation and calibration mode.

- **Voltage Slewing**: The `updateSlew()` function is called regularly to ensure any ongoing voltage slew is processed.
- **Octave Shifting**: The state of the octave up and down buttons is checked, with debounce handling to prevent accidental double presses.
- **Calibration Mode**: If calibration mode is active, the firmware processes button presses to adjust calibration values and store them in EEPROM. The High C button serves as the "enter" key for setting a new calibration value.

### `void startSlew(float targetVoltage, unsigned long totalSlewTime)`
This function initiates a non-blocking voltage slew from the current DAC output to a target voltage over a specified duration.

- **Target Voltage Calculation**: Converts the target voltage to the DAC's raw format.
- **Slew Control**: Calculates the time per step and sets the direction of voltage change.

### `void updateSlew()`
This function updates the current slew operation by adjusting the DAC output incrementally until the target voltage is reached.

- **Timing Management**: Uses `micros()` for precise timing of each step.
- **Completion Check**: Ends the slew operation once the target voltage is achieved.

### `void blinkTriggerPulse(int count, int duration)`
This utility function blinks the trigger output pin a specified number of times with a given duration.

- **Visual Feedback**: Typically used to indicate a reset or special event (e.g., calibration reset).

### `void printEEPROMUsage()`
Prints information about EEPROM usage to the Serial Monitor. Useful for debugging and ensuring calibration data is correctly stored.

### `float applyCalibration(int note)`
Retrieves the calibrated voltage for a specified note index. Returns the corresponding value from `calibrationValues`.

## Special Modes and Controls

### Calibration Mode
Calibration mode allows the user to fine-tune the output voltage for each note. It is entered by holding both octave buttons during power-up. In this mode:
- **Note Selection**: Each button in the matrix corresponds to a note. Pressing a button outputs the current voltage for that note.
- **Voltage Adjustment**: The potentiometer is used to adjust the voltage. The High C button confirms the adjustment, and the new value is stored in EEPROM.

### EEPROM Management
The EEPROM is used to store calibration data so that it persists across power cycles. The data structure includes a signature to verify the integrity of the data, and a flag to distinguish between float and integer formats.

## Normal Operation
In normal operation, the firmware reads the button matrix to determine which notes are pressed. It then applies the corresponding calibration value and outputs the appropriate voltage via the DAC. The octave shift buttons allow the user to adjust the note range, and the potentiometer controls the slew rate, providing smooth transitions between voltages.

## Summary
This firmware provides a robust and flexible solution for managing note inputs, octave shifts, and calibration on the Nocturne Alchemy Platform. It is designed to be easily understandable and maintainable, with clear separation of concerns and detailed inline comments to guide further development.




Check the Wiki for More Info: https://github.com/seanrieger/Slight-Of-Hand-1U-Eurorack-CV-Keyboard-Module/wiki
