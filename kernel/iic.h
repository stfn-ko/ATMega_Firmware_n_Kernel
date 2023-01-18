///////////////////////////////////////////////////////////////////////////////
/// IIC.H
///
/// IIC peripheral driver for ATMega328p
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _IIC_H_
#define _IIC_H_

///
/// IIC reconstructed as a class. This will be a singleton class that allows
/// access to IIC functions

namespace Kernel {

    class IIC {

        public:

            ///////////////////////////////////////////////////////////////////////////////
            /// IIC
            ///
            /// Initialize the IIC subsystem
            ///
            /// @scope: PUBLIC, CONSTRUCTOR
            /// @context: TASK
            /// @param: NONE
            /// @return: NONE
            ///
            ///////////////////////////////////////////////////////////////////////////////

            IIC();

            ///////////////////////////////////////////////////////////////////////////////
			/// Get
			///
			/// Return the singleton class
			///
			/// @context: ANY
			/// @scope: PUBLIC
			/// @param: none
			/// @return: reference to single instance of static class
			///
			///////////////////////////////////////////////////////////////////////////////

			static IIC& Get(void);
            
            ///////////////////////////////////////////////////////////////////////////////
            /// IICWrite
            ///
            /// Write a single byte of data to the IIC address
            ///
            /// @scope: INTERNAL
            /// @context: TASK
            /// @param: addr - unsigned char. Address. Top 7 bits used
            /// @param: dbyte - unsigned char. Data to send
            ///
            ///////////////////////////////////////////////////////////////////////////////

            int IICWrite(unsigned char addr,unsigned char * dbytes, unsigned int nToSend);

            ///////////////////////////////////////////////////////////////////////////////
            /// IICRead
            ///
            /// Wread multiple bytes of data from the IIC address
            ///
            /// @scope: INTERNAL
            /// @context: TASK
            /// @param: addr - unsigned char. Address. Top 7 bits used
            /// @param: dbytes - unsigned char * Pointer to buffer big enough to receive
            ///                  data
            /// @param: nToRecv - number of bytes to receive
            ///
            ///////////////////////////////////////////////////////////////////////////////

            int IICRead(unsigned char addr,unsigned char * dbytes, unsigned int nToRecv);
    };
}
        
#endif
