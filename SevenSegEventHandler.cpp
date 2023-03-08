#include "SevenSegEventHandler.h"


SevenSegEventHandler::SevenSegEventHandler()
{
  // DATA (SER on HC595) is on PORTD bit 4
  // CLK  (SRCLK on HC595) is on PORTB bit 0
  // EN   (ECLK on HC595) is on PORTD bit 7

  DDRD |= 0b10010000;  // set to output
  DDRB |= 0b00000001; // set to output
  PORTD &= ~0b10000000;   // latch high
  PORTB &= ~0b00000001;   // data pin low
  PORTD &= ~0b00010000;   // clock pin low

  Kernel::OS.MessageQueue.Subscribe(MSG_ID_UPDATE_7SEG, this);
  Kernel::OS.MessageQueue.Post(MSG_ID_UPDATE_7SEG, (void *)0b11111111, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
}


void SevenSegEventHandler::EventHandler(int _posted_msg_id, void * _context)
{
  unsigned char to_decimal = this->segment[min((uint16_t)_context % 16, 12)];

  if ((uint16_t)_context & this->msb_mask)
    to_decimal &= ~this->lsb_mask;

  for (auto i = 0; i < 8; ++i)
  {
    if (to_decimal & (this->lsb_mask<<i))
      PORTD |= 0b00010000;
    else
      PORTD &= ~0b00010000;

    PORTB |= this->lsb_mask;
    PORTB &= ~this->lsb_mask;
  }

  PORTD |= this->msb_mask; // latch high -> dont want catching noise on the bus while idle
  PORTD &= ~this->msb_mask; // latch low
}
