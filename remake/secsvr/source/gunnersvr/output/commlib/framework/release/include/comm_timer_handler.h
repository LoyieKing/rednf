// Restored from gunnersvr binary (DWARF layout + disassembly 对照).
// sizeof = 36：ZEN_Timer_Handler (vptr 0 + timer_queue_ 4) 后接
//   time_provider_ 8, stat_monitor_ 12, svrd_app_ 16, last_check_ 20,
//   is_first_record_ 24, game_id_ 28, classify_id_ 32。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TIMER_HANDLER_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TIMER_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include <stdint.h>
#include <time.h>

struct Comm_Time_Provider;
struct Comm_Stat_Monitor;
struct Comm_Svrd_Appliction;

// sizeof = 36
struct Comm_Timer_Handler : public ZEN_Timer_Handler { // line 15
private:
    Comm_Time_Provider *time_provider_;   // 0x08
    Comm_Stat_Monitor *stat_monitor_;     // 0x0c
    Comm_Svrd_Appliction *svrd_app_;      // 0x10
    time_t last_check_;                   // 0x14
    bool is_first_record_;                // 0x18
    unsigned int game_id_;                // 0x1c
    uint32_t classify_id_;                // 0x20
public:
    Comm_Timer_Handler(ZEN_Timer_Queue *arg0); // line 108
    virtual ~Comm_Timer_Handler(); // line 116
protected:
    virtual int handle_timeout(const ZEN_Time_Value &now_time, const void *act); // line 18
public:
    void notify_reload(); // line 34
    void update_time(const ZEN_Time_Value &now_time); // line 50
    void check_monitor(const ZEN_Time_Value &now_time); // line 56
    void report_status(); // line 78
    int init(); // line 124
    uint32_t get_svr_classify_id() const; // line 148
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TIMER_HANDLER_H_H_
