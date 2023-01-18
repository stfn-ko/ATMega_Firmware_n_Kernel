///////////////////////////////////////////////////////////////////////////////
/// RPS.H
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

#ifndef _RPS_H_
#define _RPS_H_

//
// We define a single class with only static member functions to encapsulate
// the functions relating to initialization and querying of the RPS.

class RPS {

	public:

		// These members need to be public. Otherwise the interrupt handlers,
		// which can not be members of a class, are unable to access them.
		// We could call into a member function in the class and keep them
		// private, but this adds the unnecessary overhead of a function call

		static unsigned char RPS::lastPinC; 	// the last state of pin C
		static unsigned long RPS::currpscount;	// the current RPS count
		static unsigned long RPS::rpscount;		// the working (incrementing) count

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

		static void Init(void);

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

		static int GetRPS(void);

};

#endif

