// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_app_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APP_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APP_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_time_value.h"

// sizeof = 12
struct Zerg_App_Timer_Handler : public ZEN_Timer_Handler { // line 14
public:
static const int ZERGAPP_TIME_ID[];
static const int RETRY_TIME_INTERVAL;
static const int HEARTBEAT_TIME_INTERVAL;
static const int AUTOCONNECT_RETRY_INTERVAL;
static const unsigned int SEND_FRAME_CHECK_INTERVAL;
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
extern const int HEARTBEAT_TIME_INTERVAL; // line 24
extern const int AUTOCONNECT_RETRY_INTERVAL; // line 26
extern const unsigned int SEND_FRAME_CHECK_INTERVAL; // line 29
extern int ZERGAPP_TIME_ID[]; // line 18
extern ZEN_Time_Value now_time_; // line 32

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APP_HANDLER_H_H_
