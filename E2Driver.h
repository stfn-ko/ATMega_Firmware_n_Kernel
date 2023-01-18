///////////////////////////////////////////////////////////////////////////////
/// E2Driver.h
///
/// E2 non-volatile memory driver class. This encapsulates the low-level IIC
/// calls to the E2 memory device and provides higher level member functions
/// to provide for random read/writes to the memory space.
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _E2DRIVER_H_
#define _E2DRIVER_H_

#include "kernel.h"
//#include <cstring>

class E2Driver {
    static const unsigned char E2_DEFAULT_IIC_ADDRESS = 0;

    struct DATA_BUFFER
    {
      unsigned char page_address[2];
      char data[32];
    };

  public: //TODO: make protected 
    const unsigned char E2_IIC_ADDRESS;
    const uint16_t E2_LAST_ADDRESS = 2047; // the address 2047 is reserved for the control block

    // custom constructor : takes E2 address ->
    // enables multiple EEPROM devices instantiation ->
    //doesnt have unique value checker (might crush on the bus)
    E2Driver(unsigned char _iic_address = E2_DEFAULT_IIC_ADDRESS) : E2_IIC_ADDRESS(0xA0 | _iic_address << 1) {};

    // write to eeprom : takes page address and input data ->
    int memory_write(uint16_t _page_address = 0, const char * _input_data = "empty data input!");

    // reads specific 32B page : takes page address
    int memory_read(uint16_t _page_address = 0);

    // reads from any address to any address on the E2: takes start address and end address of the read ->
    //int memory_read_from_to(uint16_t _start_address = 0, uint16_t _end_address = 1);

    // reads all pages at once ->
    int memory_read_all();
};


#endif
