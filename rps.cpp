///////////////////////////////////////////////////////////////////////////////
/// RPS.CPP
///
/// This module is associated with calculating the revolutions per second.
/// Similar to the PWM module, we implement this in a class with only static
/// member functions, as it makes no sense to be able to create 'instances'
/// of this since there is only one beam-breaker tachometer in the system.
///
/// Now note also that this is interrupt driven. We use two separate interrupts
/// The pin change interrupt is triggered when the beam is broken. The timer
/// interrupt is triggered when a hardware timer expires.
///
/// The way this works is to set up a timer to generate an interrupt at a
/// fixed hardware interval. This interval needs to be precise, so we have
/// to use a hardware timer. This timeout can not be implemented in software
/// alone.
///
/// Between each of the timer interrupts, we count the number of times the
/// beam is broken. When the pin-change interrupt is triggered, its
/// interrupt service routine is called. All we need to do in this interrupt
/// service routine is to increment a count
///
/// We can then store this count in a separate variable in the timer interrupt
/// to be used later, and and zero the working counter.
///
/// If we know the number of beam-breaks in a given fixed interval, we can
/// calculate the revs per second easily.
///
/// A word about interrupt handers (interrupt service functions) in C++
/// -------------------------------------------------------------------
///
/// Interrupt handlers have a very specific calling convention, as they are
/// called directly by the CPU in response to a hardware event (see lecture
/// notes). Interrupt handlers, being called by the system, thus 'belong'
/// to the system, and thus can not be declared as a member function
/// of a class, particularly not of a specific instance of a class. Think
/// for a moment; if the CPU can call an interrupt handler asynchronously
/// at any time, there is no way the CPU can 'know' to which instance
/// of any given class to direct the interrupt. So interrupt handlers
/// are declared as normal functions that are not part of a class. This
/// in turn means that an interrupt handler can only utilize parts of a
/// class that are in scope of the function (e.g. global instances, module
/// instances or static members of classes).
///
///////////////////////////////////////////////////////////////////////////////

#include <kernel.h>
#include "rps.h"

// We do not have a constructor in a class with static member variables, so
// we declare these here and initialize as follows

static unsigned char RPS::lastPinC=0;
static unsigned long RPS::currpscount=0;	// the current RPS count
static unsigned long RPS::rpscount=0;		// the working (incrementing) count

///////////////////////////////////////////////////////////////////////
/// Init
///
/// Initialize the pin change interrupt and the interval timer required
/// for the RPS calculation
///
/// @scope: PUBLIC, STATIC
/// @context: TASK
/// @param: NONE
/// @return: NONE
///
///////////////////////////////////////////////////////////////////////

static void RPS::Init(void)
{
	// First set up Timer1 to give an interrupt every 0.262144 seconds. We use
	// this to count revs. We have a 16MHz clock. We use the prescaler to
	// divide by 64, then the timer will overflow every 65536 cycles. With a
	// 16MHz clock, that is 0.262144 seconds. To do this we need to set relevant
	// values in three registers (see relevant section in ATMega328 datasheet):
	// 1. TCCR1A and TCCR1B to set the Timer1 prescaler to 64. This means that
	//    the timer will clokc at 16MHz/64=250kHz. Since Timer1 is a 16 bit
	//    timer, it will wrap around with a frequency of 250kHz/65536=3.814Hz.
	//    This means that interrupts, which occur when the timer wraps around,
	//    will occur every 1/3.814=0.262 of a second.
	// 2. Enable the overflow interrupt (set the appropriate bit in TIMSK1).


	//code goes here

	//
	// now set up the pin change interrupts on A1. We use pin change interrupt 0,
	// as this is used by the beam-breaker tacho. We need to do three things:
	//    1.  set the relevant pin of port C to an input (see the schematic
	//        to determine which pin is needed - it is the one driven from
	//        the beam breaker
	//    2.  set up the pin change interrupt mask register to enable
	//        the relevant pin change interrupt (see ATMega328 datasheet)
	//    3.  Enable the pin change interrupt (see PCICR in the ATMega328p
	//        datasheet

	//code goes here
}

///////////////////////////////////////////////////////////////////////
/// GetRPS
///
/// Return the last calculated revolutions per second as an integer
///
/// @scope: PUBLIC, STATIC
/// @context: ANY
/// @param: NONE
/// @return: int - returns the current revolutions per second
///
///////////////////////////////////////////////////////////////////////

static int RPS::GetRPS(void)
{
	// This needs one line of code. It takes currpscount (this is the number
	// of beam-breaks per 0.2621 of a second) and converts it to a number
	// of revolutions per second (note there are three blades on the fan)!
	// This should be returned as an int.

	return 0;	// CHANGE THIS TO RETURN YOUR RPS VALUE
}

///////////////////////////////////////////////////////////////////////////////
/// ISR - Pin change interrupt
///
/// This ISR is triggered when the beam is brkoen
///
/// @context: INTERRUPT
/// @scope: INTERNAL
///
///////////////////////////////////////////////////////////////////////////////

ISR(PCINT1_vect)
{
	// Ok. The problem here is that the rotary encoder is on the same
	// interrupt as the beam-breaker. The pin-change interrupt covers more
	// than a single pin! We need to discriminate. Although
	// we are not using the rotary encoder in this application, we may do
	// so at some time in the future, so good programming technique ensures
	// that we do not store up problems for later.
	// So we need two tests before we can confirm that it is 'our' beam break
	// pin that has triggered the interrrupt. We need two tests.
	// If the relevant bit of PINC is high, AND the similar bit of the variable
	// 'RPS::lastPinC' is zero, it means that there has been a clear transition
	// of the relevant bit from low to high. If both these tests pass, then
	// all we need to do is increment rpscount.
	//
	// So:
	// - If PINC bit is '1' AND lastPinC bit is '0'
	//    - increment RPS::rpscount.
	// - set RPS::lastPinC to PINC.
	//

	//code goes here

	// - Save the value of the relevant bit of PINC in the RPS::lastPinC variable

	//code goes here
}

///////////////////////////////////////////////////////////////////////////////
/// ISR - Timer 1 overflow.
///
/// This ISR is triggered every time the timer 1 overflow wraps around
///
/// @context: INTERRUPT
/// @scope: INTERNAL
///
///////////////////////////////////////////////////////////////////////////////

ISR(TIMER1_OVF_vect)
{
	// if this is called, we need to count the number of pin-change
	// interrupts we have, and save this in the currpm global.
	//
	// We need to:
	//   - capture the current value of RPS::rpscount in RPS::currpscount
	//   - then set the RPS::rpscount to zero for the next period of 0.262

	//code goes here
}



