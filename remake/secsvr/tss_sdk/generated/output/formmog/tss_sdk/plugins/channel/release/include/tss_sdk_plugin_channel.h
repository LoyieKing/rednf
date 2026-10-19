// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/plugins/channel/release/include/tss_sdk_plugin_channel.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_CHANNEL_RELEASE_INCLUDE_TSS_SDK_PLUGIN_CHANNEL_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_CHANNEL_RELEASE_INCLUDE_TSS_SDK_PLUGIN_CHANNEL_H_H_

#include "src/formmog/tss_sdk/plugins/uic/tss_sdk_plugin_uic.h"

// sizeof = 28
struct ._489 { // line 33
public:
unsigned int idx_;
unsigned int uin_;
unsigned int user_id_;
unsigned int client_ver_;
unsigned int server_ip_;
unsigned int client_ip_;
const char *player_name_;
};
// sizeof = 16
struct ._495 { // line 47
public:
short unsigned int msg_id_;
unsigned int uin_;
unsigned int busi_seq_;
unsigned int seq_;
short unsigned int body_len_;
};
// sizeof = 40
struct ._491 { // line 52
public:
unsigned int uin_;
._488 input_type_;
._489 msg_result_flag;
char if_replace_;
unsigned int message_len_;
unsigned char *message_;
short unsigned int callback_data_len_;
unsigned char *callback_data_;
short unsigned int game_data_len_;
unsigned char *game_data_;
};
enum ._489 {
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_IPC_CHANNEL = 1,
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_SOCKET_CHANNEL = 2,
    TSS_SDK_PLUGIN_CHANNEL_TYPE_ID_GAME_CHANNEL = 3,
};

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_PLUGINS_CHANNEL_RELEASE_INCLUDE_TSS_SDK_PLUGIN_CHANNEL_H_H_
