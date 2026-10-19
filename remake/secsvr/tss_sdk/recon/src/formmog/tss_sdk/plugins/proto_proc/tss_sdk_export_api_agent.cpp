// Semantic reconstruction from libtss_sdk.so.1.0.394
// (ELF32 i386, GCC 4.1.x, -O2 -fPIC, SHA-256
//  8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7).
//
// Translation units covered (all in src/formmog/tss_sdk/busi_interf/):
//   anti/tss_sdk_anti_impl.cpp
//     send_anti_data_to_client    0x0008a970-0x0008aae4
//     tss_sdk_add_user            0x0008a270-0x0008a395
//     tss_sdk_del_user            0x0008a1e0-0x0008a26f
//     tss_sdk_is_cheat_pkg        0x0008a560-0x0008a60f
//     tss_sdk_decrypt_pkg         0x0008a3a0-0x0008a55f
//     tss_sdk_encrypt_pkg         0x0008a610-0x0008a7a8
//     punish_user_callback        0x0008a7b0-0x0008a838  (local symbol)
//     tss_sdk_anti_init           0x0008a840-0x0008a8f8
//   captcha/tss_sdk_captcha_impl.cpp
//     captcha_callback            0x0008b570-0x0008b64d  (local symbol)
//     tss_sdk_captcha_init        0x0008b650-0x0008b6b4
//     tss_sdk_captcha_start       0x0008b950-0x0008ba7e
//     tss_sdk_captcha_change_pic  0x0008b7b0-0x0008b82f
//     tss_sdk_captcha_verify      0x0008b830-0x0008b947
//   safemode/tss_sdk_safemode_impl.cpp
//     tss_sdk_safemode_init              0x0008bfd0-0x0008c0cc
//     tss_sdk_secoptverify_append_param  0x0008bf30-0x0008bfc7
//     tss_sdk_secoptverify_user_operate  0x0008c120-0x0008c20e
//     safemode_plugin_callback_proc      0x0008c210-0x0008c37c
//   uic/tss_sdk_uic_impl.cpp
//     plugin_callback_proc             0x0008c790-0x0008c7e8
//     tss_sdk_uic_init                 0x0008c7f0-0x0008c86e
//     tss_sdk_uic_judge_input_chat     0x0008ca40-0x0008cb06
//     tss_sdk_uic_judge_input_name     0x0008c960-0x0008ca30
//
// Each original TU owns a file-local `static TssSdkInit *g_tss_sdk` that the
// TU static initializer sets with tss_get_sdk() (e.g. captcha TU initializer
// 0x0008bdc3/0x0008bdc8).  The four blocks below share one such pointer; the
// object returned by tss_get_sdk() is the read-only 28-byte table at
// 0x00297944.
//
// The plugin callbacks stored in the init tables are implemented here too;
// their addresses and (ORIG) symbol bindings are:
//   tss_sdk_impl::punish_user_callback(void*)          0x0008a7b0  local
//   tss_sdk_impl::captcha_callback(void*)              0x0008b570  local
//   tss_sdk_impl::safemode_plugin_callback_proc(void*) 0x0008c210  global
//   tss_sdk_impl::plugin_callback_proc(void*)          0x0008c790  global
//   tss_sdk_impl::send_anti_data_to_client(void*)      0x0008a970  global
//
// DWARF layouts used here (exact, all tss_sdk_if/package structures are
// byte-packed; sizes and offsets are from .debug_info):
//   TssSdkPluginInitData     12: +0 plugin_id_, +4 callback_proc_, +8 init_data_
//   TssSdkPluginNotifyData    8: +0 plugin_id_, +4 notify_data_
//   TssSdkAntiAddUserInfo    16: +0 uin_, +4 client_ver_, +8 client_ip_,
//                                +c role_name_ (char const*)
//   TssSdkAntiDelUserInfo     4: +0 uin_
//   TssSdkAntiInitInfo        8: +0 send_data_to_client_, +4 punish_user_
//   TssSdkAntiDecryptPkgInfo 20: +0 uin_, +4 anti_data_,
//                                +8 anti_data_len_, +c game_pkg_buf_,
//                                +10 game_pkg_buf_len_
//   TssSdkAntiEncryptPkgInfo 24: +0 uin_, +4 cmd_id_, +8 game_pkg_,
//                                +c game_pkg_len_, +10 anti_data_,
//                                +14 anti_data_len_
//   TssSdkAntiGamePkgInfo    16: +0 uin_, +4 cmd_id_, +8 game_pkg_,
//                                +c game_pkg_len_
//   TssSdkCaptchaInitInfo     8: +0 on_recv_pic_data_, +4 on_verify_rsp_
//   TssSdkCaptchaStartInfo   27: +0 uin_, +4 req_seq_, +8 client_ip_,
//                                +c max_pic_change_num_(u16),
//                                +e max_verify_num_(u16), +10 timeout_,
//                                +14 captcha_type_(u8), +15 busi_info_len_(u16),
//                                +17 busi_info_ (u8 const*)
//   TssSdkCaptchaChangePicInfo 4: +0 captcha_seq_
//   TssSdkCaptchaVerifyReqInfo 8: +0 captcha_seq_, +4 answer_ (char const*)
//   TssSdkCaptchaPicInfo     37: +0 uin_, +4 req_seq_, +8 captcha_seq_,
//                                +c timeout_, +10 verify_times_limit_(u16),
//                                +12 verify_times_(u16),
//                                +14 change_times_limit_(u16),
//                                +16 change_times_(u16), +18 picture_len_(u16),
//                                +1a picture_, +1e captcha_type_(u8),
//                                +1f busi_info_len_(u16), +21 busi_info_
//   TssSdkCaptchaVerifyRspInfo 16: +0 req_seq_, +4 result_(i32),
//                                +8 err_msg_len_, +c err_msg_
//   TssSdkSafemodeInitData   16: +0 set_limit_info_, +4 set_safe_status_,
//                                +8 set_verify_info_, +c set_verify_result_
//   TssSdkSecOptVerifyAppendParamInfo 1032: +0 url_[1024], +400 param_count_,
//                                +404 param_
//   TssSdkSecOptVerifyReqInfo 82: +0 uin_, +4 seq_, +8 op_level_,
//                                +c op_scene_len_(i16), +e op_scene_[64],
//                                +4e client_ip_
//   TssSdkUicInitInfo         4: +0 on_chat_judge_result_
//   TssSdkUicNameUserInputInfo 13: +0 msg_len_, +4 msg_,
//                                +8 if_replace_(char), +9 msg_result_flag_(i32)
//   TssSdkUicChatUserInputInfo 24: +0 uin_, +4 msg_len_, +8 msg_,
//                                +c callback_data_len_(u16), +e callback_data_,
//                                +12 game_data_len_(u16), +14 game_data_
//   SafemodePkg              90: head 6 + body union 84 (plugin -> SDK)
//   CaptchaPkg             4641: head 16 + body union 4625 (plugin -> SDK)
//   UserControlPunishUser    32: +0 uin_, +4 game_id_(u16), +6 room_id_(u16),
//                                +8 punish_mode_(i32), +c punish_time_(i32),
//                                +10 punish_info_len_(u32), +14 punish_info_,
//                                +18 punish_reason_len_(u32), +1c punish_reason_
//   UicChatJudgeResult         28: naturally aligned, +0 uin_, +4 msg_result_flag_,
//                                +8 message_len_, +c message_, +10 callback_data_len_(u16),
//                                +14 callback_data_, +18 input_type_
//   TssSdkPluginChannelGameChannelPkg 14: +0 busi_type_, +4 uin_, +8 buf_,
//                                +c buf_size_(i16)
//   sec_proto::DPInterDataHead 5: +0 packet_size_(u16),
//                                +2 protocol_type_(u16), +4 data_type_(u8)
//
// Message ids (DWARF enumerators where present):
//   proto_proc: 201 is_cheating, 202 encrypt, 203 decrypt,
//               304 is_cmd_in_up_list, 305 is_cmd_in_down_list
//   user_control: 1 add, 2 delete, 3 punish, 4 get_userinfo,
//                 5 get_userinfo_by_uin
//   captcha head msg ids: 0x0c81 start, 0x0c83 verify, 0x0c86 change_pic;
//                         plugin replies 0x0c82 pic data, 0x0c84 verify rsp
//   safemode head msg types: 0x16 append_param, 0x18 user_operate/launch_verify
//                         plugin replies 0x15 limit_status, 0x17 safe_status,
//                         0x19 verify_info, 0x1a verify_status
//
// No external symbol is synthesised: every call below is a direct call to the
// shipped symbol of the same name/offset.

#include <string.h>

#include <arpa/inet.h>

// ---------------------------------------------------------------------------
// External runtime boundary.
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// tss_sdk_plugin_channel.h: the channel plugin hands the game-channel buffer
// to the callback registered by the SDK (DWARF ._494, byte-packed, sizeof 14).
// send_anti_data_to_client() is that callback.
// ---------------------------------------------------------------------------

struct TssSdkPluginChannelGameChannelPkg {
    int busi_type_;            // +0  0 antibot packet, 1 DP packet
    unsigned int uin_;         // +4
    unsigned char *buf_;       // +8
    short buf_size_;           // +c
} __attribute__((packed));

// ---------------------------------------------------------------------------
// sec_proto wire head used to re-frame the DP packet in
// send_anti_data_to_client() (DWARF sizeof 5; same shape as the unit that
// owns DPInterDataHead::pack).
// ---------------------------------------------------------------------------

namespace sec_proto {

struct DPInterDataHead {
    unsigned short packet_size_;    // +0
    unsigned short protocol_type_;  // +2
    unsigned char data_type_;       // +4

    int pack(char *buf, unsigned int cut_size, unsigned int *used,
             unsigned int protocol_version) const;
} __attribute__((packed));

}  // namespace sec_proto

#pragma pack(push, 1)

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

enum ETssSdkProcResult {
    TSS_SDK_PROC_OK = 0,
    TSS_SDK_PROC_INVALID_ARG = 1,
    TSS_SDK_PROC_INTERNAL_ERR = 2,
    TSS_SDK_PROC_FAIL = 3
};

typedef int (*TssSdkPluginCallbackProc)(void *arg);

struct TssSdkPluginInitData {
    ETssSdkPluginId plugin_id_;               // +0
    TssSdkPluginCallbackProc callback_proc_;  // +4
    void *init_data_;                         // +8
};

struct TssSdkPluginNotifyData {
    ETssSdkPluginId plugin_id_;  // +0
    void *notify_data_;          // +4
};

}  // namespace tss_sdk_if

// Forward declarations for callback parameter types.
struct TssSdkAntiDataInfo;
struct TssSdkPunishUserInfo;
struct TssSdkSecOptLimitInfo;
struct TssSdkSecOptSafeStatus;
struct TssSdkSecOptVerifyInfo;
struct TssSdkSecOptVerifyResult;
struct TssSdkUicJudgeResult;
struct TssSdkCaptchaPicInfo;
struct TssSdkCaptchaVerifyRspInfo;

// tss_sdk_anti_interface.h callback typedefs (file 439 lines 156/160).
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkSendDataToClient)(
    const TssSdkAntiDataInfo *data);
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkPunishUser)(
    const TssSdkPunishUserInfo *info);

// tss_sdk_safemode_interface.h callback typedefs (lines 84/114/127/150).
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkSecOptVerifyAppendParam)(
    const struct TssSdkSecOptVerifyAppendParamInfo *info);
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkSecOptVerifySetLimitInfo)(
    const TssSdkSecOptLimitInfo *info);
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkSecOptSetUserSafeStatus)(
    const TssSdkSecOptSafeStatus *status);
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkSecOptSetVerifyInfo)(
    const TssSdkSecOptVerifyInfo *info);
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkSecOptSetVerifyResult)(
    const TssSdkSecOptVerifyResult *result);

// tss_sdk_uic_interface.h callback typedef (line 75).
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkUicChatOnJudgeResult)(
    const TssSdkUicJudgeResult *result);

// tss_sdk_captcha_interface.h callback typedefs (lines 105/106).
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkCaptchaRecvPicData)(
    const TssSdkCaptchaPicInfo *pic_info);
typedef tss_sdk_if::ETssSdkProcResult (*TssSdkCaptchaVerifyRsp)(
    const TssSdkCaptchaVerifyRspInfo *rsp_info);

// ---------------------------------------------------------------------------
// Public argument structures (DWARF file 439).
// ---------------------------------------------------------------------------

struct TssSdkAntiDataInfo {           // sizeof 10
    unsigned int uin_;                // +0
    unsigned short anti_data_len_;    // +4
    const unsigned char *anti_data_;  // +6
};

struct TssSdkPunishUserInfo {          // sizeof 24
    unsigned int uin_;                 // +0
    int punish_mode_;                  // +4
    unsigned int punish_time_;         // +8
    unsigned short punish_reason_len_; // +c
    const unsigned char *punish_reason_; // +e
    unsigned short punish_info_len_;   // +12
    const unsigned char *punish_info_; // +14
};

struct TssSdkAntiAddUserInfo {   // sizeof 16
    unsigned int uin_;           // +0
    unsigned int client_ver_;    // +4
    unsigned int client_ip_;     // +8
    const char *role_name_;      // +c
};

struct TssSdkAntiDelUserInfo {  // sizeof 4
    unsigned int uin_;          // +0
};

struct TssSdkAntiInitInfo {                     // sizeof 8
    TssSdkSendDataToClient send_data_to_client_;  // +0
    TssSdkPunishUser punish_user_;                // +4
};

struct TssSdkAntiDecryptPkgInfo {    // sizeof 20
    unsigned int uin_;               // +0
    const unsigned char *anti_data_; // +4
    unsigned int anti_data_len_;     // +8
    unsigned char *game_pkg_buf_;    // +c
    unsigned int game_pkg_buf_len_;  // +10
};

struct TssSdkAntiEncryptPkgInfo {    // sizeof 24
    unsigned int uin_;               // +0
    unsigned int cmd_id_;            // +4
    const unsigned char *game_pkg_;  // +8
    unsigned int game_pkg_len_;      // +c
    unsigned char *anti_data_;       // +10
    unsigned int anti_data_len_;     // +14
};

struct TssSdkAntiGamePkgInfo {       // sizeof 16
    unsigned int uin_;               // +0
    unsigned int cmd_id_;            // +4
    const unsigned char *game_pkg_;  // +8
    unsigned int game_pkg_len_;      // +c
};

struct TssSdkCaptchaInitInfo {                  // sizeof 8
    TssSdkCaptchaRecvPicData on_recv_pic_data_;  // +0
    TssSdkCaptchaVerifyRsp on_verify_rsp_;       // +4
};

struct TssSdkCaptchaStartInfo {           // sizeof 27
    unsigned int uin_;                    // +0
    unsigned int req_seq_;                // +4
    unsigned int client_ip_;              // +8
    unsigned short max_pic_change_num_;   // +c
    unsigned short max_verify_num_;       // +e
    unsigned int timeout_;                // +10
    unsigned char captcha_type_;          // +14
    unsigned short busi_info_len_;        // +15
    const unsigned char *busi_info_;      // +17
};

struct TssSdkCaptchaChangePicInfo {  // sizeof 4
    unsigned int captcha_seq_;       // +0
};

struct TssSdkCaptchaVerifyReqInfo {  // sizeof 8
    unsigned int captcha_seq_;       // +0
    const char *answer_;             // +4
};

// tss_sdk_captcha_interface.h callback payload (DWARF ._487, sizeof 37).
// picture_/busi_info_ point into the plugin's CaptchaPkg buffer.
struct TssSdkCaptchaPicInfo {            // sizeof 37
    unsigned int uin_;                   // +0
    unsigned int req_seq_;               // +4
    unsigned int captcha_seq_;           // +8
    unsigned int timeout_;               // +c
    unsigned short verify_times_limit_;  // +10
    unsigned short verify_times_;        // +12
    unsigned short change_times_limit_;  // +14
    unsigned short change_times_;        // +16
    unsigned short picture_len_;         // +18
    const unsigned char *picture_;       // +1a
    unsigned char captcha_type_;         // +1e
    unsigned short busi_info_len_;       // +1f
    const unsigned char *busi_info_;     // +21
};

// tss_sdk_captcha_interface.h callback payload (DWARF ._490, sizeof 16).
// result_ holds the TSS_SDK_CAPTCHA_RESULT_* enumerators.
struct TssSdkCaptchaVerifyRspInfo {  // sizeof 16
    unsigned int req_seq_;           // +0
    int result_;                     // +4
    unsigned int err_msg_len_;       // +8
    const char *err_msg_;            // +c
};

struct TssSdkSecOptSafeStatus {  // sizeof 8
    unsigned int uin_;           // +0
    unsigned int status_;        // +4
};

struct TssSdkSecOptLimitInfo {  // sizeof 1032
    unsigned int uin_;          // +0
    int limit_status_;          // +4
    char unlock_url_[1024];     // +8
};

struct TssSdkSecOptVerifyInfo {  // sizeof 1032
    unsigned int uin_;           // +0
    unsigned int seq_;           // +4
    char verify_url_[1024];      // +8
};

struct TssSdkSecOptVerifyResult {  // sizeof 12
    unsigned int uin_;             // +0
    unsigned int seq_;             // +4
    unsigned int result_;          // +8
};

struct TssSdkSafemodeInitData {                       // sizeof 16
    TssSdkSecOptVerifySetLimitInfo set_limit_info_;         // +0
    TssSdkSecOptSetUserSafeStatus set_safe_status_;   // +4
    TssSdkSecOptSetVerifyInfo set_verify_info_;       // +8
    TssSdkSecOptSetVerifyResult set_verify_result_;   // +c
};

struct TssSdkSecOptVerifyParam {   // sizeof 8
    const char *param_key_;        // +0
    const char *param_value_;      // +4
};

struct TssSdkSecOptVerifyAppendParamInfo {  // sizeof 1032
    char url_[1024];                        // +0
    unsigned int param_count_;              // +400
    const TssSdkSecOptVerifyParam *param_;  // +404
};

struct TssSdkSecOptVerifyReqInfo {  // sizeof 82
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    unsigned int op_level_;         // +8
    short op_scene_len_;            // +c
    char op_scene_[64];             // +e
    unsigned int client_ip_;        // +4e
};

struct TssSdkUicJudgeResult {          // sizeof 22
    unsigned int uin_;                 // +0
    int msg_result_flag_;              // +4
    unsigned int msg_len_;             // +8
    const unsigned char *msg_;         // +c
    unsigned short callback_data_len_; // +10
    const unsigned char *callback_data_; // +12
};

struct TssSdkUicInitInfo {                  // sizeof 4
    TssSdkUicChatOnJudgeResult on_chat_judge_result_;  // +0
};

struct TssSdkUicNameUserInputInfo {  // sizeof 13
    unsigned int msg_len_;           // +0
    const unsigned char *msg_;       // +4
    char if_replace_;                // +8
    int msg_result_flag_;            // +9
};

struct TssSdkUicChatUserInputInfo {       // sizeof 24
    unsigned int uin_;                    // +0
    unsigned int msg_len_;                // +4
    const unsigned char *msg_;            // +8
    unsigned short callback_data_len_;    // +c
    const unsigned char *callback_data_;  // +e
    unsigned short game_data_len_;        // +12
    const unsigned char *game_data_;      // +14
};

namespace tss_sdk_impl {
// uic_tdirty.h: 28-byte judge-result record passed to plugin_callback_proc
// (distinct from the 36-byte shared-memory UicUserInputInfo).
#pragma pack(push, 4)
struct UicChatJudgeResult {
    unsigned int uin_;                  // +0x00
    int msg_result_flag_;               // +0x04
    unsigned int message_len_;          // +0x08
    unsigned char *message_;            // +0x0c
    unsigned short callback_data_len_;  // +0x10
    unsigned char *callback_data_;      // +0x14
    int input_type_;                    // +0x18
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// File-local tables and state (one set for the merged export unit).
// ---------------------------------------------------------------------------

typedef int (*TssSdkInitFn)(unsigned int instance_id);
typedef int (*TssSdkInitPluginsFn)(
    const tss_sdk_if::TssSdkPluginInitData *plugin_init_data,
    unsigned int plugin_count);
typedef int (*TssSdkFiniFn)(tss_sdk_if::ETssSdkPluginId plugin_id);
typedef int (*TssSdkProcFn)();
typedef int (*TssSdkNotifyFn)(
    tss_sdk_if::TssSdkPluginNotifyData notify_data);
typedef int (*TssSdkGetLastErrorCodeFn)();
typedef const char *(*TssSdkGetLastErrorFn)();

// tss_sdk_init.h: anonymous structure, sizeof 28.
struct TssSdkInterface {
    TssSdkInitFn init;                          // +0x00
    TssSdkInitPluginsFn init_plugins;           // +0x04
    TssSdkFiniFn fini;                          // +0x08
    TssSdkProcFn proc;                          // +0x0c
    TssSdkNotifyFn notify;                      // +0x10
    TssSdkGetLastErrorCodeFn get_last_error_code;  // +0x14
    TssSdkGetLastErrorFn get_last_error;        // +0x18
};

// Callback helpers from the same original TUs (see file header).  The first
// two are file-local in ORIG (nm binding 't'), the other three are exported.
static int punish_user_callback(void *data);           // 0x0008a7b0
static int captcha_callback(void *data);               // 0x0008b570
int safemode_plugin_callback_proc(void *data);         // 0x0008c210
int plugin_callback_proc(void *data);                  // 0x0008c790
int send_anti_data_to_client(void *data);              // 0x0008a970

// tss_sdk_anti_impl.cpp file-scope state.
static TssSdkAntiInitInfo g_anti_init_info_;

// captcha/tss_sdk_captcha_impl.cpp file-scope state.  g_captcha_info is the
// 8-byte slot at 0x0029e445 that captcha_callback calls through; this build's
// tss_sdk_captcha_init never stores into it (its argument is unused).
static bool g_tss_sdk_captcha_initialized;
static TssSdkCaptchaInitInfo g_captcha_info __attribute__((used));

// safemode/tss_sdk_safemode_impl.cpp file-scope state.
static bool s_tss_sdk_safemode_inited;
static TssSdkSecOptVerifySetLimitInfo g_set_limitinfo;
static TssSdkSecOptSetUserSafeStatus g_set_safestatus;
static TssSdkSecOptSetVerifyInfo g_set_verifyinfo;
static TssSdkSecOptSetVerifyResult g_set_verifyresult;

// uic/tss_sdk_uic_impl.cpp file-scope state.
static bool g_tss_sdk_uic_initialized;
static TssSdkUicChatOnJudgeResult g_on_judge_result;

// ---------------------------------------------------------------------------
// proto_proc plugin package (tss_sdk_proto_proc_plugin.h, sizeof 36).
// ---------------------------------------------------------------------------

enum ETssSdkProtoProcMsgId {
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG = 201,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG = 202,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG = 203,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST = 304,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST = 305
};

struct ProtoProcPkgHead {  // sizeof 4
    int msg_id_;          // +0
};

struct ProtoProcDecryptListPkg {  // sizeof 16
    int check_result_;            // +0
    int cmd_id_;                  // +4
    unsigned int uin_;            // +8
    unsigned int idx_;            // +c
};

struct ProtoProcDownPkg {          // sizeof 32
    int encrypt_result_;           // +0
    int cmd_id_;                   // +4
    unsigned int uin_;             // +8
    unsigned int idx_;             // +c
    const unsigned char *input_buf_;   // +10
    unsigned int input_buf_size_;      // +14
    unsigned char *output_buf_;        // +18
    unsigned int output_buf_size_;     // +1c
};

struct ProtoProcNotifyPkg {    // sizeof 16
    int notify_result_;        // +0
    unsigned int main_id_;     // +4
    const unsigned char *buf_; // +8
    unsigned int buf_size_;    // +c
};

struct ProtoProcAntiCheatingPkg {   // sizeof 24
    int is_cheating_pkg_;           // +0
    int cmd_id_;                    // +4
    unsigned int uin_;              // +8
    unsigned int idx_;              // +c
    const unsigned char *game_pkg_; // +10
    unsigned int game_pkg_len_;     // +14
};

struct ProtoProcAntibotPkg {   // sizeof 12
    unsigned int uin_;         // +0
    const unsigned char *buf_; // +4
    unsigned int buf_size_;    // +8
};

struct ProtoProcDpPkg {            // sizeof 28
    int decrypt_result_;           // +0
    unsigned int idx_;             // +4
    unsigned int uin_;             // +8
    const unsigned char *input_buf_;   // +c
    unsigned int input_buf_size_;      // +10
    unsigned char *output_buf_;        // +14
    unsigned int output_buf_size_;     // +18
};

struct ProtoProcDownListPkg {  // sizeof 16
    int cmd_id_;               // +0
    unsigned int uin_;         // +4
    unsigned int idx_;         // +8
    int is_in_down_list_;      // +c
};

union ProtoProcPkgBody {  // sizeof 32
    ProtoProcDecryptListPkg decrypt_list_pkg_;  // 16
    ProtoProcDownPkg down_pkg_;                 // 32
    ProtoProcNotifyPkg notify_pkg_;             // 16
    ProtoProcAntiCheatingPkg anti_cheating_pkg_; // 24
    ProtoProcAntibotPkg antibot_pkg_;           // 12
    ProtoProcDpPkg dp_pkg_;                     // 28
    ProtoProcDownListPkg down_list_pkg_;        // 16
};

struct ProtoProcPkg {  // sizeof 36
    ProtoProcPkgHead head_;  // +0
    ProtoProcPkgBody body_;  // +4
};

// ---------------------------------------------------------------------------
// user_control plugin package (tss_sdk_user_control_plugin.h, sizeof 36).
// ---------------------------------------------------------------------------

enum ETssSdkUserControlMsgId {
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER = 1,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER = 2,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER = 3,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO = 4,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN = 5
};

struct UserControlPkgHead {  // sizeof 4
    int msg_id_;             // +0
};

struct UserControlAddUser {  // sizeof 28
    unsigned int idx_;           // +0
    unsigned int uin_;           // +4
    unsigned int user_id_;       // +8
    unsigned int client_ver_;    // +c
    unsigned int server_ip_;     // +10
    unsigned int client_ip_;     // +14
    const char *player_name_;    // +18
};

struct UserControlDeleteUser {  // sizeof 8
    unsigned int idx_;          // +0
    unsigned int uin_;          // +4
};

struct UserControlPunishUser {        // sizeof 32
    unsigned int uin_;                // +0
    unsigned short game_id_;          // +4
    unsigned short room_id_;          // +6
    int punish_mode_;                 // +8
    int punish_time_;                 // +c
    unsigned int punish_info_len_;    // +10
    const unsigned char *punish_info_; // +14
    unsigned int punish_reason_len_;  // +18
    const unsigned char *punish_reason_; // +1c
};

struct UserControlUserInfo {   // sizeof 12
    unsigned int uin_;         // +0
    unsigned int idx_;         // +4
    void *user_info_;          // +8
};

union UserControlPkgBody {  // sizeof 32
    UserControlAddUser add_user_;        // 28
    UserControlDeleteUser delete_user_;  // 8
    UserControlPunishUser punish_user_;  // 32
    UserControlUserInfo user_info_;      // 12
};

struct UserControlPkg {  // sizeof 36
    UserControlPkgHead head_;  // +0
    UserControlPkgBody body_;  // +4
};

// ---------------------------------------------------------------------------
// captcha plugin package (tss_sdk_captcha_plugin.h, sizeof 4641).
// ---------------------------------------------------------------------------

struct CaptchaPkgHead {  // sizeof 16
    unsigned short msg_id_;  // +0
    unsigned int uin_;       // +2
    unsigned int busi_seq_;  // +6
    unsigned int seq_;       // +a
    unsigned short body_len_; // +e
};

struct CaptchaStartReq {           // sizeof 531
    unsigned int client_ip_;       // +0
    unsigned int platform_busid_;  // +4
    unsigned short max_pic_change_num_; // +8
    unsigned short max_verify_num_;     // +a
    unsigned int timeout_;              // +c
    unsigned char busi_id_;             // +10
    unsigned short busi_info_len_;      // +11
    char busi_info_[512];               // +13
};

struct CaptchaPicReq {              // sizeof 4625
    unsigned int timeout_;          // +0
    unsigned short verify_times_limit_; // +4
    unsigned short verify_times_;       // +6
    unsigned short change_times_limit_; // +8
    unsigned short change_times_;       // +a
    unsigned short picture_len_;        // +c
    unsigned char picture_[4096];       // +e
    unsigned char busi_id_;             // +100e
    unsigned short busi_info_len_;      // +100f
    char busi_info_[512];               // +1011
};

struct CaptchaVerifyReq {       // sizeof 65
    unsigned char answer_len_;  // +0
    unsigned char answer_[64];  // +1
};

struct CaptchaVerifyRsp {   // sizeof 130
    unsigned char result_;  // +0
    unsigned char msg_len_; // +1
    char msg_[128];         // +2
};

union CaptchaPkgBody {  // sizeof 4625
    CaptchaStartReq captcha_req_;   // 531
    CaptchaPicReq captcha_pic_;     // 4625
    CaptchaVerifyReq verify_req_;   // 65
    CaptchaVerifyRsp captcha_res_;  // 130
};

struct CaptchaPkg {  // sizeof 4641
    CaptchaPkgHead head_;  // +0
    CaptchaPkgBody body_;  // +10
};

// ---------------------------------------------------------------------------
// safemode plugin package (tss_sdk_safemode_plugin.h, sizeof 90).
// ---------------------------------------------------------------------------

struct SafemodePkgHead {   // sizeof 6
    unsigned short msg_type_; // +0
    unsigned int uin_;        // +2
};

struct SafemodeUpdate {      // sizeof 38
    unsigned short ipc_days_; // +0
    signed char safemode_level_;  // +2
    signed char has_verify_mb_;   // +3
    unsigned short event_type_;   // +4
    signed char reserve_[32];     // +6
};

struct SafemodeAck {         // sizeof 38
    unsigned short ipc_days_; // +0
    signed char safemode_level_;  // +2
    signed char has_verify_mb_;   // +3
    unsigned short event_type_;   // +4
    signed char reserve_[32];     // +6
};

struct SafemodeLimitStatus {  // sizeof 20
    int ret_;                 // +0
    unsigned int uin_;        // +4
    int limit_status_;        // +8
    unsigned int unlock_url_len_; // +c
    const char *unlock_url_;      // +10
};

struct SafemodeSafeStatus {  // sizeof 8
    unsigned int uin_;       // +0
    unsigned int status_;    // +4
};

struct SafemodeVerifyInfo {      // sizeof 14
    unsigned int uin_;           // +0
    unsigned int seq_;           // +4
    short verify_url_len_;       // +8
    char *verify_url_;           // +a (DWARF: char*; the callback writes the
                                 //     terminator through it)
};

struct SafemodeLaunchVerify {  // sizeof 84
    unsigned int uin_;         // +0
    unsigned int seq_;         // +4
    unsigned int op_level_;    // +8
    unsigned int op_scene_len_; // +c
    char op_scene_[64];        // +10
    unsigned int client_ip_;   // +50
};

struct SafemodeVerifyStatus {  // sizeof 12
    unsigned int uin_;         // +0
    unsigned int seq_;         // +4
    unsigned int result_;      // +8
};

struct SafemodeAppendParam {  // sizeof 16
    const char *url_;         // +0
    unsigned int url_buf_max_len_; // +4
    unsigned int param_count_;     // +8
    const TssSdkSecOptVerifyParam *param_; // +c
};

union SafemodePkgBody {  // sizeof 84
    SafemodeUpdate safemode_update_;       // 38
    SafemodeAck safemode_ack_;             // 38
    SafemodeLimitStatus limit_status_;     // 20
    SafemodeAppendParam append_param_;     // 16
    SafemodeSafeStatus mba_safe_status_;   // 8
    SafemodeVerifyInfo mba_verify_info_;   // 14
    SafemodeLaunchVerify mba_launch_verify_; // 84
    SafemodeVerifyStatus mba_verify_status_; // 12
};

struct SafemodePkg {  // sizeof 90
    SafemodePkgHead head_;  // +0
    SafemodePkgBody body_;  // +6
};

// ---------------------------------------------------------------------------
// uic plugin input (uic/tss_sdk_uic_impl.cpp, sizeof 40).
// ---------------------------------------------------------------------------

enum EUicInputType {
    EUITYPE_CHAT = 0,
    EUITYPE_NAME = 1,
    EUITYPE_IMAGE = 2,
    EUITYPE_VIDEO = 3
};

enum EMsgResultFlag {
    EMSG_NORMAL_FLAG = 0,
    EMSG_EVIL_FLAG = 1,
    EMSG_DIRTY_FLAG = 2,
    EMSG_NOT_OPERATE_FLAG = 3
};

}  // namespace tss_sdk_impl

#pragma pack(pop)

namespace tss_sdk_impl {

// uic/tss_sdk_uic_impl.cpp internal plugin input.  Unlike the public
// tss_sdk_if request structures this one is naturally aligned: DWARF gives
// sizeof 40 and puts message_len_ at +0x10 (three padding bytes after
// if_replace_).
struct UicPluginInput {              // sizeof 40
    unsigned int uin_;               // +0
    EUicInputType input_type_;       // +4
    int msg_result_flag_;            // +8
    char if_replace_;                // +c
    unsigned int message_len_;       // +10
    const unsigned char *message_;   // +14
    unsigned short callback_data_len_; // +18
    const unsigned char *callback_data_; // +1c
    unsigned short game_data_len_;   // +20
    const unsigned char *game_data_; // +24
};

// uic/uic_user_input_info.h: one queued input record, also the object the uic
// plugin passes to plugin_callback_proc().  Naturally aligned: DWARF gives
// sizeof 28 with callback_data_ at +0x14 (two padding bytes after the u16).


}  // namespace tss_sdk_impl

// Compile-time verification of every DWARF size used above.
typedef char TssSdkLayoutCheck_PluginInitData[
    sizeof(tss_sdk_if::TssSdkPluginInitData) == 12 ? 1 : -1];
typedef char TssSdkLayoutCheck_PluginNotifyData[
    sizeof(tss_sdk_if::TssSdkPluginNotifyData) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_Interface[
    sizeof(tss_sdk_impl::TssSdkInterface) == 28 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiAddUser[
    sizeof(TssSdkAntiAddUserInfo) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiDelUser[
    sizeof(TssSdkAntiDelUserInfo) == 4 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiInit[
    sizeof(TssSdkAntiInitInfo) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiDecrypt[
    sizeof(TssSdkAntiDecryptPkgInfo) == 20 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiEncrypt[
    sizeof(TssSdkAntiEncryptPkgInfo) == 24 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiGame[
    sizeof(TssSdkAntiGamePkgInfo) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaInit[
    sizeof(TssSdkCaptchaInitInfo) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaStart[
    sizeof(TssSdkCaptchaStartInfo) == 27 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaChange[
    sizeof(TssSdkCaptchaChangePicInfo) == 4 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaVerifyReq[
    sizeof(TssSdkCaptchaVerifyReqInfo) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeInit[
    sizeof(TssSdkSafemodeInitData) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptAppend[
    sizeof(TssSdkSecOptVerifyAppendParamInfo) == 1032 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptReq[
    sizeof(TssSdkSecOptVerifyReqInfo) == 82 ? 1 : -1];
typedef char TssSdkLayoutCheck_UicInit[
    sizeof(TssSdkUicInitInfo) == 4 ? 1 : -1];
typedef char TssSdkLayoutCheck_UicName[
    sizeof(TssSdkUicNameUserInputInfo) == 13 ? 1 : -1];
typedef char TssSdkLayoutCheck_UicChat[
    sizeof(TssSdkUicChatUserInputInfo) == 24 ? 1 : -1];
typedef char TssSdkLayoutCheck_UicJudgeResult[
    sizeof(TssSdkUicJudgeResult) == 22 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcPkg[
    sizeof(tss_sdk_impl::ProtoProcPkg) == 36 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlPkg[
    sizeof(tss_sdk_impl::UserControlPkg) == 36 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaPkg[
    sizeof(tss_sdk_impl::CaptchaPkg) == 4641 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodePkg[
    sizeof(tss_sdk_impl::SafemodePkg) == 90 ? 1 : -1];
typedef char TssSdkLayoutCheck_UicPluginInput[
    sizeof(tss_sdk_impl::UicPluginInput) == 40 ? 1 : -1];
typedef char TssSdkLayoutCheck_UicChatJudgeResult[
    sizeof(tss_sdk_impl::UicChatJudgeResult) == 28 ? 1 : -1];
typedef char TssSdkLayoutCheck_ChannelGamePkg[
    sizeof(TssSdkPluginChannelGameChannelPkg) == 14 ? 1 : -1];
typedef char TssSdkLayoutCheck_DPInterDataHead[
    sizeof(sec_proto::DPInterDataHead) == 5 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaPicInfo[
    sizeof(TssSdkCaptchaPicInfo) == 37 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaVerifyRspInfo[
    sizeof(TssSdkCaptchaVerifyRspInfo) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_AntiDataInfo[
    sizeof(TssSdkAntiDataInfo) == 10 ? 1 : -1];
typedef char TssSdkLayoutCheck_PunishUserInfo[
    sizeof(TssSdkPunishUserInfo) == 24 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptSafeStatus[
    sizeof(TssSdkSecOptSafeStatus) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptLimitInfo[
    sizeof(TssSdkSecOptLimitInfo) == 1032 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptVerifyInfo[
    sizeof(TssSdkSecOptVerifyInfo) == 1032 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptVerifyResult[
    sizeof(TssSdkSecOptVerifyResult) == 12 ? 1 : -1];
typedef char TssSdkLayoutCheck_SecOptVerifyParam[
    sizeof(TssSdkSecOptVerifyParam) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcHead[
    sizeof(tss_sdk_impl::ProtoProcPkgHead) == 4 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcDecryptList[
    sizeof(tss_sdk_impl::ProtoProcDecryptListPkg) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcDown[
    sizeof(tss_sdk_impl::ProtoProcDownPkg) == 32 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcNotify[
    sizeof(tss_sdk_impl::ProtoProcNotifyPkg) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcAntiCheating[
    sizeof(tss_sdk_impl::ProtoProcAntiCheatingPkg) == 24 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcAntibot[
    sizeof(tss_sdk_impl::ProtoProcAntibotPkg) == 12 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcDp[
    sizeof(tss_sdk_impl::ProtoProcDpPkg) == 28 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcDownList[
    sizeof(tss_sdk_impl::ProtoProcDownListPkg) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_ProtoProcBody[
    sizeof(tss_sdk_impl::ProtoProcPkgBody) == 32 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlHead[
    sizeof(tss_sdk_impl::UserControlPkgHead) == 4 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlAdd[
    sizeof(tss_sdk_impl::UserControlAddUser) == 28 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlDelete[
    sizeof(tss_sdk_impl::UserControlDeleteUser) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlPunish[
    sizeof(tss_sdk_impl::UserControlPunishUser) == 32 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlUserInfo[
    sizeof(tss_sdk_impl::UserControlUserInfo) == 12 ? 1 : -1];
typedef char TssSdkLayoutCheck_UserControlBody[
    sizeof(tss_sdk_impl::UserControlPkgBody) == 32 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaHead[
    sizeof(tss_sdk_impl::CaptchaPkgHead) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaStartReq[
    sizeof(tss_sdk_impl::CaptchaStartReq) == 531 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaPicReq[
    sizeof(tss_sdk_impl::CaptchaPicReq) == 4625 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaVerifyPkgReq[
    sizeof(tss_sdk_impl::CaptchaVerifyReq) == 65 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaVerifyPkgRsp[
    sizeof(tss_sdk_impl::CaptchaVerifyRsp) == 130 ? 1 : -1];
typedef char TssSdkLayoutCheck_CaptchaBody[
    sizeof(tss_sdk_impl::CaptchaPkgBody) == 4625 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeHead[
    sizeof(tss_sdk_impl::SafemodePkgHead) == 6 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeUpdate[
    sizeof(tss_sdk_impl::SafemodeUpdate) == 38 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeAck[
    sizeof(tss_sdk_impl::SafemodeAck) == 38 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeLimitStatus[
    sizeof(tss_sdk_impl::SafemodeLimitStatus) == 20 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeSafeStatus[
    sizeof(tss_sdk_impl::SafemodeSafeStatus) == 8 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeVerifyInfo[
    sizeof(tss_sdk_impl::SafemodeVerifyInfo) == 14 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeLaunchVerify[
    sizeof(tss_sdk_impl::SafemodeLaunchVerify) == 84 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeVerifyStatus[
    sizeof(tss_sdk_impl::SafemodeVerifyStatus) == 12 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeAppendParam[
    sizeof(tss_sdk_impl::SafemodeAppendParam) == 16 ? 1 : -1];
typedef char TssSdkLayoutCheck_SafemodeBody[
    sizeof(tss_sdk_impl::SafemodePkgBody) == 84 ? 1 : -1];

// Offsets the reconstructed bodies actually touch.
typedef char TssSdkOffsetCheck_DecryptOutputSize[
    __builtin_offsetof(TssSdkAntiDecryptPkgInfo, game_pkg_buf_len_) == 0x10
        ? 1 : -1];
typedef char TssSdkOffsetCheck_EncryptAntiDataLen[
    __builtin_offsetof(TssSdkAntiEncryptPkgInfo, anti_data_len_) == 0x14
        ? 1 : -1];
typedef char TssSdkOffsetCheck_GamePkgLen[
    __builtin_offsetof(TssSdkAntiGamePkgInfo, game_pkg_len_) == 0xc
        ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaStartInfo[
    __builtin_offsetof(TssSdkCaptchaStartInfo, busi_info_) == 0x17
        ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaVerifyAnswer[
    __builtin_offsetof(TssSdkCaptchaVerifyReqInfo, answer_) == 4
        ? 1 : -1];
typedef char TssSdkOffsetCheck_SecOptClientIp[
    __builtin_offsetof(TssSdkSecOptVerifyReqInfo, client_ip_) == 0x4e
        ? 1 : -1];
typedef char TssSdkOffsetCheck_SecOptOpScene[
    __builtin_offsetof(TssSdkSecOptVerifyReqInfo, op_scene_) == 0xe
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicNameResult[
    __builtin_offsetof(TssSdkUicNameUserInputInfo, msg_result_flag_) == 9
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicChatCallbackData[
    __builtin_offsetof(TssSdkUicChatUserInputInfo, callback_data_) == 0xe
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicJudgeResultCallbackData[
    __builtin_offsetof(TssSdkUicJudgeResult, callback_data_) == 0x12
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicPluginMessageLen[
    __builtin_offsetof(tss_sdk_impl::UicPluginInput, message_len_) == 0x10
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicPluginMessage[
    __builtin_offsetof(tss_sdk_impl::UicPluginInput, message_) == 0x14
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicPluginCallbackLen[
    __builtin_offsetof(tss_sdk_impl::UicPluginInput, callback_data_len_) == 0x18
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicPluginCallbackData[
    __builtin_offsetof(tss_sdk_impl::UicPluginInput, callback_data_) == 0x1c
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicPluginGameLen[
    __builtin_offsetof(tss_sdk_impl::UicPluginInput, game_data_len_) == 0x20
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicPluginGameData[
    __builtin_offsetof(tss_sdk_impl::UicPluginInput, game_data_) == 0x24
        ? 1 : -1];
typedef char TssSdkOffsetCheck_ProtoProcBody[
    __builtin_offsetof(tss_sdk_impl::ProtoProcPkg, body_) == 4 ? 1 : -1];
typedef char TssSdkOffsetCheck_ProtoProcDpOutputSize[
    __builtin_offsetof(tss_sdk_impl::ProtoProcDpPkg, output_buf_size_) == 0x18
        ? 1 : -1];
typedef char TssSdkOffsetCheck_ProtoProcDownOutputSize[
    __builtin_offsetof(tss_sdk_impl::ProtoProcDownPkg, output_buf_size_) == 0x1c
        ? 1 : -1];
typedef char TssSdkOffsetCheck_ProtoProcAntiCheatingGamePkg[
    __builtin_offsetof(tss_sdk_impl::ProtoProcAntiCheatingPkg, game_pkg_) == 0x10
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UserControlBody[
    __builtin_offsetof(tss_sdk_impl::UserControlPkg, body_) == 4 ? 1 : -1];
typedef char TssSdkOffsetCheck_UserControlPlayerName[
    __builtin_offsetof(tss_sdk_impl::UserControlAddUser, player_name_) == 0x18
        ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaHeadSeq[
    __builtin_offsetof(tss_sdk_impl::CaptchaPkgHead, seq_) == 0xa ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaHeadBusiSeq[
    __builtin_offsetof(tss_sdk_impl::CaptchaPkgHead, busi_seq_) == 6 ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaBody[
    __builtin_offsetof(tss_sdk_impl::CaptchaPkg, body_) == 0x10 ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaBusiInfo[
    __builtin_offsetof(tss_sdk_impl::CaptchaStartReq, busi_info_) == 0x13
        ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaAnswer[
    __builtin_offsetof(tss_sdk_impl::CaptchaVerifyReq, answer_) == 1
        ? 1 : -1];
typedef char TssSdkOffsetCheck_SafemodeHeadUin[
    __builtin_offsetof(tss_sdk_impl::SafemodePkgHead, uin_) == 2 ? 1 : -1];
typedef char TssSdkOffsetCheck_SafemodeBody[
    __builtin_offsetof(tss_sdk_impl::SafemodePkg, body_) == 6 ? 1 : -1];
typedef char TssSdkOffsetCheck_SafemodeLaunchScene[
    __builtin_offsetof(tss_sdk_impl::SafemodeLaunchVerify, op_scene_) == 0x10
        ? 1 : -1];
typedef char TssSdkOffsetCheck_SafemodeLaunchClientIp[
    __builtin_offsetof(tss_sdk_impl::SafemodeLaunchVerify, client_ip_) == 0x50
        ? 1 : -1];
typedef char TssSdkOffsetCheck_SafemodeAppendCount[
    __builtin_offsetof(tss_sdk_impl::SafemodeAppendParam, param_count_) == 8
        ? 1 : -1];
typedef char TssSdkOffsetCheck_SafemodeAppendParam[
    __builtin_offsetof(tss_sdk_impl::SafemodeAppendParam, param_) == 0xc
        ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaPicPicture[
    __builtin_offsetof(TssSdkCaptchaPicInfo, picture_) == 0x1a ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaPicType[
    __builtin_offsetof(TssSdkCaptchaPicInfo, captcha_type_) == 0x1e ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaPicBusiInfo[
    __builtin_offsetof(TssSdkCaptchaPicInfo, busi_info_) == 0x21 ? 1 : -1];
typedef char TssSdkOffsetCheck_CaptchaVerifyErrMsg[
    __builtin_offsetof(TssSdkCaptchaVerifyRspInfo, err_msg_) == 0xc ? 1 : -1];
typedef char TssSdkOffsetCheck_UicUserInputCallbackData[
    __builtin_offsetof(tss_sdk_impl::UicChatJudgeResult, callback_data_) == 0x14
        ? 1 : -1];
typedef char TssSdkOffsetCheck_UicUserInputType[
    __builtin_offsetof(tss_sdk_impl::UicChatJudgeResult, input_type_) == 0x18
        ? 1 : -1];
typedef char TssSdkOffsetCheck_ChannelGameBufSize[
    __builtin_offsetof(TssSdkPluginChannelGameChannelPkg, buf_size_) == 0xc
        ? 1 : -1];
typedef char TssSdkOffsetCheck_DPInterDataHeadDataType[
    __builtin_offsetof(sec_proto::DPInterDataHead, data_type_) == 4 ? 1 : -1];

extern "C" const tss_sdk_impl::TssSdkInterface *tss_get_sdk();

// ORIG static-initialization blocks.  This slice merges four ORIG CUs:
//   tss_sdk_anti_impl.cpp, tss_sdk_captcha_impl.cpp,
//   tss_sdk_safemode_impl.cpp, tss_sdk_uic_impl.cpp
// Each carried its own 16-entry COMM_STAT_ITEM_WITH_NAME array, __ioinit and
// file-local g_tss_sdk = tss_get_sdk(); the single g_tss_sdk below is the
// shared reconstruction pointer for the four split bodies.  Emit one real
// TU-local status/iostream block per ORIG CU; the shared
// Singleton<TssSdkCommSvc> pre-create stays covered by its split slice.
#define TSS_SDK_STATIC_INIT_ID tss_sdk_anti_impl
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID tss_sdk_captcha_impl
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID tss_sdk_safemode_impl
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID tss_sdk_uic_impl
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

namespace tss_sdk_impl {

// The shipped TU static initializers call tss_get_sdk() before main.
static const TssSdkInterface *g_tss_sdk = tss_get_sdk();

// ---------------------------------------------------------------------------
// tss_sdk_anti_impl.cpp
// ---------------------------------------------------------------------------

// 0x0008a270.
tss_sdk_if::ETssSdkProcResult tss_sdk_add_user(
    TssSdkAntiAddUserInfo *add_user_info) {
    if (add_user_info == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_add_user: invalid param, add_user_info=%p",
            add_user_info);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_add_user: uin=%d, cli_ver=%u, client_ip=%x, role_name=%s",
        add_user_info->uin_, add_user_info->client_ver_,
        add_user_info->client_ip_, add_user_info->role_name_);

    UserControlPkg user_control_pkg;
    user_control_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER;
    user_control_pkg.body_.add_user_.idx_ = 0;
    user_control_pkg.body_.add_user_.uin_ = add_user_info->uin_;
    user_control_pkg.body_.add_user_.user_id_ = add_user_info->uin_;
    user_control_pkg.body_.add_user_.client_ver_ = add_user_info->client_ver_;
    user_control_pkg.body_.add_user_.server_ip_ = 0;
    user_control_pkg.body_.add_user_.client_ip_ = add_user_info->client_ip_;
    user_control_pkg.body_.add_user_.player_name_ = add_user_info->role_name_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL;
    notify_data.notify_data_ = &user_control_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0 || user_control_pkg.body_.add_user_.idx_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "add user fail. ret=%d, uin=%u, idx=%d",
            ret, add_user_info->uin_,
            user_control_pkg.body_.add_user_.idx_);
        return tss_sdk_if::TSS_SDK_PROC_FAIL;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "add user successful, uin=%u, idx=%d",
        add_user_info->uin_, user_control_pkg.body_.add_user_.idx_);
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008a1e0.
tss_sdk_if::ETssSdkProcResult tss_sdk_del_user(
    const TssSdkAntiDelUserInfo *del_user_info) {
    if (del_user_info == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_del_user: invalid param, del_user_info=%p",
            del_user_info);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_del_user:uin=%u.", del_user_info->uin_);

    UserControlPkg user_control_pkg;
    user_control_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER;
    user_control_pkg.body_.delete_user_.uin_ = del_user_info->uin_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL;
    notify_data.notify_data_ = &user_control_pkg;

    // The shipped code ignores notify()'s result here.
    g_tss_sdk->notify(notify_data);
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008a560.
int tss_sdk_is_cheat_pkg(const TssSdkAntiGamePkgInfo *up_pkg_info) {
    if (up_pkg_info == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_is_cheat_pkg: invalid param, up_pkg_info=%p",
            up_pkg_info);
        return 0;
    }

    ProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG;
    proto_proc_pkg.body_.anti_cheating_pkg_.is_cheating_pkg_ = 0;
    proto_proc_pkg.body_.anti_cheating_pkg_.cmd_id_ = up_pkg_info->cmd_id_;
    proto_proc_pkg.body_.anti_cheating_pkg_.uin_ = up_pkg_info->uin_;
    proto_proc_pkg.body_.anti_cheating_pkg_.idx_ = 0;
    proto_proc_pkg.body_.anti_cheating_pkg_.game_pkg_ =
        up_pkg_info->game_pkg_;
    proto_proc_pkg.body_.anti_cheating_pkg_.game_pkg_len_ =
        up_pkg_info->game_pkg_len_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    // The shipped code ignores notify()'s result here.
    g_tss_sdk->notify(notify_data);

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_is_cheat_pkg, uin=%u, cmd_id=%d, is_cheating_pkg=%d.",
        up_pkg_info->uin_, up_pkg_info->cmd_id_,
        proto_proc_pkg.body_.anti_cheating_pkg_.is_cheating_pkg_);

    return proto_proc_pkg.body_.anti_cheating_pkg_.is_cheating_pkg_;
}

// 0x0008a3a0.
tss_sdk_if::ETssSdkProcResult tss_sdk_decrypt_pkg(
    TssSdkAntiDecryptPkgInfo *decrypt_pkg_info) {
    if (decrypt_pkg_info == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_decrypt_pkg: invalid param, decrypt_pkg_info=%p",
            decrypt_pkg_info);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    if (decrypt_pkg_info->anti_data_ == 0 ||
        decrypt_pkg_info->game_pkg_buf_ == 0 ||
        decrypt_pkg_info->anti_data_len_ == 0 ||
        decrypt_pkg_info->game_pkg_buf_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_decrypt_pkg: invalid param, uin=%u, anti_data_=%p, "
            "anti_data_len_=%u, game_pkg_=%p, game_pkg_len_=%u",
            decrypt_pkg_info->uin_, decrypt_pkg_info->anti_data_,
            decrypt_pkg_info->anti_data_len_,
            decrypt_pkg_info->game_pkg_buf_,
            decrypt_pkg_info->game_pkg_buf_len_);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_decrypt_pkg: uin=%u, anti_data_=%p, anti_data_len_=%u, "
        "game_pkg_buf_=%p, game_pkg_buf_len_=%u",
        decrypt_pkg_info->uin_, decrypt_pkg_info->anti_data_,
        decrypt_pkg_info->anti_data_len_, decrypt_pkg_info->game_pkg_buf_,
        decrypt_pkg_info->game_pkg_buf_len_);

    ProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG;
    proto_proc_pkg.body_.dp_pkg_.decrypt_result_ = -1;
    proto_proc_pkg.body_.dp_pkg_.idx_ = 0;
    proto_proc_pkg.body_.dp_pkg_.uin_ = decrypt_pkg_info->uin_;
    proto_proc_pkg.body_.dp_pkg_.input_buf_ = decrypt_pkg_info->anti_data_;
    proto_proc_pkg.body_.dp_pkg_.input_buf_size_ =
        decrypt_pkg_info->anti_data_len_;
    proto_proc_pkg.body_.dp_pkg_.output_buf_ =
        decrypt_pkg_info->game_pkg_buf_;
    proto_proc_pkg.body_.dp_pkg_.output_buf_size_ =
        decrypt_pkg_info->game_pkg_buf_len_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    int result = proto_proc_pkg.body_.dp_pkg_.decrypt_result_;
    if (ret != 0 || result == -1) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_decrypt_pkg: decrypt fail. uin=%u, ret=%d, "
            "anti_data_len_=%u,res=%d",
            decrypt_pkg_info->uin_, ret,
            decrypt_pkg_info->anti_data_len_, result);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }
    if (result != 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_decrypt_pkg: decrypt succ. uin=%u, output_buf_size=%u",
        decrypt_pkg_info->uin_,
        proto_proc_pkg.body_.dp_pkg_.output_buf_size_);

    decrypt_pkg_info->game_pkg_buf_len_ =
        proto_proc_pkg.body_.dp_pkg_.output_buf_size_;
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008a610.
tss_sdk_if::ETssSdkProcResult tss_sdk_encrypt_pkg(
    TssSdkAntiEncryptPkgInfo *down_pkg_info) {
    if (down_pkg_info == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_encrypt_pkg: invalid param, down_pkg_info=%p",
            down_pkg_info);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    if (down_pkg_info->anti_data_ == 0 ||
        down_pkg_info->game_pkg_ == 0 ||
        down_pkg_info->anti_data_len_ == 0 ||
        down_pkg_info->game_pkg_len_ == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_encrypt_pkg: invalid param, uin=%u, anti_data_=%p, "
            "anti_data_len_=%u, game_pkg_=%p, game_pkg_len_=%u",
            down_pkg_info->uin_, down_pkg_info->anti_data_,
            down_pkg_info->anti_data_len_, down_pkg_info->game_pkg_,
            down_pkg_info->game_pkg_len_);
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_encrypt_pkg: uin=%u, anti_data_=%p, anti_data_len_=%u, "
        "game_pkg_=%p, game_pkg_len_=%u",
        down_pkg_info->uin_, down_pkg_info->anti_data_,
        down_pkg_info->anti_data_len_, down_pkg_info->game_pkg_,
        down_pkg_info->game_pkg_len_);

    ProtoProcPkg proto_proc_pkg;
    proto_proc_pkg.head_.msg_id_ =
        TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG;
    proto_proc_pkg.body_.down_pkg_.encrypt_result_ = -1;
    proto_proc_pkg.body_.down_pkg_.cmd_id_ = down_pkg_info->cmd_id_;
    proto_proc_pkg.body_.down_pkg_.uin_ = down_pkg_info->uin_;
    proto_proc_pkg.body_.down_pkg_.idx_ = 0;
    proto_proc_pkg.body_.down_pkg_.input_buf_ = down_pkg_info->game_pkg_;
    proto_proc_pkg.body_.down_pkg_.input_buf_size_ =
        down_pkg_info->game_pkg_len_;
    proto_proc_pkg.body_.down_pkg_.output_buf_ = down_pkg_info->anti_data_;
    proto_proc_pkg.body_.down_pkg_.output_buf_size_ =
        down_pkg_info->anti_data_len_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    notify_data.notify_data_ = &proto_proc_pkg;

    // The shipped code ignores notify()'s return value and uses the
    // encrypt_result_ field written by the plugin.
    g_tss_sdk->notify(notify_data);

    ZEN_Trace_LogMsg::debug_debugEx(
        "tss_sdk_encrypt_pkg: uin_=%u, cmd_id_=%u, "
        "output_buffer_size=%u, encrypt_result=%d",
        down_pkg_info->uin_, down_pkg_info->cmd_id_,
        proto_proc_pkg.body_.down_pkg_.output_buf_size_,
        proto_proc_pkg.body_.down_pkg_.encrypt_result_);

    if (proto_proc_pkg.body_.down_pkg_.encrypt_result_ != 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    down_pkg_info->anti_data_len_ =
        proto_proc_pkg.body_.down_pkg_.output_buf_size_;
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008a7b0 (local symbol).  Registered for the user_control plugin; the
// plugin's 32-byte punish package is translated into the public
// TssSdkPunishUserInfo and handed to the punish callback saved by
// tss_sdk_anti_init().  The shipped code neither checks data nor the saved
// callback and returns the callback's result.
int punish_user_callback(void *data) {
    UserControlPunishUser *user_info = (UserControlPunishUser *)data;
    TssSdkPunishUserInfo punish_user_info;
    punish_user_info.uin_ = user_info->uin_;
    punish_user_info.punish_mode_ = user_info->punish_mode_;
    punish_user_info.punish_time_ = user_info->punish_time_;
    punish_user_info.punish_reason_len_ =
        (unsigned short)user_info->punish_reason_len_;
    punish_user_info.punish_reason_ = user_info->punish_reason_;
    punish_user_info.punish_info_len_ =
        (unsigned short)user_info->punish_info_len_;
    punish_user_info.punish_info_ = user_info->punish_info_;

    ZEN_Trace_LogMsg::debug_debugEx(
        "punish_user_, uin = %u, punish_mode = %d, punish_time = %d, "
        "punish_reason_len = %d, punish_info_len = %d",
        punish_user_info.uin_, punish_user_info.punish_mode_,
        punish_user_info.punish_time_,
        (int)punish_user_info.punish_reason_len_,
        (int)punish_user_info.punish_info_len_);

    return g_anti_init_info_.punish_user_(&punish_user_info);
}

// 0x0008a840.
tss_sdk_if::ETssSdkProcResult tss_sdk_anti_init(
    const TssSdkAntiInitInfo *init_data) {
    ZEN_Trace_LogMsg::debug_infoex(
        "tss_sdk_anti_init , punish_user=%p, send_data_to_client_=%p",
        init_data->punish_user_, init_data->send_data_to_client_);

    g_anti_init_info_ = *init_data;

    tss_sdk_if::TssSdkPluginInitData plugin_array[2];
    plugin_array[0].plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_PROTO_PROC;
    plugin_array[0].callback_proc_ = 0;
    plugin_array[0].init_data_ = 0;
    plugin_array[1].plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_USER_CONTROL;
    plugin_array[1].callback_proc_ = punish_user_callback;
    plugin_array[1].init_data_ = 0;

    int rc = g_tss_sdk->init_plugins(plugin_array, 2);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_anti init fail. ret=%d", rc);
        return tss_sdk_if::TSS_SDK_PROC_FAIL;
    }

    ZEN_Trace_LogMsg::debug_infoex("tss_sdk_anti_init successful");
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008a970.  Callback registered for the channel plugin: it receives the
// 14-byte game-channel package and forwards the buffer to the
// send_data_to_client callback saved by tss_sdk_anti_init().
//
// busi_type_ 0 is an antibot packet: it is re-framed into a 5-byte
// DPInterDataHead (packet_size_ = payload length, protocol_type_ 0,
// data_type_ 1) placed at send_buffer + 4 and the payload is appended at
// send_buffer + 4 + head_size; the leading 4 bytes stay unwritten but are
// reported in anti_data_len_.  busi_type_ 1 is a DP packet that is forwarded
// verbatim.  Return codes: -1 buffer overflow, -2 unknown busi_type_,
// -3 send callback failed, 0 otherwise.
int send_anti_data_to_client(void *sec_data) {
    TssSdkPluginChannelGameChannelPkg *pkg =
        (TssSdkPluginChannelGameChannelPkg *)sec_data;
    TssSdkAntiDataInfo send_data_info;
    send_data_info.uin_ = pkg->uin_;
    unsigned char send_buffer[0x10000];

    if (pkg->busi_type_ == 0) {
        const int dp_big_pkg_flag_size = 4;
        sec_proto::DPInterDataHead head;
        head.packet_size_ = (unsigned short)pkg->buf_size_;
        head.protocol_type_ = 0;
        head.data_type_ = 1;
        size_t use_len = 0;
        head.pack((char *)send_buffer + dp_big_pkg_flag_size,
                  sizeof(send_buffer), &use_len, 0);
        use_len += dp_big_pkg_flag_size;
        if (use_len + (int)pkg->buf_size_ > sizeof(send_buffer)) {
            return -1;
        }
        memcpy(send_buffer + use_len, pkg->buf_, pkg->buf_size_);
        send_data_info.anti_data_ = send_buffer;
        send_data_info.anti_data_len_ =
            (unsigned short)(use_len + pkg->buf_size_);
        ZEN_Trace_LogMsg::debug_debugEx(
            "send antibot pkg.cmd=%d len=%d\n",
            ntohs(*(const unsigned short *)pkg->buf_),
            (int)pkg->buf_size_);
    } else if (pkg->busi_type_ == 1) {
        send_data_info.anti_data_ = pkg->buf_;
        send_data_info.anti_data_len_ = (unsigned short)pkg->buf_size_;
        ZEN_Trace_LogMsg::debug_debugEx(
            "send dp pkg.cmd=%d len=%d\n",
            ntohs(*(const unsigned short *)(pkg->buf_ + 2)),
            (int)pkg->buf_size_);
    } else {
        return -2;
    }

    if (g_anti_init_info_.send_data_to_client_ == 0) {
        return 0;
    }
    if (g_anti_init_info_.send_data_to_client_(&send_data_info) != 0) {
        return -3;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// captcha/tss_sdk_captcha_impl.cpp
// ---------------------------------------------------------------------------

// 0x0008b570 (local symbol).  Registered for the captcha plugin, which
// reuses CaptchaPkg for its replies: msg_id_ 0x0c82 carries picture data,
// 0x0c84 the verification result.  Both branches call through the
// TssSdkCaptchaInitInfo slot 0x0029e445 (g_captcha_info), which this build
// never fills in.
int captcha_callback(void *data) {
    CaptchaPkg *captcha_pkg = (CaptchaPkg *)data;
    CaptchaPkgHead *head = &captcha_pkg->head_;

    if (head->msg_id_ == 0x0c82) {
        const unsigned char *pic_data =
            captcha_pkg->body_.captcha_pic_.picture_;
        TssSdkCaptchaPicInfo captcha_pic;
        captcha_pic.uin_ = head->uin_;
        captcha_pic.req_seq_ = head->busi_seq_;
        captcha_pic.captcha_seq_ = head->seq_;
        captcha_pic.timeout_ = captcha_pkg->body_.captcha_pic_.timeout_;
        captcha_pic.verify_times_limit_ =
            captcha_pkg->body_.captcha_pic_.verify_times_limit_;
        captcha_pic.verify_times_ =
            captcha_pkg->body_.captcha_pic_.verify_times_;
        captcha_pic.change_times_limit_ =
            captcha_pkg->body_.captcha_pic_.change_times_limit_;
        captcha_pic.change_times_ =
            captcha_pkg->body_.captcha_pic_.change_times_;
        captcha_pic.picture_len_ =
            captcha_pkg->body_.captcha_pic_.picture_len_;
        captcha_pic.picture_ = pic_data;
        captcha_pic.captcha_type_ =
            captcha_pkg->body_.captcha_pic_.busi_id_;
        captcha_pic.busi_info_len_ =
            captcha_pkg->body_.captcha_pic_.busi_info_len_;
        captcha_pic.busi_info_ =
            (const unsigned char *)captcha_pkg->body_.captcha_pic_.busi_info_;
        g_captcha_info.on_recv_pic_data_(&captcha_pic);
    } else if (head->msg_id_ == 0x0c84) {
        CaptchaVerifyRsp *verify_rsp = &captcha_pkg->body_.captcha_res_;
        TssSdkCaptchaVerifyRspInfo verify_res;
        // ORIG reads the head's busi_seq_ (+6) here, not seq_ (+a).
        verify_res.req_seq_ = head->busi_seq_;
        verify_res.result_ = verify_rsp->result_;
        verify_res.err_msg_len_ = verify_rsp->msg_len_;
        verify_res.err_msg_ = verify_rsp->msg_;
        g_captcha_info.on_verify_rsp_(&verify_res);
    }

    return 0;
}

// 0x0008b650.  init_data is not inspected by the shipped function.
tss_sdk_if::ETssSdkProcResult tss_sdk_captcha_init(
    const TssSdkCaptchaInitInfo *) {
    if (g_tss_sdk_captcha_initialized) {
        return tss_sdk_if::TSS_SDK_PROC_OK;
    }

    tss_sdk_if::TssSdkPluginInitData plugin_array[1];
    plugin_array[0].plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA;
    plugin_array[0].callback_proc_ = captcha_callback;
    plugin_array[0].init_data_ = 0;

    int rc = g_tss_sdk->init_plugins(plugin_array, 1);
    if (rc != 0) {
        return tss_sdk_if::TSS_SDK_PROC_FAIL;
    }

    g_tss_sdk_captcha_initialized = true;
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008b950.
tss_sdk_if::ETssSdkProcResult tss_sdk_captcha_start(
    const TssSdkCaptchaStartInfo *start_info) {
    CaptchaPkg captch_pkg;
    captch_pkg.head_.msg_id_ = 0x0c81;
    captch_pkg.head_.uin_ = start_info->uin_;
    captch_pkg.head_.busi_seq_ = start_info->req_seq_;
    captch_pkg.body_.captcha_req_.client_ip_ = start_info->client_ip_;
    captch_pkg.body_.captcha_req_.platform_busid_ = 0;
    captch_pkg.body_.captcha_req_.max_pic_change_num_ =
        start_info->max_pic_change_num_;
    captch_pkg.body_.captcha_req_.max_verify_num_ =
        start_info->max_verify_num_;
    captch_pkg.body_.captcha_req_.timeout_ = start_info->timeout_;
    captch_pkg.body_.captcha_req_.busi_id_ = start_info->captcha_type_;

    if (start_info->busi_info_len_ > 0x200) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    captch_pkg.body_.captcha_req_.busi_info_len_ =
        start_info->busi_info_len_;
    memcpy(captch_pkg.body_.captcha_req_.busi_info_,
           start_info->busi_info_, start_info->busi_info_len_);

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA;
    notify_data.notify_data_ = &captch_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008b7b0.
tss_sdk_if::ETssSdkProcResult tss_sdk_captcha_change_pic(
    const TssSdkCaptchaChangePicInfo *change_pic_info) {
    CaptchaPkg captch_pkg;
    captch_pkg.head_.msg_id_ = 0x0c86;
    captch_pkg.head_.busi_seq_ = change_pic_info->captcha_seq_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA;
    notify_data.notify_data_ = &captch_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008b830.
tss_sdk_if::ETssSdkProcResult tss_sdk_captcha_verify(
    const TssSdkCaptchaVerifyReqInfo *verify_req) {
    const char *answer = verify_req->answer_;
    if (answer == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_capthca_verify: bad arg. answer is empty");
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    CaptchaPkg captch_pkg;
    captch_pkg.head_.msg_id_ = 0x0c83;
    captch_pkg.head_.seq_ = verify_req->captcha_seq_;

    unsigned char answer_len =
        (unsigned char)(strlen(answer) + 1);
    if (answer_len > 0x40) {
        ZEN_Trace_LogMsg::debug_errorex(
            "tss_sdk_capthca_verify: bad arg. answer is too long");
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    captch_pkg.body_.verify_req_.answer_len_ = answer_len;
    memcpy(captch_pkg.body_.verify_req_.answer_, answer, answer_len);

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_CAPTCHA;
    notify_data.notify_data_ = &captch_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// ---------------------------------------------------------------------------
// safemode/tss_sdk_safemode_impl.cpp
// ---------------------------------------------------------------------------

// 0x0008bfd0.
tss_sdk_if::ETssSdkProcResult tss_sdk_safemode_init(
    const TssSdkSafemodeInitData *init_data) {
    if (init_data == 0 || init_data->set_limit_info_ == 0 ||
        init_data->set_safe_status_ == 0 ||
        init_data->set_verify_info_ == 0 ||
        init_data->set_verify_result_ == 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    if (s_tss_sdk_safemode_inited) {
        return tss_sdk_if::TSS_SDK_PROC_OK;
    }

    g_set_limitinfo = init_data->set_limit_info_;
    g_set_safestatus = init_data->set_safe_status_;
    g_set_verifyinfo = init_data->set_verify_info_;
    g_set_verifyresult = init_data->set_verify_result_;

    tss_sdk_if::TssSdkPluginInitData plugin_array[1];
    plugin_array[0].plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE;
    plugin_array[0].callback_proc_ = safemode_plugin_callback_proc;
    plugin_array[0].init_data_ = 0;

    int rc = g_tss_sdk->init_plugins(plugin_array, 1);
    if (rc != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "init_plugins fail. err=%d %s", rc,
            g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_FAIL;
    }

    s_tss_sdk_safemode_inited = true;
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008bf30.
tss_sdk_if::ETssSdkProcResult tss_sdk_secoptverify_append_param(
    TssSdkSecOptVerifyAppendParamInfo *info) {
    if (info == 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    SafemodePkg safemode_pkg;
    safemode_pkg.head_.msg_type_ = 0x16;
    safemode_pkg.head_.uin_ = 0;
    SafemodeAppendParam *append_url = &safemode_pkg.body_.append_param_;
    append_url->url_ = info->url_;
    append_url->url_buf_max_len_ = sizeof(info->url_);
    append_url->param_count_ = info->param_count_;
    append_url->param_ = info->param_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE;
    notify_data.notify_data_ = &safemode_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008c120.
tss_sdk_if::ETssSdkProcResult tss_sdk_secoptverify_user_operate(
    const TssSdkSecOptVerifyReqInfo &req) {
    SafemodePkg safemode_pkg;
    safemode_pkg.head_.msg_type_ = 0x18;
    safemode_pkg.head_.uin_ = req.uin_;
    SafemodeLaunchVerify *launch_verify =
        &safemode_pkg.body_.mba_launch_verify_;
    launch_verify->uin_ = req.uin_;
    launch_verify->seq_ = req.seq_;
    launch_verify->op_level_ = req.op_level_;
    launch_verify->op_scene_len_ = req.op_scene_len_;
    memcpy(launch_verify->op_scene_, req.op_scene_,
           sizeof(req.op_scene_));
    launch_verify->client_ip_ = req.client_ip_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE;
    notify_data.notify_data_ = &safemode_pkg;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008c210.  Registered for the safemode plugin, which reports limit and
// verification state back through SafemodePkg with the same 0x15/0x17/0x19/
// 0x1a message types.  Every branch returns the saved callback's result;
// invalid input and oversized URLs return TSS_SDK_PROC_INVALID_ARG (1).
int safemode_plugin_callback_proc(void *sec_data) {
    if (sec_data == 0) {
        return 1;
    }
    SafemodePkg *pkg = (SafemodePkg *)sec_data;

    switch (pkg->head_.msg_type_) {
    case 0x15: {  // SafemodeLimitStatus
        TssSdkSecOptLimitInfo info;
        info.uin_ = pkg->body_.limit_status_.uin_;
        info.limit_status_ = pkg->body_.limit_status_.limit_status_;
        if (pkg->body_.limit_status_.unlock_url_len_ >
            sizeof(info.unlock_url_) - 1) {
            ZEN_Trace_LogMsg::debug_errorex("not enough buf.");
            return 1;
        }
        memcpy(info.unlock_url_, pkg->body_.limit_status_.unlock_url_,
               pkg->body_.limit_status_.unlock_url_len_);
        info.unlock_url_[pkg->body_.limit_status_.unlock_url_len_] = 0;
        return g_set_limitinfo(&info);
    }
    case 0x17: {  // SafemodeSafeStatus
        TssSdkSecOptSafeStatus status;
        status.uin_ = pkg->body_.mba_safe_status_.uin_;
        status.status_ = pkg->body_.mba_safe_status_.status_;
        return g_set_safestatus(&status);
    }
    case 0x19: {  // SafemodeVerifyInfo
        TssSdkSecOptVerifyInfo verify_info;
        short verify_url_len = pkg->body_.mba_verify_info_.verify_url_len_;
        verify_info.uin_ = pkg->body_.mba_verify_info_.uin_;
        verify_info.seq_ = pkg->body_.mba_verify_info_.seq_;
        if ((unsigned short)verify_url_len > sizeof(verify_info.verify_url_) - 1) {
            ZEN_Trace_LogMsg::debug_errorex("not enough buf.");
            return 1;
        }
        memcpy(verify_info.verify_url_, pkg->body_.mba_verify_info_.verify_url_,
               verify_url_len);
        // The shipped code terminates the plugin's source string, not the copy.
        pkg->body_.mba_verify_info_.verify_url_[verify_url_len] = 0;
        return g_set_verifyinfo(&verify_info);
    }
    case 0x1a: {  // SafemodeVerifyStatus
        TssSdkSecOptVerifyResult verify_status;
        verify_status.uin_ = pkg->body_.mba_verify_status_.uin_;
        verify_status.seq_ = pkg->body_.mba_verify_status_.seq_;
        verify_status.result_ = pkg->body_.mba_verify_status_.result_;
        return g_set_verifyresult(&verify_status);
    }
    default:
        return 1;
    }
}

// ---------------------------------------------------------------------------
// uic/tss_sdk_uic_impl.cpp
// ---------------------------------------------------------------------------

// 0x0008c790.  Registered for the uic plugin, which hands over one queued
// UicChatJudgeResult record.  The SDK reports it as TssSdkUicJudgeResult via
// the callback saved by tss_sdk_uic_init(), mapping the plugin's
// EMSG_NOT_OPERATE_FLAG result to MSG_NORMAL_FLAG and returning the
// callback's result.
int plugin_callback_proc(void *sec_data) {
    UicChatJudgeResult *rsp_pkg = (UicChatJudgeResult *)sec_data;
    TssSdkUicJudgeResult rsp_user;
    rsp_user.uin_ = rsp_pkg->uin_;
    rsp_user.msg_len_ = rsp_pkg->message_len_;
    rsp_user.msg_ = rsp_pkg->message_;
    rsp_user.callback_data_len_ = rsp_pkg->callback_data_len_;
    rsp_user.callback_data_ = rsp_pkg->callback_data_;
    rsp_user.msg_result_flag_ =
        (rsp_pkg->msg_result_flag_ == EMSG_NOT_OPERATE_FLAG)
            ? 0  // MSG_NORMAL_FLAG of the SDK-side result enum
            : (int)rsp_pkg->msg_result_flag_;
    return g_on_judge_result(&rsp_user);
}

// 0x0008c7f0.
tss_sdk_if::ETssSdkProcResult tss_sdk_uic_init(
    const TssSdkUicInitInfo *init_info) {
    if (g_tss_sdk_uic_initialized) {
        return tss_sdk_if::TSS_SDK_PROC_OK;
    }
    if (init_info == 0 || init_info->on_chat_judge_result_ == 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    g_on_judge_result = init_info->on_chat_judge_result_;

    tss_sdk_if::TssSdkPluginInitData plugin_array[1];
    plugin_array[0].plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_UIC;
    plugin_array[0].callback_proc_ = plugin_callback_proc;
    plugin_array[0].init_data_ = 0;

    int rc = g_tss_sdk->init_plugins(plugin_array, 1);
    if (rc != 0) {
        return tss_sdk_if::TSS_SDK_PROC_FAIL;
    }

    g_tss_sdk_uic_initialized = true;
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008c960.
tss_sdk_if::ETssSdkProcResult tss_sdk_uic_judge_input_name(
    TssSdkUicNameUserInputInfo *input_info) {
    if (input_info == 0 || input_info->msg_ == 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    UicPluginInput plugin_input;
    plugin_input.uin_ = 0;
    plugin_input.input_type_ = EUITYPE_NAME;
    plugin_input.if_replace_ = input_info->if_replace_;
    plugin_input.message_len_ = input_info->msg_len_;
    plugin_input.message_ = input_info->msg_;
    plugin_input.callback_data_len_ = 0;
    plugin_input.callback_data_ = 0;
    plugin_input.game_data_len_ = 0;
    plugin_input.game_data_ = 0;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_UIC;
    notify_data.notify_data_ = &plugin_input;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    input_info->msg_result_flag_ = plugin_input.msg_result_flag_;
    return tss_sdk_if::TSS_SDK_PROC_OK;
}

// 0x0008ca40.
tss_sdk_if::ETssSdkProcResult tss_sdk_uic_judge_input_chat(
    const TssSdkUicChatUserInputInfo *input_info) {
    if (input_info == 0 || input_info->msg_ == 0) {
        return tss_sdk_if::TSS_SDK_PROC_INVALID_ARG;
    }

    UicPluginInput plugin_input;
    plugin_input.uin_ = input_info->uin_;
    plugin_input.input_type_ = EUITYPE_CHAT;
    plugin_input.if_replace_ = 1;
    plugin_input.message_len_ = input_info->msg_len_;
    plugin_input.message_ = input_info->msg_;
    plugin_input.callback_data_len_ = input_info->callback_data_len_;
    plugin_input.callback_data_ = input_info->callback_data_;
    plugin_input.game_data_len_ = input_info->game_data_len_;
    plugin_input.game_data_ = input_info->game_data_;

    tss_sdk_if::TssSdkPluginNotifyData notify_data;
    notify_data.plugin_id_ = tss_sdk_if::ETSS_SDK_PLUGIN_ID_UIC;
    notify_data.notify_data_ = &plugin_input;

    int ret = g_tss_sdk->notify(notify_data);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "notify fail. err=%d %s", ret, g_tss_sdk->get_last_error());
        return tss_sdk_if::TSS_SDK_PROC_INTERNAL_ERR;
    }

    return tss_sdk_if::TSS_SDK_PROC_OK;
}

}  // namespace tss_sdk_impl
