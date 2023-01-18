///////////////////////////////////////////////////////////////////////////////
/// ENGD3051_CompleteTemplate.ino
///////////////////////////////////////////////////////////////////////////////

/*
#include "SevenSegEventHandler.h"
#include "KeypadTask.h"
#include "control.h"
#include "display.h"
#include "msgids.h"
#include "LogTask.h"
#include "rps.h"
#include "pwm.h"
SevenSegEventHandler	sevenseg;	// this is all we need, the rest is done in the constructor of this class
KeypadTask				keypad;		// this is all we need - the rest is done in the constructors.
Control					control;	// our control loop
Display					display;
LogTask					logger;		// our datalogger module
*/

void UserInit()
{
	Serial.begin(115200);
  // Start initializing stuff. This could be done in a number of orders, but we
	// should start with the display and end with control:
	//
	// call the Start() function on the display object
	// call the RPS::Init() function on the static RPS object
	// call the PWM::Init() function on the static PWM object
	// call the logger.Start() function
	// call the control.Start() function

	//... your code goes here.

	// set the date

	// ... your code to set the date goes here.

	// we are now done with initialization. We must tell the display that we are done
	// with the initialization process. To do this, we post the message MSG_ID_INIT_COMPLETE
	// to the message queue, to tell anyone listening that we have finished initializing.

	//... your code goes here.
}
