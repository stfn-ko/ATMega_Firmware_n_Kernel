#include "KeypadTask.h"

KeypadTask::KeypadTask() : key_state(KEY_IDLE)
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
  unsigned char iicreg[2]; iicreg[0] = 0x12;
  unsigned char registered_key;

  Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 1);
  Kernel::OS.IICDriver.IICRead(this->KEYPAD_DEFAULT_IIC_ADDRESS, &registered_key, 1);

  switch (key_state)
  {
    case KEY_IDLE:
      // this isn't a bug in gcc-avr, it is just a scoping rule for switch statements ->
      // the explicit use of brackets ensures that variables from case:n are not intialized in ->
      // subsequent cases.
      {
        if (registered_key & 0b01111000)
        {
          this->key_timer.Set(20);
          this->last_pressed = registered_key;
          this->key_state = KEY_PRESS_DETECTED;
          break;
        }

        iicreg[1] = 0x05;
        if ((registered_key & 0x07) == 0x05) iicreg[1] = 0x06;
        else if ((registered_key & 0x07) == 0x06) iicreg[1] = 0x03;

        Kernel::OS.IICDriver.IICWrite(this->KEYPAD_DEFAULT_IIC_ADDRESS, iicreg, 2);

        break;
      }
    case KEY_PRESS_DETECTED:
      {
        if (this->key_timer.isExpired() && (registered_key & this->last_pressed & 0b01111000))
        {
          for (const auto& it : this->KEY_MAP)
            if (it[0] == registered_key)
            {
              Kernel::OS.MessageQueue.Post(MSG_ID_UPDATE_7SEG, (void*)it[1], Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
              Kernel::OS.MessageQueue.Post(MSG_ID_KEY_PRESSED, (void*)it[1], Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
              this->key_state = KEY_PRESSED;
            }
        }
        else if (!(registered_key & 0b01111000))
          this->key_state = KEY_IDLE;

        break;
      }
    case KEY_PRESSED:
      {
        if (registered_key == this->last_pressed)
          break;

        for (const auto& it : this->KEY_MAP)
          if (it[0] == registered_key)
            Kernel::OS.MessageQueue.Post(MSG_ID_KEY_PRESSED, (void*)it[1], Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);

        Kernel::OS.MessageQueue.Post(MSG_ID_UPDATE_7SEG, (void*)0b11111111, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
        this->key_state = KEY_IDLE;
        break;
      }
    default:
      {
        key_state = KEY_IDLE;
        break;
      }
  }

}
