// Reconstruction of src/protocol/formmog/formmog_proto_public_uictransadapt.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function in this file is one of the 9 sec_proto symbols the ORIG
// linker attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_public_uictransadapt.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, the CU's only out-of-line
// tsf4g_tdr definition; weak in the ORIG and emitted by every CU whose DWARF
// attributes it).
//
// The "ORIG 0xADDR, N bytes" comments are the ORIG symbol table entries
// (nm -DC -S); the "ORIG .cpp:NNN" comments are DW_AT_call_line values of the
// inlined tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py), so
// the call sequence below is the ORIG's, not an inference from the header.
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -19 TDR_ERR_NULL_POINTER_PARAMETER.

#include "formmog_proto_public_uictransadapt.h"

#include <stdarg.h>
#include <stddef.h>

// Compile-time layout assertions: the DWARF byte size and member offsets of the
// ORIG CU (cu_layout.py, "## ReqUICTransAdaptMsgForwardPkg").  A violated
// assertion is a negative array bound, i.e. a compile error, so a clean build
// of this TU proves the reconstructed layout matches the ORIG's.
typedef char assert_uicta_size[
    sizeof(sec_proto::ReqUICTransAdaptMsgForwardPkg) == 33044 ? 1 : -1];
typedef char assert_uicta_src_id[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, src_id_) == 2 ? 1 : -1];
typedef char assert_uicta_cmd_id[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, cmd_id_) == 6 ? 1 : -1];
typedef char assert_uicta_is_encrypted[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg,
             is_encrypted_) == 11 ? 1 : -1];
typedef char assert_uicta_cmd_buf_len[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg,
             cmd_buf_len_) == 12 ? 1 : -1];
typedef char assert_uicta_cmd_buf[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg, cmd_buf_) == 14 ? 1 : -1];
typedef char assert_uicta_rolename_len[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg,
             rolename_len_) == 32782 ? 1 : -1];
typedef char assert_uicta_rolename[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg,
             rolename_) == 32784 ? 1 : -1];
typedef char assert_uicta_client_ip[
    offsetof(sec_proto::ReqUICTransAdaptMsgForwardPkg,
             client_ip_) == 33040 ? 1 : -1];

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, WEAK in the ORIG (nm -DC: 'W').  The ORIG's
// TdrBuf.h defines this varargs wrapper inside the class body, i.e. implicitly
// inline, so every CU that calls it emits its own weak comdat copy and the
// linker folds them; the calls stay out-of-line (a varargs callee is never
// inlined) and go through the PLT.  The compat header declares it out-of-line,
// so the definition repeats the specifier to reproduce the ORIG binding
// without touching the shared header: binding 'W' here, 'T' otherwise.
// textize() forwards to the inlined vtextize(format, ap) at TdrBuf.h:182.
inline TdrError::ErrorType TdrWriteBuf::textize(const char* format, ...) {
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
// ReqUICTransAdaptMsgForwardPkg (sizeof 33044)
// ---------------------------------------------------------------------------

// ORIG 0x001551c0, 0x35 bytes. memset(this, 0, sizeof(*this)): the ORIG passes
// size 0x8114 = 33044 to memset@plt.
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x001557b0, 0x26f bytes. ORIG .cpp: 73/83/93/103/113/123 field writes,
// 138 bulk cmd_buf_, 148 rolename_len_, 163 bulk rolename_, 173 client_ip_;
// cutVer is not read. Both bulk writes are guarded by
// `length > sizeof(array)` -> TDR_ERR_REFER_SURPASS_COUNT (ORIG: cmp $0x8000
// / cmp $0x100 followed by mov $0xfffffff9) before the capacity check the
// writeBytes primitive performs (ORIG: mov $0xffffffff, i.e. -1).
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::pack(
    TdrWriteBuf& destBuf, unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(cmd_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_buf_len_ > 32768) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = destBuf.writeBytes(cmd_buf_, cmd_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rolename_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rolename_len_ > 256) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = destBuf.writeBytes(rolename_, rolename_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00155a20, 0x85 bytes. ORIG .cpp: ctor at 43, dtor at 50.
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::pack(
    char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00155ab0, 0x253 bytes. ORIG .cpp: 223/233/243/253/263/273 field reads,
// 288 bulk cmd_buf_, 298 rolename_len_, 313 bulk rolename_, 323 client_ip_.
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::unpack(
    TdrReadBuf& srcBuf, unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(cmd_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_buf_len_ > 32768) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = srcBuf.readBytes(cmd_buf_, cmd_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rolename_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rolename_len_ > 256) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = srcBuf.readBytes(rolename_, rolename_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00155d10, 0x66 bytes. ORIG .cpp: ctor at 193.
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::unpack(
    const char* buffer, unsigned int size, unsigned int* usedSize,
    unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00155200, 0x388 bytes. Field names and formats are the ORIG's own
// .rodata references: [world_id_] "%d", [src_id_] "%u", [cmd_id_] "%u",
// [need_encrypt_] "0x%02x", [is_encrypted_] "0x%02x", [cmd_buf_len_] "%d",
// printArray [cmd_buf_] then a textize(" %d") loop, [rolename_len_] "%d",
// printArray [rolename_] then a textize(" 0x%02x") loop, [client_ip_] "%u".
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::visualize(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]",
                                    "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]",
                                    "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_id_]",
                                    "%u", cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[need_encrypt_]", "0x%02x",
                                    need_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[is_encrypted_]", "0x%02x",
                                    is_encrypted_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[cmd_buf_len_]", "%d", cmd_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_buf_len_ > 32768) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cmd_buf_]",
                                 cmd_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_buf_len_ != 0) {
        for (uint32_t i = 0; i < cmd_buf_len_; ++i) {
            ret = destBuf.textize(" %d", cmd_buf_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[rolename_len_]", "%d", rolename_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rolename_len_ > 256) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rolename_]",
                                 rolename_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rolename_len_ != 0) {
        for (uint32_t i = 0; i < rolename_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", rolename_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]",
                                    "%u", client_ip_);
    return ret;
}

// ORIG 0x001556f0, 0xb6 bytes. ORIG .cpp: ctor at 343, dtor at 350.
TdrError::ErrorType ReqUICTransAdaptMsgForwardPkg::visualize(
    char* buffer, unsigned int size, unsigned int* usedSize, int indent,
    char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00155590, 0x8b bytes. ORIG .cpp: 518 getUsedSize, 519 getTotalSize,
// 524 writeChar('\0', pos), 526 getBeginPtr.
const char* ReqUICTransAdaptMsgForwardPkg::visualize_ex(
    TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00155620, 0xc9 bytes. ORIG .cpp: ctor at 489, getUsedSize at 492,
// getBeginPtr/dtor at 505.
const char* ReqUICTransAdaptMsgForwardPkg::visualize_ex(
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
