/// This module manages the LCD display. It also manages user input to set
/// RPS. We may, in future, want to split off the user input aspect from
/// the display aspect into separate modules, especially when we need to
/// provide a facility to manage user input from other
///
/// @Note: Includes Kernel::Task to use the TaskLoop for FSM ->
/// and to respond to messages from the message queue


#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "control.h"
#include <LiquidCrystal_I2C.h>
#include "SevenSegEventHandler.h"

class Display : public Kernel::Task
{
    static constexpr unsigned char DISPLAY_DEFAULT_IIC_ADDRESS = 0x3F;
    
    uint16_t actual_rps, current_rps, input_rps, demand_rps = 0;

    bool init_complete, is_key_pressed = false;
    
    unsigned int cursor_position = 9;
    
    unsigned char current_key;
    
    char user_input_values[3];

    Kernel::OSTimer error_timer;

    LiquidCrystal_I2C lcd;

    enum DISPLAY_STATE
    {
      INIT_DISPLAY,
      REFRESH_DISPLAY,
      IDLE_DISPLAY,
      NEW_ACTUAL_RPS,
      NEW_DEMAND_RPS,
      FIRST_KEY_PRESS,
      NEXT_KEY_PRESS,
      VALIDATE_INPUT,
      DISPLAY_ERROR
    } display_state;


  protected:
    /// This is the task loop function, called repeatedly. This implements
    /// the display FSM
    virtual void TaskLoop(void);

    /// This will be called by the message queue, once the class instance
    /// has been registered by the kernel. It is overloaded from the
    /// base class, so will be called via the vtable for this class.
    /// It is protected - so it can not be called from outside the class
    /// by any other mechanism
    virtual void EventHandler(int _posted_msg_id, void * _context);


  public:   
    Display();
};


#endif
