///////////////////////////////////////////////////////////////////////////////
/// DISPLAY.H
///
/// This module manages the LCD display. It also manages user input to set
/// RPS. We may, in future, want to split off the user input aspect from
/// the display aspect into separate modules, especially when we need to
/// provide a facility to manage user input from other
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <kernel.h>
//#include <LiquidCrystal_I2C.h>

// Our LCD display is another I2C device, hanging off the same bus as
// the E2 and the RTC, but it has a different address

#define DISP_I2C_ADDR	0x3f

//
// This is our display class. It inherits from Kernel::Task as it
// includes an FSM, requiring the TaskLoop, and it also needs
// to respond to messages from the message queue. As Kernel::Task
// itself inherits from Kernel::EventReceiver, then we have the
// necessary function to receive messages.

class Display : public Kernel::Task {

	private:

		// These are the states required of our state machine

		enum _DISPSTATE {

			INIT,			// system is initializing
			REFSH,			// refresh main display
			IDLE,			// waiting for input/change
			NEWRPS,			// a new actual RPS value has been submitted
			NEWDEMAND,		// a new demand RPS value has been submitted
			FIRSTKEY,		// first press of a key
			NEXTKEY,		// subsequent presses of a key
			VALIDATE,		// field entry complete, validate input
			ERROR			// display error.

		} state;			// this is the state variable for the display FSM

		//LiquidCrystal_I2C lcd;	// an instance of our LCD display

		// Member variables containing the demanded and
		// actual RPS to display

		unsigned int ActualRPS;		// the actual value of RPS
		unsigned int currentRPS;	// the current value of RPS - need this to detect change
		unsigned int EnteredRPS;	// value entered
		unsigned int DemandRPS;		// the entered value of demand RPS.

		boolean      bInitComplete;	// flag indicating initialization complete
		boolean		 bKeyPressed;	// fag indicating a key has been pressed

		unsigned char curKey;		// the current key entered

		unsigned int curpos;		// current cursor position

		// The character sequence entered by the user. We keep this because
		// it is needed in both task and message handler

		char numarr[5];

		Kernel::OSTimer * errtimer;	// this is a pointer to a timer object.

	public:

		///////////////////////////////////////////////////////////////////////
		/// Display
		///
		/// Constructor. This initializes the display class
		///
		/// @scope: PUBLIC, CONSTRUCTOR
		/// @context: TASK
		/// @param: none
		/// @return: none
		///
		///////////////////////////////////////////////////////////////////////

		Display();

	protected:

		///////////////////////////////////////////////////////////////////////
		/// TaskLoop
		///
		/// This is the task loop function, called repeatedly. This implements
		/// the display FSM.
		///
		/// @scope: PUBLIC
		/// @param: none
		/// @return: none
		/// @context: TASK
		///
		///////////////////////////////////////////////////////////////////////

		virtual void TaskLoop(void);

		///////////////////////////////////////////////////////////////////////
		/// EventHandler
		///
		/// This will be called by the message queue, once the class instance
		/// has been registered by the kernel. It is overloaded from the
		/// base class, so will be called via the vtable for this class.
		/// It is protected - so it can not be called from outside the class
		/// by any other mechanism
		///
		/// @scope: PROTECTED
		/// @param: int id - this is the ID of the message that has been
		///                  posted to this class. There is one message handler
		///                  per class, but this can be registered to more than
		///                  one message id
		/// @param: void * context - The context is simply an integer in this
		///                  case, this is the mapping we want displayed.
		/// @return: none
		///
		////////////////////////////////////////////////////////////////////////

		virtual void EventHandler(int id, void * context);

};


#endif
