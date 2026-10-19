// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/plugins/channel/channel_plugin.cpp
//
// Reconstructed plugin methods (ranges are symbol ranges, padding included):
//   TssSdkChannelPlugin::TssSdkChannelPlugin() (C2)     0x0009cc90-0x0009cd6e
//   TssSdkChannelPlugin::TssSdkChannelPlugin() (C1)     0x0009cd70-0x0009ce4e
//   tss_sdk_impl::get_obj_TssSdkChannelPlugin()         0x0009ce50-0x0009cee5
//   TssSdkChannelPlugin::fini()                        0x0009d470-0x0009d7b0
//   TssSdkChannelPlugin::ipc_channel_recv(void*)        0x0009d7c0-0x0009d8d8
//   TssSdkChannelPlugin::socket_channel_recv(void*)     0x0009d8e0-0x0009da30
//   TssSdkChannelPlugin::notify(void*)                  0x0009da40-0x0009e804
//   TssSdkChannelPlugin::init(void*)                    0x0009e810-0x0009fbc5
//   TssSdkChannelPlugin::proc(void*)                    0x0009fbd0-0xa0095d
//   TssSdkChannelPlugin::~TssSdkChannelPlugin() (D1)    0x000a0960-0x000a09ff
//   TssSdkChannelPlugin::~TssSdkChannelPlugin() (D0)    0x000a0a00-0x000a0aa7
//   send_pkg_to_game_channel(...)                       0x0009ca50-0x0009ca63
//   send_pkg_to_socket_channel(...)                     0x0009cb50-0x0009cb93
//   send_pkg_to_ipc_channel(...)                        0x0009cba0-0x0009cbd5
//
// The class is 75908 bytes:
//   +0x00000 ITssSdkPlugin base (16 bytes)
//   +0x00010 BusChannelIdMgr svr_id_pool_mgr_                  (24)
//   +0x00028 unsigned int channel_id_
//   +0x0002c bool m_is_use_socket_channel_
//   +0x0002d bool m_is_use_ipc_channel_
//   +0x00030 CIpcChannel m_ipc_channel_                       (75856)
//   +0x12880 unsigned char *recv_buf_
//
// CIpcChannel is itself 75856 bytes:
//   +0x0000 char recv_buf_[65535]
//   +0x10000 ZEN_Thread_Mutex thread_mutex_                   (28)
//   +0x1001c ZEN_BusPipe_TwoWay bus_channel_                  (10292)
//
// Channel plugins never carry business data locally.  IPC traffic is a
// length-prefixed block in the zenlib shared-memory deque; socket traffic is
// posted to the zenlib SocketChannel thread with the business id from the
// configuration table; game traffic is handed to the callback registered on
// the ITssSdkPlugin base.

#include <cstddef>
#include <new>
#include <sstream>
#include <string>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"
#include "src/commlib/zenlib/zen_singleton.h"

// Canonical tag-394 declarations.  Every zenlib/framework type this plugin
// touches is declared exactly once by these headers; the former local models
// (ZEN_Thread_Mutex, ZEN_BusPipe_TwoWay, CIpcChannel, BusChannelIdMgr,
// SocketChannel, ZEN_Server_Status, Comm_Stat_Monitor, ZEN_Timer_Queue/Wheel,
// SERVICES_ID, DP_CONFIG, the plugin/packet wrappers) are gone.
#include "src/commlib/framework/comm_cfgsvr_sdk.h"  // SERVICES_ID, Comm_App_Frame
#include "src/commlib/framework/comm_socket_channel.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
// ZEN_File_Lock (8) and ZEN_ShareMem_Mmap (16) were defined by the ORIG TU
// through the zen predefine chain; in the reconstruction the canonical
// declarations are zen_lock_file_lock.h and closing/dp_config.h (there is no
// separate recon zen_share_mem_mmap.h - dp_config.h carries that class).
#include "src/commlib/zenlib/zen_lock_file_lock.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/comm_stat_monitor.h"
#include "src/formmog/tss_sdk/layout/tss_sdk_channel.h"
#include "src/formmog/tss_sdk/layout/zen_bus_pipe.h"
#include "src/formmog/tss_sdk/layout/zen_timer_queue.h"
#include "src/formmog/tss_sdk/plugins/tss_sdk_config_shared.h"

// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// tss_sdk_plugin_channel.h: the 12-byte channel init block passed by the
// SDK export layer (same declaration used by tss_sdk_exports_timer_agent.cpp).
struct TssSdkPluginChannelInitData {
    unsigned int unique_instance_id_;   // +0x00
    unsigned int ipc_bus_size_;         // +0x04
    int pid_channel_mode_;              // +0x08
};


namespace tss_sdk_impl {

// uic_socket_tcp_handler.h.  The concrete handler is defined by
// channel_misc_agent.cpp (uic_socket_tcp_handler.cpp); this declaration is
// identical to that TU's and only fixes the allocation size (88) and the
// constructor symbol called by init().
class UICSocketChannelTCPHandler : public SocketChannelTCPHandler {
public:
    UICSocketChannelTCPHandler();
    virtual ~UICSocketChannelTCPHandler();

    virtual int check_if_full_frame(unsigned char *buf, size_t buf_size,
                                    bool *full, int *frame_size);
    virtual void define_comm_head_except_frame_length(Comm_App_Frame *frame);
    static const int FRAME_LEN = 26;
};

// Received channel block used by proc()/socket_channel_recv()/ipc_channel_recv.
// The first four bytes of the pointed-to data are the length placed in the
// zenlib deque node; buf_size is that length.
struct STssChannelData {
    const char *buf;
    size_t buf_size;
};

// channel_stat_define.h ids.  The same block 0x68fb1..0x68fb9 is registered
// with Comm_Stat_Monitor::initialize() from init().
enum {
    STAT_TSSSDK_IPC_RECV_SUCC = 0x68fb1,
    STAT_TSSSDK_IPC_RECV_FAIL = 0x68fb2,
    STAT_TSSSDK_IPC_SEND_SUCC = 0x68fb3,
    STAT_TSSSDK_IPC_SEND_FAIL = 0x68fb4,
    STAT_TSSSDK_SOCKET_RECV_SUCC = 0x68fb5,
    STAT_TSSSDK_SOCKET_RECV_FAIL = 0x68fb6,
    STAT_TSSSDK_SOCKET_SEND_SUCC = 0x68fb7,
    STAT_TSSSDK_SOCKET_SEND_FAIL = 0x68fb8,
    STAT_TSSSDK_ANTIBOT_LOST_PKG = 0x68fb9,
    STAT_TSSSDK_BLOCK_END = 0x68fba
};

// Exact numeric error codes written by this TU.  There are no symbolic names
// for them in the shipped DWARF.
enum {
    ERR_TSSSDK_NULL_ARG = 0x49571,
    ERR_TSSSDK_SVR_ID_INIT = 0x49572,
    ERR_TSSSDK_SOCKET_SETUP = 0x49573,
    ERR_TSSSDK_IPC_OPEN = 0x49574,
    ERR_TSSSDK_SOCKET_NOT_USED = 0x49576,
    ERR_TSSSDK_IPC_NOT_USED = 0x49577,
    ERR_TSSSDK_CLOSE_SOCKET = 0x49578,
    ERR_TSSSDK_CLOSE_IPC = 0x49579,
    ERR_TSSSDK_CHANNEL_EMPTY = 0x4957a,
    ERR_TSSSDK_IPC_RECV = 0x4957c,
    ERR_TSSSDK_SOCKET_RECV = 0x4957d,
    ERR_TSSSDK_IPC_SEND = 0x4957e,
    ERR_TSSSDK_SOCKET_SEND = 0x4957f,
    ERR_TSSSDK_GAME_SEND = 0x49582,
    ERR_TSSSDK_NOTIFY_PKG = 0x49583,
    ERR_TSSSDK_STAT_INIT = 0x497c9
};

}  // namespace tss_sdk_impl

// The error code lives in the base's per-thread CThreadErrorInfo at this+0x08.
// The shipped methods read/write it directly, not through a virtual.
#define m_plugin_last_error_code \
    (error_info_.last_error_info()->error_code)

// ---------------------------------------------------------------------------
// Channel stat table constructed by this TU's static initialization, at
// .bss 0x002a3020 (local symbol STR_TSSSDK_STAT, 0x33c bytes).  Only the five
// named counters are explicitly initialized; the last four entries are
// default-constructed ZEN_STATUS_ITEM_WITHNAME objects.
// ---------------------------------------------------------------------------
static ZEN_STATUS_ITEM_WITHNAME STR_TSSSDK_STAT[9] = {
    ZEN_STATUS_ITEM_WITHNAME(tss_sdk_impl::STAT_TSSSDK_IPC_RECV_SUCC,
                             STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_RECV_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(tss_sdk_impl::STAT_TSSSDK_IPC_RECV_FAIL,
                             STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_RECV_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(tss_sdk_impl::STAT_TSSSDK_IPC_SEND_SUCC,
                             STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_SEND_SUCC_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(tss_sdk_impl::STAT_TSSSDK_IPC_SEND_FAIL,
                             STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_IPC_CHANNLE_SEND_FAIL_NUM"),
    ZEN_STATUS_ITEM_WITHNAME(tss_sdk_impl::STAT_TSSSDK_ANTIBOT_LOST_PKG,
                             STATICS_PER_FIVE_MINTUES,
                             "TSSSDK_ANTIBOT_LOST_PKG_NUM")
};

// Compile-time layout checks against the canonical declarations.
typedef char channel_plugin_layout_check
    [(sizeof(tss_sdk_impl::ITssSdkPlugin) == 16 &&
      sizeof(BusChannelIdMgr) == 24 &&
      sizeof(ZEN_Thread_Mutex) == 28 &&
      sizeof(ZEN_BusPipe_TwoWay) == 10292 &&
      sizeof(tss_sdk_impl::CIpcChannel) == 75856 &&
      sizeof(SocketChannelInitData) == 16 &&
      sizeof(TssSdkServerAddrInfo) == 38 &&
      sizeof(TssSdkCommConfig) == 1226 &&
      sizeof(TssSdkConfig) == 1515 &&
      sizeof(TssSdkPluginChannelInitData) == 12 &&
      sizeof(TssSdkPluginChannelIpcChannelPkg) == 8 &&
      sizeof(TssSdkPluginChannelSocketChannelPkg) == 10 &&
      sizeof(TssSdkPluginChannelGameChannelPkg) == 14 &&
      sizeof(TssSdkPluginChannelBody) == 14 &&
      sizeof(TssSdkPluginChannelPkg) == 18 &&
      sizeof(ZEN_STATUS_ITEM_ID) == 12 &&
      sizeof(ZEN_STATUS_ITEM) == 24 &&
      sizeof(ZEN_STATUS_ITEM_WITHNAME) == 92 &&
      sizeof(Comm_Stat_Monitor) == 140 &&
      sizeof(tss_sdk_impl::UICSocketChannelTCPHandler) == 88 &&
      sizeof(SERVICES_ID) == 6 &&
      sizeof(ZEN_Timer_Wheel) == 0x60 &&
      sizeof(tss_sdk_impl::TssSdkChannelPlugin) == 75908)
         ? 1
         : -1];

namespace tss_sdk_impl {

// The repository's canonical CIpcChannel declaration leaves the destructor
// out of line; this TU owns the definition because its plugin member is
// destroyed here.
CIpcChannel::~CIpcChannel() {}



// 0x0009cc90 / 0x0009cd70.  Both shipped constructors are identical (C1/C2).
TssSdkChannelPlugin::TssSdkChannelPlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL),
      svr_id_pool_mgr_(),
      channel_id_(0),
      m_is_use_socket_channel_(false),
      m_is_use_ipc_channel_(false),
      m_ipc_channel_(),
      recv_buf_(NULL) {
    ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkChannelPlugin ctor.");
}

// 0x000a0960 / 0x000a0a00.  The body is empty: member destruction is
// CIpcChannel's inline destructor (ZEN_BusPipe_TwoWay then ZEN_Thread_Mutex),
// BusChannelIdMgr's destructor, and finally ITssSdkPlugin.  The shipped
// destructor does not delete recv_buf_ and does not call fini().
TssSdkChannelPlugin::~TssSdkChannelPlugin() {}

// 0x0009ce50.  Function-local static; the shipped build uses the usual
// __cxa_guard_acquire / __cxa_atexit sequence and __tcf_0 for destruction.
TssSdkChannelPlugin *get_obj_TssSdkChannelPlugin() {
    static TssSdkChannelPlugin s_obj_TssSdkChannelPlugin;
    return &s_obj_TssSdkChannelPlugin;
}

// 0x0009e810.
int TssSdkChannelPlugin::init(void *init_data) {
    if (NULL == init_data) {
        std::string err_str(
            "In CTssSdkChannelPlugin::init, init_data is NULL.");
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_NULL_ARG;
        return m_plugin_last_error_code;
    }

    TssSdkPluginChannelInitData *init_channel_data =
        (TssSdkPluginChannelInitData *)init_data;

    int rc;
    unsigned int channel_id;

    rc = svr_id_pool_mgr_.init(200);
    if (rc != 0) {
        std::ostringstream err_str_stream;
        err_str_stream
            << "In TssSdkChannelPlugin::init, get_svr_id failed. rc is "
            << rc << ".";
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_SVR_ID_INIT;
        return m_plugin_last_error_code;
    }

    if (init_channel_data->pid_channel_mode_ != 0) {
        channel_id = svr_id_pool_mgr_.get_bus_channel_id();
    } else {
        channel_id = svr_id_pool_mgr_.get_bus_channel_id(
            init_channel_data->unique_instance_id_);
    }

    channel_id_ = channel_id;

    // The shipped code writes the same id into the comm-service singleton and
    // does not validate a negative get_bus_channel_id() result.
    Singleton<TssSdkCommSvc>::instance()->channel_id_ = channel_id;

    if (g_config.comm_config_.use_ipc_ != 0) {
        m_is_use_ipc_channel_ = true;
        rc = m_ipc_channel_.open(channel_id_,
                                 init_channel_data->ipc_bus_size_);
        if (rc != 0) {
            std::ostringstream err_str_stream;
            err_str_stream << "Open IPC channel failed. rc is " << rc << ".";
            std::string err_str = err_str_stream.str();
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_IPC_OPEN;
            return m_plugin_last_error_code;
        }
    }

    if (g_config.comm_config_.use_socket_ != 0) {
        m_is_use_socket_channel_ = true;
        recv_buf_ = new (std::nothrow) unsigned char[0x10000];
        if (recv_buf_ == NULL) {
            std::ostringstream err_str_stream;
            err_str_stream << "new unsigned char failed";
            std::string err_str = err_str_stream.str();
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_SOCKET_SETUP;
            return m_plugin_last_error_code;
        }

        for (int i = 0; i < (int)g_config.comm_config_.server_num_; ++i) {
            SocketChannelInitData init_data;
            init_data.business_id_ =
                g_config.comm_config_.server_addrs_info_[i].plugin_id_;
            init_data.ip_ = g_config.comm_config_.server_addrs_info_[i].ip_;
            init_data.port_ = g_config.comm_config_.server_addrs_info_[i].port_;
            init_data.handler_ =
                new (std::nothrow) tss_sdk_impl::UICSocketChannelTCPHandler();

            rc = SocketChannel::instance()->init_connection(init_data);
            if (rc != 0) {
                std::ostringstream err_str_stream;
                err_str_stream << "Open socket channel failed. rc is "
                               << rc << ".";
                std::string err_str = err_str_stream.str();
                error_info_.set_last_error_msg(err_str.c_str());
                m_plugin_last_error_code = ERR_TSSSDK_SOCKET_SETUP;
                return m_plugin_last_error_code;
            }
        }
    }

    unsigned int svr_id = channel_id_;
    SERVICES_ID self_info(0x2b, svr_id);

    ZEN_Timer_Queue::instance(new ZEN_Timer_Wheel(
        0x400, 0xf731400, 0x64,
        ZEN_Timer_Queue::TRIGGER_MODE_SYSTEM_CLOCK, true));

    rc = Comm_Stat_Monitor::instance()->initialize(
        false, self_info, 9, STR_TSSSDK_STAT, true);
    if (rc != 0) {
        std::ostringstream err_str_stream;
        err_str_stream << "Init stat monitor failed. rc is " << rc << ".";
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_STAT_INIT;
        return m_plugin_last_error_code;
    }

    for (unsigned int statistic_id = STAT_TSSSDK_IPC_RECV_SUCC;
         statistic_id < STAT_TSSSDK_BLOCK_END; ++statistic_id) {
        Comm_Stat_Monitor::instance()->set_by_statid(statistic_id, 0, 0, 0);
    }

    return 0;
}

// 0x0009d470.
int TssSdkChannelPlugin::fini() {
    m_plugin_last_error_code = 0;

    if (m_is_use_socket_channel_) {
        int rc = SocketChannel::instance()->fini();
        if (rc != 0) {
            std::ostringstream err_str_stream;
            err_str_stream << "Close socket channel failed. rc is "
                           << rc << ".";
            std::string err_str = err_str_stream.str();
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_CLOSE_SOCKET;
        }

        if (recv_buf_ != NULL) {
            delete[] recv_buf_;
            recv_buf_ = NULL;
        }
    }

    if (m_is_use_ipc_channel_) {
        int rc = m_ipc_channel_.close();
        if (rc != 0) {
            std::ostringstream err_str_stream;
            err_str_stream << "Close IPC channel failed. rc is " << rc << ".";
            std::string err_str = err_str_stream.str();
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_CLOSE_IPC;
        }
    }

    channel_id_ = 0;
    m_is_use_socket_channel_ = false;
    m_is_use_ipc_channel_ = false;

    return m_plugin_last_error_code;
}

// 0x0009d7c0.  proc_data is STssChannelData.
int TssSdkChannelPlugin::ipc_channel_recv(void *proc_data) {
    if (proc_data == NULL) {
        std::string err_str(
            "In CTssSdkChannelPlugin::ipc_channel_recv, proc_data is NULL.");
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_NULL_ARG;
        return m_plugin_last_error_code;
    }

    STssChannelData *channel_data = (STssChannelData *)proc_data;
    return m_ipc_channel_.recv((void **)&channel_data->buf,
                               (unsigned int *)&channel_data->buf_size);
}

// 0x0009d8e0.  SocketChannel::recv_data returns 0x1433 when no complete
// block is queued; every other return is treated as success here and proc()
// retries on the next poll.  The value 0x1433 is mapped to -1.
int TssSdkChannelPlugin::socket_channel_recv(void *proc_data) {
    if (proc_data == NULL) {
        std::string err_str(
            "In CTssSdkChannelPlugin::socket_channel_recv, proc_data is NULL.");
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_NULL_ARG;
        return m_plugin_last_error_code;
    }

    int rc;
    int business_id = 0;
    size_t recv_buf_len = 0;
    STssChannelData *channel_data = (STssChannelData *)proc_data;

    rc = SocketChannel::instance()->recv_data(
        &business_id, recv_buf_, (unsigned int *)&recv_buf_len);

    channel_data->buf = (const char *)recv_buf_;
    channel_data->buf_size = recv_buf_len;

    if (rc == 0x1433) {
        return -1;
    }
    return 0;
}

// 0x0009da40.
int TssSdkChannelPlugin::notify(void *notify_data) {
    if (notify_data == NULL) {
        std::string err_str(
            "In CTssSdkChannelPlugin::notify, notify_data is NULL.");
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_NULL_ARG;
        return m_plugin_last_error_code;
    }

    TssSdkPluginChannelPkg *pkg = (TssSdkPluginChannelPkg *)notify_data;
    int rc;

    switch (pkg->head_.type_id_) {
    case TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_IPC_CHANNEL: {
        if (!m_is_use_ipc_channel_) {
            std::string err_str("IPC channel is not used.");
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_IPC_NOT_USED;
            return m_plugin_last_error_code;
        }

        rc = send_pkg_to_ipc_channel(pkg->body_.ipc_channel_pkg_);
        if (rc == 0) {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_IPC_SEND_SUCC, game_id, 0, 1);
            return 0;
        }

        {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_IPC_SEND_FAIL, game_id, 0, 1);
        }

        std::ostringstream err_str_stream;
        err_str_stream << "Send to IPC channel failed."
                       << "rc is " << rc << ".";
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_IPC_SEND;
        return m_plugin_last_error_code;
    }

    case TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_SOCKET_CHANNEL: {
        if (!m_is_use_socket_channel_) {
            std::string err_str("Socket channel is not used.");
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_SOCKET_NOT_USED;
            return m_plugin_last_error_code;
        }

        rc = send_pkg_to_socket_channel(pkg->body_.socket_channel_pkg_);
        if (rc == 0) {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_SOCKET_SEND_SUCC, game_id, 0, 1);
            return 0;
        }

        {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_SOCKET_SEND_FAIL, game_id, 0, 1);
        }

        std::ostringstream err_str_stream;
        err_str_stream << "Send to socket channel failed."
                       << "rc is " << rc << ".";
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_SOCKET_SEND;
        return m_plugin_last_error_code;
    }

    case TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_GAME_CHANNEL: {
        rc = send_pkg_to_game_channel(pkg->body_.game_channel_pkg_);
        if (rc == 0) {
            return 0;
        }

        std::ostringstream err_str_stream;
        err_str_stream << "Send to game channel failed."
                       << "rc is " << rc << ".";
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_GAME_SEND;
        return m_plugin_last_error_code;
    }

    default: {
        std::string err_str("Error notify pkg.");
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_NOTIFY_PKG;
        return m_plugin_last_error_code;
    }
    }
}

// 0x0009fbd0.  Socket channel takes precedence when both flags are set.
int TssSdkChannelPlugin::proc(void *proc_data) {
    if (m_is_use_socket_channel_) {
        if (proc_data == NULL) {
            std::string err_str(
                "In CTssSdkChannelPlugin::proc, proc_data is NULL.");
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_NULL_ARG;
            return m_plugin_last_error_code;
        }

        int rc = socket_channel_recv(proc_data);
        if (rc == 0) {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_SOCKET_RECV_SUCC, game_id, 0, 1);
            return 0;
        }

        if (rc == -1) {
            std::string err_str("socket channel is empty.");
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_CHANNEL_EMPTY;
            return m_plugin_last_error_code;
        }

        {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_SOCKET_RECV_FAIL, game_id, 0, 1);
        }

        std::ostringstream err_str_stream;
        err_str_stream << "Recv from IPC channel failed. "
                       << "rc is " << rc;
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_SOCKET_RECV;
        return m_plugin_last_error_code;
    }

    if (m_is_use_ipc_channel_) {
        if (proc_data == NULL) {
            std::string err_str(
                "In CTssSdkChannelPlugin::proc, proc_data is NULL.");
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_NULL_ARG;
            return m_plugin_last_error_code;
        }

        int rc = ipc_channel_recv(proc_data);
        if (rc == 0) {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_IPC_RECV_SUCC, game_id, 0, 1);
            return 0;
        }

        if (rc == -1) {
            std::string err_str("IPC channel is empty.");
            error_info_.set_last_error_msg(err_str.c_str());
            m_plugin_last_error_code = ERR_TSSSDK_CHANNEL_EMPTY;
            return m_plugin_last_error_code;
        }

        {
            unsigned int game_id = 0;
            if (tss_dp_comm::DP_CONFIG::glb_dp_config_head_ != NULL) {
                game_id =
                    tss_dp_comm::DP_CONFIG::glb_dp_config_head_->game_id;
            }
            Comm_Stat_Monitor::instance()->increase_by_statid(
                STAT_TSSSDK_IPC_RECV_FAIL, game_id, 0, 1);
        }

        std::ostringstream err_str_stream;
        err_str_stream << "Recv from IPC channel failed. "
                       << "rc is " << rc;
        std::string err_str = err_str_stream.str();
        error_info_.set_last_error_msg(err_str.c_str());
        m_plugin_last_error_code = ERR_TSSSDK_IPC_RECV;
        return m_plugin_last_error_code;
    }

    return 0;
}

// 0x0009cba0.  CIpcChannel::send() checks the 0xffff length limit and pushes
// the node unchanged into the shared-memory send deque.
int TssSdkChannelPlugin::send_pkg_to_ipc_channel(
    const TssSdkPluginChannelIpcChannelPkg &ipc_channel_pkg) {
    return m_ipc_channel_.send(ipc_channel_pkg.buf_,
                               ipc_channel_pkg.buf_size_);
}

// 0x0009cb50.  The socket path ignores buf_size_; the frame length is the
// first word of the buffer handed to SocketChannel::send_data().
int TssSdkChannelPlugin::send_pkg_to_socket_channel(
    const TssSdkPluginChannelSocketChannelPkg &socket_channel_pkg) {
    return SocketChannel::instance()->send_data(
        socket_channel_pkg.business_id_, socket_channel_pkg.buf_);
}

// 0x0009ca50.  Game traffic is delivered to the callback installed on the
// ITssSdkPlugin base (+0x04); no channel or stat state is touched.
int TssSdkChannelPlugin::send_pkg_to_game_channel(
    const TssSdkPluginChannelGameChannelPkg &game_channel_pkg) {
    return callback_proc_((void *)&game_channel_pkg);
}

}  // namespace tss_sdk_impl
