///////////////////////////////////////////////////////////////////////////////
/// CONTROL.H
///
/// Main control task. This performs task-time async operations
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef CONTROL_H_
#define CONTROL_H_

#include <kernel.h>


//
// RPS_MAX and RPS_MIN are defined here. We use them here as well
// as in the display module.

#define RPS_MAX	300
#define RPS_MIN	20

// Our control task class, derived from Task

class Control : public Kernel::Task {

	private:

		Kernel::OSTimer	tm;	 		// our display and log event timer
		unsigned int tmcount;		// a counter to give us the 1 second log.
		unsigned int demandRPS;		// the value of RPS asked for
		unsigned int actualRPS;		// the actual value of RPS

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

		void SendLogMessage(unsigned int DemandRPS, unsigned int ActualRPS);


	public:

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

		Control();

	protected:

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

		virtual void TaskLoop();

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
