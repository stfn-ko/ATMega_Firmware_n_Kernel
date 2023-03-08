#include "rps.h"

unsigned char RPS::pinC_last_state = 0;
uint16_t RPS::current_rps_count = 0;
uint16_t RPS::rps_count = 0;

static void RPS::Init(void)
{
  TCCR1A = 0;
  TCCR1B = 0b000000011;
  TIMSK1 |= 0b00000001;

  DDRC &= ~0b00001000;
  PCMSK1 |= 0b00001000;
  PCICR |= 0b00000010;
}


static int RPS::GetRPS(void)
{
  return (current_rps_count * 100) / (26 * 3);
}


ISR(PCINT1_vect)
{
  if ((PINC & 0x08) && !(RPS::pinC_last_state & 0b00001000))
    ++RPS::rps_count;
  
  RPS::pinC_last_state = PINC;
}


ISR(TIMER1_OVF_vect)
{
  RPS::current_rps_count = RPS::rps_count;
  RPS::rps_count = 0;
}
