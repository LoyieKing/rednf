// Auto-generated header stub from DWARF info (class layout reconstructed from DWARF, fixed to compile)
// Original path: src/commlib/zenlib/zen_timer_queue_wheel.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_WHEEL_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_WHEEL_H_H_

#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include <stddef.h>
#include <stdint.h>
#include <vector>

// sizeof = 96
struct ZEN_Timer_Wheel : public ZEN_Timer_Queue { // line 16
    // sizeof = 12
    struct ZEN_WHEEL_TIMER_NODE { // line 24
    public:
    int list_prev_; // line 27
    int list_next_; // line 29
    int wheel_point_id_; // line 32
    ZEN_WHEEL_TIMER_NODE() : list_prev_(-1), list_next_(-1), wheel_point_id_(-1) {} // line 35
    ~ZEN_WHEEL_TIMER_NODE() {} // line 42
    };
protected:
unsigned int timer_length_mesc_; // line 52
size_t num_wheel_point_; // line 55
std::vector<int> timer_wheel_point_; // line 57 (wheel point -> head timer node id)
size_t proc_wheel_start_; // line 60
std::vector<ZEN_WHEEL_TIMER_NODE> wheel_node_list_; // line 63 (per timer node list linkage)
static const unsigned int DEFAULT_TIMER_LENGTH_MESC; // line 68, value 259200000 (0xf731400)
public:
ZEN_Timer_Wheel(size_t num_timer_node, unsigned int timer_length_mesc, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node); // line 16
ZEN_Timer_Wheel(); // line 35
virtual ~ZEN_Timer_Wheel(); // line 44
int initiate(size_t num_timer_node, unsigned int timer_length_mesc, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node); // line 53
virtual int schedule_timer(ZEN_Timer_Handler *timer_hdl, const void *action, const ZEN_Time_Value &delay_time, const ZEN_Time_Value &interval_time); // line 193
virtual int cancel_timer(int timer_id); // line 225
virtual int extend_node(size_t num_timer_node, size_t &old_num_node); // line 95
protected:
virtual int reschedule_timer(int timer_id, uint64_t now_trigger_msec); // line 248
virtual int get_frist_nodeid(int &first_node_id); // line 276
virtual size_t dispatch_timer(const ZEN_Time_Value &now_time, uint64_t now_trigger_msec); // line 316
void bind_wheel_listnode(int time_node_id); // line 121
void unbind_wheel_listnode(int time_node_id); // line 153
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_WHEEL_H_H_
