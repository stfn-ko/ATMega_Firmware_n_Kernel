
#include "EventReceiver.h"

namespace Kernel {

	EventReceiver::~EventReceiver()
	{
		// we must ensure that if we are deleted (or any derived class is deleted), we first
		// deregister ourselves from any message queue.
		//
		// Normally, in an embedded system, message handlers will not be manipulated dynamically and if
		// created on the heap with 'new', they won't be deleted during the life of the program (i.e. until
		// the device is switched off. So this normally won't be called.
	}
}
