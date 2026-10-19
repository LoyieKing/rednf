// Partial reconstruction of libtss_sdk.so.1.0.394 (SHA-256
// 8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7).
//
// Original source unit:
//   src/formmog/tss_sdk/adapter/antibotinterface.cpp
// Reconstructed here:
//   AntibotInterface::IsCmdInDownList 0x00088670..0x000886e4 (0x74 bytes)
//   AntibotInterface::IsCmdInUpList   0x000886f0..0x0008876a (0x7a bytes)
//   AntibotInterface::QueryProtectPolicy
//                                     0x00088bd0..0x00088d24 (0x154 bytes)
//   AntibotInterface::ProcessAntiBotInput
//                                     0x00089d50..0x00089e1b (0xcb bytes)
//   AntibotInterface::AntiCheatPacket 0x00089fd0..0x0008a131 (0x161 bytes)
//
// The disassembly is authoritative.  The complete base/derived class
// declarations are shared through iantibotinterface.h; the packet and trace
// types below remain local ABI-faithful declarations.

#include "iantibotinterface.h"

// ---------------------------------------------------------------------------
// Resolved external dependencies (nm -C; PLT targets)
// ---------------------------------------------------------------------------
// _ZN16ZEN_Trace_LogMsg13debug_debugExEPKcz  0x001b3dd0
// _ZN16ZEN_Trace_LogMsg12debug_infoexEPKcz   0x001b3d80
// _ZN16ZEN_Trace_LogMsg13debug_errorexEPKcz  0x001b3d30
// tss_get_sdk                                 0x00091700 (unmangled/C linkage)
//
// The generated DWARF header truncates these to one formal parameter; the
// resolved symbols are variadic (mangled type suffix `PKcz`).
class ZEN_Trace_LogMsg {
public:
    static void debug_debugEx(const char *str_format, ...);
    static void debug_infoex(const char *str_format, ...);
    static void debug_errorex(const char *str_format, ...);
};

// ---------------------------------------------------------------------------
// TssSdk function-pointer table
// ---------------------------------------------------------------------------
// tss_get_sdk() returns tss_get_sdk::tss_sdk (DWARF location 0x00297944), a
// 28-byte struct of function pointers.  The methods below use exactly:
//   +0x0c proc()                                      (ProcessAntiBotInput)
//   +0x10 notify(tss_sdk_if::TssSdkPluginNotifyData)  (all four others)
// This is NOT an IBusiness vtable: g_business (IBusiness*) at 0x0029d1a0 is
// never referenced by these five functions.
namespace tss_sdk_if {

enum ETssSdkPluginId {
    ETSS_SDK_PLUGIN_ID_INVALID = -2,
    ETSS_SDK_PLUGIN_ID_ALL = -1,
    ETSS_SDK_PLUGIN_ID_BEGIN = 0,
    ETSS_SDK_PLUGIN_ID_CHANNEL = 1,
    ETSS_SDK_PLUGIN_ID_USER_CONTROL = 2,
    ETSS_SDK_PLUGIN_ID_PROTO_PROC = 3,
    ETSS_SDK_PLUGIN_ID_SAFEMODE = 4,
    ETSS_SDK_PLUGIN_ID_CAPTCHA = 5,
    ETSS_SDK_PLUGIN_ID_UIC = 6,
    ETSS_SDK_PLUGIN_ID_DATA_TRANS = 7,
    ETSS_SDK_PLUGIN_ID_END = 8
};

// DWARF size 8: plugin_id_ at +0, notify_data_ at +4.  Passed by value to
// TssSdk::notify; the two dwords land in [esp] and [esp+4].
struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;
    void *notify_data_;
};

} // namespace tss_sdk_if

struct TssSdk {
    int (*init)(unsigned int);
    int (*init_plugins)(const int *arg0, unsigned int arg1);
    int (*fini)(int arg0);
    int (*proc)();
    int (*notify)(tss_sdk_if::TssSdkPluginNotifyData);
    int (*get_last_error_code)();
    const char *(*get_last_error)();
};

// ORIG antibotinterface.cpp has one file-local pointer `g_tss_sdk` at
// 0x0029d064, assigned once from tss_get_sdk() (0x890c0).  The reconstruction
// split that TU, so the single definition lives in antibot_client_agent.cpp
// (hidden visibility) and every split half refers to it.
__attribute__((visibility("hidden"))) extern TssSdk *g_tss_sdk;

// ---------------------------------------------------------------------------
// Protocol package structures
// ---------------------------------------------------------------------------
// DWARF (antibotinterface.cpp CU):
//   TssSdkPluginProtoProcPkg        size 36: head_ +0, body_ +4 (union 32)
//   TssSdkPluginProtoProcHead       size 4 : msg_id_ +0
//   decrypt_list_pkg_ (._501)       size 16: check_result_ 0, cmd_id_ 4,
//                                           uin_ 8, idx_ 12
//   down_pkg_ (._502)               size 32
//   notify_pkg_ (._503)             size 16
//   anti_cheating_pkg_ (._504)      size 24: is_cheating_pkg_ 0, cmd_id_ 4,
//                                           uin_ 8, idx_ 12, game_pkg_ 16,
//                                           game_pkg_len_ 20
//   antibot_pkg_ (._505)            size 12
//   dp_pkg_ (._506)                 size 28
//   down_list_pkg_ (._507)          size 16: cmd_id_ 0, uin_ 4, idx_ 8,
//                                           is_in_down_list_ 12
//
// `CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntiCheatingPkg>` has DWARF
// size 25 with core_pkg_ at +1, proving the protocol packages have alignment
// 1 (packed).  __attribute__((packed)) reproduces that layout on GCC 4.1 and
// on the current syntax checker.
enum ETssSdkPluginProtoProcMsgId {
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG = 201,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG = 202,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG = 203,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST = 304,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST = 305,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ON_NOTIFY_SERVER = 306,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG_OLD = 307,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG_OLD = 308,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ANTIBOT_PKG = 309
};

struct TssSdkPluginProtoProcHead {
    ETssSdkPluginProtoProcMsgId msg_id_;
} __attribute__((packed));

struct TssSdkPluginProtoProcDecryptListPkg {
    int check_result_;
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
} __attribute__((packed));

struct TssSdkPluginProtoProcDownPkg {
    int encrypt_result_;
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
    const unsigned char *input_buf_;
    unsigned int input_buf_size_;
    unsigned char *output_buf_;
    unsigned int output_buf_size_;
} __attribute__((packed));

struct TssSdkPluginProtoProcNotifyPkg {
    int notify_result_;
    unsigned int main_id_;
    const unsigned char *buf_;
    unsigned int buf_size_;
} __attribute__((packed));

struct TssSdkPluginProtoProcAntiCheatingPkg {
    int is_cheating_pkg_;
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
    const unsigned char *game_pkg_;
    unsigned int game_pkg_len_;
} __attribute__((packed));

struct TssSdkPluginProtoProcAntibotPkg {
    unsigned int uin_;
    const unsigned char *buf_;
    unsigned int buf_size_;
} __attribute__((packed));

struct TssSdkPluginProtoProcDpPkg {
    int decrypt_result_;
    unsigned int idx_;
    unsigned int uin_;
    const unsigned char *input_buf_;
    unsigned int input_buf_size_;
    unsigned char *output_buf_;
    unsigned int output_buf_size_;
} __attribute__((packed));

struct TssSdkPluginProtoProcDownListPkg {
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
    int is_in_down_list_;
} __attribute__((packed));

union TssSdkPluginProtoProcBody {
    TssSdkPluginProtoProcDecryptListPkg decrypt_list_pkg_;
    TssSdkPluginProtoProcDownPkg down_pkg_;
    TssSdkPluginProtoProcNotifyPkg notify_pkg_;
    TssSdkPluginProtoProcAntiCheatingPkg anti_cheating_pkg_;
    TssSdkPluginProtoProcAntibotPkg antibot_pkg_;
    TssSdkPluginProtoProcDpPkg dp_pkg_;
    TssSdkPluginProtoProcDownListPkg down_list_pkg_;
} __attribute__((packed));

struct TssSdkPluginProtoProcPkg {
    TssSdkPluginProtoProcHead head_;
    TssSdkPluginProtoProcBody body_;
} __attribute__((packed));

// ---------------------------------------------------------------------------
// Core-dump trace helper (inlined by the original TU)
// ---------------------------------------------------------------------------
// DWARF layouts:
//   CTssCoreDumpPkgTrace<TssSdkPluginNotifyData>                size 12
//       is_in_func_ +0, core_pkg_ +4
//   CTssCoreDumpPkgTrace<unsigned int>                          size  8
//       is_in_func_ +0, core_pkg_ +4
//   CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntiCheatingPkg>  size 25
//       is_in_func_ +0, core_pkg_ +1
//
// Original source line numbers in the DWARF line table:
//   ctor 53 (is_in_func_ = false), 55 (debug_infoex registration)
//   enter 61 (is_in_func_ = true), 62 (core_pkg_ = pkg)
//   leave 66 (is_in_func_ = false)
//   adapter ctor 71, dtor 75
// The function-local static object is initialized once through
// __cxa_guard_acquire/__cxa_guard_release; its guard and object addresses are
// given in AGENT_REPORT.md.
template <typename T>
class CTssCoreDumpPkgTrace {
public:
    class CTssCoreDumpPkgTraceAdapter {
    public:
        CTssCoreDumpPkgTraceAdapter(const CTssCoreDumpPkgTrace &dump_info,
                                    const T &pkg)
            : dump_info_(dump_info) {
            dump_info_.enter(pkg);
        }
        ~CTssCoreDumpPkgTraceAdapter() {
            dump_info_.leave();
        }

    private:
        const CTssCoreDumpPkgTrace &dump_info_;
    };

    CTssCoreDumpPkgTrace(const char *func, const char *type)
        : is_in_func_(false) {
        ZEN_Trace_LogMsg::debug_infoex(
            "CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p",
            func, type, (void *)&is_in_func_, (void *)&core_pkg_);
    }

    void enter(const T &pkg) const {
        is_in_func_ = true;
        core_pkg_ = pkg;
    }

    void leave() const {
        is_in_func_ = false;
    }

private:
    mutable bool is_in_func_;
    mutable T core_pkg_;
};

// Definitions for the class declared in iantibotinterface.h.  SSafeModePkg
// remains forward-declared there (real declaration in ibusiness.h, sizeof 44).

// ---------------------------------------------------------------------------
// 0x00088670..0x000886e4
// ---------------------------------------------------------------------------
// Control flow:
//   head_.msg_id_ = 305
//   body_.down_list_pkg_.cmd_id_ = cmd_id
//   body_.down_list_pkg_.idx_    = key       (uin_ deliberately not touched)
//   body_.down_list_pkg_.is_in_down_list_ = 0
//   g_tss_sdk->notify({plugin_id=3, data=&pkg})       [table slot +0x10]
//   debug_debugEx(fmt, key, cmd_id, is_in_down_list_)
//   return is_in_down_list_;                          (not normalized)
int AntibotInterface::IsCmdInDownList(unsigned int key, int cmd_id) {
    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST;
    proto_proc_pkg.body_.down_list_pkg_.cmd_id_ = cmd_id;
    proto_proc_pkg.body_.down_list_pkg_.idx_ = key;
    // DownList is the only method that returns the raw plugin result.
    proto_proc_pkg.body_.down_list_pkg_.is_in_down_list_ = 0;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    g_tss_sdk->notify(notify_data);

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::IsCmdInDownList, key=%u, cmd_id=%d, res=%d",
        key, cmd_id,
        proto_proc_pkg.body_.down_list_pkg_.is_in_down_list_);
    return proto_proc_pkg.body_.down_list_pkg_.is_in_down_list_;
}

// ---------------------------------------------------------------------------
// 0x000886f0..0x0008876a
// ---------------------------------------------------------------------------
// Control flow:
//   head_.msg_id_ = 304
//   body_.decrypt_list_pkg_.cmd_id_ = cmd_id
//   body_.decrypt_list_pkg_.idx_    = key       (uin_ deliberately not touched)
//   body_.decrypt_list_pkg_.check_result_ = -1
//   g_tss_sdk->notify({plugin_id=3, data=&pkg})       [table slot +0x10]
//   debug_debugEx(fmt, key, cmd_id, check_result_)
//   return check_result_ == 0;                         (sete)
int AntibotInterface::IsCmdInUpList(unsigned int key, int cmd_id) {
    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST;
    proto_proc_pkg.body_.decrypt_list_pkg_.cmd_id_ = cmd_id;
    proto_proc_pkg.body_.decrypt_list_pkg_.idx_ = key;
    proto_proc_pkg.body_.decrypt_list_pkg_.check_result_ = -1;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    g_tss_sdk->notify(notify_data);

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::IsCmdInUpList, key=%d, cmd_id=%d, result=%d.",
        key, cmd_id,
        proto_proc_pkg.body_.decrypt_list_pkg_.check_result_);
    return proto_proc_pkg.body_.decrypt_list_pkg_.check_result_ == 0;
}

// ---------------------------------------------------------------------------
// 0x00088bd0..0x00088d24
// ---------------------------------------------------------------------------
// Control flow:
//   if (!safemode_pkg) { debug_errorex(...); return -1; }
//   debug_debugEx(fmt, uin, safemode_pkg)
//   one-time static CTssCoreDumpPkgTrace<TssSdkPluginNotifyData>
//       guard 0x0029d068, object 0x0029d070 (size 12)
//   adapter enter copies notify_data into core_pkg_
//   rc = g_tss_sdk->notify({plugin_id=4, data=safemode_pkg})  [slot +0x10]
//   adapter leave (is_in_func_ = 0)
//   debug_debugEx(fmt, rc)
//   return rc == 0;
int AntibotInterface::QueryProtectPolicy(int channel_id, int room_id,
                                         unsigned int uin,
                                         SSafeModePkg *safemode_pkg) {
    if (safemode_pkg == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::QueryProtectPolicy, invalid param,uin=%u,safemode_pkg=%p",
            uin, (void *)safemode_pkg);
        return -1;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::QueryProtectPolicy, uin=%u,safemode_pkg=%p", uin,
        (void *)safemode_pkg);

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE;
    notify_data.notify_data_ = safemode_pkg;

    static CTssCoreDumpPkgTrace<tss_sdk_if::TssSdkPluginNotifyData>
        tmp_core_dump_info(__func__, "TssSdkPluginNotifyData");
    CTssCoreDumpPkgTrace<tss_sdk_if::TssSdkPluginNotifyData>::
        CTssCoreDumpPkgTraceAdapter temp_trace(tmp_core_dump_info, notify_data);

    int rc = g_tss_sdk->notify(notify_data);
    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::QueryProtectPolicy, rc=%d", rc);
    return rc == 0;
}

// ---------------------------------------------------------------------------
// 0x00089d50..0x00089e1b
// ---------------------------------------------------------------------------
// Control flow:
//   unsigned int dummy = 0;
//   one-time static CTssCoreDumpPkgTrace<unsigned int>
//       guard 0x0029d0c8, object 0x0029d0d0 (size 8)
//   adapter enter(dummy) -> is_in_func_=1, core_pkg_=0
//   return g_tss_sdk->proc();                         [table slot +0x0c]
//   adapter leave (is_in_func_ = 0) during return
// The proc() result is returned unchanged (no xor eax,eax in the epilogue).
int AntibotInterface::ProcessAntiBotInput() {
    unsigned int dummy = 0;

    static CTssCoreDumpPkgTrace<unsigned int> tmp_core_dump_info(
        __func__, "unsigned int");
    CTssCoreDumpPkgTrace<unsigned int>::CTssCoreDumpPkgTraceAdapter temp_trace(
        tmp_core_dump_info, dummy);

    return g_tss_sdk->proc();
}

// ---------------------------------------------------------------------------
// 0x00089fd0..0x0008a131
// ---------------------------------------------------------------------------
// Control flow:
//   head_.msg_id_ = 201
//   body_.anti_cheating_pkg_.cmd_id_ = cmd_id
//   body_.anti_cheating_pkg_.idx_    = key       (uin_ deliberately not touched)
//   body_.anti_cheating_pkg_.game_pkg_ = 0
//   body_.anti_cheating_pkg_.game_pkg_len_ = 0
//   body_.anti_cheating_pkg_.is_cheating_pkg_ = 0
//   one-time static CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntiCheatingPkg>
//       guard 0x0029d080, object 0x0029d088 (size 25; core_pkg_ at +1)
//   adapter enter copies the 24-byte anti_cheating_pkg_
//   g_tss_sdk->notify({plugin_id=3, data=&pkg})       [table slot +0x10]
//   adapter leave
//   debug_debugEx(fmt, key, cmd_id, is_cheating_pkg_)
//   return is_cheating_pkg_;
int AntibotInterface::AntiCheatPacket(unsigned int key, int cmd_id) {
    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG;
    proto_proc_pkg.body_.anti_cheating_pkg_.cmd_id_ = cmd_id;
    proto_proc_pkg.body_.anti_cheating_pkg_.idx_ = key;
    proto_proc_pkg.body_.anti_cheating_pkg_.game_pkg_ = 0;
    proto_proc_pkg.body_.anti_cheating_pkg_.game_pkg_len_ = 0;
    proto_proc_pkg.body_.anti_cheating_pkg_.is_cheating_pkg_ = 0;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    static CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntiCheatingPkg>
        tmp_core_dump_info(__func__,
                           "TssSdkPluginProtoProcAntiCheatingPkg");
    CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntiCheatingPkg>::
        CTssCoreDumpPkgTraceAdapter temp_trace(
            tmp_core_dump_info,
            proto_proc_pkg.body_.anti_cheating_pkg_);

    g_tss_sdk->notify(notify_data);

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::AntiCheatPacket, key=%u, cmd_id=%d, is_cheating_pkg=%d.",
        key, cmd_id,
        proto_proc_pkg.body_.anti_cheating_pkg_.is_cheating_pkg_);
    return proto_proc_pkg.body_.anti_cheating_pkg_.is_cheating_pkg_;
}
