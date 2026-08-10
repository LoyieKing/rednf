// Reconstructed from zergsvr disassembly (2026-08-10).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_comm_manager.cpp

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>

#include "src/commlib/zenlib/zen_event_handle_base.h"

#include "src/commsvr/zergsvr/zerg_predefine.h"
#include "src/commsvr/zergsvr/zerg_comm_manager.h"
#include "src/commsvr/zergsvr/zerg_configure.h"
#include "src/commsvr/zergsvr/zerg_accept_handler.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_udp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "src/commsvr/zergsvr/zerg_app_handler.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_bus_mmap_pipe.h"
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"

namespace {

// 原代码直接取 ZEN_Bus_MMAPPipe::bus_pipe_pointer_[1]（send 管道，偏移 0x2010）。
// 基类成员为 protected，Zerg_Comm_Manager 不是子类；这里用局部派生类做同语义访问，
// 内联后机器码与原二进制一致（mov 0x2010(%reg),%eax + shm_dequechunk 调用）。
struct Zerg_SendPipe_Accessor : ZEN_Bus_MMAPPipe {
    static ZEN_LIB::shm_dequechunk *send_pipe(Zerg_MMAP_BusPipe *pipe) {
        return ((Zerg_SendPipe_Accessor *)pipe)->bus_pipe_pointer_[1];
    }
};

} // namespace

Zerg_Comm_Manager *Zerg_Comm_Manager::instance_ = NULL;

// line 13
Zerg_Comm_Manager::Zerg_Comm_Manager() {
    error_try_num_ = 3;
    monitor_size_ = 0;
    zerg_mmap_pipe_ = NULL;
    zbuffer_storage_ = NULL;
    server_status_ = NULL;
    send_frame_count_ = 0;
    zerg_mmap_pipe_ = Zerg_MMAP_BusPipe::instance();
    zbuffer_storage_ = ZBuffer_Storage::instance();
    server_status_ = Comm_Stat_Monitor::instance();
    count_start_time_ = (unsigned int)Zerg_App_Timer_Handler::now_time_.sec();
    memset(monitor_cmd_, 0, sizeof(monitor_cmd_));
}

// line 30
Zerg_Comm_Manager::~Zerg_Comm_Manager() {
    for (size_t i = 0; i < zerg_acceptor_.size(); i++) {
        // 原类析构为 protected，经 ZEN_Event_Handler 基类（public 虚析构）删除，
        // 机器码同为 vtable[3] 的 deleting destructor（call *0x18(%edx)）。
        delete (ZEN_Event_Handler *)zerg_acceptor_[i];
    }
    for (size_t i = 0; i < zerg_updsvc_.size(); i++) {
        delete (ZEN_Event_Handler *)zerg_updsvc_[i];
    }
}

// line 307
void Zerg_Comm_Manager::check_freamcount(unsigned int now) {
    unsigned int interval;
    unsigned int frame_per_sec;

    if (count_start_time_ >= now) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Zerg_Comm_Manager::check_freamcount time err.now:%u, count_start_time_:%u",
            now, count_start_time_);
        return;
    }
    interval = now - count_start_time_;
    frame_per_sec = send_frame_count_ / interval;
    if (frame_per_sec > SEND_FRAME_ALERT_VALUE) {
        ZEN_Trace_LogMsg::debug_alertex(
            "[zergsvr] Zerg_Comm_Manager::check_freamcount ALERT frame_per_sec:%u, send_frame_count_:%u, interval:%u.",
            frame_per_sec, send_frame_count_, interval);
        send_frame_count_ = 0;
        count_start_time_ = now;
    } else {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Zerg_Comm_Manager::check_freamcount frame_per_sec:%u, send_frame_count_:%u, interval:%u.",
            frame_per_sec, send_frame_count_, interval);
        send_frame_count_ = 0;
        count_start_time_ = now;
    }
}

// line 218
int Zerg_Comm_Manager::popall_sendpipe_write(size_t want_send_frame, size_t &num_send_frame) {
    int ret;
    ZByteBuffer *tmpbuf;
    Comm_App_Frame *proc_frame;
    size_t i;

    num_send_frame = 0;
    while (!Zerg_SendPipe_Accessor::send_pipe(zerg_mmap_pipe_)->empty()
           && num_send_frame < want_send_frame) {
        tmpbuf = zbuffer_storage_->allocate_buffer();
        proc_frame = (Comm_App_Frame *)tmpbuf->get_use_point();
        if (!Zerg_SendPipe_Accessor::send_pipe(zerg_mmap_pipe_)->empty()) {
            Zerg_SendPipe_Accessor::send_pipe(zerg_mmap_pipe_)
                ->pop_front((ZEN_LIB::dequechunk_node *)proc_frame);
        }
        tmpbuf->size_of_use_ = *(unsigned int *)proc_frame;
        if (proc_frame->frame_option_ & 0x10000) {
            Comm_App_Frame::dumpoutput_framehead(proc_frame, "[TRACK MONITOR][SEND]",
                                                 RS_INFO);
        }
        for (i = 0; i < monitor_size_; i++) {
            if (proc_frame->frame_command_ == monitor_cmd_[i]) {
                proc_frame->frame_option_ |= 0x10000;
                Comm_App_Frame::dumpoutput_framehead(proc_frame,
                                                     "[TRACK MONITOR][SEND]",
                                                     RS_INFO);
            }
        }
        if (proc_frame->frame_option_ & 0x80) {
            UDP_Svc_Handler::send_all_to_udp(proc_frame);
        } else {
            ret = TCP_Svc_Handler::process_send_data(tmpbuf);
            if (ret != 0) {
                if (proc_frame->frame_option_ & 0x4) {
                    ZEN_Trace_LogMsg::debug_errorex(
                        "[zergsvr] A Frame frame len[%u] cmd[%u] uin[%u] recv_service[%u|%u] proxy_service[%u|%u] send_service[%u|%u] option [%u],ret =%d Discard!",
                        proc_frame->frame_length_, proc_frame->frame_command_,
                        proc_frame->frame_command_, proc_frame->recv_service_.services_type_,
                        proc_frame->recv_service_.services_id_,
                        proc_frame->proxy_service_.services_type_,
                        proc_frame->proxy_service_.services_id_,
                        proc_frame->send_service_.services_type_,
                        proc_frame->send_service_.services_id_,
                        proc_frame->frame_option_, ret);
                }
                server_status_->increase_by_statid(0x2335, proc_frame->app_id_,
                                                   0, 1);
            } else {
                send_frame_count_++;
            }
        }
        zbuffer_storage_->free_byte_buffer(tmpbuf);
        num_send_frame++;
    }
    return 0;
}

// line 46
int Zerg_Comm_Manager::get_config(const Zerg_MMAP_BusPipe::ZERG_CONFIG &config) {
    int ret = 0;
    const size_t LEN_TMP_BUFFER = 0x101;
    char err_buf[LEN_TMP_BUFFER];
    size_t i;

    memset(err_buf, 0, LEN_TMP_BUFFER);
    error_try_num_ = 0;
    memset(monitor_cmd_, 0, sizeof(monitor_cmd_));
    error_try_num_ = config.comm_cfg.retry_error;
    if (error_try_num_ > 9) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Get configure file error. %s.", err_buf);
        return 0x1398;
    }
    monitor_size_ = config.monitor_cfg.monitor_num;
    if (monitor_size_ > MAX_NUMBER_OF_MONITOR_FRAME) {
        ZEN_Trace_LogMsg::debug_errorex("MONITORCMD|NUMMONITOR key error.");
        return 0x1398;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Zerg_Comm_Manager::get_config monitor_size_ = %u",
        monitor_size_);
    if (monitor_size_ != 0) {
        monitor_cmd_[0] = config.monitor_cfg.monitor_cmds[0];
        if (monitor_cmd_[0] == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Get configure file error. %s.", err_buf);
            return 0x1398;
        }
        for (i = 1; i < monitor_size_; i++) {
            monitor_cmd_[i] = config.monitor_cfg.monitor_cmds[i];
            if (monitor_cmd_[i] == 0) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[framework] Get configure file error. %s.", err_buf);
                return 0x1398;
            }
        }
    }
    return 0;
}

// line 170
int Zerg_Comm_Manager::check_safeport(ZEN_Sockaddr_In &inetadd) {
    const unsigned short UNSAFE_PORT1 = 0x400;  // 1024
    const unsigned short UNSAFE_PORT2 = 0x50;   // 80
    const unsigned short UNSAFE_PORT3 = 0xcea;  // 3306
    const unsigned short UNSAFE_PORT4 = 0x8ca0; // 36000
    const unsigned short SAFE_PORT1 = 0xdac0;   // 56000
    unsigned short port = inetadd.get_port_number();

    if (port <= UNSAFE_PORT1 || port == UNSAFE_PORT2 || port == UNSAFE_PORT3
        || port == UNSAFE_PORT4 || port == SAFE_PORT1) {
        if (Zerg_Server_Config::instance()->zerg_insurance_) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Unsafe port %u,if you need to open this port,please close insurance. ",
                port);
            return 0x114933ae;
        }
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Warn!Warn! Unsafe port %u.Please notice! ", port);
    }
    return 0;
}

// line 100
int Zerg_Comm_Manager::init_socketpeer(ZERG_SERVICES_INFO &init_svcid) {
    int ret;
    UDP_Svc_Handler *tmp_udphdl;
    TCP_Accept_Handler *tmp_acceptor;

    ret = check_safeport(init_svcid.zerg_ip_addr_);
    if (ret != 0) {
        return ret;
    }
    if (init_svcid.zerg_svc_info_.services_type_ > 0x752f) {
        tmp_udphdl = new UDP_Svc_Handler(init_svcid.zerg_svc_info_,
                                         init_svcid.zerg_ip_addr_,
                                         init_svcid.zerg_sessionkey_);
        ret = tmp_udphdl->init_udp_services();
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Init udp scoket fail ret = %d.", ret);
            return 0x114933af;
        }
        zerg_updsvc_.push_back(tmp_udphdl);
        return 0;
    }
    tmp_acceptor = new TCP_Accept_Handler(init_svcid.zerg_svc_info_,
                                          init_svcid.zerg_ip_addr_,
                                          init_svcid.zerg_sessionkey_);
    ret = tmp_acceptor->create_listen();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Init tcp accept scoket fail ret = %d.error[%u|%s]",
            ret, errno, strerror(errno));
        return ret;
    }
    zerg_acceptor_.push_back(tmp_acceptor);
    return 0;
}

// line 341
Zerg_Comm_Manager * Zerg_Comm_Manager::instance() {
    if (instance_ == NULL) {
        instance_ = new Zerg_Comm_Manager();
    }
    return instance_;
}

// line 353
void Zerg_Comm_Manager::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}
