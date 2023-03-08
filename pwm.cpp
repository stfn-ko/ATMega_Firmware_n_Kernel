#include "pwm.h"


static void PWM::Init(void)
{
  DDRD |= 0b01000000;
  TCCR0A |= 0b10000011;
  OCR0A = 0;
}


static void PWM::SetPWM(unsigned char _duty)
{
  OCR0A = _duty;
}
