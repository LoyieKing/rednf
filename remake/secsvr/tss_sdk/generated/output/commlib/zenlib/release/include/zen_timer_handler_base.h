// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_timer_handler_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TIMER_HANDLER_BASE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TIMER_HANDLER_BASE_H_H_

// sizeof = 8
struct ZEN_Timer_Handler { // line 31
public:
int (***_vptr.ZEN_Timer_Handler)(...);
protected:
ZEN_Timer_Queue *timer_queue_;
public:
ZEN_Timer_Handler(const ZEN_Timer_Handler &arg0);
protected:
ZEN_Timer_Handler(ZEN_Timer_Queue *arg0); // line 7
ZEN_Timer_Handler(); // line 12
virtual ~ZEN_Timer_Handler(); // line 16
public:
virtual int handle_timeout(const ZEN_Time_Value &arg0, const void *arg1); // line 49
virtual int timer_close(); // line 33
ZEN_Timer_Queue * timer_queue(); // line 21
void timer_queue(ZEN_Timer_Queue *set_timer_queue); // line 27
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_TIMER_HANDLER_BASE_H_H_
