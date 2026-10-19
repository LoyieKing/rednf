// Reconstruction of src/protocol/formmog/formmog_proto_captcha_client.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function below is one of the 61 sec_proto symbols the ORIG DWARF
// attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_captcha_client.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, weak in the ORIG; weak here
// so each CU of the library can carry its own comdat copy).
//
// The "ORIG .cpp:NNN" comments are DW_AT_call_line values of the inlined
// tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py), i.e. the
// ORIG's statement order.  The generator emits, per class,
//   construct(), pack(char*), pack(TdrWriteBuf&), unpack(char const*),
//   unpack(TdrReadBuf&), visualize(char*), visualize(TdrWriteBuf&),
//   visualize_ex(char*), visualize_ex(TdrWriteBuf&)
// with a 10-line stride per scalar field and 15 for a length+byte-array pair;
// TSSCaptchaBody (the anonymous union) is emitted after the four arms and
// TSSCaptchaPkg last.  The class blocks the cited lines delimit are, in file
// order: TSSCaptchaHead 43..326, TSSSetCaptcha 349..746, TSSCaptchaPic
// 769..1252, TSSVerifyReq 1275..1504, TSSVerifyRes 1527..1784,
// TSSCaptchaBody 1842..2003, TSSCaptchaPkg 2127..2384.
//
// Recovered protocol facts (all measured in the ORIG machine code):
//  * union selectors are the message ids: 3201 set_captcha_, 3202 captcha_pic_,
//    3203 verify_req_, 3204 captcha_res_; unknown selectors return TDR_NO_ERROR.
//  * every nested call in this CU is version 1 (the head has no version field);
//    TSSCaptchaPkg::pack/unpack pass the literal 1 to head_/body_ and use
//    head_.msg_id_ as the union selector.
//  * TSSCaptchaPkg::pack stamps head_.body_len_ (offset 14) with
//    (uint16_t)(getUsedSize() - bodyStartPos) at startPos + 14; the ORIG does
//    the two-step (pos > limit_, limit_ - pos <= 1) capacity check.
//  * TSSCaptchaPkg::unpack rejects head_.body_len_ larger than the remaining
//    buffer with TDR_ERR_SHORT_BUF_FOR_READ before unpacking the body.
//  * visualize formats follow the ORIG's per-field spec, which is not purely
//    type-derived: u32 -> "%u", u16 -> "%d", u8 -> "0x%02x", uint8_t arrays ->
//    " %d", int8_t arrays -> " 0x%02x" (verified per call site).
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -19 TDR_ERR_NULL_POINTER_PARAMETER.

#include "formmog_proto_captcha_client.h"

#include <string.h>

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, weak.  textize() forwards to the inlined
// vtextize(format, ap) at TdrBuf.h:182; the ORIG declaration is inline, hence
// the comdat/weak binding reproduced with __attribute__((weak)).
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
// TSSCaptchaHead (sizeof 16)
// ---------------------------------------------------------------------------

// ORIG: movl $0x0 at +0/+4/+8/+0xc = the memset(16) expansion.
TdrError::ErrorType TSSCaptchaHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 73 msg_id_ u16, 83 uin_ u32, 93 busi_seq_ u32, 103 seq_ u32,
// 113 body_len_ u16.  cutVer is not read.
TdrError::ErrorType TSSCaptchaHead::pack(TdrWriteBuf& destBuf,
                                         unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(busi_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 43, dtor at 50.
TdrError::ErrorType TSSCaptchaHead::pack(char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 163 msg_id_, 173 uin_, 183 busi_seq_, 193 seq_, 203 body_len_.
TdrError::ErrorType TSSCaptchaHead::unpack(TdrReadBuf& srcBuf,
                                           unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(busi_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 133.
TdrError::ErrorType TSSCaptchaHead::unpack(const char* buffer, unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: msg_id_ "%d", uin_/busi_seq_/seq_ "%u", body_len_ "%d"
// (printVariable call sites at 271 ff).
TdrError::ErrorType TSSCaptchaHead::visualize(TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[msg_id_]",
                                    "%d", msg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u",
                                    uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[busi_seq_]",
                                    "%u", busi_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_]", "%u",
                                    seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_len_]",
                                    "%d", body_len_);
    return ret;
}

// ORIG .cpp: ctor at 223, dtor at 230.
TdrError::ErrorType TSSCaptchaHead::visualize(char* buffer, unsigned int size,
                                              unsigned int* usedSize, int indent,
                                              char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 318 getUsedSize, 319 getTotalSize, 324 writeChar('\0', pos),
// 326 getBeginPtr.
const char* TSSCaptchaHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 289, getUsedSize at 292, getBeginPtr/dtor at 305.
const char* TSSCaptchaHead::visualize_ex(char* buffer, unsigned int size,
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
// TSSSetCaptcha (sizeof 531)
// ---------------------------------------------------------------------------

TdrError::ErrorType TSSSetCaptcha::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 379 client_ip_ u32, 389 platform_busid_ u32,
// 399 max_pic_change_num_ u16, 409 max_verify_num_ u16, 419 timeout_ u32,
// 429 busi_id_ u8, 439 busi_info_len_ u16, 454 writeBytes(busi_info_).
TdrError::ErrorType TSSSetCaptcha::pack(TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(platform_busid_);
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

// ORIG .cpp: ctor at 349, dtor at 356.
TdrError::ErrorType TSSSetCaptcha::pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 504 client_ip_, 514 platform_busid_, 524 max_pic_change_num_,
// 534 max_verify_num_, 544 timeout_, 554 busi_id_, 564 busi_info_len_,
// 579 readBytes(busi_info_).
TdrError::ErrorType TSSSetCaptcha::unpack(TdrReadBuf& srcBuf,
                                          unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(platform_busid_);
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

// ORIG .cpp: ctor at 474.
TdrError::ErrorType TSSSetCaptcha::unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 691 (after the busi_info_ dump);
// client_ip_/platform_busid_/timeout_ "%u", max_pic_change_num_/
// max_verify_num_/busi_info_len_ "%d", busi_id_ "0x%02x", busi_info_ elements
// " 0x%02x" (int8_t, indexed with a uint16_t counter).
TdrError::ErrorType TSSSetCaptcha::visualize(TdrWriteBuf& destBuf, int indent,
                                             char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]",
                                    "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[platform_busid_]", "%u", platform_busid_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_pic_change_num_]", "%d",
                                    max_pic_change_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[max_verify_num_]", "%d", max_verify_num_);
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

// ORIG .cpp: ctor at 599, dtor at 606.
TdrError::ErrorType TSSSetCaptcha::visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 738 getUsedSize, 739 getTotalSize, 744 writeChar('\0', pos),
// 746 getBeginPtr.
const char* TSSSetCaptcha::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                        char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 709, getUsedSize at 712, getBeginPtr/dtor at 725.
const char* TSSSetCaptcha::visualize_ex(char* buffer, unsigned int size,
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
// TSSCaptchaPic (sizeof 4625)
// ---------------------------------------------------------------------------

TdrError::ErrorType TSSCaptchaPic::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 799 timeout_ u32, 809/819/829/839/849 the five u16 counters,
// 864 writeBytes(picture_) bounded by 4096, 874 busi_id_ u8,
// 884 busi_info_len_ u16, 899 writeBytes(busi_info_) bounded by 512.
TdrError::ErrorType TSSCaptchaPic::pack(TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(verify_times_limit_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(verify_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(change_times_limit_);
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

// ORIG .cpp: ctor at 769, dtor at 776.
TdrError::ErrorType TSSCaptchaPic::pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 949 timeout_, 959..999 the u16 counters, 1014 readBytes(picture_),
// 1024 busi_id_, 1034 busi_info_len_, 1049 readBytes(busi_info_).
TdrError::ErrorType TSSCaptchaPic::unpack(TdrReadBuf& srcBuf,
                                          unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(verify_times_limit_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(verify_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(change_times_limit_);
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

// ORIG .cpp: ctor at 919.
TdrError::ErrorType TSSCaptchaPic::unpack(const char* buffer, unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 1153 (after the picture_ dump)
// and 1197 (after busi_info_).  timeout_ "%u", the five counters and
// busi_info_len_ "%d", busi_id_ "0x%02x", picture_ elements " %d" (uint8_t),
// busi_info_ elements " 0x%02x" (int8_t).
TdrError::ErrorType TSSCaptchaPic::visualize(TdrWriteBuf& destBuf, int indent,
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
                                    "[change_times_limit_]", "%d",
                                    change_times_limit_);
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

// ORIG .cpp: ctor at 1069, dtor at 1076.
TdrError::ErrorType TSSCaptchaPic::visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1244 getUsedSize, 1245 getTotalSize, 1250 writeChar('\0', pos),
// 1252 getBeginPtr.
const char* TSSCaptchaPic::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                        char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1215, getUsedSize at 1218, getBeginPtr/dtor at 1231.
const char* TSSCaptchaPic::visualize_ex(char* buffer, unsigned int size,
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
// TSSVerifyReq (sizeof 65)
// ---------------------------------------------------------------------------

TdrError::ErrorType TSSVerifyReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 1305 writeUInt8(answer_len_), 1320 writeBytes(answer_) bounded by
// sizeof(answer_) (cmp $0x40).
TdrError::ErrorType TSSVerifyReq::pack(TdrWriteBuf& destBuf,
                                       unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(answer_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (answer_len_ > 64) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < answer_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(answer_, answer_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1275, dtor at 1282.
TdrError::ErrorType TSSVerifyReq::pack(char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1370 readUInt8(answer_len_), 1385 readBytes(answer_).
TdrError::ErrorType TSSVerifyReq::unpack(TdrReadBuf& srcBuf,
                                         unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(answer_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (answer_len_ > 64) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < answer_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(answer_, answer_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1340.
TdrError::ErrorType TSSVerifyReq::unpack(const char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 1449; answer_len_ "0x%02x",
// answer_ elements " %d" (uint8_t, uint8_t counter).
TdrError::ErrorType TSSVerifyReq::visualize(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[answer_len_]", "0x%02x", answer_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (answer_len_ > 64) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[answer_]",
                                 answer_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (answer_len_ != 0) {
        for (uint8_t i = 0; i < answer_len_; ++i) {
            ret = destBuf.textize(" %d", answer_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 1405, dtor at 1412.
TdrError::ErrorType TSSVerifyReq::visualize(char* buffer, unsigned int size,
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
const char* TSSVerifyReq::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                       char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1467, getUsedSize at 1470, getBeginPtr/dtor at 1483.
const char* TSSVerifyReq::visualize_ex(char* buffer, unsigned int size,
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
// TSSVerifyRes (sizeof 130)
// ---------------------------------------------------------------------------

TdrError::ErrorType TSSVerifyRes::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 1557 writeUInt8(result_), 1567 writeUInt8(msg_len_),
// 1582 writeBytes(msg_) bounded by sizeof(msg_) (cmp $0x80).
TdrError::ErrorType TSSVerifyRes::pack(TdrWriteBuf& destBuf,
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

// ORIG .cpp: ctor at 1527, dtor at 1534.
TdrError::ErrorType TSSVerifyRes::pack(char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1632 readUInt8(result_), 1642 readUInt8(msg_len_),
// 1657 readBytes(msg_).
TdrError::ErrorType TSSVerifyRes::unpack(TdrReadBuf& srcBuf,
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

// ORIG .cpp: ctor at 1602.
TdrError::ErrorType TSSVerifyRes::unpack(const char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 1729; result_ and msg_len_
// "0x%02x", msg_ elements " 0x%02x" (int8_t).
TdrError::ErrorType TSSVerifyRes::visualize(TdrWriteBuf& destBuf, int indent,
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

// ORIG .cpp: ctor at 1677, dtor at 1684.
TdrError::ErrorType TSSVerifyRes::visualize(char* buffer, unsigned int size,
                                            unsigned int* usedSize, int indent,
                                            char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1776 getUsedSize, 1777 getTotalSize, 1782 writeChar('\0', pos),
// 1784 getBeginPtr.
const char* TSSVerifyRes::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                       char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1747, getUsedSize at 1750, getBeginPtr/dtor at 1763.
const char* TSSVerifyRes::visualize_ex(char* buffer, unsigned int size,
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
// TSSCaptchaBody (anonymous union, sizeof 4625)
// ---------------------------------------------------------------------------

// ORIG .cpp: selector == 0x0c81 -> set_captcha_, 0x0c82 -> captcha_pic_,
// 0x0c83 -> verify_req_, 0x0c84 -> captcha_res_; the selector is compared as
// a 64-bit value ((low32 ^ id) | high32) and an unknown selector returns
// TDR_NO_ERROR.
TdrError::ErrorType TSSCaptchaBody::construct(int64_t union_selector) {
    if (union_selector == 3201) {
        return set_captcha_.construct();
    } else if (union_selector == 3202) {
        return captcha_pic_.construct();
    } else if (union_selector == 3203) {
        return verify_req_.construct();
    } else if (union_selector == 3204) {
        return captcha_res_.construct();
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: every arm is packed with cutVer 1 (movl $0x1 into the third arg).
TdrError::ErrorType TSSCaptchaBody::pack(int64_t union_selector,
                                         TdrWriteBuf& destBuf,
                                         unsigned int cutVer) const throw() {
    if (union_selector == 3201) {
        return set_captcha_.pack(destBuf, 1);
    } else if (union_selector == 3202) {
        return captcha_pic_.pack(destBuf, 1);
    } else if (union_selector == 3203) {
        return verify_req_.pack(destBuf, 1);
    } else if (union_selector == 3204) {
        return captcha_res_.pack(destBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: ctor at 1842, dtor at 1849; the wrapper's own cutVer is forwarded.
TdrError::ErrorType TSSCaptchaBody::pack(int64_t union_selector, char* buffer,
                                         unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

TdrError::ErrorType TSSCaptchaBody::unpack(int64_t union_selector,
                                           TdrReadBuf& srcBuf,
                                           unsigned int cutVer) {
    if (union_selector == 3201) {
        return set_captcha_.unpack(srcBuf, 1);
    } else if (union_selector == 3202) {
        return captcha_pic_.unpack(srcBuf, 1);
    } else if (union_selector == 3203) {
        return verify_req_.unpack(srcBuf, 1);
    } else if (union_selector == 3204) {
        return captcha_res_.unpack(srcBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: ctor at 1919.
TdrError::ErrorType TSSCaptchaBody::unpack(int64_t union_selector,
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
TdrError::ErrorType TSSCaptchaBody::visualize(int64_t union_selector,
                                              TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (union_selector == 3201) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[set_captcha_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = set_captcha_.visualize(destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3202) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[captcha_pic_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = captcha_pic_.visualize(destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3203) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[verify_req_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = verify_req_.visualize(destBuf, indent >= 0 ? indent + 1 : indent, separator);
    } else if (union_selector == 3204) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[captcha_res_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = captcha_res_.visualize(destBuf, indent >= 0 ? indent + 1 : indent, separator);
    }
    return ret;
}

// ORIG .cpp: ctor at 1996, dtor at 2003.
TdrError::ErrorType TSSCaptchaBody::visualize(int64_t union_selector,
                                              char* buffer, unsigned int size,
                                              unsigned int* usedSize, int indent,
                                              char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent,
                                        separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ---------------------------------------------------------------------------
// TSSCaptchaPkg (sizeof 4641)
// ---------------------------------------------------------------------------

// ORIG: movl $0x1221 (4641) into the memset size.
TdrError::ErrorType TSSCaptchaPkg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 2155 startPos, 2174 bodyStart, 2184/2185 the head_.body_len_
// stamp at startPos + 14.  Both nested calls get the literal version 1, and
// the union selector is head_.msg_id_.
TdrError::ErrorType TSSCaptchaPkg::pack(TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw() {
    unsigned int startPos = destBuf.getUsedSize();
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    unsigned int bodyStart = destBuf.getUsedSize();
    ret = body_.pack(head_.msg_id_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16((uint16_t)(destBuf.getUsedSize() - bodyStart),
                              startPos + 14);
    return ret;
}

// ORIG .cpp: ctor at 2127, dtor at 2134.
TdrError::ErrorType TSSCaptchaPkg::pack(char* buffer, unsigned int size,
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
TdrError::ErrorType TSSCaptchaPkg::unpack(TdrReadBuf& srcBuf,
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

// ORIG .cpp: ctor at 2205.
TdrError::ErrorType TSSCaptchaPkg::unpack(const char* buffer, unsigned int size,
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
TdrError::ErrorType TSSCaptchaPkg::visualize(TdrWriteBuf& destBuf, int indent,
                                             char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head_]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = head_.visualize(destBuf, indent >= 0 ? indent + 1 : indent, separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.visualize(head_.msg_id_, destBuf, indent >= 0 ? indent + 1 : indent, separator);
    return ret;
}

// ORIG .cpp: ctor at 2276, dtor at 2283.
TdrError::ErrorType TSSCaptchaPkg::visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize, int indent,
                                             char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 2376 getUsedSize, 2377 getTotalSize, 2382 writeChar('\0', pos),
// 2384 getBeginPtr.
const char* TSSCaptchaPkg::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                        char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 2347, getUsedSize at 2350, getBeginPtr/dtor at 2363.
const char* TSSCaptchaPkg::visualize_ex(char* buffer, unsigned int size,
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
