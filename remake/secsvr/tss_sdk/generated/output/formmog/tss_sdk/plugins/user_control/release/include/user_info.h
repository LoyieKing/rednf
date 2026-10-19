// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/plugins/user_control/release/include/user_info.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_USER_CONTROL_RELEASE_INCLUDE_USER_INFO_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_USER_CONTROL_RELEASE_INCLUDE_USER_INFO_H_H_

#include <time.h>

extern const int MAX_CLIENT_CODE; // line 76
extern const int MAX_ROLE_NAME; // line 78
extern const char E_MAX_DELAY_MODE; // line 179
extern const int MAX_BIN_FILE; // line 77
namespace tss_sdk_impl {
    namespace UserInfo {
        // sizeof = 100
        struct ._482 { // line 81
        public:
        unsigned int uin_;
        unsigned int game_id_;
        unsigned int room_id_;
        unsigned int client_ip_;
        unsigned int server_ip_;
        unsigned int game_usr_id_;
        unsigned int client_version_;
        unsigned int client_user_id_;
        unsigned int flags_;
        char role_name_[];
        };
        enum E_CLIENT_STATE {
            ECS_NOT_READY = 0,
            ECS_NOT_INIT = 1,
            ECS_INIT_MSG = 2,
            ECS_INIT_MSG2 = 3,
            ECS_INIT_ING = 4,
            ECS_INIT_CONFIRM = 5,
            ECS_INIT_FAILED = 6,
        };
        // sizeof = 48
        struct ._483 { // line 105
        public:
        tss_sdk_impl::UserInfo::E_CLIENT_STATE client_state_;
        unsigned int last_encrypt_sequence_;
        unsigned int last_game_decrypt_sequence_;
        unsigned int last_internal_decrypt_sequence_;
        unsigned int request_code_count_;
        short unsigned int server_cur_code_;
        short unsigned int server_new_code_;
        short unsigned int client_code_[];
        short unsigned int bin_file_code_;
        short unsigned int send_bin_file_count_;
        unsigned int down_cmd_list_crc_;
        short int up_cmd_list_pos_;
        short int down_cmd_list_pos_;
        };
        // sizeof = 20
        struct ._484 { // line 128
        public:
        time_t last_check_init_pkg_time_;
        time_t last_send_test_pkg_time_;
        time_t last_recv_stat_pkg_time_;
        time_t last_send_bin_code_time_;
        time_t last_send_bin_file_time_;
        };
        // sizeof = 52
        struct ._485 { // line 137
        public:
        unsigned int internal_msg_send_count_;
        unsigned int fix_encrypt_key_;
        unsigned int internal_fix_encrypt_count_;
        unsigned int fix_encrypt_count_;
        unsigned int dynamic_encrypt_count_;
        unsigned int internal_fix_decrypt_count_;
        unsigned int game_fix_decrypt_count_;
        unsigned int internal_dynamic_decrypt_count_;
        unsigned int game_dynamic_decrypt_count_;
        unsigned int antibot_last_seq_;
        unsigned int send_to_secsvr_seq_;
        unsigned int last_report_time_;
        unsigned int now_report_time_;
        };
        // sizeof = 128
        struct DPHackBanModeInfo { // line 165
        public:
        time_t keep_time_[];
        };
        // sizeof = 8
        struct DPHackCfUserInfo { // line 169
        public:
        unsigned char game_mode_;
        unsigned char role_posi_;
        unsigned char role_type_;
        unsigned char game_map_;
        unsigned char channel_id_;
        short unsigned int room_id_;
        };
        // sizeof = 3
        struct DPHackDelayCmd { // line 178
        public:
        static const char E_MAX_DELAY_MODE;
        unsigned char delay_mode_[];
        int set_flag(unsigned char arg0, unsigned char arg1); // line 182
        int clear_flag(unsigned char arg0, unsigned char arg1); // line 191
        };
        // sizeof = 180
        struct ._486 { // line 203
        public:
        unsigned int safemode_level_;
        unsigned int dnf_client_uid_;
        time_t drop_cmd_[];
        tss_sdk_impl::UserInfo::DPHackBanModeInfo ban_mode_;
        tss_sdk_impl::UserInfo::DPHackCfUserInfo cf_user_info_;
        tss_sdk_impl::UserInfo::DPHackDelayCmd delay_cmd_;
        unsigned char captcha_state_;
        };
        // sizeof = 100
        struct ._488 { // line 81
        public:
        unsigned int uin_;
        unsigned int game_id_;
        unsigned int room_id_;
        unsigned int client_ip_;
        unsigned int server_ip_;
        unsigned int game_usr_id_;
        unsigned int client_version_;
        unsigned int client_user_id_;
        unsigned int flags_;
        char role_name_[];
        };
        // sizeof = 48
        struct ._489 { // line 105
        public:
        tss_sdk_impl::UserInfo::E_CLIENT_STATE client_state_;
        unsigned int last_encrypt_sequence_;
        unsigned int last_game_decrypt_sequence_;
        unsigned int last_internal_decrypt_sequence_;
        unsigned int request_code_count_;
        short unsigned int server_cur_code_;
        short unsigned int server_new_code_;
        short unsigned int client_code_[];
        short unsigned int bin_file_code_;
        short unsigned int send_bin_file_count_;
        unsigned int down_cmd_list_crc_;
        short int up_cmd_list_pos_;
        short int down_cmd_list_pos_;
        };
        // sizeof = 20
        struct ._490 { // line 128
        public:
        time_t last_check_init_pkg_time_;
        time_t last_send_test_pkg_time_;
        time_t last_recv_stat_pkg_time_;
        time_t last_send_bin_code_time_;
        time_t last_send_bin_file_time_;
        };
        // sizeof = 52
        struct ._491 { // line 137
        public:
        unsigned int internal_msg_send_count_;
        unsigned int fix_encrypt_key_;
        unsigned int internal_fix_encrypt_count_;
        unsigned int fix_encrypt_count_;
        unsigned int dynamic_encrypt_count_;
        unsigned int internal_fix_decrypt_count_;
        unsigned int game_fix_decrypt_count_;
        unsigned int internal_dynamic_decrypt_count_;
        unsigned int game_dynamic_decrypt_count_;
        unsigned int antibot_last_seq_;
        unsigned int send_to_secsvr_seq_;
        unsigned int last_report_time_;
        unsigned int now_report_time_;
        };
        // sizeof = 180
        struct ._492 { // line 203
        public:
        unsigned int safemode_level_;
        unsigned int dnf_client_uid_;
        time_t drop_cmd_[];
        tss_sdk_impl::UserInfo::DPHackBanModeInfo ban_mode_;
        tss_sdk_impl::UserInfo::DPHackCfUserInfo cf_user_info_;
        tss_sdk_impl::UserInfo::DPHackDelayCmd delay_cmd_;
        unsigned char captcha_state_;
        };
        // sizeof = 100
        struct ._500 { // line 81
        public:
        unsigned int uin_;
        unsigned int game_id_;
        unsigned int room_id_;
        unsigned int client_ip_;
        unsigned int server_ip_;
        unsigned int game_usr_id_;
        unsigned int client_version_;
        unsigned int client_user_id_;
        unsigned int flags_;
        char role_name_[];
        };
        // sizeof = 48
        struct ._501 { // line 105
        public:
        tss_sdk_impl::UserInfo::E_CLIENT_STATE client_state_;
        unsigned int last_encrypt_sequence_;
        unsigned int last_game_decrypt_sequence_;
        unsigned int last_internal_decrypt_sequence_;
        unsigned int request_code_count_;
        short unsigned int server_cur_code_;
        short unsigned int server_new_code_;
        short unsigned int client_code_[];
        short unsigned int bin_file_code_;
        short unsigned int send_bin_file_count_;
        unsigned int down_cmd_list_crc_;
        short int up_cmd_list_pos_;
        short int down_cmd_list_pos_;
        };
        // sizeof = 20
        struct ._502 { // line 128
        public:
        time_t last_check_init_pkg_time_;
        time_t last_send_test_pkg_time_;
        time_t last_recv_stat_pkg_time_;
        time_t last_send_bin_code_time_;
        time_t last_send_bin_file_time_;
        };
        // sizeof = 52
        struct ._503 { // line 137
        public:
        unsigned int internal_msg_send_count_;
        unsigned int fix_encrypt_key_;
        unsigned int internal_fix_encrypt_count_;
        unsigned int fix_encrypt_count_;
        unsigned int dynamic_encrypt_count_;
        unsigned int internal_fix_decrypt_count_;
        unsigned int game_fix_decrypt_count_;
        unsigned int internal_dynamic_decrypt_count_;
        unsigned int game_dynamic_decrypt_count_;
        unsigned int antibot_last_seq_;
        unsigned int send_to_secsvr_seq_;
        unsigned int last_report_time_;
        unsigned int now_report_time_;
        };
        // sizeof = 180
        struct ._504 { // line 203
        public:
        unsigned int safemode_level_;
        unsigned int dnf_client_uid_;
        time_t drop_cmd_[];
        tss_sdk_impl::UserInfo::DPHackBanModeInfo ban_mode_;
        tss_sdk_impl::UserInfo::DPHackCfUserInfo cf_user_info_;
        tss_sdk_impl::UserInfo::DPHackDelayCmd delay_cmd_;
        unsigned char captcha_state_;
        };
        // sizeof = 100
        struct ._496 { // line 81
        public:
        unsigned int uin_;
        unsigned int game_id_;
        unsigned int room_id_;
        unsigned int client_ip_;
        unsigned int server_ip_;
        unsigned int game_usr_id_;
        unsigned int client_version_;
        unsigned int client_user_id_;
        unsigned int flags_;
        char role_name_[];
        };
        // sizeof = 48
        struct ._497 { // line 105
        public:
        tss_sdk_impl::UserInfo::E_CLIENT_STATE client_state_;
        unsigned int last_encrypt_sequence_;
        unsigned int last_game_decrypt_sequence_;
        unsigned int last_internal_decrypt_sequence_;
        unsigned int request_code_count_;
        short unsigned int server_cur_code_;
        short unsigned int server_new_code_;
        short unsigned int client_code_[];
        short unsigned int bin_file_code_;
        short unsigned int send_bin_file_count_;
        unsigned int down_cmd_list_crc_;
        short int up_cmd_list_pos_;
        short int down_cmd_list_pos_;
        };
        // sizeof = 20
        struct ._498 { // line 128
        public:
        time_t last_check_init_pkg_time_;
        time_t last_send_test_pkg_time_;
        time_t last_recv_stat_pkg_time_;
        time_t last_send_bin_code_time_;
        time_t last_send_bin_file_time_;
        };
        // sizeof = 52
        struct ._499 { // line 137
        public:
        unsigned int internal_msg_send_count_;
        unsigned int fix_encrypt_key_;
        unsigned int internal_fix_encrypt_count_;
        unsigned int fix_encrypt_count_;
        unsigned int dynamic_encrypt_count_;
        unsigned int internal_fix_decrypt_count_;
        unsigned int game_fix_decrypt_count_;
        unsigned int internal_dynamic_decrypt_count_;
        unsigned int game_dynamic_decrypt_count_;
        unsigned int antibot_last_seq_;
        unsigned int send_to_secsvr_seq_;
        unsigned int last_report_time_;
        unsigned int now_report_time_;
        };
    } // namespace UserInfo
} // namespace tss_sdk_impl

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_USER_CONTROL_RELEASE_INCLUDE_USER_INFO_H_H_
