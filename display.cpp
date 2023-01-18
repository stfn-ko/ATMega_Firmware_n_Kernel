///////////////////////////////////////////////////////////////////////////////
/// DISPLAY.CPP
///
/// This module manages the LCD display. It also manages user input to set
/// RPS. We may, in future, want to split off the user input aspect from
/// the display aspect into separate modules, especially when we need to
/// provide a facility to manage user input from other
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#include "display.h"
#include "msgids.h"

//
// Temporary location for RPS_MAX and RPS_MIN - in the final version
// this will be moved as they will be needed by the control code.

#define RPS_MAX	300
#define RPS_MIN	20

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

//Display::Display() : state(INIT), ActualRPS(0), currentRPS(0), EnteredRPS(0),
//                     lcd(DISP_I2C_ADDR,16,2), curpos(9), bKeyPressed(false),
//					 bInitComplete(false)
//{
	// After initialization, we need to subscribe this class to the
	// messages we need to receive from the message queue.
	// These are:
	//
	// MSG_ID_INIT_COMPLETE  - the firmware has completed its initialization
	// MSG_ID_NEW_ACTUAL_RPS - a new actual RPS has been measured for display
	// MSG_ID_KEY_PRESSED    - a key on the keypad has been pressed
	//
	// Your code goes here...

//}

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

void Display::TaskLoop(void)
{
	switch(state) {

		case INIT:		// Startup. Owing to a bug in the LiquidCrystal library, we
						// can not do this in the constructor. The reason for this
						// is that initialization of the LCD display will not work
						// if carried out during the initialization of a
						// variable on the heap before the UserInit() function is
						// called. So we have to implement
						// an initialization sequence in the state machine.
						//
						// This means we need to do the following:
						//
						// - lcd.init()
			            // - Turn on backlight on display
			            // - Clear the display
			            // - Set the cursor to the origin (top left)
			            // - Print the 'starting' message.

						//....your code goes here....

						// NOTE. The only condition for transitioning out of this state
						// is when a message MSG_ID_INIT_COMPLETE comes in. Thus,
						// to save code, rather than using a flag and testing it here,
						// we transition out of INIT to REFSH in the message handler
						// function EventHandler. This would not be appropriate style
						// if the code here contained additional transition conditions.

						// - Check if the firmware has sent in the MSG_ID_INIT_COMPLETE
						//   message; if it has:
			            //   - transition to state REFSH.

						break;

		case REFSH:		// Refresh the display

						// In this state, we are performing a complete re-write of the
						// display text. So
						//
						// - clear the LCD display
						// - set the cursor to the origin
						// - Print the _text label_ and value for the demand RPS value
						// - reset the cursor to the start of the second line
						// - print the _text label_ and value for the actual RPS value
						// - We have now refreshed the whole display, now unconditionally
						//   transition the FSM to IDLE

						//... your code goes here...

						break;

		case IDLE:		// idle state. We are waiting here for a change; this will
						// either be an RPS input from the user (a key press), or a change
						// to the displayed RPS. In all cases, this comes
						// in from a message.
						// So, in sequence:
						// - check if a key pressed. If so:
						//   - check if the key is a number (enter or BS can be ignored). If so:
						//     - transition to FIRSTKEY
						//   - otherwise
						//     - check if a new actual RPS value has been submitted. This could be
						//       detected simply by saving the previous value in a member variable,
						//       then checking if the value of ActualRPS has differed from the stored
						//       value - if it is different, then the display needs updating.
						//       If an update is needed:
						//       - transition to NEWRPS

						//
						// ... your code goes here ...

						break;


		case NEWRPS:	// a new actual RPS value has been provided by the firmware

						// All that needs to happen here is to
						// - move the cursor to the position that the numeric value of actual RPS is displayed
						//   on the display.
					    // - overwrite the value on the display with the new value of ActualRPS.
						// - unconditionally transition to IDLE
						//
						//.... your code goes here....

						break;

		case FIRSTKEY:	// first time a key is pressed.
						// This is the first press. Set up the
						// display:
						curpos=9;

						// The trick here is to use the 'curpos' member variable to contain the
						// cursor position. You can also use the member array 'numarr' to store
						// the sequence of numbers entered by the keypad as a string. Backspaces
						// will simply reduce the cursor position by one, and also the index into
						// the 'numarr' array by one. If the key pressed is numeric, we convert to
						// ASCII and add into the same relative position in Then, later,
						// when we are ready to validate, we can use the 'sscanf' function to read
						// an integer value from the string.
						//
						// So, in the FIRSTKEY state we know the key pressed must be numeric,
						// so we set up the value. Since we may press enter at any point in the
						// string, we first can fill the 'numarr' array with the string representation
						// of the value of DemandRPS, using the sprintf function or similar. We then
						// overwrite the first character of the string with the key pressed (after
						// conversion to ASCII. We then set up the display: you could either clear
						// it and replace the text with 'New RPS: 103' etc, or you could simply
						// leave the page in place and edit in situ. This is up to you. So first
						// turn on the cursor, and allow the cursor to flash (or similar - it is
						// up to you how you display your cursor).
						// set 'curpos' to the first cursor position, and set numarr[0] to the
						// key pressed, display the new value, increment curpos to put the cursor
						// in the next position. Then transition to state NEXTKEY.
						//
						//... your code goes here...

						break;


		case NEXTKEY:	// If we are in this state, we are waiting for a subsequent
						// key press (the first would have been handled in state FIRSTKEY). So first
						// check for a backspace - if this has been pressed simply reduce the cursor
						// position by one and set the cursor position on the display, staying in this
						// state (providing we are not at the beginning of the field. If we try to
						// backspace to a position before the start of the entry field, ignore the backspace).
						// If an enter key is pressed, then turn off the cursor, use the sscanf function
						// or similar to update EnteredRPS with the value represented by the string in
						// 'numarr', then transition to state VALIDATE to check the value.
						// Otherwise, it is a regular digit. Check we are not beyond the last digit in the
						// field (remember RPS is a three-digit integer with leading zeroes) - if it is
						// beyond the last digit then ignore the press. If it is within the field, then
						// update the display and the relevant digit within 'numarr', advance the
						// cursor and stay in this state.


						break;

		case VALIDATE:	// Validate the entered value. The value of EnteredRPS would have been updated
						// during the transition from state NEXTKEY. Here we just have to check it to
						// confirm it is within bounds, and accept it if so, or transition to ERROR
						// if it is not.
						// So, check if EnteredRPS is less than RPS_MAX, greater or equal to RPS_MIN
						// or alternatively is zero (zero is also a valid value). If these conditions
						// are met, then set DemandRPS to the value of EnteredRPS, post the message
						// MSG_ID_NEW_RPS_ENTERED to the message queue with the value of EnteredRPS as the
						// payload and transition to state REFSH (the value is ok and accepted)
						//
						// If the conditions are not met, we need to create a 2s timer and display
						// 'ERR' in place of the value. So we create a new timer (use the 'new' object
						// creation method to set 'errtimer' to the address of the new Kernel::OSTimer). We
						// do it this way because we only need the timer here, we do not want to chew up scarce
						// memory leaving the timer object resident when we don't need it. We also overwrite
						// the field on the display with 'ERR', then transition the state to ERROR.

						break;

		case ERROR:		if(errtimer->isExpired()) {

							// we won't need the timer now,
							// free mem associated with it.

							delete errtimer;

							// refresh the display to EnteredRPS

//							lcd.setCursor(9,0);
//							lcd.print(numarr);
//							lcd.setCursor(9,0);
//							lcd.blink();
//							state=REFSH;
						}
						break;

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

void Display::EventHandler(int id, void * context)
{
	// We need to respond to these messages:

	switch(id) {

		case MSG_ID_INIT_COMPLETE:	// The rest of the firmware will tell us when its
									// initialization is complete. We only have to
									// respond to this message if the FSM is in the
									// INIT state. If not, we can just ignore it.
									// We can actually action the state transition
									// here, but only because there is no other transition
									// condition in the FSM loop for state INIT.

									bInitComplete=true;
									break;

		case MSG_ID_NEW_ACTUAL_RPS:	// the actual RPS has changed
									// We just update our internal value of RPS

									ActualRPS=(unsigned int)context;
									break;

		case MSG_ID_KEY_PRESSED:	// someone has pressed a key
									// We use a boolean fag here to indicate to the FSM
									// that a key has been pressed.

									bKeyPressed=true;
									curKey=(unsigned char)context;
									break;

		default: break;				// should never get here
	}

}
