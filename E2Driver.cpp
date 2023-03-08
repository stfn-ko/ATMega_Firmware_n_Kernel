#include "E2Driver.h"

// doesn't need data length parameter, because writes are fixed size
int E2Driver::memory_write(uint16_t _page_address, const char * _input_data)
{
  if (_page_address > this->E2_LAST_ADDRESS)
    return -1;

  // check for input size to avoid buffer overflow,
  // return an error in case _input_data size is bigger than PAGE_SIZE
  if (_input_data[-1] == '\0' && strlen(_input_data) > this->E2_PAGE_SIZE)
    return -1;
  else if (sizeof(_input_data) >  this->E2_PAGE_SIZE)
    return -1;


  DATA_BUFFER load;

  // automatically translates page address into
  // accepted by E2 0-65535 page address
  // prevents from unwanted data overwrites ->
  // due to page boundary limits exess
  load.page_address[0] = (_page_address * this->E2_PAGE_SIZE) >> 8;
  load.page_address[1] = _page_address * this->E2_PAGE_SIZE;

  // made safe from buffer overflow ->
  // made safe from undefinded behaviour (double null-terminator) ->
  // null-terminated regardless
  strncpy(load.data, _input_data, this->E2_PAGE_SIZE);
  load.data[this->E2_PAGE_SIZE] = '\0';

  return Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, (unsigned char *)&load, this->E2_PAGE_SIZE + 2);
}


int E2Driver::memory_read(uint16_t _page_address, unsigned char* _output_data, uint8_t _data_length)
{
  if (_page_address > this->E2_LAST_ADDRESS)
    return -1;
  else if (!_data_length)
    return -1;
  else if ( _data_length > this->E2_PAGE_SIZE)
    _data_length = this->E2_PAGE_SIZE;

  DATA_BUFFER load;

  load.page_address[0] = (_page_address * this->E2_PAGE_SIZE) >> 8;
  load.page_address[1] = _page_address * this->E2_PAGE_SIZE;

  if (Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, load.page_address, 2))
    return -1;
  if (Kernel::OS.IICDriver.IICRead(this->E2_IIC_ADDRESS, (unsigned char *)&load.data, _data_length))
    return -1;

  strncpy((char*)_output_data, (char*)load.data, _data_length);
  _output_data[-1] = '\0';

  return 0;
}
