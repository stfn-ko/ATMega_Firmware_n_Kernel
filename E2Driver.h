// E2 non-volatile memory driver class. This encapsulates the low-level IIC
// calls to the E2 memory device and provides higher level member functions
// to provide for random read/writes to the memory space.

#ifndef _E2DRIVER_H_
#define _E2DRIVER_H_

#if __cplusplus < 201103L
  #error "This code requires at least C++11"
#endif

#include "kernel.h"

class E2Driver {
    static constexpr uint16_t E2_MEMORY_SIZE = 65535;
    static constexpr unsigned char E2_DEFAULT_IIC_ADDRESS = 0;

  protected:
    const unsigned char E2_IIC_ADDRESS;
    static constexpr uint8_t E2_PAGE_SIZE = 64;
    static constexpr uint16_t E2_LAST_ADDRESS = E2_MEMORY_SIZE / E2_PAGE_SIZE;

    struct DATA_BUFFER
    {
      unsigned char page_address[2];
      char data[E2_PAGE_SIZE];
    };


  public:
    // custom constructor : takes E2 address ->
    // enables multiple EEPROM devices instantiation ->
    //doesnt have unique value checker (might crush on the bus)
    E2Driver(unsigned char _iic_address = E2_DEFAULT_IIC_ADDRESS) : E2_IIC_ADDRESS(0xA0 | _iic_address << 1)
    {
      static_assert(E2_PAGE_SIZE > 0, "E2 page size has to be more than 0");
      static_assert(E2_PAGE_SIZE <= 128, "E2 page size has to be less than 128");
      static_assert(E2_PAGE_SIZE && !(E2_PAGE_SIZE & (E2_PAGE_SIZE - 1)), "E2 page size has to be a power of 2");
    };

    // write to eeprom : takes page address and input data ->
    int memory_write(uint16_t _page_address = 0, const char * _input_data = "empty data input!");
    
    // reads specific page : takes page address
    //int memory_read(uint16_t _page_address = 0, unsigned char* _output_data = (unsigned char*)"error");
    int memory_read(uint16_t _page_address = 0, unsigned char* _output_data  = (unsigned char*)"error", uint8_t _data_length = E2_PAGE_SIZE);
};


#endif
