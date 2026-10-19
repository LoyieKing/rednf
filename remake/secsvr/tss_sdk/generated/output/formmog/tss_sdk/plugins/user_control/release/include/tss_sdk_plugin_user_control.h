// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/plugins/user_control/release/include/tss_sdk_plugin_user_control.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_USER_CONTROL_RELEASE_INCLUDE_TSS_SDK_PLUGIN_USER_CONTROL_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_USER_CONTROL_RELEASE_INCLUDE_TSS_SDK_PLUGIN_USER_CONTROL_H_H_

enum ._510 {
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER = 1,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER = 2,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER = 3,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO = 4,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN = 5,
};
// sizeof = 28
struct ._511 { // line 33
public:
unsigned int idx_;
unsigned int uin_;
unsigned int user_id_;
unsigned int client_ver_;
unsigned int server_ip_;
unsigned int client_ip_;
const char *player_name_;
};
// sizeof = 8
struct ._512 { // line 44
public:
unsigned int idx_;
unsigned int uin_;
};
// sizeof = 32
struct ._513 { // line 50
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
struct ._514 { // line 63
public:
unsigned int uin_;
unsigned int idx_;
tss_sdk_impl::UserInfo *user_info_;
};
// sizeof = 16
struct ._515 { // line 48
public:
int check_result_;
int cmd_id_;
unsigned int uin_;
unsigned int idx_;
};
// sizeof = 32
struct ._516 { // line 57
public:
int encrypt_result_;
int cmd_id_;
unsigned int uin_;
unsigned int idx_;
const unsigned char *input_buf_;
unsigned int input_buf_size_;
unsigned char *output_buf_;
unsigned int output_buf_size_;
};
// sizeof = 16
struct ._517 { // line 70
public:
int notify_result_;
unsigned int main_id_;
const unsigned char *buf_;
unsigned int buf_size_;
};
enum ._524 {
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER = 1,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER = 2,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER = 3,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO = 4,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN = 5,
};
enum ._497 {
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_ADD_USER = 1,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_DELETE_USER = 2,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_PUNISH_USER = 3,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO = 4,
    TSS_SDK_PLUGIN_USER_CONTROL_MSG_ID_GET_USERINFO_BY_UIN = 5,
};

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_USER_CONTROL_RELEASE_INCLUDE_TSS_SDK_PLUGIN_USER_CONTROL_H_H_
