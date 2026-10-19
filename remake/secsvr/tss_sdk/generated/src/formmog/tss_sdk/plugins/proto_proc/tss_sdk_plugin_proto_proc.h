// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_plugin_proto_proc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_TSS_SDK_PLUGIN_PROTO_PROC_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_TSS_SDK_PLUGIN_PROTO_PROC_H_H_

#include "src/formmog/tss_sdk/plugins/safemode/tss_sdk_plugin_safemode.h"

enum ._498 {
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG = 201,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG = 202,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG = 203,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST = 304,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST = 305,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ON_NOTIFY_SERVER = 306,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG_OLD = 307,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG_OLD = 308,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ANTIBOT_PKG = 309,
};
// sizeof = 130
struct ._499 { // line 92
public:
unsigned char result_;
unsigned char msg_len_;
char msg_[];
};
// sizeof = 84
union ._501 { // line 139
public:
._493 safemode_update_;
._492 safemode_ack_;
._494 limit_status_;
._500 append_param_;
._495 mba_safe_status_;
._497 mba_verify_info_;
._496 mba_launch_verify_;
._498 mba_verify_status_;
};
// sizeof = 32
struct ._502 { // line 57
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
struct ._503 { // line 70
public:
int notify_result_;
unsigned int main_id_;
const unsigned char *buf_;
unsigned int buf_size_;
};
// sizeof = 24
struct ._504 { // line 78
public:
int is_cheating_pkg_;
int cmd_id_;
unsigned int uin_;
unsigned int idx_;
const unsigned char *game_pkg_;
unsigned int game_pkg_len_;
};
// sizeof = 14
struct ._505 { // line 102
public:
unsigned int uin_;
unsigned int seq_;
short int verify_url_len_;
char *verify_url_;
};
// sizeof = 28
struct ._506 { // line 95
public:
int decrypt_result_;
unsigned int idx_;
unsigned int uin_;
const unsigned char *input_buf_;
unsigned int input_buf_size_;
unsigned char *output_buf_;
unsigned int output_buf_size_;
};
// sizeof = 16
struct ._507 { // line 106
public:
int cmd_id_;
unsigned int uin_;
unsigned int idx_;
int is_in_down_list_;
};
// sizeof = 32
union ._508 { // line 114
public:
._501 decrypt_list_pkg_;
._502 down_pkg_;
._503 notify_pkg_;
._504 anti_cheating_pkg_;
._505 antibot_pkg_;
._506 dp_pkg_;
._507 down_list_pkg_;
};
// sizeof = 84
union ._509 { // line 139
public:
._501 safemode_update_;
._500 safemode_ack_;
._502 limit_status_;
._508 append_param_;
._503 mba_safe_status_;
._505 mba_verify_info_;
._504 mba_launch_verify_;
._506 mba_verify_status_;
};
enum ._512 {
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CHEATING_PKG = 201,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG = 202,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG = 203,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_UP_LIST = 304,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_IS_CMD_IN_DOWN_LIST = 305,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ON_NOTIFY_SERVER = 306,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ENCRYPT_PKG_OLD = 307,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_DECRYPT_PKG_OLD = 308,
    TSS_SDK_PLUGIN_PROTO_PROC_MSG_ID_ANTIBOT_PKG = 309,
};
// sizeof = 24
struct ._518 { // line 78
public:
int is_cheating_pkg_;
int cmd_id_;
unsigned int uin_;
unsigned int idx_;
const unsigned char *game_pkg_;
unsigned int game_pkg_len_;
};
// sizeof = 12
struct ._519 { // line 88
public:
unsigned int uin_;
const unsigned char *buf_;
unsigned int buf_size_;
};
// sizeof = 28
struct ._520 { // line 95
public:
int decrypt_result_;
unsigned int idx_;
unsigned int uin_;
const unsigned char *input_buf_;
unsigned int input_buf_size_;
unsigned char *output_buf_;
unsigned int output_buf_size_;
};

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_TSS_SDK_PLUGIN_PROTO_PROC_H_H_
