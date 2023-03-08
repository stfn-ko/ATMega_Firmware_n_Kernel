#include "RTCDriver.h"


RTCDriver::RTCDriver(unsigned char _iic_address) : RTC_ICC_ADDRESS(_iic_address)
{
  // set ST bit to 1 (0x80)
  unsigned char iicregs[2] = {0x00, 0x80};

  // write the address
  Kernel::OS.IICDriver.IICWrite(this->RTC_ICC_ADDRESS, iicregs, 2);
}


int RTCDriver::set_date(RTC_DATE& _input_date)
{
  RTC_LOG load;

  int max_month_days[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // adjust max_month_days for leap year
  if ((_input_date.year + 2000) % 400 == 0 || ((_input_date.year + 2000) % 4 == 0 && (_input_date.year + 2000) % 100 != 0))
    max_month_days[1] = 28;

  // rules for sensible input
  if (_input_date.month < 1 || _input_date.month > 12)
    return -1;

  if (_input_date.date < 1 || _input_date.date > max_month_days[_input_date.month - 1])
    return -1;

  if (_input_date.year < 0 || _input_date.year > 99)
    return -1;

  if (_input_date.minute < 0 || _input_date.minute > 59)
    return -1;

  if (_input_date.second < 0 || _input_date.second > 59)
    return -1;

  load.address = 0;
  load.date.second =  ((_input_date.second % 10) | ((_input_date.second / 10) << 4)) | 0x80;
  load.date.minute =  ((_input_date.minute % 10) | ((_input_date.minute / 10) << 4)) | 0x80;
  load.date.hour =    ((_input_date.hour % 10) | ((_input_date.hour / 10) << 4)) | 0x00; // 24hr format

  load.date.date =    ((_input_date.date % 10) | ((_input_date.date / 10) << 4)) | 0x00;
  load.date.month =   ((_input_date.month % 10) | ((_input_date.month / 10) << 4)) | 0x00;
  load.date.year =    ((_input_date.year % 10) | ((_input_date.year / 10) << 4));

  // Writing the date data to the RTC
  return Kernel::OS.IICDriver.IICWrite(this->RTC_ICC_ADDRESS, (unsigned char *)&load, sizeof(RTC_LOG));
}


int RTCDriver::get_date(RTC_DATE& _input_date)
{
  RTC_DATE load;
  unsigned char iicregs[2] = {0, 0};

  // write the address ->
  // return error (-1) if unsuccessful
  if (Kernel::OS.IICDriver.IICWrite(this->RTC_ICC_ADDRESS, iicregs, 1))
    return -1;

  // read the values ->
  // return error (-1) if unsuccessful
  if (Kernel::OS.IICDriver.IICRead(this->RTC_ICC_ADDRESS, (unsigned char *)&load, sizeof(RTC_DATE)))
    return -1;

  // update input date ->
  // translate retrieved RTC date into human-readable format 
  _input_date.hour = (load.hour & 0x0f) + (10 * ((load.hour >> 4) & 0x07));
  _input_date.minute = (load.minute & 0x0f) + (10 * ((load.minute >> 4) & 0x07));
  _input_date.second = (load.second & 0x0f) + (10 * ((load.second >> 4) & 0x07));

  _input_date.date = (load.date & 0x0f) + (10 * ((load.date >> 4) & 0x07));
  _input_date.month = (load.month & 0x0f) + (10 * ((load.month >> 4) & 0x07));
  _input_date.year = (load.year & 0x0f) + (10 * (load.year >> 4));

  return 0; 
}
