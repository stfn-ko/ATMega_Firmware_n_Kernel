/// This class, inherits from EventReceiver and is used to update
/// the seven-segment display according to the specification.
///
/// It hooks the message queue to receive just one message ID.


#ifndef _SEVENSEGEVENTHANDLER_H_
#define _SEVENSEGEVENTHANDLER_H_

#include "kernel.h"
#include "msgids.h"

class SevenSegEventHandler : public Kernel::EventReceiver
{
    static constexpr unsigned char msb_mask = 0b10000000; // most significant bit mask
    static constexpr unsigned char lsb_mask = 0b00000001; // least significant bit mask
    
    const unsigned char segment[13] =
    {
      0b00000011, //0
      0b10011111, //1
      0b00100101, //...
      0b00001101,
      0b10011001,
      0b01001001,
      0b01000001,
      0b00011111,
      0b00000001,
      0b00001001, //9
      0b11000001, //b
      0b01100001, //E
      0b11111111
    };

  protected:
    virtual void EventHandler(int _posted_msg_id, void * _context);

  public:
    SevenSegEventHandler();

};


#endif
