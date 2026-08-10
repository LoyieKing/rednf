// Reconstructed from zergsvr DWARF + disassembly (2026-08-10, svc_udp).
// Original path: src/commsvr/zergsvr/zerg_udp_ctrl_handler.cpp
// UDP_Svc_Handler：UDP 控制连接处理器，类布局见 zerg_udp_ctrl_handler.h（sizeof=60）。
//
// 还原要点（函数级地址见 file_symbols.py）：
//  - 构造：ZEN_Event_Handler(ZEN_Reactor::instance())，拷贝 bind_addr/svr_info，
//    sessionkey_verify_=arg，dgram_databuf_=0，ip_restrict_=instance()，最后
//    ary_udpsvc_handler_.push_back(this)。
//  - init_all_static_data 只赋 3 个静态：server_status_/zerg_comm_mgr_/game_id_。
//  - get_udpctrl_conf 只做 if_proxy_ = (config->comm_cfg.is_proxy == 1)。
//  - zerg_comm_manager.h 是坏桩（monitor_cmd_[] 柔性数组使后置成员偏移错误），
//    本文件不 include；handle_input 中对 Zerg_Comm_Manager 的成员按二进制构造器
//    (0x0807ddf0) 核对过的偏移直接访问：monitor_size_@0x1c、monitor_cmd_@0x20、
//    zerg_mmap_pipe_@0x60、server_status_@0x68。
//  - comm_zerg_mmappipe.h 当前编译不过，禁止 include；Zerg_MMAP_BusPipe 仅前向声明，
//    收包队列 chunk 指针位于 mmap_pipe+0x200c（按 handle_input 反汇编）。
//  - read_data_from_udp 里 frame_option_ 低 16 位置 0x80 用 memcpy 保字写入
//    （二进制为 movw $0x80,4(%edi)，高 16 位保留线上值）。

#include "src/commsvr/zergsvr/zerg_udp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "src/commsvr/zergsvr/zerg_ip_restrict.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include "src/commlib/zenlib/zen_os_adapt_error.h"
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commsvr/zergsvr/zerg_comm_manager.h"
#include "src/protocol/common/comm_conf_zerg.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>

const size_t UDP_Svc_Handler::ONCE_MAX_READ_UDP_NUMBER = 0x100;
std::vector<UDP_Svc_Handler *> UDP_Svc_Handler::ary_udpsvc_handler_;
UDPSessionKeyMgr *UDP_Svc_Handler::udp_session_mgr_;
Comm_Stat_Monitor *UDP_Svc_Handler::server_status_;
Zerg_Comm_Manager *UDP_Svc_Handler::zerg_comm_mgr_;
bool UDP_Svc_Handler::if_proxy_;
unsigned int UDP_Svc_Handler::game_id_;

UDP_Svc_Handler::UDP_Svc_Handler(const SERVICES_ID &svr_info,
                                 const ZEN_Sockaddr_In &bind_addr,
                                 bool sessionkey_verify)
    : ZEN_Event_Handler(ZEN_Reactor::instance()),
      udp_bind_addr_(bind_addr),
      my_svc_info_(svr_info),
      sessionkey_verify_(sessionkey_verify),
      dgram_databuf_(0),
      ip_restrict_(Zerg_IPRestrict_Mgr::instance()) {
    ary_udpsvc_handler_.push_back(this);
}

UDP_Svc_Handler::~UDP_Svc_Handler() {
    if (dgram_databuf_ != 0) {
        ZBuffer_Storage::instance()->free_byte_buffer(dgram_databuf_);
    }
}

int UDP_Svc_Handler::init_udp_services() {
    int ret;
    const socklen_t opval = 0x50000;
    socklen_t opvallen = sizeof(opval);

    dgram_databuf_ = ZBuffer_Storage::instance()->allocate_buffer();
    ret = dgram_peer_.open(&udp_bind_addr_, AF_INET, 0, false);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] init_udp_services ,UDP bind ip address [%s|%u] fail.",
            udp_bind_addr_.get_host_addr(), udp_bind_addr_.get_port_number());
        handle_close();
        return 0x114933af;
    }

    dgram_peer_.sock_enable(O_NONBLOCK);
    dgram_peer_.setsockopt(SOL_SOCKET, SO_RCVBUF, &opval, opvallen);
    dgram_peer_.setsockopt(SOL_SOCKET, SO_SNDBUF, &opval, opvallen);
    ret = this->reactor()->register_handler(this, ZEN_EVENT_READ);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] init_udp_services ,UDP bind ip address [%s|%u] fail.",
            udp_bind_addr_.get_host_addr(), udp_bind_addr_.get_port_number());
        return 0x114933af;
    }

    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] init_udp_services ,UDP bind ip address [%s|%u] success.",
        udp_bind_addr_.get_host_addr(), udp_bind_addr_.get_port_number());
    return 0;
}

ZEN_SOCKET UDP_Svc_Handler::get_handle() const {
    return dgram_peer_.get_handle();
}

int UDP_Svc_Handler::handle_close() {
    if (dgram_peer_.get_handle() != (ZEN_SOCKET)-1) {
        ZEN_Event_Handler::handle_close();
        dgram_peer_.close();
    }
    delete this;
    return 0;
}

int UDP_Svc_Handler::write_data_to_udp(Comm_App_Frame *send_frame) {
    ssize_t szsend;
    ZEN_Sockaddr_In remote_addr(send_frame->recv_service_.services_id_,
                                send_frame->recv_service_.services_type_);
    size_t send_len = send_frame->frame_length_;

    send_frame->framehead_encode();
    szsend = dgram_peer_.sendto(send_frame, send_len, 0, &remote_addr);
    if (szsend > 0) {
        ZEN_Trace_LogMsg::debug_debugEx(
            "[zergsvr] UDP send data success. peer IP [%s|%u] handle:%u send len :%u.",
            remote_addr.get_host_addr(), remote_addr.get_port_number(),
            dgram_peer_.get_handle(), send_len);
        server_status_->increase_by_statid(0x2339, game_id_, 0, 1);
        server_status_->increase_by_statid(0x233b, game_id_, 0, szsend);
        return 0;
    }

    const char *error_str = strerror(errno);
    ZEN_Trace_LogMsg::debug_errorex(
        "[zergsvr] UDP send data error. peer IP [%s|%u] handle:%u ZEN_OS::last_error()=%d|%s.",
        remote_addr.get_host_addr(), remote_addr.get_port_number(),
        dgram_peer_.get_handle(), errno, error_str);
    return 0x11493391;
}

int UDP_Svc_Handler::send_all_to_udp(Comm_App_Frame *send_frame) {
    for (size_t i = 0; i < ary_udpsvc_handler_.size(); i++) {
        if (ary_udpsvc_handler_[i]->my_svc_info_ == send_frame->send_service_) {
            return ary_udpsvc_handler_[i]->write_data_to_udp(send_frame);
        }
    }
    ZEN_Trace_LogMsg::debug_errorex("[zergsvr] Error UDP Send Svc Info %u|%u.",
                                    send_frame->send_service_.services_type_,
                                    send_frame->send_service_.services_id_);
    return 0x114933a6;
}

int UDP_Svc_Handler::read_data_from_udp(size_t &size_revc) {
    int ret;
    ssize_t recvret;
    ZEN_Sockaddr_In remote_addr;

    size_revc = 0;
    recvret = dgram_peer_.recvfrom(dgram_databuf_->buffer_data_,
                                   Comm_App_Frame::MAX_LEN_OF_APPFRAME_DATA,
                                   0, &remote_addr);
    if (recvret < 0) {
        if (errno == EAGAIN || errno == EINVAL) {
            return 0;
        }
        const char *error_str = strerror(errno);
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] UDP receive data error IP[%s|%u] peer:%u ZEN_OS::last_error()=%d|%s.",
            remote_addr.get_host_addr(), remote_addr.get_port_number(),
            dgram_peer_.get_handle(), errno, error_str);
        return 0x11493391;
    }

    ret = ip_restrict_->check_iprestrict(remote_addr);
    if (ret != 0) {
        return ret;
    }
    if (recvret == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] UDP Peer IP [%s|%u] recv return 0, I don't know how to process.?",
            remote_addr.get_host_addr(), remote_addr.get_port_number());
        return 0;
    }

    server_status_->increase_by_statid(0x2338, game_id_, 0, 1);
    server_status_->increase_by_statid(0x233a, game_id_, 0, recvret);

    Comm_App_Frame *proc_frame = (Comm_App_Frame *)dgram_databuf_->buffer_data_;
    proc_frame->framehead_decode();
    if (proc_frame->frame_length_ != (unsigned int)recvret) {
        return 0x114933a6;
    }

    proc_frame->send_ip_address_ = remote_addr.get_ip_address();
    proc_frame->send_service_.services_type_ = remote_addr.get_port_number();
    proc_frame->send_service_.services_id_ = remote_addr.get_ip_address();
    proc_frame->recv_service_ = my_svc_info_;
    // 二进制为 movw $0x80,4(%edi)：仅写低 16 位，高 16 位保留线上值。
    const unsigned short recv_frame_option = 0x80;
    memcpy(&proc_frame->frame_option_, &recv_frame_option, sizeof(recv_frame_option));
    size_revc = proc_frame->frame_length_;

    ZEN_Trace_LogMsg::debug_debugEx(
        "[zergsvr] UDP recviese data success. peer IP [%s|%u] handle:%u .recv len :%u.",
        remote_addr.get_host_addr(), remote_addr.get_port_number(),
        dgram_peer_.get_handle(), size_revc);
    return 0;
}

int UDP_Svc_Handler::handle_input() {
    int ret;
    size_t i;
    size_t szrevc;

    for (i = 0; i < ONCE_MAX_READ_UDP_NUMBER; i++) {
        szrevc = 0;
        ret = read_data_from_udp(szrevc);
        if (ret != 0) {
            break;
        }
        if (szrevc == 0) {
            continue;
        }

        Comm_App_Frame *proc_frame = (Comm_App_Frame *)dgram_databuf_->buffer_data_;
        Zerg_Comm_Manager *zerg_mgr = zerg_comm_mgr_;
        if (proc_frame->frame_command_ - 0x11490c80 <= 0x270f) {
            continue;
        }
        if ((proc_frame->frame_option_ & 0x10000) == 0) {
            for (size_t j = 0; j < zerg_mgr->monitor_size_; j++) {
                if (zerg_mgr->monitor_cmd_[j] == proc_frame->frame_command_) {
                    proc_frame->frame_option_ |= 0x10000;
                    Comm_App_Frame::dumpoutput_framehead(proc_frame,
                                                         "[TRACK MONITOR][RECV]",
                                                         RS_INFO);
                }
            }
        }

        ZEN_LIB::shm_dequechunk *recv_chunk =
            zerg_mgr->zerg_mmap_pipe_->bus_pipe_pointer_[1];
        Comm_Stat_Monitor *status = zerg_mgr->server_status_;
        if (recv_chunk->push_end((const ZEN_LIB::dequechunk_node *)proc_frame)) {
            status->increase_by_statid(0x232f, proc_frame->app_id_, 0, 1);
        } else {
            size_t free_size = recv_chunk->freesize();
            ZEN_Trace_LogMsg::debug_alertex(
                "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. "
                "Please increase and check. nodesize=%u, freesize=%u",
                0, proc_frame->frame_length_, free_size);
            status->increase_by_statid(0x233e, proc_frame->app_id_, 0, 1);
        }
    }
    return 0;
}

int UDP_Svc_Handler::init_all_static_data() {
    server_status_ = Comm_Stat_Monitor::instance();
    zerg_comm_mgr_ = Zerg_Comm_Manager::instance();
    game_id_ = CfgSvrSdk::instance()->get_game_id();
    return 0;
}

int UDP_Svc_Handler::get_udpctrl_conf(const conf_zerg::zerg_config *config) {
    if_proxy_ = (config->comm_cfg.is_proxy == 1);
    return 0;
}
