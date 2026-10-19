// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel.cpp
// Address range 0x000d7340-0x000d8170.
//
// Error codes returned by the original API: 0x1430 (no business),
// 0x1431 (push failed), 0x1432 (no business info), 0x1433 (nothing queued),
// 0x1434 (invalid argument).
#include "src/commlib/framework/comm_socket_channel.h"

#include "src/commlib/zenlib/zen_recon_support.h"

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace {

const char *const SOCKET_CHANNEL_STARTED =
    "[socket channel]socket channel thread started";
const char *const SOCKET_CHANNEL_STOPED =
    "[socket channel]socket channel thread stoped";
const char *const SOCKET_CHANNEL_REACTOR_INIT =
    "[socket channel]ZEN_Reactor and ZEN_Epoll_Reactor initialized.";
const char *const SOCKET_CHANNEL_RECV_INVALID =
    "[socket channel]SocketChannel::recv_data invalid args";
const char *const SOCKET_CHANNEL_RECV_BIZ_INVALID =
    "[socket channel]SocketChannel::recv_business_data invalid args";
const char *const SOCKET_CHANNEL_SEND_NULL =
    "[socket channel]send_buf == NULL";
const char *const SOCKET_CHANNEL_NO_BUSINESS =
    "[socket channel]can not find corresponding business";
const char *const SOCKET_CHANNEL_PUSH_FAILED =
    "[socket channel]push_send_buf failed";
const char *const SOCKET_CHANNEL_INIT_INVALID =
    "[socket channel]init_connection invalid args";
const char *const SOCKET_CHANNEL_INIT_THREAD_FAILED =
    "[socket channel]init_connection start thread failed";
const char *const SOCKET_CHANNEL_INIT_CONNECT_FAILED =
    "[socket channel]addr.handler_->InitConnect failed";
const char *const SOCKET_CHANNEL_CREATE_INFO_FAILED =
    "[socket channel]create socket channel info for business %d failed, "
    "ret = %d";
const char *const SOCKET_CHANNEL_NEW_INFO_FAILED =
    "[socket channel]new SocketChannelBusinessInfo() for business %d failed";
const char *const SOCKET_CHANNEL_NOT_FOUND_INFO =
    "[socket channel]not find socket channel info for business %d, now create";
const char *const SOCKET_CHANNEL_CONNECTING =
    "[socket channel]connecting to %s|%d";

// ORIG instantiations for this CU: ZEN_Read_Guard<ZEN_Thread_RW_Mutex> for
// the business-info map lock and ZEN_Lock_Guard<ZEN_Thread_Mutex> for the
// start-thread lock (layouts in src/commlib/zenlib/zen_lock_guard.h).
typedef ZEN_Read_Guard<ZEN_Thread_RW_Mutex> SocketChannelReadGuard;
typedef ZEN_Lock_Guard<ZEN_Thread_Mutex> SocketChannelWriteGuard;

} // namespace

SocketChannel *SocketChannel::instance_ = NULL;

SocketChannel::SocketChannel()
    : is_thread_run_(false), start_thread_lock_((const char *)0) {
    ZEN_Reactor::instance(new ZEN_Epoll_Reactor(0x400, false, 0x400));
    ZEN_Trace_LogMsg::debug_infoex(SOCKET_CHANNEL_REACTOR_INIT);
}

SocketChannel::~SocketChannel() {
    is_thread_run_ = false;
}

SocketChannel *SocketChannel::instance() {
    if (instance_ == NULL) {
        instance_ = new (std::nothrow) SocketChannel();
    }
    return instance_;
}

int SocketChannel::stop_thread() {
    is_thread_run_ = false;
    return 0;
}

int SocketChannel::start_thread() {
    int result = 0;
    {
        SocketChannelWriteGuard guard(start_thread_lock_);
        if (!is_thread_run_) {
            is_thread_run_ = true;
            unsigned long thread_id;
            if (activate(0x1eb6f1, &thread_id, 0, 0x3c00000, 0) != 0) {
                is_thread_run_ = false;
                result = -1;
            }
        }
    }
    return result;
}

int SocketChannel::fini() {
    stop_thread();
    wait_join();
    ZEN_Trace_LogMsg::debug_debugEx(SOCKET_CHANNEL_STOPED);
    {
        SocketChannelReadGuard guard(business_info_map_rwlock_);
        std::map<int, SocketChannelBusinessInfo *>::iterator it =
            business_info_map_.begin();
        while (it != business_info_map_.end()) {
            SocketChannelBusinessInfo *info = it->second;
            if (info != NULL) {
                delete info;
            }
            business_info_map_.erase(it++);
        }
    }
    return 0;
}

int SocketChannel::svc() {
    ZEN_Trace_LogMsg::debug_infoex(SOCKET_CHANNEL_STARTED);
    ZEN_Reactor *reactor = ZEN_Reactor::instance();
    ZEN_Time_Value time_value(0, 0);
    time_value.usec(1000);
    while (is_thread_run_) {
        unsigned int ret = 0;
        reactor->handle_events(&time_value, &ret);
        send_to_net();
    }
    return 0;
}

int SocketChannel::send_to_net() {
    {
        SocketChannelReadGuard guard(business_info_map_rwlock_);
        std::map<int, SocketChannelBusinessInfo *>::iterator it =
            business_info_map_.begin();
        for (; it != business_info_map_.end(); ++it) {
            SocketChannelBusinessInfo *info = it->second;
            for (;;) {
                size_t node_size = 0;
                if (info->bus_->peek_send_bus(node_size) != 0) {
                    break;
                }
                SocketChannelBaseHandler *handler =
                    info->get_first_valid_handler();
                if (handler == NULL) {
                    break;
                }
                void *node = NULL;
                if (info->bus_->pop_send_bus(node) != 0) {
                    break;
                }
                handler->send((unsigned char *)node + 0x32,
                              *(unsigned int *)node - 0x32);
            }
        }
    }
    return 0;
}

SocketChannelBusinessInfo *SocketChannel::get_business_info(int business_id) {
    SocketChannelBusinessInfo *result = NULL;
    {
        SocketChannelReadGuard guard(business_info_map_rwlock_);
        std::map<int, SocketChannelBusinessInfo *>::iterator it =
            business_info_map_.find(business_id);
        if (it != business_info_map_.end()) {
            result = it->second;
        }
    }
    return result;
}

int SocketChannel::recv_data(int *business_id, void *recv_buf,
                             size_t *recv_buf_len) {
    if (business_id == NULL || recv_buf == NULL || recv_buf_len == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_RECV_INVALID);
        return 0x1434;
    }
    int result = 0x1433;
    {
        SocketChannelReadGuard guard(business_info_map_rwlock_);
        std::map<int, SocketChannelBusinessInfo *>::iterator it =
            business_info_map_.begin();
        for (; it != business_info_map_.end(); ++it) {
            SocketChannelBusinessInfo *info = it->second;
            if (info->bus_->pop_recv_bus(recv_buf) != 0) {
                continue;
            }
            *recv_buf_len = *(unsigned int *)recv_buf;
            *business_id = it->first;
            result = 0;
            break;
        }
    }
    return result;
}

int SocketChannel::recv_business_data(int business_id, void *recv_buf,
                                      size_t *recv_buf_len) {
    if (recv_buf == NULL || recv_buf_len == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_RECV_BIZ_INVALID);
        return 0x1434;
    }
    SocketChannelBusinessInfo *info = get_business_info(business_id);
    if (info == NULL) {
        return 0x1432;
    }
    if (info->bus_->pop_recv_bus(recv_buf) != 0) {
        return 0x1433;
    }
    *recv_buf_len = *(unsigned int *)recv_buf;
    return 0;
}

int SocketChannel::send_data(int business_id, const void *send_buf) {
    if (send_buf == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_SEND_NULL);
        return 0x1434;
    }
    SocketChannelBusinessInfo *info = get_business_info(business_id);
    if (info == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_NO_BUSINESS);
        return 0x1430;
    }
    if (info->bus_->push_send_bus(send_buf) != 0) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_PUSH_FAILED);
        return 0x1431;
    }
    return 0;
}

int SocketChannel::init_connection(SocketChannelInitData &addr) {
    if (addr.business_id_ < 0 || addr.handler_ == NULL || addr.ip_ == NULL
        || addr.port_ == 0) {
        if (addr.handler_ != NULL) {
            delete addr.handler_;
            addr.handler_ = NULL;
        }
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_INIT_INVALID);
        return 0x1434;
    }

    addr.handler_->reactor(ZEN_Reactor::instance());

    if (start_thread() != 0) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_INIT_THREAD_FAILED);
        if (addr.handler_ != NULL) {
            delete addr.handler_;
        }
        addr.handler_ = NULL;
        return -1;
    }

    SocketChannelBusinessInfo *info = get_business_info(addr.business_id_);
    if (info == NULL) {
        ZEN_Trace_LogMsg::debug_infoex(SOCKET_CHANNEL_NOT_FOUND_INFO,
                                       addr.business_id_);
        info = new (std::nothrow) SocketChannelBusinessInfo();
        if (info == NULL) {
            ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_NEW_INFO_FAILED,
                                            addr.business_id_);
            if (addr.handler_ != NULL) {
                delete addr.handler_;
            }
            addr.handler_ = NULL;
            return -1;
        }
        int ret = info->init();
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_CREATE_INFO_FAILED,
                                            addr.business_id_, ret);
            delete info;
            if (addr.handler_ != NULL) {
                delete addr.handler_;
            }
            addr.handler_ = NULL;
            return -1;
        }
        {
            SocketChannelReadGuard guard(business_info_map_rwlock_);
            business_info_map_.insert(
                std::make_pair(addr.business_id_, info));
        }
    }

    SocketChannelBaseHandler *handler = addr.handler_;
    if (handler->init_connect(addr, info->bus_) != 0) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_CHANNEL_INIT_CONNECT_FAILED);
        delete addr.handler_;
        addr.handler_ = NULL;
        return -1;
    }

    info->add_handler(handler);
    ZEN_Trace_LogMsg::debug_infoex(SOCKET_CHANNEL_CONNECTING, addr.ip_,
                                   (int)addr.port_);
    return 0;
}
