// Reconstructed from libtss_sdk.so.1.0.394
//   src/formmog/tss_sdk/adapter/antibotinterface.cpp lines 464-491, 505-548,
//   555-596, 683-715.
//
// Partial TU. This file contains only the four Antibot packet entry points:
//   0x00089770 AntibotInterface::OnRecvBufferFromClient
//   0x00089900 AntibotInterface::RecvBufferForEncrypt
//   0x00089b30 AntibotInterface::RecvBufferForDecrypt
//   0x00089e20 AntibotInterface::OnNotifyServer
//
// The complete base/derived hierarchy is declared in iantibotinterface.h and
// shared with the ctor/factory/other implementation TUs.  Helper packet/trace
// types below remain ABI-faithful local declarations, not a replacement for
// the real plugin headers.
//
// The global data table used below is g_tss_sdk (TssSdk*, 0x0029d064), not
// g_business (0x0029d1a0).  All four methods call g_tss_sdk->notify(...) through
// the TssSdk function table slot at +0x10.  g_tss_sdk is initialized once by
// the original TU's static initializer at 0x000890bb with tss_get_sdk():
//     g_tss_sdk = tss_get_sdk();
// It is declared extern here; the integration TU must define it once.

#include "iantibotinterface.h"

struct ZEN_Trace_LogMsg {
    static void debug_infoex(const char *str_format, ...);
    static void debug_debugEx(const char *str_format, ...);
    static void debug_errorex(const char *str_format, ...);
};

namespace tss_sdk_if {

enum TssSdkPluginId {
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

// size 8; notify() receives this by value (two stack dwords: id, data pointer).
struct TssSdkPluginNotifyData {
    TssSdkPluginId plugin_id_;
    void *notify_data_;
};

} // namespace tss_sdk_if

enum TssSdkPluginProtoProcMsgId {
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

// All layouts below were read from DWARF of the antibotinterface.cpp CU and
// cross-checked against the store/load offsets in the disassembly.
struct TssSdkPluginProtoProcHead {                       // size 4
    TssSdkPluginProtoProcMsgId msg_id_;
};

struct TssSdkPluginProtoProcDecryptListPkg {            // size 16
    int check_result_;
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
};

struct TssSdkPluginProtoProcDownPkg {                   // size 32
    int encrypt_result_;
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
    const unsigned char *input_buf_;
    unsigned int input_buf_size_;
    unsigned char *output_buf_;
    unsigned int output_buf_size_;
};

struct TssSdkPluginProtoProcNotifyPkg {                 // size 16
    int notify_result_;
    unsigned int main_id_;
    const unsigned char *buf_;
    unsigned int buf_size_;
};

struct TssSdkPluginProtoProcAntiCheatingPkg {           // size 24
    int is_cheating_pkg_;
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
    const unsigned char *game_pkg_;
    unsigned int game_pkg_len_;
};

struct TssSdkPluginProtoProcAntibotPkg {                // size 12
    unsigned int uin_;
    const unsigned char *buf_;
    unsigned int buf_size_;
};

struct TssSdkPluginProtoProcDpPkg {                     // size 28
    int decrypt_result_;
    unsigned int idx_;
    unsigned int uin_;
    const unsigned char *input_buf_;
    unsigned int input_buf_size_;
    unsigned char *output_buf_;
    unsigned int output_buf_size_;
};

struct TssSdkPluginProtoProcDownListPkg {               // size 16
    int cmd_id_;
    unsigned int uin_;
    unsigned int idx_;
    int is_in_down_list_;
};

union TssSdkPluginProtoProcBody {                       // size 32
    TssSdkPluginProtoProcDecryptListPkg decrypt_list_pkg_;
    TssSdkPluginProtoProcDownPkg down_pkg_;
    TssSdkPluginProtoProcNotifyPkg notify_pkg_;
    TssSdkPluginProtoProcAntiCheatingPkg anti_cheating_pkg_;
    TssSdkPluginProtoProcAntibotPkg antibot_pkg_;
    TssSdkPluginProtoProcDpPkg dp_pkg_;
    TssSdkPluginProtoProcDownListPkg down_list_pkg_;
};

struct TssSdkPluginProtoProcPkg {                       // size 36
    TssSdkPluginProtoProcHead head_;
    TssSdkPluginProtoProcBody body_;
};

// tss_sdk_if.h function table.  Only notify (+0x10) is used here; the other
// slots are present to pin down the table layout.
struct TssSdk {
    int (*init)(unsigned int);
    int (*init_plugins)(const void *arg0, unsigned int arg1);
    int (*fini)(int);
    int (*proc)();
    int (*notify)(tss_sdk_if::TssSdkPluginNotifyData notify_data);
    int (*get_last_error_code)();
    const char *(*get_last_error)();
};

__attribute__((visibility("hidden"))) extern TssSdk *g_tss_sdk;

// Local reconstruction of the TU-local trace helpers at antibotinterface.cpp
// lines 51-85.  CTssCoreDumpPkgTrace is packed: is_in_func_ at +0 and
// core_pkg_ at +1 (DWARF byte sizes: 13/33/29/17 for the four packages).
#pragma pack(push, 1)
template <typename T>
class CTssCoreDumpPkgTrace {
public:
    CTssCoreDumpPkgTrace(const char *func, const char *type)
        : is_in_func_(0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p",
            func, type, (const void *)this, (const void *)&core_pkg_);
    }

    void enter(const T &pkg) {
        is_in_func_ = 1;
        core_pkg_ = pkg;
    }

    void leave() {
        is_in_func_ = 0;
    }

private:
    char is_in_func_;
    T core_pkg_;
};
#pragma pack(pop)

template <typename T>
class CTssCoreDumpPkgTraceAdapter {
public:
    CTssCoreDumpPkgTraceAdapter(CTssCoreDumpPkgTrace<T> &dump_info,
                                const T &pkg)
        : dump_info_(dump_info) {
        dump_info_.enter(pkg);
    }

    ~CTssCoreDumpPkgTraceAdapter() {
        dump_info_.leave();
    }

private:
    CTssCoreDumpPkgTrace<T> &dump_info_;
};

// Compile-time layout checks (GCC C++98-compatible negative-array trick).
typedef char antibot_pkg_size_check[
    sizeof(TssSdkPluginProtoProcAntibotPkg) == 12 ? 1 : -1];
typedef char down_pkg_size_check[
    sizeof(TssSdkPluginProtoProcDownPkg) == 32 ? 1 : -1];
typedef char dp_pkg_size_check[
    sizeof(TssSdkPluginProtoProcDpPkg) == 28 ? 1 : -1];
typedef char notify_pkg_size_check[
    sizeof(TssSdkPluginProtoProcNotifyPkg) == 16 ? 1 : -1];
typedef char proto_proc_pkg_size_check[
    sizeof(TssSdkPluginProtoProcPkg) == 36 ? 1 : -1];
typedef char notify_data_size_check[
    sizeof(tss_sdk_if::TssSdkPluginNotifyData) == 8 ? 1 : -1];
typedef char antibot_trace_size_check[
    sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntibotPkg>) == 13 ? 1 : -1];
typedef char down_trace_size_check[
    sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcDownPkg>) == 33 ? 1 : -1];
typedef char dp_trace_size_check[
    sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcDpPkg>) == 29 ? 1 : -1];
typedef char notify_trace_size_check[
    sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcNotifyPkg>) == 17 ? 1 : -1];

// Definitions for the class declared in iantibotinterface.h.

// 0x00089770 .. 0x000898f2 (source lines 464-491)
// Guard: buffer != NULL && buffer_size != 0.
// Returns -1 on bad input, otherwise 0 (the notify return is discarded).
int AntibotInterface::OnRecvBufferFromClient(int /*channel_id*/,
                                             int /*room_id*/,
                                             unsigned int uin,
                                             unsigned char *buffer,
                                             unsigned short buffer_size) {
    if ((buffer == 0) || (buffer_size == 0)) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::OnRecvBufferFromClient, invaid param, "
            "uin=%u, buffer=%p, buffer_size=%u",
            uin, buffer, (unsigned int)buffer_size);
        return -1;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::OnRecvBufferFromClient, uin=%u, buffer_size=%u",
        uin, (unsigned int)buffer_size);

    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ANTIBOT_PKG;
    proto_proc_pkg.body_.antibot_pkg_.uin_ = uin;
    proto_proc_pkg.body_.antibot_pkg_.buf_ = buffer;
    proto_proc_pkg.body_.antibot_pkg_.buf_size_ = buffer_size;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    static CTssCoreDumpPkgTrace<TssSdkPluginProtoProcAntibotPkg>
        tmp_core_dump_info(__func__, "TssSdkPluginProtoProcAntibotPkg");
    CTssCoreDumpPkgTraceAdapter<TssSdkPluginProtoProcAntibotPkg> temp_trace(
        tmp_core_dump_info, proto_proc_pkg.body_.antibot_pkg_);

    g_tss_sdk->notify(notify_data);
    return 0;
}

// 0x00089900 .. 0x00089b25 (source lines 505-548)
// Guard: input_buffer, output_buffer, input_buffer_size and
//        output_buffer_size must all be non-zero.
// The plugin owns the result in encrypt_result_ (initialized to -1); the
// notify() return value is not used.  output_buf_size_ is copied back to the
// caller's unsigned int reference after notify.
int AntibotInterface::RecvBufferForEncrypt(
    unsigned int key, unsigned char *input_buffer,
    unsigned int input_buffer_size, unsigned char *output_buffer,
    unsigned int &output_buffer_size) {
    if ((input_buffer == 0) || (output_buffer == 0) ||
        (input_buffer_size == 0) || (output_buffer_size == 0)) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::RecvBufferForEncrypt, invaid param, "
            "key=%u, input_buffer=%p, input_buffer_size=%u, "
            "output_buffer=%p, output_buffer_size=%u",
            key, input_buffer, input_buffer_size, output_buffer,
            output_buffer_size);
        return -1;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::RecvBufferForEncrypt, key=%u, "
        "input_buffer=%p, input_buffer_size=%u, output_buffer=%p, "
        "output_buffer_size=%u",
        key, input_buffer, input_buffer_size, output_buffer,
        output_buffer_size);

    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG_OLD;
    proto_proc_pkg.body_.down_pkg_.cmd_id_ = 0;
    proto_proc_pkg.body_.down_pkg_.uin_ = 0;
    proto_proc_pkg.body_.down_pkg_.idx_ = key;
    proto_proc_pkg.body_.down_pkg_.input_buf_ = input_buffer;
    proto_proc_pkg.body_.down_pkg_.input_buf_size_ = input_buffer_size;
    proto_proc_pkg.body_.down_pkg_.output_buf_ = output_buffer;
    proto_proc_pkg.body_.down_pkg_.output_buf_size_ = output_buffer_size;
    proto_proc_pkg.body_.down_pkg_.encrypt_result_ = -1;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    static CTssCoreDumpPkgTrace<TssSdkPluginProtoProcDownPkg>
        tmp_core_dump_info(__func__, "TssSdkPluginProtoProcDownPkg");
    CTssCoreDumpPkgTraceAdapter<TssSdkPluginProtoProcDownPkg> temp_trace(
        tmp_core_dump_info, proto_proc_pkg.body_.down_pkg_);

    g_tss_sdk->notify(notify_data);

    output_buffer_size = proto_proc_pkg.body_.down_pkg_.output_buf_size_;
    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::RecvBufferForEncrypt, key=%u, "
        "output_buffer_size=%u, encrypt_result=%d",
        key, output_buffer_size,
        proto_proc_pkg.body_.down_pkg_.encrypt_result_);
    return proto_proc_pkg.body_.down_pkg_.encrypt_result_;
}

// 0x00089b30 .. 0x00089d4a (source lines 555-596)
// Guard: input_buffer, output_buffer, input_buffer_size and
//        output_buffer_size must all be non-zero.
// The plugin owns decrypt_result_ (initialized to -1); notify()'s return value
// is not used.  output_buf_size_ is truncated back into the unsigned short
// reference after notify.
int AntibotInterface::RecvBufferForDecrypt(
    unsigned int key, unsigned char *input_buffer,
    unsigned short input_buffer_size, unsigned char *output_buffer,
    unsigned short &output_buffer_size) {
    if ((input_buffer == 0) || (output_buffer == 0) ||
        (input_buffer_size == 0) || (output_buffer_size == 0)) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::RecvBufferForDecrypt, invaid param, "
            "key=%u, input_buffer=%p, input_buffer_size=%u, "
            "output_buffer=%p, output_buffer_size=%u",
            key, input_buffer, (unsigned int)input_buffer_size,
            output_buffer, (unsigned int)output_buffer_size);
        return -1;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::RecvBufferForDecrypt, key=%u, "
        "input_buffer=%p, input_buffer_size=%u, output_buffer=%p, "
        "output_buffer_size=%u",
        key, input_buffer, (unsigned int)input_buffer_size, output_buffer,
        (unsigned int)output_buffer_size);

    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG_OLD;
    proto_proc_pkg.body_.dp_pkg_.uin_ = 0;
    proto_proc_pkg.body_.dp_pkg_.idx_ = key;
    proto_proc_pkg.body_.dp_pkg_.input_buf_ = input_buffer;
    proto_proc_pkg.body_.dp_pkg_.input_buf_size_ = input_buffer_size;
    proto_proc_pkg.body_.dp_pkg_.output_buf_ = output_buffer;
    proto_proc_pkg.body_.dp_pkg_.output_buf_size_ = output_buffer_size;
    proto_proc_pkg.body_.dp_pkg_.decrypt_result_ = -1;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    static CTssCoreDumpPkgTrace<TssSdkPluginProtoProcDpPkg>
        tmp_core_dump_info(__func__, "TssSdkPluginProtoProcDpPkg");
    CTssCoreDumpPkgTraceAdapter<TssSdkPluginProtoProcDpPkg> temp_trace(
        tmp_core_dump_info, proto_proc_pkg.body_.dp_pkg_);

    g_tss_sdk->notify(notify_data);

    output_buffer_size =
        (unsigned short)proto_proc_pkg.body_.dp_pkg_.output_buf_size_;
    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::RecvBufferForDecrypt, key=%u, "
        "output_buffer_size=%u, decrypt_result=%d",
        key, (unsigned int)output_buffer_size,
        proto_proc_pkg.body_.dp_pkg_.decrypt_result_);
    return proto_proc_pkg.body_.dp_pkg_.decrypt_result_;
}

// 0x00089e20 .. 0x00089fcc (source lines 683-715)
// Guard: buffer != NULL && buffer_size > 0 (signed short test, so negative
// values are rejected as well).  The plugin owns notify_result_ (initialized
// to 0); notify()'s return value is not used.
int AntibotInterface::OnNotifyServer(int /*channel_id*/, int /*room_id*/,
                                     unsigned int main_id,
                                     unsigned char *buffer,
                                     short buffer_size) {
    if ((buffer == 0) || (buffer_size <= 0)) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::OnNotifyServer, invalid param, "
            "buffer=%p, buffer_size=%d, main_id=%u.",
            buffer, (int)buffer_size, main_id);
        return -1;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::OnNotifyServer, buffer=%p, buffer_size=%d, "
        "main_id=%u.",
        buffer, (int)buffer_size, main_id);

    TssSdkPluginProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ON_NOTIFY_SERVER;
    proto_proc_pkg.body_.notify_pkg_.main_id_ = main_id;
    proto_proc_pkg.body_.notify_pkg_.buf_ = buffer;
    proto_proc_pkg.body_.notify_pkg_.buf_size_ = buffer_size;
    proto_proc_pkg.body_.notify_pkg_.notify_result_ = 0;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    static CTssCoreDumpPkgTrace<TssSdkPluginProtoProcNotifyPkg>
        tmp_core_dump_info(__func__, "TssSdkPluginProtoProcNotifyPkg");
    CTssCoreDumpPkgTraceAdapter<TssSdkPluginProtoProcNotifyPkg> temp_trace(
        tmp_core_dump_info, proto_proc_pkg.body_.notify_pkg_);

    g_tss_sdk->notify(notify_data);

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::OnNotifyServer, notify_result=%d.",
        proto_proc_pkg.body_.notify_pkg_.notify_result_);
    return proto_proc_pkg.body_.notify_pkg_.notify_result_;
}
