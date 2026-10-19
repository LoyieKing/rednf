// Canonical layout of ZEN_Timer_Queue / ZEN_Timer_Wheel (zenlib timer wheels).
//
// ORIG evidence (audit/tu_report.py --type + DWARF definition DIEs in CU
// zen_timer_queue_base.cpp):
//   ZEN_Timer_Queue size=60, bases: ZEN_NON_Copyable @0 (EBO)
//     +0    <vptr>                  (polymorphic; vfuncs are declared in the
//                                    ORIG base header, this TU only carries
//                                    the vptr)
//     +4    num_timer_node_                 size_t
//     +8    timer_precision_mesc_           unsigned int
//     +12   num_use_node_                   size_t
//     +16   trigger_mode_                   enum TRIGGER_MODE
//     +20   dynamic_expand_node_            bool        (padding 21..23)
//     +24   time_node_ary_        std::vector<ZEN_TIMER_NODE>  (12)
//     +36   free_node_id_head_              int
//     +40   free_node_id_list_    std::vector<int>            (12)
//     +52   prev_trigger_msec_              uint64_t            (8)
//   ZEN_Timer_Queue::ZEN_TIMER_NODE size=28
//     +0 time_id_ int, +4 interval_time_ ZEN_Time_Value (8),
//     +12 action_ <fn ptr>, +16 timer_handle_ <ptr>,
//     +20 next_trigger_point_ uint64_t
//   enum TRIGGER_MODE { TRIGGER_MODE_SYSTEM_CLOCK = 1, TRIGGER_MODE_CPU_TICK = 2 }
//   ZEN_Timer_Wheel size=96, base ZEN_Timer_Queue @0
//     +60   timer_length_mesc_              unsigned int
//     +64   num_wheel_point_                size_t
//     +68   timer_wheel_point_    std::vector<...>  (12)
//     +80   proc_wheel_start_               size_t
//     +84   wheel_node_list_      std::vector<...>  (12)
//   The model prints the four std::vector members as `anon12` sub-objects
//   because GCC 4.1 emitted their instantiations unnamed; the DWARF member
//   names above are the real ones.
//
// ZEN_Time_Value comes from the shared zenlib declaration
// (commlib/zenlib/zen_recon_support.h).
#ifndef TSS_SDK_LAYOUT_ZEN_TIMER_QUEUE_H_
#define TSS_SDK_LAYOUT_ZEN_TIMER_QUEUE_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>

#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

class ZEN_Timer_Queue : public ZEN_NON_Copyable {
public:
    // Nested in ORIG: the mangled names are
    //   _ZN13ZEN_Timer_Queue10initializeEjjNS_12TRIGGER_MODEEb
    //   _ZN14ZEN_Timer_WheelC1EjjjN13ZEN_Timer_Queue12TRIGGER_MODEEb
    enum TRIGGER_MODE {
        TRIGGER_MODE_SYSTEM_CLOCK = 1,
        TRIGGER_MODE_CPU_TICK = 2
    };

    struct ZEN_TIMER_NODE {          // 28 bytes
        int time_id_;                       // +0x00
        ZEN_Time_Value interval_time_;      // +0x04
        void (*action_)(void *);            // +0x0c
        void *timer_handle_;                // +0x10
        uint64_t next_trigger_point_;       // +0x14
    };
    // ORIG is polymorphic (vtable 0x002d5c30, first slot the virtual
    // destructor), so the vptr is the real +0x00 slot; declaring the virtual
    // destructor reproduces it.  The remaining ORIG virtuals (schedule_timer,
    // cancel_timer, get_frist_nodeid, dispatch_timer, expire, extend_node,
    // close, reschedule_timer) do not change the layout and are not called by
    // the reconstruction, so they are not re-declared here.
    virtual ~ZEN_Timer_Queue();

    // Two distinct ORIG symbols: _ZN13ZEN_Timer_Queue8instanceEv and
    // _ZN13ZEN_Timer_Queue8instanceEPS_ (the setter overload).
    static ZEN_Timer_Queue *instance();
    static ZEN_Timer_Queue *instance(ZEN_Timer_Queue *set);

    // Member order is the DWARF order of the ORIG definition DIE
    // (_ZN13ZEN_Timer_Queue... / CU zen_timer_queue_base.cpp): the previous
    // revision declared the two vectors first and carried a hand-written
    // `void *vptr_` at +0x24, which put every member at the wrong offset.
    size_t num_timer_node_;                       // +0x04
    unsigned int timer_precision_mesc_;           // +0x08
    size_t num_use_node_;                         // +0x0c
    TRIGGER_MODE trigger_mode_;                   // +0x10
    bool dynamic_expand_node_;                    // +0x14
    std::vector<ZEN_TIMER_NODE> time_node_ary_;   // +0x18
    int free_node_id_head_;                       // +0x24
    std::vector<int> free_node_id_list_;          // +0x28
    uint64_t prev_trigger_msec_;                  // +0x34
};

class ZEN_Timer_Wheel : public ZEN_Timer_Queue {
public:
    ZEN_Timer_Wheel();
    // ORIG 0x001b0970 (_ZN14ZEN_Timer_WheelC1EjjjN13ZEN_Timer_Queue12TRIGGER_MODEEb);
    // the default constructor is at 0x001b0870.
    ZEN_Timer_Wheel(unsigned int timer_length_mesc, unsigned int max_wheel_point,
                    unsigned int precision_mesc, TRIGGER_MODE trigger_mode,
                    bool dynamic_expand_node);

    struct ZEN_WHEEL_TIMER_NODE {    // 12 bytes
        int list_prev_;                 // +0x00
        int list_next_;                 // +0x04
        int wheel_point_id_;            // +0x08
    };

private:
    unsigned int timer_length_mesc_;              // +0x3c
    size_t num_wheel_point_;                      // +0x40
    std::vector<ZEN_WHEEL_TIMER_NODE> timer_wheel_point_;   // +0x44
    size_t proc_wheel_start_;                     // +0x50
    std::vector<ZEN_WHEEL_TIMER_NODE> wheel_node_list_;     // +0x54
};

TSS_SDK_STATIC_ASSERT(sizeof(ZEN_Timer_Queue::ZEN_TIMER_NODE) == 28,
                      zen_timer_node_size_must_be_28);
TSS_SDK_STATIC_ASSERT(sizeof(ZEN_Timer_Queue) == 60,
                      zen_timer_queue_size_must_be_60);
TSS_SDK_STATIC_ASSERT(sizeof(ZEN_Timer_Wheel) == 96,
                      zen_timer_wheel_size_must_be_96);

#endif  // TSS_SDK_LAYOUT_ZEN_TIMER_QUEUE_H_
