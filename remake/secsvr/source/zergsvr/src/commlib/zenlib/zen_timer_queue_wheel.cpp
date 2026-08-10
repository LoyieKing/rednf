// Restored from gunnersvr oracle binary (DWARF + disassembly).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_timer_queue_wheel.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// 语义要点（与 0x0809c240..0x0809cdb0 反汇编一致）：
//  - initiate：timer_length_mesc_ 下限钳制为 3600000ms（1 小时）；轮盘槽数 =
//    timer_length_mesc_ / timer_precision_mesc_ + 1；槽数组 resize 后全填 INVALID_TIMER_ID。
//  - bind：槽位 = ((next_trigger_point_ - prev_trigger_msec_) / timer_precision_mesc_
//    + proc_wheel_start_) % num_wheel_point_（商先截断为 32 位，与 __udivdi3 用法一致）；
//    插入链表头部。
//  - unbind：按 wheel_point_id_ 找到槽，若为头则摘头，否则双向链表摘除，再复位三个链表字段。
//  - get_frist_nodeid 原版怪癖：扫描到非空槽后既不入参也不返回值（first_node_id
//    始终保持 -1，返回值恒为 -1），予以保留。
//  - dispatch：now < prev 或 elapsed > timer_length_mesc_ 时报错/告警并只前移 prev；
//    否则逐槽逐节点回调 handle_timeout（虚 [vptr+8]），回调后若节点 timer_handle_
//    仍非空则虚调用 reschedule_timer（[vptr+0x28]），再重读槽头继续。
//  - extend_node：在基类扩容后把 wheel_node_list_ 同步到 num_timer_node_ 大小
//    （多了 insert 补齐、少了 erase 截断），新节点三字段均为 -1。
//  - 构造函数失败时打印 "[zenlib] ZEN_Timer_Wheel::initiate fail."。

#include <iostream> // 保留：原 TU 包含 <iostream>，产生同款 ios_base::Init 静态初始化

#include <stddef.h>
#include <stdint.h>
#include <algorithm>
#include <vector>

#include "src/commlib/zenlib/zen_timer_queue_wheel.h"
#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include "src/commlib/zenlib/zen_timer_handler_base.h"
#include "src/commlib/zenlib/zen_time_value.h"

// TU-local declaration; real class lives in zen_trace_log_msg.{h,cpp}
// (its generated stub header is being restored in the same round).
struct ZEN_Trace_LogMsg {
    static void debug_errorex(const char *str_format, ...);
    static void debug_alertex(const char *str_format, ...);
};

const unsigned int ZEN_Timer_Wheel::DEFAULT_TIMER_LENGTH_MESC = 259200000;

ZEN_Timer_Wheel::ZEN_Timer_Wheel() {
    timer_length_mesc_ = 0;
    num_wheel_point_ = 0;
    proc_wheel_start_ = 0;
}

ZEN_Timer_Wheel::ZEN_Timer_Wheel(size_t num_timer_node, unsigned int timer_length_mesc,
                                 unsigned int timer_precision_mesc,
                                 ZEN_Timer_Queue::TRIGGER_MODE trigger_mode,
                                 bool dynamic_expand_node) {
    timer_length_mesc_ = 0;
    num_wheel_point_ = 0;
    proc_wheel_start_ = 0;
    if (initiate(num_timer_node, timer_length_mesc, timer_precision_mesc,
                 trigger_mode, dynamic_expand_node) != 0) {
        ZEN_Trace_LogMsg::debug_errorex("[zenlib] ZEN_Timer_Wheel::initiate fail.");
    }
}

ZEN_Timer_Wheel::~ZEN_Timer_Wheel() {
}

int ZEN_Timer_Wheel::initiate(size_t num_timer_node, unsigned int timer_length_mesc,
                              unsigned int timer_precision_mesc,
                              ZEN_Timer_Queue::TRIGGER_MODE trigger_mode,
                              bool dynamic_expand_node) {
    // 原版最小轮盘时长：1 小时（3600000ms），小于则钳制。
    if (timer_length_mesc < 3600000) {
        timer_length_mesc = 3600000;
    }
    timer_length_mesc_ = timer_length_mesc;

    int ret = ZEN_Timer_Queue::initialize(num_timer_node, timer_precision_mesc,
                                          trigger_mode, dynamic_expand_node);
    if (ret != 0) {
        return ret;
    }

    num_wheel_point_ = timer_length_mesc_ / timer_precision_mesc + 1;
    timer_wheel_point_.resize(num_wheel_point_);
    std::fill(timer_wheel_point_.begin(), timer_wheel_point_.end(),
              ZEN_Timer_Queue::INVALID_TIMER_ID);
    proc_wheel_start_ = 0;
    return ret;
}

void ZEN_Timer_Wheel::bind_wheel_listnode(int time_node_id) {
    ZEN_TIMER_NODE *time_node = &time_node_ary_[time_node_id];
    // 商先按 32 位截断（二进制 __udivdi3 后仅用低 32 位再加 proc_wheel_start_）。
    uint32_t wheel_offset = (uint32_t)((time_node->next_trigger_point_
                                        - prev_trigger_msec_) / timer_precision_mesc_);
    int wheel_point_id = (int)((wheel_offset + proc_wheel_start_)
                               % num_wheel_point_);

    int old_head = timer_wheel_point_[wheel_point_id];
    timer_wheel_point_[wheel_point_id] = time_node_id;
    wheel_node_list_[time_node_id].wheel_point_id_ = wheel_point_id;
    if (old_head != ZEN_Timer_Queue::INVALID_TIMER_ID) {
        wheel_node_list_[old_head].list_prev_ = time_node_id;
        wheel_node_list_[time_node_id].list_next_ = old_head;
    }
}

void ZEN_Timer_Wheel::unbind_wheel_listnode(int time_node_id) {
    int wheel_point_id = wheel_node_list_[time_node_id].wheel_point_id_;
    int *head_node = &timer_wheel_point_[wheel_point_id];
    if (*head_node == time_node_id) {
        *head_node = wheel_node_list_[time_node_id].list_next_;
    } else {
        int prev_node = wheel_node_list_[time_node_id].list_prev_;
        int next_node = wheel_node_list_[time_node_id].list_next_;
        if (prev_node != ZEN_Timer_Queue::INVALID_TIMER_ID) {
            wheel_node_list_[prev_node].list_next_ = next_node;
        }
        if (next_node != ZEN_Timer_Queue::INVALID_TIMER_ID) {
            wheel_node_list_[next_node].list_prev_ = prev_node;
        }
    }
    wheel_node_list_[time_node_id].list_prev_ =
        ZEN_Timer_Queue::INVALID_TIMER_ID;
    wheel_node_list_[time_node_id].list_next_ =
        ZEN_Timer_Queue::INVALID_TIMER_ID;
    wheel_node_list_[time_node_id].wheel_point_id_ =
        ZEN_Timer_Queue::INVALID_TIMER_ID;
}

int ZEN_Timer_Wheel::cancel_timer(int timer_id) {
    unbind_wheel_listnode(timer_id);
    return ZEN_Timer_Queue::cancel_timer(timer_id);
}

int ZEN_Timer_Wheel::reschedule_timer(int timer_id, uint64_t now_trigger_msec) {
    bool continue_trigger = false;
    calc_next_trigger(timer_id, now_trigger_msec, continue_trigger);
    if (continue_trigger == false) {
        return cancel_timer(timer_id);
    }
    unbind_wheel_listnode(timer_id);
    bind_wheel_listnode(timer_id);
    return 0;
}

int ZEN_Timer_Wheel::schedule_timer(ZEN_Timer_Handler *timer_hdl,
                                    const void *action,
                                    const ZEN_Time_Value &delay_time,
                                    const ZEN_Time_Value &interval_time) {
    int time_node_id = ZEN_Timer_Queue::INVALID_TIMER_ID;
    ZEN_TIMER_NODE *alloc_time_node = NULL;

    int ret = alloc_timernode(timer_hdl, action, delay_time, interval_time,
                              time_node_id, alloc_time_node);
    if (ret != 0) {
        return ZEN_Timer_Queue::INVALID_TIMER_ID;
    }
    bind_wheel_listnode(time_node_id);
    return time_node_id;
}

size_t ZEN_Timer_Wheel::dispatch_timer(const ZEN_Time_Value &now_time,
                                       uint64_t now_trigger_msec) {
    size_t dispatch_num = 0;

    if (now_trigger_msec < prev_trigger_msec_) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] ZEN_Timer_Wheel error. now_trigger_msec[%llu] < prev_trigger_msec_[%llu],may be you adjust systime time to past time.",
            now_trigger_msec, prev_trigger_msec_);
        prev_trigger_msec_ = now_trigger_msec;
        return 0;
    }

    uint64_t elapsed_msec = now_trigger_msec - prev_trigger_msec_;
    if (elapsed_msec > timer_length_mesc_) {
        ZEN_Trace_LogMsg::debug_alertex(
            "[zenlib] ZEN_Timer_Wheel alert. now_trigger_msec[%llu], prev_trigger_msec_[%llu],elapsed_msec[%llu],timer_length_mesc_[%llu],may be you adjust systime time to future time or you ,or wheel is too little ,or dispatch_timer or expire invoke too little.",
            now_trigger_msec, prev_trigger_msec_, elapsed_msec,
            (uint64_t)timer_length_mesc_);
        prev_trigger_msec_ = now_trigger_msec;
        return 0;
    }

    size_t dispatch_slot_num =
        (size_t)(elapsed_msec / timer_precision_mesc_);
    for (size_t i = 0; i < dispatch_slot_num; ++i) {
        int wheel_point_id =
            (int)((proc_wheel_start_ + i) % num_wheel_point_);
        int time_node_id = timer_wheel_point_[wheel_point_id];
        while (time_node_id != ZEN_Timer_Queue::INVALID_TIMER_ID) {
            ZEN_TIMER_NODE *time_node = &time_node_ary_[time_node_id];
            time_node->timer_handle_->handle_timeout(now_time,
                                                     time_node->action_);
            ++dispatch_num;
            if (time_node_ary_[time_node_id].timer_handle_ != 0) {
                reschedule_timer(time_node_id, now_trigger_msec);
            }
            time_node_id = timer_wheel_point_[wheel_point_id];
        }
    }

    if (dispatch_slot_num != 0) {
        prev_trigger_msec_ +=
            (uint64_t)dispatch_slot_num * timer_precision_mesc_;
        proc_wheel_start_ =
            (proc_wheel_start_ + dispatch_slot_num) % num_wheel_point_;
    }
    return dispatch_num;
}

int ZEN_Timer_Wheel::extend_node(size_t num_timer_node, size_t &old_num_node) {
    int ret = ZEN_Timer_Queue::extend_node(num_timer_node, old_num_node);
    if (ret != 0) {
        return ret;
    }

    ZEN_WHEEL_TIMER_NODE init_node;
    if (num_timer_node_ >= wheel_node_list_.size()) {
        wheel_node_list_.insert(wheel_node_list_.end(),
                                num_timer_node_ - wheel_node_list_.size(),
                                init_node);
    } else {
        wheel_node_list_.erase(wheel_node_list_.begin() + num_timer_node_,
                               wheel_node_list_.end());
    }
    return ret;
}

int ZEN_Timer_Wheel::get_frist_nodeid(int &first_node_id) {
    // 原版怪癖：扫描到第一个非空槽后既不回填 first_node_id 也不返回该 id，
    // 本函数恒返回 INVALID_TIMER_ID（first_node_id 恒为 -1）。
    first_node_id = ZEN_Timer_Queue::INVALID_TIMER_ID;
    if (num_timer_node_ != 0 && num_wheel_point_ != 0) {
        for (size_t i = 0; i < num_wheel_point_; ++i) {
            if (timer_wheel_point_[(proc_wheel_start_ + i) % num_wheel_point_]
                != ZEN_Timer_Queue::INVALID_TIMER_ID) {
                break;
            }
        }
    }
    return ZEN_Timer_Queue::INVALID_TIMER_ID;
}
