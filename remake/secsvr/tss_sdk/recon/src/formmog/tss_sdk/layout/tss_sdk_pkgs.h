// Canonical layouts of the TssSdkPlugin* wire packets and the core-dump trace
// wrapper.
//
// ORIG evidence: audit/tu_report.py --type 'CTssCoreDumpPkgTrace<...>' gives
// both the trace wrapper and, in the flattened leaf list, the exact member
// names/offsets/sizes of each packet:
//   TssSdkPluginProtoProcDpPkg              28  decrypt_result_ i32 @0,
//       idx_ u32 @4, uin_ u32 @8, input_buf_ u8* @12, input_buf_size_ u32 @16,
//       output_buf_ u8* @20, output_buf_size_ u32 @24
//   TssSdkPluginProtoProcDownPkg            32  encrypt_result_ i32 @0, ...
//   TssSdkPluginProtoProcAntiCheatingPkg    24  is_cheating_pkg_ i32 @0,
//       cmd_id_ i32 @4, uin_ u32 @8, idx_ u32 @12, game_pkg_ u8* @16,
//       game_pkg_len_ u32 @20
//   TssSdkPluginProtoProcAntibotPkg         12  uin_ u32 @0, buf_ u8* @4,
//       buf_size_ u32 @8
//   TssSdkPluginProtoProcNotifyPkg          16  notify_result_ i32 @0,
//       main_id_ u32 @4, buf_ u8* @8, buf_size_ u32 @12
//   TssSdkPluginUserControlAddUser          28  idx_ u32 @0, uin_ u32 @4,
//       user_id_ u32 @8, client_ver_ u32 @12, server_ip_ u32 @16,
//       client_ip_ u32 @20, player_name_ char* @24
//   TssSdkPluginUserControlDeleteUser        8  idx_ u32 @0, uin_ u32 @4
//   tss_sdk_if::TssSdkPluginNotifyData      12
//   tagAntibotPunishInfo                    28
//   CTssCoreDumpPkgTrace<PKG> = char + PKG  (e.g. 29 = 1 + 28)
// and the channel/user-control envelope layouts are the ones the reconstructed
// tss_sdk_comm_svc_agent.cpp already uses (TssSdkPluginChannelHead 4,
// TssSdkPluginChannelIpcChannelPkg 8, ...SocketChannelPkg 10 packed,
// ...GameChannelPkg 14 packed, ...ChannelPkg 18 packed,
// TssSdkPluginUserControlHead 4, TssSdkPluginUserControlPunishUser 32,
// ...GetUserInfo 12, ...Body 32, ...Pkg 36).
//
// TODO(migrate): tss_sdk_comm_svc_agent.cpp, proto_proc_plugin_agent.cpp,
// usercontrol_plugin_agent.cpp, antibot_packets_agent.cpp and
// antibot_client_agent.cpp still declare their own packet models (the audit
// reports TssSdkPluginProtoProcPkg / TssSdkPluginUserControlPkg / ...Body ODR
// splits between them).
#ifndef TSS_SDK_LAYOUT_TSS_SDK_PKGS_H_
#define TSS_SDK_LAYOUT_TSS_SDK_PKGS_H_

#include "src/formmog/tss_sdk/layout/layout_contract.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"

namespace tss_sdk_if {

struct TssSdkPluginNotifyData {
    unsigned int main_id_;      // +0
    unsigned int sub_id_;       // +4
    unsigned int data_;         // +8
};

}  // namespace tss_sdk_if

// sdk_plugin_proto_proc.h packets.
struct TssSdkPluginProtoProcDpPkg {
    int decrypt_result_;             // +0
    unsigned int idx_;               // +4
    unsigned int uin_;               // +8
    unsigned char *input_buf_;       // +12
    unsigned int input_buf_size_;    // +16
    unsigned char *output_buf_;      // +20
    unsigned int output_buf_size_;   // +24
};

struct TssSdkPluginProtoProcDownPkg {
    int encrypt_result_;             // +0
    int cmd_id_;                     // +4
    unsigned int uin_;               // +8
    unsigned int idx_;               // +12
    unsigned char *input_buf_;       // +16
    unsigned int input_buf_size_;    // +20
    unsigned char *output_buf_;      // +24
    unsigned int output_buf_size_;   // +28
};

struct TssSdkPluginProtoProcAntiCheatingPkg {
    int is_cheating_pkg_;            // +0
    int cmd_id_;                     // +4
    unsigned int uin_;               // +8
    unsigned int idx_;               // +12
    unsigned char *game_pkg_;        // +16
    unsigned int game_pkg_len_;      // +20
};

struct TssSdkPluginProtoProcAntibotPkg {
    unsigned int uin_;               // +0
    unsigned char *buf_;             // +4
    unsigned int buf_size_;          // +8
};

struct TssSdkPluginProtoProcNotifyPkg {
    int notify_result_;              // +0
    unsigned int main_id_;           // +4
    unsigned char *buf_;             // +8
    unsigned int buf_size_;          // +12
};

// sdk_plugin_proto_proc.h wrappers (packed: head 4 + body 32 = 36).
struct TssSdkPluginProtoProcDecryptListPkg {   // 16 bytes
    int check_result_;               // +0
    int cmd_id_;                     // +4
    unsigned int uin_;               // +8
    unsigned int idx_;               // +12
};

struct TssSdkPluginProtoProcDownListPkg {      // 16 bytes
    int cmd_id_;                     // +0
    unsigned int uin_;               // +4
    unsigned int idx_;               // +8
    int is_in_down_list_;            // +12
};

union __attribute__((packed)) TssSdkPluginProtoProcBody {   // 32 bytes
    TssSdkPluginProtoProcDecryptListPkg decrypt_list_pkg_;
    TssSdkPluginProtoProcDownPkg down_pkg_;
    TssSdkPluginProtoProcNotifyPkg notify_pkg_;
    TssSdkPluginProtoProcAntiCheatingPkg anti_cheating_pkg_;
    TssSdkPluginProtoProcAntibotPkg antibot_pkg_;
    TssSdkPluginProtoProcDpPkg dp_pkg_;
    TssSdkPluginProtoProcDownListPkg down_list_pkg_;
};

struct TssSdkPluginProtoProcPkgHead {          // 4 bytes
    int msg_id_;                     // +0
};

struct __attribute__((packed)) TssSdkPluginProtoProcPkg {   // 36 bytes
    TssSdkPluginProtoProcPkgHead head_;   // +0
    TssSdkPluginProtoProcBody body_;      // +4
};

// sdk_plugin_user_control.h packets.
struct TssSdkPluginUserControlAddUser {
    unsigned int idx_;               // +0
    unsigned int uin_;               // +4
    unsigned int user_id_;           // +8
    unsigned int client_ver_;        // +12
    unsigned int server_ip_;         // +16
    unsigned int client_ip_;         // +20
    const char *player_name_;        // +24
};

struct TssSdkPluginUserControlDeleteUser {
    unsigned int idx_;               // +0
    unsigned int uin_;               // +4
};

// sdk_plugin_channel.h packets (envelope sizeof 18, fully packed).
enum ETssSdkPluginChannelTypeId {
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_IPC_CHANNEL = 1,
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_SOCKET_CHANNEL = 2,
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_GAME_CHANNEL = 3
};

struct TssSdkPluginChannelHead {             // 4 bytes
    ETssSdkPluginChannelTypeId type_id_;     // +0
};

struct TssSdkPluginChannelIpcChannelPkg {    // 8 bytes
    const char *buf_;                        // +0
    unsigned int buf_size_;                  // +4
};

struct __attribute__((packed)) TssSdkPluginChannelSocketChannelPkg {  // 10
    int business_id_;                        // +0
    const char *buf_;                        // +4
    short buf_size_;                         // +8
};

struct __attribute__((packed)) TssSdkPluginChannelGameChannelPkg {    // 14
    int busi_type_;                          // +0
    unsigned int uin_;                       // +4
    unsigned char *buf_;                     // +8
    short buf_size_;                         // +12
};

union __attribute__((packed)) TssSdkPluginChannelBody {               // 14
    TssSdkPluginChannelIpcChannelPkg ipc_channel_pkg_;
    TssSdkPluginChannelSocketChannelPkg socket_channel_pkg_;
    TssSdkPluginChannelGameChannelPkg game_channel_pkg_;
};

struct __attribute__((packed)) TssSdkPluginChannelPkg {               // 18
    TssSdkPluginChannelHead head_;
    TssSdkPluginChannelBody body_;
};

// sdk_plugin_user_control.h wrappers (head 4 + body 32 = 36).
enum ETssSdkPluginUserControlMsgId {
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER = 1,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER = 2,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER = 3,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO = 4,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN = 5
};

struct TssSdkPluginUserControlHead {           // 4 bytes
    ETssSdkPluginUserControlMsgId msg_id_;       // +0
};

struct TssSdkPluginUserControlPunishUser {     // 32 bytes
    unsigned int uin_;               // +0
    unsigned short game_id_;         // +4
    unsigned short room_id_;         // +6
    int punish_mode_;                // +8
    int punish_time_;                // +12
    unsigned int punish_info_len_;   // +16
    const unsigned char *punish_info_;    // +20
    unsigned int punish_reason_len_;      // +24
    const unsigned char *punish_reason_;  // +28
};

struct TssSdkPluginUserControlGetUserInfo {    // 12 bytes
    unsigned int uin_;               // +0
    unsigned int idx_;               // +4
    void *user_info_;                // +8
};

union TssSdkPluginUserControlBody {            // 32 bytes
    TssSdkPluginUserControlAddUser add_user_;
    TssSdkPluginUserControlDeleteUser delete_user_;
    TssSdkPluginUserControlPunishUser punish_user_;
    TssSdkPluginUserControlGetUserInfo user_info_;
};

struct TssSdkPluginUserControlPkg {            // 36 bytes
    TssSdkPluginUserControlHead head_;   // +0
    TssSdkPluginUserControlBody body_;   // +4
};

// sec_proto / antibotinterface.h wire type (CU antibotinterface.cpp):
//   iGameID i32 @0, iBanTimeSecond i32 @4, iVerifyTimeout i32 @8,
//   iVerifyMaxErrorCount i32 @12, iVerifyLevel i32 @16,
//   punish_info_len u32 @20, punish_info const u8* @24  -> 28 bytes.
struct tagAntibotPunishInfo {
    int iGameID;                       // +0
    int iBanTimeSecond;                // +4
    int iVerifyTimeout;                // +8
    int iVerifyMaxErrorCount;          // +12
    int iVerifyLevel;                  // +16
    unsigned int punish_info_len;      // +20
    const unsigned char *punish_info;  // +24
};
// The trace wrapper is packed in ORIG (29 = 1 + sizeof(DpPkg)), so it carries
// no tail padding.  Its ctor/enter()/leave() are header-inline in ORIG (no
// symbol exists for any instantiation): the ctor clears the flag and logs
// through the caller's function-local static guard, enter() sets the flag and
// copies the packet, leave() clears it.  The bodies are the ones the shipped
// inlined code executes (established by the adapter TUs' reconstructions).
template <class PKG>
struct __attribute__((packed)) CTssCoreDumpPkgTrace {
    class CTssCoreDumpPkgTraceAdapter {
    public:
        PKG &dump_info_;
    };

    char is_in_func_;   // +0
    PKG core_pkg_;      // +1

    __attribute__((always_inline))
    CTssCoreDumpPkgTrace(const char *func, const char *type) : is_in_func_(0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p",
            func, type, (void *)this, (void *)&core_pkg_);
    }

    __attribute__((always_inline))
    void enter(const PKG &pkg) {
        is_in_func_ = 1;
        core_pkg_ = pkg;
    }

    __attribute__((always_inline))
    void leave() {
        is_in_func_ = 0;
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginProtoProcDpPkg) == 28,
                      dp_pkg_size_must_be_28);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginProtoProcDownPkg) == 32,
                      down_pkg_size_must_be_32);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginProtoProcAntiCheatingPkg) == 24,
                      anticheating_pkg_size_must_be_24);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginProtoProcAntibotPkg) == 12,
                      antibot_pkg_size_must_be_12);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginProtoProcNotifyPkg) == 16,
                      notify_pkg_size_must_be_16);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginUserControlAddUser) == 28,
                      add_user_size_must_be_28);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkPluginUserControlDeleteUser) == 8,
                      delete_user_size_must_be_8);
TSS_SDK_STATIC_ASSERT(sizeof(tss_sdk_if::TssSdkPluginNotifyData) == 12,
                      notify_data_size_must_be_12);
TSS_SDK_STATIC_ASSERT(sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcDpPkg>) == 29,
                      coredump_trace_dp_pkg_size_must_be_29);
TSS_SDK_STATIC_ASSERT(sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcDownPkg>) == 33,
                      coredump_trace_down_pkg_size_must_be_33);
TSS_SDK_STATIC_ASSERT(sizeof(CTssCoreDumpPkgTrace<TssSdkPluginProtoProcNotifyPkg>) == 17,
                      coredump_trace_notify_pkg_size_must_be_17);

#endif  // TSS_SDK_LAYOUT_TSS_SDK_PKGS_H_
