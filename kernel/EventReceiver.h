///////////////////////////////////////////////////////////////////////////////
/// EventReceiver.H
///
/// Base (virtual) event receiver class. This should be inherited from and the
/// protected member function EventHandler overridden to

#ifndef _EVENTRECEIVER_H_
#define _EVENTRECEIVER_H_

namespace Kernel {

	class EventReceiver {

		friend class MHCLASS; // the internal class used by the MQ needs to access the protected EventHandler member function

		public:

			virtual ~EventReceiver();

		protected:
			virtual void EventHandler(int id, void * context) {}; // implement, but do nothing for the base class

	};

}

#endif
