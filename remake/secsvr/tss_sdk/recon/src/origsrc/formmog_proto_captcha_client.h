// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_captcha_client.cpp of
// libtss_sdk.so.1.0.394 (tag 394).
//
// Layout source: DWARF of the ORIG translation unit
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_captcha_client.cpp
// (cu_layout.py), cross-checked against the member offsets the ORIG
// pack()/unpack()/visualize() bodies touch:
//
//   TSSCaptchaHead  sizeof   16 : u16/u32/u32/u32/u16
//   TSSSetCaptcha   sizeof  531 : u32/u32/u16/u16/u32/u8/u16/u8[512]
//   TSSCaptchaPic   sizeof 4625 : u32/u16/u16/u16/u16/u16/u8[4096]/u8/u16/i8[512]
//   TSSVerifyReq    sizeof   65 : u8/u8[64]
//   TSSVerifyRes    sizeof  130 : u8/u8/i8[128]
//   TSSCaptchaBody  sizeof 4625 : anonymous union of the four above
//   TSSCaptchaPkg   sizeof 4641 : head_ at 0, body_ at 16
//
// Byte-packed: DWARF puts TSSSetCaptcha::platform_busid_ at +4 and
// TSSCaptchaPic::busi_id_ at +4110, which only holds with #pragma pack(1) -
// the packing the secsvr-generated formmog_proto_*.h headers use.
//
// NOTE on name reuse across the two captcha CUs: this header's TSSCaptchaHead/
// Body/Pkg/Pic and TSSVerifyReq/Res are distinct types from the ones in
// formmog_proto_captcha_client_old.h (which are named TSSCaptchaOldHead /
// TSSCaptchaOldBody / TSSCaptchaOldPkg / BusiCaptchaPic / BusiCaptchaReq /
// CaptchaReq / VerifyReq / VerifyRes / S2CVerifyRes / ReportLog /
// CaptchaBusiFinish).  Only TSSCaptchaHead (here) and TSSCaptchaOldHead (old)
// are related by name; their layouts differ and ORIG defines each in its own
// header.
//
// Codegen compensation, NOT an ORIG source attribute: the pack() declarations
// carry throw() so GCC 4.1.2 does not emit the spurious EH cleanup landing pad
// inside pack(char*, ...) that the ORIG (GCC 4.1.0) does not have.  Proof: no
// _Unwind_Resume reference exists in any pack(char*)/unpack(char const*)
// wrapper of this CU, while the visualize/visualize_ex(char*) wrappers - whose
// callees call the out-of-line TdrBufUtil::printVariable - do reference it.
// throw() is not part of the Itanium mangling, so all symbol names match the
// ORIG.  See AGENT_REPORT_genproto_captcha.md.
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_H_H_

#include <stddef.h>
#include <stdint.h>

#include "tsf4g_tdr_compat.h"

namespace sec_proto {

#pragma pack(push, 1)

class TSSCaptchaHead {
public:
    uint16_t msg_id_;
    uint32_t uin_;
    uint32_t busi_seq_;
    uint32_t seq_;
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

class TSSSetCaptcha {
public:
    uint32_t client_ip_;
    uint32_t platform_busid_;
    uint16_t max_pic_change_num_;
    uint16_t max_verify_num_;
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

class TSSCaptchaPic {
public:
    uint32_t timeout_;
    uint16_t verify_times_limit_;
    uint16_t verify_times_;
    uint16_t change_times_limit_;
    uint16_t change_times_;
    uint16_t picture_len_;
    uint8_t picture_[4096];
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

class TSSVerifyReq {
public:
    uint8_t answer_len_;
    uint8_t answer_[64];

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

class TSSVerifyRes {
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

// DWARF member of TSSCaptchaBody has no DW_AT_name (anonymous union), so the
// four arms are promoted into the class scope.  The union selector values are
// the protocol message ids the ORIG compares against: 3201 (0x0c81)
// set_captcha_, 3202 (0x0c82) captcha_pic_, 3203 (0x0c83) verify_req_,
// 3204 (0x0c84) captcha_res_.
class TSSCaptchaBody {
public:
    union {
        TSSSetCaptcha set_captcha_;
        TSSCaptchaPic captcha_pic_;
        TSSVerifyReq verify_req_;
        TSSVerifyRes captcha_res_;
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

class TSSCaptchaPkg {
public:
    TSSCaptchaHead head_;
    TSSCaptchaBody body_;

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
// form: a negative array bound fails the compile).  These are the values the
// ORIG pack()/unpack() code touches, so a packing regression in this header
// cannot go unnoticed.
typedef char tss_captcha_head_size_check[sizeof(TSSCaptchaHead) == 16 ? 1 : -1];
typedef char tss_captcha_head_uin_offset_check[
    offsetof(TSSCaptchaHead, uin_) == 2 ? 1 : -1];
typedef char tss_captcha_head_body_len_offset_check[
    offsetof(TSSCaptchaHead, body_len_) == 14 ? 1 : -1];
typedef char tss_set_captcha_size_check[sizeof(TSSSetCaptcha) == 531 ? 1 : -1];
typedef char tss_set_captcha_busi_id_offset_check[
    offsetof(TSSSetCaptcha, busi_id_) == 16 ? 1 : -1];
typedef char tss_set_captcha_busi_info_offset_check[
    offsetof(TSSSetCaptcha, busi_info_) == 19 ? 1 : -1];
typedef char tss_captcha_pic_size_check[sizeof(TSSCaptchaPic) == 4625 ? 1 : -1];
typedef char tss_captcha_pic_picture_offset_check[
    offsetof(TSSCaptchaPic, picture_) == 14 ? 1 : -1];
typedef char tss_captcha_pic_busi_id_offset_check[
    offsetof(TSSCaptchaPic, busi_id_) == 4110 ? 1 : -1];
typedef char tss_verify_req_size_check[sizeof(TSSVerifyReq) == 65 ? 1 : -1];
typedef char tss_verify_res_size_check[sizeof(TSSVerifyRes) == 130 ? 1 : -1];
typedef char tss_captcha_body_size_check[sizeof(TSSCaptchaBody) == 4625 ? 1 : -1];
typedef char tss_captcha_body_arm_offset_check[
    offsetof(TSSCaptchaBody, captcha_pic_) == 0 ? 1 : -1];
typedef char tss_captcha_pkg_size_check[sizeof(TSSCaptchaPkg) == 4641 ? 1 : -1];
typedef char tss_captcha_pkg_body_offset_check[
    offsetof(TSSCaptchaPkg, body_) == 16 ? 1 : -1];


#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_H_H_
