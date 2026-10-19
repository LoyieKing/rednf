// Semantic reconstruction from libtss_sdk.so.1.0.394
// (ELF32 i386, GCC 4.1.x, -O2 -fPIC, SHA-256
//  8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7).
//
// This file covers the timer handler, the ITssSdkPlugin base, and the public
// interface factory exports.  The shipped code comes from several original
// translation units; each block below names its source file and address range.
//
//   src/formmog/tss_sdk/framework/tss_sdk_time_handler.cpp
//     TssSdkTimerHandler::TssSdkTimerHandler() C1  0x0009b450-0x0009b492
//     TssSdkTimerHandler::TssSdkTimerHandler() C2  0x0009b4a0-0x0009b4e2
//     TssSdkTimerHandler::~TssSdkTimerHandler() D2 0x0009b000-0x0009b015
//     TssSdkTimerHandler::~TssSdkTimerHandler() D1 0x0009b020-0x0009b035
//     TssSdkTimerHandler::check_monitor(time_t)    0x0009b4f0-0x0009b5a4
//     TssSdkTimerHandler::update_time(time_t)      0x0009b5b0-0x0009b614
//
//   src/formmog/tss_sdk/framework/tss_sdk_plugin.cpp
//     ITssSdkPlugin::ITssSdkPlugin(ETssSdkPluginId) C2 0x000977b0-0x00097899
//     ITssSdkPlugin::ITssSdkPlugin(ETssSdkPluginId) C1 0x000978a0-0x00097989
//     ITssSdkPlugin::~ITssSdkPlugin() D0           0x00097200-0x00097242
//     ITssSdkPlugin::~ITssSdkPlugin() D1           0x00097250-0x00097281
//     ITssSdkPlugin::~ITssSdkPlugin() D2           0x00097290-0x000972c1
//     ITssSdkPlugin::get_last_error()              0x000972d0-0x000973a0
//     ITssSdkPlugin::register_callback(...)        0x00097110-0x0009711e
//     ITssSdkPlugin::get_id()                      0x00097120-0x0009712b
//     ITssSdkPlugin::get_last_error_code()         0x000971d0-0x000971f8
//
//   src/formmog/tss_sdk/adapter/antibotinterface.cpp
//     GetAntiBotInterfaceAll()                     0x00088980-0x000889ac
//
//   src/formmog/tss_sdk/busi_interf/busi_impl/tss_sdk_busi_impl.cpp
//     tss_sdk_get_comm_interf(...)                 0x0008b010-0x0008b0f9
//
//   src/formmog/tss_sdk/busi_interf/anti/tss_sdk_anti_impl.cpp
//     tss_sdk_get_anti_interf(...)                 0x0008a900-0x0008a96c
//
//   src/formmog/tss_sdk/busi_interf/captcha/tss_sdk_captcha_impl.cpp
//     tss_sdk_get_captcha_interf(...)              0x0008b760-0x0008b7a5
//
//   src/formmog/tss_sdk/busi_interf/safemode/tss_sdk_safemode_impl.cpp
//     tss_sdk_get_safemode_interf(...)             0x0008c0d0-0x0008c115
//
//   src/formmog/tss_sdk/busi_interf/uic/tss_sdk_uic_impl.cpp
//     tss_sdk_get_uic_interf(...)                  0x0008c910-0x0008c955
//
// DWARF layouts used here (exact):
//   TssSdkTimerHandler (anonymous DIE, tss_sdk_time_handler.cpp) sizeof 12:
//     +0x00 Comm_Time_Provider* time_provider_
//     +0x04 Comm_Stat_Monitor*  stat_monitor_
//     +0x08 time_t             last_check_
//   ITssSdkPlugin (tss_sdk_plugin.cpp) sizeof 16:
//     +0x00 vptr
//     +0x04 TssSdkPluginCallbackProc callback_proc_
//     +0x08 CThreadErrorInfo error_info_ (sizeof 4)
//     +0x0c ETssSdkPluginId sdk_plugin_id_
//   vtable 0x00292d80 (address point 0x00292d88):
//     D1 0x97250, D0 0x97200, init, fini, proc, notify (last four are
//     __cxa_pure_virtual in the shipped image).
//   AntibotInterface sizeof 4, base IAntibotInterface sizeof 4; global object
//   g_antibot_if lives at 0x0029d060 and is constructed by the TU static
//   initializer at 0x00089093-0x000890b6 (call AntibotInterface::C1 at
//   0x00088580, __cxa_atexit for D1 at 0x00088b60).  The same initializer
//   sets the TU-local g_tss_sdk to tss_get_sdk() at 0x000890bb.
//
// Interface tables (file addresses, all pointers are R_386_32 relocations):
//   0x0029790c s_tss_sdk_anti_interf     (20 bytes, tss_sdk_anti_impl.cpp)
//     +0x00 tss_sdk_add_user            (0x8a270)
//     +0x04 tss_sdk_del_user            (0x8a1e0)
//     +0x08 tss_sdk_is_cheat_pkg        (0x8a560)
//     +0x0c tss_sdk_decrypt_pkg         (0x8a3a0)
//     +0x10 tss_sdk_encrypt_pkg         (0x8a610)
//   0x00297920 s_tss_sdk_captcha_interf (12 bytes)
//     +0x00 tss_sdk_captcha_start       (0x8b950)
//     +0x04 tss_sdk_captcha_change_pic  (0x8b7b0)
//     +0x08 tss_sdk_captcha_verify      (0x8b830)
//   0x0029792c s_tss_sdk_safemode_interf (8 bytes)
//     +0x00 tss_sdk_secoptverify_append_param (0x8bf30)
//     +0x04 tss_sdk_secoptverify_user_operate (0x8c120)
//   0x00297934 s_tss_sdk_uic_interf      (8 bytes)
//     +0x00 tss_sdk_uic_judge_input_chat (0x8ca40)
//     +0x04 tss_sdk_uic_judge_input_name (0x8c960)
//   0x0029de50 tss_sdk_busi_interf       (4 bytes; function-local static)
//
// The callback parameter structures are intentionally left incomplete where
// the shipped mangled names do not expose them; their exact names are taken
// from the relocation symbol names, and their DWARF sizes are recorded in the
// comments.  No interface slot is left null or pointed at a stub.

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <string>
#include <utility>
#include <vector>
#include "src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.h"

// Complete IAntibotInterface/AntibotInterface declarations shared with the
// adapter implementation TUs and the closing base-vtable owner.
#include "../../adapter/iantibotinterface.h"

// ---------------------------------------------------------------------------
// Canonical tag-394 runtime boundary.  ZEN_Trace_LogMsg, ZEN_Time_Value and
// ZEN_Server_Status come from zen_server_status.h (pulled in by the static
// block); Comm_Stat_Monitor from its layout header.  The former local stubs
// are gone: one authoritative declaration per type.
// ---------------------------------------------------------------------------
#include "src/formmog/tss_sdk/layout/comm_stat_monitor.h"

// ORIG static-initialization blocks.  This slice merges two ORIG CUs:
//   tss_sdk_plugin.cpp (16 status ctors + ioinit + status dtor; its
//                       Singleton<TssSdkPluginMgr>::ObjectCreator is the one
//                       shared weak object already constructed by the split
//                       slices, not a second instance)
//   tss_sdk_time_handler.cpp (16 status ctors + ioinit + status dtor)
// Emit one real TU-local status/iostream block per ORIG CU.
#define TSS_SDK_STATIC_INIT_ID tss_sdk_plugin
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID tss_sdk_time_handler
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

class Comm_Time_Provider {
public:
    static Comm_Time_Provider *instance();

    ZEN_Time_Value cur_time_;  // +0, sizeof(Comm_Time_Provider) == 8
};

// ---------------------------------------------------------------------------
// tss_sdk_if types used by the plugin base and by tss_get_sdk().
// ---------------------------------------------------------------------------

namespace tss_sdk_if {


typedef int (*TssSdkPluginCallbackProc)(void *arg);

// tss_sdk_plugin.h line 44, sizeof 12.
struct TssSdkPluginInitData {
    ETssSdkPluginId plugin_id_;                 // +0
    TssSdkPluginCallbackProc callback_proc_;    // +4
    void *init_data_;                           // +8
};

// tss_sdk_plugin.h line 49, sizeof 8.
struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;  // +0
    void *notify_data_;          // +4
};

}  // namespace tss_sdk_if

// ---------------------------------------------------------------------------
// tss_sdk_plugin.cpp: ITssSdkPlugin base.
// ---------------------------------------------------------------------------

#include "src/commlib/zenlib/zen_singleton.h"

namespace tss_sdk_impl {



ITssSdkPlugin::ITssSdkPlugin(tss_sdk_if::ETssSdkPluginId sdk_plugin_id)
    : callback_proc_(0), sdk_plugin_id_(sdk_plugin_id) {
    error_info_.last_error_info()->error_code = 0;
    Singleton<TssSdkPluginMgr>::instance()->register_plugin(this);
}

ITssSdkPlugin::~ITssSdkPlugin() {
}

void ITssSdkPlugin::register_callback(
    tss_sdk_if::TssSdkPluginCallbackProc callback_proc) {
    callback_proc_ = callback_proc;
}

tss_sdk_if::ETssSdkPluginId ITssSdkPlugin::get_id() {
    return sdk_plugin_id_;
}

int ITssSdkPlugin::get_last_error_code() {
    return error_info_.last_error_info()->error_code;
}

// 0x000972d0.  The shipped code builds an empty std::string, then either
// assigns the empty literal (code == 0) or the thread error buffer with
// strlen() bytes.
std::string ITssSdkPlugin::get_last_error() {
    std::string last_err_str;
    if (error_info_.last_error_info()->error_code == 0) {
        last_err_str.assign("", 0);
    } else {
        const char *msg = error_info_.get_last_error_msg();
        last_err_str.assign(msg, strlen(msg));
    }
    return last_err_str;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// tss_sdk_time_handler.cpp: TssSdkTimerHandler.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

// tss_sdk_time_handler.h line 14, sizeof 12.  Non-virtual destructor.
class TssSdkTimerHandler {
public:
    TssSdkTimerHandler();
    ~TssSdkTimerHandler();

    void update_time(time_t now_sec);
    void check_monitor(time_t now_sec);

private:
    Comm_Time_Provider *time_provider_;  // +0x00
    Comm_Stat_Monitor *stat_monitor_;    // +0x04
    time_t last_check_;                  // +0x08
};

TssSdkTimerHandler::TssSdkTimerHandler()
    : time_provider_(Comm_Time_Provider::instance()),
      stat_monitor_(Comm_Stat_Monitor::instance()),
      last_check_(time(0)) {
}

// 0x0009b000 / 0x0009b020: both shipped destructor variants store NULL into
// the two provider pointers and leave last_check_ untouched.
TssSdkTimerHandler::~TssSdkTimerHandler() {
    time_provider_ = 0;
    stat_monitor_ = 0;
}

// 0x0009b5b0: construct a ZEN_Time_Value from the supplied second count and
// assign it into Comm_Time_Provider::cur_time_.
void TssSdkTimerHandler::update_time(time_t now_sec) {
    ZEN_Time_Value zen_now(now_sec);
    time_provider_->cur_time_ = zen_now;
}

// 0x0009b4f0: only fires once per five-minute boundary (now_sec % 300 == 0)
// and only when now_sec differs from the last accepted boundary.  It bumps
// ZEN status item 0x2395, runs the monitor's overtime pass, logs a warning
// when more than 300 seconds elapsed since the previous boundary, and stores
// the new boundary.
void TssSdkTimerHandler::check_monitor(time_t now_sec) {
    if (now_sec % 300 != 0) {
        return;
    }
    if (now_sec == last_check_) {
        return;
    }

    // Stack arguments are statid 0x2395, 0, 0, and the long-long increment 1.
    stat_monitor_->increase_by_statid(0x2395, 0, 0, 1);
    stat_monitor_->check_overtime(now_sec);

    long real_second = now_sec - last_check_;
    if (real_second > 300) {
        ZEN_Trace_LogMsg::debug_errorex(
            "check monitor more than five minutes:real_second=%d",
            real_second);
    }
    last_check_ = now_sec;
}

}  // namespace tss_sdk_impl

// ---------------------------------------------------------------------------
// Common SDK function table returned by tss_get_sdk().
// ---------------------------------------------------------------------------

typedef int (*TssSdkInit)(unsigned int instance_id);
typedef int (*TssSdkInitPlugins)(
    const tss_sdk_if::TssSdkPluginInitData *plugin_init_data,
    unsigned int plugin_count);
typedef int (*TssSdkFini)(tss_sdk_if::ETssSdkPluginId plugin_id);
typedef int (*TssSdkProc)();
typedef int (*TssSdkNotify)(
    tss_sdk_if::TssSdkPluginNotifyData notify_data);
typedef int (*TssSdkGetLastErrorCode)();
typedef const char *(*TssSdkGetLastError)();

// DWARF: anonymous structure, sizeof 28, members init, init_plugins, fini,
// proc, notify, get_last_error_code, get_last_error in that order.
struct TssSdkInterface {
    TssSdkInit init;                            // +0x00
    TssSdkInitPlugins init_plugins;             // +0x04
    TssSdkFini fini;                            // +0x08
    TssSdkProc proc;                            // +0x0c
    TssSdkNotify notify;                        // +0x10
    TssSdkGetLastErrorCode get_last_error_code;// +0x14
    TssSdkGetLastError get_last_error;          // +0x18
};

extern "C" const TssSdkInterface *tss_get_sdk();

// ---------------------------------------------------------------------------
// tss_sdk_busi_impl.cpp: C-style business interface.
// ---------------------------------------------------------------------------

// DWARF: anonymous structure, sizeof 4, member proc_ (TssSdkProc).
struct TssSdkBusiInterf {
    TssSdkProc proc_;  // +0x00
};

// DWARF: anonymous structure, sizeof 4, member unique_instance_id_ (u32).
struct TssSdkCommInitInfo {
    unsigned int unique_instance_id_;  // +0x00
};

// tss_sdk_plugin_channel.h line 19, sizeof 12.  The comm factory builds this
// block for the CHANNEL plugin: the caller supplies only the instance id, the
// factory supplies the IPC bus size (8 MiB) and the pid channel mode (0).
struct TssSdkPluginChannelInitData {
    unsigned int unique_instance_id_;  // +0x00
    unsigned int ipc_bus_size_;        // +0x04
    int pid_channel_mode_;             // +0x08
};

// g_is_old_sdk is a TU-external global (D, size 4) cleared on every call.
extern int g_is_old_sdk;

// The file-local pointer is initialized by the TU's static initializer
// (0x0008b4a3: call tss_get_sdk; store to 0x0029de40).
static const TssSdkInterface *g_tss_sdk = tss_get_sdk();

namespace tss_sdk_impl {
int send_anti_data_to_client(void *data);
}

// 0x0008b010.
extern "C" const TssSdkBusiInterf *tss_sdk_get_comm_interf(
    const TssSdkCommInitInfo *init_info) {
    g_is_old_sdk = 0;

    // Function-local statics: the table is initialized through a guard
    // (0x0029de48) because g_tss_sdk->proc is not a link-time constant; the
    // separate initialized flag is 0x0029de54 and starts false in BSS.
    static TssSdkBusiInterf tss_sdk_busi_interf = { g_tss_sdk->proc };
    static bool is_tss_sdk_busi_interf_inited = false;

    if (!is_tss_sdk_busi_interf_inited) {
        if (g_tss_sdk->init(init_info->unique_instance_id_) != 0) {
            return 0;
        }

        // The shipped stack block at [ebp-0x18] is a 12-byte
        // TssSdkPluginChannelInitData: instance id from the caller, IPC bus
        // size 0x800000, pid channel mode 0.  plugin_init_data.init_data_
        // points at that block.
        TssSdkPluginChannelInitData init_channel_data;
        init_channel_data.unique_instance_id_ =
            init_info->unique_instance_id_;
        init_channel_data.ipc_bus_size_ = 0x800000;
        init_channel_data.pid_channel_mode_ = 0;

        tss_sdk_if::TssSdkPluginInitData plugin_init_data;
        plugin_init_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_CHANNEL;
        plugin_init_data.callback_proc_ =
            tss_sdk_impl::send_anti_data_to_client;
        plugin_init_data.init_data_ = &init_channel_data;

        if (g_tss_sdk->init_plugins(&plugin_init_data, 1) != 0) {
            return 0;
        }
    }

    is_tss_sdk_busi_interf_inited = true;
    return &tss_sdk_busi_interf;
}

// ---------------------------------------------------------------------------
// tss_sdk_anti_impl.cpp: anti interface.
// ---------------------------------------------------------------------------

// Return enums recovered from DWARF (anonymous enum DIEs).
enum TssSdkProcResult {
    TSS_SDK_PROC_OK = 0,
    TSS_SDK_PROC_INVALID_ARG = 1,
    TSS_SDK_PROC_INTERNAL_ERR = 2,
    TSS_SDK_PROC_FAIL = 3
};

enum TssSdkAntiDecryptResult {
    TSS_SDK_ANTI_DECRYPT_OK = 0,
    TSS_SDK_ANTI_INTERNAL_PKG = 1
};

enum TssSdkAntiEncryptResult {
    TSS_SDK_ANTI_ENCRYPT_OK = 0,
    TSS_SDK_ANTI_NOT_NEED_ENCRYPT = 1
};

// Parameter types are the real mangled names (from the interface-table
// relocations); only the pointer/reference declarations are needed here.
struct TssSdkAntiAddUserInfo;          // DWARF size 16
struct TssSdkAntiDelUserInfo;          // DWARF size 4
struct TssSdkAntiGamePkgInfo;          // DWARF size 16
struct TssSdkAntiDecryptPkgInfo;       // DWARF size 20
struct TssSdkAntiEncryptPkgInfo;       // DWARF size 24

// Callback parameter structures of TssSdkAntiInitInfo.  DWARF gives the
// sizes but no public names; the names below follow the callback names.
struct TssSdkAntiSendDataToClientInfo;  // DWARF size 10
struct TssSdkAntiPunishUserInfo;        // DWARF size 24

typedef TssSdkProcResult (*TssSdkSendDataToClient)(
    const TssSdkAntiSendDataToClientInfo *info);
typedef TssSdkProcResult (*TssSdkPunishUser)(
    const TssSdkAntiPunishUserInfo *info);

// DWARF: anonymous structure, sizeof 8.
struct TssSdkAntiInitInfo {
    TssSdkSendDataToClient send_data_to_client_;  // +0x00
    TssSdkPunishUser punish_user_;                // +0x04
};

typedef TssSdkProcResult (*TssSdkAddUser)(TssSdkAntiAddUserInfo *info);
typedef TssSdkProcResult (*TssSdkDelUser)(
    const TssSdkAntiDelUserInfo *info);
typedef int (*TssSdkIsCheatPkg)(const TssSdkAntiGamePkgInfo *info);
typedef TssSdkAntiDecryptResult (*TssSdkDecryptPkg)(
    TssSdkAntiDecryptPkgInfo *info);
typedef TssSdkAntiEncryptResult (*TssSdkEncryptPkg)(
    TssSdkAntiEncryptPkgInfo *info);

// DWARF: anonymous structure, sizeof 20.
struct TssSdkAntiInterf {
    TssSdkAddUser add_user_;          // +0x00
    TssSdkDelUser del_user_;          // +0x04
    TssSdkIsCheatPkg is_cheat_pkg_;   // +0x08
    TssSdkDecryptPkg decrypt_pkg_;    // +0x0c
    TssSdkEncryptPkg encrypt_pkg_;    // +0x10
};

namespace tss_sdk_impl {
TssSdkProcResult tss_sdk_add_user(TssSdkAntiAddUserInfo *info);
TssSdkProcResult tss_sdk_del_user(const TssSdkAntiDelUserInfo *info);
int tss_sdk_is_cheat_pkg(const TssSdkAntiGamePkgInfo *info);
TssSdkAntiDecryptResult tss_sdk_decrypt_pkg(TssSdkAntiDecryptPkgInfo *info);
TssSdkAntiEncryptResult tss_sdk_encrypt_pkg(TssSdkAntiEncryptPkgInfo *info);
int tss_sdk_anti_init(const TssSdkAntiInitInfo *init_data);
}  // namespace tss_sdk_impl

// 0x0008a900.  Both callback slots must be non-null before the initializer
// runs; a nonzero initializer result is logged and turned into a null return.
extern "C" const TssSdkAntiInterf *tss_sdk_get_anti_interf(
    const TssSdkAntiInitInfo *init_data) {
    if (init_data == 0 || init_data->punish_user_ == 0 ||
        init_data->send_data_to_client_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex("anti_init: invalid arg.");
        return 0;
    }

    int err = tss_sdk_impl::tss_sdk_anti_init(init_data);
    if (err != 0) {
        ZEN_Trace_LogMsg::debug_errorex("anti init fail. err=%d", err);
        return 0;
    }

    // Function-local static, file offset 0x0029790c, sizeof 20.
    static TssSdkAntiInterf s_tss_sdk_anti_interf = {
        tss_sdk_impl::tss_sdk_add_user,
        tss_sdk_impl::tss_sdk_del_user,
        tss_sdk_impl::tss_sdk_is_cheat_pkg,
        tss_sdk_impl::tss_sdk_decrypt_pkg,
        tss_sdk_impl::tss_sdk_encrypt_pkg
    };
    return &s_tss_sdk_anti_interf;
}

// ---------------------------------------------------------------------------
// tss_sdk_captcha_impl.cpp: captcha interface.
// ---------------------------------------------------------------------------

struct TssSdkCaptchaStartInfo;        // DWARF size not used here
struct TssSdkCaptchaChangePicInfo;    // DWARF size not used here
struct TssSdkCaptchaVerifyReqInfo;    // DWARF size not used here

struct TssSdkCaptchaRecvPicDataInfo;  // DWARF size 37
struct TssSdkCaptchaVerifyRspInfo;    // DWARF size 16

typedef TssSdkProcResult (*TssSdkCaptchaRecvPicData)(
    const TssSdkCaptchaRecvPicDataInfo *info);
typedef TssSdkProcResult (*TssSdkCaptchaVerifyRsp)(
    const TssSdkCaptchaVerifyRspInfo *info);

// DWARF: anonymous structure, sizeof 8.
struct TssSdkCaptchaInitInfo {
    TssSdkCaptchaRecvPicData on_recv_pic_data_;  // +0x00
    TssSdkCaptchaVerifyRsp on_verify_rsp_;       // +0x04
};

typedef TssSdkProcResult (*TssSdkCaptchaStart)(
    const TssSdkCaptchaStartInfo *info);
typedef TssSdkProcResult (*TssSdkCaptchaChangePic)(
    const TssSdkCaptchaChangePicInfo *info);
typedef TssSdkProcResult (*TssSdkCaptchaVerify)(
    const TssSdkCaptchaVerifyReqInfo *info);

// DWARF: anonymous structure, sizeof 12.
struct TssSdkCaptchaInterf {
    TssSdkCaptchaStart start_;           // +0x00
    TssSdkCaptchaChangePic change_pic_;  // +0x04
    TssSdkCaptchaVerify verify_;         // +0x08
};

namespace tss_sdk_impl {
TssSdkProcResult tss_sdk_captcha_start(const TssSdkCaptchaStartInfo *info);
TssSdkProcResult tss_sdk_captcha_change_pic(
    const TssSdkCaptchaChangePicInfo *info);
TssSdkProcResult tss_sdk_captcha_verify(
    const TssSdkCaptchaVerifyReqInfo *info);
int tss_sdk_captcha_init(const TssSdkCaptchaInitInfo *init_info);
}  // namespace tss_sdk_impl

// 0x0008b760.  The wrapper does not pre-validate init_info; the initializer
// owns all argument checks.
extern "C" const TssSdkCaptchaInterf *tss_sdk_get_captcha_interf(
    const TssSdkCaptchaInitInfo *init_info) {
    int err = tss_sdk_impl::tss_sdk_captcha_init(init_info);
    if (err != 0) {
        ZEN_Trace_LogMsg::debug_errorex("captcha init fail. err=%d", err);
        return 0;
    }

    // Function-local static, file offset 0x00297920, sizeof 12.
    static TssSdkCaptchaInterf s_tss_sdk_captcha_interf = {
        tss_sdk_impl::tss_sdk_captcha_start,
        tss_sdk_impl::tss_sdk_captcha_change_pic,
        tss_sdk_impl::tss_sdk_captcha_verify
    };
    return &s_tss_sdk_captcha_interf;
}

// ---------------------------------------------------------------------------
// tss_sdk_safemode_impl.cpp: safemode interface.
// ---------------------------------------------------------------------------

struct TssSdkSecOptVerifyAppendParamInfo;  // DWARF size 1032
struct TssSdkSecOptVerifyReqInfo;          // DWARF size not used here

struct TssSdkSecOptVerifySetLimitInfoData; // DWARF size 1032
struct TssSdkSecOptSetUserSafeStatusInfo;  // DWARF size 8
struct TssSdkSecOptSetVerifyInfoData;      // DWARF size 1032
struct TssSdkSecOptSetVerifyResultInfo;    // DWARF size 12

typedef TssSdkProcResult (*TssSdkSecOptVerifySetLimitInfo)(
    const TssSdkSecOptVerifySetLimitInfoData *info);
typedef TssSdkProcResult (*TssSdkSecOptSetUserSafeStatus)(
    const TssSdkSecOptSetUserSafeStatusInfo *info);
typedef TssSdkProcResult (*TssSdkSecOptSetVerifyInfo)(
    const TssSdkSecOptSetVerifyInfoData *info);
typedef TssSdkProcResult (*TssSdkSecOptSetVerifyResult)(
    const TssSdkSecOptSetVerifyResultInfo *info);

// DWARF: anonymous structure, sizeof 16.  The shipped initializer type name
// is visible in tss_sdk_safemode_init's mangled symbol.
struct TssSdkSafemodeInitData {
    TssSdkSecOptVerifySetLimitInfo set_limit_info_;  // +0x00
    TssSdkSecOptSetUserSafeStatus set_safe_status_;  // +0x04
    TssSdkSecOptSetVerifyInfo set_verify_info_;      // +0x08
    TssSdkSecOptSetVerifyResult set_verify_result_;  // +0x0c
};

typedef TssSdkProcResult (*TssSdkSecOptVerifyAppendParam)(
    TssSdkSecOptVerifyAppendParamInfo *info);
typedef TssSdkProcResult (*TssSdkSecOptVerifyUserOperate)(
    const TssSdkSecOptVerifyReqInfo &info);

// DWARF: anonymous structure, sizeof 8.
struct TssSdkSafemodeInterf {
    TssSdkSecOptVerifyAppendParam append_param_;  // +0x00
    TssSdkSecOptVerifyUserOperate verify_user_;   // +0x04
};

namespace tss_sdk_impl {
TssSdkProcResult tss_sdk_secoptverify_append_param(
    TssSdkSecOptVerifyAppendParamInfo *info);
TssSdkProcResult tss_sdk_secoptverify_user_operate(
    const TssSdkSecOptVerifyReqInfo &info);
int tss_sdk_safemode_init(const TssSdkSafemodeInitData *init_info);
}  // namespace tss_sdk_impl

// 0x0008c0d0.
extern "C" const TssSdkSafemodeInterf *tss_sdk_get_safemode_interf(
    const TssSdkSafemodeInitData *init_info) {
    int err = tss_sdk_impl::tss_sdk_safemode_init(init_info);
    if (err != 0) {
        ZEN_Trace_LogMsg::debug_errorex("safemode init fail. err=%d", err);
        return 0;
    }

    // Function-local static, file offset 0x0029792c, sizeof 8.
    static TssSdkSafemodeInterf s_tss_sdk_safemode_interf = {
        tss_sdk_impl::tss_sdk_secoptverify_append_param,
        tss_sdk_impl::tss_sdk_secoptverify_user_operate
    };
    return &s_tss_sdk_safemode_interf;
}

// ---------------------------------------------------------------------------
// tss_sdk_uic_impl.cpp: user-input-check interface.
// ---------------------------------------------------------------------------

struct TssSdkUicChatUserInputInfo;  // DWARF size not used here
struct TssSdkUicNameUserInputInfo;  // DWARF size 13

struct TssSdkUicChatOnJudgeResultInfo;  // DWARF size 22

typedef TssSdkProcResult (*TssSdkUicChatOnJudgeResult)(
    const TssSdkUicChatOnJudgeResultInfo *info);

// DWARF: anonymous structure, sizeof 4.
struct TssSdkUicInitInfo {
    TssSdkUicChatOnJudgeResult on_chat_judge_result_;  // +0x00
};

typedef TssSdkProcResult (*TssSdkUicJudgeUserInputChat)(
    const TssSdkUicChatUserInputInfo *info);
typedef TssSdkProcResult (*TssSdkUicJudgeUserInputName)(
    TssSdkUicNameUserInputInfo *info);

// DWARF: anonymous structure, sizeof 8.
struct TssSdkUicInterf {
    TssSdkUicJudgeUserInputChat judge_user_input_chat_;  // +0x00
    TssSdkUicJudgeUserInputName judge_user_input_name_;  // +0x04
};

namespace tss_sdk_impl {
TssSdkProcResult tss_sdk_uic_judge_input_chat(
    const TssSdkUicChatUserInputInfo *info);
TssSdkProcResult tss_sdk_uic_judge_input_name(
    TssSdkUicNameUserInputInfo *info);
int tss_sdk_uic_init(const TssSdkUicInitInfo *init_info);
}  // namespace tss_sdk_impl

// 0x0008c910.
extern "C" const TssSdkUicInterf *tss_sdk_get_uic_interf(
    const TssSdkUicInitInfo *init_info) {
    int err = tss_sdk_impl::tss_sdk_uic_init(init_info);
    if (err != 0) {
        ZEN_Trace_LogMsg::debug_errorex("uic init fail. err=%d", err);
        return 0;
    }

    // Function-local static, file offset 0x00297934, sizeof 8.
    static TssSdkUicInterf s_tss_sdk_uic_interf = {
        tss_sdk_impl::tss_sdk_uic_judge_input_chat,
        tss_sdk_impl::tss_sdk_uic_judge_input_name
    };
    return &s_tss_sdk_uic_interf;
}

// ---------------------------------------------------------------------------
// antibotinterface.cpp: GetAntiBotInterfaceAll and its singleton object.
// ---------------------------------------------------------------------------

// DWARF: IAntibotInterface sizeof 4 with 18 pure virtual slots (vtable
// 0x00292d20, address point 0x00292d28, no virtual destructor).
// AntibotInterface sizeof 4 derives from it; vtable 0x00292ca0, address point
// 0x00292ca8, 18 business slots followed by D1/D0 at 0x00292cf0/f4.
// The complete declarations are shared through adapter/iantibotinterface.h.

// File-local BSS object at 0x0029d060, sizeof 4.  The TU static initializer
// constructs it through AntibotInterface::C1 and registers its destructor.
static AntibotInterface g_antibot_if;

// 0x00088980.  The function only logs and returns the address of the global
// object; it performs no lazy initialization of its own.
extern "C" IAntibotInterface *GetAntiBotInterfaceAll() {
    ZEN_Trace_LogMsg::debug_infoex("In GetAntiBotInterfaceAll.");
    return &g_antibot_if;
}
