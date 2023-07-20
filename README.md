# Firmware and Kernel for ATMega328p

> Kernel belongs to my professor at De Montfort University (DMU) John Gow.

## Features and Specification
This repository contains firmware for a motor speed control system with specific features and requirements. The key features and specifications are as follows:

### 1. Display (Main Operation)

- A 2x16 LCD display that shows two three-digit values: demanded speed (set by the operator via the keypad) and actual speed (measured).
- The display layout is up to the designer, but it should be clear, legible, and unambiguous.
- Leading zeroes should not be suppressed.
- When changing the speed via the keypad, a cursor should indicate the entry of digits one at a time, with backspace functionality.

### 2. Keypad

- A key is accepted as 'pressed' when the debounce is complete and consistent with an actual keypress.
- If multiple keys are pressed simultaneously, the first detected key press is accepted.
- Numerical values can be set by the operator via the keypad.
- The '#' key acts as an 'enter' key, accepting the entered value.
- The '*' key is the backspace key, deleting the previous digit entered (if any).

### 3. 7-Segment LED Display

- Displays the current key being pressed while it is pressed.
- '#' is displayed as 'E', and '*' is displayed as 'b'.
- When no key is pressed, the decimal point is illuminated.
- The decimal point is extinguished while a key is pressed.

### 4. Control

- On power-up/reset, the display shows the message "Starting..." until the firmware is fully initialized (within 2 seconds).
- The date and time can be hardcoded during initialization for testing purposes.
- Once initialized, the display should behave as required by section 1.
- A valid date and time should be programmed into the real-time clock, and the display should read accordingly.
- The motor should be stationary with both demanded and actual speed reading zero.
- The 7-segment display should be blank, with the decimal point illuminated.

### 5. Motor Speed Control

- Motor speed control is achieved using open-loop control.
- Maximum displayed speed error (demanded value to actual value) is 10%, with some slack allowed for different motor characteristics.
- Maximum actual speed error (measured by an external tachometer) is 10%.
- Minimum motor speed is 50 revolutions per second (rps).
- Maximum motor speed is determined experimentally using PWM with a duty cycle of 1.
- Operators cannot enter speeds outside the specified range, showing an 'ERR' message.

### 6. Data Logging

- Data is logged once per second when the open-loop speed controller is active (demanded speeds greater than zero).
- Logging is suspended when the demanded speed is zero.
- Log entries are made every 2 seconds and additionally when a new speed value is entered via the keypad.
- Each log entry records the day, month, year, hour, minute, second, demanded speed, and actual speed.
- Logged entries are written to the external I2C EEPROM.
- When the demanded speed is set to 0 RPS, data logging stops, and the logged data is sent to the serial port in human-readable comma-separated variable (CSV) format, suitable for importing into spreadsheet packages like Excel.
- The external EEPROM employs a wrap-around mechanism to overwrite data from the beginning without corrupting the storage.

## Schematics

- ### [Development Board](./schematics/Dev_Board_Project)
- ### [Datasheet for MCP7490](./schematics/MCHPS03117)
- ### [Datasheet for 24LC512](./schematics/MCHPS05656)
