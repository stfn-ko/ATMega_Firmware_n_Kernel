///////////////////////////////////////////////////////////////////////////////
/// TASK.H
///
/// Virtual base class for Task. This can not be directly implemented
/// and must be derived
///
///////////////////////////////////////////////////////////////////////////////

#ifndef TASK_H_
#define TASK_H_

#include "EventReceiver.h"

namespace Kernel {

	class Task : public EventReceiver {

		public:

			///////////////////////////////////////////////////////////////////////////////
			/// Task
			///
			/// Constructor. This will set up the task, but does not actually register it
			/// (the caller must call Task.Start() to start the task).
			///
			/// @scope: PUBLIC
			/// @context: TASK
			///	@param: NONE
			/// @return: NONE
			///
			///////////////////////////////////////////////////////////////////////////////

			Task() {};

			///////////////////////////////////////////////////////////////////////////////
			/// ~Task
			///
			/// Destructor. Essential in this context as we must be sure we deregister
			/// any running task from the task manager before we delete our resources.
			/// We may have to be careful here of a race condition: derived task
			/// destructors will be called first, and conceivably may delete resources
			/// that are used between the derived class destructor being called, and the
			/// base class destructor being called. However, if this is likely to be an
			/// issue there is no harm in the derived class deregistering the task at that
			/// point
			///
			///////////////////////////////////////////////////////////////////////////////

			virtual ~Task();

			///////////////////////////////////////////////////////////////////////////////
			/// TaskLoop
			///
			/// The main task function. This must be overridden by descendents
			///
			/// @scope: PUBLIC
			/// @context: TASK
			/// @param: NONE
			/// @return: NONE
			///
			///////////////////////////////////////////////////////////////////////////////

			virtual void TaskLoop(void)=0;

			///////////////////////////////////////////////////////////////////////////////
			/// Start
			///
			/// Start the task.
			///
			/// @scope: PUBLIC
			/// @context: TASK
			/// @param: NONE
			/// @return: NONE
			///
			///////////////////////////////////////////////////////////////////////////////

			void Start(void);

	};
}





#endif
