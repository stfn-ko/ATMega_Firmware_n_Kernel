/// This module is associated with calculating the revolutions per second.
/// Similar to the PWM module, we implement this in a class with only static
/// member functions, as it makes no sense to be able to create 'instances'
/// of this since there is only one beam-breaker tachometer in the system.
/// Now note also that this is interrupt driven. We use two separate interrupts
/// The pin change interrupt is triggered when the beam is broken. The timer
/// interrupt is triggered when a hardware timer expires.
/// The way this works is to set up a timer to generate an interrupt at a
/// fixed hardware interval. This interval needs to be precise, so we have
/// to use a hardware timer. This timeout can not be implemented in software
/// alone.
/// Between each of the timer interrupts, we count the number of times the
/// beam is broken. When the pin-change interrupt is triggered, its
/// interrupt service routine is called. All we need to do in this interrupt
/// service routine is to increment a count


#ifndef _RPS_H_
#define _RPS_H_

#include <kernel.h>

class RPS
{
  public:
    static unsigned char pinC_last_state;   // the last state of pin C
    static uint16_t current_rps_count, rps_count;  //static variables are defaulted to 0 by the system, but need to check where the variable is first used

    /// Initialize the pin change interrupt and the interval timer required
    /// for the RPS calculation
    static void Init(void);

    /// Return the last calculated revolutions per second as an integer
    static int GetRPS(void);
};

#endif
