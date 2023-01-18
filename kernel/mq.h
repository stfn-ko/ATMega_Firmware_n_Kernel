///////////////////////////////////////////////////////////////////////////////
/// mq.h
///
/// Task message queuing system
///
/// Mechanism to allow IPC between interrupt and task contexts, or task and
/// task contexts.
///
/// Dr J A Gow 2020
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _MQ_H_
#define _MQ_H_

#include "sysincs.h"
#include "EventReceiver.h"

namespace Kernel {

	#define MSG_ID_NOMESSAGE			-1		// used to indicate a null message
	#define MSG_MAX_MSG_IDS				26

	//
	// context enum

	typedef enum MQCONTEXT {
		MQ_CONTEXT_TASK,
		MQ_CONTEXT_INTERRUPT
	};

	//
	// ownership enum

	typedef enum MQOWNER {
		MQ_OWNER_CALLER,
		MQ_OWNER_MQ
	};

	//
	// Prototype of message handler callback function for function-based task handlers

	typedef void (* PFNMSGHANDLER)(void * context);

	//
	// Message queue class

	class MQClass {

		private:

			friend void ::loop();		// the kernel needs to access the Loop function

			/// our internals.

			void *	internals;

			//////////////////////////////////////////////////////////////////////////////
			/// MQClass
			///
			/// CONSTRUCTOR, PRIVATE
			///
			/// Initialize the message queue block
			///
			/// @context: TASK
			/// @scope:   EXPORTED
			/// @param:   none
			///
			//////////////////////////////////////////////////////////////////////////////

			MQClass(void);

			//////////////////////////////////////////////////////////////////////////////
			/// Loop
			///
			/// Called by the task handler. Processes up to MaxMessages and returns
			///
			/// @context:	TASK
			/// @scope:     EXPORTED
			/// @param:     int MaxMessages -  maximum number of messages to process in
			///             this iteration
			/// @return:    none
			///
			//////////////////////////////////////////////////////////////////////////////

			void Loop(int MaxMessages);

		public:

			//////////////////////////////////////////////////////////////////////////////
			/// Get
			///
			/// Returns a reference to the singleton class.
			///
			/// @context: ANY
			/// @scope: PUBLIC, STATIC
			/// @param: none
			/// @return: MQClass&
			///
			//////////////////////////////////////////////////////////////////////////////

			static MQClass& Get(void);

			//////////////////////////////////////////////////////////////////////////////
			/// ~MQClass
			///
			/// DESTRUCTOR
			///
			/// This class is a singleton, part of the kernel, so this function is
			/// not normally called
			///
			//////////////////////////////////////////////////////////////////////////////

			~MQClass();

			//////////////////////////////////////////////////////////////////////////////
			/// Subscribe
			///
			/// Subscribe a static function to a given message. Pass address of handler
			/// function to be called.
			///
			/// @context:	TASK
			/// @scope:     EXPORTED
			/// @param:     PFNMSGHANDLER handler - pointer to the handler function
			/// @param:     void * context - pointer to context data
			/// @param:     boolean CallerOwns - set TRUE if the message queue is not to
			///	            free the context data when done.
			/// @return:	zero if successfully queued, nonzero if error occurred
			///
			//////////////////////////////////////////////////////////////////////////////

			int Subscribe(int msgid, PFNMSGHANDLER handler);

			//////////////////////////////////////////////////////////////////////////////
			/// Subscribe
			///
			/// Subscribe an EventHandler (or descendent) class to a given message. Pass
			/// address of class instance to be called.
			///
			/// @context:	TASK
			/// @scope:     EXPORTED
			/// @param:     PFNERHANDLER handler - pointer to a member function handler
			/// @param:     void * context - pointer to context data
			/// @param:     boolean CallerOwns - set TRUE if the message queue is not to
			///	            free the context data when done.
			/// @return:	zero if successfully queued, nonzero if error occurred
			///
			//////////////////////////////////////////////////////////////////////////////

			int Subscribe(int msgid, EventReceiver * handler);

			//////////////////////////////////////////////////////////////////////////////
			/// Unsubscribe
			///
			/// Unsubscribe a static function from a given message. Pass address of handler
			/// function to be unsusbscribed.
			///
			/// @context:	TASK
			/// @scope:     EXPORTED
			/// @param:     msgid - message id to unsubscribe
			/// @param:     PFNMSGHANDLER handler - pointer to the handler function
			/// @return:	zero if successfully queued, nonzero if error occurred
			///
			//////////////////////////////////////////////////////////////////////////////

			int Unsubscribe(int msgid, PFNMSGHANDLER handler);

			//////////////////////////////////////////////////////////////////////////////
			/// Unsubscribe
			///
			/// Unsubscribe an EventHandler (or descendent) class from a given message.
			/// Pass address of class to be unsusbscribed.
			///
			/// @context:	TASK
			/// @scope:     EXPORTED
			/// @param:     msgid - message id to unsubscribe
			/// @param:     PFNMSGHANDLER handler - pointer to the handler function
			/// @return:	zero if successfully queued, nonzero if error occurred
			///
			//////////////////////////////////////////////////////////////////////////////

			int Unsubscribe(int msgid, EventReceiver * handler);

			//////////////////////////////////////////////////////////////////////////////
			/// UnsubscribeAllIDs
			///
			/// Unsubscribe an EventHandler (or descendent) class from all messages
			/// Pass address of class to be unsusbscribed. This is called by the event
			/// handler class destructors so we don't send events off into wonderland
			/// when a handler is deleted
			///
			/// @context:	TASK
			/// @scope:     EXPORTED
			/// @param:     PFNMSGHANDLER handler - pointer to the handler function
			/// @return:	zero if successfully queued, nonzero if error occurred
			///
			//////////////////////////////////////////////////////////////////////////////

			int UnsubscribeAllIDs(EventReceiver * handler);

			//////////////////////////////////////////////////////////////////////////////
			/// Post
			///
			/// Post a message. Pass address of handler function to be
			/// called.
			///
			/// @context:	TASK, INTERRUPT
			/// @scope:     EXPORTED
			/// @param:     int msgid
			/// @param:     void * context - pointer to context data
			/// @param:     boolean CallerOwns - set TRUE if the message queue is not to
			///	            free the context data when done.
			/// @return:	zero if successfully posted, nonzero if error occurred
			///
			//////////////////////////////////////////////////////////////////////////////

			int Post(int msgid, void * context, MQOWNER CallerOwns, MQCONTEXT isIntCtx);

	};
} // namespace Kernel

#endif
