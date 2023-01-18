///////////////////////////////////////////////////////////////////////////////
/// TASK.H
///
/// Virtual base class for Task. This can not be directly implemented
/// and must be derived
///
///////////////////////////////////////////////////////////////////////////////

#include "kernel.h"

namespace Kernel {

	#include "Task.h"

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
	/// point (except a small performance penalty).
	///
	///////////////////////////////////////////////////////////////////////////////

	Task::~Task()
	{
		Kernel::OS.TaskManager.DeregisterTaskHandler(this);
	}

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

	void Task::Start(void)
	{
		Kernel::OS.TaskManager.RegisterTaskHandler(this);
	}

};
