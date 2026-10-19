// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/plugins/captcha/release/include/tss_sdk_plugin_captcha.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_CAPTCHA_RELEASE_INCLUDE_TSS_SDK_PLUGIN_CAPTCHA_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_CAPTCHA_RELEASE_INCLUDE_TSS_SDK_PLUGIN_CAPTCHA_H_H_

// sizeof = 4625
struct ._497 { // line 70
public:
unsigned int timeout_;
short unsigned int verify_times_limit_;
short unsigned int verify_times_;
short unsigned int change_times_limit_;
short unsigned int change_times_;
short unsigned int picture_len_;
unsigned char picture_[];
unsigned char busi_id_;
short unsigned int busi_info_len_;
char busi_info_[];
};
// sizeof = 28
struct ._498 { // line 33
public:
unsigned int idx_;
unsigned int uin_;
unsigned int user_id_;
unsigned int client_ver_;
unsigned int server_ip_;
unsigned int client_ip_;
const char *player_name_;
};
// sizeof = 32
struct ._500 { // line 50
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

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_CAPTCHA_RELEASE_INCLUDE_TSS_SDK_PLUGIN_CAPTCHA_H_H_
