// Canonical layout of tss_sdk_impl::UserInfo (400 bytes) and its parts.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_sdk_impl::UserInfo'
//   struct tss_sdk_impl::UserInfo size=400 bases=0 vfuncs=0
//     +0    base_info_.uin_                             unsigned int      4
//     +4    base_info_.game_id_                         unsigned int      4
//     +8    base_info_.room_id_                         unsigned int      4
//     +12   base_info_.client_ip_                       unsigned int      4
//     +16   base_info_.server_ip_                       unsigned int      4
//     +20   base_info_.game_usr_id_                     unsigned int      4
//     +24   base_info_.client_version_                  unsigned int      4
//     +28   base_info_.client_user_id_                  unsigned int      4
//     +32   base_info_.flags_                           unsigned int      4
//     +36   base_info_.role_name_                       char[64]         64
//     +100  user_code_info_.client_state_               enum E_CLIENT_STATE 4
//     +104  user_code_info_.last_encrypt_sequence_      unsigned int      4
//     +108  user_code_info_.last_game_decrypt_sequence_ unsigned int      4
//     +112  user_code_info_.last_internal_decrypt_sequence_ unsigned int  4
//     +116  user_code_info_.request_code_count_         unsigned int      4
//     +120  user_code_info_.server_cur_code_            unsigned short    2
//     +122  user_code_info_.server_new_code_            unsigned short    2
//     +124  user_code_info_.client_code_                unsigned short[5] 10
//     +134  user_code_info_.bin_file_code_              unsigned short    2
//     +136  user_code_info_.send_bin_file_count_        unsigned short    2
//     +140  user_code_info_.down_cmd_list_crc_          unsigned int      4
//     +144  user_code_info_.up_cmd_list_pos_            short             2
//     +146  user_code_info_.down_cmd_list_pos_          short             2
//     +148  user_timer_.last_check_init_pkg_time_       time_t            4
//     +152  user_timer_.last_send_test_pkg_time_        time_t            4
//     +156  user_timer_.last_recv_stat_pkg_time_        time_t            4
//     +160  user_timer_.last_send_bin_code_time_        time_t            4
//     +164  user_timer_.last_send_bin_file_time_        time_t            4
//     +168..+219 user_stat_: 13 * unsigned int                            52
//     +220  hack_data_.safemode_level_                  unsigned int      4
//     +224  hack_data_.dnf_client_uid_                  unsigned int      4
//     +228  hack_data_.drop_cmd_                        time_t[8]        32
//     +260  hack_data_.ban_mode_                        UserInfo::DPHackBanModeInfo 128
//     +388  hack_data_.cf_user_info_                    UserInfo::DPHackCfUserInfo   8
//     +396  hack_data_.delay_cmd_                       UserInfo::DPHackDelayCmd     3
//     +399  hack_data_.captcha_state_                   unsigned char     1
//
//   python3 audit/tu_report.py --type 'tss_sdk_impl::UserInfo::DPHackBanModeInfo'
//     +0  keep_time_  time_t[32]  size=128
//   python3 audit/tu_report.py --type 'tss_sdk_impl::UserInfo::DPHackCfUserInfo'
//     +0 game_mode_ u8, +1 role_posi_ u8, +2 role_type_ u8, +3 game_map_ u8,
//     +4 channel_id_ u8, (1 byte padding) +6 room_id_ unsigned short   : 8
//   python3 audit/tu_report.py --type 'tss_sdk_impl::UserInfo::DPHackDelayCmd'
//     +0  delay_mode_  unsigned char[3]  size=3
//
// ORIG declares the five outer aggregates as anonymous structs inside
// UserInfo (their DIE names are empty, which is why the model prints the
// flattened leaf paths `base_info_.uin_` here but `base_info_` for the
// reconstruction).  This header keeps the named aggregates the existing
// translation units already use, and declares DPHackBanModeInfo /
// DPHackCfUserInfo / DPHackDelayCmd both at namespace scope and as nested
// type names of UserInfo, so `tss_sdk_impl::UserInfo::DPHackBanModeInfo`
// keeps resolving while the byte layout stays ORIG-identical.
//
// No enum is registered for E_CLIENT_STATE in the ORIG type model (its DIE is
// the unnamed `._NNN` form); the enumerator set below is the one the
// reconstruction already used and must not change the 4-byte encoding.
#ifndef TSS_SDK_LAYOUT_USER_INFO_H_
#define TSS_SDK_LAYOUT_USER_INFO_H_

#include <time.h>

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl {

enum E_CLIENT_STATE {
    ECS_NOT_READY = 0,
    ECS_NOT_INIT = 1,
    ECS_INIT_MSG = 2,
    ECS_INIT_MSG2 = 3,
    ECS_INIT_ING = 4,
    ECS_INIT_CONFIRM = 5,
    ECS_INIT_FAILED = 6,
};

// +0x00, 100 bytes.
struct UserInfoBaseInfo {
    unsigned int uin_;             // +0x00
    unsigned int game_id_;         // +0x04
    unsigned int room_id_;         // +0x08
    unsigned int client_ip_;       // +0x0c
    unsigned int server_ip_;       // +0x10
    unsigned int game_usr_id_;     // +0x14
    unsigned int client_version_;  // +0x18
    unsigned int client_user_id_;  // +0x1c
    unsigned int flags_;           // +0x20
    char role_name_[64];           // +0x24
};

// +0x64, 48 bytes.
struct UserInfoCodeInfo {
    E_CLIENT_STATE client_state_;                 // +0x00
    unsigned int last_encrypt_sequence_;          // +0x04
    unsigned int last_game_decrypt_sequence_;     // +0x08
    unsigned int last_internal_decrypt_sequence_; // +0x0c
    unsigned int request_code_count_;             // +0x10
    unsigned short server_cur_code_;              // +0x14
    unsigned short server_new_code_;              // +0x16
    unsigned short client_code_[5];               // +0x18
    unsigned short bin_file_code_;                // +0x22
    unsigned short send_bin_file_count_;          // +0x24
    unsigned int down_cmd_list_crc_;              // +0x28
    short up_cmd_list_pos_;                       // +0x2c
    short down_cmd_list_pos_;                     // +0x2e
};

// +0x94, 20 bytes.
struct UserInfoTimer {
    time_t last_check_init_pkg_time_;   // +0x00
    time_t last_send_test_pkg_time_;    // +0x04
    time_t last_recv_stat_pkg_time_;    // +0x08
    time_t last_send_bin_code_time_;    // +0x0c
    time_t last_send_bin_file_time_;    // +0x10
};

// +0xa8, 52 bytes.
struct UserInfoStat {
    unsigned int internal_msg_send_count_;         // +0x00
    unsigned int fix_encrypt_key_;                 // +0x04
    unsigned int internal_fix_encrypt_count_;      // +0x08
    unsigned int fix_encrypt_count_;               // +0x0c
    unsigned int dynamic_encrypt_count_;           // +0x10
    unsigned int internal_fix_decrypt_count_;      // +0x14
    unsigned int game_fix_decrypt_count_;          // +0x18
    unsigned int internal_dynamic_decrypt_count_;  // +0x1c
    unsigned int game_dynamic_decrypt_count_;      // +0x20
    unsigned int antibot_last_seq_;                // +0x24
    unsigned int send_to_secsvr_seq_;              // +0x28
    unsigned int last_report_time_;                // +0x2c
    unsigned int now_report_time_;                 // +0x30
};

// ORIG scopes this inside UserInfo; 128 bytes, one member.
struct DPHackBanModeInfo {
    time_t keep_time_[32];   // +0x00
};

// ORIG scopes this inside UserInfo; 8 bytes.  Byte +0x05 is compiler padding
// (the ORIG DIE has no member there), so no placeholder member is declared.
struct DPHackCfUserInfo {
    unsigned char game_mode_;            // +0x00
    unsigned char role_posi_;            // +0x01
    unsigned char role_type_;            // +0x02
    unsigned char game_map_;             // +0x03
    unsigned char channel_id_;           // +0x04
    unsigned short room_id_;             // +0x06
};

// ORIG scopes this inside UserInfo; 3 bytes.
struct DPHackDelayCmd {
    unsigned char delay_mode_[3];   // +0x00
};

// +0xdc, 180 bytes.
struct UserInfoHackData {
    unsigned int safemode_level_;        // +0x00
    unsigned int dnf_client_uid_;        // +0x04
    time_t drop_cmd_[8];                 // +0x08
    DPHackBanModeInfo ban_mode_;         // +0x28
    DPHackCfUserInfo cf_user_info_;      // +0xa8
    DPHackDelayCmd delay_cmd_;           // +0xb4
    unsigned char captcha_state_;        // +0xb7
};

// 400 bytes.
struct UserInfo {
    typedef tss_sdk_impl::DPHackBanModeInfo DPHackBanModeInfo;
    typedef tss_sdk_impl::DPHackCfUserInfo DPHackCfUserInfo;
    typedef tss_sdk_impl::DPHackDelayCmd DPHackDelayCmd;

    // ORIG user_info.h inline flag helpers.  No symbol is emitted for any of
    // them (`nm -C orig | grep check_user_flag` is empty); every caller
    // inlines the bit test / update, e.g. bill_cf_hackers_agent.cpp:100..108.
    bool check_user_flag(int flag) const {
        return (base_info_.flags_ & (unsigned int)flag) != 0;
    }
    void set_user_flag(int flag) { base_info_.flags_ |= (unsigned int)flag; }
    void clear_user_flag(int flag) {
        base_info_.flags_ &= ~(unsigned int)flag;
    }

    // ORIG user_info.h:46, inlined into every caller (no symbol).  A client is
    // considered initialised once the code handshake passed ECS_INIT_ING, or
    // as soon as either dynamic-decrypt counter moved.
    bool is_client_inited() const {
        return user_code_info_.client_state_ > ECS_INIT_ING ||
               user_stat_.internal_dynamic_decrypt_count_ != 0 ||
               user_stat_.game_dynamic_decrypt_count_ != 0;
    }

    // ORIG user_info.cpp: _ZN12tss_sdk_impl8UserInfo4initEjttl, declared and
    // defined out of line in the target.  The body memsets the whole 400-byte
    // object, sets game_id_/uin_/room_id_, the two cmd-list positions (-1)
    // and the three timer stamps; uic_manager_agent.cpp defines it here.
    int init(unsigned int uin, unsigned short game_id, unsigned short room_id,
             long now);

    UserInfoBaseInfo base_info_;       // +0x000, 100
    UserInfoCodeInfo user_code_info_;  // +0x064, 48
    UserInfoTimer user_timer_;         // +0x094, 20
    UserInfoStat user_stat_;           // +0x0a8, 52
    UserInfoHackData hack_data_;       // +0x0dc, 180
};

TSS_SDK_STATIC_ASSERT(sizeof(UserInfoBaseInfo) == 100,
                      userinfobaseinfo_size_must_be_100);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoBaseInfo, room_id_) == 8,
                      userinfobaseinfo_room_id_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoBaseInfo, role_name_) == 36,
                      userinfobaseinfo_role_name_offset);
TSS_SDK_STATIC_ASSERT(sizeof(UserInfoCodeInfo) == 48,
                      userinfocodeinfo_size_must_be_48);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoCodeInfo, client_code_) == 24,
                      userinfocodeinfo_client_code_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoCodeInfo, down_cmd_list_pos_) == 46,
                      userinfocodeinfo_down_cmd_list_pos_offset);
TSS_SDK_STATIC_ASSERT(sizeof(UserInfoTimer) == 20,
                      userinfotimer_size_must_be_20);
TSS_SDK_STATIC_ASSERT(sizeof(UserInfoStat) == 52,
                      userinfostat_size_must_be_52);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoStat, now_report_time_) == 48,
                      userinfostat_now_report_time_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackBanModeInfo) == 128,
                      dphackbanmodeinfo_size_must_be_128);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackCfUserInfo) == 8,
                      dphackcfuserinfo_size_must_be_8);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(DPHackCfUserInfo, room_id_) == 6,
                      dphackcfuserinfo_room_id_offset);
TSS_SDK_STATIC_ASSERT(sizeof(DPHackDelayCmd) == 3,
                      dphackdelaycmd_size_must_be_3);
TSS_SDK_STATIC_ASSERT(sizeof(UserInfoHackData) == 180,
                      userinfohackdata_size_must_be_180);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoHackData, ban_mode_) == 40,
                      userinfohackdata_ban_mode_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoHackData, cf_user_info_) == 168,
                      userinfohackdata_cf_user_info_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoHackData, delay_cmd_) == 176,
                      userinfohackdata_delay_cmd_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfoHackData, captcha_state_) == 179,
                      userinfohackdata_captcha_state_offset);
TSS_SDK_STATIC_ASSERT(sizeof(UserInfo) == 400, userinfo_size_must_be_400);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfo, user_code_info_) == 100,
                      userinfo_user_code_info_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfo, user_timer_) == 148,
                      userinfo_user_timer_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfo, user_stat_) == 168,
                      userinfo_user_stat_offset);
TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(UserInfo, hack_data_) == 220,
                      userinfo_hack_data_offset);

}  // namespace tss_sdk_impl

#endif  // TSS_SDK_LAYOUT_USER_INFO_H_
