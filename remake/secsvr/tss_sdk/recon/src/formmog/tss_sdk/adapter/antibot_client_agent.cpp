// Reconstructed from libtss_sdk.so.1.0.394
// (SHA-256 8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7).
//
// Partial translation unit for src/formmog/tss_sdk/adapter/antibotinterface.cpp:
//   AntibotInterface::Initialize(IBusiness*, char*)          0x000889b0..0x00088b2a
//   AntibotInterface::AddClient(int,int,unsigned,unsigned,
//                               TUserInfo*)                  0x00089450..0x00089638
//   AntibotInterface::DeleteClient(int,int,unsigned)         0x00089640..0x0008976b
//
// The complete base/derived hierarchy is declared in iantibotinterface.h and
// shared with the other adapter TUs, the factory and the closing vtable
// owner.  The three method definitions below emit exactly the original
// mangled symbols:
//
// IBusiness is intentionally opaque: none of these three methods performs an
// IBusiness vtable call. g_business/g_channel_id/g_tss_sdk are TU-local (b)
// globals in the original object; they are declared extern here so that the
// integration owner provides exactly one definition shared by all restored
// methods of the original translation unit.
//
// ABI-faithful reconstruction; not claimed to be byte-identical.

#include <stdlib.h>

// Complete base/derived class declarations shared by every adapter, factory
// and closing TU.  IBusiness stays opaque; the header forward-declares it.
#include "iantibotinterface.h"

// Canonical tag-394 plugin packets: TssSdkPluginUserControlHead (4),
// ...AddUser (28), ...DeleteUser (8), the 32-byte TssSdkPluginUserControlBody
// union and ...Pkg (36), plus the ORIG header-inline CTssCoreDumpPkgTrace<>
// wrapper.  The local copies this TU used to carry are gone.
#include "src/formmog/tss_sdk/layout/tss_sdk_pkgs.h"

// sizeof = 76, layout per DWARF: szPlayerName[0..63], iUserID@64,
// uiSvrIp@68, uiCltIp@72.
struct TUserInfo {
    char szPlayerName[64];
    int iUserID;
    unsigned int uiSvrIp;
    unsigned int uiCltIp;
};

// ZEN_Trace_LogMsg (zen_trace_log_msg.h) is the single canonical static-only
// logging boundary pulled in by layout/tss_sdk_pkgs.h; the local 3-entry model
// of this TU is gone.

// tss_sdk_if::ETssSdkPluginId
enum ETssSdkPluginId {
    ETSS_SDK_PLUGIN_ID_CHANNEL = 1,
    ETSS_SDK_PLUGIN_ID_USER_CONTROL = 2,
    ETSS_SDK_PLUGIN_ID_PROTO_PROC = 3,
    ETSS_SDK_PLUGIN_ID_SAFEMODE = 4,
    ETSS_SDK_PLUGIN_ID_CAPTCHA = 5
};

// tss_sdk_if::TssSdkPluginCallbackProc
typedef int (*TssSdkPluginCallbackProc)(void *arg);

// tss_sdk_if::TssSdkPluginCallback, sizeof = 12.
struct TssSdkPluginCallback {
    ETssSdkPluginId plugin_id_;
    TssSdkPluginCallbackProc callback_proc_;
    void *init_data_;
};

// tss_sdk_if::TssSdkPluginNotifyData, sizeof = 8.
struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;
    void *notify_data_;
};

// tss_sdk_if init table, sizeof = 28, addresses are vtable-like data slots:
// init@0, init_plugins@4, fini@8, proc@12, notify@16, get_last_error_code@20,
// get_last_error@24. Only the slots used by the three methods are modelled.
struct TssSdkApi {
    int (*init)(unsigned int channel_id);
    int (*init_plugins)(const TssSdkPluginCallback *plugin_array,
                        unsigned int plugin_count);
    int (*fini)(int plugin_id);
    int (*proc)();
    int (*notify)(TssSdkPluginNotifyData notify_data);
    int (*get_last_error_code)();
    const char *(*get_last_error)();
};

// Init data handed to the channel plugin (DWARF: 12 bytes).
struct TssSdkInitChannelData {
    unsigned int unique_instance_id_;
    unsigned int ipc_bus_size_;
    int pid_channel_mode_;
};

// tss_sdk_if::TssSdkPluginUserControlHead / AddUser / DeleteUser, the
// 32-byte TssSdkPluginUserControlBody union and TssSdkPluginUserControlPkg are
// the canonical declarations of layout/tss_sdk_pkgs.h; the local add/delete +
// pad_ union that the audit reported as a second TssSdkPluginUserControlBody
// layout is gone.

// ---------------------------------------------------------------------------
// Original file-scope globals (all local 'b' symbols in the shipped object).
//
// The reconstruction split ORIG's single antibotinterface.cpp across
// antibot_client_agent.cpp / antibot_callbacks_agent.cpp /
// antibot_packets_agent.cpp / antibot_policy_agent.cpp.  The three objects are
// therefore defined once here with hidden visibility: one real instance is
// shared by the split halves of the original TU and none of the symbols is
// exported from the .so (ORIG has them local).
// ---------------------------------------------------------------------------
extern "C" void *tss_get_sdk();

// 0x0029d064, pointer returned by the imported tss_get_sdk() during the
// adapter's static initialization (0x000890bb).
__attribute__((visibility("hidden"))) TssSdkApi *g_tss_sdk =
    (TssSdkApi *)tss_get_sdk();
// 0x0029d1a0; set by Initialize, read by the other adapter callbacks.
__attribute__((visibility("hidden"))) IBusiness *g_business = 0;
// 0x0029d1a4.
__attribute__((visibility("hidden"))) unsigned int g_channel_id = 0;

// ORIG antibotinterface.cpp static-initialization block: independent
// COMM_STAT_ITEM_WITH_NAME[16] + __ioinit + status dtor registration.  The
// module object g_antibot_if (AntibotInterface) is the single shared instance
// constructed by tss_sdk_exports_timer_agent.cpp; do not construct a second
// one here.  g_tss_sdk above supplies the block's tss_get_sdk() business call.
#define TSS_SDK_STATIC_INIT_ID antibotinterface
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

// Callbacks registered with the framework (defined in the same original TU).
extern int send_buffer_to_client_cb_(void *sec_data);   // 0x00089120
extern int punish_user_cb_(void *sec_data);             // 0x00089260
extern int process_safemode_cb_(void *sec_data);        // 0x000888f0

// CTssCoreDumpPkgTrace<> (sizeof = 1 + sizeof(PKG), core_pkg_ at +1) with its
// ORIG header-inline ctor/enter()/leave() comes from
// layout/tss_sdk_pkgs.h; the local template of this TU is gone.

// 0x000889b0..0x00088b2a
int AntibotInterface::Initialize(IBusiness *busi, char *ipc_path_name) {
    ZEN_Trace_LogMsg::debug_infoex(
        "In AntibotInterface::Initialize. busi=%p, ipc_path_name=%s",
        (void *)busi, ipc_path_name);

    if (busi == 0 || ipc_path_name == 0) {
        return -1;
    }

    unsigned int ipc_path_name_num =
        (unsigned int)strtol(ipc_path_name, 0, 10);

    g_business = busi;

    int rc = g_tss_sdk->init(ipc_path_name_num);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::Initialize. rc=%d", rc);
        return rc;
    }

    g_channel_id = ipc_path_name_num;

    TssSdkInitChannelData init_channel_data;
    init_channel_data.unique_instance_id_ = ipc_path_name_num;
    init_channel_data.ipc_bus_size_ = 0x800000;
    init_channel_data.pid_channel_mode_ = 0;

    TssSdkPluginCallback plugin_array[5];
    plugin_array[0].plugin_id_ = ETSS_SDK_PLUGIN_ID_CHANNEL;
    plugin_array[0].callback_proc_ = send_buffer_to_client_cb_;
    plugin_array[0].init_data_ = &init_channel_data.unique_instance_id_;
    plugin_array[1].plugin_id_ = ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    plugin_array[1].callback_proc_ = 0;
    plugin_array[1].init_data_ = 0;
    plugin_array[2].plugin_id_ = ETSS_SDK_PLUGIN_ID_USER_CONTROL;
    plugin_array[2].callback_proc_ = punish_user_cb_;
    plugin_array[2].init_data_ = 0;
    plugin_array[3].plugin_id_ = ETSS_SDK_PLUGIN_ID_SAFEMODE;
    plugin_array[3].callback_proc_ = process_safemode_cb_;
    plugin_array[3].init_data_ = 0;
    plugin_array[4].plugin_id_ = ETSS_SDK_PLUGIN_ID_CAPTCHA;
    plugin_array[4].callback_proc_ = 0;
    plugin_array[4].init_data_ = 0;

    rc = g_tss_sdk->init_plugins(plugin_array, 5);

    ZEN_Trace_LogMsg::debug_infoex(
        "In AntibotInterface::Initialize, rc=%d, g_channel_id=%d, "
        "unique_instance_id=%d",
        rc, (int)g_channel_id, (int)init_channel_data.unique_instance_id_);

    g_channel_id = init_channel_data.unique_instance_id_;
    return rc;
}

// 0x00089450..0x00089638
int AntibotInterface::AddClient(int channel_id, int room_id, unsigned int uin,
                                unsigned int cli_ver, TUserInfo *user_info) {
    // channel_id/room_id are not consumed by the shipped body.
    (void)channel_id;
    (void)room_id;

    TssSdkPluginUserControlPkg user_control_pkg;
    user_control_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER;
    user_control_pkg.body_.add_user_.idx_ = 0;
    user_control_pkg.body_.add_user_.uin_ = uin;
    user_control_pkg.body_.add_user_.client_ver_ = cli_ver;
    user_control_pkg.body_.add_user_.server_ip_ = 0;
    user_control_pkg.body_.add_user_.client_ip_ = 0;

    if (user_info != 0) {
        user_control_pkg.body_.add_user_.user_id_ =
            (unsigned int)user_info->iUserID;
        user_control_pkg.body_.add_user_.server_ip_ = user_info->uiSvrIp;
        user_control_pkg.body_.add_user_.client_ip_ = user_info->uiCltIp;
        user_control_pkg.body_.add_user_.player_name_ =
            user_info->szPlayerName;
    } else {
        user_control_pkg.body_.add_user_.user_id_ = 0xffffffffu;
        user_control_pkg.body_.add_user_.player_name_ = 0;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::AddClient,uin=%u, cli_ver=%u, user_id=%u, "
        "server_ip=%u, client_ip=%u, player_name=%s",
        uin, cli_ver, user_control_pkg.body_.add_user_.user_id_,
        user_control_pkg.body_.add_user_.server_ip_,
        user_control_pkg.body_.add_user_.client_ip_,
        user_control_pkg.body_.add_user_.player_name_);

    // Guarded static at _ZZN16AntibotInterface9AddClientEiijjP9TUserInfoE18
    // tmp_core_dump_info (0x0029d180, 29 bytes); guard at 0x0029d178.
    static CTssCoreDumpPkgTrace<TssSdkPluginUserControlAddUser>
        tmp_core_dump_info("AddClient", "TssSdkPluginUserControlAddUser");

    tmp_core_dump_info.enter(user_control_pkg.body_.add_user_);

    TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = ETSS_SDK_PLUGIN_ID_USER_CONTROL;
    notify_data.notify_data_ = &user_control_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    int idx = (int)user_control_pkg.body_.add_user_.idx_;

    if (ret == 0 && idx > 0) {
        ZEN_Trace_LogMsg::debug_debugEx(
            "In AntibotInterface::AddClient,uin=%u, idx=%d", uin, idx);
    } else {
        ZEN_Trace_LogMsg::debug_errorex(
            "In AntibotInterface::AddClient fail, uin=%u, ret=%d, idx=%d",
            uin, ret, idx);
    }

    tmp_core_dump_info.leave();
    return idx;
}

// 0x00089640..0x0008976b
int AntibotInterface::DeleteClient(int channel_id, int room_id,
                                   unsigned int uin) {
    // channel_id/room_id are not consumed by the shipped body.
    (void)channel_id;
    (void)room_id;

    TssSdkPluginUserControlPkg user_control_pkg;
    user_control_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER;
    user_control_pkg.body_.delete_user_.uin_ = uin;
    // NOTE: the shipped code never writes body_.delete_user_.idx_; the field
    // is copied into the trace and passed to the plugin uninitialized. It is
    // deliberately left untouched here rather than inventing a value.

    ZEN_Trace_LogMsg::debug_debugEx(
        "In AntibotInterface::DeleteClient, uin=%u", uin);

    // Guarded static at _ZZN16AntibotInterface12DeleteClientEiijE18
    // tmp_core_dump_info (0x0029d168, 9 bytes); guard at 0x0029d160.
    static CTssCoreDumpPkgTrace<TssSdkPluginUserControlDeleteUser>
        tmp_core_dump_info("DeleteClient", "TssSdkPluginUserControlDeleteUser");

    tmp_core_dump_info.enter(user_control_pkg.body_.delete_user_);

    TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = ETSS_SDK_PLUGIN_ID_USER_CONTROL;
    notify_data.notify_data_ = &user_control_pkg;

    g_tss_sdk->notify(notify_data);

    tmp_core_dump_info.leave();
    return 0;
}
