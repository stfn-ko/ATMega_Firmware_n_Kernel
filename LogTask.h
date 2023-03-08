/// Derived from Task - this uses a timer to write the RTC value to the
/// display at periodic intervals


#ifndef LOGTASK_H_
#define LOGTASK_H_

#include "msgids.h"
#include "E2Driver.h"
#include "RTCDriver.h"

class LogTask : public Kernel::Task, public RTCDriver, public E2Driver
{
    static constexpr uint16_t CONTROL_BLOCK_MAGIC = 0xAAAA;
    static constexpr uint8_t MAX_LOG_MESSAGE_SIZE = E2Driver::E2_PAGE_SIZE - sizeof(RTCDriver::RTC_DATE);
    static constexpr uint8_t MAX_MESSAGE_QUEUE_SIZE = 3;
    //#define LOG_MAX_ENTRIES ((65536%128)-1)  == -1 ... no comments

    enum LOG_SYSTEM_STATE
    {
      INIT,
      READY_RW, //rw - read/write
      WRITE_LOG_MSG,
      WRITE_CTRL_BLOCK,
      READBACK_LOG,
      IIC_FAIL
    } log_system_state;

    struct __attribute__((packed)) LOG_PAGE 
    {
      RTCDriver::RTC_DATE date;
      char message[MAX_LOG_MESSAGE_SIZE];
      struct LOG_PAGE* next_page;
    };

    struct CONTROL_BLOCK
    {
      uint16_t magic;
      uint16_t number_of_entries;
      uint16_t oldest_entry;
      uint16_t next_free_entry;
    } control_block;

    LOG_PAGE* pHead = NULL;
    LOG_PAGE* pEnd = NULL;

    RTCDriver rtc;
    E2Driver e2;

    uint16_t next_to_read = 0;
    uint8_t message_queue_size, IIC_failures = 0;

    bool start_readback, start_delete = false;

    int CreateLogEntry(const char* _message);
    void LogPageToSerial(LOG_PAGE* _page);

  protected:
    virtual void TaskLoop();
    virtual void EventHandler(int _message_id, void * _context);

  public:
    LogTask();

    int SetDate(uint8_t _hour, uint8_t _minute, uint8_t _second, uint8_t _day, uint8_t _month, uint8_t _year);
};


#endif
