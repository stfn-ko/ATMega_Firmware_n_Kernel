///////////////////////////////////////////////////////////////////////////////
/// taskring.cpp
///
/// Simple task scheduler for single-tasking applications
///
/// Dr J A Gow 2020
///
///////////////////////////////////////////////////////////////////////////////

#include "taskring.h"
#include <stdlib.h>

#define TR_TASKTYPE_FCTN	0
#define TR_TASKTYPE_CLASS	1

//
// task ring internals

namespace Kernel {

	// Task state structure

	typedef class TASKSTATE * 	PTASKSTATE;
	class TASKSTATE {
		public:
			void *				handler;
			uint8_t				type;
			union {
				PFNTASKHANDLER		fHandler;
			} pointers;
			void *				context;
			PTASKSTATE			pNext;
			TASKSTATE() : pNext(NULL) {};
			virtual ~TASKSTATE() {};
			virtual void Call()=0;
	};

	typedef class TASKSTATE_C	PTASKSTATE_C;
	class TASKSTATE_C : public TASKSTATE {
		public:
			Task *				Handler;
			TASKSTATE_C(Task * handler) : Handler(handler) {};
			void Call() { Handler->TaskLoop(); };
	};

	typedef class TASKSTATE_F	PTASKSTATE_F;
	class TASKSTATE_F : public TASKSTATE {
		public:
			PFNTASKHANDLER	Handler;
			void *			context;
			TASKSTATE_F(PFNTASKHANDLER handler, void * context) : Handler(handler),context(context) {};
			void Call() { Handler(context); };
	};

	// Task internal structure

	typedef class TASKINTERNALS *	PTASKINTERNALS;
	class TASKINTERNALS {
		public:
			PTASKSTATE	pHead;
			PTASKSTATE	pCur;
			TASKINTERNALS() : pHead(NULL),pCur(NULL) {};
	};

	///////////////////////////////////////////////////////////////////////////////
	/// TASKRing
	///
	/// CONSTRUCTOR, PRIVATE
	///
	/// Initialize the task handler subsystem. This class should be a singleton
	/// within the kernel.
	///
	/// @scope: 	EXPORTED
	/// @context: 	TASK
	/// @param:  	none
	/// @return:	zero for success.
	///
	///////////////////////////////////////////////////////////////////////////////

	TaskRing::TaskRing(void)
	{
		this->internals=new TASKINTERNALS;
	}

	///////////////////////////////////////////////////////////////////////////////
	/// Get
	///
	/// Obtain the singleton class instance
	///
	/// @scope: PUBLIC
	/// @context: ANY
	/// @param: none
	/// @return: reference to singleton class
	///
	//////////////////////////////////////////////////////////////////////////////

	static TaskRing& TaskRing::Get(void)
	{
		static TaskRing tr;
		return tr;
	}

	///////////////////////////////////////////////////////////////////////////////
	/// Loop
	///
	/// Called by the kernel at task time to sequentially call the handlers.
	///
	/// @scope:	  EXPORTED
	/// @context: TASK
	/// @param:   none
	///
	///////////////////////////////////////////////////////////////////////////////

	void TaskRing::Loop(void)
	{
		PTASKINTERNALS internal=(PTASKINTERNALS)(this->internals);

		if(internal->pCur==NULL) {
			internal->pCur=internal->pHead;
		}

		if(internal->pCur) {
			internal->pCur->Call();					// dispatch to the task handler
			internal->pCur=internal->pCur->pNext;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	/// TASKRegisterTaskHandler
	///
	/// Passed a pointer to a function, and a pointer to some context data,
	/// will register this function to be called by the scheduler at task time. At
	/// all times the context data is 'owned' by the caller
	/// Can block, but will block other tasks. Should not be called in interrupt
	/// context.
	///
	/// @scope:   EXPORTED
	/// @context: TASK
	/// @param:   PFNHANDLER pfnHandler
	/// @param:   (void *) context
	///
	///////////////////////////////////////////////////////////////////////////////

	int TaskRing::RegisterTaskHandler(PFNTASKHANDLER handler, void * context)
	{
		int rc=-1;
		if(handler) {
			PTASKSTATE pNew = new TASKSTATE_F((void *)handler,context);
			PTASKINTERNALS internal=(PTASKINTERNALS)(this->internals);
			if(pNew) {
				pNew->pNext=internal->pHead;
				internal->pHead=pNew;
				rc=0;
			}
		}
		return rc;
	}

	///////////////////////////////////////////////////////////////////////////////
	/// RegisterTaskHandler (task class)
	///
	/// Passed a pointer to a task class, will register this function to be
	/// called by the scheduler at task time. In this call, the context should be
	/// part of a task derived from class Task and the context ptr is not used
	///
	/// @scope: EXPORTED
	/// @context: TASK
	/// @param:   Task * task
	///
	///////////////////////////////////////////////////////////////////////////////

	int TaskRing::RegisterTaskHandler(Task * task)
	{
		int rc=-1;
		if(task) {
			PTASKSTATE pNew = new TASKSTATE_C(task);
			PTASKINTERNALS internal=(PTASKINTERNALS)(this->internals);
			if(pNew) {
				pNew->pNext=internal->pHead;
				internal->pHead=pNew;
				rc=0;
			}
		}
		return rc;
	}

	///////////////////////////////////////////////////////////////////////////////
	/// DeregisterTaskHandler
	///
	/// Passed a pointer to a task class, will deregister this task from the task
	/// manager. Essential if Task classes are dynamically destroyed.
	///
	///////////////////////////////////////////////////////////////////////////////

	int TaskRing::DeregisterTaskHandler(Task * task)
	{

	}

}
