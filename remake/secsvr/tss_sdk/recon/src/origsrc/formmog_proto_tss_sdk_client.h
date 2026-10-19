// Private protocol header reconstructed for
// src/protocol/formmog/formmog_proto_tss_sdk_client.cpp of
// libtss_sdk.so.1.0.394 (tag 394).
//
// Layout source: DWARF of the ORIG translation unit
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_tss_sdk_client.cpp
// (cu_layout.py), not the lossy generated header: the generated header flattens
// the anonymous unions of DPInterDataBody / TClientPkgBody / TClientFrameBody
// into a nested namespace and turns the trailing arrays into flexible arrays.
//
// Every member offset below is byte-packed as DWARF says (e.g. ClientPkgHead
// seq_ at +2, TSpecialControl cmdid at +5, DPClientInitDataPtc code_id at +32),
// which only holds with #pragma pack(1) - the packing the secs
// generator emits for this protocol family.
//
// The unions are anonymous (the DWARF union member of the class has no
// DW_AT_name), exactly as in the ORIG.
//
// Codegen compensation, NOT an ORIG source attribute: the pack()/unpack()
// declarations carry throw().  The ORIG was built by GCC 4.1.0 whose
// intra-TU analysis knew these member functions cannot throw (their bodies
// only call the inline TdrWriteBuf/TdrReadBuf primitives and other such
// members); the vendor 4.1.2 build emits a spurious EH cleanup landing pad
// (31 bytes + alignment) for every *wrapper* that holds a TdrWriteBuf/
// TdrReadBuf local and calls one of them.  Proof the ORIG has no such region:
// _Unwind_Resume is referenced only inside the visualize/visualize_ex(char*)
// wrappers of the CU (the ones calling the out-of-line
// TdrBufUtil::printVariable), never in the pack/unpack(char*) wrappers - e.g.
// DPClientCode::pack(char*) (0x0015deb0..0x0015df35) and
// DPClientCode::unpack(char*) (0x0015fb00..0x0015fb66) end in `ret` with no
// landing pad.  An empty exception specification is not part of the Itanium
// mangling, so every emitted symbol name stays identical to the ORIG, and the
// compensation removes exactly the region the ORIG does not have.
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_CLIENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_CLIENT_H_H_

#include <stddef.h>
#include <stdint.h>

#include "tsf4g_tdr_compat_client.h"

namespace sec_proto {

#pragma pack(push, 1)

class DPClientCode {
public:
    uint32_t code_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPBinCount {
public:
    uint16_t bin_count_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPBinFile {
public:
    uint16_t length;
    uint16_t bin_code_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPConfigData {
public:
    uint8_t proto_check_switch_;
    uint8_t seq_check_switch_;
    uint8_t raw_check_switch_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPDynamicAlgorithmHead {
public:
    uint32_t code_id_;
    uint32_t crc_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPFixAlgorithmHead {
public:
    uint32_t key_id_;
    uint32_t algorithm_type_;
    uint32_t crc;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPClientDecryptInfo {
public:
    uint32_t decrypt_id_;
    uint32_t encrypt_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DpClientShortCmdData {
public:
    uint16_t cmd_id_;
    uint16_t cmd_count_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPClientCmdData {
public:
    uint32_t cmd_id_;
    uint32_t cmd_count_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPClientLogPtc {
public:
    uint32_t error_id_;
    uint8_t is_encrypt_;
    uint8_t error_msg_len_;
    uint8_t error_msg_[128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPProtocolRequestCode {
public:
    uint16_t size;
    uint32_t encrypt_code_id_;
    uint32_t decrypt_code_id_;
    uint16_t bin_file_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPInterDataHead {
public:
    uint16_t packet_size_;
    uint16_t protocol_type_;
    uint8_t data_type_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TClientFrameHead {
public:
    uint32_t packetLen;
    uint16_t totalSize;
    uint16_t protoId;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TSafeMode {
public:
    uint32_t iSafemodeLevel;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TSpecialControl {
public:
    uint16_t shPacketSize;
    uint16_t shPacketType;
    uint8_t chDataType;
    uint32_t cmdid;
    uint32_t bufLen;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TReviveMouse {
public:
    uint32_t cmdID;
    uint32_t isHint;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class ClientPkgHead {
public:
    uint16_t protocol_id_;
    uint32_t seq_;
    uint16_t sln_id_;
    uint16_t sub_sln_id_;
    uint16_t body_len_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPClientInitDataPtc {
public:
    uint8_t code_ver_[16];
    uint32_t client_info_;
    uint32_t client_user_id_;
    uint32_t client_code_id_;
    uint32_t server_code_id_;
    uint16_t bin_code_id_;
    uint16_t bin_count_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TClientFramePiece {
public:
    uint16_t totalPiece;
    uint16_t pieceSerial;
    uint32_t totalCrc;
    uint16_t pieceDataSize;
    uint8_t pieceData[51200];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPCmdList {
public:
    uint16_t up_cmd_count_;
    uint16_t down_cmd_count_;
    uint32_t up_cmd_list_[128];
    uint32_t down_cmd_list_[128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPServerCode {
public:
    uint8_t client_version_[16];
    uint32_t code_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPClientStatPtc {
public:
    uint32_t internal_msg_send_count_;
    uint32_t internal_msg_recv_count_;
    uint32_t game_pkg_recv_count_;
    uint32_t fix_encrypt_count_;
    uint32_t game_fix_decrypt_count_;
    uint32_t game_dynamic_decrypt_count_;
    uint32_t seq_less_count_;
    uint32_t seq_greater_count_;
    uint32_t seq_discard_count_;
    uint32_t down_cmd_list_crc_;
    uint16_t need_encrypt_count_;
    DPClientCmdData need_encrypt_list_[128];
    uint16_t client_error_count_;
    DpClientShortCmdData client_error_list_[128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPAlgorithmData {
public:
    uint32_t usr_id_crc_;
    uint32_t custom_data_;
    uint8_t pad_len_;
    uint8_t pad_data_[256];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class DPInterDataBody {
public:
    union {
        DPClientCode client_code_;
        DPDynamicAlgorithmHead dynamic_algorithm_data_;
        DPBinFile bin_file_;
        DPCmdList cmd_list;
        DPBinCount bin_count_;
        DPServerCode server_code_;
        DPProtocolRequestCode request_code_;
        DPClientInitDataPtc init_data_ptc_;
        DPClientStatPtc client_stat_ptc_;
        DPClientDecryptInfo decrypt_info_;
        DPClientLogPtc client_log_ptc_;
        uint8_t cs_test_data_[10];
        uint8_t sc_test_data_[10];
        DPFixAlgorithmHead fix_algorithm_data_;
        DPConfigData config_data_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t unionSelector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t unionSelector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(int64_t unionSelector,
                                        char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t unionSelector,
                                          tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t unionSelector,
                                          const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t unionSelector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        int indent,
                                        char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t unionSelector,
                                        char* buffer,
                                        unsigned int size,
                                        unsigned int* usedSize,
                                        int indent,
                                        char separator) const;
};

class TClientFrameBody {
public:
    union {
        TClientFramePiece framePiece;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t unionSelector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t unionSelector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(int64_t unionSelector,
                                        char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t unionSelector,
                                          tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t unionSelector,
                                          const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t unionSelector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        int indent,
                                        char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t unionSelector,
                                        char* buffer,
                                        unsigned int size,
                                        unsigned int* usedSize,
                                        int indent,
                                        char separator) const;
};

class TClientPkgBody {
public:
    union {
        TReviveMouse reviseMouse;
        TSpecialControl specialControl;
        TSafeMode safeMode;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t unionSelector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t unionSelector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(int64_t unionSelector,
                                        char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t unionSelector,
                                          tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t unionSelector,
                                          const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t unionSelector,
                                        tsf4g_tdr::TdrWriteBuf& destBuf,
                                        int indent,
                                        char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t unionSelector,
                                        char* buffer,
                                        unsigned int size,
                                        unsigned int* usedSize,
                                        int indent,
                                        char separator) const;
};

class DPInterData {
public:
    DPInterDataHead head;
    DPInterDataBody body;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TClientFrame {
public:
    TClientFrameHead head;
    TClientFrameBody body;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

class TClientPkg {
public:
    ClientPkgHead head;
    TClientPkgBody body;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw();
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw();
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf,
                                             int indent,
                                             char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent,
                             char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size,
                             unsigned int* usedSize, int indent,
                             char separator) const;
};

// ---------------------------------------------------------------------
// Packed-layout assertions. Every constant is the ORIG DWARF
// (cu_layout.py / dwarf_type_decl.py) value; a compile error here means the
// parallel-array/heap-first layout was NOT reproduced.
// ---------------------------------------------------------------------
typedef char check_size_DPClientCode[sizeof(DPClientCode) == 4 ? 1 : -1];
typedef char check_off_DPClientCode_code_id_[offsetof(DPClientCode, code_id_) == 0 ? 1 : -1];
typedef char check_size_DPBinCount[sizeof(DPBinCount) == 2 ? 1 : -1];
typedef char check_off_DPBinCount_bin_count_[offsetof(DPBinCount, bin_count_) == 0 ? 1 : -1];
typedef char check_size_DPBinFile[sizeof(DPBinFile) == 4 ? 1 : -1];
typedef char check_off_DPBinFile_length[offsetof(DPBinFile, length) == 0 ? 1 : -1];
typedef char check_off_DPBinFile_bin_code_id_[offsetof(DPBinFile, bin_code_id_) == 2 ? 1 : -1];
typedef char check_size_DPConfigData[sizeof(DPConfigData) == 3 ? 1 : -1];
typedef char check_off_DPConfigData_proto_check_switch_[offsetof(DPConfigData, proto_check_switch_) == 0 ? 1 : -1];
typedef char check_off_DPConfigData_seq_check_switch_[offsetof(DPConfigData, seq_check_switch_) == 1 ? 1 : -1];
typedef char check_off_DPConfigData_raw_check_switch_[offsetof(DPConfigData, raw_check_switch_) == 2 ? 1 : -1];
typedef char check_size_DPDynamicAlgorithmHead[sizeof(DPDynamicAlgorithmHead) == 8 ? 1 : -1];
typedef char check_off_DPDynamicAlgorithmHead_code_id_[offsetof(DPDynamicAlgorithmHead, code_id_) == 0 ? 1 : -1];
typedef char check_off_DPDynamicAlgorithmHead_crc_[offsetof(DPDynamicAlgorithmHead, crc_) == 4 ? 1 : -1];
typedef char check_size_DPFixAlgorithmHead[sizeof(DPFixAlgorithmHead) == 12 ? 1 : -1];
typedef char check_off_DPFixAlgorithmHead_key_id_[offsetof(DPFixAlgorithmHead, key_id_) == 0 ? 1 : -1];
typedef char check_off_DPFixAlgorithmHead_algorithm_type_[offsetof(DPFixAlgorithmHead, algorithm_type_) == 4 ? 1 : -1];
typedef char check_off_DPFixAlgorithmHead_crc[offsetof(DPFixAlgorithmHead, crc) == 8 ? 1 : -1];
typedef char check_size_DPClientDecryptInfo[sizeof(DPClientDecryptInfo) == 8 ? 1 : -1];
typedef char check_off_DPClientDecryptInfo_decrypt_id_[offsetof(DPClientDecryptInfo, decrypt_id_) == 0 ? 1 : -1];
typedef char check_off_DPClientDecryptInfo_encrypt_id_[offsetof(DPClientDecryptInfo, encrypt_id_) == 4 ? 1 : -1];
typedef char check_size_DpClientShortCmdData[sizeof(DpClientShortCmdData) == 4 ? 1 : -1];
typedef char check_off_DpClientShortCmdData_cmd_id_[offsetof(DpClientShortCmdData, cmd_id_) == 0 ? 1 : -1];
typedef char check_off_DpClientShortCmdData_cmd_count_[offsetof(DpClientShortCmdData, cmd_count_) == 2 ? 1 : -1];
typedef char check_size_DPClientCmdData[sizeof(DPClientCmdData) == 8 ? 1 : -1];
typedef char check_off_DPClientCmdData_cmd_id_[offsetof(DPClientCmdData, cmd_id_) == 0 ? 1 : -1];
typedef char check_off_DPClientCmdData_cmd_count_[offsetof(DPClientCmdData, cmd_count_) == 4 ? 1 : -1];
typedef char check_size_DPClientLogPtc[sizeof(DPClientLogPtc) == 134 ? 1 : -1];
typedef char check_off_DPClientLogPtc_error_id_[offsetof(DPClientLogPtc, error_id_) == 0 ? 1 : -1];
typedef char check_off_DPClientLogPtc_is_encrypt_[offsetof(DPClientLogPtc, is_encrypt_) == 4 ? 1 : -1];
typedef char check_off_DPClientLogPtc_error_msg_len_[offsetof(DPClientLogPtc, error_msg_len_) == 5 ? 1 : -1];
typedef char check_off_DPClientLogPtc_error_msg_[offsetof(DPClientLogPtc, error_msg_) == 6 ? 1 : -1];
typedef char check_size_DPProtocolRequestCode[sizeof(DPProtocolRequestCode) == 12 ? 1 : -1];
typedef char check_off_DPProtocolRequestCode_size[offsetof(DPProtocolRequestCode, size) == 0 ? 1 : -1];
typedef char check_off_DPProtocolRequestCode_encrypt_code_id_[offsetof(DPProtocolRequestCode, encrypt_code_id_) == 2 ? 1 : -1];
typedef char check_off_DPProtocolRequestCode_decrypt_code_id_[offsetof(DPProtocolRequestCode, decrypt_code_id_) == 6 ? 1 : -1];
typedef char check_off_DPProtocolRequestCode_bin_file_id_[offsetof(DPProtocolRequestCode, bin_file_id_) == 10 ? 1 : -1];
typedef char check_size_DPInterDataHead[sizeof(DPInterDataHead) == 5 ? 1 : -1];
typedef char check_off_DPInterDataHead_packet_size_[offsetof(DPInterDataHead, packet_size_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataHead_protocol_type_[offsetof(DPInterDataHead, protocol_type_) == 2 ? 1 : -1];
typedef char check_off_DPInterDataHead_data_type_[offsetof(DPInterDataHead, data_type_) == 4 ? 1 : -1];
typedef char check_size_TClientFrameHead[sizeof(TClientFrameHead) == 8 ? 1 : -1];
typedef char check_off_TClientFrameHead_packetLen[offsetof(TClientFrameHead, packetLen) == 0 ? 1 : -1];
typedef char check_off_TClientFrameHead_totalSize[offsetof(TClientFrameHead, totalSize) == 4 ? 1 : -1];
typedef char check_off_TClientFrameHead_protoId[offsetof(TClientFrameHead, protoId) == 6 ? 1 : -1];
typedef char check_size_TSafeMode[sizeof(TSafeMode) == 4 ? 1 : -1];
typedef char check_off_TSafeMode_iSafemodeLevel[offsetof(TSafeMode, iSafemodeLevel) == 0 ? 1 : -1];
typedef char check_size_TSpecialControl[sizeof(TSpecialControl) == 13 ? 1 : -1];
typedef char check_off_TSpecialControl_shPacketSize[offsetof(TSpecialControl, shPacketSize) == 0 ? 1 : -1];
typedef char check_off_TSpecialControl_shPacketType[offsetof(TSpecialControl, shPacketType) == 2 ? 1 : -1];
typedef char check_off_TSpecialControl_chDataType[offsetof(TSpecialControl, chDataType) == 4 ? 1 : -1];
typedef char check_off_TSpecialControl_cmdid[offsetof(TSpecialControl, cmdid) == 5 ? 1 : -1];
typedef char check_off_TSpecialControl_bufLen[offsetof(TSpecialControl, bufLen) == 9 ? 1 : -1];
typedef char check_size_TReviveMouse[sizeof(TReviveMouse) == 8 ? 1 : -1];
typedef char check_off_TReviveMouse_cmdID[offsetof(TReviveMouse, cmdID) == 0 ? 1 : -1];
typedef char check_off_TReviveMouse_isHint[offsetof(TReviveMouse, isHint) == 4 ? 1 : -1];
typedef char check_size_ClientPkgHead[sizeof(ClientPkgHead) == 12 ? 1 : -1];
typedef char check_off_ClientPkgHead_protocol_id_[offsetof(ClientPkgHead, protocol_id_) == 0 ? 1 : -1];
typedef char check_off_ClientPkgHead_seq_[offsetof(ClientPkgHead, seq_) == 2 ? 1 : -1];
typedef char check_off_ClientPkgHead_sln_id_[offsetof(ClientPkgHead, sln_id_) == 6 ? 1 : -1];
typedef char check_off_ClientPkgHead_sub_sln_id_[offsetof(ClientPkgHead, sub_sln_id_) == 8 ? 1 : -1];
typedef char check_off_ClientPkgHead_body_len_[offsetof(ClientPkgHead, body_len_) == 10 ? 1 : -1];
typedef char check_size_DPClientInitDataPtc[sizeof(DPClientInitDataPtc) == 36 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_code_ver_[offsetof(DPClientInitDataPtc, code_ver_) == 0 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_client_info_[offsetof(DPClientInitDataPtc, client_info_) == 16 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_client_user_id_[offsetof(DPClientInitDataPtc, client_user_id_) == 20 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_client_code_id_[offsetof(DPClientInitDataPtc, client_code_id_) == 24 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_server_code_id_[offsetof(DPClientInitDataPtc, server_code_id_) == 28 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_bin_code_id_[offsetof(DPClientInitDataPtc, bin_code_id_) == 32 ? 1 : -1];
typedef char check_off_DPClientInitDataPtc_bin_count_[offsetof(DPClientInitDataPtc, bin_count_) == 34 ? 1 : -1];
typedef char check_size_TClientFramePiece[sizeof(TClientFramePiece) == 51210 ? 1 : -1];
typedef char check_off_TClientFramePiece_totalPiece[offsetof(TClientFramePiece, totalPiece) == 0 ? 1 : -1];
typedef char check_off_TClientFramePiece_pieceSerial[offsetof(TClientFramePiece, pieceSerial) == 2 ? 1 : -1];
typedef char check_off_TClientFramePiece_totalCrc[offsetof(TClientFramePiece, totalCrc) == 4 ? 1 : -1];
typedef char check_off_TClientFramePiece_pieceDataSize[offsetof(TClientFramePiece, pieceDataSize) == 8 ? 1 : -1];
typedef char check_off_TClientFramePiece_pieceData[offsetof(TClientFramePiece, pieceData) == 10 ? 1 : -1];
typedef char check_size_DPCmdList[sizeof(DPCmdList) == 1028 ? 1 : -1];
typedef char check_off_DPCmdList_up_cmd_count_[offsetof(DPCmdList, up_cmd_count_) == 0 ? 1 : -1];
typedef char check_off_DPCmdList_down_cmd_count_[offsetof(DPCmdList, down_cmd_count_) == 2 ? 1 : -1];
typedef char check_off_DPCmdList_up_cmd_list_[offsetof(DPCmdList, up_cmd_list_) == 4 ? 1 : -1];
typedef char check_off_DPCmdList_down_cmd_list_[offsetof(DPCmdList, down_cmd_list_) == 516 ? 1 : -1];
typedef char check_size_DPServerCode[sizeof(DPServerCode) == 20 ? 1 : -1];
typedef char check_off_DPServerCode_client_version_[offsetof(DPServerCode, client_version_) == 0 ? 1 : -1];
typedef char check_off_DPServerCode_code_id_[offsetof(DPServerCode, code_id_) == 16 ? 1 : -1];
typedef char check_size_DPClientStatPtc[sizeof(DPClientStatPtc) == 1580 ? 1 : -1];
typedef char check_off_DPClientStatPtc_internal_msg_send_count_[offsetof(DPClientStatPtc, internal_msg_send_count_) == 0 ? 1 : -1];
typedef char check_off_DPClientStatPtc_internal_msg_recv_count_[offsetof(DPClientStatPtc, internal_msg_recv_count_) == 4 ? 1 : -1];
typedef char check_off_DPClientStatPtc_game_pkg_recv_count_[offsetof(DPClientStatPtc, game_pkg_recv_count_) == 8 ? 1 : -1];
typedef char check_off_DPClientStatPtc_fix_encrypt_count_[offsetof(DPClientStatPtc, fix_encrypt_count_) == 12 ? 1 : -1];
typedef char check_off_DPClientStatPtc_game_fix_decrypt_count_[offsetof(DPClientStatPtc, game_fix_decrypt_count_) == 16 ? 1 : -1];
typedef char check_off_DPClientStatPtc_game_dynamic_decrypt_count_[offsetof(DPClientStatPtc, game_dynamic_decrypt_count_) == 20 ? 1 : -1];
typedef char check_off_DPClientStatPtc_seq_less_count_[offsetof(DPClientStatPtc, seq_less_count_) == 24 ? 1 : -1];
typedef char check_off_DPClientStatPtc_seq_greater_count_[offsetof(DPClientStatPtc, seq_greater_count_) == 28 ? 1 : -1];
typedef char check_off_DPClientStatPtc_seq_discard_count_[offsetof(DPClientStatPtc, seq_discard_count_) == 32 ? 1 : -1];
typedef char check_off_DPClientStatPtc_down_cmd_list_crc_[offsetof(DPClientStatPtc, down_cmd_list_crc_) == 36 ? 1 : -1];
typedef char check_off_DPClientStatPtc_need_encrypt_count_[offsetof(DPClientStatPtc, need_encrypt_count_) == 40 ? 1 : -1];
typedef char check_off_DPClientStatPtc_need_encrypt_list_[offsetof(DPClientStatPtc, need_encrypt_list_) == 42 ? 1 : -1];
typedef char check_off_DPClientStatPtc_client_error_count_[offsetof(DPClientStatPtc, client_error_count_) == 1066 ? 1 : -1];
typedef char check_off_DPClientStatPtc_client_error_list_[offsetof(DPClientStatPtc, client_error_list_) == 1068 ? 1 : -1];
typedef char check_size_DPAlgorithmData[sizeof(DPAlgorithmData) == 265 ? 1 : -1];
typedef char check_off_DPAlgorithmData_usr_id_crc_[offsetof(DPAlgorithmData, usr_id_crc_) == 0 ? 1 : -1];
typedef char check_off_DPAlgorithmData_custom_data_[offsetof(DPAlgorithmData, custom_data_) == 4 ? 1 : -1];
typedef char check_off_DPAlgorithmData_pad_len_[offsetof(DPAlgorithmData, pad_len_) == 8 ? 1 : -1];
typedef char check_off_DPAlgorithmData_pad_data_[offsetof(DPAlgorithmData, pad_data_) == 9 ? 1 : -1];
typedef char check_size_DPInterDataBody[sizeof(DPInterDataBody) == 1580 ? 1 : -1];
typedef char check_off_DPInterDataBody_client_code_[offsetof(DPInterDataBody, client_code_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_dynamic_algorithm_data_[offsetof(DPInterDataBody, dynamic_algorithm_data_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_bin_file_[offsetof(DPInterDataBody, bin_file_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_cmd_list[offsetof(DPInterDataBody, cmd_list) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_bin_count_[offsetof(DPInterDataBody, bin_count_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_server_code_[offsetof(DPInterDataBody, server_code_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_request_code_[offsetof(DPInterDataBody, request_code_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_init_data_ptc_[offsetof(DPInterDataBody, init_data_ptc_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_client_stat_ptc_[offsetof(DPInterDataBody, client_stat_ptc_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_decrypt_info_[offsetof(DPInterDataBody, decrypt_info_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_client_log_ptc_[offsetof(DPInterDataBody, client_log_ptc_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_cs_test_data_[offsetof(DPInterDataBody, cs_test_data_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_sc_test_data_[offsetof(DPInterDataBody, sc_test_data_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_fix_algorithm_data_[offsetof(DPInterDataBody, fix_algorithm_data_) == 0 ? 1 : -1];
typedef char check_off_DPInterDataBody_config_data_[offsetof(DPInterDataBody, config_data_) == 0 ? 1 : -1];
typedef char check_size_TClientFrameBody[sizeof(TClientFrameBody) == 51210 ? 1 : -1];
typedef char check_off_TClientFrameBody_framePiece[offsetof(TClientFrameBody, framePiece) == 0 ? 1 : -1];
typedef char check_size_TClientPkgBody[sizeof(TClientPkgBody) == 13 ? 1 : -1];
typedef char check_off_TClientPkgBody_reviseMouse[offsetof(TClientPkgBody, reviseMouse) == 0 ? 1 : -1];
typedef char check_off_TClientPkgBody_specialControl[offsetof(TClientPkgBody, specialControl) == 0 ? 1 : -1];
typedef char check_off_TClientPkgBody_safeMode[offsetof(TClientPkgBody, safeMode) == 0 ? 1 : -1];
typedef char check_size_DPInterData[sizeof(DPInterData) == 1585 ? 1 : -1];
typedef char check_off_DPInterData_head[offsetof(DPInterData, head) == 0 ? 1 : -1];
typedef char check_off_DPInterData_body[offsetof(DPInterData, body) == 5 ? 1 : -1];
typedef char check_size_TClientFrame[sizeof(TClientFrame) == 51218 ? 1 : -1];
typedef char check_off_TClientFrame_head[offsetof(TClientFrame, head) == 0 ? 1 : -1];
typedef char check_off_TClientFrame_body[offsetof(TClientFrame, body) == 8 ? 1 : -1];
typedef char check_size_TClientPkg[sizeof(TClientPkg) == 25 ? 1 : -1];
typedef char check_off_TClientPkg_head[offsetof(TClientPkg, head) == 0 ? 1 : -1];
typedef char check_off_TClientPkg_body[offsetof(TClientPkg, body) == 12 ? 1 : -1];

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_CLIENT_H_H_
