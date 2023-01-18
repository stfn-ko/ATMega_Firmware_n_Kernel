///////////////////////////////////////////////////////////////////////////////
/// SevenSegEventHandler.cpp
///
/// This class, inherits from EventReceiver and is used to update
/// the seven-segment display according to the specification.
///
/// It hooks the message queue to receive just one message ID.
///
///////////////////////////////////////////////////////////////////////////////

#include "SevenSegEventHandler.h"
#include "msgids.h"

// You will likely need a mapping table, to map a given message to the
// actual state of the segments. You can declare this as 'const' to ensure
// it is stored in flash, not RAM. An example of such a declaration is below.
// implementation of the mapping table. As this is a 'const', it is immutable.
// With 'const' or 'static' member variables, we have to instantiate them in the
// class implementation file (the .cpp file)

const unsigned char SevenSegEventHandler::vseg[13]= {

		// your data goes here.
};

///////////////////////////////////////////////////////////////////////
/// SevenSegEventHandler
///
/// Constructor - set up the hardware
///
/// @scope: PUBLIC, CONSTRUCTOR
/// @param: none
/// @return: none
/// @context: TASK
///
///////////////////////////////////////////////////////////////////////

SevenSegEventHandler::SevenSegEventHandler()
{
	// This is the code that configures the pins on the ATMega328 as
	// outputs WITHOUT CHANGING OTHER PINS (note the use of bitwise-OR)

	// DATA (SER on HC595) is on PORTD bit 4
	// CLK  (SRCLK on HC595) is on PORTB bit 0
	// EN   (ECLK on HC595) is on PORTD bit 7

	DDRD |= 0b10010000;	// set to o/p
	DDRB |= 0b00000001; // set to o/p

	// set EN high, data low, clock low (as initial)

	PORTD |= 0b10000000;		// EN high
	PORTB &= ~0b00000001;		// data pin low
	PORTD &= ~0b00010000;		// cock pin low

	// This line registers the message handler with the OS in order
	// that it can receive messages posted to it under the MSG_ID_CHANGE_7SEG
	// ID

	//Kernel::OS.MessageQueue.Subscribe(MSG_ID_UPDATE_7SEG,this);

	// finally, post a message to ourselves to set the display blank
	// and the d.p. on.

	Kernel::OS.MessageQueue.Post(MSG_ID_UPDATE_7SEG, (void *)0b11101, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
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

void SevenSegEventHandler::EventHandler(int id, void * context)
{
	unsigned int value = (unsigned int)context;

	// The 'value' argument to this function contains the  section below sends messages, once each time the timer expires, to the
	// seven segment driver. It sends an integer where the bottom 8 bits are used, all other
	// bits should be set to zero.

	// The bits 4 to 0 represent the number displayed. If a message with the value of '0' is
	// sent, then '0' is displayed. Values 1-8 display numbers 1-9, the value 9 displays 'b'
	// and the value 10 displays 'E'.
	// Bit 4 represents the value of the decimal point. If set, the decimal point is
	// illuminated. If cleared, the decimal point is cleared.
	//
	// Message value	DP state	Display				Message value	DP state	Display
	//		0x00			Off			0					0x10		On			0
	//		0x01			Off			1					0x11		On			1
	//		0x02			Off			2					0x12		On			2
	//		0x03			Off			3					0x13		On			3
	//		0x04			Off			4					0x14		On			4
	//		0x05			Off			5					0x15		On			5
	//		0x06			Off			6					0x16		On			6
	//		0x07			Off			7					0x17		On			7
	//		0x08			Off			8					0x18		On			8
	//		0x09			Off			9					0x19		On			9
	// 		0x0a			Off			b					0x1a		On			b
	//		0x0b			Off			E					0x1b		On			E
	//
	// If the values of the bits 3-0 are outside of the range 0-0x0a, then the display should be blank
	// The code here sequences through the ranges above

	// your implementation goes here...

}
