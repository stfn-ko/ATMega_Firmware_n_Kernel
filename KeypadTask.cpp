///////////////////////////////////////////////////////////////////////////////
/// KeypadTask.h
///
/// The keypad driver task.
///
/// The keypad is hooked to the port expander. Unfortunately the port expander
/// is unable to generate interrupts, so this must be polled. This means
/// we need a task.
///
/// This task class implements an FSM that monitors the keypad. It integrates
/// with the rest of the system by posting three message types:
/// MSG_ID_KEY_PRESSED when a key is pressed
/// MSG_ID_KEY_RELEASED when the key is released.
/// MSG_ID_UPDATE_7SEG to update the seven-segment display (as this update
/// only depends on the keypad
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#include "KeypadTask.h"

KeypadTask::KeypadTask() : key_state(KEY_IDLE), key_timer(10), last_pressed(0)
{
  // set bottom 3 pins (GPIA0, GPIAO1, GPIAO3) as outputs
  unsigned char iicreg[2] = {0x00, 0xf8};
  Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 2);

  // invert column 1 and 2 input bits state to high(1 - key not pressed; 0 - key pressed)
  iicreg[0] = 0x12; iicreg[1] = 0x06;
  Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 2);

  // invert row input bits state to high(1 - key not pressed; 0 - key pressed)
  iicreg[0] = 0x02; iicreg[1] = 0x78;
  Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 2);

  // register MCP23017
  Kernel::OS.TaskManager.RegisterTaskHandler(this);
  Start();
}


void KeypadTask::TaskLoop(void)
{
  // Key state in this current cycle
  unsigned char matrix;

  // GPIOA register address
  unsigned char iicreg[2] = {0x12, 0x00};
  Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 1);
  Kernel::OS.IICDriver.IICRead(this->KEYPAD_DEFAULT_IIC_ADDRESS, &matrix, 1);

  switch (key_state)
  { 
    // this isn't a bug in gcc-avr, it is just a scoping rule for switch statements ->
    // the explicit use of brackets ensures that variables from case:n are not intialized in ->
    // subsequent cases.
    case KEY_IDLE:
      {
        // check if key has been pressed
        // This will be so if any of the
        // bits 6 downto 4 are set.
        // If more than one bit is set, some hamfisted user
        // has hammered more than one key. So up to us which one
        // we use, or just ignore it. We will ignore double-press
        // here as we can't guarantee what has been pressed

        // A way to do this is to check if the column is a power of 2 - i.e.
        // one bit set. A neat trick is this: if the row is in a variable
        // called 'm', then if we check (m && !(m&(m-1))) we will be able
        // to test this - see if you can work out how it works!

        // If more than one key in the same column is pressed simultaneously,
        // ignore them all.

        // Examine the value of the variable matrix to see if a
        // key has been pressed
        // For this exercise, double keys will be ignored.
        // If a press is detected, change state to KEY_PRESS_DETECTED and set the
        // debounce timer. For future comparisons, save the current
        // state of matrix.

        // m only contains bit-info about rows
        unsigned char m = matrix & 0x78;

        if (!(m && (m & (m - 1))))
        {
          this->last_pressed = matrix;
          this->key_timer.Set(500);
          this->key_state = KEY_PRESS_DETECTED;
          break;
        }

        // Otherwise move onto the next column. Set the current column
        // to logic 1 and the next column to logic 0. Wrap round to
        // the first column if necessary
        iicreg[1] = 0x05;
        if ((matrix & 0x07) == 0x05) iicreg[1] = 0x06;
        else if ((matrix & 0x07) == 0x06) iicreg[1] = 0x03;

        Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 2);
        break;
      }
    case KEY_PRESS_DETECTED:
      {
        if (matrix != this->last_pressed)
          this->key_state = KEY_IDLE;

        else if (key_timer.isExpired())
        {
          // - Post the value of the key to the KEY_PRESSED message.
          // - Post the value of the key to the CHANGE_7SEG message.
          this->key_state = KEY_PRESSED;
        }

        break;
      }
    case KEY_PRESSED:
      {
        // check for a key release. This will happen if the matrix value has missing bits that
        // are set to '1' in the last_pressed value
        // Check the value of the key. Use this to check for a key release.
        if (matrix == this->last_pressed) {
          for (const auto& it : this->KEY_MAP)
            if (it[0] == matrix); // - Post the value of key to the CHANGE_7SEG message.
        }
        else {
          // - Post the value of the (previously debounced) key to the KEY_RELEASED message
          // - Post the value of dot point to the CHANGE_7SEG message.
          this->key_state = KEY_IDLE;
        }

        break;
      }
    default:
      {
        key_state = KEY_IDLE;
        break;
      }
  }

}
