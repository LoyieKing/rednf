// Auto-generated header stub from DWARF info
// Original path: src/commsvr/gunnersvr/gunner_timer_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TIMER_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TIMER_HANDLER_H_H_

// sizeof = 8
struct GunnerTimerHandler : public ZEN_Timer_Handler { // line 21
public:
GunnerTimerHandler(const GunnerTimerHandler &arg0);
GunnerTimerHandler(ZEN_Timer_Queue *arg0); // line 5
virtual ~GunnerTimerHandler(); // line 11
virtual int handle_timeout(const ZEN_Time_Value &now, const void *act); // line 16
};

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TIMER_HANDLER_H_H_
