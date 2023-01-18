///////////////////////////////////////////////////////////////////////////////
/// SevenSegEventHandler.cpp
///
/// This class, inherits from EventReceiver and is used to update
/// the seven-segment display according to the specification.
///
/// It hooks the message queue to receive just one message ID.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _SEVENSEGEVENTHANDLER_H_
#define _SEVENSEGEVENTHANDLER_H_


#include "kernel.h"

class SevenSegEventHandler : public Kernel::EventReceiver {

	private:
			// private variables and member functions go here, if needed

			// You will likely need a mapping table, to map a given message to the
			// actual state of the segments. You can declare this as 'const' to ensure
			// it is stored in flash, not RAM. An example of such a declaration is below.
			//
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
			// We deal with the d.p. in software so we only need a table 11 deep.

			// We declare it static, as it is a read-only table and thus we only need a
			// single instance of this, irrespective of how many instances of the actual class
			// we have.

			const static unsigned char vseg[13];
	public:

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

		SevenSegEventHandler();

protected:

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
