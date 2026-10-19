// Reconstruction of src/protocol/formmog/formmog_proto_public_latransadapt.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function in this file is one of the 81 sec_proto symbols the ORIG
// linker attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_public_latransadapt.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, weak, the CU's only
// out-of-line tsf4g_tdr definition; defined weak here so it stays
// link-compatible with the identically-named weak symbol the ORIG carries).
//
// The "ORIG .cpp:NNN" comments are DW_AT_call_line values of the inlined
// tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py), i.e. the
// ORIG's own call sequence and source positions, not an inference.
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -19 TDR_ERR_NULL_POINTER_PARAMETER.
//
// None of the pack()/unpack() functions of this CU looks at cutVer: the ORIG
// code has no version prelude at all (verified function by function: the
// argument is never loaded).

#include "formmog_proto_public_latransadapt.h"

#include <string.h>

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, weak. textize() forwards to the inlined
// vtextize(format, ap) at TdrBuf.h:182.
__attribute__((weak))
__attribute__((weak)) TdrError::ErrorType TdrWriteBuf::textize(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    TdrError::ErrorType ret = vtextize(format, ap);
    va_end(ap);
    return ret;
}


} // namespace tsf4g_tdr

namespace sec_proto {
using namespace tsf4g_tdr;


// ===========================================================================
// LATransAdaptDPData (sizeof 32009)
//
// ORIG .cpp: pack(char*) 43/50, pack 73..118, unpack(char*) 138,
// unpack 168..213, visualize(char*) 233/240, visualize 301,
// visualize_ex(char*) 319..335, visualize_ex(TdrWriteBuf&) 348..356.
// ===========================================================================

TdrError::ErrorType LATransAdaptDPData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPData::pack(char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPData::pack(TdrWriteBuf& destBuf,
                                             unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(dp_msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32000) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPData::unpack(const char* buffer,
                                               unsigned int size,
                                               unsigned int* usedSize,
                                               unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPData::unpack(TdrReadBuf& srcBuf,
                                               unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(dp_msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32000) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPData::visualize(char* buffer,
                                                  unsigned int size,
                                                  unsigned int* usedSize,
                                                  int indent,
                                                  char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPData::visualize(TdrWriteBuf& destBuf,
                                                  int indent,
                                                  char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]",
                                    "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]",
                                    "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dp_msg_id_]",
                                    "0x%02x", dp_msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]",
                                    "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32000) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_]",
                                 (int64_t)data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ != 0) {
        for (uint16_t i = 0; i < data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptDPData::visualize_ex(char* buffer, unsigned int size,
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

const char* LATransAdaptDPData::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                             char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptAntibotRPCodeResult (sizeof 32006)
//
// ORIG .cpp: pack(char*) 379/386, pack 409..434, unpack(char*) 454,
// unpack 484..509, visualize(char*) 529/536, visualize 581,
// visualize_ex(char*) 599..615, visualize_ex(TdrWriteBuf&) 628..636.
// ===========================================================================

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rpcode_result_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_len_ > 32000) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < rpcode_result_data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(rpcode_result_data_, rpcode_result_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rpcode_result_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_len_ > 32000) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < rpcode_result_data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(rpcode_result_data_, rpcode_result_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRPCodeResult::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]",
                                    "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[rpcode_result_data_len_]", "%d",
                                    rpcode_result_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_len_ > 32000) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator,
                                 "[rpcode_result_data_]",
                                 (int64_t)rpcode_result_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_len_ != 0) {
        for (uint16_t i = 0; i < rpcode_result_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpcode_result_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptAntibotRPCodeResult::visualize_ex(
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

const char* LATransAdaptAntibotRPCodeResult::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptAntibotErrMsg (sizeof 1028)
//
// ORIG .cpp: pack(char*) 659/666, pack 689..714, unpack(char*) 734,
// unpack 764..789, visualize(char*) 809/816, visualize 861,
// visualize_ex(char*) 879..895, visualize_ex(TdrWriteBuf&) 908..916.
// ===========================================================================

TdrError::ErrorType LATransAdaptAntibotErrMsg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotErrMsg::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotErrMsg::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeInt16(err_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(err_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < err_msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(err_msg_, err_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotErrMsg::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotErrMsg::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readInt16(err_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(err_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < err_msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(err_msg_, err_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotErrMsg::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotErrMsg::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[err_id_]",
                                    "%d", err_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[err_msg_len_]", "%d", err_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[err_msg_]",
                                 (int64_t)err_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_len_ != 0) {
        for (uint16_t i = 0; i < err_msg_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", err_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptAntibotErrMsg::visualize_ex(
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

const char* LATransAdaptAntibotErrMsg::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptAntibotRoleName (sizeof 1026)
//
// ORIG .cpp: pack(char*) 939/946, pack 969..984, unpack(char*) 1004,
// unpack 1034..1049, visualize(char*) 1069/1076, visualize 1113,
// visualize_ex(char*) 1131..1147, visualize_ex(TdrWriteBuf&) 1160..1168.
// ===========================================================================

TdrError::ErrorType LATransAdaptAntibotRoleName::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRoleName::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRoleName::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(role_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (role_name_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < role_name_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(role_name_, role_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRoleName::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRoleName::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(role_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (role_name_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < role_name_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(role_name_, role_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRoleName::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRoleName::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[role_name_len_]", "%d", role_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (role_name_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[role_name_]",
                                 (int64_t)role_name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (role_name_len_ != 0) {
        for (uint16_t i = 0; i < role_name_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", role_name_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptAntibotRoleName::visualize_ex(
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

const char* LATransAdaptAntibotRoleName::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptAntibotClientIP (sizeof 21)
//
// ORIG .cpp: pack(char*) 1191/1198, pack 1221..1266, unpack(char*) 1286,
// unpack 1316..1361, visualize(char*) 1381/1388, visualize 1449,
// visualize_ex(char*) 1467..1483, visualize_ex(TdrWriteBuf&) 1496..1504.
// ===========================================================================

TdrError::ErrorType LATransAdaptAntibotClientIP::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotClientIP::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotClientIP::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_intranet_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeBytes(client_key_signature_, 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotClientIP::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotClientIP::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_intranet_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readBytes(client_key_signature_, 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotClientIP::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotClientIP::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[flag_]",
                                    "0x%02x", flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[client_intranet_ip_]", "%u",
                                    client_intranet_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator,
                                 "[client_key_signature_]", (int64_t)16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", client_key_signature_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptAntibotClientIP::visualize_ex(
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

const char* LATransAdaptAntibotClientIP::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptAntibotSigInfo (sizeof 10252)
//
// ORIG .cpp: pack(char*) 1527/1534, pack 1557..1577, unpack(char*) 1597,
// unpack 1627..1647, visualize(char*) 1667/1674, visualize 1714,
// visualize_ex(char*) 1732..1748, visualize_ex(TdrWriteBuf&) 1761..1769.
// ===========================================================================

TdrError::ErrorType LATransAdaptAntibotSigInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotSigInfo::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotSigInfo::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(total_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(mac_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(sig_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_len_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < sig_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(sig_, sig_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotSigInfo::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotSigInfo::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(total_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(mac_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(sig_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_len_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < sig_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(sig_, sig_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotSigInfo::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotSigInfo::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[total_size_]", "%d", total_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ip_]", "%u",
                                    ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mac_id_]",
                                    "%u", mac_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sig_len_]",
                                    "%d", sig_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_len_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[sig_]",
                                 (int64_t)sig_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_len_ != 0) {
        for (uint16_t i = 0; i < sig_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", sig_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptAntibotSigInfo::visualize_ex(
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

const char* LATransAdaptAntibotSigInfo::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptAntibotRPFMC (sizeof 10246)
//
// ORIG .cpp: pack(char*) 1792/1799, pack 1822..1847, unpack(char*) 1867,
// unpack 1897..1922, visualize(char*) 1942/1949, visualize 1994,
// visualize_ex(char*) 2012..2028, visualize_ex(TdrWriteBuf&) 2041..2049.
// ===========================================================================

TdrError::ErrorType LATransAdaptAntibotRPFMC::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRPFMC::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRPFMC::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (result_len_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < result_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(result_, result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRPFMC::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRPFMC::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (result_len_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < result_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(result_, result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptAntibotRPFMC::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptAntibotRPFMC::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u",
                                    uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[result_len_]", "%d", result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (result_len_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[result_]",
                                 (int64_t)result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (result_len_ != 0) {
        for (uint16_t i = 0; i < result_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", result_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

const char* LATransAdaptAntibotRPFMC::visualize_ex(
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

const char* LATransAdaptAntibotRPFMC::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptDPCmdMonitor (sizeof 14)
//
// ORIG .cpp: pack(char*) 2072/2079, pack 2102..2142, unpack(char*) 2162,
// unpack 2192..2232, visualize(char*) 2252/2259, visualize (return value of
// the last printVariable, no separator write, no array),
// visualize_ex(char*) 2318..2334, visualize_ex(TdrWriteBuf&) 2347..2355.
// ===========================================================================

TdrError::ErrorType LATransAdaptDPCmdMonitor::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPCmdMonitor::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPCmdMonitor::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeInt32(monitor_cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPCmdMonitor::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPCmdMonitor::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readInt32(monitor_cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPCmdMonitor::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPCmdMonitor::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]",
                                    "%u", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]",
                                    "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[monitor_cmd_id_]", "%d",
                                    monitor_cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[is_need_encrypt_]", "0x%02x",
                                    is_need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[is_encrypted_]", "0x%02x",
                                    is_encrypted_);
    return ret;
}

const char* LATransAdaptDPCmdMonitor::visualize_ex(
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

const char* LATransAdaptDPCmdMonitor::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ===========================================================================
// LATransAdaptDPCmdBufferMonitor (sizeof 32786)
//
// ORIG .cpp: pack(char*) 2378/2385, pack 2408..2473, unpack(char*) 2493,
// unpack 2523..2588, visualize(char*) 2608/2615, visualize 2692,
// visualize_ex(char*) 2710..2726, visualize_ex(TdrWriteBuf&) 2739..2747.
// ===========================================================================

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeInt32(monitor_cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32768) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readInt32(monitor_cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32768) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType LATransAdaptDPCmdBufferMonitor::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]",
                                    "%u", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]",
                                    "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[monitor_cmd_id_]", "%d",
                                    monitor_cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[is_need_encrypt_]", "0x%02x",
                                    is_need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[is_encrypted_]", "0x%02x",
                                    is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]",
                                    "%u", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32768) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
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

const char* LATransAdaptDPCmdBufferMonitor::visualize_ex(
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

const char* LATransAdaptDPCmdBufferMonitor::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

} // namespace sec_proto
