///////////////////////////////////////////////////////////////////////////////
/// CONTROL.CPP
///
/// Main control task. This performs task-time async operations
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#include "control.h"
#include "kernel.h"
#include "msgids.h"
#include "rps.h"
#include "pwm.h"

//////////////////////////////////////////////////////////////////////
/// Control
///
/// Constructor. This sets up our temporary log timer
///
/// @scope: PUBLIC, CONSTRUCTOR
/// @context: TASK
/// @param: NONE
/// @return: NONE
///
//////////////////////////////////////////////////////////////////////

Control::Control() : tm(250), tmcount(0), demandRPS(0), actualRPS(0)
{
	// We need to subscribe to one message here: the MSG_ID_NEW_RPS_ENTERED.
	// ... your code goes here...
}

////////////////////////////////////////////////////////////////////////
/// TaskLoop
///
/// Main task loop - this is overloaded from class Task and performs
/// the deed!
///
/// @scope: PUBLIC
/// @context: TASK
/// @param: none
/// @return: none
///
////////////////////////////////////////////////////////////////////////

void Control::TaskLoop()
{
	// The control module simply has to (a) ensure that the display
	// is updated regularly from the actual PWM value and (b) trigger the
	// datalogger at a relevant interval. Since the display is slow, we can update
	// this at every quarter of a second.
	// The datalogger must log the speed for all demanded speeds greater than zero, once
	// every 2 seconds. So we can use the same kernel timer here and simply create
	// the log entry every eghit times the 250ms timer expires.
	// Each time the 250ms timer expires, we use RPS::GetRPS() to get the actual
	// RPS value from the counter in our variable 'actualRPS()', then post it to
	// the display via the message queue using the message
	// ID MSG_ID_NEW_ACTUAL_RPS and the actualRPS as the context.
	//
	// Every egeigth timer expiry (which corresponds to every wto seconds), we log
	// the value of RPS, by posting it to the datalogger via our little private helper
	// function SendLogMessage(), unless it is zero.

	if(tm.isExpired()) {

		//.... your code goes here.


	    // reinitialize the timer.

		tm.Restart();
	}
}

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

void Control::EventHandler(int id, void * context)
{

	// We only handle the one ID to which we are subscribed

	if(id==MSG_ID_NEW_RPS_ENTERED) {

		// If we receive this message, it means a new RPS value
		// has been entered via the keypad, and this can be obtained
		// from the 'context' argument, typecast to an unsigned int.
		//
		// Note it will already have been validated, so we don't have
		// to check it again.
		//
		// So, we need first to set the new duty. We can, as a starting
		// point, scale the range between zero and RPS_MAX to an 8-bit duty
		// cycle value between 0 and 255, then send this duty value to the
		// PWM module using PWM::SetPWM(). It may need a slightly different
		// scale factor later.
		//
		// Remember these are integer values, so multiplication should
		// take place before division into a longer integer type than a
		// unsigned char, then the unsigned char conversion can be done later

		// We now need to 'fit' this between 0 and 255. To get a
		// reasonable open-loop PWM, we will need to do some
		// calculation. But for now we scale to be between RPS_MIN and RPS_MAX

		//... your PWM setting code goes here

		// Now, if we are setting this to zero, we must trigger a logger dump.
		// If set to any other value, we must log the change. We check if
		// the new RPS value is zero. If so, we trigger a datalog readback by
		// posting the message MSG_ID_DATALOG_DUMPLOG (with a null context).
		// If the new RPS value is nonzero, we log the new value by calling
		// our little private helper function SendLogMessage() with the new
		// demand value and with the actual RPS value.

	}
}

//////////////////////////////////////////////////////////////////////
/// SendLogMessage
///
/// A little helper function that takes two numeric arguments
/// corresponding to the demanded speed and the actual speed, and
/// constructs the string message before sending to the log.
///
/// @scope:   PRIVATE
/// @context: TASK
/// @param: unsigned int DemandRPS
/// @param: unsigned int ActualRPS
/// @return: none
///
//////////////////////////////////////////////////////////////////////

void Control::SendLogMessage(unsigned int DemandRPS, unsigned int ActualRPS)
{
	//
	// This is a little helper function. Since we have to send a log to the datalogger
	// from two places (once timed every 2 seconds, the other when a new value is
	// entered, we implement a little function.
	//
	// This should do the following:
	// Create a String object on the heap (NOT locally): i.e. use the 'new'
	// operator. This is because this object needs to be passed to the message
	// queue, it is then deleted when the object is de-queued.
	// So:
	// Create the String object:
	// Fill it with something like "Demand: xxx Actual: xxx" where
	// the xxx is the relevant value, interpreted from the arguments.
	// Post the message to the message queue with ID MSG_ID_DATALOG_EVENT, and
	// with the String object as the payload.

}


