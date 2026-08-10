// Reconstructed from gunnersvr DWARF + disassembly.
// sizeof = 8：继承 ZEN_Timer_Handler（vptr + timer_queue_）。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TIMER_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TIMER_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"

// sizeof = 8
struct GunnerTimerHandler : public ZEN_Timer_Handler { // line 21
public:
    GunnerTimerHandler(const GunnerTimerHandler &arg0);
    GunnerTimerHandler(ZEN_Timer_Queue *timer_queue); // line 5
    virtual ~GunnerTimerHandler(); // line 11
    virtual int handle_timeout(const ZEN_Time_Value &now, const void *act); // line 16
};

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TIMER_HANDLER_H_H_
