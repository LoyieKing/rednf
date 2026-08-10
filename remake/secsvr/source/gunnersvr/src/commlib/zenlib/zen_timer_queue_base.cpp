// Restored from gunnersvr oracle binary (DWARF + disassembly).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_timer_queue_base.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include "src/commlib/zenlib/zen_timer_handler_base.h"
#include "src/commlib/zenlib/zen_time_value.h"

#include <stddef.h>
#include <sys/time.h>
#include <time.h>

// ZEN_OS::get_uptime() returns timeval by value (hidden pointer, ret 0x4).
namespace ZEN_OS {
timeval get_uptime();
}

ZEN_Timer_Queue *ZEN_Timer_Queue::instance_ = 0;

const int ZEN_Timer_Queue::INVALID_TIMER_ID = -1;
const unsigned int ZEN_Timer_Queue::DEFAULT_TIMER_PRECISION_MSEC = 100;
const uint64_t ZEN_Timer_Queue::MSEC_PER_NSEC = 1000000;
const size_t ZEN_Timer_Queue::ONCE_EXTEND_NODE_NUMBER = 8192;

ZEN_Timer_Queue * ZEN_Timer_Queue::instance() {
    return instance_;
}

void ZEN_Timer_Queue::clean_instance() {
    if (instance_ != 0) {
        delete instance_;
    }
    instance_ = 0;
}

void ZEN_Timer_Queue::instance(ZEN_Timer_Queue *pinstatnce) {
    clean_instance();
    instance_ = pinstatnce;
}

int ZEN_Timer_Queue::get_first_timeout(ZEN_Time_Value *first_timeout) {
    int ret = INVALID_TIMER_ID;
    int time_node_id = INVALID_TIMER_ID;
    uint64_t now_trigger_msec = 0;
    uint64_t first_timeout_msec = 0;

    get_frist_nodeid(ret);
    if (ret == INVALID_TIMER_ID) {
        return INVALID_TIMER_ID;
    }

    timeval tv;
    gettimeofday(&tv, 0);
    ZEN_Time_Value now_time(tv);

    if (trigger_mode_ == TRIGGER_MODE_SYSTEM_CLOCK) {
        now_trigger_msec = now_time.total_msec();
    } else if (trigger_mode_ == TRIGGER_MODE_CPU_TICK) {
        ZEN_Time_Value cpu_tick(ZEN_OS::get_uptime());
        prev_trigger_msec_ = cpu_tick.total_msec();
    }

    time_node_id = ret;
    if (time_node_ary_[time_node_id].next_trigger_point_ > now_trigger_msec) {
        first_timeout_msec = time_node_ary_[ret].next_trigger_point_ - now_trigger_msec;
    }
    first_timeout->total_msec(first_timeout_msec);
    return 0;
}

void ZEN_Timer_Queue::calc_next_trigger(int time_node_id, uint64_t now_trigger_msec, bool &continue_trigger) {
    ZEN_TIMER_NODE *prc_time_node = &time_node_ary_[time_node_id];
    continue_trigger = false;
    if (prc_time_node->interval_time_ == ZEN_Time_Value::ZERO_TIME_VALUE) {
        continue_trigger = false;
        return;
    }
    continue_trigger = true;
    prc_time_node->next_trigger_point_ = now_trigger_msec + prc_time_node->interval_time_.total_msec();
}

int ZEN_Timer_Queue::cancel_timer(const ZEN_Timer_Handler *timer_hdl) {
    if (num_timer_node_ != 0) {
        for (size_t i = 0; i < num_timer_node_; ++i) {
            if (time_node_ary_[i].timer_handle_ == timer_hdl) {
                cancel_timer(i);
            }
        }
    }
    return INVALID_TIMER_ID;
}

int ZEN_Timer_Queue::close() {
    for (size_t i = 0; i < num_timer_node_; ++i) {
        ZEN_Timer_Handler *time_hdl = time_node_ary_[i].timer_handle_;
        if (time_hdl != 0) {
            time_hdl->timer_close();
        }
    }
    return 0;
}

int ZEN_Timer_Queue::free_timernode(int time_node_id) {
    if (time_node_ary_[time_node_id].timer_handle_ != 0) {
        if (time_node_id >= num_timer_node_) {
            return INVALID_TIMER_ID;
        }
        free_node_id_list_[time_node_id] = free_node_id_head_;
        free_node_id_head_ = time_node_id;
        time_node_ary_[time_node_id].clear();
        time_node_ary_[time_node_id].action_ = 0;
        time_node_ary_[time_node_id].timer_handle_ = 0;
        time_node_ary_[time_node_id].next_trigger_point_ = 0;
        --num_use_node_;
    }
    return INVALID_TIMER_ID;
}

int ZEN_Timer_Queue::cancel_timer(int timer_id) {
    return free_timernode(timer_id);
}

int ZEN_Timer_Queue::alloc_timernode(ZEN_Timer_Handler *timer_hdl, const void *action, const ZEN_Time_Value &delay_time, const ZEN_Time_Value &interval_time, int &time_node_id, ZEN_TIMER_NODE *&alloc_time_node) {
    int ret = 0;
    size_t old_num_node;

    time_node_id = INVALID_TIMER_ID;
    alloc_time_node = 0;

    if (free_node_id_head_ == INVALID_TIMER_ID) {
        if (dynamic_expand_node_ == false) {
            return INVALID_TIMER_ID;
        }
        old_num_node = 0;
        if (extend_node(num_timer_node_ + ONCE_EXTEND_NODE_NUMBER, old_num_node) != 0) {
            return INVALID_TIMER_ID;
        }
    }

    time_node_id = free_node_id_head_;
    free_node_id_head_ = free_node_id_list_[free_node_id_head_];
    alloc_time_node = &time_node_ary_[time_node_id];
    alloc_time_node->timer_handle_ = timer_hdl;
    alloc_time_node->interval_time_ = interval_time;
    alloc_time_node->action_ = action;
    alloc_time_node->next_trigger_point_ = prev_trigger_msec_ + delay_time.total_msec();
    ++num_use_node_;
    return ret;
}

int ZEN_Timer_Queue::initialize(size_t num_timer_node, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node) {
    size_t old_num_node = 0;

    timer_precision_mesc_ = timer_precision_mesc;
    num_timer_node_ = 0;
    num_use_node_ = 0;
    dynamic_expand_node_ = dynamic_expand_node;
    trigger_mode_ = trigger_mode;

    extend_node(num_timer_node, old_num_node);

    if (trigger_mode_ == TRIGGER_MODE_SYSTEM_CLOCK) {
        timeval tv;
        gettimeofday(&tv, 0);
        ZEN_Time_Value now_time(tv);
        prev_trigger_msec_ = now_time.total_msec();
    } else if (trigger_mode_ == TRIGGER_MODE_CPU_TICK) {
        ZEN_Time_Value cpu_tick(ZEN_OS::get_uptime());
        prev_trigger_msec_ = cpu_tick.total_msec();
    }
    return 0;
}

size_t ZEN_Timer_Queue::expire() {
    uint64_t now_trigger_msec = 0;

    timeval tv;
    gettimeofday(&tv, 0);
    ZEN_Time_Value now_time(tv);

    if (trigger_mode_ == TRIGGER_MODE_SYSTEM_CLOCK) {
        now_trigger_msec = now_time.total_msec();
    } else if (trigger_mode_ == TRIGGER_MODE_CPU_TICK) {
        ZEN_Time_Value cpu_tick(ZEN_OS::get_uptime());
        prev_trigger_msec_ = cpu_tick.total_msec();
    }
    return dispatch_timer(now_time, now_trigger_msec);
}

int ZEN_Timer_Queue::extend_node(size_t num_timer_node, size_t &old_num_node) {
    size_t i;

    old_num_node = num_timer_node_;
    num_timer_node_ = num_timer_node;
    if ((num_timer_node & 0x80000000u) != 0) {
        return INVALID_TIMER_ID;
    }

    ZEN_TIMER_NODE node;
    time_node_ary_.resize(num_timer_node, node);
    free_node_id_list_.resize(num_timer_node, 0);

    for (i = old_num_node; i < num_timer_node_; ++i) {
        time_node_ary_[i].time_id_ = i;
    }
    for (i = old_num_node; i < num_timer_node_ - 1; ++i) {
        free_node_id_list_[i] = i + 1;
    }
    free_node_id_list_[num_timer_node_ - 1] = INVALID_TIMER_ID;
    free_node_id_head_ = old_num_node;
    return 0;
}

ZEN_Timer_Queue::ZEN_Timer_Queue() {
    num_timer_node_ = 0;
    timer_precision_mesc_ = DEFAULT_TIMER_PRECISION_MSEC;
    num_use_node_ = 0;
    dynamic_expand_node_ = false;
    free_node_id_head_ = INVALID_TIMER_ID;
}

ZEN_Timer_Queue::ZEN_Timer_Queue(size_t num_timer_node, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node) {
    num_timer_node_ = 0;
    timer_precision_mesc_ = DEFAULT_TIMER_PRECISION_MSEC;
    num_use_node_ = 0;
    free_node_id_head_ = INVALID_TIMER_ID;
    prev_trigger_msec_ = 0;
    dynamic_expand_node_ = dynamic_expand_node;
    initialize(num_timer_node, timer_precision_mesc, trigger_mode, dynamic_expand_node);
}

ZEN_Timer_Queue::~ZEN_Timer_Queue() {
}
