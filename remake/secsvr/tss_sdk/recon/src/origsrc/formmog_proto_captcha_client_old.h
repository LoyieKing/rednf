// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_captcha_client_old.cpp of
// libtss_sdk.so.1.0.394 (tag 394).
//
// Layout source: DWARF of the ORIG translation unit
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_captcha_client_old.cpp
// (cu_layout.py), cross-checked against the member offsets the ORIG
// pack()/unpack()/visualize() bodies touch:
//
//   TSSCaptchaOldHead  sizeof   24 : u16/u16/u32/u32/u32/u32/u16/u16
//   CaptchaReq         sizeof   24 : u32/u32/u32/u16/u16/u32/u32
//   VerifyReq          sizeof  267 : u8/u8[64]/u16/u8[200]
//   VerifyRes          sizeof  142 : u32/u32/u32/u8/u8/i8[128]
//   S2CVerifyRes       sizeof  130 : u8/u8/i8[128]
//   ReportLog          sizeof  518 : u16/u16/u16/i8[512]
//   BusiCaptchaReq     sizeof  539 : u32/u32/u32/u16/u16/u32/u32/u8/u16/i8[512]
//   CaptchaBusiFinish  sizeof  527 : u32/u32/u32/u8/u16/i8[512]
//   BusiCaptchaPic     sizeof 4827 : u32/u16*5/u8[4096]/u16/u8[200]/u8/u16/i8[512]
//   TSSCaptchaOldBody  sizeof 4827 : anonymous union of the eight above
//   TSSCaptchaOldPkg   sizeof 4851 : head_ at 0, body_ at 24
//
// Byte-packed: DWARF puts TSSCaptchaOldHead::version_ at +2 and
// BusiCaptchaReq::busi_info_len_ at +25, which only holds with
// #pragma pack(1) - the packing the secsvr-generated formmog_proto_*.h headers
// use.
//
// NOTE on name reuse across the two captcha CUs: ORIG defines TSSCaptchaHead,
// TSSCaptchaBody, TSSCaptchaPkg and TSSCaptchaPic in
// formmog_proto_captcha_client.cpp (this library's "new" captcha protocol) and
// the *Old-suffixed plus unprefixed CaptchaReq/VerifyReq/VerifyRes/
// BusiCaptchaReq/CaptchaBusiFinish/BusiCaptchaPic/ReportLog/S2CVerifyRes in
// this "old" protocol file.  No type name is defined in both CUs; the two
// header sets therefore do not collide.  The new CU's TSSCaptchaPic
// (u32/u16*5/u8[4096]/u8/u16/i8[512], sizeof 4625) and this CU's
// BusiCaptchaPic (adds session_len_/session_, sizeof 4827) are different
// message bodies with different message ids (3202 vs 3090).
//
// Codegen compensation, NOT an ORIG source attribute: the pack() declarations
// carry throw(), the same compensation the uic pilot documents.  The ORIG
// (GCC 4.1.0) leaves no EH cleanup region in pack(char*, ...)/unpack(char
// const*, ...), because those wrappers' only throwing call is the nothrow
// pack(TdrWriteBuf&, unsigned int); the vendor 4.1.2 build inserts a spurious
// landing pad.  Measured proof for this CU: no _Unwind_Resume reference inside
// any pack/unpack wrapper, while the visualize/visualize_ex(char*) wrappers do
// reference it.  throw() does not participate in Itanium mangling, so all
// emitted symbol names match the ORIG.  See AGENT_REPORT_genproto_captcha.md.
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_OLD_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_OLD_H_H_

#include <stddef.h>
#include <stdint.h>

#include "tsf4g_tdr_compat.h"

namespace sec_proto {

#pragma pack(push, 1)

class TSSCaptchaOldHead {
public:
    uint16_t msg_id_;
    uint16_t version_;
    uint32_t uin_;
    uint32_t client_ip_;
    uint32_t seq_;
    uint32_t send_time_;
    uint16_t head_len_;
    uint16_t body_len_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class CaptchaReq {
public:
    uint32_t app_id_;
    uint32_t antibot_bus_id_;
    uint32_t zone_id_;
    uint16_t max_pic_change_num_;
    uint16_t max_verify_num_;
    uint32_t op_type_;
    uint32_t timeout_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class VerifyReq {
public:
    uint8_t code_len_;
    uint8_t code_[64];
    uint16_t session_len_;
    uint8_t session_[200];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class VerifyRes {
public:
    uint32_t app_id_;
    uint32_t antibot_bus_id_;
    uint32_t zone_id_;
    uint8_t result_;
    uint8_t msg_len_;
    int8_t msg_[128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class S2CVerifyRes {
public:
    uint8_t result_;
    uint8_t msg_len_;
    int8_t msg_[128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class ReportLog {
public:
    uint16_t log_src_;
    uint16_t log_type_;
    uint16_t log_len_;
    int8_t log_msg_[512];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class BusiCaptchaReq {
public:
    uint32_t app_id_;
    uint32_t antibot_bus_id_;
    uint32_t zone_id_;
    uint16_t max_pic_change_num_;
    uint16_t max_verify_num_;
    uint32_t op_type_;
    uint32_t timeout_;
    uint8_t busi_id_;
    uint16_t busi_info_len_;
    int8_t busi_info_[512];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class CaptchaBusiFinish {
public:
    uint32_t app_id_;
    uint32_t antibot_bus_id_;
    uint32_t zone_id_;
    uint8_t busi_id_;
    uint16_t busi_info_len_;
    int8_t busi_info_[512];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class BusiCaptchaPic {
public:
    uint32_t timeout_;
    uint16_t verify_times_limit_;
    uint16_t verify_times_;
    uint16_t max_change_times_;
    uint16_t change_times_;
    uint16_t picture_len_;
    uint8_t picture_[4096];
    uint16_t session_len_;
    uint8_t session_[200];
    uint8_t busi_id_;
    uint16_t busi_info_len_;
    int8_t busi_info_[512];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

// The eight-way union.  DWARF gives no DW_AT_name for the member, so the arms
// are promoted into the class scope.  Selector values (the protocol message
// ids the ORIG compares against): 1 busi_captcha_req_, 3073 captcha_req_,
// 3075 verify_req_, 3076 verify_res_, 3078 s2c_verify_res_, 3090
// busi_captcha_pic_, 3091 report_log_, 3092 captcha_busi_finish_.
class TSSCaptchaOldBody {
public:
    union {
        BusiCaptchaReq busi_captcha_req_;
        CaptchaReq captcha_req_;
        VerifyReq verify_req_;
        VerifyRes verify_res_;
        S2CVerifyRes s2c_verify_res_;
        BusiCaptchaPic busi_captcha_pic_;
        ReportLog report_log_;
        CaptchaBusiFinish captcha_busi_finish_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer,
                                        unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector,
                                          tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector,
                                          const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector,
                                             tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector,
                                             char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
};

class TSSCaptchaOldPkg {
public:
    TSSCaptchaOldHead head_;
    TSSCaptchaOldBody body_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};
// Compile-time layout assertions against the ORIG DWARF sizes/offsets (C++03
// form: a negative array bound fails the compile).
typedef char tss_old_head_size_check[sizeof(TSSCaptchaOldHead) == 24 ? 1 : -1];
typedef char tss_old_head_version_offset_check[
    offsetof(TSSCaptchaOldHead, version_) == 2 ? 1 : -1];
typedef char tss_old_head_head_len_offset_check[
    offsetof(TSSCaptchaOldHead, head_len_) == 20 ? 1 : -1];
typedef char tss_old_head_body_len_offset_check[
    offsetof(TSSCaptchaOldHead, body_len_) == 22 ? 1 : -1];
typedef char captcha_req_size_check[sizeof(CaptchaReq) == 24 ? 1 : -1];
typedef char verify_req_size_check[sizeof(VerifyReq) == 267 ? 1 : -1];
typedef char verify_req_session_offset_check[
    offsetof(VerifyReq, session_) == 67 ? 1 : -1];
typedef char verify_res_size_check[sizeof(VerifyRes) == 142 ? 1 : -1];
typedef char s2c_verify_res_size_check[sizeof(S2CVerifyRes) == 130 ? 1 : -1];
typedef char report_log_size_check[sizeof(ReportLog) == 518 ? 1 : -1];
typedef char busi_captcha_req_size_check[sizeof(BusiCaptchaReq) == 539 ? 1 : -1];
typedef char busi_captcha_req_busi_info_offset_check[
    offsetof(BusiCaptchaReq, busi_info_) == 27 ? 1 : -1];
typedef char captcha_busi_finish_size_check[
    sizeof(CaptchaBusiFinish) == 527 ? 1 : -1];
typedef char busi_captcha_pic_size_check[sizeof(BusiCaptchaPic) == 4827 ? 1 : -1];
typedef char busi_captcha_pic_picture_offset_check[
    offsetof(BusiCaptchaPic, picture_) == 14 ? 1 : -1];
typedef char busi_captcha_pic_session_offset_check[
    offsetof(BusiCaptchaPic, session_) == 4112 ? 1 : -1];
typedef char busi_captcha_pic_busi_info_offset_check[
    offsetof(BusiCaptchaPic, busi_info_) == 4315 ? 1 : -1];
typedef char tss_old_body_size_check[sizeof(TSSCaptchaOldBody) == 4827 ? 1 : -1];
typedef char tss_old_body_arm_offset_check[
    offsetof(TSSCaptchaOldBody, busi_captcha_pic_) == 0 ? 1 : -1];
typedef char tss_old_pkg_size_check[sizeof(TSSCaptchaOldPkg) == 4851 ? 1 : -1];
typedef char tss_old_pkg_body_offset_check[
    offsetof(TSSCaptchaOldPkg, body_) == 24 ? 1 : -1];

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_OLD_H_H_
