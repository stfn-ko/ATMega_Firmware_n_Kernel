#ifndef _PWM_H_
#define _PWM_H_

#include <kernel.h>

class PWM {
	public:
		/// Initialize the PWM subsystem and start it (with a zero pulse width)
		static void Init(void);

		/// Set the pulse width. This is an unsigned 8-bit value from
		/// 0 to 0xff - representing duty cycles from 0 to 1
		static void SetPWM(unsigned char _duty);
};

#endif
