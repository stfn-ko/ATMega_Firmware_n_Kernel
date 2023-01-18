///////////////////////////////////////////////////////////////////////////////
/// LogTask.h
///
/// Dr J A Gow 2022
///
/// Derived from Task - this uses a timer to write the RTC value to the
/// display at periodic intervals
///
///////////////////////////////////////////////////////////////////////////////

#ifndef LOGTASK_H_
#define LOGTASK_H_

#include "kernel.h"
#include "RTCDriver.h"
#include "E2Driver.h"

///
/// A lot of the thinking behind this is how we lay out the log in the E2. We have
/// a 65536-byte memory that is linearly readable, but 128 byte page-writable. If we
/// have writes that span pages and do not occupy an integral number of pages,
/// we would end up with the writes being quite time-consuming and inefficient.
/// We could put the date in one page and the log message in the next, but the date
/// will not fill a whole page and this would be woefully inefficient.
///
/// The E2 driver will handle cross-page writes, but this will require looping and
/// thus cost additional time in the log write, or you would have to use an
/// FSM and an additional task to ensure that the write completed in the background
///
/// So let's limit the length of each datalog entry to 128 bytes. We will store
/// the date in binary form (to limit length) and the log message immediately beyond
/// this. This will then form one page. While the log message may not fill up the
/// entire page (meaning we do not use all the memory), the time penalty for the
/// otherwise resulting writes on a non-page boundary will compensate. As always
/// it is swings and roundabouts.
///
/// We also need a control page to store the pointers to the next free log entry
/// slot. Now here we have to be careful about wear. The control page will likely
/// be present at the same address in memory, and be written once as the control
/// information is updated. Thus this page will wear more quickly than the pages
/// that are rotated. This could lead to premature failure of the E2. However,
/// attempts to minimize this would lead to a need to scan the memory on startup-
/// leading to longer initialization times. So we take a punt here. The spec does
/// not mention wear, so as this is a proof of concept we will not worry about
/// this here (perhaps in a later improved version). However, you may think
/// about this differently!
///
/// However, since we need to write the control page after every log message,
/// we need an FSM to allow the write to take place in the background (remember
/// in a multitasking environment you can't block. So we implement this logger
/// as a Task, which allows the FSM to run in the background

// forward declarations

typedef struct _LOGPAGE LOGPAGE;
typedef LOGPAGE * PLOGPAGE;

// Our logger class, derived from Task as we do our logging in the background

class LogTask : public Kernel::Task, public RTCDriver{

	private:

		// This defines the states of our logger state variable. We declare
		// it here as we only ever use this declaration in instances of this
		// class. Note that this typedef defines an enum _type_ only, the
		// actual instantiation of the state variable is later.

		typedef enum _STATE {
			INIT,				// initializing
			READY,				// ready to either receive log messages, or readback
			WRITELOG,			// write the log message
			WRITECB,			// write the control block
			READBACK,			// read back the log
			IICFAIL				// hard error state if the IIC drivers persistently fail
		} LOGSTATE;

		// This is a typical control block. This is what we
		// store in a defined address of our E2, and where the logger function can always
		// find it. It stores a magic number (so we can tell if the E2 is blank or if
		// a valid control block has been written previously),
		// the total number of entries present in the log (so if we only have a
		// half-written E2 when the power fails we know how many log messages
		// we can read back), the address of the oldest entry in the log, and the
		// address of the next free entry.

		typedef struct _LOGCONTROL {
			unsigned int	magic;			// are we a valid block?
			unsigned int	nEntries;		// number of entries in log
			unsigned int	addrOldest;		// oldest entry
			unsigned int 	addrNextFree;	// next free entry
		} __attribute__((packed)) LOGCONTROL;
		typedef LOGCONTROL *	PLOGCONTROL;

		// Note here, the object 'owns' the RTC. (i.e. we declare an instance of the
		// RTCDriver in this class. Is this a good idea? In this case, yes - only if
		// the object is the only object that will use this instance of the RTC, which in
		// our case, it is.
		//
		// In our application, only the datalogger uses the RTC, so this is justified.

		RTCDriver		rtc;		// we have an instance of our RTCDriver
		E2Driver		e2;			// we have an instance of our E2.
		LOGSTATE		state;		// THIS IS THE STATE VARIABLE FOR OUR FSM
		LOGCONTROL 		lc;			// This is the RAM copy of our control block.

		// These are the pointers to the head and end of a linked-list containing
		// the log messages sent to the FSM. This allows us to be able to capture
		// a log message sent while the FSM is still processing the previous log.
		// Again, this is not the only way to achieve the specification - in this
		// example I would accept a system that did not use a linked list, but simply
		// would reject a log message if sent while the FSM was processing the first
		// (as we can ensure in our firmware that requests to log a message do not
		// come in one after the other in short order. However, a more generic
		// fault-tolerant datalogger would need some method of dealing with this.
		//
		// The linked-list is essentially used as a FIFO queue (First In First Out)

		PLOGPAGE		pHead;		// first entry in list
		PLOGPAGE		pEnd;		// last entry in list
		int				nInList;

		// These are variables we can use in our state machine.

		int				IICFailCount;
		unsigned int	NextToRead;

		// These last two are flags that allow us to pass conditions from the
		// message queue handler to the state machine

		boolean			InitReadback;	// set if we want to read back the log
		boolean			InitDelete;		// set if we want to delete the entire log

		// note these private functions - we can only 'see' these functions
		// and call them from within other functions that make up this class.
		// We can not call functions in the 'private:' section from outside
		// the class. Therefore this is the place to put 'helper' functions
		// that we use to perform specific tasks internally within the class.

		////////////////////////////////////////////////////////////////////////
		/// CreateLogEntry
		///
		/// This private function is called from the incoming message handler
		/// function. It timestamps the log message and attaches it to the
		/// internal queue for the FSM to pick up.
		///
		/// @scope: PRIVATE
		/// @context: TASK
		/// @param: message string (<=127 characters) to write to the log
		/// @return: none
		///
		///////////////////////////////////////////////////////////////////////

		int CreateLogEntry(String * message);

		///////////////////////////////////////////////////////////////////////
		/// LogPageToSerial
		///
		/// Accept an argument of a PLOGPAGE and dump the contents to
		/// the serial port. This is used for during the readback - allowing
		/// us to read back one message at a time to the serial port.
		///
		/// @scope: PRIVATE
		/// @context: TASK
		/// @param: PLOGPAGE
		/// @return: none
		///
		///////////////////////////////////////////////////////////////////////

		void LogPageToSerial(PLOGPAGE page);

	public:

		////////////////////////////////////////////////////////////////////////
		/// LogTask
		///
		/// Constructor. This is called automatically when an instance of the
		/// class is created. It registers the class with the message
		/// queue.
		///
		/// @scope: PUBLIC, CONSTRUCTOR
		/// @context: TASK
		/// @param: NONE
		/// @return: NONE
		///
		////////////////////////////////////////////////////////////////////////

		LogTask();

		////////////////////////////////////////////////////////////////////////
		/// SetDate
		///
		/// This function can be called to set the date and time on the I2C
		/// RTC internal colck. This is just a passthrough to the specific
		/// instance of the RTC that the datalogger is using. Such a function
		/// is known as an 'accessor function' - it allows code external to the
		/// class to manipulate a private member variable in a controlled manner
		///
		/// @scope: PUBLIC
		/// @context: TASK
		/// @param: NONE
		/// @return: 0 success, nonzero error
		///
		////////////////////////////////////////////////////////////////////////

		int SetDate(RTC_DATE& date);

	protected:

		////////////////////////////////////////////////////////////////////////
		/// TaskLoop
		///
		/// Main task loop - this is overloaded from class Task and contains
		/// the FSM core.
		///
		/// @scope: PROTECTED
		/// @context: TASK
		/// @param: none
		/// @return: none
		///
		////////////////////////////////////////////////////////////////////////

		virtual void TaskLoop();

		////////////////////////////////////////////////////////////////////////
		/// EventHandler
		///
		/// This is overloaded from the base class EventHandler, and
		/// will deal with the posted events coming in
		///
		/// @scope: PROTECTED
		/// @context: TASK
		/// @param: int id - the id of the message being received
		/// @param: void * context: this will point to the payload in the
		///		    message - for MSG_ID_DATALOG_DELETELOG and
		///         MSG_ID_DATALOG_DUMPLOG, this is NULL. For
		///         MSG_ID_DATALOG_LOGEVENT will be a pointer to a Sting object
		///			- this object is 'owned' by the message queue and we thus
		///         do not need to free it off.
		///
		////////////////////////////////////////////////////////////////////////

		virtual void EventHandler(int id, void * context);
};

#endif
