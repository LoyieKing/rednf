// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/busi_interf/publish/release/include/tss_sdk_captcha.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_CAPTCHA_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_CAPTCHA_H_H_

enum ._485 {
    TSS_SDK_CAPTCHA_RESULT_VERIFY_OK = 0,
    TSS_SDK_CAPTCHA_RESULT_VERIFY_FAILED = 1,
    TSS_SDK_CAPTCHA_RESULT_CLIENT_TIMEOUT = 2,
    TSS_SDK_CAPTCHA_RESULT_SVR_TIMEOUT = 3,
    TSS_SDK_CAPTCHA_RESULT_MANY_CHANGE_TIMES = 5,
    TSS_SDK_CAPTCHA_RESULT_VERIFY_ALL_FAILED = 6,
    TSS_SDK_CAPTCHA_RESULT_CLIENT_CANCELED = 7,
    TSS_SDK_CAPTCHA_RESULT_AREA_UNCOVERED = 8,
};
typedef ._482 (**TssSdkCaptchaStart)(const ._486 *arg0);
typedef ._482 (**TssSdkCaptchaChangePic)(const ._488 *arg0);
typedef ._482 (**TssSdkCaptchaVerify)(const ._489 *arg0);
typedef ._482 (**TssSdkCaptchaRecvPicData)(const ._487 *arg0);
typedef ._482 (**TssSdkCaptchaVerifyRsp)(const ._490 *arg0);

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_BUSI_INTERF_PUBLISH_RELEASE_INCLUDE_TSS_SDK_CAPTCHA_H_H_
