///////////////////////////////////////////////////////////////////////////////
/// LogTask.cpp
///
/// Dr J A Gow 2022
///
/// Derived from Task - this uses a timer to write a timestamped log message
/// to the serial port
///
/// This is the implementation
///
///////////////////////////////////////////////////////////////////////////////

#include "LogTask.h"
#include "msgids.h"

// RTC and E2 I2C bus addresses - we use these when initializing the private
// member variables 'rtc' and 'e2'.

#define IIC_ADDR_RTC	0xDE
#define IIC_ADDR_E2  	0xA0

// numbers to help us locate log entries in pages.

#define LOG_MAX_MSG_LEN	(128-sizeof(RTCDriver::RTC_DATE))	// this is the maximum size of a log message
#define LOG_CTRL_PAGE_ADDR	0					// address of the control block
#define LOG_TOP_PAGE_ADDR	(65536-128)			// address of the topmost page in the E2
#define LOG_MAX_ENTRIES     ((65536%128)-1)		// maximum possible number of log entries

// our chosen 'magic number'. This can be anything you want it to be, but should not be 0xffff
// as 0xffff is the likely value that a blank EEPROM will read back.

#define LOG_MAGIC		0xbeef

// maximum size of the internal log queue waiting to be written. This defines the maximum
// size of the linked-list queue of log messages waiting for the FSM to write them to the
// EEPROM.

#define LOG_MAX_MESSAGE_STACK	3

// structure of a single log page in E2. Note the additional member 'nextPage', which is
// a linkage pointer. As new log message requests come in from the firmware, we create a new LOGPAGE
// structure and attach it to the 'nextPage' pointer by setting the 'nextPage' pointer in the current
// structure to the address of the newly created one. This creates a list of structures where we can
// access the next one in the list by dereferencing the 'nextPage' pointer in the current entry. If
// the 'nextPage' pointer is NULL, we have come to the end of the list.
// Note that we do not and must not write the linkage pointer to E2 - this is only for internal RAM use

typedef struct _LOGPAGE {

	RTCDriver::RTC_DATE		dateBlock;
	unsigned char message[LOG_MAX_MSG_LEN];
	struct _LOGPAGE *	nextPage;

} __attribute__((packed)) LOGPAGE;
typedef LOGPAGE *	PLOGPAGE;


///////////////////////////////////////////////////////////////////////
/// LogTask
///
/// Constructor. This is called automatically when an instance of the
/// class is created. It initializes the timer.
///
/// Note here, we don't pass a timeout to the constructor. The timeout
/// is hardcoded in the class. Ask yourself: is this a good idea?
/// Maybe we will need to change the timeout dynamically?
///
/// Actually we may use a different mechanism to trigger a log.
///
////////////////////////////////////////////////////////////////////////

LogTask::LogTask() : rtc(IIC_ADDR_RTC), e2(IIC_ADDR_E2),
		             pHead(NULL), pEnd(NULL), nInList(0), state(INIT),IICFailCount(0),
					 InitReadback(false),NextToRead(0),InitDelete(false)
{
	// register this class instance as a message handler with the kernel.
	// In this function we need to call Kernel::OS.MessageQueue.Subscribe for each
	// message ID we want to respond to.
	//
	// For the 'EventReceiver *' argument of Kernel::OS.MessageQueue.Subscribe, you should
	// pass a pointer to this instance of the class. This can be found by using the
	// implicit 'this' pointer.
	//
	// An example is provided below for the datalog event. You will also need to
	// register to receive the two other events that the datalogger must respond to;
	// the readback event and the erase event - this must be done in this constructor

//	Kernel::OS.MessageQueue.Subscribe(MSG_ID_DATALOG_LOGEVENT,this);
}

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

int LogTask::SetDate(RTC_DATE& date)
{
	return rtc.set_date(date);	// this is all you need to do here.
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

void LogTask::TaskLoop()
{
	int rc;

	// Remember, when a write to the EEPROM is made, the EEPROM device caches the write
	// in a buffer, then once the IIC command is complete, it actually performs the write
	// to the EE array internally, as writes are much slower than reads. During this time,
	// the device will not respond. Thus, if we try to perform two writes one after the
	// other, the second call to Kernel::OS.IICDriver.IICWrite will return a nonzero
	// error code. We must be ready for this. So we use an FSM to allow the state of the
	// EEPROM to be monitored and writes to occur once the device is once again ready.
	//
	// This is the whole reason why we need an FSM here (or similar). We can not simply
	// block, waiting for the EEPROM to finish its write. This would waste CPU cycles
	// and stop other tasks from running (look at the architecture of the task manager).
	//
	// We can use this to determine if sufficient time had elapsed between the first
	// write and the second one
	//

	// Now, whatever state we are in, if the IICFailCount exceeds a given level, lock
	// out the datalogger. We can do this from any state; that is why it is outside
	// the switch block. I would suggest something like this:

	// if(IICFailCount>5) {
	// 	 state=IICFAIL;
	//	 Serial.print("DATALOGGER IIC LOCKED OUT - IICFAILCOUNT EXCEEDED\n");
	// }

	// and the FSM itself

	switch(state) {


		case INIT:		// we load the control block here. We only have to read it once; after this, it
						// is in RAM and we can update on the fly, writing back when it is modded.
						// Saves reading it on each log write

						//
						// So, what we want to do here is this:
						//
						// 1. Read the control block from the fixed address at LOG_CTRL_PAGE_ADDR into
						//    the member variable 'lc', checking the return code from the E2Read call.
						// 2. If the read is successful (return from E2Read is zero):
						//        - check the lc.magic that has been read back.
						//        - if this is NOT equal to LOG_MAGIC:
						//			  - we know we have either a blank E2, or a corrupted one. So:
						//            - set lc.magic to LOG_MAGIC.
						//            - set lc.nEntries to zero, set lc.addrOldest and lc.addrNextFree to
						//              the start page of your log blocks.
						//        - else
						//            - we are ok, so we can transition the FSM to state 'READY'
						//            - set IICFailCount to zero, as we have successfully read the block
						//    else
						// 		  - increment IICFailCount, as we have had an IIC fail, and have the FSM
						//          stay in state INIT.


						break;

		case READY:		// ready to do one of three things:
						// 1. Check InitDelete
						//    If set we want to erase the datalog and start afresh. This is easier than it sounds
						//        - we simply need to reset the control block to its default values, then write it
						//	      - set lc.magic to the magic number
						//        - set lc.nEntries to zero
						//        - set lc.addrOldest and lc.addrNextFree to the start page of your log message pages
						//        - transition the FSM to 'WRITECB'
						//	  else
						//        2. Check InitReadback. If set
						//            - set InitReadback to false
			            //            - set the member variable NextToRead to the address in E2 of the oldest log
			            //            - message; this can be found in lc.addrOldest
				        //            - transition the FSM to 'READBACK'
			            //        else
			            //            we are neither reading back the log or deleting it.
			            //            3. Check if a log message has been queued during this iteration. We can get this
			            //               by checking if pHead is not null, and nInList is nonzero.
			            //               If it has:
						//                   - transition to state 'WRITELOG'
						//
			            // If none of the above conditions apply, stay in the state we are in.

						break;


		case WRITELOG:	// write the log message to the relevant block

						// Here we write the log message to the E2, and update our control block.
						// So, the first thing we need to do is write the LOGPAGE (except the 'nextPage' member)
						// to the EEPROM at the address given by lc.addrNextFree. We have already tested that
						// we have a message to write when we transitioned to this state. So we know that pHead
						// will point to the next message to write.
						// So. the first thing to do is call e2.E2Write with lc.addrNextFree as the E2 address
						// to write, then pHead as the message pointer and the length will be the sum of:
						// sizeof(RTCDATE)+strlen(pHead->message)+1
						// the additional '1' is to accommodate the trailing zero on the string, which is not
						// accounted for by the call to 'strlen'.
						//
						// We test the return code from e2.E2Write:
						// If it is zero (write is successful)
						//    - we need to dequeue the message we have successfully written from the linked-list.
						//      This can be done with the following code - if you look at this line by line it will
						//      make sense:
						//
						//      PLOGPAGE todelete;
						//      todelete=pHead;
						//      pHead=pHead->nextPage;
						//      if(pHead==NULL) {	// if we are the only one in the list
						//    	    pEnd=NULL;      // make sure first entry and last entry are NULL
						//      }
						//      nInList--;			// one less in linked-list
						//      delete todelete;	//.free memory associated with the entry
						//
						//      Having done this, we need to update our control block in RAM, before
						//      writing it. To do this:
						//
						//      Check if lc.nEntries < LOG_MAX_ENTRIES. If so
						//          - increment lc.nEntries
						//
						//      We now need to set lc.addrNextFree to the next free address in E2 to write
						//      the next log message. So add sizeof(LOGPAGE)-sizeof(PLOGPAGE) to the value
						//      of lc.addrNextFree. Note that a LOGPAGE has been declared to be exactly 128
						//      bytes long, with an additional member of nextPage (of type PLOGPAGE, a pointer
						//      to a LOGPAGE). We do not want to write the 'nextPage' pointer to the E2, as this
						//      is only used internally - thus to get the exact page size we must subtract the
						//      length of the PLOGPAGE from the sizeof(LOGPAGE).
						//
						//      Incrementing addrNextFree, eventually, after we fill up the E2, will result in a value
						//      that exceeds the length of the E2. In this case we must wrap it around. So then:
						//
						//      Check if lc.addrNextFree > LOG_TOP_PAGE_ADDR. If so
						//          - set lc.addrNextFree to the first page. If we put our control block in
						//            page 0, this will be 0x80. But you could put your control block in the last
						//            page and adjust these accordingly - this is up to you.
						//
						//      Check if lc.addrNextFree == lc.addrOldest. If so
						//          - add sizeof(LOGPAGE)-sizeof(PLOGPAGE) to lc.addrOldest.
						//
						//      Check if lc.addrOldest > LOG_TOP_PAGE_ADDR. If so
						//          - set lc.addrOldest to the first page. If we put our control block in
						//            page 0, this will be 0x80. But you could put your control block in the last
						//            page and adjust these accordingly - this is up to you.
						//
						//		Set the IICFailCount to zero, as we have had a successful page write
						//      Transition the FSM to 'WRITECB'
						//
						// else
						//    - we have a failed write. Increment IICFailCount by 1 and stay in this state.
						//      This will automatically re-try this write on the next pass.

						break;



		case WRITECB:	// Here we write the control block. Now this may fail as the IIC memory
						// may not have completed its write from the state WRITELOG. However
						// we want to let the CPU do other things while we wait for it (takes about
						// 4us). So we poke it. If it does not respond, we increment IICFailCount and
			            // try again on the next pass. Eventually it will respond, and we can return to READY.
			            // If it never responds (a hardware fault) we transition to the error state earlier on
						// in the code before the switch statement.
			            //
						// So, we:
						//
						// call e2.E2Write with the address of the control block in EEPROM (LOG_CTRL_PAGE_ADDR)
						// as the address, the address of 'lc' (cast to an unsigned char *) as the data to write
						// and the sizeof(LOGCONTROL) as the length - this is the size of the control block.
						//
						// Test the return code:
						// If zero (success):
						//    - set IICFailCount to zero (no failure)
						//    - transition FSM to 'READY'.
						// else
						//    - increment IICFailCount.
						//
						break;



		case READBACK:	// We come here if there has been a request to read back the entire log.
						// While we are in this state, writes to the EEPROM are stopped, (but we can still
						// queue new log messages in the linked-list to be written once the readback is complete
			            // and the FSM has returned to 'READY'.
						// The pages are read back one at a time on each iteration of the state machine
			            // to avoid hogging the CPU.

						// No writes can take place here, so we can use the addresses in the control
						// block to read back the log, earliest first:

						// NextToRead will start at the oldest - we keep going until all messages
						// are read. We use NextToRead here as it is persistent over iterations of this
			            // state machine.
						//
			            // So: first create a local variable
			            // LOGPAGE lp;
			            // to temporarily store the log page read back from the EEPROM.
						//
						// Then call e2.E2Read with NextToRead as the EEPROM address, the address of 'lp' cast to
						// an unsigned char * as the data block address, and sizeof(LOGPAGE)-sizeof(PLOGPAGE) as the
						// length. We are not using the nextPage member of LOGPAGE here.
						// Then test the return code from e2.E2Read:
						// If zero (success)
						//     - simply call LogPageToSerial from this class, with the address of 'lp' as the argument.
						//       This will dump the log message to the serial port.
						//     - Increment NextToRead
						//     - If NextToRead==lc.addrNextFree:
						//         - we have read back the whole log, so transition the FSM to state 'READY'
						//     - else
						//         - check we need to wrap NextToRead.
						//         - if NextToRead>LOG_TOP_PAGE_ADDR
						//             - set NextToRead=0x80 (or wherever you have put your first log page)
						// else
						//     - the E2 read has failed. Increment IICFailCount by one.
						//


		case IICFAIL:	// there is a massive failure of the IIC bus - this is a locked state
						// requiring a powercycle to get out of!
						// We actually need no code at all here as we never transition out of
						// the state.

						break;
	}

}

////////////////////////////////////////////////////////////////////////
/// CreateLogEntry
///
/// Create a timestamped log string and write it to the log, with a
/// copy to the serial prot. We use the Arduino string class here
/// as it has some useful internal features with regard to chopping,
/// hacking and measuring.
///
/// @scope: PRIVATE
/// @context: TASK
/// @param: message string (<=127 characters) to write to the log
/// @return: none
///
///////////////////////////////////////////////////////////////////////

int LogTask::CreateLogEntry(String * message)
{
	RTC_DATE date;
	int rc=-1;
	LOGPAGE	   * lp;

	// Now create the new log entry on the heap

	// First check we are not exceeding the maximum number of messages
	// we can queue.

	if(nInList<LOG_MAX_MESSAGE_STACK) {

		// create a new LOGPAGE on the heap (it must be valid outside
		// this function) and set the nextPage pointer to NULL

		lp=new LOGPAGE;

		// check we have actually got the memory - if we have run
		// out of RAM this will return NULL, and we can do nothing
		// with it. If the pointer is valid, it will not be NULL

		if(lp!=NULL) {

			// we are ok.
			// make sure that the nextPage member of our new structure is not NULL

			lp->nextPage=NULL;	// let's not leave floating pointers.

			// from the RTC driver, get the timestamp in the dateBlock of our new
			// LOGPAGE structure

			if((rc=rtc.get_date(lp->dateBlock))==0) {

				// we now have the date, add in the message:
				// Copy the message from the 'String' passed in via the message
				// queue, to the char array in lp->message up to a maximum of the
				// message size. As our pages are 128 bytes long, if we have one
				// log message per page, we have 128-sizeof(RTCDATE) available
				// for our string, that is passed in from the code that logs the
				// event.

				message->toCharArray(lp->message,LOG_MAX_MSG_LEN);

				// write the log to the serial port. This is only for debugging
				// purposes and these lines can be removed when things have been
				// proven to work.

				LogPageToSerial(lp);

				// attach it to our internal queue so that the FSM can pick it
				// up to write it to the E2.

				if(pHead==NULL) {
					pHead=lp;
					pEnd=lp;
				} else {
					pEnd->nextPage=lp;
					pEnd=lp;
				}
				nInList++;

				rc=0;	// we're done, successfully

			} else {
				// an IIC error prevents us from reading the RTC
				// delete the message block as we can't fill it. rc is already
				// set to a nonzero value,
				delete(lp);
			}
		}
	}
	return rc;
}

///////////////////////////////////////////////////////////////////////
/// LogPageToSerial
///
/// Accept an argument of a LOGPAGE and dump the contents to
/// the serial port
///
/// @scope: PRIVATE
/// @context: TASK
/// @param: LOGPAGE *
/// @return: none
///
///////////////////////////////////////////////////////////////////////

void LogTask::LogPageToSerial(PLOGPAGE page)
{
	// You should already have the code to do this - it may need
	// some tweaks. All you need to do here is take the argument 'page', which
	// is a pointer to a LOGPAGE (i.e. members are accessed through page-> syntax
	// rather than page. syntax) and print the log message to the serial port.
	// This function should not change the contents of the structure pointed
	// to by 'page'
}

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

void LogTask::EventHandler(int id, void * context)
{
	// This class receives messages from the message queue. This, (apart from
	// SetDate) is the primary way in which the firmware communicates with
	// the datalogger.
	//
	// There are three messages (these are defined in msgids.h and could
	// be anything you like, as long as they are unique.
	//
	// MSG_ID_DATALOG_LOGEVENT: This can be sent to instruct the datalogger
	// module to log an event. In the payload is a string: this could be
	// a char array, or if you prefer you could use an Arduino 'String'
	// class. Note, if you choose to use an Arduino 'String' class to pass
	// the message, you will need to use static_cast<String *> on the
	// void pointer passed in the 'context' argument to this function
	// in order to retrieve the String pointer. You should then delete
	// the memory associated with the payload here, once it has been used.
	//
	// MSG_ID_DATALOG_DUMPLOG: This has a null 'context'. It sets a flag to
	// instruct the FSM to read back the datalog to the serial port.
	//
	// MSG_ID_DATALOG_DELETELOG: This has a null 'context'. It is rarely
	// used, except in debugging. It sets a flag to tell the FSM to
	// erase the control block, thus causing the datalogger to work
	// as if the EEPROM was blank, deleting all existing log entries
	// and starting from scratch.

	switch(id) {

		case MSG_ID_DATALOG_LOGEVENT:	{
											String * str=static_cast<String *>(context);
											CreateLogEntry(str);
											delete str;
										}
										break;

		case MSG_ID_DATALOG_DUMPLOG:	InitReadback=true;
										break;

		case MSG_ID_DATALOG_DELETELOG:	InitDelete=true;
										break;

		default:						break; 		// we shouldn't get here.

	}
}
