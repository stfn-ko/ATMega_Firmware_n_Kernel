///////////////////////////////////////////////////////////////////////////////
/// RTCDriver.h
///
/// RTC driver class. This encapsulates the low-level IIC calls to the RTC
/// and provides higher level member functions to allow the time to be
/// set and queried. All the BCD conversion and bitmasking should take
/// place here. Further member functions can be added to query the status
/// bits in the RTC, if needed.
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _RTCDRIVER_H_
#define _RTCDRIVER_H_

#include "kernel.h"

class RTCDriver {
    static const unsigned char RTC_DEFAULT_IIC_ADDRESS = 222;
    const unsigned char RTC_ICC_ADDRESS;

  public:
    struct RTC_DATE
    {
      uint8_t second;
      uint8_t minute;
      uint8_t hour;
      uint8_t weekday;
      uint8_t date;
      uint8_t month;
      uint8_t year;
    };

    struct RTC_LOG
    {
      uint8_t address;
      RTC_DATE date;
    };

    // custom constructor : takes RTC address ->
    // enables multiple Real-Time-Clock devices instantiation
    RTCDriver(unsigned char _iic_address = RTC_DEFAULT_IIC_ADDRESS);

    // Setter : sets date in human-readable format and converts it into rtc-readable format ->
    // doesn't allow encoded format via refrence
    int set_date(RTC_DATE& _input_date);


    // Getter : gets in rtc-readable format and converts it into human-readable format

    int get_date(RTC_DATE& _input_date);
};

#endif
