#include "E2Driver.h"


int E2Driver::memory_write(uint16_t _page_address, const uint8_t *_input_data, uint8_t _data_length)
{
  if (!_data_length || (_page_address > this->E2_LAST_ADDRESS) || (_data_length > this->E2_PAGE_SIZE))
    return -1;

  DATA_BUFFER load;

  load.page_address[0] = (_page_address * this->E2_PAGE_SIZE) >> 8;
  load.page_address[1] = _page_address * this->E2_PAGE_SIZE;

  memcpy(load.data, _input_data, _data_length);
  return Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, (unsigned char *)&load, _data_length + 2);
}


int E2Driver::memory_read(uint16_t _page_address, uint8_t* _output_data, uint8_t _data_length)
{
  if (!_data_length || (_page_address > this->E2_LAST_ADDRESS) || (_data_length > this->E2_PAGE_SIZE))
    return -1;

  char address[2] = {(_page_address * this->E2_PAGE_SIZE) >> 8, _page_address * this->E2_PAGE_SIZE};

  if (Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, address, 2))
    return -1;
  if (Kernel::OS.IICDriver.IICRead(this->E2_IIC_ADDRESS, (unsigned char *)_output_data, _data_length))
    return -1;
    
  return 0;
}
