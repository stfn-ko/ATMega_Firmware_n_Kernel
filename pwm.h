///////////////////////////////////////////////////////////////////////////////
/// PWM.H
///
/// This describes a simple, straightforward function-based approach to
/// controlling PWM.
///
/// So why not use a 'normal' class here? Well, the answer is that classes
/// define a new type, essentially an object. We can then instantiate more
/// than one instance of that object. An example of this would be the RTC
/// or EEPROM drivers. We can instantiate one instance of the object for
/// each IC we physically have in the system.
///
/// However, with PWM, we only have one PWM generator in the chip. It thus
/// makes no sense to create an object type, of which multiple instances
/// can be created, since we can only have one physical instance of the
/// hardware.
///
/// We could use a class with only static members - this is a type of class
/// where there is no concept of an 'instance' - a static class is really
/// an aggregate of functions. As such, it still helps with readability and
/// encapsulation, but  An example of this is the Arduino 'Serial'
/// class, of which you have used extensively. We create one here to show
/// how this is done.
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _PWM_H_
#define _PWM_H_

class PWM {

	// note, no member variables. If any are required, they should be
	// declared 'static'

	public:

		// note the absence of a constructor. Constructors make no sense
	    // on a class with only static members.

		///////////////////////////////////////////////////////////////////////
		/// Init
		///
		/// Initialize the PWM subsystem and start it (with a zero pulse width)
		///
		/// @scope: STATIC, PUBLIC
	    /// @context: TASK
	    /// @param: NONE
	    /// @return: NONE
		///
		//////////////////////////////////////////////////////////////////////

		static void Init(void);

		//////////////////////////////////////////////////////////////////////
		/// SetPWM
		///
		/// Set the pulse width. This is an unsigned 8-bit value from
		/// 0 to 0xff - representing duty cycles from 0 to 1
		///
		/// @scope: STATIC, PUBLIC
		/// @context: ANY
		/// @param: unsigned char duty - the duty cycle required from 0-0xff
		/// @return: none
		///
		//////////////////////////////////////////////////////////////////////

		static void SetPWM(unsigned char duty);

};

#endif
