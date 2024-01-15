<H1>Slight Of Hand</H1>
<P>This is a 1u (Intellijel format) Eurorack module that functions as a CV keyboard. It's a play of words of the phrase "Sleight of hand" due to its small size.  More to come here soon.</P>
<h2>Features</H2>
  <ul>
  <li>4 octaves of control from C0 to C4</li>
  <li>Portamento / Slew Control Potentiometer</li>
  <li>Trigger Out</li>
  <li>CV Out - 1V/0</li>
  <li>Octave up and down button</li>
  </ul>


<H2>Calibration Mode</H2>
<h3>Overview</h3>
<p>The Eurorack CV Keyboard Module features a Calibration Mode that allows users to fine-tune the voltage output for each note, ensuring precise pitch control. This mode is particularly useful for adjusting the module to align with specific musical tuning requirements or to compensate for hardware variances.</p>

<h3>Entering Calibration Mode</h3>
<P>Initiate Calibration Mode: Press and hold both the "Octave Up" and "Octave Down" buttons simultaneously for 2 seconds.
Confirmation: The module indicates entry into Calibration Mode by setting the trigger output to HIGH. A message "Entering Calibration Mode" will also be displayed on the serial monitor.</P>

<H3>Calibration Process</H3>
<p>Selecting a Note: Press any note button (except High C) to select it for calibration. The selected note index will be displayed on the serial monitor with a prompt to press High C to confirm and adjust.</p>

<em>Confirming Selection:</em> Press the High C button to confirm the selection. This action activates the potentiometer for calibration adjustment.

Adjusting Calibration: Turn the potentiometer to adjust the voltage for the selected note. The adjustment range is limited to one half-step above and below the default voltage for finer control. The current adjusted voltage will be displayed on the serial monitor.

Saving Calibration: Release the High C button to save the adjusted voltage. The new calibration value is stored in EEPROM, ensuring persistence across power cycles.

Special Note Handling
High C (C4): In the highest octave (Octave 3), High C is set to a fixed voltage (e.g., 3.95V). In other octaves, its voltage is based on the calibration values.

Resetting to Default Calibration Values
Initiate Reset: While in Calibration Mode, press and hold the "Octave Down" button for 8 seconds without pressing the "Octave Up" button.

Confirmation: The module confirms the reset by blinking the trigger output 6 times. A message "Calibration values reset to defaults" will be displayed on the serial monitor. All notes are reset to their default calibration values.

Exiting Calibration Mode
Exit: Press and hold both the "Octave Up" and "Octave Down" buttons simultaneously for 2 seconds. The module exits Calibration Mode, indicated by the trigger output set to LOW and a message "Exiting Calibration Mode" displayed on the serial monitor.
Notes
Calibration Mode is designed for precision tuning. It is recommended to use a reliable voltage reference or tuner for accurate calibration.
Be cautious while adjusting calibration values, especially for the lower and upper voltage limits.
Always confirm your adjustments before exiting Calibration Mode to ensure the
desired tuning is achieved.
