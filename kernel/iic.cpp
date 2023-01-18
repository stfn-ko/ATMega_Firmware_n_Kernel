///////////////////////////////////////////////////////////////////////////////
/// IIC.CPP
///
/// IIC peripheral driver for ATMega328p
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "iic.h"

namespace Kernel
{

    ///////////////////////////////////////////////////////////////////////////////
    /// IIC
    ///
    /// Initialize the IIC subsystem
    ///
    /// @scope: EXPORTED
    /// @context: TASK
    /// @param: NONE
    /// @return: NONE
    ///
    ///////////////////////////////////////////////////////////////////////////////

    IIC::IIC()
    {
        // TWBR=20;		// set bit rate and prescaler
        TWBR = 80;
        TWSR = 0x10;
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

    static IIC &IIC::Get(void)
    {
        static IIC iic;
        return iic;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// IICWrite
    ///
    /// Write a single byte of data to the IIC address
    ///
    /// @scope: EXPORTED
    /// @context: TASK
    /// @param: addr - unsigned char. Address. Top 7 bits used
    /// @param: dbyte - unsigned char. Data to send
    ///
    ///////////////////////////////////////////////////////////////////////////////

    int IIC::IICWrite(unsigned char addr, unsigned char *dbytes, unsigned int nToSend)
    {
        int rc = 0;
        // This is out of the data sheet!
        // Polled I2C write transfer

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA); // send start bit

        while (!(TWCR & (1 << TWINT)))
            ; // wait for ack
        if (TWSR & 0x08)
        { // start bit set
            // send the address, with W bit set to zero
            TWDR = addr & 0xfe;                // load address
            TWCR = (1 << TWINT) | (1 << TWEN); // whang it in
            while (!(TWCR & (1 << TWINT)))
                ; // wait for complete
            if ((TWSR & 0xf8) == 0x18)
            { // check addr ack received
                for (int idx = 0; (idx < nToSend); idx++)
                {
                    TWDR = *(dbytes + idx);            // load byte
                    TWCR = (1 << TWINT) | (1 << TWEN); // whang it in
                    while (!(TWCR & (1 << TWINT)))
                        ; // wait for complete
                    if ((TWSR & 0xf8) != 0x28)
                    { // check for data ack received
                        rc = -2;
                        break;
                    }
                }
            }
            else
            {
                rc = -2;
            }
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // send stop bit
            while (TWCR & (1 << TWSTO))
                ; // wait for it to be cleared
        }
        else
        {
            rc = -1;
        }
        return rc;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// IICRead
    ///
    /// Wread multiple bytes of data from the IIC address
    ///
    /// @scope: EXPORTED
    /// @context: TASK
    /// @param: addr - unsigned char. Address. Top 7 bits used
    /// @param: dbytes - unsigned char * Pointer to buffer big enough to receive
    ///                  data
    /// @param: nToRecv - number of bytes to receive
    ///
    ///////////////////////////////////////////////////////////////////////////////

    int IIC::IICRead(unsigned char addr, unsigned char *dbytes, unsigned int nToRecv)
    {
        int rc = 0;
        // This is out of the data sheet!
        // Polled I2C read transfer

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA); // send start bit
        while (!(TWCR & (1 << TWINT)))
            ; // wait for ack
        if (TWSR & 0x08)
        { // start bit set
            // send the address, with W bit set to 1
            TWDR = addr | 0x01;                // load address
            TWCR = (1 << TWINT) | (1 << TWEN); // whang it in
            while (!(TWCR & (1 << TWINT)))
                ; // wait for complete
            if ((TWSR & 0xf8) == 0x40)
            { // check addr ack received
                for (int idx = 0; idx < nToRecv; idx++)
                {
                    // check if we need ack before receiving.
                    TWCR = (idx == (nToRecv - 1)) ? (1 << TWINT) | (1 << TWEN) : (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
                    while (!(TWCR & (1 << TWINT)))
                        ; // wait for ready
                    if (((TWSR & 0xf8) == 0x50) || ((TWSR & 0xf8) == 0x58))
                    {                           // check for data ack received
                        *(dbytes + idx) = TWDR; // load byte
                    }
                    else
                    {
                        rc = -2;
                        break;
                    }
                }
            }
            else
            {
                rc = -2;
            }
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // send stop bit
            while (TWCR & (1 << TWSTO))
                ; // wait for it to be cleared
        }
        else
        {
            rc = -1;
        }
        return rc;
    }
}
