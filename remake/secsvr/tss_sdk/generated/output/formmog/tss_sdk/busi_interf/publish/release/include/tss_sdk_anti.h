// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/busi_interf/publish/release/include/tss_sdk_anti.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_ANTI_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_ANTI_H_H_

#include "src/formmog/tss_sdk/plugins/uic/tss_sdk_plugin_uic.h"

// sizeof = 16
struct ._485 { // line 22
public:
unsigned int uin_;
unsigned int client_ver_;
unsigned int client_ip_;
const char *role_name_;
};
// sizeof = 27
struct ._486 { // line 40
public:
unsigned int uin_;
unsigned int req_seq_;
unsigned int client_ip_;
short unsigned int max_pic_change_num_;
short unsigned int max_verify_num_;
unsigned int timeout_;
unsigned char captcha_type_;
short unsigned int busi_info_len_;
const unsigned char *busi_info_;
};
// sizeof = 37
struct ._487 { // line 57
public:
unsigned int uin_;
unsigned int req_seq_;
unsigned int captcha_seq_;
unsigned int timeout_;
short unsigned int verify_times_limit_;
short unsigned int verify_times_;
short unsigned int change_times_limit_;
short unsigned int change_times_;
short unsigned int picture_len_;
const unsigned char *picture_;
unsigned char captcha_type_;
short unsigned int busi_info_len_;
const unsigned char *busi_info_;
};
// sizeof = 16
struct ._488 { // line 61
public:
unsigned int uin_;
unsigned int cmd_id_;
const unsigned char *game_pkg_;
unsigned int game_pkg_len_;
};
// sizeof = 28
struct ._490 { // line 40
public:
unsigned int uin_;
._489 msg_result_flag_;
unsigned int message_len_;
unsigned char *message_;
short unsigned int callback_data_len_;
unsigned char *callback_data_;
._488 input_type_;
};
enum ._491 {
    PUNISH_KICK_CHANNEL = 2,
    PUNISH_KICK_CLIENT = 5,
    PUNISH_VERIFY_CAPTCHA = 9,
    PUNISH_MODIFY_ROOM_NAME = 10,
    PUNISH_SHOW_TIPS = 11,
};
// sizeof = 32
struct ._492 { // line 57
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
enum ._493 {
    TSS_SDK_ANTI_ENCRYPT_OK = 0,
    TSS_SDK_ANTI_NOT_NEED_ENCRYPT = 1,
};
enum ._494 {
    TSS_SDK_ANTI_DECRYPT_OK = 0,
    TSS_SDK_ANTI_INTERNAL_PKG = 1,
};
typedef ._482 (**TssSdkAddUser)(._485 *arg0);
typedef ._482 (**TssSdkDelUser)(const ._486 *arg0);
typedef int (**TssSdkIsCheatPkg)(const ._488 *arg0);
typedef ._494 (**TssSdkDecryptPkg)(._487 *arg0);
typedef ._493 (**TssSdkEncryptPkg)(._489 *arg0);
typedef ._482 (**TssSdkSendDataToClient)(const ._490 *arg0);
typedef ._482 (**TssSdkPunishUser)(const ._492 *arg0);
// sizeof = 531
struct ._496 { // line 57
public:
unsigned int client_ip_;
unsigned int platform_busid_;
short unsigned int max_pic_change_num_;
short unsigned int max_verify_num_;
unsigned int timeout_;
unsigned char busi_id_;
short unsigned int busi_info_len_;
char busi_info_[];
};

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_ANTI_H_H_
