# Contributing to Slight Of Hand

First off, thank you for considering contributing to Slight Of Hand! It's people like you that make the open-source modular synthesis community such a great place.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [How Can I Contribute?](#how-can-i-contribute)
4. [Development Process](#development-process)
5. [Style Guidelines](#style-guidelines)
6. [Commit Guidelines](#commit-guidelines)
7. [Pull Request Process](#pull-request-process)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inspiring community for all. Please be respectful and constructive in your communications.

### Our Standards

**Positive behaviors:**

- Using welcoming and inclusive language
- Being respectful of differing viewpoints
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

**Unacceptable behaviors:**

- Trolling, insulting/derogatory comments, and personal attacks
- Public or private harassment
- Publishing others' private information without permission
- Other conduct which could reasonably be considered inappropriate

---

## Getting Started

### Prerequisites

- Arduino IDE 1.8.x or 2.x
- Basic understanding of C/C++
- Familiarity with Arduino development
- Git for version control

### Setting Up Development Environment

1. **Clone the repository:**
   
   ```bash
   git clone https://github.com/flatsixmodular/slight-of-hand.git
   cd slight-of-hand
   ```

2. **Install required libraries:**
   
   - Open Arduino IDE
   - Go to Sketch > Include Library > Manage Libraries
   - Search for "Adafruit MCP4725"
   - Click Install

3. **Open the project:**
   
   - File > Open
   - Navigate to `SlightOfHand_v1-5-4-Stable.ino`

4. **Compile to verify setup:**
   
   - Sketch > Verify/Compile
   - Should compile with 0 errors

---

## How Can I Contribute?

### Reporting Bugs

**Before submitting a bug report:**

- Check the documentation to see if the behavior is expected
- Search existing issues to see if it's already reported
- Test with the latest firmware version

**Submitting a bug report:**

Use the issue tracker and include:

- **Clear title** describing the problem
- **Steps to reproduce** the behavior
- **Expected behavior** vs. actual behavior
- **Firmware version** you're using
- **Hardware details** (Arduino Nano type, power supply, etc.)
- **Any modifications** you've made to the code

**Example:**

```
Title: Calibration mode octave buttons skip values

Description:
When in calibration mode, pressing the octave up button once 
sometimes increments the octave by 2 instead of 1.

Steps to reproduce:
1. Enter calibration mode (hold both octave buttons on power-up)
2. Press octave up button once
3. Press Low C button
4. Observe voltage output

Expected: Should show 1.000V (C1)
Actual: Shows 2.000V (C2)

Version: v1.5.3
Hardware: Arduino Nano (old bootloader)
Modifications: None
```

### Suggesting Enhancements

**Before suggesting an enhancement:**

- Check if it's already in the roadmap (see CHANGELOG.md)
- Search existing issues for similar suggestions

**Submitting an enhancement suggestion:**

Use the issue tracker with the tag `enhancement` and include:

- **Clear title** describing the feature
- **Detailed description** of the proposed functionality
- **Use cases** - why is this useful?
- **Implementation ideas** (optional)
- **Backward compatibility** - will this break existing functionality?

**Example:**

```
Title: Add MIDI output support

Description:
Add MIDI note output alongside CV output, allowing Slight Of Hand 
to control MIDI synthesizers.

Use cases:
- Control both CV and MIDI gear from one keyboard
- Record performances via MIDI
- Use with DAW integration

Implementation ideas:
- Use Arduino MIDI library
- MIDI out on unused pin
- Configuration mode to enable/disable

Compatibility: Should not affect CV operation when disabled
```

### Contributing Code

See [Development Process](#development-process) below for detailed guidelines.

---

## Development Process

### 1. Pick an Issue or Create One

- Browse the [issue tracker](https://github.com/flatsixmodular/slight-of-hand/issues)
- Comment on the issue to let others know you're working on it
- Get feedback on your approach before starting major work

### 2. Create a Branch

```bash
# For new features
git checkout -b feature/descriptive-name

# For bug fixes
git checkout -b bugfix/issue-number-description

# For documentation
git checkout -b docs/what-you-are-documenting
```

### 3. Make Your Changes

**Important rules:**

- ✅ Only modify `SlightOfHand_v1-5-4-Stable.ino` (the main file)
- ❌ Do NOT modify `CalibrationMode.cpp` or `EEPROMHandling.cpp` (shared files)
- ✅ Add comments explaining non-obvious logic
- ✅ Test thoroughly before committing

**Shared files are shared across all Nocturne Alchemy modules!**  
Changes to shared files affect Arp Of Darkness, Seventh Summoner, etc.

### 4. Test Your Changes

Run through the complete [testing checklist](DEVELOPER.md#testing-procedures):

- [ ] Code compiles with 0 errors and 0 warnings
- [ ] All 12 keyboard buttons work
- [ ] Octave up/down buttons work
- [ ] High C button works
- [ ] Potentiometer controls slew
- [ ] Gate output works correctly
- [ ] Calibration mode entry works
- [ ] Calibration mode octave buttons work
- [ ] Calibration adjustment works
- [ ] Calibration values persist after power cycle
- [ ] Normal operation works after exiting calibration

### 5. Document Your Changes

- Update comments in the code
- Update CHANGELOG.md if appropriate
- Update README.md if user-facing changes
- Add/update examples if new features

### 6. Commit Your Changes

See [Commit Guidelines](#commit-guidelines) below.

### 7. Push and Create Pull Request

```bash
git push origin your-branch-name
```

Then create a pull request on GitHub. See [Pull Request Process](#pull-request-process).

---

## Style Guidelines

### Code Style

**Follow existing conventions:**

- 4 spaces for indentation (no tabs)
- Opening braces on same line
- Descriptive variable names
- Comments for non-obvious logic

**Example:**

```cpp
// Good
void handleButtonPress(int noteIndex) {
    if (noteIndex >= 0 && noteIndex < 12) {
        int fullNote = noteIndex + octaveShift * 12;
        outputNoteVoltage(fullNote);
    }
}

// Bad
void hbp(int n){
if(n>=0&&n<12){
int fn=n+octaveShift*12;outputNoteVoltage(fn);}}
```

### Variable Naming

- **camelCase** for variables: `currentVoltage`, `lastDebounceTime`
- **UPPER_CASE** for constants: `MAX_NOTES`, `DEBOUNCE_DELAY`
- **Descriptive names**: `octaveShift` not `os`

### Function Naming

- **camelCase** for functions: `updateSlew()`, `handleCalibrationMode()`
- **Verb-noun** pattern: `outputNoteVoltage()`, `blinkTriggerPulse()`
- **Descriptive**: `applyCalibration()` not `cal()`

### Comments

**Good comments explain WHY, not WHAT:**

```cpp
// Good - explains why
octaveShift = 0;  // Reset to ensure calibration starts at octave 0

// Bad - just repeats the code
octaveShift = 0;  // Set octave shift to zero
```

**Use comment headers for major sections:**

```cpp
/*******************************
      Helper Functions
*******************************/
```

**Use inline comments for complex logic:**

```cpp
// Map exponentially from rate 0.07675 (30ms) to 0.004605 (500ms)
portamentoRate = 0.07675 - (exponential * (0.07675 - 0.004605));
```

### Memory Management

- Prefer `const` for constants (saves RAM)
- Use `F()` macro for strings: `Serial.println(F("Text"));`
- Be mindful of Arduino Nano's limited RAM (2KB)

---

## Commit Guidelines

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

### Types

- **feat**: A new feature
- **fix**: A bug fix
- **docs**: Documentation changes
- **style**: Code style changes (formatting, no logic change)
- **refactor**: Code refactoring (no functional change)
- **perf**: Performance improvements
- **test**: Adding or updating tests
- **chore**: Maintenance tasks

### Subject Line

- Use imperative mood: "Add feature" not "Added feature"
- Don't capitalize first letter
- No period at the end
- 50 characters or less

### Body

- Wrap at 72 characters
- Explain WHAT and WHY, not HOW
- Separate from subject with blank line

### Footer

- Reference issues: `Fixes #42` or `Closes #123`
- Note breaking changes: `BREAKING CHANGE: description`

### Examples

**Good:**

```
fix: reset octaveShift when entering calibration mode

Calibration was starting at the wrong octave if octaveShift
was non-zero from previous normal operation. This caused
calibration values to be stored in incorrect array indices.

Now explicitly reset octaveShift to 0 when entering
calibration mode, matching behavior of other Nocturne
Alchemy modules.

Fixes #42
```

**Bad:**

```
Fixed bug
```

---

## Pull Request Process

### Before Submitting

1. **Update your branch** with latest main:
   
   ```bash
   git fetch origin
   git rebase origin/main
   ```

2. **Squash commits** if needed (multiple WIP commits):
   
   ```bash
   git rebase -i HEAD~3  # Last 3 commits
   ```

3. **Run final tests** - complete checklist

4. **Update documentation** - CHANGELOG.md, comments, etc.

### Submitting the PR

1. **Title:** Clear description of changes
   
   - Good: "Add two-zone potentiometer mapping"
   - Bad: "Update code"

2. **Description:** Use this template:
   
   ```markdown
   ## What does this PR do?
   Brief description of changes
   
   ## Why is this needed?
   Explanation of the problem or enhancement
   
   ## How was this tested?
   - [ ] Compiled successfully
   - [ ] Tested normal operation
   - [ ] Tested calibration mode
   - [ ] Tested edge cases
   
   ## Screenshots/Videos
   (if applicable)
   
   ## Related Issues
   Fixes #42
   ```

3. **Checklist:** Ensure all items are checked:
   
   - [ ] Code follows style guidelines
   - [ ] Shared files unchanged (unless necessary)
   - [ ] Self-reviewed code
   - [ ] Commented complex logic
   - [ ] Documentation updated
   - [ ] Tested thoroughly
   - [ ] No compiler warnings

### Review Process

1. **Maintainer review** - typically within 1 week
2. **Feedback addressed** - respond to comments
3. **Approval** - maintainer approves changes
4. **Merge** - maintainer merges to main

### After Merge

- Delete your feature branch
- Pull latest main
- Celebrate! 🎉

---

## Questions?

- **General questions:** Open a GitHub Discussion
- **Bug reports:** Open an Issue
- **Feature ideas:** Open an Issue with `enhancement` tag
- **Direct contact:** See FlatSix Modular website

---

## Recognition

Contributors will be:

- Listed in CONTRIBUTORS.md
- Credited in release notes
- Thanked in the community!

---

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (CC BY-NC 4.0).

---

**Thank you for contributing to Slight Of Hand!**

*The modular synthesis community thrives on collaboration and open sharing of knowledge. Your contributions help make this project better for everyone.* 🎹✨
