// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_info.cpp
// Address range 0x000d8b00-0x000d9012 (business-info part 0x000d8ba0-0x000d8fd8).
//
// Both mutating and reading paths take the *read* lock (vtable slot 0x1c) and
// release it through slot 0x10, exactly like the ORIG.
#include "src/commlib/framework/comm_socket_channel_info.h"

#include <new>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace {

const char *const SOCKET_INFO_BUS_INIT_FAIL =
    "[socket channel info]sndrcv_bus_->init() failed, ret = %d";
const char *const SOCKET_INFO_NEW_VECTOR_FAIL =
    "[socket channel info]new SOCKET_HANDLERS_VECTOR() failed";
const char *const SOCKET_INFO_NEW_BUS_FAIL =
    "[socket channel info]new SocketChannelBus() failed";
const char *const SOCKET_INFO_NEED_RECONNECT =
    "[socket channel]need reconnect";
const char *const SOCKET_INFO_NO_IDLE =
    "[socket channel]have not idle connection";

// ORIG guard for this CU: ZEN_Read_Guard<ZEN_Thread_RW_Mutex> on
// handlers_vector_rwlock_ (lock_read via vtable slot 0x1c, unlock via 0x10).
typedef ZEN_Read_Guard<ZEN_Thread_RW_Mutex> SocketChannelReadGuard;

} // namespace

SocketChannelBusinessInfo::SocketChannelBusinessInfo()
    : business_id_(-1), bus_(NULL), handler_ary_(NULL), lock_() {
}

SocketChannelBusinessInfo::~SocketChannelBusinessInfo() {
    fini();
}

int SocketChannelBusinessInfo::init() {
    handler_ary_ = new (std::nothrow)
        std::vector<SocketChannelBaseHandler *>();
    if (handler_ary_ == NULL) {
        handler_ary_ = NULL;
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_INFO_NEW_VECTOR_FAIL);
        return -1;
    }

    bus_ = new (std::nothrow) SocketChannelBus();
    if (bus_ == NULL) {
        bus_ = NULL;
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_INFO_NEW_BUS_FAIL);
        return -1;
    }

    int ret = bus_->init();
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_INFO_BUS_INIT_FAIL, ret);
        return -1;
    }
    return 0;
}

int SocketChannelBusinessInfo::fini() {
    if (handler_ary_ != NULL) {
        while (!handler_ary_->empty()) {
            SocketChannelBaseHandler *handler = handler_ary_->front();
            if (handler != NULL) {
                delete handler;
            }
            handler_ary_->erase(handler_ary_->begin());
        }
        delete handler_ary_;
        handler_ary_ = NULL;
    }

    if (bus_ != NULL) {
        delete bus_;
        bus_ = NULL;
    }
    return 0;
}

int SocketChannelBusinessInfo::add_handler(
    SocketChannelBaseHandler *handler) {
    {
        SocketChannelReadGuard guard(lock_);
        handler_ary_->push_back(handler);
    }
    return 0;
}

SocketChannelBaseHandler *SocketChannelBusinessInfo::get_first_valid_handler(
    ) {
    SocketChannelBaseHandler *result = NULL;
    {
        SocketChannelReadGuard guard(lock_);
        std::vector<SocketChannelBaseHandler *>::iterator it =
            handler_ary_->begin();
        for (; it != handler_ary_->end(); ++it) {
            SocketChannelBaseHandler *handler = *it;
            if (!handler->is_valid()) {
                ZEN_Trace_LogMsg::debug_errorex(SOCKET_INFO_NEED_RECONNECT);
                handler->connect_server();
                continue;
            }
            if (handler->is_idle()) {
                result = handler;
                break;
            }
        }
        if (result == NULL) {
            ZEN_Trace_LogMsg::debug_debugEx(SOCKET_INFO_NO_IDLE);
        }
    }
    return result;
}
