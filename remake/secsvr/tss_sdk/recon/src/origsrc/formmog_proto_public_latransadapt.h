// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_public_latransadapt.cpp of
// libtss_sdk.so.1.0.394.
//
// Layout source: DWARF of the ORIG translation unit (cu_layout.py on CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_public_latransadapt.cpp
// ).  The generated header is not present in the sources available here, so
// every member name/type/offset below comes from the CU's own DWARF, and the
// compiler-inserted memset sizes in construct() cross-check every sizeof:
//
//   LATransAdaptDPData               32009 : 2 + 4 + 1 + 2 + 32000
//   LATransAdaptAntibotRPCodeResult  32006 : 4 + 2 + 32000
//   LATransAdaptAntibotErrMsg         1028 : 2 + 2 + 1024
//   LATransAdaptAntibotRoleName       1026 : 2 + 1024
//   LATransAdaptAntibotSigInfo       10252 : 2 + 4 + 4 + 2 + 10240
//   LATransAdaptAntibotClientIP         21 : 1 + 4 + 16
//   LATransAdaptAntibotRPFMC         10246 : 4 + 2 + 10240
//   LATransAdaptDPCmdMonitor            14 : 4 + 4 + 4 + 1 + 1
//   LATransAdaptDPCmdBufferMonitor   32786 : 4 + 4 + 4 + 1 + 1 + 4 + 32768
//
// Every member sits at the sum of its predecessors (DWARF offsets), i.e. the
// structs are byte-packed exactly like the other formmog_proto_*.h headers.
//
// Codegen compensation, NOT an ORIG source attribute: the pack()/unpack()
// declarations carry throw().  ORIG (GCC 4.1.0) FDEs for the pack(char*)/
// unpack(char*) wrappers contain no cleanup region (no _Unwind_Resume; only
// visualize/visualize_ex(char*) wrappers reference it), while the vendor
// 4.1.2 build adds a spurious EH landing pad for every wrapper that owns a
// TdrWriteBuf/TdrReadBuf local.  An empty exception specification does not
// take part in the Itanium mangling, so the emitted symbol names stay
// identical to the ORIG.  Same compensation as formmog_proto_tss_sdk_uic.h;
// see AGENT_REPORT_genproto_latrans.md.
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_LATRANSADAPT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_LATRANSADAPT_H_H_

#include <stdint.h>
#include <stddef.h>

#include "tsf4g_tdr_compat_latrans.h"

namespace sec_proto {

#pragma pack(push, 1)

struct LATransAdaptDPData {
    uint16_t room_id_;
    uint32_t time_stamp_;
    uint8_t dp_msg_id_;
    uint16_t data_len_;
    uint8_t data_[32000];

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

struct LATransAdaptAntibotRPCodeResult {
    uint32_t time_stamp_;
    uint16_t rpcode_result_data_len_;
    uint8_t rpcode_result_data_[32000];

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

struct LATransAdaptAntibotErrMsg {
    int16_t err_id_;
    uint16_t err_msg_len_;
    char err_msg_[1024];

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

struct LATransAdaptAntibotRoleName {
    uint16_t role_name_len_;
    char role_name_[1024];

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

struct LATransAdaptAntibotSigInfo {
    uint16_t total_size_;
    uint32_t ip_;
    uint32_t mac_id_;
    uint16_t sig_len_;
    uint8_t sig_[10240];

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

struct LATransAdaptAntibotClientIP {
    uint8_t flag_;
    uint32_t client_intranet_ip_;
    uint8_t client_key_signature_[16];

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

struct LATransAdaptAntibotRPFMC {
    uint32_t uin_;
    uint16_t result_len_;
    uint8_t result_[10240];

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

struct LATransAdaptDPCmdMonitor {
    uint32_t room_id_;
    uint32_t time_stamp_;
    int32_t monitor_cmd_id_;
    uint8_t is_need_encrypt_;
    uint8_t is_encrypted_;

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

struct LATransAdaptDPCmdBufferMonitor {
    uint32_t room_id_;
    uint32_t time_stamp_;
    int32_t monitor_cmd_id_;
    uint8_t is_need_encrypt_;
    uint8_t is_encrypted_;
    uint32_t data_len_;
    uint8_t data_[32768];

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

// Compile-time cross-check of the DWARF layout: sizes are the CU's DWARF
// DW_AT_byte_size values, also visible as the memset length constants of the
// ORIG construct() bodies; each offset is the first byte of the trailing
// array/last member, i.e. it only holds for the packed layout above.
typedef char latrans_assert_DPData_size[sizeof(LATransAdaptDPData) == 32009 ? 1 : -1];
typedef char latrans_assert_DPData_data_off[offsetof(LATransAdaptDPData, data_) == 9 ? 1 : -1];
typedef char latrans_assert_RPCodeResult_size[sizeof(LATransAdaptAntibotRPCodeResult) == 32006 ? 1 : -1];
typedef char latrans_assert_RPCodeResult_data_off[offsetof(LATransAdaptAntibotRPCodeResult, rpcode_result_data_) == 6 ? 1 : -1];
typedef char latrans_assert_ErrMsg_size[sizeof(LATransAdaptAntibotErrMsg) == 1028 ? 1 : -1];
typedef char latrans_assert_ErrMsg_msg_off[offsetof(LATransAdaptAntibotErrMsg, err_msg_) == 4 ? 1 : -1];
typedef char latrans_assert_RoleName_size[sizeof(LATransAdaptAntibotRoleName) == 1026 ? 1 : -1];
typedef char latrans_assert_RoleName_name_off[offsetof(LATransAdaptAntibotRoleName, role_name_) == 2 ? 1 : -1];
typedef char latrans_assert_SigInfo_size[sizeof(LATransAdaptAntibotSigInfo) == 10252 ? 1 : -1];
typedef char latrans_assert_SigInfo_sig_off[offsetof(LATransAdaptAntibotSigInfo, sig_) == 12 ? 1 : -1];
typedef char latrans_assert_ClientIP_size[sizeof(LATransAdaptAntibotClientIP) == 21 ? 1 : -1];
typedef char latrans_assert_ClientIP_sig_off[offsetof(LATransAdaptAntibotClientIP, client_key_signature_) == 5 ? 1 : -1];
typedef char latrans_assert_RPFMC_size[sizeof(LATransAdaptAntibotRPFMC) == 10246 ? 1 : -1];
typedef char latrans_assert_RPFMC_result_off[offsetof(LATransAdaptAntibotRPFMC, result_) == 6 ? 1 : -1];
typedef char latrans_assert_DPCmdMonitor_size[sizeof(LATransAdaptDPCmdMonitor) == 14 ? 1 : -1];
typedef char latrans_assert_DPCmdMonitor_enc_off[offsetof(LATransAdaptDPCmdMonitor, is_encrypted_) == 13 ? 1 : -1];
typedef char latrans_assert_DPCmdBufferMonitor_size[sizeof(LATransAdaptDPCmdBufferMonitor) == 32786 ? 1 : -1];
typedef char latrans_assert_DPCmdBufferMonitor_data_off[offsetof(LATransAdaptDPCmdBufferMonitor, data_) == 18 ? 1 : -1];

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_LATRANSADAPT_H_H_
