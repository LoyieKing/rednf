// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_public_safemodetransadapt.cpp of
// libtss_sdk.so.1.0.394 (tag 394).
//
// Layout source: DWARF of the ORIG translation unit
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_public_safemodetransadapt.cpp
// (cu_layout.py), cross-checked against the member offsets the ORIG code
// touches in pack()/unpack()/visualize():
//
//   ReqSafemodeTransAdaptLoginNotify   sizeof   14 : u16/u32/u32/u32
//   RspSafemodeTransAdaptStatUpdate    sizeof    7 : u16/u32/u8
//   ReqSafemodeTransAdaptEventNotify   sizeof   12 : u16/u32/u16/u32
//   ReqSafemodeTransAdaptTradeNotify   sizeof 2073 : u16/u32/u8/u32/u32/u32/u32/u16/u8[2048]
//   ReqSafemodeTransAdaptDataForward   sizeof 2056 : u16/u32/u16/u8[2048]
//
// The structures are byte-packed: DWARF puts ReqSafemodeTransAdaptLoginNotify::
// src_id_ at +2 (u32 right after a u16) and ReqSafemodeTransAdaptTradeNotify::
// send_uin_ at +7, which only holds with #pragma pack(1) - the packing the
// secsvr-generated formmog_proto_*.h headers use.
//
// Codegen compensation, NOT an ORIG source attribute: the pack() declarations
// below carry throw().  The ORIG was built by GCC 4.1.0, which knew the
// pack(TdrWriteBuf&, unsigned int) member cannot throw (its body only calls the
// inline TdrWriteBuf primitives), so its callers hold no EH cleanup region;
// the vendor 4.1.2 build emits a spurious landing pad for every caller that
// owns a local with a non-trivial destructor.  The ORIG's own code proves this:
// ReqSafemodeTransAdaptDataForward::pack(char*, unsigned int, unsigned int*,
// unsigned int) at 0x145f00 has no _Unwind_Resume reference (nor has any other
// pack(char*)/unpack(char const*...) wrapper in this CU), while the
// visualize/visualize_ex(char*) wrappers - whose callees call the out-of-line
// TdrBufUtil::printVariable - do carry one.  throw() is not part of the Itanium
// mangling for an empty exception specification, so every emitted symbol name
// stays identical to the ORIG.  See AGENT_REPORT_genproto_captcha.md.
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_SAFEMODETRANSADAPT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_SAFEMODETRANSADAPT_H_H_

#include <stddef.h>
#include <stdint.h>

#include "tsf4g_tdr_compat.h"

namespace sec_proto {

#pragma pack(push, 1)

class ReqSafemodeTransAdaptLoginNotify {
public:
    uint16_t world_id_;
    uint32_t src_id_;
    uint32_t client_ip_;
    uint32_t login_time_;

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

class RspSafemodeTransAdaptStatUpdate {
public:
    uint16_t world_id_;
    uint32_t src_id_;
    uint8_t stat_level_;

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

class ReqSafemodeTransAdaptEventNotify {
public:
    uint16_t world_id_;
    uint32_t src_id_;
    uint16_t event_type_;
    uint32_t event_value_;

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

class ReqSafemodeTransAdaptTradeNotify {
public:
    uint16_t world_id_;
    uint32_t src_id_;
    uint8_t type_;
    uint32_t send_uin_;
    uint32_t target_uin_;
    uint32_t send_ip_;
    uint32_t target_ip_;
    uint16_t buf_len_;
    uint8_t trade_buf_[2048];

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

class ReqSafemodeTransAdaptDataForward {
public:
    uint16_t world_id_;
    uint32_t src_id_;
    uint16_t data_len_;
    uint8_t data_[2048];

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
typedef char safemode_login_size_check[
    sizeof(ReqSafemodeTransAdaptLoginNotify) == 14 ? 1 : -1];
typedef char safemode_login_src_offset_check[
    offsetof(ReqSafemodeTransAdaptLoginNotify, src_id_) == 2 ? 1 : -1];
typedef char safemode_login_client_ip_offset_check[
    offsetof(ReqSafemodeTransAdaptLoginNotify, client_ip_) == 6 ? 1 : -1];
typedef char safemode_stat_size_check[
    sizeof(RspSafemodeTransAdaptStatUpdate) == 7 ? 1 : -1];
typedef char safemode_event_size_check[
    sizeof(ReqSafemodeTransAdaptEventNotify) == 12 ? 1 : -1];
typedef char safemode_trade_size_check[
    sizeof(ReqSafemodeTransAdaptTradeNotify) == 2073 ? 1 : -1];
typedef char safemode_trade_send_uin_offset_check[
    offsetof(ReqSafemodeTransAdaptTradeNotify, send_uin_) == 7 ? 1 : -1];
typedef char safemode_trade_buf_offset_check[
    offsetof(ReqSafemodeTransAdaptTradeNotify, trade_buf_) == 25 ? 1 : -1];
typedef char safemode_forward_size_check[
    sizeof(ReqSafemodeTransAdaptDataForward) == 2056 ? 1 : -1];
typedef char safemode_forward_data_offset_check[
    offsetof(ReqSafemodeTransAdaptDataForward, data_) == 8 ? 1 : -1];

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_SAFEMODETRANSADAPT_H_H_
