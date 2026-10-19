// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/safemode/tss_sdk_plugin_safemode.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_SAFEMODE_TSS_SDK_PLUGIN_SAFEMODE_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_SAFEMODE_TSS_SDK_PLUGIN_SAFEMODE_H_H_

#include "src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_plugin_proto_proc.h"

// sizeof = 16
struct ._521 { // line 106
public:
int cmd_id_;
unsigned int uin_;
unsigned int idx_;
int is_in_down_list_;
};
// sizeof = 32
union ._522 { // line 114
public:
._515 decrypt_list_pkg_;
._516 down_pkg_;
._517 notify_pkg_;
._518 anti_cheating_pkg_;
._519 antibot_pkg_;
._520 dp_pkg_;
._521 down_list_pkg_;
};
// sizeof = 38
struct ._523 { // line 68
public:
short unsigned int ipc_days_;
signed char safemode_level_;
signed char has_verify_mb_;
short unsigned int event_type_;
signed char reserve_[];
};
// sizeof = 20
struct ._524 { // line 77
public:
int ret_;
unsigned int uin_;
int limit_status_;
unsigned int unlock_url_len_;
char *unlock_url_;
};
// sizeof = 28
struct ._525 { // line 33
public:
unsigned int idx_;
unsigned int uin_;
unsigned int user_id_;
unsigned int client_ver_;
unsigned int server_ip_;
unsigned int client_ip_;
const char *player_name_;
};
// sizeof = 84
struct ._526 { // line 92
public:
unsigned int uin_;
unsigned int seq_;
unsigned int op_level_;
unsigned int op_scene_len_;
char op_scene_[];
unsigned int client_ip_;
};
// sizeof = 32
struct ._527 { // line 50
public:
unsigned int uin_;
short unsigned int game_id_;
short unsigned int room_id_;
int punish_mode_;
int punish_time_;
unsigned int punish_info_len_;
const unsigned char *punish_info_;
unsigned int punish_reason_len_;
const unsigned char *punish_reason_;
};
// sizeof = 12
struct ._528 { // line 110
public:
unsigned int uin_;
unsigned int seq_;
unsigned int result_;
};
// sizeof = 8
struct ._529 { // line 126
public:
char *param_key_;
char *param_value_;
};
// sizeof = 16
struct ._530 { // line 132
public:
char *url_;
unsigned int url_buf_max_len_;
unsigned int param_count_;
._529 *param_;
};
// sizeof = 84
union ._531 { // line 139
public:
._523 safemode_update_;
._522 safemode_ack_;
._524 limit_status_;
._530 append_param_;
._525 mba_safe_status_;
._527 mba_verify_info_;
._526 mba_launch_verify_;
._528 mba_verify_status_;
};
// sizeof = 90
struct ._532 { // line 151
public:
._521 head_;
._531 body_;
};
// sizeof = 90
struct ._510 { // line 151
public:
._499 head_;
._509 body_;
};
extern const char TSS_SDK_SAFEMODE_PLUGIN_PROMPT_PARAM_KEY[]; // line 117
extern const char TSS_SDK_SAFEMODE_PLUGIN_VERIFY_PARAM_PROMPT[]; // line 118
extern const char TSS_SDK_SAFEMODE_PLUGIN_VERIFY_PARAM_PROMPT_TEST_1[]; // line 119
extern const char TSS_SDK_SAFEMODE_PLUGIN_VERIFY_PARAM_PROMPT_TEST_2[]; // line 120
extern const char TSS_SDK_SAFEMODE_PLUGIN_ENCODE_DECODE_KEY[]; // line 121
extern const char TSS_SDK_SAFEMODE_PLUGIN_VERIFY_CGI[]; // line 123

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_SAFEMODE_TSS_SDK_PLUGIN_SAFEMODE_H_H_
