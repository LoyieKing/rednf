// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/framework/tss_sdk_comm_svc.cpp
//
// Reconstructed methods (address ranges include padding up to the next
// symbol):
//   TssSdkCommSvc::TssSdkCommSvc()                        0x0008d100-0x0008d20f
//   TssSdkCommSvc::TssSdkCommSvc() (C2)                   0x0008d6c0-0x0008d7cf
//   TssSdkCommSvc::~TssSdkCommSvc() (D1)                  0x0008d030-0x0008d078
//   TssSdkCommSvc::~TssSdkCommSvc() (D2)                  0x0008d0b0-0x0008d0f8
//   TssSdkCommSvc::is_need_trans_uin(unsigned int)        0x0008cf20-0x0008cf2e
//   TssSdkCommSvc::get_last_error_code()                  0x0008cfd0-0x0008cff8
//   TssSdkCommSvc::set_errno(int)                         0x0008d000-0x0008d02b
//   TssSdkCommSvc::get_last_error()                       0x0008d7d0-0x0008d8a3
//   TssSdkCommSvc::get_user_info_by_uin(unsigned int)     0x0008d8b0-0x0008e502
//   TssSdkCommSvc::get_user_info(unsigned int&, unsigned) 0x0008e510-0x0008f142
//   TssSdkCommSvc::recv_buf_from_backend(const char**,
//                                        size_t*)         0x0008f150-0x0008fbd5
//   TssSdkCommSvc::send_buf_to_backend(const char*, size_t,
//                                      bool, int)          0x0008fbe0-0x00090885
//   TssSdkCommSvc::send_buf_to_client(int, unsigned int,
//                                     unsigned char*, short) 0x00090890-0x000914ed
//
// The class is 40 bytes:
//   +0x00 unsigned int channel_id_
//   +0x04 TssSdkPluginMgr *plugin_mgr_
//   +0x08 DPInterface *dp_if_
//   +0x0c TssSdkChannelStatus sdk_channel_status_ (24 bytes)
//          +0x0c total_recv_, +0x10 succ_recv_, +0x14 fail_recv_,
//          +0x18 total_send_, +0x1c succ_send_, +0x20 fail_send_
//   +0x24 CThreadErrorInfo *error_info_
//
// There is no user table, shared-memory segment, IPC fd or socket fd in this
// object.  User lookup is delegated to the active user-control plugin through
// ITssSdkPlugin::notify(); backend I/O is delegated to the active channel
// plugin through ITssSdkPlugin::proc()/notify().  Only the six channel
// counters are local state.

#include <cstring>
#include <sstream>
#include <string>
#include <cstddef>

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

// ORIG tss_sdk_comm_svc.cpp static-initialization block: independent
// 16-entry COMM_STAT_ITEM_WITH_NAME + __ioinit + status dtor.  It is emitted
// before the pre-create roots below so the ORIG dynamic-init order
// (status/ioinit -> CommSvc -> six plugins -> PluginMgr) is preserved.  The
// canonical logging boundary it pulls in replaces the former local
// ZEN_Trace_LogMsg stub.
#define TSS_SDK_STATIC_INIT_ID tss_sdk_comm_svc
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID


// ---------------------------------------------------------------------------
// Channel packet wire layouts (tss_sdk_plugin_channel.h, lines 19-77).
//
// GLOBAL scope, not tss_sdk_impl: the shipped tss_sdk_comm_svc.cpp CU hangs all
// of these DIEs straight off the compile unit (no DW_TAG_namespace parent), and
// tss_sdk_impl::TssSdkChannelPlugin::send_pkg_to_ipc_channel mangles with an
// unqualified parameter type:
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin23send_pkg_to_ipc_channelERK32TssSdkPluginChannelIpcChannelPkg
// ---------------------------------------------------------------------------

enum ETssSdkPluginChannelTypeId {
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_IPC_CHANNEL = 1,
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_SOCKET_CHANNEL = 2,
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_GAME_CHANNEL = 3
};

// sizeof = 4
struct TssSdkPluginChannelHead {
    ETssSdkPluginChannelTypeId type_id_;
};

// sizeof = 8
struct TssSdkPluginChannelIpcChannelPkg {
    const char *buf_;
    unsigned int buf_size_;
};

// sizeof = 10 (packed in the shipped header)
struct __attribute__((packed)) TssSdkPluginChannelSocketChannelPkg {
    int business_id_;
    const char *buf_;
    short buf_size_;
};

// sizeof = 14 (packed in the shipped header)
struct __attribute__((packed)) TssSdkPluginChannelGameChannelPkg {
    int busi_type_;
    unsigned int uin_;
    unsigned char *buf_;
    short buf_size_;
};

// sizeof = 14 (packed: the largest member is 14 bytes)
union __attribute__((packed)) TssSdkPluginChannelBody {
    TssSdkPluginChannelIpcChannelPkg ipc_channel_pkg_;
    TssSdkPluginChannelSocketChannelPkg socket_channel_pkg_;
    TssSdkPluginChannelGameChannelPkg game_channel_pkg_;
};

// sizeof = 18 (verified from DWARF byte_size, no tail padding)
struct __attribute__((packed)) TssSdkPluginChannelPkg {
    TssSdkPluginChannelHead head_;
    TssSdkPluginChannelBody body_;
};

namespace tss_sdk_impl {




// ---------------------------------------------------------------------------
// Packet wire layouts.
//
// DWARF names for the wrapper types are the anonymous "._NNN" names emitted
// by GCC 4.1 (decl_file tss_sdk_plugin_user_control.h line 83 and
// tss_sdk_plugin_channel.h line 77); the inner package/union types carry the
// source names below and the plugin entry points use them by reference.
// ---------------------------------------------------------------------------

enum ETssSdkPluginUserControlMsgId {
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER = 1,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER = 2,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER = 3,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO = 4,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN = 5
};

// sizeof = 4
struct TssSdkPluginUserControlHead {
    ETssSdkPluginUserControlMsgId msg_id_;
};

// sizeof = 28
struct TssSdkPluginUserControlAddUser {
    unsigned int idx_;
    unsigned int uin_;
    unsigned int user_id_;
    unsigned int client_ver_;
    unsigned int server_ip_;
    unsigned int client_ip_;
    const char *player_name_;
};

// sizeof = 8
struct TssSdkPluginUserControlDeleteUser {
    unsigned int idx_;
    unsigned int uin_;
};

// sizeof = 32
struct TssSdkPluginUserControlPunishUser {
    unsigned int uin_;
    unsigned short game_id_;
    unsigned short room_id_;
    int punish_mode_;
    int punish_time_;
    unsigned int punish_info_len_;
    const unsigned char *punish_info_;
    unsigned int punish_reason_len_;
    const unsigned char *punish_reason_;
};

// sizeof = 12
struct TssSdkPluginUserControlGetUserInfo {
    unsigned int uin_;
    unsigned int idx_;
    UserInfo *user_info_;
};

// sizeof = 32
union TssSdkPluginUserControlBody {
    TssSdkPluginUserControlAddUser add_user_;
    TssSdkPluginUserControlDeleteUser delete_user_;
    TssSdkPluginUserControlPunishUser punish_user_;
    TssSdkPluginUserControlGetUserInfo user_info_;
};

// sizeof = 36
struct TssSdkPluginUserControlPkg {
    TssSdkPluginUserControlHead head_;
    TssSdkPluginUserControlBody body_;
};

// ---------------------------------------------------------------------------
// TssSdkCommSvc, tss_sdk_comm_svc.h lines 33-140.
// ---------------------------------------------------------------------------


// Error codes written into CThreadErrorInfo.  They are plain integer
// constants in the shipped object; no named enum survives in DWARF.
static const int TSS_SDK_ERR_SEND_BUF_NULL          = 0x49448;  // 300104
static const int TSS_SDK_ERR_GET_ACTIVE_PLUGIN_FAIL = 0x49449;  // 300105
static const int TSS_SDK_ERR_NOTIFY_FAIL            = 0x4944a;  // 300106
static const int TSS_SDK_ERR_PROC_FAIL              = 0x4944b;  // 300107
static const int TSS_SDK_ERR_PROC_TIMEOUT           = 0x4944c;  // 300108
static const int TSS_SDK_PROC_TIMEOUT               = 0x4957a;  // 300410
static const int TSS_SDK_NOTIFY_TIMEOUT             = 0x4957b;  // 300411

// ---------------------------------------------------------------------------
// Lifecycle.  The singleton manager and DPInterface are process-wide objects;
// only error_info_ is owned by this class.
// ---------------------------------------------------------------------------

TssSdkCommSvc::TssSdkCommSvc()
    : channel_id_(0) {
    // 0x8d100 order: allocate the error block, clear its code, then bind the
    // process-wide manager/interface and clear the channel counters.
    error_info_ = new CThreadErrorInfo();
    error_info_->last_error_info()->error_code = 0;
    plugin_mgr_ = Singleton<TssSdkPluginMgr>::instance();
    dp_if_ = DPInterface::instance();
    sdk_channel_status_.total_recv_ = 0;
    sdk_channel_status_.succ_recv_ = 0;
    sdk_channel_status_.fail_recv_ = 0;
    sdk_channel_status_.total_send_ = 0;
    sdk_channel_status_.succ_send_ = 0;
    sdk_channel_status_.fail_send_ = 0;
}

TssSdkCommSvc::~TssSdkCommSvc() {
    if (error_info_ != 0) {
        delete error_info_;
    }
    error_info_ = 0;
}

bool TssSdkCommSvc::is_need_trans_uin(unsigned int game_id) {
    return game_id == 3;
}

int TssSdkCommSvc::get_last_error_code() {
    return error_info_->last_error_info()->error_code;
}

void TssSdkCommSvc::set_errno(int err_no) {
    error_info_->last_error_info()->error_code = err_no;
}

std::string TssSdkCommSvc::get_last_error() {
    std::string last_err_str;
    if (error_info_->last_error_info()->error_code == 0) {
        last_err_str.assign("", 0);
    } else {
        last_err_str.assign(error_info_->get_last_error_msg());
    }
    return last_err_str;
}

// ---------------------------------------------------------------------------
// User lookup: asks the user-control plugin.  The plugin owns the per-process
// user table; this object only forwards the request/response packet.
// ---------------------------------------------------------------------------

UserInfo *TssSdkCommSvc::get_user_info_by_uin(unsigned int uin) {
    ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL);
    if (plugin == 0) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        // Matches 0x8dd5e: the original dereferences the null plugin here
        // (get_id is called with this == 0); preserved for fidelity.
        err_str_stream << "In CTssSdkCommSvc::get_user_info, plugin mgr gets "
                          "active plugin"
                      << plugin->get_id() << "failed. " << err_str;
        error_info_->set_last_error_msg(err_str_stream.str().c_str());
        error_info_->last_error_info()->error_code =
            TSS_SDK_ERR_GET_ACTIVE_PLUGIN_FAIL;
        return 0;
    }

    TssSdkPluginUserControlPkg pkg;
    pkg.head_.msg_id_ = TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN;
    pkg.body_.user_info_.uin_ = uin;
    pkg.body_.user_info_.idx_ = 0;
    pkg.body_.user_info_.user_info_ = 0;

    int rc = plugin->notify(&pkg);
    if (rc == 0) {
        return pkg.body_.user_info_.user_info_;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream << "In CTssSdkCommSvc::get_user_info, plugin "
                  << plugin->get_id() << " notify failed. " << err_str;
    error_info_->set_last_error_msg(err_str_stream.str().c_str());
    error_info_->last_error_info()->error_code = TSS_SDK_ERR_NOTIFY_FAIL;
    return 0;
}

UserInfo *TssSdkCommSvc::get_user_info(unsigned int &uin, unsigned int idx) {
    ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL);
    if (plugin == 0) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream << "In CTssSdkCommSvc::get_user_info, plugin mgr gets "
                          "active plugin"
                      << (int)tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL
                      << "failed. " << err_str;
        error_info_->set_last_error_msg(err_str_stream.str().c_str());
        error_info_->last_error_info()->error_code =
            TSS_SDK_ERR_GET_ACTIVE_PLUGIN_FAIL;
        return 0;
    }

    TssSdkPluginUserControlPkg pkg;
    pkg.head_.msg_id_ = TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO;
    pkg.body_.user_info_.uin_ = uin;
    pkg.body_.user_info_.idx_ = idx;
    pkg.body_.user_info_.user_info_ = 0;

    int rc = plugin->notify(&pkg);
    if (rc == 0) {
        uin = pkg.body_.user_info_.uin_;
        return pkg.body_.user_info_.user_info_;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream << "In CTssSdkCommSvc::get_user_info, plugin "
                  << plugin->get_id() << " notify failed. " << err_str;
    error_info_->set_last_error_msg(err_str_stream.str().c_str());
    error_info_->last_error_info()->error_code = TSS_SDK_ERR_NOTIFY_FAIL;
    return 0;
}

// ---------------------------------------------------------------------------
// Backend receive.  rc == TSS_SDK_PROC_TIMEOUT is reported as an error code
// but does not increment fail_recv_; other non-zero values do.
// ---------------------------------------------------------------------------

int TssSdkCommSvc::recv_buf_from_backend(const char **recv_buf,
                                         std::size_t *recv_buf_size) {
    ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL);
    if (plugin == 0) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream << "In CTssSdkCommSvc::recv_buf_from_backend, plugin "
                          "mgr gets active plugin "
                      << (int)tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL
                      << " failed. " << err_str;
        error_info_->set_last_error_msg(err_str_stream.str().c_str());
        error_info_->last_error_info()->error_code =
            TSS_SDK_ERR_GET_ACTIVE_PLUGIN_FAIL;
        return error_info_->last_error_info()->error_code;
    }

    struct {
        const char *buf;
        std::size_t buf_size;
    } channel_data;

    int rc = plugin->proc(&channel_data);
    ++sdk_channel_status_.total_recv_;
    if (rc == 0) {
        ++sdk_channel_status_.succ_recv_;
        *recv_buf = channel_data.buf;
        *recv_buf_size = channel_data.buf_size;
        return 0;
    }

    if (rc == TSS_SDK_PROC_TIMEOUT) {
        error_info_->last_error_info()->error_code = TSS_SDK_ERR_PROC_TIMEOUT;
    } else {
        ++sdk_channel_status_.fail_recv_;
        error_info_->last_error_info()->error_code = TSS_SDK_ERR_PROC_FAIL;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream << "In CTssSdkCommSvc::recv_buf_from_backend, plugin "
                  << plugin->get_id() << " proc failed. " << err_str;
    error_info_->set_last_error_msg(err_str_stream.str().c_str());
    return error_info_->last_error_info()->error_code;
}

// ---------------------------------------------------------------------------
// Backend send.  use_ipc selects the IPC package (type 1); otherwise the
// socket package (type 2) is used.  A notify timeout logs through
// ZEN_Trace_LogMsg and is treated as success, but still counts as a failed
// send.
// ---------------------------------------------------------------------------

int TssSdkCommSvc::send_buf_to_backend(const char *send_buf,
                                       std::size_t send_buf_size, bool use_ipc,
                                       int business_id) {
    if (send_buf == 0) {
        std::string err_str =
            "In CTssSdkCommSvc::send_buf_to_backend, send_buf is NULL.";
        error_info_->set_last_error_msg(err_str.c_str());
        error_info_->last_error_info()->error_code =
            TSS_SDK_ERR_SEND_BUF_NULL;
        return error_info_->last_error_info()->error_code;
    }

    ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL);
    if (plugin == 0) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream << "In CTssSdkCommSvc::send_buf_to_backend, plugin "
                          "mgr gets active plugin "
                      << (int)tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL
                      << " failed. " << err_str;
        error_info_->set_last_error_msg(err_str_stream.str().c_str());
        error_info_->last_error_info()->error_code =
            TSS_SDK_ERR_GET_ACTIVE_PLUGIN_FAIL;
        return error_info_->last_error_info()->error_code;
    }

    TssSdkPluginChannelPkg pkg;
    if (use_ipc) {
        pkg.head_.type_id_ = TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_IPC_CHANNEL;
        pkg.body_.ipc_channel_pkg_.buf_ = send_buf;
        pkg.body_.ipc_channel_pkg_.buf_size_ =
            (unsigned int)send_buf_size;
    } else {
        pkg.head_.type_id_ = TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_SOCKET_CHANNEL;
        pkg.body_.socket_channel_pkg_.business_id_ = business_id;
        pkg.body_.socket_channel_pkg_.buf_ = send_buf;
        pkg.body_.socket_channel_pkg_.buf_size_ = (short)send_buf_size;
    }

    int rc = plugin->notify(&pkg);
    ++sdk_channel_status_.total_send_;
    if (rc == 0) {
        ++sdk_channel_status_.succ_send_;
        return 0;
    }

    ++sdk_channel_status_.fail_send_;
    if (rc == TSS_SDK_NOTIFY_TIMEOUT) {
        std::string err_str = plugin->get_last_error();
        ZEN_Trace_LogMsg::debug_debugEx("%s", err_str.c_str());
        return 0;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream << "In CTssSdkCommSvc::send_buf_to_backend, plugin "
                  << plugin->get_id() << " notify failed. " << err_str;
    error_info_->set_last_error_msg(err_str_stream.str().c_str());
    error_info_->last_error_info()->error_code = TSS_SDK_ERR_NOTIFY_FAIL;
    return error_info_->last_error_info()->error_code;
}

// ---------------------------------------------------------------------------
// Client send.  Always builds the game-channel package (type 3).  This path
// does not touch the channel counters.
// ---------------------------------------------------------------------------

int TssSdkCommSvc::send_buf_to_client(int busi_type, unsigned int uin,
                                      unsigned char *buf, short buf_size) {
    ITssSdkPlugin *plugin = plugin_mgr_->get_active_plugin(
        tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL);
    if (plugin == 0) {
        std::ostringstream err_str_stream;
        std::string err_str = plugin_mgr_->get_last_error();
        err_str_stream << "In CTssSdkCommSvc::send_buf_to_client, plugin mgr "
                          "gets active plugin"
                      << (int)tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL
                      << "failed. " << err_str;
        error_info_->set_last_error_msg(err_str_stream.str().c_str());
        error_info_->last_error_info()->error_code =
            TSS_SDK_ERR_GET_ACTIVE_PLUGIN_FAIL;
        return error_info_->last_error_info()->error_code;
    }

    TssSdkPluginChannelPkg pkg;
    pkg.head_.type_id_ = TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_GAME_CHANNEL;
    pkg.body_.game_channel_pkg_.busi_type_ = busi_type;
    pkg.body_.game_channel_pkg_.uin_ = uin;
    pkg.body_.game_channel_pkg_.buf_ = buf;
    pkg.body_.game_channel_pkg_.buf_size_ = buf_size;

    int rc = plugin->notify(&pkg);
    if (rc == 0) {
        return 0;
    }

    std::ostringstream err_str_stream;
    std::string err_str = plugin->get_last_error();
    err_str_stream << "In CTssSdkCommSvc::send_buf_to_client, plugin "
                  << plugin->get_id() << "notify failed. " << err_str;
    error_info_->set_last_error_msg(err_str_stream.str().c_str());
    error_info_->last_error_info()->error_code = TSS_SDK_ERR_NOTIFY_FAIL;
    return error_info_->last_error_info()->error_code;
}

}  // namespace tss_sdk_impl

// DWARF byte_size == 40.
typedef char tss_recon_comm_svc_size_must_be_40[
    sizeof(tss_sdk_impl::TssSdkCommSvc) == 40 ? 1 : -1];

// ---------------------------------------------------------------------------
// Startup pre-creation, in the ORIG absolute order.
//
// The shipped library constructs the comm-service singleton first (anti/export
// TU, 0x8ae3e..0x8ae9a; the ctor at 0x8d100 allocates the TLS error block and
// binds the manager singleton and DPInterface), then calls the six plugin
// accessors (comm_svc TU, 0x8d553..0x8d58f), then runs the object_creator
// roots again at 0x8d595..0x8d60f (guards already set).  dataTrans is
// deliberately absent: the shipped TU never calls
// get_obj_TssSdkDataTransPlugin() during static initialization.
//
// This TU is the single owner of both instance() definitions (ORIG places the
// weak TssSdkPluginMgr copy at 0x914f0 directly after
// TssSdkCommSvc::send_buf_to_client and the TssSdkCommSvc copy at 0x8af00 in
// the anti/export TU).  The roots below are emitted in ORIG source order so
// the dynamic initializers run CommSvc -> six plugins -> PluginMgr even though
// the reconstructed link list places the export TU after this one.
// ---------------------------------------------------------------------------


// zen_singleton.h instance() body.  This TU is the only definition of the two
// out-of-line instance() specializations; the framework TU additionally
// instantiates the weak object_creator roots that share these guard/storage
// symbols (ORIG framework static-init step 9).

// 0x8ae3e..0x8ae9a: pre-create TssSdkCommSvc.  Its ctor obtains the manager,
// the thread error info and DPInterface.  ObjectCreator's ctor only calls
// instance(); instance() never refers back to object_creator, so this cannot
// recurse into the guard being held here.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// 0x8d553..0x8d58f: the six plugin accessors, in shipped order.  Each
// accessor's function-local static constructs the plugin; its ITssSdkPlugin
// base constructor registers it with Singleton<TssSdkPluginMgr>.
namespace tss_sdk_impl {

class TssSdkCaptchaPlugin;
class TssSdkChannelPlugin;
class TssSdkProtoProcPlugin;
class TssSdkSafeModePlugin;
class TssSdkUicPlugin;
class TssSdkUserControlPlugin;

TssSdkCaptchaPlugin *get_obj_TssSdkCaptchaPlugin();
TssSdkChannelPlugin *get_obj_TssSdkChannelPlugin();
TssSdkProtoProcPlugin *get_obj_TssSdkProtoProcPlugin();
TssSdkSafeModePlugin *get_obj_TssSdkSafeModePlugin();
TssSdkUicPlugin *get_obj_TssSdkUicPlugin();
TssSdkUserControlPlugin *get_obj_TssSdkUserControlPlugin();

static TssSdkCaptchaPlugin *g_obj_TssSdkCaptchaPlugin
    __attribute__((used)) = get_obj_TssSdkCaptchaPlugin();
static TssSdkChannelPlugin *g_obj_TssSdkChannelPlugin
    __attribute__((used)) = get_obj_TssSdkChannelPlugin();
static TssSdkProtoProcPlugin *g_obj_TssSdkProtoProcPlugin
    __attribute__((used)) = get_obj_TssSdkProtoProcPlugin();
static TssSdkSafeModePlugin *g_obj_TssSdkSafeModePlugin
    __attribute__((used)) = get_obj_TssSdkSafeModePlugin();
static TssSdkUicPlugin *g_obj_TssSdkUicPlugin
    __attribute__((used)) = get_obj_TssSdkUicPlugin();
static TssSdkUserControlPlugin *g_obj_TssSdkUserControlPlugin
    __attribute__((used)) = get_obj_TssSdkUserControlPlugin();

}  // namespace tss_sdk_impl

// 0x8d5ae: pre-create TssSdkPluginMgr (already constructed by the first
// plugin's base ctor; the guard makes this path a no-op).
template Singleton<tss_sdk_impl::TssSdkPluginMgr>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkPluginMgr>::object_creator;

template tss_sdk_impl::TssSdkCommSvc *
Singleton<tss_sdk_impl::TssSdkCommSvc>::instance();
template tss_sdk_impl::TssSdkPluginMgr *
Singleton<tss_sdk_impl::TssSdkPluginMgr>::instance();
