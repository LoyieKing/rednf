// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_timer_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TIMER_HANDLER_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TIMER_HANDLER_H_H_

#include <stdint.h>
#include <time.h>

// sizeof = 40
struct Comm_Timer_Handler : public ZEN_Timer_Handler { // line 15
private:
Comm_Time_Provider *time_provider_;
Comm_Stat_Monitor *stat_monitor_;
Comm_Svrd_Appliction *svrd_app_;
time_t last_check_;
time_t last_check_status_;
bool is_first_record_;
unsigned int game_id_;
uint32_t classify_id_;
public:
Comm_Timer_Handler(const Comm_Timer_Handler &arg0);
Comm_Timer_Handler(ZEN_Timer_Queue *arg0); // line 115
virtual ~Comm_Timer_Handler(); // line 123
protected:
virtual int handle_timeout(const ZEN_Time_Value &now_time, const void *act); // line 18
public:
void notify_reload(); // line 37
void update_time(const ZEN_Time_Value &now_time); // line 53
void check_monitor(const ZEN_Time_Value &now_time); // line 59
void check_status(const ZEN_Time_Value &now_time); // line 80
int init(); // line 131
uint32_t get_svr_classify_id() const; // line 156
};

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TIMER_HANDLER_H_H_
