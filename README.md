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
<P>Initiate Calibration Mode: Press and hold both the "Octave Up" and "Octave Down" buttons simultaneously for 2 seconds.
Confirmation: The module indicates entry into Calibration Mode by setting the trigger output to HIGH. </P>

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

Check the Wiki for More Info: https://github.com/seanrieger/Slight-Of-Hand-1U-Eurorack-CV-Keyboard-Module/wiki
