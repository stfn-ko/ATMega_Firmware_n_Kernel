#include "control.h"


Control::Control()
{
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_NEW_RPS_ENTERED, this);
}


void Control::TaskLoop()
{
  if (!this->timer.isExpired())
    return;

  if (this->demand_rps != 0)
    this->timer_counter++;

  this->actual_rps = RPS::GetRPS();
  Kernel::OS.MessageQueue.Post(MSG_ID_NEW_ACTUAL_RPS, (void*)this->actual_rps, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);

  if (this->timer_counter >= 8 && this->demand_rps > 0)
  {
    this->SendLogMessage(this->demand_rps, this->actual_rps);
    timer_counter = 0;
  }

  this->timer.Restart();
}


void Control::EventHandler(int _posted_msg_id, void * _context)
{
  if (_posted_msg_id != MSG_ID_NEW_RPS_ENTERED)
    return;

  if ((uint16_t)_context >= 330 || (uint16_t)_context == 0)
    this->demand_rps = (uint16_t)_context;
  else if ((uint16_t)_context >= 310)
    this->demand_rps = (uint16_t)_context - 10;
  else if ((uint16_t)_context >= 230)
    this->demand_rps = (uint16_t)_context - 40;
  else if ((uint16_t)_context >= 230)
    this->demand_rps = (uint16_t)_context - 55;
  else if ((uint16_t)_context >= 150)
    this->demand_rps = (uint16_t)_context - 40;
  else if ((uint16_t)_context >= 120)
    this->demand_rps = (uint16_t)_context - 35;
  else if ((uint16_t)_context >= 100)
    this->demand_rps = (uint16_t)_context - 25;
  else if ((uint16_t)_context >= 70)
    this->demand_rps = (uint16_t)_context - 5;
  else if ((uint16_t)_context >= 50)
    this->demand_rps = (uint16_t)_context + 5;

  PWM::SetPWM(this->demand_rps * 255 / this->RPS_MAX);

  this->demand_rps = (uint16_t)_context;

  if (this->demand_rps == 0)
    Kernel::OS.MessageQueue.Post(MSG_ID_DATALOG_DUMPLOG, NULL, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
  else
    this->SendLogMessage(this->demand_rps, this->actual_rps);
}


void Control::SendLogMessage(uint16_t _demand_rps, uint16_t _actual_rps)
{
  char* log_message = new char[38];
  sprintf(log_message, "- Demand RPS: %03d\n- Actual RPS: %03d\n", _demand_rps, _actual_rps);
  Kernel::OS.MessageQueue.Post(MSG_ID_DATALOG_LOGEVENT, static_cast<void*>(log_message), Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
}
