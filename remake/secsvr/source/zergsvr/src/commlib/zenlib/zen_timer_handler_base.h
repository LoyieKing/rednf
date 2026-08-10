// Restored from gunnersvr binary (DWARF layout + vtable/disassembly 对照)
// Original path: src/commlib/zenlib/zen_timer_handler_base.h
// 类布局按 DWARF：sizeof = 8；timer_queue_ @0x04 (protected)
// vtable 槽位(declaration order)：~ZEN_Timer_Handler, handle_timeout(=0 纯虚), timer_close
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_HANDLER_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_HANDLER_BASE_H_H_

struct ZEN_Timer_Queue;
struct ZEN_Time_Value;

// sizeof = 8
struct ZEN_Timer_Handler { // line 31
public:
protected:
ZEN_Timer_Queue *timer_queue_;
public:
protected:
ZEN_Timer_Handler(ZEN_Timer_Queue *set_timer_queue); // line 7
ZEN_Timer_Handler(); // line 12
virtual ~ZEN_Timer_Handler(); // line 16
public:
virtual int handle_timeout(const ZEN_Time_Value &arg0, const void *arg1) = 0; // line 49 (pure virtual)
virtual int timer_close(); // line 33
ZEN_Timer_Queue * timer_queue(); // line 21
void timer_queue(ZEN_Timer_Queue *set_timer_queue); // line 27
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_HANDLER_BASE_H_H_
