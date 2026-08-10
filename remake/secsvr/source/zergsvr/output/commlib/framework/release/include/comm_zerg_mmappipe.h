// Restored from zergsvr DWARF + disassembly (2026-08-10)
// Zerg_MMAP_BusPipe：comm_zerg_mmappipe.cpp 全部 13 符号已还原。
// 继承 ZEN_BusPipe_TwoWay，自身成员（zergsvr 布局，与 gunnersvr 不同）：
//   if_check_pthrad_(0x281c) zerg_svr_info_(0x281d) monitor_(0x2824) = 10280
//   （zergsvr 无 config_ 成员、无 is_monitor_uin 逻辑）
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_ZERG_MMAPPIPE_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_ZERG_MMAPPIPE_H_H_

#include <stddef.h>

#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_svrd_config.h"

struct Comm_Stat_Monitor;
struct Comm_Svrd_Config;
struct ZenINIPropertyTree;
namespace conf_zerg {
    struct zerg_config;
}

// sizeof = 10280
struct Zerg_MMAP_BusPipe : public ZEN_BusPipe_TwoWay { // line 22
    friend struct Comm_SvrdApp_NonCtrl;
public:
    typedef conf_zerg::zerg_config ZERG_CONFIG;
protected:
    bool if_check_pthrad_;            // 0x281c
    SERVICES_ID zerg_svr_info_;       // 0x281d
    Comm_Stat_Monitor *monitor_;      // 0x2824
public:
    static char send_buffer_[0x10000]; // line 28
    static Zerg_MMAP_BusPipe *zerg_bus_instance_; // line 38
public:
    Zerg_MMAP_BusPipe(); // line 41
    ~Zerg_MMAP_BusPipe(); // line 43
    int initialize(SERVICES_ID &svrinfo, size_t size_recv_pipe, size_t size_send_pipe,
                   size_t max_frame_len, bool if_restore, bool if_check_pthread); // line 53
    int getpara_from_zergcfg(const ZERG_CONFIG &zerg_config); // line 56
    int getpara_from_zergcfg(const ZenINIPropertyTree &zerglingcfg); // line 57
    int init_after_getcfg(size_t max_frame_len, bool if_restore); // line 63
    void get_mmapfile_name(char *mmapfile, size_t buflen); // line 66
    int pop_front_recvpipe(Comm_App_Frame *&frame); // line 70（头文件内联）
    int push_back_sendpipe(Comm_App_Frame *frame); // line 72（头文件内联）
    int pipe_sendbuf_to_service(unsigned int cmd, unsigned int qquin,
                                unsigned int transaction_id, unsigned int backfill_trans_id,
                                const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc,
                                const SERVICES_ID &sendsvc, const unsigned char *buf,
                                size_t buf_len, unsigned int app_id, unsigned int option); // line 127
    template<typename T>
    int pipe_sendmsg_to_service(unsigned int cmd, unsigned int qquin,
                                unsigned int transaction_id, unsigned int backfill_trans_id,
                                const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc,
                                const SERVICES_ID &sendsvc, const T &info,
                                unsigned int app_id, unsigned int option);
    static void instance(Zerg_MMAP_BusPipe *pinstatnce); // line 136
    static Zerg_MMAP_BusPipe * instance(); // line 134
    static void clean_instance(); // line 138
};

// 头文件内联实现（gunnersvr 无 out-of-line 符号）
inline int Zerg_MMAP_BusPipe::pop_front_recvpipe(Comm_App_Frame *&frame) {
    frame = (Comm_App_Frame *)send_buffer_;
    return ZEN_BusPipe_TwoWay::pop_front_recvpipe((ZEN_LIB::dequechunk_node *)send_buffer_);
}

inline int Zerg_MMAP_BusPipe::push_back_sendpipe(Comm_App_Frame *frame) {
    return ZEN_BusPipe_TwoWay::push_back_sendpipe((ZEN_LIB::dequechunk_node *)frame);
}

// 头文件模板（gunnersvr 弱符号，comm_bill_record.cpp 实例化）
template<typename T>
int Zerg_MMAP_BusPipe::pipe_sendmsg_to_service(unsigned int cmd, unsigned int qquin,
                                               unsigned int transaction_id,
                                               unsigned int backfill_trans_id,
                                               const SERVICES_ID &rcvsvc,
                                               const SERVICES_ID &proxysvc,
                                               const SERVICES_ID &sendsvc,
                                               const T &info, unsigned int app_id,
                                               unsigned int option) {
    Comm_App_Frame *send_frame = (Comm_App_Frame *)send_buffer_;
    int ret = 0;
    send_frame->init_framehead(0x10000, option, cmd);
    send_frame->frame_uin_ = qquin;
    send_frame->app_id_ = app_id;
    send_frame->send_service_ = sendsvc;
    send_frame->proxy_service_ = proxysvc;
    send_frame->recv_service_ = rcvsvc;
    send_frame->transaction_id_ = transaction_id;
    send_frame->backfill_trans_id_ = backfill_trans_id;
    if (send_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_framehead(send_frame, "[TRACK MONITOR][Send]",
                                             RS_ERROR);
    }
    ret = send_frame->appdata_encode<T>(0xffbd, info, 0, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] [%s]TDR encode fail.ret =%d,Please check your code and buffer len.",
            "int Zerg_MMAP_BusPipe::pipe_sendmsg_to_service(unsigned int, unsigned int, unsigned int, unsigned int, const SERVICES_ID&, const SERVICES_ID&, const SERVICES_ID&, const T&, unsigned int, unsigned int)",
            ret);
        return ret;
    }
    if (send_frame->frame_length_ - 0x32 > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            send_frame->frame_length_);
        return 0x13b1;
    }
    if (!bus_pipe_pointer_[1]->push_end((const ZEN_LIB::dequechunk_node *)send_frame)) {
        ZEN_Trace_LogMsg::debug_alertex(
            "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
            1, send_frame->frame_length_, bus_pipe_pointer_[1]->freesize());
        monitor_->increase_by_statid(0x238f, send_frame->app_id_,
                                     send_frame->frame_command_, 1);
        return 0x1396;
    }
    monitor_->increase_by_statid(0x238e, send_frame->app_id_,
                                 send_frame->frame_command_, 1);
    monitor_->increase_by_statid(0x239c, send_frame->app_id_,
                                 send_frame->frame_command_,
                                 send_frame->frame_length_);
    return 0;
}

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_ZERG_MMAPPIPE_H_H_
