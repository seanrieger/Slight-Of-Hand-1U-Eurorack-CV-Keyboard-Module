# Slight Of Hand v1.5.4 - Quick Reference Guide

## 🎹 One-Page User Guide

---

## Normal Operation

### Playing Notes

- **12 keyboard buttons:** C, C#, D, D#, E, F, F#, G, G#, A, A#, B
- **High C button:** Plays C in current octave (C1-C4)
- **Octave Up/Down:** Shift note range (0-3 octaves, 49 total notes)
- **Gate output:** 5V trigger when any key pressed

### Portamento/Slew Control

**Potentiometer Position → Slew Time:**

```
7 o'clock        → INSTANT (no slew)
8-9 o'clock      → 30-70ms (very fast, subtle character)
10-11 o'clock    → 100-200ms (quick glide)
12 o'clock       → ~400ms (expressive portamento)
1 o'clock        → 500ms (transition point)
2-3 o'clock      → 800-1200ms (dramatic sweep)
4-5 o'clock      → 1500-2000ms (ultra-slow glide)
```

**Musical Zones:**

- **Dead zone (7 o'clock):** Instant notes, no slew
- **Fast zone (7→1 o'clock, 70% of pot):** Everyday use, 30-500ms
- **Slow zone (1→5 o'clock, 30% of pot):** Dramatic effects, 550-2000ms

---

## Calibration Mode

### Enter Calibration

1. Power off module
2. Hold **Octave Up + Octave Down**
3. Power on (gate goes HIGH to confirm)

### Check Calibration Values

Without holding High C:

1. Press keyboard button to see voltage for that note
2. Use octave buttons to navigate octaves (0-4 in calibration mode)
3. Example: Octave 0, press C → 0.000V | Octave 1, press C → 1.000V

### Adjust Calibration

With High C held:

1. Press keyboard button to select note
2. Hold **High C** button
3. Turn pot to adjust voltage (±0.125V range)
4. Release **High C** to save to EEPROM
5. Repeat for all 49 notes

### Calibrate High C (C4)

- Use octave buttons to shift to octave 4
- Low C button now represents C4 for calibration

### Exit Calibration

Hold **Octave Up + Octave Down** for 2 seconds

### Reset to Defaults

In calibration mode:

- Hold **Octave Down** for 8 seconds (gate blinks 6×)

---

## Specifications

| Feature      | Spec                        |
| ------------ | --------------------------- |
| **Format**   | 1U × 26HP × 40mm            |
| **CV Range** | 0-4V (1V/octave)            |
| **Gate**     | 5V trigger                  |
| **Power**    | 40mA +12V                   |
| **Notes**    | 49 (C0 to C4)               |
| **Slew**     | 0ms, 30-2000ms              |
| **Octaves**  | 4 (normal), 5 (calibration) |

---

## Firmware Upload

### Web Loader (Easiest)

1. Go to: **flatsixmodular.com/firmware**
2. Unplug from rack power
3. Connect USB cable to module
4. Click upload button (**check N or O bootloader**)
5. Select USB Serial port
6. Wait for "Done!"
7. Disconnect USB, reconnect power

### Arduino IDE

1. Open `.ino` file in Arduino IDE
2. Tools > Board: **Arduino Nano**
3. Tools > Processor: **ATmega328P** (check bootloader)
4. Sketch > Upload
5. Wait for completion

---

## Troubleshooting

| Problem                         | Solution                                              |
| ------------------------------- | ----------------------------------------------------- |
| **No CV output**                | Check calibration, verify I2C DAC connection          |
| **No gate**                     | Check wiring, verify trigger pin connection           |
| **Jumpy slew**                  | Pot in dead zone? Turn slightly clockwise             |
| **Can't calibrate**             | Both octave buttons held on power-up?                 |
| **Wrong octave in calibration** | Update to v1.5.4 (fixes octave bugs)                  |
| **Upload fails**                | Check bootloader type (N vs O), verify USB connection |
| **Erratic voltages**            | Re-calibrate or reset to defaults                     |

---

## Tips & Tricks

### Musical Applications

**Fast Zone (30-500ms):**

- Smooth legato melodies
- Bass lines with character
- Quick harmonic transitions
- Rhythmic portamento effects

**Slow Zone (550-2000ms):**

- Cinematic pitch sweeps
- Theremin-style playing
- Ambient soundscapes
- Dramatic special effects

### Getting Started

1. **Start with pot at 7 o'clock** (instant)
2. **Slowly turn right** to add portamento
3. **Find your sweet spot** (usually 10-12 o'clock for melodic playing)
4. **Explore slow zone** (1-5 o'clock) for creative effects

### Calibration Best Practices

- Use a **reliable multimeter** for accuracy
- Start at **C0**, work through all 49 notes systematically
- **Take your time** (20-30 minutes for full calibration)
- **Test octave jumps** when done to verify accuracy
- **Write down values** if you want to replicate calibration

---

## Patch Ideas

### Lead Synth Voice

- Slight Of Hand → VCO (V/Oct input)
- Gate out → Envelope Generator → VCA
- Set slew to 10-12 o'clock for expressive glide
- Use octave buttons for range changes

### Bass Line Controller

- Slight Of Hand → Bass VCO (V/Oct)
- Gate out → Filter Envelope
- Set slew to 8-9 o'clock for funky slides
- Stay in octave 0-1 for sub/bass range

### Theremin-Style Performance

- Slight Of Hand → Oscillator
- Set slew to 3-5 o'clock (slow)
- Play single-finger melodies with dramatic glides
- Use High C button for octave jumps

### Quantized Random

- Slight Of Hand → Sample & Hold Clock
- Random CV → Sample & Hold Input
- Slight Of Hand CV → S&H CV Out → VCO
- Creates quantized random melodies in your chosen scale

---

## Common Modifications

### Hardware Mods

- Add CV input for slew control (requires hardware mod)
- Add LED indicators for octave position
- Add expression pedal input
- MIDI output (requires additional hardware)

### Firmware Mods

- Alternative tuning systems (microtonal, just intonation)
- Arpeggiator mode
- Sequence recorder
- Alternative slew curves

**See CONTRIBUTING.md for development guidelines**

---

## Default Calibration Values

For reference, the factory default 1V/octave calibration:

```
C0:  0.000V    C1:  1.000V    C2:  2.000V    C3:  3.000V    C4:  4.000V
C#0: 0.083V    C#1: 1.083V    C#2: 2.083V    C#3: 3.083V
D0:  0.167V    D1:  1.167V    D2:  2.167V    D3:  3.167V
D#0: 0.250V    D#1: 1.250V    D#2: 2.250V    D#3: 3.250V
E0:  0.333V    E1:  1.333V    E2:  2.333V    E3:  3.333V
F0:  0.417V    F1:  1.417V    F2:  2.417V    F3:  3.417V
F#0: 0.500V    F#1: 1.500V    F#2: 2.500V    F#3: 3.500V
G0:  0.583V    G1:  1.583V    G2:  2.583V    G3:  3.583V
G#0: 0.667V    G#1: 1.667V    G#2: 2.667V    G#3: 3.667V
A0:  0.750V    A1:  1.750V    A2:  2.750V    A3:  3.750V
A#0: 0.833V    A#1: 1.833V    A#2: 2.833V    A#3: 3.833V
B0:  0.917V    B1:  1.917V    B2:  2.917V    B3:  3.917V
```

**Each semitone = 0.0833V (1/12 of 1V)**

---

## What's New in v1.5.4

85% more efficient** (1 slew update/loop vs 7)  
Musical pot mapping** (70% fast, 30% slow)  
**No imperceptible ranges** (all useful)  
Exponential fast zone** (better control)  
Smooth everywhere** (no glitches)  
Fixed calibration bugs** (octave reset, double-increment)  
**Same max slew** (2 seconds preserved)  

---

## Support & Resources

**Website:** flatsixmodular.com  
**Firmware:** flatsixmodular.com/firmware  
**Instagram:** @flatsix.modular  
**Manual PDF:** Available at flatsixmodular.com

**Open Source:**

- GitHub: github.com/flatsixmodular/slight-of-hand
- Documentation: Full docs in repository
- Issues: Report bugs via GitHub
- Discussions: Share patches and ideas

---

## Nocturne Alchemy Platform

Slight Of Hand is part of the **Nocturne Alchemy Platform** - a series of 1U Eurorack modules sharing the same hardware but with different firmware:

- **Slight Of Hand** - CV Keyboard (this module)
- **Arp Of Darkness** - Arpeggiator with multiple playback modes
- **Seventh Summoner** - Multi-sequence step sequencer
- **Shroud Of Turing** - Turing machine / generative sequencer

**All firmwares are free and swappable!**  
Visit flatsixmodular.com/firmware to try different modules.

---

**Version:** 1.5.4 | **Date:** 2025-01-19 | **Status:** Stable

🎹 **Happy playing!** 🎹
