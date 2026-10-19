// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/plugins/data_trans/datatrans_plugin.cpp
//
// Reconstructed definitions (shipped symbol addresses):
//   TssSdkDataTransPlugin::TssSdkDataTransPlugin() (C1) 0x000a18f0-0x000a1950
//   TssSdkDataTransPlugin::TssSdkDataTransPlugin() (C2) 0x000a1a10-0x000a1a70
//   get_obj_TssSdkDataTransPlugin()                    0x000a1970-0x000a1a06
//   TssSdkDataTransPlugin::fini()                      0x000a1820-0x000a1826
//   TssSdkDataTransPlugin::proc(void*)                 0x000a1830-0x000a1836
//   TssSdkDataTransPlugin::forward_data_use_ipc(...)    0x000a1840-0x000a1846
//   TssSdkDataTransPlugin::init(void*)                 0x000a20d0-0x000a216f
//   TssSdkDataTransPlugin::notify(void*)               0x000a1ad0-0x000a1cbe
//   TssSdkDataTransPlugin::~TssSdkDataTransPlugin() D0  0x000a2170-0x000a21ae
//   TssSdkDataTransPlugin::~TssSdkDataTransPlugin() D1  0x000a21b0-0x000a21dd
//
// Class layout (DWARF, sizeof == 24):
//   +0x00 vptr / +0x04 callback_proc_ / +0x08 error_info_ /
//   +0x0c sdk_plugin_id_ (DATA_TRANS == 7)
//   +0x10 TssSdkCommSvc *m_sdk_comm_svc
//   +0x14 unsigned int m_channel_id
//
// proc() and forward_data_use_ipc() are deliberate no-ops in this shipped
// build (each is a two-instruction `return 0;`).  notify() only accepts
// msg_id_ == 1 (FORWARD_DATA_USE_IPC), passes the embedded IPC package to
// forward_data_use_ipc(), and returns 0.  Any other id or a non-zero return
// sets error code 0x495d5 (300501); a NULL payload uses the other shipped
// message for the same code.
//
// The IPC/TCP package layouts below are DWARF-exact; only the IPC variant is
// dispatched by notify().

#include <string>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

// ---------------------------------------------------------------------------
// External ZEN/plugin boundary: canonical tag-394 declarations.
// ---------------------------------------------------------------------------

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
// zen_singleton.h defines instance() in-class, so this TU's dynamic
// initializer inlines the guarded sequence exactly like the ORIG
// (audit/report/init_brief.md).



// ---------------------------------------------------------------------------
// Plugin packages (DWARF anonymous structs).
// ---------------------------------------------------------------------------

enum ETssSdkPluginDataTransMsgId {
    TSS_SDK_PLUGIN_DATA_TRANS_MSG_ID_FORWARD_DATA_USE_IPC = 1,
    TSS_SDK_PLUGIN_DATA_TRANS_MSG_ID_FORWARD_DATA_USE_TCP = 2
};

// sizeof == 24.
struct TssSdkPluginDataTransForwardDataUseIPCPkg {
    unsigned int game_id_;        // +0
    unsigned int room_id_;        // +4
    unsigned int uin_;            // +8
    unsigned int dest_svr_id_;    // +12
    unsigned int data_len_;       // +16
    const unsigned char *data_;   // +20
};

// sizeof == 20.
struct TssSdkPluginDataTransForwardDataUseTCPPkg {
    unsigned int game_id_;        // +0
    unsigned int room_id_;        // +4
    unsigned int uin_;            // +8
    unsigned int data_len_;       // +12
    unsigned char *data_;         // +16
};

union TssSdkPluginDataTransBody {
    TssSdkPluginDataTransForwardDataUseIPCPkg ipc_pkg_;
    TssSdkPluginDataTransForwardDataUseTCPPkg tcp_pkg_;
};                                  // sizeof == 24

struct TssSdkPluginDataTransHead {
    ETssSdkPluginDataTransMsgId msg_id_;  // +0
};

struct TssSdkPluginDataTransPkg {
    TssSdkPluginDataTransHead head_;  // +0
    TssSdkPluginDataTransBody body_;  // +4
};                                    // sizeof == 28

namespace tss_sdk_impl {

class TssSdkDataTransPlugin : public ITssSdkPlugin {
public:
    TssSdkDataTransPlugin();
    virtual ~TssSdkDataTransPlugin() {}

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *data);
    virtual int notify(void *data);

    int forward_data_use_ipc(
        const TssSdkPluginDataTransForwardDataUseIPCPkg &pkg);

private:
    TssSdkCommSvc *m_sdk_comm_svc;   // +0x10
    unsigned int m_channel_id;       // +0x14
};

TssSdkDataTransPlugin *get_obj_TssSdkDataTransPlugin();

}  // namespace tss_sdk_impl

using tss_sdk_impl::TssSdkDataTransPlugin;
using tss_sdk_impl::TssSdkCommSvc;

// ---------------------------------------------------------------------------
// ctor / dtor / singleton accessor.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

TssSdkDataTransPlugin::TssSdkDataTransPlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_DATA_TRANS),
      m_sdk_comm_svc(0),
      m_channel_id(0) {
    ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkDataTransPlugin ctor.");
}

TssSdkDataTransPlugin *get_obj_TssSdkDataTransPlugin() {
    static TssSdkDataTransPlugin s_obj_TssSdkDataTransPlugin;
    return &s_obj_TssSdkDataTransPlugin;
}

int TssSdkDataTransPlugin::init(void *init_data) {
    (void)init_data;
    m_sdk_comm_svc = ::Singleton<TssSdkCommSvc>::instance();
    m_channel_id = m_sdk_comm_svc->channel_id_;
    return 0;
}

int TssSdkDataTransPlugin::fini() {
    return 0;
}

int TssSdkDataTransPlugin::proc(void *data) {
    (void)data;
    return 0;
}

int TssSdkDataTransPlugin::forward_data_use_ipc(
    const TssSdkPluginDataTransForwardDataUseIPCPkg &pkg) {
    (void)pkg;
    return 0;
}

int TssSdkDataTransPlugin::notify(void *data) {
    if (data == 0) {
        std::string message(
            "In CTssSdkDataTransPlugin::notify, proc_data is NULL.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x495d5;  // 300501
        return error_info_.last_error_info()->error_code;
    }

    TssSdkPluginDataTransPkg *pkg =
        static_cast<TssSdkPluginDataTransPkg *>(data);
    if (pkg->head_.msg_id_ ==
        TSS_SDK_PLUGIN_DATA_TRANS_MSG_ID_FORWARD_DATA_USE_IPC) {
        if (forward_data_use_ipc(pkg->body_.ipc_pkg_) == 0) {
            return 0;
        }
    }

    std::string message(
        "In CTssSdkDataTransPlugin::notify, data trans fail. ");
    error_info_.set_last_error_msg(message.c_str());
    error_info_.last_error_info()->error_code = 0x495d5;  // 300501
    return error_info_.last_error_info()->error_code;
}

}  // namespace tss_sdk_impl

// ORIG static-init block, part 2 (definition above the first use): the weak
// object_creator template static member (ORIG .bss 0x0029d800).
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
