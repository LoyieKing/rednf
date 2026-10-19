// Reconstruction of src/protocol/formmog/formmog_proto_captcha_client_old.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// This TU provides the 97 sec_proto symbols the ORIG DWARF attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_captcha_client_old.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, weak comdat in the ORIG and
// weak here, so each CU of the library can carry its own copy).
//
// The "ORIG .cpp:NNN" comments are DW_AT_call_line values of the inlined
// tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py).  Per class
// the generator emits
//   construct(), pack(char*), pack(TdrWriteBuf&), unpack(char const*),
//   unpack(TdrReadBuf&), visualize(char*), visualize(TdrWriteBuf&),
//   visualize_ex(char*), visualize_ex(TdrWriteBuf&)
// with a 10-line stride per scalar field and 15 per length+byte-array pair.  The
// class blocks the cited lines delimit are, in file order:
//   TSSCaptchaOldHead 43..440, CaptchaReq 463..802, VerifyReq 825..1140,
//   VerifyRes 1163..1504, S2CVerifyRes 1527..1784, ReportLog 1807..2092,
//   BusiCaptchaReq 2115..2568, CaptchaBusiFinish 2591..2932,
//   BusiCaptchaPic 2955..3524, TSSCaptchaOldBody 3618..3851,
//   TSSCaptchaOldPkg 4067..4324.
//
// Recovered protocol facts (all measured in the ORIG machine code):
//  * union selectors are message ids: 1 busi_captcha_req_, 3073 captcha_req_,
//    3075 verify_req_, 3076 verify_res_, 3078 s2c_verify_res_, 3090
//    busi_captcha_pic_, 3091 report_log_, 3092 captcha_busi_finish_; unknown
//    selectors return TDR_NO_ERROR.  Every arm is called with version 1.
//  * TSSCaptchaOldHead::pack stamps head_.head_len_ (offset 20) with
//    (uint16_t)(getUsedSize() - startPos) at startPos + 20; it does not touch
//    version_ (the field is written from the object) and ignores cutVer.
//  * TSSCaptchaOldHead::unpack first peeks the u16 at getUsedSize() + 20
//    (head_len_) via readUInt16(v, pos) - the ORIG's two-step capacity check -
//    and rejects a head_len_ larger than the remaining buffer with
//    TDR_ERR_SHORT_BUF_FOR_READ before reading any field.  There is no version
//    gate: the head is fixed at version 1.
//  * TSSCaptchaOldPkg::pack/unpack pass the literal 1 to head_/body_, use
//    head_.msg_id_ as the union selector, stamp head_.body_len_ (offset 22)
//    with the body size, and reject a body_len_ larger than the remaining
//    buffer with TDR_ERR_SHORT_BUF_FOR_READ.
//  * byte-array bounds are sizeof(array): 64 code_, 200 session_,
//    4096 picture_, 128 msg_, 512 log_msg_/busi_info_.
//  * visualize formats follow the ORIG's per-field spec: u32 -> "%u",
//    u16 -> "%d", u8 -> "0x%02x", uint8_t arrays -> " %d", int8_t arrays ->
//    " 0x%02x"; writeCharWithNull(separator) is emitted after every array.
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -19 TDR_ERR_NULL_POINTER_PARAMETER.

#include "formmog_proto_captcha_client_old.h"

#include <string.h>

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, weak (`W` in nm -DC of the ORIG).
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
// TSSCaptchaOldHead (sizeof 24)
// ---------------------------------------------------------------------------

// ORIG: six movl $0 = the memset(24) expansion.
TdrError::ErrorType TSSCaptchaOldHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 71 startPos, 77 msg_id_ u16, 87 version_ u16, 97 uin_ u32,
// 107 client_ip_ u32, 117 seq_ u32, 127 send_time_ u32, 137 head_len_ u16,
// 147 body_len_ u16, 156/157 the head_len_ stamp at startPos + 20.
// cutVer is not read: the head is a fixed version-1 layout.
TdrError::ErrorType TSSCaptchaOldHead::pack(TdrWriteBuf& destBuf,
                                            unsigned int cutVer) const throw() {
    unsigned int startPos = destBuf.getUsedSize();
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(send_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(head_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16((uint16_t)(destBuf.getUsedSize() - startPos),
                              startPos + 20);
    return ret;
}

// ORIG .cpp: ctor at 43, dtor at 50.
TdrError::ErrorType TSSCaptchaOldHead::pack(char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 194 the head_len_ peek (readUInt16(headLen, getUsedSize() + 20)),
// 202 the remaining-size check (ORIG calls TdrReadBuf::getLeftSize(), which the
// shared compat model spells capacity() - size()), 223 msg_id_, 233 version_,
// 243 uin_, 253 client_ip_, 263 seq_, 273 send_time_, 283 head_len_,
// 293 body_len_.  No version gate: the head is a fixed version-1 layout.
TdrError::ErrorType TSSCaptchaOldHead::unpack(TdrReadBuf& srcBuf,
                                              unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint16_t headLen = 0;
    ret = srcBuf.readUInt16(headLen, srcBuf.getUsedSize() + 20);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < headLen) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readUInt16(msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(send_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(head_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 133.
TdrError::ErrorType TSSCaptchaOldHead::unpack(const char* buffer,
                                              unsigned int size,
                                              unsigned int* usedSize,
                                              unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: msg_id_/version_/head_len_/body_len_ "%d", uin_/client_ip_/seq_/
// send_time_ "%u" (printVariable call sites from 271).
TdrError::ErrorType TSSCaptchaOldHead::visualize(TdrWriteBuf& destBuf,
                                                 int indent,
                                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[msg_id_]",
                                    "%d", msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[version_]",
                                    "%d", version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u",
                                    uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]",
                                    "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_]", "%u",
                                    seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_time_]",
                                    "%u", send_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head_len_]",
                                    "%d", head_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_len_]",
                                    "%d", body_len_);
    return ret;
}

// ORIG .cpp: ctor at 313, dtor at 320.
TdrError::ErrorType TSSCaptchaOldHead::visualize(char* buffer,
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

// ORIG .cpp: 432 getUsedSize, 433 getTotalSize, 438 writeChar('\0', pos),
// 440 getBeginPtr.
const char* TSSCaptchaOldHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 403, getUsedSize at 406, getBeginPtr/dtor at 419.
const char* TSSCaptchaOldHead::visualize_ex(char* buffer, unsigned int size,
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
// CaptchaReq (sizeof 24)
// ---------------------------------------------------------------------------

TdrError::ErrorType CaptchaReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 493 app_id_ u32, 503 antibot_bus_id_ u32, 513 zone_id_ u32,
// 523 max_pic_change_num_ u16, 533 max_verify_num_ u16, 543 op_type_ u32,
// 553 timeout_ u32.  No cutVer gate.
TdrError::ErrorType CaptchaReq::pack(TdrWriteBuf& destBuf,
                                     unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(op_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 463, dtor at 470.
TdrError::ErrorType CaptchaReq::pack(char* buffer, unsigned int size,
                                     unsigned int* usedSize,
                                     unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType CaptchaReq::unpack(TdrReadBuf& srcBuf,
                                       unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(op_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType CaptchaReq::unpack(const char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: app_id_/antibot_bus_id_/zone_id_/op_type_/timeout_ "%u",
// max_pic_change_num_/max_verify_num_ "%d".
TdrError::ErrorType CaptchaReq::visualize(TdrWriteBuf& destBuf, int indent,
                                          char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[app_id_]",
                                    "%u", app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[antibot_bus_id_]", "%u", antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[zone_id_]",
                                    "%u", zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_pic_change_num_]", "%d",
                                    max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_verify_num_]", "%d", max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[op_type_]",
                                    "%u", op_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[timeout_]",
                                    "%u", timeout_);
    return ret;
}

// ORIG .cpp: ctor at 683, dtor at 690.
TdrError::ErrorType CaptchaReq::visualize(char* buffer, unsigned int size,
                                          unsigned int* usedSize, int indent,
                                          char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 794 getUsedSize, 795 getTotalSize, 800 writeChar('\0', pos),
// 802 getBeginPtr.
const char* CaptchaReq::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                     char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 765, getUsedSize at 768, getBeginPtr/dtor at 781.
const char* CaptchaReq::visualize_ex(char* buffer, unsigned int size,
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
// VerifyReq (sizeof 267)
// ---------------------------------------------------------------------------

TdrError::ErrorType VerifyReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 855 code_len_ u8, 870 writeBytes(code_) bound 64 (cmp $0x40),
// 880 session_len_ u16, 895 writeBytes(session_) bound 200 (cmp $0xc8).
TdrError::ErrorType VerifyReq::pack(TdrWriteBuf& destBuf,
                                    unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(code_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (code_len_ > 64) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < code_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(code_, code_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ > 200) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < session_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(session_, session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 825, dtor at 832.
TdrError::ErrorType VerifyReq::pack(char* buffer, unsigned int size,
                                    unsigned int* usedSize,
                                    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType VerifyReq::unpack(TdrReadBuf& srcBuf,
                                      unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(code_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (code_len_ > 64) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < code_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(code_, code_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ > 200) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < session_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(session_, session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType VerifyReq::unpack(const char* buffer, unsigned int size,
                                      unsigned int* usedSize,
                                      unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 1049 (after code_) and 1085 (after session_);
// code_len_ "0x%02x", session_len_ "%d", code_/session_ elements " %d"
// (uint8_t arrays).
TdrError::ErrorType VerifyReq::visualize(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[code_len_]",
                                    "0x%02x", code_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (code_len_ > 64) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[code_]",
                                 code_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (code_len_ != 0) {
        for (uint8_t i = 0; i < code_len_; ++i) {
            ret = destBuf.textize(" %d", code_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[session_len_]", "%d", session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ > 200) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[session_]",
                                 session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ != 0) {
        for (uint16_t i = 0; i < session_len_; ++i) {
            ret = destBuf.textize(" %d", session_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 1005, dtor at 1012.
TdrError::ErrorType VerifyReq::visualize(char* buffer, unsigned int size,
                                         unsigned int* usedSize, int indent,
                                         char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1132 getUsedSize, 1133 getTotalSize, 1138 writeChar('\0', pos),
// 1140 getBeginPtr.
const char* VerifyReq::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                    char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1103, getUsedSize at 1106, getBeginPtr/dtor at 1119.
const char* VerifyReq::visualize_ex(char* buffer, unsigned int size,
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
// VerifyRes (sizeof 142)
// ---------------------------------------------------------------------------

TdrError::ErrorType VerifyRes::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 1193 app_id_ u32, 1203 antibot_bus_id_ u32, 1213 zone_id_ u32,
// 1223 result_ u8, 1233 msg_len_ u8, 1248 writeBytes(msg_) bound 128
// (cmp $0x80).
TdrError::ErrorType VerifyRes::pack(TdrWriteBuf& destBuf,
                                    unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(msg_, msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1163, dtor at 1170.
TdrError::ErrorType VerifyRes::pack(char* buffer, unsigned int size,
                                    unsigned int* usedSize,
                                    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1298/1308/1318 the u32 fields, 1328 result_, 1338 msg_len_,
// 1353 readBytes(msg_).
TdrError::ErrorType VerifyRes::unpack(TdrReadBuf& srcBuf,
                                      unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(msg_, msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1268.
TdrError::ErrorType VerifyRes::unpack(const char* buffer, unsigned int size,
                                      unsigned int* usedSize,
                                      unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 1449; app_id_/antibot_bus_id_/zone_id_ "%u",
// result_/msg_len_ "0x%02x", msg_ elements " 0x%02x" (int8_t).
TdrError::ErrorType VerifyRes::visualize(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[app_id_]",
                                    "%u", app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[antibot_bus_id_]", "%u", antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[zone_id_]",
                                    "%u", zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]",
                                    "0x%02x", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[msg_len_]",
                                    "0x%02x", msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[msg_]", msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ != 0) {
        for (uint8_t i = 0; i < msg_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 1373, dtor at 1380.
TdrError::ErrorType VerifyRes::visualize(char* buffer, unsigned int size,
                                         unsigned int* usedSize, int indent,
                                         char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1496 getUsedSize, 1497 getTotalSize, 1502 writeChar('\0', pos),
// 1504 getBeginPtr.
const char* VerifyRes::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                    char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1467, getUsedSize at 1470, getBeginPtr/dtor at 1483.
const char* VerifyRes::visualize_ex(char* buffer, unsigned int size,
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
// S2CVerifyRes (sizeof 130)
// ---------------------------------------------------------------------------

TdrError::ErrorType S2CVerifyRes::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 1557 result_ u8, 1567 msg_len_ u8, 1582 writeBytes(msg_) bound 128
// (cmp $0x80).
TdrError::ErrorType S2CVerifyRes::pack(TdrWriteBuf& destBuf,
                                       unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(msg_, msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType S2CVerifyRes::pack(char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType S2CVerifyRes::unpack(TdrReadBuf& srcBuf,
                                         unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < msg_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(msg_, msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType S2CVerifyRes::unpack(const char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 1729; result_/msg_len_ "0x%02x", msg_
// elements " 0x%02x" (int8_t).
TdrError::ErrorType S2CVerifyRes::visualize(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]",
                                    "0x%02x", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[msg_len_]",
                                    "0x%02x", msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[msg_]", msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (msg_len_ != 0) {
        for (uint8_t i = 0; i < msg_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

TdrError::ErrorType S2CVerifyRes::visualize(char* buffer, unsigned int size,
                                            unsigned int* usedSize, int indent,
                                            char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

const char* S2CVerifyRes::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                       char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

const char* S2CVerifyRes::visualize_ex(char* buffer, unsigned int size,
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
// ReportLog (sizeof 518)
// ---------------------------------------------------------------------------

TdrError::ErrorType ReportLog::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 1837 log_src_ u16, 1847 log_type_ u16, 1857 log_len_ u16,
// 1872 writeBytes(log_msg_) bound 512 (cmp $0x200).
TdrError::ErrorType ReportLog::pack(TdrWriteBuf& destBuf,
                                    unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(log_src_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(log_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(log_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (log_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < log_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(log_msg_, log_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1807, dtor at 1814.
TdrError::ErrorType ReportLog::pack(char* buffer, unsigned int size,
                                    unsigned int* usedSize,
                                    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType ReportLog::unpack(TdrReadBuf& srcBuf,
                                      unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(log_src_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(log_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(log_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (log_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < log_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(log_msg_, log_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType ReportLog::unpack(const char* buffer, unsigned int size,
                                      unsigned int* usedSize,
                                      unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 2037; log_src_/log_type_/log_len_ "%d",
// log_msg_ elements " 0x%02x" (int8_t).
TdrError::ErrorType ReportLog::visualize(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[log_src_]",
                                    "%d", log_src_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[log_type_]",
                                    "%d", log_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[log_len_]",
                                    "%d", log_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (log_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[log_msg_]",
                                 log_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (log_len_ != 0) {
        for (uint16_t i = 0; i < log_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", log_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

TdrError::ErrorType ReportLog::visualize(char* buffer, unsigned int size,
                                         unsigned int* usedSize, int indent,
                                         char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

const char* ReportLog::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                    char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

const char* ReportLog::visualize_ex(char* buffer, unsigned int size,
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
// BusiCaptchaReq (sizeof 539)
// ---------------------------------------------------------------------------

TdrError::ErrorType BusiCaptchaReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 2145 app_id_, 2155 antibot_bus_id_, 2165 zone_id_ (u32),
// 2175 max_pic_change_num_, 2185 max_verify_num_ (u16), 2195 op_type_,
// 2205 timeout_ (u32), 2215 busi_id_ u8, 2225 busi_info_len_ u16,
// 2240 writeBytes(busi_info_) bound 512.
TdrError::ErrorType BusiCaptchaReq::pack(TdrWriteBuf& destBuf,
                                         unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(op_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 2115, dtor at 2122.
TdrError::ErrorType BusiCaptchaReq::pack(char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType BusiCaptchaReq::unpack(TdrReadBuf& srcBuf,
                                           unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(op_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType BusiCaptchaReq::unpack(const char* buffer,
                                           unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 2513; 5 u32 "%u", 2 u16 + busi_info_len_
// "%d", busi_id_ "0x%02x", busi_info_ elements " 0x%02x" (int8_t).
TdrError::ErrorType BusiCaptchaReq::visualize(TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[app_id_]",
                                    "%u", app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[antibot_bus_id_]", "%u", antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[zone_id_]",
                                    "%u", zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_pic_change_num_]", "%d",
                                    max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_verify_num_]", "%d", max_verify_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[op_type_]",
                                    "%u", op_type_);
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
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[busi_info_]",
                                 busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ != 0) {
        for (uint16_t i = 0; i < busi_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", busi_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

TdrError::ErrorType BusiCaptchaReq::visualize(char* buffer, unsigned int size,
                                              unsigned int* usedSize, int indent,
                                              char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

const char* BusiCaptchaReq::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

const char* BusiCaptchaReq::visualize_ex(char* buffer, unsigned int size,
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
// CaptchaBusiFinish (sizeof 527)
// ---------------------------------------------------------------------------

TdrError::ErrorType CaptchaBusiFinish::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 2621 app_id_, 2631 antibot_bus_id_, 2641 zone_id_ (u32),
// 2651 busi_id_ u8, 2661 busi_info_len_ u16, 2676 writeBytes(busi_info_).
TdrError::ErrorType CaptchaBusiFinish::pack(TdrWriteBuf& destBuf,
                                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 2591, dtor at 2598.
TdrError::ErrorType CaptchaBusiFinish::pack(char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType CaptchaBusiFinish::unpack(TdrReadBuf& srcBuf,
                                              unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType CaptchaBusiFinish::unpack(const char* buffer,
                                              unsigned int size,
                                              unsigned int* usedSize,
                                              unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 2877; app_id_/antibot_bus_id_/zone_id_ "%u",
// busi_id_ "0x%02x", busi_info_len_ "%d", busi_info_ elements " 0x%02x".
TdrError::ErrorType CaptchaBusiFinish::visualize(TdrWriteBuf& destBuf,
                                                 int indent,
                                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[app_id_]",
                                    "%u", app_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[antibot_bus_id_]", "%u", antibot_bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[zone_id_]",
                                    "%u", zone_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[busi_id_]",
                                    "0x%02x", busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[busi_info_len_]", "%d", busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[busi_info_]",
                                 busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ != 0) {
        for (uint16_t i = 0; i < busi_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", busi_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

TdrError::ErrorType CaptchaBusiFinish::visualize(char* buffer,
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

const char* CaptchaBusiFinish::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

const char* CaptchaBusiFinish::visualize_ex(char* buffer, unsigned int size,
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
// BusiCaptchaPic (sizeof 4827)
// ---------------------------------------------------------------------------

TdrError::ErrorType BusiCaptchaPic::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 2985 timeout_ u32, 2995 verify_times_limit_, 3005 verify_times_,
// 3015 max_change_times_, 3025 change_times_, 3035 picture_len_ (u16),
// 3050 writeBytes(picture_) bound 4096 (cmp $0x1000), 3060 session_len_ u16,
// 3075 writeBytes(session_) bound 200 (cmp $0xc8), 3085 busi_id_ u8,
// 3095 busi_info_len_ u16, 3110 writeBytes(busi_info_) bound 512.
TdrError::ErrorType BusiCaptchaPic::pack(TdrWriteBuf& destBuf,
                                         unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(verify_times_limit_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(verify_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(max_change_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(change_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(picture_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (picture_len_ > 4096) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < picture_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(picture_, picture_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ > 200) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < session_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(session_, session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 2955, dtor at 2962.
TdrError::ErrorType BusiCaptchaPic::pack(char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType BusiCaptchaPic::unpack(TdrReadBuf& srcBuf,
                                           unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(verify_times_limit_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(verify_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(max_change_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(change_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(picture_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (picture_len_ > 4096) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < picture_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(picture_, picture_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ > 200) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < session_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(session_, session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < busi_info_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(busi_info_, busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType BusiCaptchaPic::unpack(const char* buffer,
                                           unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull at 3389 (after picture_), 3425 (after session_)
// and 3469 (after busi_info_); timeout_ "%u", the five u16 and busi_info_len_
// "%d", busi_id_ "0x%02x", picture_/session_ elements " %d" (uint8_t, the
// session_ loop indexed by a uint16_t counter), busi_info_ elements
// " 0x%02x" (int8_t).
TdrError::ErrorType BusiCaptchaPic::visualize(TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[timeout_]",
                                    "%u", timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[verify_times_limit_]", "%d",
                                    verify_times_limit_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[verify_times_]", "%d", verify_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_change_times_]", "%d",
                                    max_change_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[change_times_]", "%d", change_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[picture_len_]", "%d", picture_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (picture_len_ > 4096) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[picture_]",
                                 picture_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (picture_len_ != 0) {
        for (uint16_t i = 0; i < picture_len_; ++i) {
            ret = destBuf.textize(" %d", picture_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[session_len_]", "%d", session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ > 200) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[session_]",
                                 session_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (session_len_ != 0) {
        for (uint16_t i = 0; i < session_len_; ++i) {
            ret = destBuf.textize(" %d", session_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[busi_id_]",
                                    "0x%02x", busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[busi_info_len_]", "%d", busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ > 512) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[busi_info_]",
                                 busi_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (busi_info_len_ != 0) {
        for (uint16_t i = 0; i < busi_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", busi_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 3305, dtor at 3312.
TdrError::ErrorType BusiCaptchaPic::visualize(char* buffer, unsigned int size,
                                              unsigned int* usedSize, int indent,
                                              char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 3516 getUsedSize, 3517 getTotalSize, 3522 writeChar('\0', pos),
// 3524 getBeginPtr.
const char* BusiCaptchaPic::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 3487, getUsedSize at 3490, getBeginPtr/dtor at 3503.
const char* BusiCaptchaPic::visualize_ex(char* buffer, unsigned int size,
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
// TSSCaptchaOldBody (anonymous union, sizeof 4827)
// ---------------------------------------------------------------------------

// ORIG: selector 1 -> busi_captcha_req_, 3073 -> captcha_req_,
// 3075 -> verify_req_, 3076 -> verify_res_, 3078 -> s2c_verify_res_,
// 3090 -> busi_captcha_pic_, 3091 -> report_log_, 3092 -> captcha_busi_finish_;
// the selector is compared as a 64-bit value and an unknown selector returns
// TDR_NO_ERROR.
TdrError::ErrorType TSSCaptchaOldBody::construct(int64_t union_selector) {
    if (union_selector == 1) {
        return busi_captcha_req_.construct();
    } else if (union_selector == 3073) {
        return captcha_req_.construct();
    } else if (union_selector == 3075) {
        return verify_req_.construct();
    } else if (union_selector == 3076) {
        return verify_res_.construct();
    } else if (union_selector == 3078) {
        return s2c_verify_res_.construct();
    } else if (union_selector == 3090) {
        return busi_captcha_pic_.construct();
    } else if (union_selector == 3091) {
        return report_log_.construct();
    } else if (union_selector == 3092) {
        return captcha_busi_finish_.construct();
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG: every arm is packed with cutVer 1 (movl $0x1 into the third arg).
TdrError::ErrorType TSSCaptchaOldBody::pack(int64_t union_selector,
                                            TdrWriteBuf& destBuf,
                                            unsigned int cutVer) const throw() {
    if (union_selector == 1) {
        return busi_captcha_req_.pack(destBuf, 1);
    } else if (union_selector == 3073) {
        return captcha_req_.pack(destBuf, 1);
    } else if (union_selector == 3075) {
        return verify_req_.pack(destBuf, 1);
    } else if (union_selector == 3076) {
        return verify_res_.pack(destBuf, 1);
    } else if (union_selector == 3078) {
        return s2c_verify_res_.pack(destBuf, 1);
    } else if (union_selector == 3090) {
        return busi_captcha_pic_.pack(destBuf, 1);
    } else if (union_selector == 3091) {
        return report_log_.pack(destBuf, 1);
    } else if (union_selector == 3092) {
        return captcha_busi_finish_.pack(destBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: ctor at 3618, dtor at 3625; the wrapper's cutVer is forwarded.
TdrError::ErrorType TSSCaptchaOldBody::pack(int64_t union_selector,
                                            char* buffer, unsigned int size,
                                            unsigned int* usedSize,
                                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType TSSCaptchaOldBody::unpack(int64_t union_selector,
                                              TdrReadBuf& srcBuf,
                                              unsigned int cutVer) {
    if (union_selector == 1) {
        return busi_captcha_req_.unpack(srcBuf, 1);
    } else if (union_selector == 3073) {
        return captcha_req_.unpack(srcBuf, 1);
    } else if (union_selector == 3075) {
        return verify_req_.unpack(srcBuf, 1);
    } else if (union_selector == 3076) {
        return verify_res_.unpack(srcBuf, 1);
    } else if (union_selector == 3078) {
        return s2c_verify_res_.unpack(srcBuf, 1);
    } else if (union_selector == 3090) {
        return busi_captcha_pic_.unpack(srcBuf, 1);
    } else if (union_selector == 3091) {
        return report_log_.unpack(srcBuf, 1);
    } else if (union_selector == 3092) {
        return captcha_busi_finish_.unpack(srcBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: ctor at 3731.
TdrError::ErrorType TSSCaptchaOldBody::unpack(int64_t union_selector,
                                              const char* buffer,
                                              unsigned int size,
                                              unsigned int* usedSize,
                                              unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG: printVariable(..., "[<arm>_]", true) then the arm's visualize with
// (indent >= 0 ? indent + 1 : indent); unknown selector -> TDR_NO_ERROR.
TdrError::ErrorType TSSCaptchaOldBody::visualize(int64_t union_selector,
                                                 TdrWriteBuf& destBuf,
                                                 int indent,
                                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (union_selector == 1) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[busi_captcha_req_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = busi_captcha_req_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3073) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[captcha_req_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = captcha_req_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3075) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[verify_req_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = verify_req_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3076) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[verify_res_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = verify_res_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3078) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[s2c_verify_res_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = s2c_verify_res_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3090) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[busi_captcha_pic_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = busi_captcha_pic_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3091) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[report_log_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = report_log_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3092) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[captcha_busi_finish_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = captcha_busi_finish_.visualize(
            destBuf, indent >= 0 ? indent + 1 : indent, separator);
    }
    return ret;
}

// ORIG .cpp: ctor at 3844, dtor at 3851.
TdrError::ErrorType TSSCaptchaOldBody::visualize(int64_t union_selector,
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
// TSSCaptchaOldPkg (sizeof 4851)
// ---------------------------------------------------------------------------

// ORIG: movl $0x12f3 (4851) into the memset size.
TdrError::ErrorType TSSCaptchaOldPkg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG: head_.pack(destBuf, 1); bodyStart = getUsedSize();
// body_.pack((int64_t)head_.msg_id_, destBuf, 1); then
// writeUInt16((uint16_t)(getUsedSize() - bodyStart), startPos + 22) - offset 22
// is head_.body_len_.
TdrError::ErrorType TSSCaptchaOldPkg::pack(TdrWriteBuf& destBuf,
                                           unsigned int cutVer) const throw() {
    unsigned int startPos = destBuf.getUsedSize();
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    unsigned int bodyStart = destBuf.getUsedSize();
    ret = body_.pack(head_.msg_id_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16((uint16_t)(destBuf.getUsedSize() - bodyStart),
                              startPos + 22);
    return ret;
}

// ORIG .cpp: ctor at 4067, dtor at 4074.
TdrError::ErrorType TSSCaptchaOldPkg::pack(char* buffer, unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG: head_.unpack(srcBuf, 1); then
// `if (head_.body_len_ > srcBuf.capacity() - srcBuf.size()) return
// TDR_ERR_SHORT_BUF_FOR_READ;` then body_.unpack(head_.msg_id_, srcBuf, 1).
TdrError::ErrorType TSSCaptchaOldPkg::unpack(TdrReadBuf& srcBuf,
                                             unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (head_.body_len_ > srcBuf.capacity() - srcBuf.size()) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = body_.unpack(head_.msg_id_, srcBuf, 1);
    return ret;
}

TdrError::ErrorType TSSCaptchaOldPkg::unpack(const char* buffer,
                                             unsigned int size,
                                             unsigned int* usedSize,
                                             unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG: printVariable "[head_]" (bool form), head_.visualize, printVariable
// "[body_]" (bool form), body_.visualize(head_.msg_id_, ...), each nested call
// with indent >= 0 ? indent + 1 : indent.
TdrError::ErrorType TSSCaptchaOldPkg::visualize(TdrWriteBuf& destBuf,
                                                int indent,
                                                char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head_]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = head_.visualize(destBuf, indent >= 0 ? indent + 1 : indent,
                          separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.visualize(head_.msg_id_, destBuf,
                          indent >= 0 ? indent + 1 : indent, separator);
    return ret;
}

// ORIG .cpp: ctor at 4216, dtor at 4223.
TdrError::ErrorType TSSCaptchaOldPkg::visualize(char* buffer,
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

// ORIG .cpp: 4316 getUsedSize, 4317 getTotalSize, 4322 writeChar('\0', pos),
// 4324 getBeginPtr.
const char* TSSCaptchaOldPkg::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                           char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 4287, getUsedSize at 4290, getBeginPtr/dtor at 4303.
const char* TSSCaptchaOldPkg::visualize_ex(char* buffer, unsigned int size,
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
