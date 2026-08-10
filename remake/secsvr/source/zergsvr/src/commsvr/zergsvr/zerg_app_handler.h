// Reconstructed from zergsvr binary DWARF (类布局/常量值) + 反汇编。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APP_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APP_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_time_value.h"

// sizeof = 12
struct Zerg_App_Timer_Handler : public ZEN_Timer_Handler { // line 14
public:
static const int ZERGAPP_TIME_ID[];
static const int RETRY_TIME_INTERVAL = 2; // line 21（DWARF const_value）
static const int HEARTBEAT_TIME_INTERVAL = 1; // line 24
static const int AUTOCONNECT_RETRY_INTERVAL = 5; // line 26
static const unsigned int SEND_FRAME_CHECK_INTERVAL = 10; // line 29
static ZEN_Time_Value now_time_;
protected:
unsigned int heartbeat_counter_;
public:
Zerg_App_Timer_Handler(const Zerg_App_Timer_Handler &arg0);
Zerg_App_Timer_Handler(ZEN_Timer_Queue *arg0); // line 25
protected:
virtual ~Zerg_App_Timer_Handler(); // line 38
virtual int handle_timeout(const ZEN_Time_Value &time_now, const void *arg); // line 44
virtual int timer_close(); // line 72
};
#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APP_HANDLER_H_H_
