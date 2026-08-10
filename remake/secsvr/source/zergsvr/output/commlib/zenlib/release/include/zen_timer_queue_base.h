// Auto-generated header stub from DWARF info (headers fixed to compile)
// Original path: src/commlib/zenlib/zen_timer_queue_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_BASE_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include <stddef.h>
#include <stdint.h>
#include <vector>

struct ZEN_Timer_Handler;

// sizeof = 60
struct ZEN_Timer_Queue : public ZEN_NON_Copyable { // line 28
    enum TRIGGER_MODE {
        TRIGGER_MODE_SYSTEM_CLOCK = 1,
        TRIGGER_MODE_CPU_TICK = 2,
    };
    // sizeof = 28
    struct ZEN_TIMER_NODE {
    public:
    int time_id_;
    ZEN_Time_Value interval_time_;
    const void *action_;
    ZEN_Timer_Handler *timer_handle_;
    uint64_t next_trigger_point_;
    ZEN_TIMER_NODE() : time_id_(INVALID_TIMER_ID), interval_time_(ZEN_Time_Value::ZERO_TIME_VALUE), action_(0), timer_handle_(0), next_trigger_point_(0) {} // line 81/86
    ~ZEN_TIMER_NODE() {} // line 90
    void clear() { interval_time_.set(0, 0); } // line 94/96
    ZEN_Timer_Queue::ZEN_TIMER_NODE & operator=(const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg0) {
        time_id_ = arg0.time_id_;
        interval_time_ = arg0.interval_time_;
        action_ = arg0.action_;
        timer_handle_ = arg0.timer_handle_;
        next_trigger_point_ = arg0.next_trigger_point_;
        return *this;
    }
    };
public:
static const int INVALID_TIMER_ID;
static const unsigned int DEFAULT_TIMER_PRECISION_MSEC;
static const uint64_t MSEC_PER_NSEC;
static const size_t ONCE_EXTEND_NODE_NUMBER;
protected:
size_t num_timer_node_;
unsigned int timer_precision_mesc_;
size_t num_use_node_;
ZEN_Timer_Queue::TRIGGER_MODE trigger_mode_;
bool dynamic_expand_node_;
std::vector<ZEN_TIMER_NODE> time_node_ary_;
int free_node_id_head_;
std::vector<int> free_node_id_list_;
uint64_t prev_trigger_msec_;
static ZEN_Timer_Queue *instance_;
public:
ZEN_Timer_Queue(const ZEN_Timer_Queue &arg0);
protected:
ZEN_Timer_Queue(size_t arg0, unsigned int arg1, ZEN_Timer_Queue::TRIGGER_MODE arg2, bool arg3); // line 18
ZEN_Timer_Queue(); // line 34
public:
virtual ~ZEN_Timer_Queue(); // line 44
virtual int schedule_timer(ZEN_Timer_Handler *arg0, const void *arg1, const ZEN_Time_Value &arg2, const ZEN_Time_Value &arg3) = 0; // line 166
virtual int cancel_timer(int timer_id) = 0; // line 161
protected:
virtual int get_frist_nodeid(int &arg0) = 0; // line 175
virtual size_t dispatch_timer(const ZEN_Time_Value &arg0, uint64_t arg1) = 0; // line 180
public:
virtual size_t expire(); // line 394
virtual int cancel_timer(const ZEN_Timer_Handler *timer_hdl); // line 167
virtual int extend_node(size_t num_timer_node, size_t &old_num_node) = 0; // line 114
virtual int close(); // line 96
protected:
virtual int reschedule_timer(int arg0, uint64_t arg1) = 0; // line 205
int initialize(size_t num_timer_node, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node); // line 53
int alloc_timernode(ZEN_Timer_Handler *timer_hdl, const void *action, const ZEN_Time_Value &delay_time, const ZEN_Time_Value &interval_time, int &time_node_id, ZEN_Timer_Queue::ZEN_TIMER_NODE *&alloc_time_node); // line 205
void calc_next_trigger(int time_node_id, uint64_t now_trigger_msec, bool &continue_trigger); // line 294
int free_timernode(int time_node_id); // line 319
int get_first_timeout(ZEN_Time_Value *first_timeout); // line 349
public:
static ZEN_Timer_Queue * instance(); // line 424
static void instance(ZEN_Timer_Queue *pinstatnce); // line 430
static void clean_instance(); // line 438
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_BASE_H_H_
