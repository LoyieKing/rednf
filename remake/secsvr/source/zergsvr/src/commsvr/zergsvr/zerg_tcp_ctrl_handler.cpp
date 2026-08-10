// Reconstructed from zergsvr binary (DWARF + disassembly).
// TCP_Svc_Handler：TCP 服务连接处理器（accept/connect 两种模式共用池）。

#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"

#include <errno.h>
#include <netinet/tcp.h>
#include <string.h>

#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_cfgsvr_sdk.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include "src/commlib/zenlib/zen_trace_log_debug.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "src/commsvr/zergsvr/zerg_comm_manager.h"

// ---- 静态成员定义 ----
const int TCP_Svc_Handler::TCPCTRL_TIME_ID[] = {1, 2};
const size_t TCP_Svc_Handler::MAX_OF_ACCEPT_PEER_SEND_DEQUE = 0x100;
const size_t TCP_Svc_Handler::MAX_OF_CONNECT_PEER_SEND_DEQUE = 0x100;
const size_t TCP_Svc_Handler::MAX_OF_SPEC_NO_ENCRYPT_CMD = 0;
const unsigned int TCP_Svc_Handler::DEFAULT_TIME_OUT_SEC = 30;
const unsigned int TCP_Svc_Handler::STAT_TIMER_INTERVAL_SEC = 30;
const size_t TCP_Svc_Handler::MAX_SESSION_KEY_LEN = 0x4000;

Zerg_Comm_Manager *TCP_Svc_Handler::zerg_comm_mgr_ = 0;
ZBuffer_Storage *TCP_Svc_Handler::zbuffer_storage_ = 0;
Comm_Stat_Monitor *TCP_Svc_Handler::server_status_ = 0;
size_t TCP_Svc_Handler::max_accept_svr_ = 0;
size_t TCP_Svc_Handler::max_connect_svr_ = 0;
size_t TCP_Svc_Handler::accpet_threshold_warn_ = 0;
size_t TCP_Svc_Handler::threshold_warn_number_ = 0;
bool TCP_Svc_Handler::if_proxy_ = false;
unsigned int TCP_Svc_Handler::max_frame_len_ = 0;
unsigned int TCP_Svc_Handler::connect_timeout_ = 0;
unsigned int TCP_Svc_Handler::receive_timeout_ = 0;
Zerg_Auto_Connector TCP_Svc_Handler::zerg_auto_connect_;
Service_Info_Set TCP_Svc_Handler::svr_peer_info_set_;
size_t TCP_Svc_Handler::num_accept_peer_ = 0;
size_t TCP_Svc_Handler::num_connect_peer_ = 0;
bool TCP_Svc_Handler::if_check_frame_ = false;
ZEN_LIB::lordrings<TCP_Svc_Handler *> TCP_Svc_Handler::pool_of_acpthdl_;
ZEN_LIB::lordrings<TCP_Svc_Handler *> TCP_Svc_Handler::pool_of_cnthdl_;
size_t TCP_Svc_Handler::snd_buf_size_ = 0;
size_t TCP_Svc_Handler::connect_send_deque_size_ = 0;
unsigned int TCP_Svc_Handler::handler_id_builder_ = 0;
unsigned int TCP_Svc_Handler::game_id_ = 0;

// ---- 构造 / 析构 ----

TCP_Svc_Handler::TCP_Svc_Handler(TCP_Svc_Handler::HANDLER_MODE handler_mode)
    : ZEN_Event_Handler(ZEN_Reactor::instance()),
      ZEN_Timer_Handler(ZEN_Timer_Queue::instance()),
      handler_mode_(handler_mode),
      my_svc_info_(0, 0),
      peer_svr_info_(0, 0),
      rcv_buffer_(0),
      recieve_counter_(0),
      send_counter_(0),
      recieve_bytes_(0),
      send_bytes_(0),
      peer_status_(PEER_STATUS_NOACTIVE),
      timeout_time_id_(-1),
      receive_times_(0),
      sessionkey_verify_(false),
      if_force_close_(false),
      start_live_time_(0) {
    if (handler_mode == HANDLER_MODE_CONNECT) {
        snd_buffer_deque_.resize((unsigned int)connect_send_deque_size_);
    } else if (handler_mode == HANDLER_MODE_ACCEPTED) {
        snd_buffer_deque_.resize((unsigned int)snd_buf_size_);
    }
}

TCP_Svc_Handler::~TCP_Svc_Handler() {
}

// ---- 简单访问器 ----

unsigned int TCP_Svc_Handler::get_handle_id() {
    return get_handle();
}

TCP_Svc_Handler::PEER_STATUS TCP_Svc_Handler::get_peer_status() {
    return peer_status_;
}

void TCP_Svc_Handler::get_max_peer_num(size_t &maxaccept, size_t &maxconnect) {
    maxaccept = max_accept_svr_;
    maxconnect = max_connect_svr_;
}

const ZEN_Sockaddr_In &TCP_Svc_Handler::get_peer_sockaddr() const {
    return peer_address_;
}

ZEN_SOCKET TCP_Svc_Handler::get_handle() const {
    return socket_peer_.get_handle();
}

short unsigned int TCP_Svc_Handler::get_peer_port() {
    return peer_address_.get_port_number();
}

const char *TCP_Svc_Handler::get_peer_address() {
    return peer_address_.get_host_addr();
}

// ---- 静态服务查找 / 关闭 ----

int TCP_Svc_Handler::find_services_peer(const SERVICES_ID &svr_info,
                                        TCP_Svc_Handler *&svchanle) {
    int ret = svr_peer_info_set_.find_services_peerinfo(svr_info, svchanle);
    return ret;
}

int TCP_Svc_Handler::close_services_peer(const SERVICES_ID &svr_info) {
    int ret;
    TCP_Svc_Handler *svchanle = 0;
    ret = svr_peer_info_set_.find_services_peerinfo(svr_info, svchanle);
    if (ret == 0) {
        svchanle->handle_close();
    }
    return ret;
}

// ---- 静态初始化 / 清理 ----

int TCP_Svc_Handler::init_all_static_data() {
    zerg_comm_mgr_ = Zerg_Comm_Manager::instance();
    zbuffer_storage_ = ZBuffer_Storage::instance();
    server_status_ = Comm_Stat_Monitor::instance();
    game_id_ = CfgSvrSdk::instance()->get_game_id();
    max_connect_svr_ = zerg_auto_connect_.numsvr_connect() + 0x10;
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Max connect svr number:%u.", max_connect_svr_);
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] allocate connect peer handle pool number:%u, size:%u.",
        max_connect_svr_, max_connect_svr_ * sizeof(TCP_Svc_Handler));

    pool_of_cnthdl_.resize((unsigned int)max_connect_svr_);
    for (size_t i = 0; i < max_connect_svr_; ++i) {
        TCP_Svc_Handler *p_handler = new TCP_Svc_Handler(HANDLER_MODE_CONNECT);
        pool_of_cnthdl_.push_back(p_handler, false);
    }

    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] allocate accept peer handle pool number:%u, size:%u.",
        max_accept_svr_, max_accept_svr_ * sizeof(TCP_Svc_Handler));
    pool_of_acpthdl_.resize((unsigned int)max_accept_svr_);
    for (size_t i = 0; i < max_accept_svr_; ++i) {
        TCP_Svc_Handler *p_handler = new TCP_Svc_Handler(HANDLER_MODE_ACCEPTED);
        pool_of_acpthdl_.push_back(p_handler, false);
    }

    svr_peer_info_set_.init_services_peerinfo((unsigned int)(max_connect_svr_ + max_accept_svr_ + 0x400));
    size_t szvalid = 0;
    size_t szsucc = 0;
    size_t szfail = 0;
    zerg_auto_connect_.reconnect_allserver(szvalid, szsucc, szfail);
    return 0;
}

int TCP_Svc_Handler::uninit_all_staticdata() {
    svr_peer_info_set_.clear_and_closeall();
    pool_of_cnthdl_.finit();
    return 0;
}

void TCP_Svc_Handler::auto_connect_allserver() {
    size_t szvalid = 0;
    size_t szsucc = 0;
    size_t szfail = 0;
    zerg_auto_connect_.reconnect_allserver(szvalid, szsucc, szfail);
}

// ---- 配置 ----

int TCP_Svc_Handler::get_tcpctrl_conf(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config) {
    receive_timeout_ = config->comm_cfg.recv_timeout;
    if (receive_timeout_ > 0x3c) {
        ZEN_Trace_LogMsg::debug_errorex("COMMCFG|CONNECTTIMEOUT key error.");
        return 0x1398;
    }
    connect_timeout_ = config->comm_cfg.connect_timeout;
    if (connect_timeout_ > 0x7d0) {
        ZEN_Trace_LogMsg::debug_errorex("COMMCFG|RECEIVETIMEOUT key error.");
        return 0x1398;
    }
    if_proxy_ = (config->comm_cfg.is_proxy == 1);
    snd_buf_size_ = config->comm_cfg.accept_send_buf_size;
    if (snd_buf_size_ > 0x64000) {
        ZEN_Trace_LogMsg::debug_errorex("COMMCFG|MAXACCEPTSVR key error.");
        return 0x1398;
    }
    if (snd_buf_size_ <= 0x32) {
        ZEN_Trace_LogMsg::debug_errorex("COMMCFG|MAXACCEPTSVR key error.");
        return 0x1398;
    }
    connect_send_deque_size_ = (size_t)((double)snd_buf_size_ * 0.5);
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Max accept svr number :%u,accept warn threshold number:%u.",
                                   connect_send_deque_size_);
    max_frame_len_ = config->comm_cfg.max_frame_len;
    if (max_frame_len_ > 0x10000) {
        ZEN_Trace_LogMsg::debug_errorex("COMMCFG|MAXFRAMELEN key error.");
        return 0x1398;
    }
    if (max_frame_len_ <= 0x400) {
        ZEN_Trace_LogMsg::debug_errorex("COMMCFG|MAXFRAMELEN key error.");
        return 0x1398;
    }
    if_check_frame_ = (config->check_cfg.check_frame == 1);
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] If check check frame :%d.", if_check_frame_ ? 1 : 0);
    max_accept_svr_ = config->comm_cfg.max_accept_svr;
    max_connect_svr_ = config->comm_cfg.connect_send_deque_size;
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] conncet send deque size :%d", max_connect_svr_);
    zerg_auto_connect_.reload_cfg(config);
    return 0;
}

// ---- 池分配 ----

TCP_Svc_Handler *TCP_Svc_Handler::AllocSvcHandlerFromPool(
    TCP_Svc_Handler::HANDLER_MODE handler_mode) {
    TCP_Svc_Handler *p_handler = 0;
    if (handler_mode == HANDLER_MODE_ACCEPTED) {
        if (pool_of_acpthdl_.size() > 0) {
            pool_of_acpthdl_.pop_front(p_handler);
        } else {
            ZEN_Trace_LogMsg::debug_infoex(
                "[zergsvr] accept handler pool is empty,capacity:%u.", pool_of_acpthdl_.capacity());
            p_handler = 0;
        }
    } else if (handler_mode == HANDLER_MODE_CONNECT) {
        if (pool_of_cnthdl_.size() > 0) {
            pool_of_cnthdl_.pop_front(p_handler);
        } else {
            p_handler = 0;
        }
    } else {
        p_handler = 0;
    }
    return p_handler;
}

// ---- 发送数据 ----

int TCP_Svc_Handler::process_send_error(ZByteBuffer *tmpbuf, bool frame_encode) {
    size_t use_start = tmpbuf->size_of_use_;
    tmpbuf->size_of_use_ = 0;
    size_t use_len = use_start;
    while (use_len > 0) {
        Comm_App_Frame *proc_frame = (Comm_App_Frame *)(tmpbuf->buffer_data_ + tmpbuf->size_of_use_);
        if (frame_encode) {
            proc_frame->framehead_decode();
        }
        if (use_len < (size_t)tmpbuf->size_of_buffer_ && (proc_frame->frame_option_ & 0x4)) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Connect peer ,send frame fail.frame len[%u] frame command[%u] frame uin[%u] snd svcid[%u|%u] proxy svc [%u|%u] recv[%u|%u] address[%s|%u],peer status[%u]. ",
                proc_frame->frame_length_, proc_frame->frame_command_, proc_frame->frame_uin_,
                proc_frame->send_service_.services_type_, proc_frame->send_service_.services_id_,
                proc_frame->proxy_service_.services_type_, proc_frame->proxy_service_.services_id_,
                proc_frame->recv_service_.services_type_, proc_frame->recv_service_.services_id_,
                get_peer_address(), get_peer_port(), (unsigned int)peer_status_);
        }
        server_status_->increase_by_statid(0x2335, game_id_, proc_frame->frame_command_, 1);
        tmpbuf->size_of_use_ += proc_frame->frame_length_;
        if (tmpbuf->size_of_use_ == use_len) {
            break;
        }
    }
    zbuffer_storage_->free_byte_buffer(tmpbuf);
    return 0;
}

int TCP_Svc_Handler::write_data_to_peer(size_t &szsend, bool &bfull) {
    bfull = false;
    szsend = 0;
    if (snd_buffer_deque_.size() == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Goto handle_output|write_data_to_peer ,but not data to send. Please check,buffer deque size=%u.",
            snd_buffer_deque_.size());
        _BACK_TRACE_STACK_SYMBOLS::back_trace_stack(RS_ERROR);
        this->reactor()->register_handler(this, ZEN_EVENT_WRITE);
        return 0;
    }

    ZByteBuffer *sndbuffer = 0;
    snd_buffer_deque_.pop_front(sndbuffer);
    ssize_t sendret = socket_peer_.send(sndbuffer->buffer_data_ + sndbuffer->size_of_use_,
                                        sndbuffer->size_of_buffer_ - sndbuffer->size_of_use_, 0);
    if (sendret > 0) {
        szsend = sendret;
        sndbuffer->size_of_use_ += sendret;
        send_bytes_ += sendret;
        if (sndbuffer->size_of_use_ == sndbuffer->size_of_buffer_) {
            bfull = true;
            ++send_counter_;
        }
        return 0;
    }

    if (errno == EAGAIN) {
        server_status_->increase_by_statid(0x2337, game_id_, 0, 1);
        return 0;
    }

    int errno_tmp = errno;
    ZEN_Trace_LogMsg::debug_errorex(
        "[zergsvr] Send data error,services[%u|%u] IP|Port [%s|%u],Peer:%d errno=%d|%s .",
        my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
        get_peer_port(), socket_peer_.get_handle(), errno_tmp, strerror(errno_tmp));
    server_status_->increase_by_statid(0x2335, game_id_, 0, 1);
    return 0x11493391;
}

int TCP_Svc_Handler::write_all_data_to_peer() {
    int ret = 0;
    while (snd_buffer_deque_.size() > 0) {
        size_t szsend = 0;
        bool bfull = false;
        ret = write_data_to_peer(szsend, bfull);
        if (ret != 0) {
            break;
        }
        if (bfull) {
            zbuffer_storage_->free_byte_buffer(snd_buffer_deque_[0]);
            snd_buffer_deque_.pop_front();
        }
    }
    if (ret == 0) {
        if (peer_status_ == PEER_STATUS_NOACTIVE) {
            process_connect_register();
            ret = 0;
        } else if (if_force_close_) {
            ret = handle_close();
            if (ret != -1) {
                ret = 0;
            }
        } else {
            ret = 0;
        }
    }
    return ret;
}

int TCP_Svc_Handler::put_frame_to_sendlist(ZByteBuffer *tmpbuf) {
    if (snd_buffer_deque_.full()) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] send buffer deque full, services[%u|%u] IP[%s|%u], deque size:%u.",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port(), snd_buffer_deque_.size());
        if_force_close_ = true;
        process_send_error(tmpbuf, true);
        handle_close();
        return 0x11493394;
    }
    if (!(tmpbuf->buffer_data_[0] & 0x10)) {
        if (!if_proxy_) {
            tmpbuf->size_of_use_ = my_svc_info_.services_id_;
            tmpbuf->size_of_buffer_ = my_svc_info_.services_type_;
        }
    }
    Comm_App_Frame *proc_frame = (Comm_App_Frame *)tmpbuf->buffer_data_;
    proc_frame->framehead_encode();
    if (snd_buffer_deque_.full()) {
        server_status_->increase_by_statid(0x2336, game_id_, 0, 1);
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] send buffer deque full, services[%u|%u] IP[%s|%u], deque size:%u.",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port(), snd_buffer_deque_.size());
        process_send_error(tmpbuf, true);
        return 0x114933ac;
    }
    snd_buffer_deque_.push_back(tmpbuf, false);
    if (send_bytes_ == 0) {
        return 0;
    }
    if (write_all_data_to_peer() != 0) {
        handle_close();
        return 0;
    }
    unite_frame_sendlist();
    return 0;
}

void TCP_Svc_Handler::unite_frame_sendlist() {
    size_t sz_deque = snd_buffer_deque_.size();
    if (sz_deque <= 1) {
        return;
    }
    ZByteBuffer *first_buf = snd_buffer_deque_[0];
    ZByteBuffer *last_buf = snd_buffer_deque_[sz_deque - 1];
    if (0x10000 - first_buf->size_of_buffer_ > last_buf->size_of_buffer_) {
        return;
    }
    memcpy(first_buf->buffer_data_ + first_buf->size_of_buffer_,
           last_buf->buffer_data_, last_buf->size_of_buffer_);
    first_buf->size_of_buffer_ += last_buf->size_of_buffer_;
    zbuffer_storage_->free_byte_buffer(last_buf);
    snd_buffer_deque_[sz_deque - 1] = 0;
    snd_buffer_deque_.pop_back();
}

int TCP_Svc_Handler::send_simple_zerg_cmd(unsigned int cmd,
                                          const SERVICES_ID &recv_services_info,
                                          unsigned int option) {
    ZByteBuffer *tmpbuf = zbuffer_storage_->allocate_buffer();
    Comm_App_Frame *proc_frame = (Comm_App_Frame *)(tmpbuf->buffer_data_);
    proc_frame->init_framehead(0x32, option, cmd);
    proc_frame->send_service_ = my_svc_info_;
    if (if_proxy_) {
        proc_frame->proxy_service_ = my_svc_info_;
    }
    proc_frame->recv_service_ = recv_services_info;
    tmpbuf->size_of_buffer_ = 0x32;
    return put_frame_to_sendlist(tmpbuf);
}

int TCP_Svc_Handler::send_zergheatbeat_reg() {
    return send_simple_zerg_cmd(0x11490c8b, my_svc_info_, 0);
}

int TCP_Svc_Handler::process_send_data(ZByteBuffer *tmpbuf) {
    server_status_->increase_by_statid(0x232e, game_id_,
                                       ((Comm_App_Frame *)(tmpbuf->buffer_data_))->frame_command_, 1);
    Comm_App_Frame *proc_frame = (Comm_App_Frame *)(tmpbuf->buffer_data_);
    SERVICES_ID *p_sendto_svrinfo = &proc_frame->recv_service_;
    if (proc_frame->recv_service_.services_type_ != 0 && if_proxy_) {
        p_sendto_svrinfo = &proc_frame->proxy_service_;
    }

    if (p_sendto_svrinfo->services_id_ == 0) {
        SERVICES_ID svrinfo(0, 0);
        if (zerg_auto_connect_.get_server(p_sendto_svrinfo->services_type_, &svrinfo) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "process_send_data: service_id==0 but cant't find auto connect had service_type=%d svrinfo",
                p_sendto_svrinfo->services_type_);
            return 0x114933ac;
        }
        p_sendto_svrinfo->services_id_ = svrinfo.services_id_;
        ZEN_Trace_LogMsg::debug_debugEx(
            "process_send_data: service_type=%d service_id=0, change service id to %d",
            p_sendto_svrinfo->services_type_, p_sendto_svrinfo->services_id_);
    }

    int ret = 0;
    TCP_Svc_Handler *svchanle = 0;
    ret = svr_peer_info_set_.find_services_peerinfo(*p_sendto_svrinfo, svchanle);
    SERVICES_ID backroute_svcinfo(0, 0);
    bool backroute_valid = false;
    if (svchanle == 0 || svchanle->peer_status_ != PEER_STATUS_ACTIVE) {
        if (zerg_auto_connect_.get_backupsvcinfo(*p_sendto_svrinfo, backroute_valid,
                                                 backroute_svcinfo) == 0) {
            if (svchanle == 0) {
                zerg_auto_connect_.reconnect_server(*p_sendto_svrinfo);
            }
            if (backroute_valid) {
                TCP_Svc_Handler *backroute_svchanle = 0;
                if (svr_peer_info_set_.find_services_peerinfo(backroute_svcinfo,
                                                              backroute_svchanle) == 0) {
                    if (backroute_svchanle->peer_status_ == PEER_STATUS_ACTIVE) {
                        svchanle = backroute_svchanle;
                        *p_sendto_svrinfo = backroute_svcinfo;
                    } else {
                        ZEN_Trace_LogMsg::debug_errorex(
                            "[zergsvr] Want to use back route to send data,but backup svc[%u|%u] not active main svc[%u|%u].",
                            backroute_svcinfo.services_type_, backroute_svcinfo.services_id_,
                            p_sendto_svrinfo->services_type_, p_sendto_svrinfo->services_id_);
                    }
                } else {
                    zerg_auto_connect_.reconnect_server(backroute_svcinfo);
                    ZEN_Trace_LogMsg::debug_errorex(
                        "[zergsvr] Want to use back route to send data,but backup svc[%u|%u] not active main svc[%u|%u].",
                        backroute_svcinfo.services_type_, backroute_svcinfo.services_id_,
                        p_sendto_svrinfo->services_type_, p_sendto_svrinfo->services_id_);
                }
            }
        }
    }

    if (svchanle == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] [SEND TO NO EXIST HANDLE] ,send to a no exist handle[%u|%u],it could have been existed. frame command[%u]. uin[%u] frame length[%u].",
            p_sendto_svrinfo->services_type_, p_sendto_svrinfo->services_id_,
            proc_frame->frame_command_, proc_frame->frame_uin_, proc_frame->frame_length_);
        return 0x114933ac;
    }
    return svchanle->put_frame_to_sendlist(tmpbuf);
}

// ---- 接收数据 ----

int TCP_Svc_Handler::check_recv_full_frame(bool &bfull, unsigned int &whole_frame_len) {
    bfull = false;
    size_t use_len = rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_;
    if (use_len <= 3) {
        if (use_len < whole_frame_len) {
            return 0;
        }
        if (whole_frame_len != 0) {
            bfull = true;
            ++recieve_counter_;
        }
        return 0;
    }

    unsigned int __x = 0;
    whole_frame_len = 0;
    for (int i = 0; i < 4; ++i) {
        unsigned char __v =
            (unsigned char)rcv_buffer_->buffer_data_[rcv_buffer_->size_of_use_ + i];
        __x = (__x << 8) | __v;
    }
    whole_frame_len = __x;
    if (whole_frame_len - 0x32 > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Recieve error frame,services[%u|%u],IP[%s|%u], famelen %u , MAX_LEN_OF_APPFRAME:%u ,recv and use len:%u|%u.",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port(), whole_frame_len, 0x10000, rcv_buffer_->size_of_use_,
            rcv_buffer_->size_of_buffer_);
        return 0x11493392;
    }
    if (whole_frame_len > max_frame_len_) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Receive error frame, services[%u|%u],IP[%s|%u],framelen :%u > max_frame_len_:%u.",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port(), whole_frame_len, max_frame_len_);
        return 0x11493392;
    }
    use_len = rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_;
    if (use_len < whole_frame_len) {
        return 0;
    }
    if (whole_frame_len != 0) {
        bfull = true;
        ++recieve_counter_;
    }
    return 0;
}

int TCP_Svc_Handler::read_data_from_peer(size_t &szrevc) {
    szrevc = 0;
    if (rcv_buffer_ == 0) {
        rcv_buffer_ = zbuffer_storage_->allocate_buffer();
    }
    ssize_t recvret = socket_peer_.recv(rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_buffer_,
                                        0x10010 - rcv_buffer_->size_of_buffer_, 0);
    if (recvret > 0) {
        szrevc = recvret;
        rcv_buffer_->size_of_buffer_ += recvret;
        recieve_bytes_ += recvret;
        return 0;
    }
    if (recvret == 0) {
        return 0x11493394;
    }
    if (errno == EAGAIN) {
        server_status_->increase_by_statid(0x2336, game_id_, 0, 1);
        return 0;
    }
    szrevc = 0;
    if (errno == EINTR) {
        return 0;
    }
    int errno_tmp = errno;
    server_status_->increase_by_statid(0x2334, game_id_, 0, 1);
    ZEN_Trace_LogMsg::debug_errorex(
        "[zergsvr] Receive data error ,services[%u|%u],IP[%s|%u] peer:%u,ZEN_OS::last_error()=%d|%s.",
        my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
        get_peer_port(), socket_peer_.get_handle(), errno_tmp, strerror(errno_tmp));
    return 0x11493391;
}

int TCP_Svc_Handler::push_frame_to_comm_mgr() {
    rcv_buffer_->size_of_use_ = 0;
    for (;;) {
        if (rcv_buffer_ == 0) {
            break;
        }
        unsigned int whole_frame_len = 0;
        bool bfull = false;
        int ret = check_recv_full_frame(bfull, whole_frame_len);
        if (ret != 0) {
            return -1;
        }
        if (!bfull) {
            break;
        }
        Comm_App_Frame *proc_frame =
            (Comm_App_Frame *)(rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_use_);
        ret = preprocess_recvframe(proc_frame);
        if (ret != 0) {
            if (ret == 0x114933a6 || ret == 0x11493396) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[zergsvr] Peer services[%u|%u] IP[%s|%u] appFrame Error,Frame Len:%u,Command:%u,Uin:%u Peer SvrType|SvrID:%u|%u,Self SvrType|SvrID:%u|%u,Send SvrType|SvrID:%u|%u,Recv SvrType|SvrID:%u|%u,Proxy SvrType|SvrID:%u|%u.",
                    my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
                    get_peer_port(), proc_frame->frame_length_, proc_frame->frame_command_,
                    proc_frame->frame_uin_, proc_frame->recv_service_.services_type_,
                    proc_frame->recv_service_.services_id_, my_svc_info_.services_type_,
                    my_svc_info_.services_id_, proc_frame->send_service_.services_type_,
                    proc_frame->send_service_.services_id_, proc_frame->proxy_service_.services_type_,
                    proc_frame->proxy_service_.services_id_, proc_frame->proxy_service_.services_type_,
                    proc_frame->proxy_service_.services_id_);
            } else {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[zergsvr] Peer services [%u|%u] IP[%s|%u] preprocess_recvframe Ret =%d.",
                    my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
                    get_peer_port(), ret);
            }
            server_status_->increase_by_statid(0x2334, game_id_, 0, 1);
            return -1;
        }

        Zerg_Comm_Manager *comm_mgr = zerg_comm_mgr_;
        if (proc_frame->frame_command_ - 0x11490c80 <= 0x270f) {
            if (proc_frame->frame_option_ & 0x1) {
                Comm_App_Frame::dumpoutput_framehead(proc_frame, "[TRACK MONITOR][RECV]", RS_ERROR);
            }
        } else {
            unsigned int i = 0;
            unsigned int monitor_num = comm_mgr->monitor_size_;
            for (; i < monitor_num; ++i) {
                if (comm_mgr->monitor_cmd_[i] == proc_frame->frame_command_) {
                    proc_frame->frame_option_ |= 0x10000;
                    ++i;
                    Comm_App_Frame::dumpoutput_framehead(proc_frame, "[TRACK MONITOR][RECV]", RS_ERROR);
                    break;
                }
            }
            if (zerg_comm_mgr_->zerg_mmap_pipe_->bus_pipe_pointer_[1]->push_end(
                    (const ZEN_LIB::dequechunk_node *)proc_frame)) {
                server_status_->increase_by_statid(0x232f, game_id_, proc_frame->frame_command_, 1);
            } else {
                ZEN_Trace_LogMsg::debug_alertex(
                    "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
                    0, proc_frame->frame_length_,
                    zerg_comm_mgr_->zerg_mmap_pipe_->bus_pipe_pointer_[1]->freesize());
                server_status_->increase_by_statid(0x233e, game_id_, proc_frame->frame_command_, 1);
            }
        }

        rcv_buffer_->size_of_use_ += whole_frame_len;
        if (rcv_buffer_->size_of_buffer_ == rcv_buffer_->size_of_use_) {
            zbuffer_storage_->free_byte_buffer(rcv_buffer_);
            rcv_buffer_ = 0;
            if (rcv_buffer_ == 0) {
                break;
            }
        }
    }
    if (rcv_buffer_ != 0 && rcv_buffer_->size_of_use_ != 0) {
        memmove(rcv_buffer_->buffer_data_,
                rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_use_,
                rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_);
        rcv_buffer_->size_of_buffer_ -= rcv_buffer_->size_of_use_;
        rcv_buffer_->size_of_use_ = 0;
    }
    return 0;
}

int TCP_Svc_Handler::process_connect_register() {
    peer_status_ = PEER_STATUS_JUST_CONNECT;
    send_simple_zerg_cmd(0x11490c81, my_svc_info_, 0);
    this->reactor()->register_handler(this, ZEN_EVENT_READ | ZEN_EVENT_WRITE);
    this->reactor()->register_handler(this, ZEN_EVENT_READ);
    ZEN_Sockaddr_In peeraddr;
    socket_peer_.getpeername(&peeraddr);
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Connect services[%u|%u] peer socket IP|Port :[%s|%u] Success.",
        my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
        get_peer_port());
    return 0;
}

int TCP_Svc_Handler::preprocess_recvframe(Comm_App_Frame *proc_frame) {
    proc_frame->framehead_decode();
    proc_frame->frame_option_ &= 0xffff0000;
    if (proc_frame->recv_service_.services_type_ != 0 && if_proxy_) {
        if (if_check_frame_) {
            if (proc_frame->proxy_service_ != my_svc_info_) {
                return 0x114933a6;
            }
            proc_frame->recv_service_.services_id_ = my_svc_info_.services_id_;
        } else {
            proc_frame->recv_service_ = my_svc_info_;
        }
    } else if (proc_frame->recv_service_.services_type_ != 0) {
        if (proc_frame->recv_service_ != my_svc_info_) {
            return 0x114933a6;
        }
    }

    if (peer_status_ == PEER_STATUS_JUST_ACCEPT) {
        if (proc_frame->recv_service_.services_type_ == 0) {
            if (proc_frame->transaction_id_ == (unsigned int)-1) {
                proc_frame->transaction_id_ = get_handle_id();
            }
            my_svc_info_ = proc_frame->send_service_;
            TCP_Svc_Handler *old_hdl = 0;
            if (svr_peer_info_set_.replace_services_peerInfo(my_svc_info_, this, old_hdl) == 0) {
                if (old_hdl != 0) {
                    old_hdl->peer_status_ = PEER_STATUS_JUST_ACCEPT;
                    old_hdl->send_simple_zerg_cmd(0x11490c88, my_svc_info_, 0x10);
                }
            }
            peer_status_ = PEER_STATUS_ACTIVE;
            ZEN_Trace_LogMsg::debug_infoex(
                "[zergsvr] Accept peer services[%u|%u],IP|Prot[%s|%u] regist success.",
                my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
                get_peer_port());
        } else {
            if (!if_proxy_) {
                my_svc_info_ = proc_frame->proxy_service_;
            }
            TCP_Svc_Handler *old_hdl = 0;
            if (svr_peer_info_set_.replace_services_peerInfo(my_svc_info_, this, old_hdl) == 0) {
                if (old_hdl != 0) {
                    old_hdl->peer_status_ = PEER_STATUS_JUST_ACCEPT;
                    old_hdl->send_simple_zerg_cmd(0x11490c88, my_svc_info_, 0x10);
                }
            }
            peer_status_ = PEER_STATUS_ACTIVE;
            ZEN_Trace_LogMsg::debug_infoex(
                "[zergsvr] Accept peer services[%u|%u],IP|Prot[%s|%u] regist success.",
                my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
                get_peer_port());
        }
    } else if (peer_status_ == PEER_STATUS_JUST_CONNECT) {
        if (proc_frame->recv_service_.services_type_ != 0 && !if_proxy_) {
            my_svc_info_ = proc_frame->proxy_service_;
        }
        peer_status_ = PEER_STATUS_ACTIVE;
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Connect peer services[%u|%u],IP|Prot[%s|%u] active success.",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port());
    }

    if (proc_frame->frame_command_ == 0x11490c81) {
        send_simple_zerg_cmd(0x11490c82, my_svc_info_, 0);
    }
    ++receive_times_;
    if (receive_times_ == 0) {
        receive_times_ = 1;
    }
    proc_frame->send_serial_number_ = htonl((unsigned int)get_peer_port());
    return 0;
}

int TCP_Svc_Handler::handle_input() {
    size_t szrecv = 0;
    int ret = read_data_from_peer(szrecv);
    if (ret != 0) {
        return -1;
    }
    ret = push_frame_to_comm_mgr();
    return (ret == -1) ? -1 : 0;
}

int TCP_Svc_Handler::handle_output() {
    int ret = 0;
    if (peer_status_ == PEER_STATUS_NOACTIVE) {
        process_connect_register();
        return 0;
    }
    ret = write_all_data_to_peer();
    return (ret == 1) ? 1 : 0;
}

// ---- 初始化 ----

void TCP_Svc_Handler::init_tcpsvr_handler(const SERVICES_ID &my_svcinfo,
                                          const ZEN_Socket_Stream &sockstream,
                                          const ZEN_Sockaddr_In &socketaddr,
                                          bool sessionkey_verify) {
    handler_mode_ = HANDLER_MODE_ACCEPTED;
    my_svc_info_ = my_svcinfo;
    peer_svr_info_.set_serviceid(0, 0);
    rcv_buffer_ = 0;
    recieve_counter_ = 0;
    send_counter_ = 0;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    socket_peer_ = sockstream;
    peer_address_ = socketaddr;
    peer_status_ = PEER_STATUS_JUST_ACCEPT;
    timeout_time_id_ = -1;
    receive_times_ = 0;
    if_force_close_ = false;
    start_live_time_ = 0;
    sessionkey_verify_ = sessionkey_verify;

    int ret = socket_peer_.sock_enable(0x800);
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Accept peer socket IP Address:[%s|%u] Success. Set ACE_NONBLOCK ret =%d.",
        get_peer_address(), get_peer_port(), ret);
    num_accept_peer_ = ++handler_id_builder_;
    if (num_accept_peer_ > max_accept_svr_) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Peer [%s|%u] great than max_accept_svr_ Reject! num_accept_peer_:%u,max_accept_svr_:%u .",
            get_peer_address(), get_peer_port(), num_accept_peer_, max_accept_svr_);
        handle_close();
        return;
    }
    if (num_accept_peer_ > threshold_warn_number_) {
        if (num_accept_peer_ % accpet_threshold_warn_ == 0) {
            ZEN_Trace_LogMsg::debug_alertex(
                "[zergsvr] Great than threshold_warn_number_ Reject! num_accept_peer_:%u,threshold_warn_number_:%u,accpet_threshold_warn_:%u,max_accept_svr_:%u .",
                num_accept_peer_, threshold_warn_number_, accpet_threshold_warn_, max_accept_svr_);
        }
        ++num_accept_peer_;
    }

    ret = this->reactor()->register_handler(this, ZEN_EVENT_READ | ZEN_EVENT_WRITE);
    if (ret != 0) {
        int errno_tmp = errno;
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Register accept [%s|%u] handler fail! ret =%u  errno=%u|%s .",
            get_peer_address(), get_peer_port(), ret, errno_tmp, strerror(errno_tmp));
        handle_close();
        return;
    }

    server_status_->set_by_statid(0x232a, 0, num_accept_peer_);
    server_status_->increase_by_statid(0x2329, game_id_, 0, 1);
    ZEN_Time_Value delay(0, 0);
    ZEN_Time_Value interval(0, 0);
    delay.sec(connect_timeout_ != 0 ? connect_timeout_ : 0x1e);
    interval.sec(receive_timeout_ != 0 ? receive_timeout_ : 0x1e);
    timeout_time_id_ = this->timer_queue()->schedule_timer(
        this, (const void *)&TCPCTRL_TIME_ID[0], delay, interval);
    int keep_alive = 1;
    socket_peer_.setsockopt(SOL_SOCKET, SO_KEEPALIVE, (const void *)&keep_alive, 4);
}

void TCP_Svc_Handler::init_tcpsvr_handler(const SERVICES_ID &my_svcinfo,
                                          const SERVICES_ID &peer_svrinfo,
                                          const ZEN_Socket_Stream &sockstream,
                                          const ZEN_Sockaddr_In &socketaddr) {
    handler_mode_ = HANDLER_MODE_CONNECT;
    my_svc_info_ = my_svcinfo;
    peer_svr_info_ = peer_svrinfo;
    rcv_buffer_ = 0;
    recieve_counter_ = 0;
    send_counter_ = 0;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    socket_peer_ = sockstream;
    peer_address_ = socketaddr;
    peer_status_ = PEER_STATUS_NOACTIVE;
    timeout_time_id_ = -1;
    receive_times_ = 0;
    sessionkey_verify_ = false;
    if_force_close_ = false;
    start_live_time_ = 0;

    int ret = socket_peer_.sock_enable(0x800);
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Connect peer socket IP Address:[%s|%u] Success. Set ACE_NONBLOCK ret =%d.",
        get_peer_address(), get_peer_port(), ret);
    snd_buffer_deque_.resize((unsigned int)snd_buf_size_);
    ret = this->reactor()->register_handler(this, ZEN_EVENT_READ);
    if (ret != 0) {
        int errno_tmp = errno;
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] Register connect [%s|%u] handler fail! ret =%u  errno=%u|%s .",
            get_peer_address(), get_peer_port(), ret, errno_tmp, strerror(errno_tmp));
        handle_close();
        return;
    }
    if (svr_peer_info_set_.add_services_peerinfo(peer_svr_info_, this) != 0) {
        handle_close();
        return;
    }
    ++num_connect_peer_;
    ZEN_Time_Value delay(0, 0);
    ZEN_Time_Value interval(0, 0);
    delay.sec(0x1e);
    interval.sec(0x1e);
    timeout_time_id_ = this->timer_queue()->schedule_timer(
        this, (const void *)&TCPCTRL_TIME_ID[0], delay, interval);
    server_status_->set_by_statid(0x232c, 0, num_connect_peer_);
    server_status_->increase_by_statid(0x232b, game_id_, 0, 1);
    int keep_alive = 1;
    socket_peer_.setsockopt(SOL_SOCKET, SO_KEEPALIVE, (const void *)&keep_alive, 4);
    int no_delay = 1;
    socket_peer_.setsockopt(IPPROTO_TCP, TCP_NODELAY, (const void *)&no_delay, 4);
}

// ---- 关闭 / 超时 ----

int TCP_Svc_Handler::handle_close() {
    ZEN_Trace_LogMsg::debug_debugEx("[zergsvr] TCP_Svc_Handler::handle_close : %u|%u.",
                                    my_svc_info_.services_type_, my_svc_info_.services_id_);
    if (timeout_time_id_ != -1) {
        this->timer_queue()->cancel_timer((const ZEN_Timer_Handler *)this);
        timeout_time_id_ = -1;
    }
    ZEN_Event_Handler::handle_close();
    socket_peer_.close();
    if (rcv_buffer_ != 0) {
        zbuffer_storage_->free_byte_buffer(rcv_buffer_);
        rcv_buffer_ = 0;
    }
    size_t sz_of_deque = snd_buffer_deque_.size();
    for (size_t i = 0; i < sz_of_deque; ++i) {
        process_send_error(snd_buffer_deque_[i], true);
        snd_buffer_deque_[i] = 0;
    }
    snd_buffer_deque_.clear();

    server_status_->increase_by_statid(0x2332, game_id_, recieve_counter_, 0);
    server_status_->increase_by_statid(0x2333, game_id_, send_counter_, 0);
    server_status_->increase_by_statid(0x2331, game_id_, recieve_bytes_, 0);
    server_status_->increase_by_statid(0x2330, game_id_, send_bytes_, 0);

    if (peer_status_ == PEER_STATUS_ACTIVE || handler_mode_ == HANDLER_MODE_ACCEPTED) {
        svr_peer_info_set_.del_services_peerInfo(my_svc_info_);
        if (!if_force_close_) {
            Comm_App_Frame appframe(0, 0x32, 0x1000000);
            appframe.init_framehead(0x32, 0, 3);
            appframe.send_service_ = my_svc_info_;
            if (zerg_comm_mgr_->zerg_mmap_pipe_->bus_pipe_pointer_[1]->push_end(
                    (const ZEN_LIB::dequechunk_node *)&appframe)) {
                server_status_->increase_by_statid(0x232f, game_id_, appframe.frame_command_, 1);
            } else {
                ZEN_Trace_LogMsg::debug_alertex(
                    "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
                    0, appframe.frame_length_,
                    zerg_comm_mgr_->zerg_mmap_pipe_->bus_pipe_pointer_[1]->freesize());
                server_status_->increase_by_statid(0x233e, game_id_, appframe.frame_command_, 1);
            }
        }
    }

    peer_status_ = PEER_STATUS_NOACTIVE;
    if (handler_mode_ == HANDLER_MODE_CONNECT) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Connect peer close, services[%u|%u] socket IP|Port :[%s|%u].",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port());
        server_status_->set_by_statid(0x232a, 0, --num_connect_peer_);
        pool_of_cnthdl_.push_back(this, false);
    } else if (handler_mode_ == HANDLER_MODE_ACCEPTED) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Accept peer close, services[%u|%u] socket IP|Port :[%s|%u].",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port());
        server_status_->set_by_statid(0x232c, 0, --num_accept_peer_);
        pool_of_acpthdl_.push_back(this, false);
    }
    return 0;
}

int TCP_Svc_Handler::handle_timeout(const ZEN_Time_Value &now_time, const void *arg) {
    const int timeid = *(const int *)arg;
    if (timeid != TCPCTRL_TIME_ID[0]) {
        return 0;
    }
    if (receive_times_ == 0) {
        if (handler_mode_ != HANDLER_MODE_ACCEPTED) {
            receive_times_ = 0;
            start_live_time_ = now_time.sec();
            ZEN_Trace_LogMsg::debug_debugEx(
                "[zergsvr] Connect or receive expire event,peer services [%u|%u] IP[%s|%u] live time %lu. recieve times=%u.",
                my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
                get_peer_port(), now_time.sec() - start_live_time_, receive_times_);
            server_status_->increase_by_statid(0x2332, game_id_, recieve_counter_, 0);
            server_status_->increase_by_statid(0x2333, game_id_, send_counter_, 0);
            server_status_->increase_by_statid(0x2331, game_id_, recieve_bytes_, 0);
            server_status_->increase_by_statid(0x2330, game_id_, send_bytes_, 0);
            recieve_counter_ = 0;
            send_counter_ = 0;
            recieve_bytes_ = 0;
            send_bytes_ = 0;
            return 0;
        }
        receive_times_ = 0;
        if (start_live_time_ == 0) {
            if (receive_timeout_ == 0) {
                start_live_time_ = now_time.sec();
                return 0;
            }
        } else if (receive_timeout_ > 0) {
            if (now_time.sec() - start_live_time_ > (time_t)receive_timeout_) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[zergsvr] Connect or receive expire event,peer services [%u|%u] IP[%s|%u] want to close handle. live time %lu. recieve times=%u.",
                    my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
                    get_peer_port(), now_time.sec() - start_live_time_, receive_times_);
                handle_close();
                return 0;
            }
        }
        start_live_time_ = now_time.sec();
        ZEN_Trace_LogMsg::debug_debugEx(
            "[zergsvr] Connect or receive expire event,peer services [%u|%u] IP[%s|%u] live time %lu. recieve times=%u.",
            my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
            get_peer_port(), now_time.sec() - start_live_time_, receive_times_);
        server_status_->increase_by_statid(0x2332, game_id_, recieve_counter_, 0);
        server_status_->increase_by_statid(0x2333, game_id_, send_counter_, 0);
        server_status_->increase_by_statid(0x2331, game_id_, recieve_bytes_, 0);
        server_status_->increase_by_statid(0x2330, game_id_, send_bytes_, 0);
        recieve_counter_ = 0;
        send_counter_ = 0;
        recieve_bytes_ = 0;
        send_bytes_ = 0;
        return 0;
    }
    receive_times_ = 0;
    start_live_time_ = now_time.sec();
    ZEN_Trace_LogMsg::debug_debugEx(
        "[zergsvr] Connect or receive expire event,peer services [%u|%u] IP[%s|%u] live time %lu. recieve times=%u.",
        my_svc_info_.services_type_, my_svc_info_.services_id_, get_peer_address(),
        get_peer_port(), now_time.sec() - start_live_time_, receive_times_);
    server_status_->increase_by_statid(0x2332, game_id_, recieve_counter_, 0);
    server_status_->increase_by_statid(0x2333, game_id_, send_counter_, 0);
    server_status_->increase_by_statid(0x2331, game_id_, recieve_bytes_, 0);
    server_status_->increase_by_statid(0x2330, game_id_, send_bytes_, 0);
    recieve_counter_ = 0;
    send_counter_ = 0;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    return 0;
}

// ---- 状态输出 ----

void TCP_Svc_Handler::dump_status_staticinfo(std::ostringstream &ostr_stream) {
    ostr_stream << "Dump TCP_Svc_Handler Static Info:" << std::endl;
    ostr_stream << "MAX ACCEPT SVR:" << max_accept_svr_ << std::endl;
    ostr_stream << "MAX_CONNECT SVR:" << max_connect_svr_ << std::endl;
    ostr_stream << "IF PROXY:" << if_proxy_ << std::endl;
    ostr_stream << "MAX FRAME LEN:" << max_frame_len_ << std::endl;
    ostr_stream << "CONNECT TIMEOUT:" << connect_timeout_ << std::endl;
    ostr_stream << "RECEIVE TIMEOUT:" << receive_timeout_ << std::endl;
    ostr_stream << "NUMBER ACCEPT PEER:" << num_accept_peer_ << std::endl;
    ostr_stream << "NUMBER CONNECT PEER:" << num_connect_peer_ << std::endl;
    ostr_stream << "NUM CONNECT PEER:" << num_connect_peer_ << std::endl;
    ostr_stream << "IF CHECK FRAME:" << if_check_frame_ << std::endl;
}

void TCP_Svc_Handler::dump_status_info(std::ostringstream &ostr_stream) {
    ostr_stream << "SELF SVC INFO:" << my_svc_info_.services_type_ << "|"
                << my_svc_info_.services_id_ << "\t";
    ostr_stream << "PEER SVC INFO:" << peer_svr_info_.services_type_ << "|"
                << peer_svr_info_.services_id_ << "\t";
    ostr_stream << "IP:" << get_peer_address() << "|" << get_peer_port() << "\t";
    ostr_stream << "STATUS:" << (int)peer_status_ << " HANDLE:" << (int)get_handle_id() << " ";
    ostr_stream << "RECV:" << recieve_bytes_ << "|" << (rcv_buffer_ != 0 ? 1 : 0) << " ";
    ostr_stream << "SEND:" << send_bytes_ << "|" << send_counter_ << std::endl;
}

void TCP_Svc_Handler::dump_svcpeer_info(std::ostringstream &ostr_stream, size_t startno,
                                        size_t numquery) {
    size_t szpeers = svr_peer_info_set_.get_services_peersize();
    ostr_stream << "SERVICES PEER NUMBER:" << (unsigned int)szpeers << std::endl;
    svr_peer_info_set_.dump_svr_peerinfo(ostr_stream, (unsigned int)startno,
                                         (unsigned int)numquery);
}
