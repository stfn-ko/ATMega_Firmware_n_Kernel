/// The keypad is hooked to the port expander. Unfortunately the port expander
/// is unable to generate interrupts, so this must be polled. This means
/// we need a task.

#ifndef _KEYPADTASK_H_
#define _KEYPADTASK_H_

#include "kernel.h"
#include "msgids.h"

class KeypadTask : public Kernel::Task
{
    static constexpr unsigned char KEYPAD_DEFAULT_IIC_ADDRESS = 64;
    
    unsigned char last_pressed = 0; 
    
    Kernel::OSTimer	key_timer = 10; 

    // map 8-bit GPIO port address, to the key value ->
    const unsigned char KEY_MAP[12][2] =
    {
      {0b00001101, 0}, //0
      {0b01000011, 1}, //1...
      {0b01000101, 2},
      {0b01000110, 3},

      {0b00100011, 4},
      {0b00100101, 5},
      {0b00100110, 6},

      {0b00010011, 7},
      {0b00010101, 8},
      {0b00010110, 9},

      {0b00001011, 10}, //*
      {0b00001110, 11}  //#
    };

    enum KEY_STATE
    {
      KEY_IDLE,
      KEY_PRESS_DETECTED,
      KEY_PRESSED,
      KEY_RELEASE_DETECTED
    } key_state;

  protected:
    /// This is the task loop function, called repeatedly. It polls the
    /// keyboard, and implements an FSM to maintain state and condition
    /// of the keyboard between presses
    virtual void TaskLoop(void);

  public:
    /// This is the constructor: simply registers this task with the
    /// kernel and starts it - it can run from the get-go.
    KeypadTask();

};


#endif
