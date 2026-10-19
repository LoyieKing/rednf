// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_public_uictransadapt.cpp of
// libtss_sdk.so.1.0.394.
//
// Layout source: DWARF of the ORIG translation unit (cu_layout.py on CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_public_uictransadapt.cpp
// ), not a generated header:
//
//   ReqUICTransAdaptMsgForwardPkg sizeof 33044
//     : 2/4/4/1/1/2/uint8[32768]/2/uint8[256]/4
//     (world_id_ 0, src_id_ 2, cmd_id_ 6, need_encrypt_ 10, is_encrypted_ 11,
//      cmd_buf_len_ 12, cmd_buf_ 14, rolename_len_ 32782, rolename_ 32784,
//      client_ip_ 33040)
//
// The members are byte-packed with #pragma pack(1): DWARF puts src_id_ at
// offset 2 and cmd_id_ at offset 6, which only holds with the packing the
// secsvr-generated formmog_proto_*.h headers use.
//
// Codegen compensation, NOT an ORIG source attribute: the pack()/unpack()
// declarations below carry throw().  The ORIG was built by GCC 4.1.0, which
// knew these member functions cannot throw (their bodies only call the inline
// TdrWriteBuf/TdrReadBuf primitives), while the vendor 4.1.2 build emits a
// spurious EH cleanup landing pad for every caller that holds a local with a
// non-trivial destructor.  The ORIG's own FDEs prove there is no cleanup
// region for these calls: in this CU _Unwind_Resume is referenced exactly
// twice, both inside the two visualize/visualize_ex(char*) wrappers (whose
// callee does call the out-of-line TdrBufUtil::printVariable), while the FDEs
// of pack(char*) / unpack(char*) carry no LSDA.  throw() is not part of the
// Itanium mangling for an empty exception specification, so every emitted
// symbol name stays identical to the ORIG, and it removes exactly the region
// the ORIG does not have.  See AGENT_REPORT_genproto_qt.md ("divergences").
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_UICTRANSADAPT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_UICTRANSADAPT_H_H_

#include <stdint.h>

#include "tsf4g_tdr_compat.h"

namespace sec_proto {

#pragma pack(push, 1)

class ReqUICTransAdaptMsgForwardPkg {
public:
    uint16_t world_id_;
    uint32_t src_id_;
    uint32_t cmd_id_;
    uint8_t need_encrypt_;
    uint8_t is_encrypted_;
    uint16_t cmd_buf_len_;
    uint8_t cmd_buf_[32768];
    uint16_t rolename_len_;
    uint8_t rolename_[256];
    uint32_t client_ip_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
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

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_UICTRANSADAPT_H_H_
