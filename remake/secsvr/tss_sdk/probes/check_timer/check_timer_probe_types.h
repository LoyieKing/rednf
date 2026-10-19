// ABI declarations used only by the check_timer differential probe.
//
// This file is deliberately a separate TU from the reconstructed
// dp_msg_handler_recv_agent.cpp.  The probe never includes that .cpp; it loads
// two independently compiled shared objects (before/after variant) and the
// original libtss_sdk.so.1.0.394, then calls their exported check_timer
// symbols.  Keep every layout assertion below in sync with the DWARF-derived
// declarations in that TU.
#ifndef TSS_SDK_CHECK_TIMER_PROBE_TYPES_H
#define TSS_SDK_CHECK_TIMER_PROBE_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

namespace sec_proto {

#pragma pack(push, 1)

struct DPClientCmdData {
    unsigned int cmd_id_;    // +0x00
    unsigned int cmd_count_; // +0x04
};

struct DpClientShortCmdData {
    unsigned short cmd_id_;    // +0x00
    unsigned short cmd_count_; // +0x02
};

struct DPClientStatPtc {
    unsigned int internal_msg_send_count_;        // +0x000
    unsigned int internal_msg_recv_count_;        // +0x004
    unsigned int game_pkg_recv_count_;            // +0x008
    unsigned int fix_encrypt_count_;              // +0x00c
    unsigned int game_fix_decrypt_count_;         // +0x010
    unsigned int game_dynamic_decrypt_count_;     // +0x014
    unsigned int seq_less_count_;                 // +0x018
    unsigned int seq_greater_count_;              // +0x01c
    unsigned int seq_discard_count_;              // +0x020
    unsigned int down_cmd_list_crc_;              // +0x024
    unsigned short need_encrypt_count_;           // +0x028
    DPClientCmdData need_encrypt_list_[128];      // +0x02a
    unsigned short client_error_count_;           // +0x42a
    DpClientShortCmdData client_error_list_[128]; // +0x42c
};

struct DPInterDataHead {
    unsigned short packet_size_;   // +0x0
    unsigned short protocol_type_; // +0x2
    unsigned char data_type_;      // +0x4
};

struct DPConfigData {
    unsigned char proto_check_switch_; // +0x0
    unsigned char seq_check_switch_;   // +0x1
    unsigned char raw_check_switch_;   // +0x2
};

union DPInterDataBody {
    DPClientStatPtc client_stat_ptc_; // +0x0
    DPConfigData config_data_;        // +0x0
    unsigned char cs_test_data_[10];  // +0x0
};

struct DPInterData {
    DPInterDataHead head; // +0x0
    DPInterDataBody body; // +0x5
};

#pragma pack(pop)

} // namespace sec_proto

namespace tss_sdk_impl {

struct UserInfo {
    enum E_CLIENT_STATE {
        ECS_NOT_READY = 0,
        ECS_NOT_INIT = 1,
        ECS_INIT_MSG = 2,
        ECS_INIT_MSG2 = 3,
        ECS_INIT_ING = 4,
        ECS_INIT_CONFIRM = 5,
        ECS_INIT_FAILED = 6
    };

    struct BaseInfo {
        unsigned int uin_;            // +0x00
        unsigned int game_id_;        // +0x04
        unsigned int room_id_;        // +0x08
        unsigned int client_ip_;      // +0x0c
        unsigned int server_ip_;      // +0x10
        unsigned int game_usr_id_;    // +0x14
        unsigned int client_version_; // +0x18
        unsigned int client_user_id_; // +0x1c
        unsigned int flags_;          // +0x20
        char role_name_[64];          // +0x24
    };

    struct UserCodeInfo {
        E_CLIENT_STATE client_state_;             // +0x00
        unsigned int last_encrypt_sequence_;      // +0x04
        unsigned int last_game_decrypt_sequence_; // +0x08
        unsigned int last_internal_decrypt_sequence_; // +0x0c
        unsigned int request_code_count_;         // +0x10
        unsigned short server_cur_code_;          // +0x14
        unsigned short server_new_code_;          // +0x16
        unsigned short client_code_[5];           // +0x18
        unsigned short bin_file_code_;            // +0x22
        unsigned short send_bin_file_count_;      // +0x24
        unsigned int down_cmd_list_crc_;          // +0x28
        short up_cmd_list_pos_;                   // +0x2c
        short down_cmd_list_pos_;                 // +0x2e
    };

    struct UserTimer {
        time_t last_check_init_pkg_time_; // +0x00
        time_t last_send_test_pkg_time_;  // +0x04
        time_t last_recv_stat_pkg_time_;  // +0x08
        time_t last_send_bin_code_time_;  // +0x0c
        time_t last_send_bin_file_time_;  // +0x10
    };

    struct UserStat {
        unsigned int internal_msg_send_count_;        // +0x00
        unsigned int fix_encrypt_key_;                // +0x04
        unsigned int internal_fix_encrypt_count_;     // +0x08
        unsigned int fix_encrypt_count_;              // +0x0c
        unsigned int dynamic_encrypt_count_;          // +0x10
        unsigned int internal_fix_decrypt_count_;     // +0x14
        unsigned int game_fix_decrypt_count_;         // +0x18
        unsigned int internal_dynamic_decrypt_count_; // +0x1c
        unsigned int game_dynamic_decrypt_count_;     // +0x20
        unsigned int antibot_last_seq_;               // +0x24
        unsigned int send_to_secsvr_seq_;             // +0x28
        unsigned int last_report_time_;               // +0x2c
        unsigned int now_report_time_;                // +0x30
    };

    struct HackData {
        unsigned int safemode_level_;    // +0x00
        unsigned int dnf_client_uid_;    // +0x04
        time_t drop_cmd_[8];             // +0x08
        unsigned char ban_mode_[128];    // +0x28
        unsigned char cf_user_info_[8];  // +0xa8
        unsigned char delay_cmd_[3];     // +0xb0
        unsigned char captcha_state_;    // +0xb3
    };

    BaseInfo base_info_;           // +0x000
    UserCodeInfo user_code_info_;  // +0x064
    UserTimer user_timer_;         // +0x094
    UserStat user_stat_;           // +0x0a8
    HackData hack_data_;           // +0x0dc
};

} // namespace tss_sdk_impl

namespace tss_dp_comm {

struct DPConfigBase {
    unsigned char is_use_dp;              // +0
    unsigned char is_restore_usr_space;   // +1
    unsigned char is_use_client_version;  // +2
    unsigned char disable_anticheat_pkg;  // +3
    unsigned char use_punish_notify;      // +4
    unsigned char dp_record_using_time;   // +5
};

struct DPProtoConfig {
    unsigned short dp_fragmentation_size;    // +0
    unsigned short dp_max_encrypt_size;      // +2
    unsigned char use_sequence_check;        // +4
    unsigned char proto_check_switch;        // +5
    unsigned char seq_check_switch;          // +6
    unsigned char raw_check_switch;          // +7
    unsigned char dp_decrypt_fail_protect;   // +8
    unsigned char explicit_use_dynamic_encrypt; // +9
};

struct DPLogConf {
    unsigned char raw_[296];
};

struct DPTimeConfig {
    unsigned int max_init_pkg_timeout;      // +0
    unsigned int max_static_pkg_timeout;    // +4
    unsigned int max_confirm_pkg_timeout;   // +8
    unsigned int max_send_test_pkg_timeout; // +12
    unsigned int check_time;                // +16
    unsigned int send_bin_file_gap;         // +20
    unsigned int send_bin_code_gap;         // +24
};

struct DPCodeInfoConf {
    unsigned char raw_[14884];
};

struct DPValueArray128 {
    unsigned char raw_[514];
};

struct DPGameCmdStruct {
    unsigned short raw_;
};

struct DPCmdHackConf {
    unsigned char raw_[7100];
};

struct __attribute__((packed)) DPConfig {
    DPConfigBase dp_base_conf;          // +0
    DPProtoConfig dp_proto_conf;        // +6
    DPLogConf dp_log_conf;              // +16
    DPTimeConfig dp_time_confg;         // +312
    DPCodeInfoConf dp_code_info;        // +340
    DPValueArray128 uin_monitor_list;   // +15224
    DPGameCmdStruct dp_up_cmd_hack;     // +15738
    DPGameCmdStruct dp_down_cmd_hack;   // +15740
    DPCmdHackConf cmd_hack;             // +15742
};

} // namespace tss_dp_comm

namespace tss_dpsdk {

// Only the output-boundary method is needed by the probe.  vm_engine_ keeps
// the first field at +0 so a stray dereference is visible as a null access
// instead of silently reading unrelated probe data.
class DPMsgHandler {
public:
    void *vm_engine_; // +0
    int SendDpMsgToClient(tss_sdk_impl::UserInfo *user,
                          sec_proto::DPInterData &pkg,
                          unsigned char *buf,
                          unsigned short buf_len);
};

} // namespace tss_dpsdk

static_assert(sizeof(time_t) == 4, "probe requires 32-bit time_t");
static_assert(sizeof(sec_proto::DPInterDataHead) == 5, "DPInterDataHead");
static_assert(sizeof(sec_proto::DPClientStatPtc) == 1580, "DPClientStatPtc");
static_assert(sizeof(sec_proto::DPInterDataBody) == 1580, "DPInterDataBody");
static_assert(sizeof(sec_proto::DPInterData) == 1585, "DPInterData");
static_assert(offsetof(sec_proto::DPInterData, body) == 5, "DPInterData body");
static_assert(sizeof(sec_proto::DPConfigData) == 3, "DPConfigData");
static_assert(offsetof(sec_proto::DPInterData, head) == 0, "DPInterData head");

static_assert(sizeof(tss_sdk_impl::UserInfo) == 400, "UserInfo size");
static_assert(offsetof(tss_sdk_impl::UserInfo, base_info_) == 0x000,
              "UserInfo base_info_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_code_info_) == 0x064,
              "UserInfo user_code_info_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_timer_) == 0x094,
              "UserInfo user_timer_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_stat_) == 0x0a8,
              "UserInfo user_stat_");
static_assert(offsetof(tss_sdk_impl::UserInfo, hack_data_) == 0x0dc,
              "UserInfo hack_data_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_code_info_) +
                      offsetof(tss_sdk_impl::UserInfo::UserCodeInfo,
                               client_state_) ==
                  0x064,
              "client_state_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_code_info_) +
                      offsetof(tss_sdk_impl::UserInfo::UserCodeInfo,
                               client_code_) ==
                  0x07c,
              "client_code_[0]");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_timer_) +
                      offsetof(tss_sdk_impl::UserInfo::UserTimer,
                               last_send_test_pkg_time_) ==
                  0x098,
              "last_send_test_pkg_time_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_timer_) +
                      offsetof(tss_sdk_impl::UserInfo::UserTimer,
                               last_recv_stat_pkg_time_) ==
                  0x09c,
              "last_recv_stat_pkg_time_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_timer_) +
                      offsetof(tss_sdk_impl::UserInfo::UserTimer,
                               last_send_bin_code_time_) ==
                  0x0a0,
              "last_send_bin_code_time_");
static_assert(offsetof(tss_sdk_impl::UserInfo, user_timer_) +
                      offsetof(tss_sdk_impl::UserInfo::UserTimer,
                               last_send_bin_file_time_) ==
                  0x0a4,
              "last_send_bin_file_time_");

static_assert(sizeof(tss_dp_comm::DPConfig) == 22842, "DPConfig size");
static_assert(offsetof(tss_dp_comm::DPConfig, dp_proto_conf) == 6,
              "DPConfig dp_proto_conf");
static_assert(offsetof(tss_dp_comm::DPConfig, dp_log_conf) == 16,
              "DPConfig dp_log_conf");
static_assert(offsetof(tss_dp_comm::DPConfig, dp_time_confg) == 0x138,
              "DPConfig dp_time_confg");
static_assert(offsetof(tss_dp_comm::DPConfig, dp_code_info) == 340,
              "DPConfig dp_code_info");
static_assert(offsetof(tss_dp_comm::DPConfig, uin_monitor_list) == 15224,
              "DPConfig uin_monitor_list");
static_assert(offsetof(tss_dp_comm::DPConfig, dp_up_cmd_hack) == 15738,
              "DPConfig dp_up_cmd_hack");
static_assert(offsetof(tss_dp_comm::DPConfig, dp_down_cmd_hack) == 15740,
              "DPConfig dp_down_cmd_hack");
static_assert(offsetof(tss_dp_comm::DPConfig, cmd_hack) == 15742,
              "DPConfig cmd_hack");
static_assert(offsetof(tss_dp_comm::DPProtoConfig, proto_check_switch) == 5,
              "proto_check_switch");
static_assert(offsetof(tss_dp_comm::DPProtoConfig, seq_check_switch) == 6,
              "seq_check_switch");
static_assert(offsetof(tss_dp_comm::DPProtoConfig, raw_check_switch) == 7,
              "raw_check_switch");
static_assert(offsetof(tss_dp_comm::DPTimeConfig, max_static_pkg_timeout) == 4,
              "max_static_pkg_timeout");
static_assert(offsetof(tss_dp_comm::DPTimeConfig, max_send_test_pkg_timeout) == 12,
              "max_send_test_pkg_timeout");
static_assert(offsetof(tss_dp_comm::DPTimeConfig, send_bin_file_gap) == 20,
              "send_bin_file_gap");
static_assert(offsetof(tss_dp_comm::DPTimeConfig, send_bin_code_gap) == 24,
              "send_bin_code_gap");

#endif // TSS_SDK_CHECK_TIMER_PROBE_TYPES_H
