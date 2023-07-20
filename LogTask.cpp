#include "LogTask.h"


LogTask::LogTask() : log_system_state(LOG_SYSTEM_STATE::INIT)
{
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_DATALOG_LOGEVENT, this);
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_DATALOG_DUMPLOG, this);
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_DATALOG_DELETELOG, this);
}


int LogTask::SetDate(uint8_t _hour, uint8_t _minute, uint8_t _second, uint8_t _day, uint8_t _month, uint8_t _year)
{
  RTCDriver::RTC_DATE new_date = {_second, _minute, _hour, 1, _day, _month, _year};
  return rtc.set_date(new_date);
}


void LogTask::TaskLoop()
{
  if (this->IIC_failures > 5)
    this->log_system_state = LOG_SYSTEM_STATE::IIC_FAIL;

  switch (this->log_system_state)
  {
    case LOG_SYSTEM_STATE::INIT:
      {
        if (e2.memory_read(E2Driver::E2_LAST_ADDRESS, (uint8_t*)&this->control_block, sizeof(CONTROL_BLOCK)))
        {
          ++this->IIC_failures;
          return;
        }

        if (this->control_block.magic != this->CONTROL_BLOCK_MAGIC)
        {
          this->control_block = {};
          this->control_block.magic = this->CONTROL_BLOCK_MAGIC;
        }

        this->IIC_failures = 0;
        this->log_system_state = LOG_SYSTEM_STATE::READY_RW;
      }
      break;

    case LOG_SYSTEM_STATE::READY_RW:
      {
        if (this->start_delete)
        {
          this->control_block = {};
          this->control_block.magic = this->CONTROL_BLOCK_MAGIC;
          this->log_system_state = LOG_SYSTEM_STATE::WRITE_CTRL_BLOCK;
        }
        else if (this->start_readback)
        {
          this->start_readback = false;
          this->next_to_read = this->control_block.oldest_entry;
          this->log_system_state = LOG_SYSTEM_STATE::READBACK_LOG;
        }
        else if (this->pHead && this->message_queue_size)
          this->log_system_state = LOG_SYSTEM_STATE::WRITE_LOG_MSG;

      }
      break;

    case LOG_SYSTEM_STATE::WRITE_LOG_MSG:
      {
        if (this->e2.memory_write(this->control_block.next_free_entry, (const uint8_t*)this->pHead, MAX_LOG_MESSAGE_SIZE))
        {
          ++this->IIC_failures;
          return;
        }

        LOG_PAGE* to_delete = this->pHead;
        this->pHead = pHead->next_page;
        if (!pHead) pEnd = NULL;
        --this->message_queue_size;
        delete to_delete;

        if (this->control_block.number_of_entries < E2Driver::E2_LAST_ADDRESS)
          ++this->control_block.number_of_entries;

        if (++this->control_block.next_free_entry >= E2Driver::E2_LAST_ADDRESS)
          this->control_block.next_free_entry = 0;

        if (this->control_block.next_free_entry == this->control_block.oldest_entry)
          ++this->control_block.oldest_entry;

        if (this->control_block.oldest_entry >= E2Driver::E2_LAST_ADDRESS)
          this->control_block.oldest_entry = 0;

        this->IIC_failures = 0;

        this->log_system_state = LOG_SYSTEM_STATE::WRITE_CTRL_BLOCK;
        break;
      }
    case LOG_SYSTEM_STATE::WRITE_CTRL_BLOCK:
      {
        if (e2.memory_write(E2Driver::E2_LAST_ADDRESS, (const uint8_t*)&this->control_block, sizeof(CONTROL_BLOCK)))
        {
          ++this->IIC_failures;
          return;
        }

        this->IIC_failures = 0;
        this->log_system_state = LOG_SYSTEM_STATE::READY_RW;
      }
      break;

    case LOG_SYSTEM_STATE::READBACK_LOG:
      {
        LOG_PAGE output_page;

        if (this->e2.memory_read(this->next_to_read, (unsigned char*)&output_page, E2Driver::E2_PAGE_SIZE))
        {
          ++this->IIC_failures;
          return;
        }

        this->LogPageToSerial(&output_page);

        if (++this->next_to_read == this->control_block.next_free_entry)
          this->log_system_state = LOG_SYSTEM_STATE::READY_RW;

        else if (this->next_to_read >= E2Driver::E2_LAST_ADDRESS)
          this->next_to_read = 0;
      }
      break;

    case LOG_SYSTEM_STATE::IIC_FAIL:
      break;

    default: break;
  }
}


int LogTask::CreateLogEntry(const char* _message)
{
  RTC_DATE date;
  LOG_PAGE* new_page_entry = new LOG_PAGE;

  if ( (strlcpy(new_page_entry->message, _message, MAX_LOG_MESSAGE_SIZE) >= MAX_LOG_MESSAGE_SIZE)
       || (this->message_queue_size >= this->MAX_MESSAGE_QUEUE_SIZE)
       || this->rtc.get_date(new_page_entry->date)
       || !new_page_entry)
  {
    delete new_page_entry;
    return -1;
  }

  new_page_entry->next_page = NULL;

  this->LogPageToSerial(new_page_entry);

  if (!this->pHead)
  {
    this->pHead = new_page_entry;
    this->pEnd = new_page_entry;
  }
  else
  {
    this->pEnd->next_page = new_page_entry;
    this->pEnd = new_page_entry;
  }

  ++this->message_queue_size;
  return 0;
}


void LogTask::LogPageToSerial(LOG_PAGE* _page)
{
  char output[E2Driver::E2_PAGE_SIZE];
  sprintf(
    output, "%02d.%02d.%02d / %02d:%02d:%02d\n%s",
    _page->date.date, _page->date.month, _page->date.year, _page->date.hour, _page->date.minute, _page->date.second,
    _page->message
  );

  Serial.println(output);
}


void LogTask::EventHandler(int _message_id, void * _context)
{
  switch (_message_id)
  {
    case MSG_ID_DATALOG_LOGEVENT:
      CreateLogEntry((const char*)_context);
      break;

    case MSG_ID_DATALOG_DUMPLOG:
      start_readback = true;
      break;

    case MSG_ID_DATALOG_DELETELOG:
      start_delete = true;
      break;

    default: break;
  }
}
