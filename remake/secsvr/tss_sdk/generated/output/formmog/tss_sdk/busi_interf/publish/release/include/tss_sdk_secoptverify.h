// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/busi_interf/publish/release/include/tss_sdk_secoptverify.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_SECOPTVERIFY_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_SECOPTVERIFY_H_H_

#include "src/formmog/tss_sdk/plugins/uic/tss_sdk_plugin_uic.h"

typedef ._482 (**TssSdkSecOptVerifySetLimitInfo)(const ._486 *arg0);
typedef ._482 (**TssSdkSecOptVerifyAppendParam)(._488 *arg0);
typedef ._482 (**TssSdkSecOptVerifyUserOperate)(const ._489 &arg0);
typedef ._482 (**TssSdkSecOptSetUserSafeStatus)(const ._490 *arg0);
typedef ._482 (**TssSdkSecOptSetVerifyInfo)(const ._491 *arg0);
enum ._492 {
    RESULT_NO_LIMIT = 0,
    RESULT_VERIFY_PASSED = 1,
    RESULT_VERIFY_FORBIDDEN = 2,
};
// sizeof = 28
struct ._493 { // line 40
public:
unsigned int uin_;
._492 msg_result_flag_;
unsigned int message_len_;
unsigned char *message_;
short unsigned int callback_data_len_;
unsigned char *callback_data_;
._491 input_type_;
};
typedef ._482 (**TssSdkSecOptSetVerifyResult)(const ._493 *arg0);
// sizeof = 40
struct ._494 { // line 52
public:
unsigned int uin_;
._491 input_type_;
._492 msg_result_flag;
char if_replace_;
unsigned int message_len_;
unsigned char *message_;
short unsigned int callback_data_len_;
unsigned char *callback_data_;
short unsigned int game_data_len_;
unsigned char *game_data_;
};
extern const unsigned int TSS_SDK_MAX_URL_LENGTH; // line 18
extern const unsigned int TSS_SDK_MAX_GAME_PARAM_LENGTH; // line 19
extern const char TSS_SDK_SEC_OPT_VERIFY_PARAM_PROMPT[]; // line 53

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_SECOPTVERIFY_H_H_
