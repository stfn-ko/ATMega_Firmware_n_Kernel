///////////////////////////////////////////////////////////////////////////////
/// E2Driver.cpp
///
/// E2 non-volatile memory driver class. This encapsulates the low-level IIC
/// calls to the E2 memory device and provides higher level member functions
/// to provide for random read/writes to the memory space.
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#include "E2Driver.h"

// doesn't need data length parameter, because writes are fixed size (32B)
int E2Driver::memory_write(uint16_t _page_address, const char * _input_data)
{
  if (_page_address >= this->E2_LAST_ADDRESS)
    return -1;

  // check for input size to avoid buffer overflow,
  // return an error in case _input_data size is bigger than 32
  if (_input_data[-1] == '\0' && strlen(_input_data) > 32)
    return -1;
  else if (sizeof(_input_data) > 32 )
    return -1;

  DATA_BUFFER load;

  // automatically translates 0-2046 page address into
  // accepted by E2 0-65535 page address
  // prevents from unwanted data overwrites ->
  // due to page boundary limits exess
  load.page_address[0] = (_page_address * 32) >> 8;
  load.page_address[1] = _page_address * 32;

  // made safe from buffer overflow ->
  // made safe from undefinded behaviour (double null-terminator) ->
  // null-terminated regardless
  strncpy(load.data, _input_data, 32);
  load.data[32] = '\0';

  return Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, (unsigned char *)&load, 34);
}


int E2Driver::memory_read(uint16_t _page_address)
{
  if (_page_address > this->E2_LAST_ADDRESS)
    return -1;

  DATA_BUFFER load;

  load.page_address[0] = (_page_address * 32) >> 8;
  load.page_address[1] = _page_address * 32;

  if (Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, load.page_address, 2))
    return -1;
  if (Kernel::OS.IICDriver.IICRead(this->E2_IIC_ADDRESS, (unsigned char *)&load.data, 32))
    return -1;

  Serial.print(load.data);

  return 0;
}

/*
  // igonres custom 32B paging ->
  // function call requirements :
  // (start address > end address) &&
  // (start address != end address) &&
  // (0 < start address < 2047 * 32 - 1 (aka 65503)) &&
  // (0 < end address < 2047 * 32 - 1 (aka 65503))
  int E2Driver::memory_read_from_to(uint16_t _start_address, uint16_t _end_address)
  {
  // input requirements validation
  //if (_start_address > this->E2_LAST_ADDR * 32 - 1) return -1;
  //else if (_start_address < 0) return -1;
  //else if (_end_address > this->E2_LAST_ADDR * 32 - 1) return -1;
  //else if (_end_address < 0) return -1;
  //else if (_start_address == _end_address) return -1;
  //else if (_start_address > _end_address) return -1;

  // instead of using DATA_BUFFER struct we are using these two values
  // to break out from the char[32] read limitation in DATA_BUFFER.data
  const char* data;
  unsigned char page_address[2];
  page_address[0] = (_start_address) >> 8;
  page_address[1] = _start_address;

  if (Kernel::OS.IICDriver.IICWrite(this->E2_IIC_ADDRESS, page_address, 2))
    return -1;

  // _end_address - _start_address -> read size in bytes
  if (Kernel::OS.IICDriver.IICRead(this->E2_IIC_ADDRESS, (unsigned char *)&data, _end_address - _start_address))
    return -1;

  Serial.print(data);
  return 0;
  }
*/

int E2Driver::memory_read_all()
{
  for (auto i = 0; i <= this->E2_LAST_ADDRESS - 1; ++i)
    if (this->memory_read(i)) return -1;
}
