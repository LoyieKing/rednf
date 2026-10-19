// Supplemental reconstruction for src/protocol/formmog/formmog_proto_public_secsvr.cpp
// (tag 394, libtss_sdk.so.1.0.394).
//
// The CU has 199 DWARF-attributed functions; 154 of them (including the weak
// tsf4g_tdr::TdrWriteBuf::textize) are already provided by the reused object
//   reuse/obj/src__protocol__formmog__formmog_proto_public_secsvr.o
// (source dnf_secsvr/source/secagent/src/protocol/formmog/formmog_proto_public_secsvr.cpp),
// verified with cu_match.sh:
//   cu=199 obj=201 matched=154
//   "in CU, absent from object" = exactly the 45 symbols implemented here
//   (9 methods x 5 classes: SecsvrCaptchaReq, SecsvrCaptchaResultRsp,
//    SecsvrGreenUnionRsp, SecsvrImInfoNotifyReq, SecsvrUserInfoNotifyReq).
// This file intentionally defines NOTHING else: no other class of the CU and no
// textize (the reused object already carries that weak symbol).
//
// Layout source: DWARF of the ORIG CU (cu_layout.py), cross-checked against the
// construct() memset sizes in the ORIG machine code:
//   SecsvrCaptchaReq              541 : 2+2+4+2+4+4+2+2+4+1+2+512
//   SecsvrCaptchaResultRsp        534 : 4+2+2+4+4+4+1+1+512
//   SecsvrGreenUnionRsp             5 : 1+1+2+1
//   SecsvrImInfoNotifyReq          38 : 2+2+2+32
//   SecsvrUserInfoNotifyReq      2056 : 2+2+4+2048
// every member sits at the sum of its predecessors, i.e. #pragma pack(1).
//
// The "ORIG .cpp:NNN" comments are DW_AT_call_line values of the inlined
// tsf4g_tdr calls (dwarf_inline_names.py), i.e. the ORIG's own call sequence.
//
// None of the pack()/unpack() functions of these five classes looks at cutVer:
// the argument is never read from the stack (checked function by function).
//
// Codegen compensation shared with the other formmog reconstructions: the
// pack()/unpack() declarations carry throw() so the vendor 4.1.2 build does not
// add the EH cleanup landing pad the ORIG (4.1.0) never emitted for the
// pack(char*)/unpack(char*) wrappers (the ORIG references _Unwind_Resume only
// from visualize/visualize_ex(char*) wrappers).  An empty exception
// specification does not take part in the Itanium mangling, so all names stay
// identical to the ORIG.

#include <stddef.h>
#include <string.h>

#include "tsf4g_tdr_compat.h"

namespace sec_proto {

#pragma pack(push, 1)

struct SecsvrCaptchaReq {
    uint16_t channel_id_;
    uint16_t room_id_;
    uint32_t version_;
    uint16_t bus_id_;
    uint32_t time_stamp_;
    uint32_t client_ip_;
    uint16_t max_pic_change_num_;
    uint16_t max_verify_num_;
    uint32_t timeout_;
    uint8_t busi_id_;
    uint16_t busi_info_len_;
    uint8_t busi_info_[512];

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

struct SecsvrCaptchaResultRsp {
    uint32_t version_;
    uint16_t room_id_;
    uint16_t bus_id_;
    uint32_t time_stamp_;
    uint32_t client_ip_;
    uint32_t app_id_;
    uint8_t result_;
    uint8_t msg_len_;
    uint8_t msg_[512];

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

struct SecsvrGreenUnionRsp {
    uint8_t result_;
    uint8_t flag_;
    uint16_t world_;
    uint8_t start_src_;

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

struct SecsvrImInfoNotifyReq {
    uint16_t msg_type_id_;
    uint16_t world_id_;
    uint16_t nick_name_len_;
    uint8_t nick_name_buf_[32];

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

struct SecsvrUserInfoNotifyReq {
    uint16_t msg_type_id_;
    uint16_t world_id_;
    uint32_t data_len_;
    uint8_t data_[2048];

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
// DW_AT_byte_size values, also visible as the ORIG construct() memset length
// constants; each offset pins the packed layout of the trailing array/member.
typedef char svx_assert_CaptchaReq_size[sizeof(SecsvrCaptchaReq) == 541 ? 1 : -1];
typedef char svx_assert_CaptchaReq_info_off[offsetof(SecsvrCaptchaReq, busi_info_) == 29 ? 1 : -1];
typedef char svx_assert_CaptchaResultRsp_size[sizeof(SecsvrCaptchaResultRsp) == 534 ? 1 : -1];
typedef char svx_assert_CaptchaResultRsp_msg_off[offsetof(SecsvrCaptchaResultRsp, msg_) == 22 ? 1 : -1];
typedef char svx_assert_GreenUnionRsp_size[sizeof(SecsvrGreenUnionRsp) == 5 ? 1 : -1];
typedef char svx_assert_GreenUnionRsp_src_off[offsetof(SecsvrGreenUnionRsp, start_src_) == 4 ? 1 : -1];
typedef char svx_assert_ImInfoNotifyReq_size[sizeof(SecsvrImInfoNotifyReq) == 38 ? 1 : -1];
typedef char svx_assert_ImInfoNotifyReq_buf_off[offsetof(SecsvrImInfoNotifyReq, nick_name_buf_) == 6 ? 1 : -1];
typedef char svx_assert_UserInfoNotifyReq_size[sizeof(SecsvrUserInfoNotifyReq) == 2056 ? 1 : -1];
typedef char svx_assert_UserInfoNotifyReq_data_off[offsetof(SecsvrUserInfoNotifyReq, data_) == 8 ? 1 : -1];

using namespace tsf4g_tdr;

// ===========================================================================
// SecsvrCaptchaReq (sizeof 541)
//
// ORIG .cpp: pack(char*) 2176/2183, pack 2206..2321, unpack(char*) 2341,
// unpack 2371..2486, visualize(char*) 2526/2533, visualize 2630,
// visualize_ex(char*) 2648..2664, visualize_ex(TdrWriteBuf&) 2677..2685.
// ===========================================================================

TdrError::ErrorType SecsvrCaptchaReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrCaptchaReq::pack(char* buffer, unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrCaptchaReq::pack(TdrWriteBuf& destBuf,
                                           unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrCaptchaReq::unpack(const char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrCaptchaReq::unpack(TdrReadBuf& srcBuf,
                                             unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrCaptchaReq::visualize(char* buffer, unsigned int size,
                                                unsigned int* usedSize,
                                                int indent,
                                                char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrCaptchaReq::visualize(TdrWriteBuf& destBuf, int indent,
                                                char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]",
                                    "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]",
                                    "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[version_]",
                                    "%u", version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bus_id_]",
                                    "%d", bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]",
                                    "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]",
                                    "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_pic_change_num_]", "%d",
                                    max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_verify_num_]", "%d",
                                    max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[timeout_]",
                                    "%u", timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[busi_id_]",
                                    "0x%02x", busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[busi_info_len_]", "%d", busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[busi_info_]",
                                 (int64_t)busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ != 0) {
        for (uint16_t i = 0; i < busi_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", busi_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* SecsvrCaptchaReq::visualize_ex(char* buffer, unsigned int size,
                                           unsigned int* usedSize, int indent,
                                           char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

const char* SecsvrCaptchaReq::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                           char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// SecsvrCaptchaResultRsp (sizeof 534)
//
// ORIG .cpp: pack(char*) 2708/2715, pack 2738..2818, unpack(char*) 2838,
// unpack 2868..2948, visualize(char*) 2988/2995, visualize 3063,
// visualize_ex(char*) 3081..3097, visualize_ex(TdrWriteBuf&) 3110..3118.
// ===========================================================================

TdrError::ErrorType SecsvrCaptchaResultRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrCaptchaResultRsp::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrCaptchaResultRsp::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    // No `msg_len_ > 512` guard here: msg_len_ is uint8_t (<= 255) while msg_
    // is 512 bytes, so the generator's count guard is provably dead - the ORIG
    // machine code contains no compare for it (this function's cmp constants
    // are only the 0x1/0x3 field-width checks) and GCC 4.1.2 warns
    // "comparison is always false" for it.  Same emitted code either way.
    if (destBuf.capacity() - destBuf.size() < msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(msg_, msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrCaptchaResultRsp::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrCaptchaResultRsp::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    // As in pack(): the uint8_t msg_len_ count guard is provably dead.
    if (srcBuf.capacity() - srcBuf.size() < msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(msg_, msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrCaptchaResultRsp::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrCaptchaResultRsp::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[version_]",
                                    "%u", version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]",
                                    "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bus_id_]",
                                    "%d", bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]",
                                    "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]",
                                    "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[app_id_]",
                                    "%u", app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]",
                                    "0x%02x", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[msg_len_]",
                                    "0x%02x", msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    // As in pack(): the uint8_t msg_len_ count guard is provably dead.
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[msg_]",
                                 (int64_t)msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ != 0) {
        for (uint8_t i = 0; i < msg_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* SecsvrCaptchaResultRsp::visualize_ex(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

const char* SecsvrCaptchaResultRsp::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// SecsvrGreenUnionRsp (sizeof 5)
//
// ORIG .cpp: pack(char*) 6540/6547, pack 6558..6588, unpack(char*) 6608,
// unpack 6638..6668, visualize(char*) 6708/6715, visualize (4 scalar fields,
// returns the last printVariable result, no separator write),
// visualize_ex(char*) 6762..6783, visualize_ex(TdrWriteBuf&) 6796..6804.
// ===========================================================================

TdrError::ErrorType SecsvrGreenUnionRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrGreenUnionRsp::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrGreenUnionRsp::pack(TdrWriteBuf& destBuf,
                                              unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(start_src_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrGreenUnionRsp::unpack(const char* buffer,
                                                unsigned int size,
                                                unsigned int* usedSize,
                                                unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrGreenUnionRsp::unpack(TdrReadBuf& srcBuf,
                                                unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(start_src_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrGreenUnionRsp::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrGreenUnionRsp::visualize(TdrWriteBuf& destBuf,
                                                   int indent,
                                                   char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]",
                                    "0x%02x", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[flag_]",
                                    "0x%02x", flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_]",
                                    "%d", world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[start_src_]",
                                    "0x%02x", start_src_);
    return ret;
}

const char* SecsvrGreenUnionRsp::visualize_ex(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

const char* SecsvrGreenUnionRsp::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// SecsvrImInfoNotifyReq (sizeof 38)
//
// ORIG .cpp: pack(char*) 5046/5053, pack 5862..5897, unpack(char*) 5032,
// unpack 5947..5982, visualize(char*) 6022/6029, visualize 6062,
// visualize_ex(char*) 6080..6096, visualize_ex(TdrWriteBuf&) 6109..6117.
// ===========================================================================

TdrError::ErrorType SecsvrImInfoNotifyReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrImInfoNotifyReq::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrImInfoNotifyReq::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(msg_type_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(nick_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (nick_name_len_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < nick_name_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(nick_name_buf_, nick_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrImInfoNotifyReq::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrImInfoNotifyReq::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(msg_type_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(nick_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (nick_name_len_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < nick_name_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(nick_name_buf_, nick_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrImInfoNotifyReq::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrImInfoNotifyReq::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[msg_type_id_]", "%d", msg_type_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[nick_name_len_]", "%d", nick_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (nick_name_len_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[nick_name_buf_]",
                                 (int64_t)nick_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (nick_name_len_ != 0) {
        for (uint16_t i = 0; i < nick_name_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", nick_name_buf_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* SecsvrImInfoNotifyReq::visualize_ex(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

const char* SecsvrImInfoNotifyReq::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                                char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// SecsvrUserInfoNotifyReq (sizeof 2056)
//
// ORIG .cpp: pack(char*) 5536/5543, pack 5554..5589, unpack(char*) 5619,
// unpack 5639..5674, visualize(char*) 5714/5721, visualize 5754,
// visualize_ex(char*) 5772..5788, visualize_ex(TdrWriteBuf&) 5801..5809.
// ===========================================================================

TdrError::ErrorType SecsvrUserInfoNotifyReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrUserInfoNotifyReq::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrUserInfoNotifyReq::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(msg_type_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrUserInfoNotifyReq::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrUserInfoNotifyReq::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(msg_type_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrUserInfoNotifyReq::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType SecsvrUserInfoNotifyReq::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[msg_type_id_]", "%d", msg_type_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]",
                                    "%u", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_]",
                                 (int64_t)data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ != 0) {
        for (uint32_t i = 0; i < data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* SecsvrUserInfoNotifyReq::visualize_ex(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

const char* SecsvrUserInfoNotifyReq::visualize_ex(TdrWriteBuf& destBuf,
                                                  int indent,
                                                  char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

} // namespace sec_proto
