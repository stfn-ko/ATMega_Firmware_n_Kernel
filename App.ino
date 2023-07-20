#include "KeypadTask.h"
#include "display.h"
#include "LogTask.h"

LogTask	logger;
Control control;
KeypadTask keypad;
Display lc_display;
SevenSegEventHandler seven_segment;

void UserInit()
{
  Serial.begin(115200);
  
  lc_display.Start();

  RPS::Init();

  PWM::Init();

  logger.Start();

  control.Start();

  if (logger.SetDate(3, 12, 2, 10, 10, 10))
    return;

  Kernel::OS.MessageQueue.Post(MSG_ID_INIT_COMPLETE, 0, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
}
