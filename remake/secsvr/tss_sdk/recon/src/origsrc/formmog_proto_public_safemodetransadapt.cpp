// Reconstruction of src/protocol/formmog/formmog_proto_public_safemodetransadapt.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function below is one of the 45 sec_proto symbols the ORIG DWARF
// attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_public_safemodetransadapt.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, the CU's only out-of-line
// tsf4g_tdr definition; weak comdat symbol in the ORIG, weak here too so that
// the other CUs of the library can each carry their own copy).
//
// The "ORIG .cpp:NNN" comments are DW_AT_call_line values of the inlined
// tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py), so the
// call sequence below is the ORIG's statement order, not an inference from the
// generated header.  The generator lays a class out as
//   construct(), pack(char*), pack(TdrWriteBuf&), unpack(char const*),
//   unpack(TdrReadBuf&), visualize(char*), visualize(TdrWriteBuf&),
//   visualize_ex(char*), visualize_ex(TdrWriteBuf&)
// with a fixed 10-line stride per field, which is why the cited line numbers
// jump by 10 inside pack()/unpack().
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -19 TDR_ERR_NULL_POINTER_PARAMETER.

#include "formmog_proto_public_safemodetransadapt.h"

#include <string.h>

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, weak (`W` in nm -DC of the ORIG).  textize()
// forwards to the inlined vtextize(format, ap) at TdrBuf.h:182; the ORIG's
// declaration is inline, hence the comdat/weak binding.  __attribute__((weak))
// reproduces that binding - the ORIG's library keeps exactly one copy of the
// weak symbol at 0x00136e90 for all CUs that use it.
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
// ReqSafemodeTransAdaptLoginNotify (sizeof 14)
// ---------------------------------------------------------------------------

// ORIG .cpp: the generator's construct() zeroes the whole object.
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 73/83/93/103 (u16, u32, u32, u32 - one 10-line block per field).
// cutVer is not read anywhere in this pack().
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 43, dtor at 50.
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 153/163/173/183.
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 123.
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: world_id_ "%d", src_id_/client_ip_/login_time_ "%u"
// (printVariable at .cpp:1139 ff).
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]",
                                    "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]",
                                    "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[login_time_]", "%u", login_time_);
    return ret;
}

// ORIG .cpp: ctor at 203, dtor at 210.
TdrError::ErrorType ReqSafemodeTransAdaptLoginNotify::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 290 getUsedSize, 291 getTotalSize, 296 writeChar('\0', pos),
// 298 getBeginPtr.
const char* ReqSafemodeTransAdaptLoginNotify::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 261, getUsedSize at 264, getBeginPtr/dtor at 277.
const char* ReqSafemodeTransAdaptLoginNotify::visualize_ex(
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

// ---------------------------------------------------------------------------
// RspSafemodeTransAdaptStatUpdate (sizeof 7)
// ---------------------------------------------------------------------------
// ORIG: movl/movw/movb (4+2+1) = the memset(7) expansion.
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 351 u16, 361 u32, 371 u8 (single-byte write needs only pos == limit).
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(stat_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 321, dtor at 328.
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 421 u16, 431 u32, 441 u8.
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(stat_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 391.
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: world_id_ "%d", src_id_ "%u", stat_level_ "0x%02x".
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]",
                                    "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[stat_level_]", "0x%02x", stat_level_);
    return ret;
}

// ORIG .cpp: ctor at 461, dtor at 468.
TdrError::ErrorType RspSafemodeTransAdaptStatUpdate::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 540 getUsedSize, 541 getTotalSize, 546 writeChar('\0', pos),
// 548 getBeginPtr.
const char* RspSafemodeTransAdaptStatUpdate::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 511, getUsedSize at 514, getBeginPtr/dtor at 527.
const char* RspSafemodeTransAdaptStatUpdate::visualize_ex(
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

// ---------------------------------------------------------------------------
// ReqSafemodeTransAdaptEventNotify (sizeof 12)
// ---------------------------------------------------------------------------
// ORIG: movl $0x0 at +0/+4/+8 = the memset(12) expansion.
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 601 u16, 611 u32, 621 u16, 631 u32.
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(event_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(event_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 571, dtor at 578.
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 681 u16, 691 u32, 701 u16, 711 u32.
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(event_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(event_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 651.
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: world_id_ "%d", src_id_ "%u", event_type_ "%d", event_value_ "%u"
// (the ORIG reuses one format pointer per width: the same "%d" literal is
// registered for world_id_ and event_type_).
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]",
                                    "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[event_type_]",
                                    "%d", event_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[event_value_]", "%u", event_value_);
    return ret;
}

// ORIG .cpp: ctor at 731, dtor at 738.
TdrError::ErrorType ReqSafemodeTransAdaptEventNotify::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 818 getUsedSize, 819 getTotalSize, 824 writeChar('\0', pos),
// 826 getBeginPtr.
const char* ReqSafemodeTransAdaptEventNotify::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 789, getUsedSize at 792, getBeginPtr/dtor at 805.
const char* ReqSafemodeTransAdaptEventNotify::visualize_ex(
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

// ---------------------------------------------------------------------------
// ReqSafemodeTransAdaptTradeNotify (sizeof 2073)
// ---------------------------------------------------------------------------

// ORIG: movl $0x819 (2073) into the memset size.
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 879 u16, 889 u32, 899 u8, 909/919/929/939 u32, 949 u16 buf_len_,
// 964 writeBytes(trade_buf_) - the byte array is bounded by sizeof(trade_buf_)
// (cmp $0x800) with -7 TDR_ERR_REFER_SURPASS_COUNT, then by the remaining
// capacity with -1 TDR_ERR_SHORT_BUF_FOR_WRITE.
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(send_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(target_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(send_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(target_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (buf_len_ > 2048) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < buf_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(trade_buf_, buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 849, dtor at 856.
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1014 u16, 1024 u32, 1034 u8, 1044/1054/1064/1074 u32, 1084 u16,
// 1099 readBytes(trade_buf_).
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(send_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(target_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(send_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(target_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (buf_len_ > 2048) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < buf_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(trade_buf_, buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 984.
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 1219 (after the trade_buf_ dump).
// Formats: world_id_/buf_len_ "%d", src_id_/send_uin_/target_uin_/send_ip_/
// target_ip_ "%u", type_ "0x%02x", array elements " 0x%02x"; the array dump is
// bounded by sizeof(trade_buf_) and the index is a uint16_t, as in the ORIG
// (movw/addw on the loop counter).
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]",
                                    "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[type_]",
                                    "0x%02x", type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_uin_]",
                                    "%u", send_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[target_uin_]",
                                    "%u", target_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_ip_]",
                                    "%u", send_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[target_ip_]",
                                    "%u", target_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[buf_len_]",
                                    "%d", buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (buf_len_ > 2048) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[trade_buf_]",
                                 buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (buf_len_ != 0) {
        for (uint16_t i = 0; i < buf_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", trade_buf_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 1119, dtor at 1126.
TdrError::ErrorType ReqSafemodeTransAdaptTradeNotify::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1266 getUsedSize, 1267 getTotalSize, 1272 writeChar('\0', pos),
// 1274 getBeginPtr.
const char* ReqSafemodeTransAdaptTradeNotify::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1237, getUsedSize at 1240, getBeginPtr/dtor at 1253.
const char* ReqSafemodeTransAdaptTradeNotify::visualize_ex(
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

// ---------------------------------------------------------------------------
// ReqSafemodeTransAdaptDataForward (sizeof 2056)
// ---------------------------------------------------------------------------

// ORIG: movl $0x808 (2056) into the memset size.
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG .cpp: 1327 u16, 1337 u32, 1347 u16, 1362 writeBytes(data_).
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 2048) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (destBuf.capacity() - destBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    ret = destBuf.writeBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1297, dtor at 1304.
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1412 u16, 1422 u32, 1432 u16, 1447 readBytes(data_).
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 2048) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (srcBuf.capacity() - srcBuf.size() < data_len_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    }
    ret = srcBuf.readBytes(data_, data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG .cpp: ctor at 1382.
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: writeCharWithNull(separator) at 1527 (after the data_ dump);
// world_id_/data_len_ "%d", src_id_ "%u", elements " 0x%02x".
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]",
                                    "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]",
                                    "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 2048) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_]",
                                 data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ != 0) {
        for (uint16_t i = 0; i < data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG .cpp: ctor at 1467, dtor at 1474.
TdrError::ErrorType ReqSafemodeTransAdaptDataForward::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG .cpp: 1574 getUsedSize, 1575 getTotalSize, 1580 writeChar('\0', pos),
// 1582 getBeginPtr.
const char* ReqSafemodeTransAdaptDataForward::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG .cpp: ctor at 1545, getUsedSize at 1548, getBeginPtr/dtor at 1561.
const char* ReqSafemodeTransAdaptDataForward::visualize_ex(
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

} // namespace sec_proto
