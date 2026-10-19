// Reconstruction of src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function in this file is one of the 43 sec_proto symbols the ORIG
// linker attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, the CU's only out-of-line
// tsf4g_tdr definition).
//
// The "ORIG .cpp:NNN" comments are DW_AT_call_line values of the inlined
// tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py), so the
// call sequence below is the ORIG's, not an inference from the header.
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -9 TDR_ERR_CUTVER_TOO_SMALL, -19 TDR_ERR_NULL_POINTER_PARAMETER,
// -20 TDR_ERR_NET_CUTVER_INVALID, -10 TDR_ERR_CUTVER_CONFILICT.

#include "formmog_proto_tss_sdk_uic.h"

#include <string.h>

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, weak. textize() forwards to the inlined
// vtextize(format, ap) at TdrBuf.h:182.
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

// ---------------------------------------------------------------------------
// UserInputCheckPkgHead (sizeof 10)
// ---------------------------------------------------------------------------

TdrError::ErrorType UserInputCheckPkgHead::construct() {
    proto_id_ = 0;
    version_ = 1;
    pkg_len_ = 0;
    cmd_ = 0;
    game_id_ = 0;
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: version stamp write is the last statement (inlined call at 137);
// the stored constant is 1 in the ORIG (movw $0x100, big-endian 0x0001).
TdrError::ErrorType UserInputCheckPkgHead::pack(TdrWriteBuf& destBuf,
                                                unsigned int cutVer) const throw() {
    unsigned int startPos = destBuf.getUsedSize();
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(cmd_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(1, startPos + 2);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 51, dtor at 58.
TdrError::ErrorType UserInputCheckPkgHead::pack(char* buffer, unsigned int size,
                                                unsigned int* usedSize,
                                                unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: the version peek is the inlined pair readUInt16(ref, pos) +
// getUsedSize() at 173, followed by the five field reads (195/205/215/225/235).
TdrError::ErrorType UserInputCheckPkgHead::unpack(TdrReadBuf& srcBuf,
                                                  unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint16_t version = 0;
    ret = srcBuf.readUInt16(version, srcBuf.getUsedSize() + 2);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (version != 1) {
        return TdrError::TDR_ERR_NET_CUTVER_INVALID;
    }
    ret = srcBuf.readUInt16(proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(cmd_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 156.
TdrError::ErrorType UserInputCheckPkgHead::unpack(const char* buffer,
                                                  unsigned int size,
                                                  unsigned int* usedSize,
                                                  unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType UserInputCheckPkgHead::visualize(TdrWriteBuf& destBuf,
                                                     int indent,
                                                     char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[proto_id_]",
                                    "%d", proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[version_]",
                                    "%d", version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pkg_len_]",
                                    "%d", pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_]", "%d",
                                    cmd_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_id_]",
                                    "%d", game_id_);
    return ret;
}

// ORIG .cpp: ctor at 255, dtor at 262.
TdrError::ErrorType UserInputCheckPkgHead::visualize(char* buffer,
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

// ORIG .cpp: 350 getUsedSize, 351 getTotalSize, 356 writeChar('\0', pos),
// 358 getBeginPtr.
const char* UserInputCheckPkgHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                                char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 321, getUsedSize at 324, getBeginPtr/dtor at 337.
const char* UserInputCheckPkgHead::visualize_ex(char* buffer, unsigned int size,
                                                unsigned int* usedSize,
                                                int indent,
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

// ---------------------------------------------------------------------------
// ReqUserInputCheck (sizeof 1169)
// ---------------------------------------------------------------------------

TdrError::ErrorType ReqUserInputCheck::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: cutVer clamp/check prelude (dead -9 branch, see AGENT_REPORT),
// then 411/421/431 uint32, 442 guarded int8, 452 length, 467 bulk bytes,
// 477 length, 492 bulk bytes.
TdrError::ErrorType ReqUserInputCheck::pack(TdrWriteBuf& destBuf,
                                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (0 == cutVer || cutVer > 2) {
        cutVer = 2;
    }
    if (0 == cutVer) {
        return TdrError::TDR_ERR_CUTVER_TOO_SMALL;
    }
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(session_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(session_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cutVer > 1) {
        ret = destBuf.writeInt8(user_input_code_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt16(user_input_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_input_data_len_ > 1024) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < user_input_data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(user_input_data_, user_input_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(user_game_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_game_data_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < user_game_data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(user_game_data_, user_game_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 381, dtor at 388.
TdrError::ErrorType ReqUserInputCheck::pack(char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 542/552/562 uint32, 573 guarded int8 (else branch zeroes the
// field), 587 length, 602 bulk bytes, 612 length, 627 bulk bytes.
TdrError::ErrorType ReqUserInputCheck::unpack(TdrReadBuf& srcBuf,
                                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (0 == cutVer || cutVer > 2) {
        cutVer = 2;
    }
    if (0 == cutVer) {
        return TdrError::TDR_ERR_CUTVER_TOO_SMALL;
    }
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(session_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(session_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cutVer > 1) {
        ret = srcBuf.readInt8(user_input_code_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    } else {
        user_input_code_ = 0;
    }
    ret = srcBuf.readUInt16(user_input_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_input_data_len_ > 1024) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < user_input_data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(user_input_data_, user_input_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(user_game_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_game_data_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < user_game_data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(user_game_data_, user_game_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 512.
TdrError::ErrorType ReqUserInputCheck::unpack(const char* buffer,
                                              unsigned int size,
                                              unsigned int* usedSize,
                                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 723 (after the user_input_data_
// dump) and 759 (final statement).
TdrError::ErrorType ReqUserInputCheck::visualize(TdrWriteBuf& destBuf,
                                                 int indent,
                                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u",
                                    uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[session_id_]",
                                    "%u", session_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[session_seq_]", "%u", session_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[user_input_code_]", "0x%02x",
                                    user_input_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[user_input_data_len_]", "%d",
                                    user_input_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_input_data_len_ > 1024) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator,
                                 "[user_input_data_]", user_input_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_input_data_len_ != 0) {
        for (uint32_t i = 0; i < user_input_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", user_input_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[user_game_data_len_]", "%d",
                                    user_game_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_game_data_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator,
                                 "[user_game_data_]", user_game_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (user_game_data_len_ != 0) {
        for (uint32_t i = 0; i < user_game_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", user_game_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 647, dtor at 654.
TdrError::ErrorType ReqUserInputCheck::visualize(char* buffer, unsigned int size,
                                                 unsigned int* usedSize,
                                                 int indent,
                                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 780 getUsedSize, 793 getBeginPtr.
const char* ReqUserInputCheck::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 777, getUsedSize at 780, getBeginPtr/dtor at 793.
const char* ReqUserInputCheck::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------------
// RspUserInputCheck (sizeof 16)
// ---------------------------------------------------------------------------

TdrError::ErrorType RspUserInputCheck::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 867/877/887/897, no cutVer use at all.
TdrError::ErrorType RspUserInputCheck::pack(TdrWriteBuf& destBuf,
                                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(session_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(session_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(check_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 837, dtor at 844.
TdrError::ErrorType RspUserInputCheck::pack(char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 947/957/967/977.
TdrError::ErrorType RspUserInputCheck::unpack(TdrReadBuf& srcBuf,
                                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(session_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(session_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(check_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 917.
TdrError::ErrorType RspUserInputCheck::unpack(const char* buffer,
                                              unsigned int size,
                                              unsigned int* usedSize,
                                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType RspUserInputCheck::visualize(TdrWriteBuf& destBuf,
                                                 int indent,
                                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u",
                                    uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[session_id_]",
                                    "%u", session_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[session_seq_]", "%u", session_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[check_result_]", "%u", check_result_);
    return ret;
}

// ORIG .cpp: ctor at 997, dtor at 1004.
TdrError::ErrorType RspUserInputCheck::visualize(char* buffer, unsigned int size,
                                                 unsigned int* usedSize,
                                                 int indent,
                                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1084 getUsedSize, 1085 getTotalSize, 1090 writeChar('\0', pos),
// 1092 getBeginPtr.
const char* RspUserInputCheck::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1055, getUsedSize at 1058, getBeginPtr/dtor at 1071.
const char* RspUserInputCheck::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------------
// UserInputCheckPkgBody (anonymous union of the two bodies, sizeof 1169)
//
// Union arm selectors are 1 = req_user_input_check_, 2 = rsp_user_input_check_
// (ORIG: xor $0x1 / xor $0x2 on the low selector word, OR-ed with the high
// word).  Unknown selectors return TDR_NO_ERROR, as in the ORIG.
// ---------------------------------------------------------------------------

TdrError::ErrorType UserInputCheckPkgBody::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    switch (union_selector) {
    case 1:
        ret = req_user_input_check_.construct();
        break;
    case 2:
        ret = rsp_user_input_check_.construct();
        break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType UserInputCheckPkgBody::pack(int64_t union_selector,
                                                TdrWriteBuf& destBuf,
                                                unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (0 == cutVer || cutVer > 2) {
        cutVer = 2;
    }
    if (0 == cutVer) {
        return TdrError::TDR_ERR_CUTVER_TOO_SMALL;
    }
    switch (union_selector) {
    case 1:
        ret = req_user_input_check_.pack(destBuf, cutVer);
        break;
    case 2:
        ret = rsp_user_input_check_.pack(destBuf, cutVer);
        break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG .cpp: ctor at 1132, dtor at 1139.
TdrError::ErrorType UserInputCheckPkgBody::pack(int64_t union_selector,
                                                char* buffer,
                                                unsigned int size,
                                                unsigned int* usedSize,
                                                unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType UserInputCheckPkgBody::unpack(int64_t union_selector,
                                                  TdrReadBuf& srcBuf,
                                                  unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (0 == cutVer || cutVer > 2) {
        cutVer = 2;
    }
    if (0 == cutVer) {
        return TdrError::TDR_ERR_CUTVER_TOO_SMALL;
    }
    switch (union_selector) {
    case 1:
        ret = req_user_input_check_.unpack(srcBuf, cutVer);
        break;
    case 2:
        ret = rsp_user_input_check_.unpack(srcBuf, cutVer);
        break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG .cpp: ctor at 1191.
TdrError::ErrorType UserInputCheckPkgBody::unpack(int64_t union_selector,
                                                  const char* buffer,
                                                  unsigned int size,
                                                  unsigned int* usedSize,
                                                  unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType UserInputCheckPkgBody::visualize(int64_t union_selector,
                                                     TdrWriteBuf& destBuf,
                                                     int indent,
                                                     char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    switch (union_selector) {
    case 1:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[req_user_input_check_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = req_user_input_check_.visualize(destBuf, indent + 1,
                                                  separator);
        } else {
            ret = req_user_input_check_.visualize(destBuf, indent, separator);
        }
        break;
    case 2:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[rsp_user_input_check_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = rsp_user_input_check_.visualize(destBuf, indent + 1,
                                                  separator);
        } else {
            ret = rsp_user_input_check_.visualize(destBuf, indent, separator);
        }
        break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG .cpp: ctor at 1250, dtor at 1257.
TdrError::ErrorType UserInputCheckPkgBody::visualize(int64_t union_selector,
                                                     char* buffer,
                                                     unsigned int size,
                                                     unsigned int* usedSize,
                                                     int indent,
                                                     char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent,
                                        separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ---------------------------------------------------------------------------
// UserInputCheckPkg (head_ at 0, body_ at 10, sizeof 1179)
// ---------------------------------------------------------------------------

TdrError::ErrorType UserInputCheckPkg::construct() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head_.construct();
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.construct((int64_t)head_.cmd_);
    return ret;
}

// ORIG .cpp: 1381 getUsedSize, 1407 writeUInt16(cutVer, startPos + 2),
// 1415 getUsedSize, 1416 writeUInt16(pkg_len, startPos + 4).
TdrError::ErrorType UserInputCheckPkg::pack(TdrWriteBuf& destBuf,
                                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (0 == cutVer || cutVer > 2) {
        cutVer = 2;
    }
    if (0 == cutVer) {
        return TdrError::TDR_ERR_CUTVER_TOO_SMALL;
    }
    unsigned int startPos = destBuf.getUsedSize();
    ret = head_.pack(destBuf, cutVer);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.pack((int64_t)head_.cmd_, destBuf, cutVer);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16((uint16_t)cutVer, startPos + 2);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(
        (uint16_t)(destBuf.getUsedSize() - startPos), startPos + 4);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1348, dtor at 1355.
TdrError::ErrorType UserInputCheckPkg::pack(char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1453 pkg_len_ peek + getUsedSize, 1469 version peek.  cutVer is
// not used: the wire version field drives the head/body unpack.
TdrError::ErrorType UserInputCheckPkg::unpack(TdrReadBuf& srcBuf,
                                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint16_t version = 0;
    uint16_t pkgLen = 0;
    ret = srcBuf.readUInt16(pkgLen, srcBuf.getUsedSize() + 4);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < pkgLen) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readUInt16(version, srcBuf.getUsedSize() + 2);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (version - 1 > 1) {
        return TdrError::TDR_ERR_NET_CUTVER_INVALID;
    }
    if (0 == version) {
        return TdrError::TDR_ERR_CUTVER_CONFILICT;
    }
    ret = head_.unpack(srcBuf, version);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.unpack((int64_t)head_.cmd_, srcBuf, version);
    return ret;
}

// ORIG .cpp: ctor at 1435.
TdrError::ErrorType UserInputCheckPkg::unpack(const char* buffer,
                                              unsigned int size,
                                              unsigned int* usedSize,
                                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType UserInputCheckPkg::visualize(TdrWriteBuf& destBuf,
                                                 int indent,
                                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head_]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = head_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = head_.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = body_.visualize((int64_t)head_.cmd_, destBuf, indent + 1,
                              separator);
    } else {
        ret = body_.visualize((int64_t)head_.cmd_, destBuf, indent, separator);
    }
    return ret;
}

// ORIG .cpp: ctor at 1522, dtor at 1529.
TdrError::ErrorType UserInputCheckPkg::visualize(char* buffer, unsigned int size,
                                                 unsigned int* usedSize,
                                                 int indent,
                                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1622 getUsedSize, 1623 getTotalSize, 1628 writeChar('\0', pos),
// 1630 getBeginPtr.
const char* UserInputCheckPkg::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1593, getUsedSize at 1596, getBeginPtr/dtor at 1609.
const char* UserInputCheckPkg::visualize_ex(char* buffer, unsigned int size,
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

} // namespace sec_proto
