// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp of libtss_sdk.so.1.0.394.
//
// Layout source: DWARF of the ORIG translation unit (dwarf_full_extract.py /
// cu_layout.py on CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp
// ), not the lossy generated header: the generated header flattens the
// anonymous union of UserInputCheckPkgBody into a named member and turns the
// trailing arrays into flexible arrays.
//
//   UserInputCheckPkgHead  sizeof 10   : 5 x uint16_t
//   ReqUserInputCheck      sizeof 1169 : 4/4/4/1/2/uint8[1024]/2/uint8[128]
//   RspUserInputCheck      sizeof 16   : 4 x uint32_t
//   UserInputCheckPkgBody  sizeof 1169 : anonymous union of the two above
//   UserInputCheckPkg      sizeof 1179 : head_ at 0, body_ at 10
//
// The members are byte-packed: DWARF puts user_input_data_len_ at offset 13 and
// user_game_data_len_ at 1039, which only holds with #pragma pack(1) - the same
// packing the secsvr-generated formmog_proto_*.h headers use.
//
// Codegen compensation, NOT an ORIG source attribute: the pack()/unpack()
// declarations below carry throw().  The ORIG was built by GCC 4.1.0, which
// knew that these member functions cannot throw (their bodies only call the
// inline TdrWriteBuf/TdrReadBuf primitives); the vendor 4.1.2 build emits a
// spurious EH cleanup landing pad (31 bytes + alignment) for every caller that
// holds a local with a non-trivial destructor.  The ORIG's own FDEs prove
// there is no cleanup region for these calls: _Unwind_Resume is referenced
// only by the visualize/visualize_ex(char*) wrappers (whose callee does call
// the out-of-line TdrBufUtil::printVariable), never by the pack(char*)/
// unpack(char*) wrappers.  throw() is not part of the Itanium mangling for an
// empty exception specification, so every emitted symbol name stays identical
// to the ORIG, and it removes exactly the region the ORIG does not have.
// See AGENT_REPORT_genproto_uic.md ("divergences").
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_UIC_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_UIC_H_H_

#include <stdint.h>

#include "tsf4g_tdr_compat.h"

namespace sec_proto {

// Declared by the ORIG header at line 36 (DWARF DW_AT_decl_file/decl_line of
// the enumeration DIE); referenced by other CUs
// (UicTdirty::init(char*, unsigned int, UserInputCode)), not by this CU.
enum UserInputCode {
    USER_INPUT_CODE_GBK = 0,
    USER_INPUT_CODE_UTF8 = 1,
};

#pragma pack(push, 1)

class UserInputCheckPkgHead {
public:
    uint16_t proto_id_;
    uint16_t version_;
    uint16_t pkg_len_;
    uint16_t cmd_;
    uint16_t game_id_;

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

class ReqUserInputCheck {
public:
    uint32_t uin_;
    uint32_t session_id_;
    uint32_t session_seq_;
    int8_t user_input_code_;
    uint16_t user_input_data_len_;
    uint8_t user_input_data_[1024];
    uint16_t user_game_data_len_;
    uint8_t user_game_data_[128];

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

class RspUserInputCheck {
public:
    uint32_t uin_;
    uint32_t session_id_;
    uint32_t session_seq_;
    uint32_t check_result_;

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

// The DWARF member of UserInputCheckPkgBody has no DW_AT_name, i.e. the union
// is anonymous and promotes the two arms into the class scope.
class UserInputCheckPkgBody {
public:
    union {
        ReqUserInputCheck req_user_input_check_;
        RspUserInputCheck rsp_user_input_check_;
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
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector,
                                          const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector,
                                             tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector,
                                             char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const;
};

class UserInputCheckPkg {
public:
    UserInputCheckPkgHead head_;
    UserInputCheckPkgBody body_;

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

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_UIC_H_H_
