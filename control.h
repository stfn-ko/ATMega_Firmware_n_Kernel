/// Main control task. This performs task-time async operations


#ifndef CONTROL_H_
#define CONTROL_H_

#include "msgids.h"
#include "rps.h"
#include "pwm.h"

class Control : public Kernel::Task {
    Kernel::OSTimer	timer = 250;

    unsigned long timer_counter, demand_rps, actual_rps = 0;

    void SendLogMessage(uint16_t _demand_rps, uint16_t _actual_rps);

  protected:
    virtual void TaskLoop();

    // The context is simply an integer in this ->
    // case, this is the mapping we want displayed
    virtual void EventHandler(int _posted_msg_id, void * _context);

  public:
    static constexpr uint16_t RPS_MAX = 340;
    static constexpr uint8_t RPS_MIN = 50;

    Control();

};

#endif
