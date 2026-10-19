// Reconstruction of src/protocol/formmog/formmog_proto_tss_sdk_client.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function in this file is one of the 256 sec_proto symbols the ORIG
// linker attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_tss_sdk_client.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, the CU's only out-of-line
// tsf4g_tdr definition).
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -7 TDR_ERR_REFER_SURPASS_COUNT,
// -8 TDR_ERR_ARG_POINTER_IS_NULL, -19 TDR_ERR_NULL_POINTER_PARAMETER.
//
// No structure of this CU is versioned: every pack()/unpack() ignores its
// cutVer argument and passes the literal 1 to its children (the ORIG never
// reads the cutVer stack slot in those bodies).

#include "formmog_proto_tss_sdk_client.h"

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

// ---------------------------------------------------------------------
// DPClientCode
// ---------------------------------------------------------------------

// ORIG 0x00155e30, 0x10 bytes: DPClientCode::construct()
TdrError::ErrorType DPClientCode::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015ab60, 0x61 bytes: DPClientCode::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPClientCode::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015deb0, 0x85 bytes: DPClientCode::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPClientCode::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015fab0, 0x50 bytes: DPClientCode::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPClientCode::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015fb00, 0x66 bytes: DPClientCode::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPClientCode::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156410, 0x4f bytes: DPClientCode::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPClientCode::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[code_id_]", "%u", code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015ddf0, 0xb6 bytes: DPClientCode::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPClientCode::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157c90, 0x8b bytes: DPClientCode::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPClientCode::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015c660, 0xc9 bytes: DPClientCode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPClientCode::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPBinCount
// ---------------------------------------------------------------------

// ORIG 0x00155e40, 0xf bytes: DPBinCount::construct()
TdrError::ErrorType DPBinCount::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00158400, 0x4e bytes: DPBinCount::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPBinCount::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(bin_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e0d0, 0x85 bytes: DPBinCount::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPBinCount::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159f50, 0x3a bytes: DPBinCount::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPBinCount::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(bin_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00159f90, 0x66 bytes: DPBinCount::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPBinCount::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156460, 0x50 bytes: DPBinCount::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPBinCount::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_count_]", "%d", bin_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e010, 0xb6 bytes: DPBinCount::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPBinCount::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157d20, 0x8b bytes: DPBinCount::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPBinCount::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015df40, 0xc9 bytes: DPBinCount::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPBinCount::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPBinFile
// ---------------------------------------------------------------------

// ORIG 0x00155e50, 0x10 bytes: DPBinFile::construct()
TdrError::ErrorType DPBinFile::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00158450, 0x71 bytes: DPBinFile::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPBinFile::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(length);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bin_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e2f0, 0x85 bytes: DPBinFile::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPBinFile::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015a000, 0x63 bytes: DPBinFile::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPBinFile::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(length);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(bin_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015a070, 0x66 bytes: DPBinFile::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPBinFile::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001564b0, 0x92 bytes: DPBinFile::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPBinFile::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[length]", "%d", length);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_code_id_]", "%d", bin_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e230, 0xb6 bytes: DPBinFile::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPBinFile::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157db0, 0x8b bytes: DPBinFile::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPBinFile::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015e160, 0xc9 bytes: DPBinFile::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPBinFile::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPConfigData
// ---------------------------------------------------------------------

// ORIG 0x00155e60, 0x13 bytes: DPConfigData::construct()
TdrError::ErrorType DPConfigData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00158390, 0x6d bytes: DPConfigData::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPConfigData::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(proto_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(seq_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(raw_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e510, 0x85 bytes: DPConfigData::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPConfigData::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001582c0, 0x5d bytes: DPConfigData::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPConfigData::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(proto_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(seq_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(raw_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00158320, 0x66 bytes: DPConfigData::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPConfigData::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156550, 0xc7 bytes: DPConfigData::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPConfigData::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[proto_check_switch_]", "%d", proto_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_check_switch_]", "%d", seq_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[raw_check_switch_]", "%d", raw_check_switch_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e450, 0xb6 bytes: DPConfigData::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPConfigData::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157e40, 0x8b bytes: DPConfigData::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPConfigData::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015e380, 0xc9 bytes: DPConfigData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPConfigData::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPDynamicAlgorithmHead
// ---------------------------------------------------------------------

// ORIG 0x00155e80, 0x17 bytes: DPDynamicAlgorithmHead::construct()
TdrError::ErrorType DPDynamicAlgorithmHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015abd0, 0xa3 bytes: DPDynamicAlgorithmHead::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPDynamicAlgorithmHead::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e730, 0x85 bytes: DPDynamicAlgorithmHead::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPDynamicAlgorithmHead::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015fb70, 0x9a bytes: DPDynamicAlgorithmHead::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPDynamicAlgorithmHead::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015fc10, 0x66 bytes: DPDynamicAlgorithmHead::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPDynamicAlgorithmHead::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156620, 0x90 bytes: DPDynamicAlgorithmHead::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPDynamicAlgorithmHead::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[code_id_]", "%u", code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc_]", "%u", crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e670, 0xb6 bytes: DPDynamicAlgorithmHead::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPDynamicAlgorithmHead::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157ed0, 0x8b bytes: DPDynamicAlgorithmHead::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPDynamicAlgorithmHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015e5a0, 0xc9 bytes: DPDynamicAlgorithmHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPDynamicAlgorithmHead::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPFixAlgorithmHead
// ---------------------------------------------------------------------

// ORIG 0x00155ee0, 0x1e bytes: DPFixAlgorithmHead::construct()
TdrError::ErrorType DPFixAlgorithmHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015ad90, 0xea bytes: DPFixAlgorithmHead::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPFixAlgorithmHead::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(key_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(algorithm_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(crc);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015eb70, 0x85 bytes: DPFixAlgorithmHead::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPFixAlgorithmHead::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015fe00, 0xe1 bytes: DPFixAlgorithmHead::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPFixAlgorithmHead::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(key_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(algorithm_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(crc);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015fef0, 0x66 bytes: DPFixAlgorithmHead::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPFixAlgorithmHead::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001566b0, 0xc4 bytes: DPFixAlgorithmHead::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPFixAlgorithmHead::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_id_]", "%u", key_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[algorithm_type_]", "%u", algorithm_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc]", "%u", crc);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015eab0, 0xb6 bytes: DPFixAlgorithmHead::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPFixAlgorithmHead::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157f60, 0x8b bytes: DPFixAlgorithmHead::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPFixAlgorithmHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015e9e0, 0xc9 bytes: DPFixAlgorithmHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPFixAlgorithmHead::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPClientDecryptInfo
// ---------------------------------------------------------------------

// ORIG 0x00155f00, 0x17 bytes: DPClientDecryptInfo::construct()
TdrError::ErrorType DPClientDecryptInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015ae80, 0xa3 bytes: DPClientDecryptInfo::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPClientDecryptInfo::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(decrypt_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(encrypt_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015ed90, 0x85 bytes: DPClientDecryptInfo::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPClientDecryptInfo::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015ff60, 0x9a bytes: DPClientDecryptInfo::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPClientDecryptInfo::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(decrypt_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(encrypt_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00160000, 0x66 bytes: DPClientDecryptInfo::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPClientDecryptInfo::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156780, 0x90 bytes: DPClientDecryptInfo::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPClientDecryptInfo::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[decrypt_id_]", "%u", decrypt_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[encrypt_id_]", "%u", encrypt_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015ecd0, 0xb6 bytes: DPClientDecryptInfo::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPClientDecryptInfo::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157ff0, 0x8b bytes: DPClientDecryptInfo::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPClientDecryptInfo::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015ec00, 0xc9 bytes: DPClientDecryptInfo::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPClientDecryptInfo::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DpClientShortCmdData
// ---------------------------------------------------------------------

// ORIG 0x00155f60, 0x10 bytes: DpClientShortCmdData::construct()
TdrError::ErrorType DpClientShortCmdData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x001584d0, 0x71 bytes: DpClientShortCmdData::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DpClientShortCmdData::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f1d0, 0x85 bytes: DpClientShortCmdData::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DpClientShortCmdData::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015a0e0, 0x63 bytes: DpClientShortCmdData::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DpClientShortCmdData::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015a150, 0x66 bytes: DpClientShortCmdData::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DpClientShortCmdData::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156810, 0x92 bytes: DpClientShortCmdData::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DpClientShortCmdData::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_id_]", "%d", cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_count_]", "%d", cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f110, 0xb6 bytes: DpClientShortCmdData::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DpClientShortCmdData::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158110, 0x8b bytes: DpClientShortCmdData::visualize_ex(TdrWriteBuf&, int, char) const
const char* DpClientShortCmdData::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015f040, 0xc9 bytes: DpClientShortCmdData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DpClientShortCmdData::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPClientCmdData
// ---------------------------------------------------------------------

// ORIG 0x00155f70, 0x17 bytes: DPClientCmdData::construct()
TdrError::ErrorType DPClientCmdData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015af30, 0xa3 bytes: DPClientCmdData::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPClientCmdData::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f3f0, 0x85 bytes: DPClientCmdData::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPClientCmdData::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160070, 0x9a bytes: DPClientCmdData::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPClientCmdData::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00160550, 0x66 bytes: DPClientCmdData::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPClientCmdData::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001568b0, 0x90 bytes: DPClientCmdData::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPClientCmdData::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_id_]", "%u", cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_count_]", "%u", cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f330, 0xb6 bytes: DPClientCmdData::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPClientCmdData::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001581a0, 0x8b bytes: DPClientCmdData::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPClientCmdData::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015f260, 0xc9 bytes: DPClientCmdData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPClientCmdData::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPClientLogPtc
// ---------------------------------------------------------------------

// ORIG 0x00155f90, 0x35 bytes: DPClientLogPtc::construct()
TdrError::ErrorType DPClientLogPtc::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015b500, 0xf7 bytes: DPClientLogPtc::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPClientLogPtc::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(error_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (error_msg_len_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < error_msg_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeBytes(error_msg_, error_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f610, 0x85 bytes: DPClientLogPtc::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPClientLogPtc::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161210, 0xec bytes: DPClientLogPtc::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPClientLogPtc::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(error_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (error_msg_len_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < error_msg_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    ret = srcBuf.readBytes(error_msg_, error_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00161300, 0x66 bytes: DPClientLogPtc::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPClientLogPtc::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156940, 0x1c0 bytes: DPClientLogPtc::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPClientLogPtc::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_id_]", "%u", error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[is_encrypt_]", "%d", is_encrypt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_msg_len_]", "%d", error_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (error_msg_len_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[error_msg_]", error_msg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (error_msg_len_ != 0) {
        for (uint32_t i = 0; i < error_msg_len_; ++i) {
            ret = destBuf.textize(" %d", error_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f550, 0xb6 bytes: DPClientLogPtc::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPClientLogPtc::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158230, 0x8b bytes: DPClientLogPtc::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPClientLogPtc::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015f480, 0xc9 bytes: DPClientLogPtc::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPClientLogPtc::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPProtocolRequestCode
// ---------------------------------------------------------------------

// ORIG 0x00155ff0, 0x1e bytes: DPProtocolRequestCode::construct()
TdrError::ErrorType DPProtocolRequestCode::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015b3f0, 0x107 bytes: DPProtocolRequestCode::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPProtocolRequestCode::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(size);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(encrypt_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(decrypt_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bin_file_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d080, 0x85 bytes: DPProtocolRequestCode::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPProtocolRequestCode::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001615b0, 0xec bytes: DPProtocolRequestCode::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPProtocolRequestCode::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(size);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(encrypt_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(decrypt_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(bin_file_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00161ad0, 0x66 bytes: DPProtocolRequestCode::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPProtocolRequestCode::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156b00, 0x104 bytes: DPProtocolRequestCode::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPProtocolRequestCode::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[size]", "%d", size);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[encrypt_code_id_]", "%u", encrypt_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[decrypt_code_id_]", "%u", decrypt_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_file_id_]", "%d", bin_file_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015cfc0, 0xb6 bytes: DPProtocolRequestCode::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPProtocolRequestCode::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157810, 0x8b bytes: DPProtocolRequestCode::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPProtocolRequestCode::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015f8c0, 0xc9 bytes: DPProtocolRequestCode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPProtocolRequestCode::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPInterDataHead
// ---------------------------------------------------------------------

// ORIG 0x001561e0, 0x14 bytes: DPInterDataHead::construct()
TdrError::ErrorType DPInterDataHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00158550, 0x8b bytes: DPInterDataHead::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPInterDataHead::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(packet_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(protocol_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(data_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d2a0, 0x85 bytes: DPInterDataHead::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPInterDataHead::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015a1c0, 0x7d bytes: DPInterDataHead::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPInterDataHead::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(packet_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(protocol_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(data_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015a240, 0x66 bytes: DPInterDataHead::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPInterDataHead::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156c10, 0xc7 bytes: DPInterDataHead::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPInterDataHead::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[packet_size_]", "%d", packet_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[protocol_type_]", "%d", protocol_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_type_]", "%d", data_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d1e0, 0xb6 bytes: DPInterDataHead::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPInterDataHead::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157930, 0x8b bytes: DPInterDataHead::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPInterDataHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015d110, 0xc9 bytes: DPInterDataHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPInterDataHead::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TClientFrameHead
// ---------------------------------------------------------------------

// ORIG 0x00156240, 0x17 bytes: TClientFrameHead::construct()
TdrError::ErrorType TClientFrameHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a3d0, 0xc4 bytes: TClientFrameHead::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TClientFrameHead::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(packetLen);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(totalSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(protoId);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d850, 0x85 bytes: TClientFrameHead::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TClientFrameHead::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015a2b0, 0xab bytes: TClientFrameHead::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TClientFrameHead::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(packetLen);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(totalSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(protoId);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015a360, 0x66 bytes: TClientFrameHead::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TClientFrameHead::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156ce0, 0xcc bytes: TClientFrameHead::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TClientFrameHead::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[packetLen]", "%u", packetLen);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[totalSize]", "%d", totalSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[protoId]", "%d", protoId);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d790, 0xb6 bytes: TClientFrameHead::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TClientFrameHead::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001579c0, 0x8b bytes: TClientFrameHead::visualize_ex(TdrWriteBuf&, int, char) const
const char* TClientFrameHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015d6c0, 0xc9 bytes: TClientFrameHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TClientFrameHead::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TSafeMode
// ---------------------------------------------------------------------

// ORIG 0x00156310, 0x10 bytes: TSafeMode::construct()
TdrError::ErrorType TSafeMode::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a4a0, 0x61 bytes: TSafeMode::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TSafeMode::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(iSafemodeLevel);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015cb80, 0x85 bytes: TSafeMode::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TSafeMode::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160da0, 0x50 bytes: TSafeMode::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TSafeMode::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(iSafemodeLevel);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00160df0, 0x66 bytes: TSafeMode::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TSafeMode::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156db0, 0x4f bytes: TSafeMode::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TSafeMode::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[iSafemodeLevel]", "%u", iSafemodeLevel);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015cac0, 0xb6 bytes: TSafeMode::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TSafeMode::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157ae0, 0x8b bytes: TSafeMode::visualize_ex(TdrWriteBuf&, int, char) const
const char* TSafeMode::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015c9f0, 0xc9 bytes: TSafeMode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TSafeMode::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TSpecialControl
// ---------------------------------------------------------------------

// ORIG 0x00156320, 0x22 bytes: TSpecialControl::construct()
TdrError::ErrorType TSpecialControl::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a510, 0x132 bytes: TSpecialControl::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TSpecialControl::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(shPacketSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(shPacketType);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(chDataType);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(cmdid);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(bufLen);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015cda0, 0x85 bytes: TSpecialControl::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TSpecialControl::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160e60, 0x10d bytes: TSpecialControl::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TSpecialControl::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(shPacketSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(shPacketType);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(chDataType);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(cmdid);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(bufLen);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001611a0, 0x66 bytes: TSpecialControl::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TSpecialControl::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156e00, 0x138 bytes: TSpecialControl::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TSpecialControl::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[shPacketSize]", "%d", shPacketSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[shPacketType]", "%d", shPacketType);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[chDataType]", "%d", chDataType);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmdid]", "%u", cmdid);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bufLen]", "%u", bufLen);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015cce0, 0xb6 bytes: TSpecialControl::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TSpecialControl::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157b70, 0x8b bytes: TSpecialControl::visualize_ex(TdrWriteBuf&, int, char) const
const char* TSpecialControl::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015cc10, 0xc9 bytes: TSpecialControl::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TSpecialControl::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TReviveMouse
// ---------------------------------------------------------------------

// ORIG 0x00156350, 0x17 bytes: TReviveMouse::construct()
TdrError::ErrorType TReviveMouse::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a650, 0xa3 bytes: TReviveMouse::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TReviveMouse::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(cmdID);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(isHint);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015be90, 0x85 bytes: TReviveMouse::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TReviveMouse::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001605c0, 0x9a bytes: TReviveMouse::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TReviveMouse::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(cmdID);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(isHint);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00160660, 0x66 bytes: TReviveMouse::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TReviveMouse::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156f40, 0x90 bytes: TReviveMouse::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TReviveMouse::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmdID]", "%u", cmdID);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[isHint]", "%u", isHint);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015cf00, 0xb6 bytes: TReviveMouse::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TReviveMouse::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157c00, 0x8b bytes: TReviveMouse::visualize_ex(TdrWriteBuf&, int, char) const
const char* TReviveMouse::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015ce30, 0xc9 bytes: TReviveMouse::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TReviveMouse::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// ClientPkgHead
// ---------------------------------------------------------------------

// ORIG 0x001563f0, 0x1e bytes: ClientPkgHead::construct()
TdrError::ErrorType ClientPkgHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a7b0, 0x115 bytes: ClientPkgHead::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType ClientPkgHead::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(protocol_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(sln_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(sub_sln_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015be00, 0x85 bytes: ClientPkgHead::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType ClientPkgHead::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001606d0, 0xfd bytes: ClientPkgHead::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType ClientPkgHead::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(protocol_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(sln_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(sub_sln_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001607d0, 0x66 bytes: ClientPkgHead::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType ClientPkgHead::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157190, 0x139 bytes: ClientPkgHead::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType ClientPkgHead::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[protocol_id_]", "%d", protocol_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_]", "%u", seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sln_id_]", "%d", sln_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sub_sln_id_]", "%d", sub_sln_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_len_]", "%d", body_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015bff0, 0xb6 bytes: ClientPkgHead::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType ClientPkgHead::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001578a0, 0x8b bytes: ClientPkgHead::visualize_ex(TdrWriteBuf&, int, char) const
const char* ClientPkgHead::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015bf20, 0xc9 bytes: ClientPkgHead::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* ClientPkgHead::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPClientInitDataPtc
// ---------------------------------------------------------------------

// ORIG 0x00155fd0, 0x14 bytes: DPClientInitDataPtc::construct()
TdrError::ErrorType DPClientInitDataPtc::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015b600, 0x204 bytes: DPClientInitDataPtc::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPClientInitDataPtc::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (code_ver_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    if (destBuf.capacity() - destBuf.size() < sizeof(code_ver_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeBytes(code_ver_, sizeof(code_ver_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_user_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(server_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bin_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bin_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f830, 0x85 bytes: DPClientInitDataPtc::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPClientInitDataPtc::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161370, 0x1cd bytes: DPClientInitDataPtc::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPClientInitDataPtc::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (code_ver_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    if (srcBuf.capacity() - srcBuf.size() < sizeof(code_ver_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    ret = srcBuf.readBytes(code_ver_, sizeof(code_ver_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_user_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(server_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(bin_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(bin_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00161540, 0x66 bytes: DPClientInitDataPtc::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPClientInitDataPtc::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001585e0, 0x226 bytes: DPClientInitDataPtc::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPClientInitDataPtc::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[code_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (uint32_t i = 0; i < 16; ++i) {
        ret = destBuf.textize(" %d", code_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_info_]", "%u", client_info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_user_id_]", "%u", client_user_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_code_id_]", "%u", client_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[server_code_id_]", "%u", server_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_code_id_]", "%d", bin_code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_count_]", "%d", bin_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015f770, 0xb6 bytes: DPClientInitDataPtc::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPClientInitDataPtc::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158810, 0x8b bytes: DPClientInitDataPtc::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPClientInitDataPtc::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015f6a0, 0xc9 bytes: DPClientInitDataPtc::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPClientInitDataPtc::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TClientFramePiece
// ---------------------------------------------------------------------

// ORIG 0x00156260, 0x35 bytes: TClientFramePiece::construct()
TdrError::ErrorType TClientFramePiece::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015b810, 0x149 bytes: TClientFramePiece::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TClientFramePiece::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(totalPiece);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(pieceSerial);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(totalCrc);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(pieceDataSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pieceDataSize > 51200) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pieceDataSize) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeBytes(pieceData, pieceDataSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015da70, 0x85 bytes: TClientFramePiece::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TClientFramePiece::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160a40, 0x147 bytes: TClientFramePiece::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TClientFramePiece::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(totalPiece);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(pieceSerial);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(totalCrc);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(pieceDataSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pieceDataSize > 51200) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pieceDataSize) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    ret = srcBuf.readBytes(pieceData, pieceDataSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00160d30, 0x66 bytes: TClientFramePiece::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TClientFramePiece::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001588a0, 0x208 bytes: TClientFramePiece::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TClientFramePiece::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[totalPiece]", "%d", totalPiece);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pieceSerial]", "%d", pieceSerial);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[totalCrc]", "%u", totalCrc);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pieceDataSize]", "%d", pieceDataSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pieceDataSize > 51200) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pieceData]", pieceDataSize);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pieceDataSize != 0) {
        for (uint32_t i = 0; i < pieceDataSize; ++i) {
            ret = destBuf.textize(" %d", pieceData[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d9b0, 0xb6 bytes: TClientFramePiece::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TClientFramePiece::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158cd0, 0x8b bytes: TClientFramePiece::visualize_ex(TdrWriteBuf&, int, char) const
const char* TClientFramePiece::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015d8e0, 0xc9 bytes: TClientFramePiece::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TClientFramePiece::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPCmdList
// ---------------------------------------------------------------------

// ORIG 0x00155dc0, 0x35 bytes: DPCmdList::construct()
TdrError::ErrorType DPCmdList::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a990, 0x1c8 bytes: DPCmdList::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPCmdList::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(up_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(down_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (up_cmd_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < up_cmd_count_; ++i) {
        ret = destBuf.writeUInt32(up_cmd_list_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (down_cmd_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < down_cmd_count_; ++i) {
        ret = destBuf.writeUInt32(down_cmd_list_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

// ORIG 0x0015c3b0, 0x85 bytes: DPCmdList::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPCmdList::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160840, 0x18c bytes: DPCmdList::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPCmdList::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(up_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(down_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (up_cmd_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < up_cmd_count_; ++i) {
        ret = srcBuf.readUInt32(up_cmd_list_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (down_cmd_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < down_cmd_count_; ++i) {
        ret = srcBuf.readUInt32(down_cmd_list_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

// ORIG 0x001609d0, 0x66 bytes: DPCmdList::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPCmdList::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158d60, 0x262 bytes: DPCmdList::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPCmdList::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[up_cmd_count_]", "%d", up_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[down_cmd_count_]", "%d", down_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (up_cmd_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[up_cmd_list_]", up_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (up_cmd_count_ != 0) {
        for (uint32_t i = 0; i < up_cmd_count_; ++i) {
            ret = destBuf.textize(" %u", up_cmd_list_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (down_cmd_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[down_cmd_list_]", down_cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (down_cmd_count_ != 0) {
        for (uint32_t i = 0; i < down_cmd_count_; ++i) {
            ret = destBuf.textize(" %u", down_cmd_list_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015c2f0, 0xb6 bytes: DPCmdList::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPCmdList::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158fd0, 0x8b bytes: DPCmdList::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPCmdList::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015c220, 0xc9 bytes: DPCmdList::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPCmdList::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPServerCode
// ---------------------------------------------------------------------

// ORIG 0x00155e00, 0x2c bytes: DPServerCode::construct()
TdrError::ErrorType DPServerCode::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015ba10, 0xae bytes: DPServerCode::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPServerCode::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (client_version_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    if (destBuf.capacity() - destBuf.size() < sizeof(client_version_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeBytes(client_version_, sizeof(client_version_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015c5d0, 0x85 bytes: DPServerCode::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPServerCode::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015f990, 0xa7 bytes: DPServerCode::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPServerCode::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (client_version_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    if (srcBuf.capacity() - srcBuf.size() < sizeof(client_version_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    ret = srcBuf.readBytes(client_version_, sizeof(client_version_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015fa40, 0x66 bytes: DPServerCode::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPServerCode::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159060, 0xf5 bytes: DPServerCode::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPServerCode::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[client_version_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (uint32_t i = 0; i < 16; ++i) {
        ret = destBuf.textize(" %d", client_version_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[code_id_]", "%u", code_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015c510, 0xb6 bytes: DPServerCode::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPServerCode::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159c70, 0x8b bytes: DPServerCode::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPServerCode::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015c440, 0xc9 bytes: DPServerCode::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPServerCode::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPClientStatPtc
// ---------------------------------------------------------------------

// ORIG 0x00155f20, 0x35 bytes: DPClientStatPtc::construct()
TdrError::ErrorType DPClientStatPtc::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015afe0, 0x410 bytes: DPClientStatPtc::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPClientStatPtc::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(internal_msg_send_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(internal_msg_recv_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_pkg_recv_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(fix_encrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_fix_decrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_dynamic_decrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_less_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_greater_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_discard_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(down_cmd_list_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(need_encrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (need_encrypt_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < need_encrypt_count_; ++i) {
        ret = need_encrypt_list_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt16(client_error_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_error_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < client_error_count_; ++i) {
        ret = client_error_list_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

// ORIG 0x0015efb0, 0x85 bytes: DPClientStatPtc::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPClientStatPtc::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160110, 0x3d0 bytes: DPClientStatPtc::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPClientStatPtc::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(internal_msg_send_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(internal_msg_recv_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(game_pkg_recv_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(fix_encrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(game_fix_decrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(game_dynamic_decrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_less_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_greater_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_discard_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(down_cmd_list_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(need_encrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (need_encrypt_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < need_encrypt_count_; ++i) {
        ret = need_encrypt_list_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = srcBuf.readUInt16(client_error_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_error_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < client_error_count_; ++i) {
        ret = client_error_list_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

// ORIG 0x001604e0, 0x66 bytes: DPClientStatPtc::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPClientStatPtc::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001573d0, 0x436 bytes: DPClientStatPtc::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPClientStatPtc::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[internal_msg_send_count_]", "%u", internal_msg_send_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[internal_msg_recv_count_]", "%u", internal_msg_recv_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_pkg_recv_count_]", "%u", game_pkg_recv_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[fix_encrypt_count_]", "%u", fix_encrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_fix_decrypt_count_]", "%u", game_fix_decrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_dynamic_decrypt_count_]", "%u", game_dynamic_decrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_less_count_]", "%u", seq_less_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_greater_count_]", "%u", seq_greater_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_discard_count_]", "%u", seq_discard_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[down_cmd_list_crc_]", "%u", down_cmd_list_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[need_encrypt_count_]", "%d", need_encrypt_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (need_encrypt_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < need_encrypt_count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[need_encrypt_list_]", (unsigned int)i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = need_encrypt_list_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = need_encrypt_list_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_error_count_]", "%d", client_error_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_error_count_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint16_t i = 0; i < client_error_count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_error_list_]", (unsigned int)i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = client_error_list_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = client_error_list_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

// ORIG 0x0015eef0, 0xb6 bytes: DPClientStatPtc::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPClientStatPtc::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158080, 0x8b bytes: DPClientStatPtc::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPClientStatPtc::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015ee20, 0xc9 bytes: DPClientStatPtc::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPClientStatPtc::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPAlgorithmData
// ---------------------------------------------------------------------

// ORIG 0x00155ea0, 0x35 bytes: DPAlgorithmData::construct()
TdrError::ErrorType DPAlgorithmData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015ac80, 0x10e bytes: DPAlgorithmData::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPAlgorithmData::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(usr_id_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(custom_data_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeBytes(pad_data_, pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e950, 0x85 bytes: DPAlgorithmData::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPAlgorithmData::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x0015fc80, 0x10e bytes: DPAlgorithmData::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPAlgorithmData::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(usr_id_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(custom_data_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    ret = srcBuf.readBytes(pad_data_, pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015fd90, 0x66 bytes: DPAlgorithmData::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPAlgorithmData::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159d00, 0x1bb bytes: DPAlgorithmData::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPAlgorithmData::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[usr_id_crc_]", "%u", usr_id_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[custom_data_]", "%u", custom_data_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "%d", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" %d", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015e890, 0xb6 bytes: DPAlgorithmData::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPAlgorithmData::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159ec0, 0x8b bytes: DPAlgorithmData::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPAlgorithmData::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015e7c0, 0xc9 bytes: DPAlgorithmData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPAlgorithmData::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// DPInterDataBody (anonymous union)
// ---------------------------------------------------------------------

// ORIG 0x00156010, 0x1c5 bytes: DPInterDataBody::construct(int64_t)
TdrError::ErrorType DPInterDataBody::construct(int64_t unionSelector) {
    if (unionSelector == 1) {
        return client_code_.construct();
    } else if (unionSelector == 2) {
        return dynamic_algorithm_data_.construct();
    } else if (unionSelector == 3) {
        return bin_file_.construct();
    } else if (unionSelector == 4) {
        return cmd_list.construct();
    } else if (unionSelector == 5) {
        return bin_count_.construct();
    } else if (unionSelector == 6) {
        return server_code_.construct();
    } else if (unionSelector == 9) {
        return request_code_.construct();
    } else if (unionSelector == 10) {
        return init_data_ptc_.construct();
    } else if (unionSelector == 11) {
        return client_stat_ptc_.construct();
    } else if (unionSelector == 12) {
        return decrypt_info_.construct();
    } else if (unionSelector == 13) {
        return client_log_ptc_.construct();
    } else if (unionSelector == 14) {
        memset(cs_test_data_, 0, sizeof(cs_test_data_));
        return TdrError::TDR_NO_ERROR;
    } else if (unionSelector == 15) {
        memset(sc_test_data_, 0, sizeof(sc_test_data_));
        return TdrError::TDR_NO_ERROR;
    } else if (unionSelector == 17) {
        return fix_algorithm_data_.construct();
    } else if (unionSelector == 20) {
        return config_data_.construct();
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015bac0, 0x2c5 bytes: DPInterDataBody::pack(int64_t, TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPInterDataBody::pack(int64_t unionSelector, TdrWriteBuf& destBuf,
                                unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (unionSelector == 1) {
        return client_code_.pack(destBuf, 1);
    } else if (unionSelector == 2) {
        return dynamic_algorithm_data_.pack(destBuf, 1);
    } else if (unionSelector == 3) {
        return bin_file_.pack(destBuf, 1);
    } else if (unionSelector == 4) {
        return cmd_list.pack(destBuf, 1);
    } else if (unionSelector == 5) {
        return bin_count_.pack(destBuf, 1);
    } else if (unionSelector == 6) {
        return server_code_.pack(destBuf, 1);
    } else if (unionSelector == 9) {
        return request_code_.pack(destBuf, 1);
    } else if (unionSelector == 10) {
        return init_data_ptc_.pack(destBuf, 1);
    } else if (unionSelector == 11) {
        return client_stat_ptc_.pack(destBuf, 1);
    } else if (unionSelector == 12) {
        return decrypt_info_.pack(destBuf, 1);
    } else if (unionSelector == 13) {
        return client_log_ptc_.pack(destBuf, 1);
    } else if (unionSelector == 14) {
        if (cs_test_data_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        if (destBuf.capacity() - destBuf.size() < 10) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        ret = destBuf.writeBytes(cs_test_data_, 10);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        return ret;
    } else if (unionSelector == 15) {
        if (sc_test_data_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        if (destBuf.capacity() - destBuf.size() < 10) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        ret = destBuf.writeBytes(sc_test_data_, 10);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        return ret;
    } else if (unionSelector == 17) {
        return fix_algorithm_data_.pack(destBuf, 1);
    } else if (unionSelector == 20) {
        return config_data_.pack(destBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015c180, 0x93 bytes: DPInterDataBody::pack(int64_t, char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPInterDataBody::pack(int64_t unionSelector, char* buffer, unsigned int size,
                                unsigned int* usedSize,
                                unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(unionSelector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001616a0, 0x2c5 bytes: DPInterDataBody::unpack(int64_t, TdrReadBuf&, unsigned int)
TdrError::ErrorType DPInterDataBody::unpack(int64_t unionSelector, TdrReadBuf& srcBuf,
                                  unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (unionSelector == 1) {
        return client_code_.unpack(srcBuf, 1);
    } else if (unionSelector == 2) {
        return dynamic_algorithm_data_.unpack(srcBuf, 1);
    } else if (unionSelector == 3) {
        return bin_file_.unpack(srcBuf, 1);
    } else if (unionSelector == 4) {
        return cmd_list.unpack(srcBuf, 1);
    } else if (unionSelector == 5) {
        return bin_count_.unpack(srcBuf, 1);
    } else if (unionSelector == 6) {
        return server_code_.unpack(srcBuf, 1);
    } else if (unionSelector == 9) {
        return request_code_.unpack(srcBuf, 1);
    } else if (unionSelector == 10) {
        return init_data_ptc_.unpack(srcBuf, 1);
    } else if (unionSelector == 11) {
        return client_stat_ptc_.unpack(srcBuf, 1);
    } else if (unionSelector == 12) {
        return decrypt_info_.unpack(srcBuf, 1);
    } else if (unionSelector == 13) {
        return client_log_ptc_.unpack(srcBuf, 1);
    } else if (unionSelector == 14) {
        if (cs_test_data_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        if (srcBuf.capacity() - srcBuf.size() < 10) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        ret = srcBuf.readBytes(cs_test_data_, 10);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        return ret;
    } else if (unionSelector == 15) {
        if (sc_test_data_ == NULL) return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        if (srcBuf.capacity() - srcBuf.size() < 10) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        ret = srcBuf.readBytes(sc_test_data_, 10);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        return ret;
    } else if (unionSelector == 17) {
        return fix_algorithm_data_.unpack(srcBuf, 1);
    } else if (unionSelector == 20) {
        return config_data_.unpack(srcBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00161a50, 0x7a bytes: DPInterDataBody::unpack(int64_t, const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPInterDataBody::unpack(int64_t unionSelector, const char* buffer,
                                  unsigned int size, unsigned int* usedSize,
                                  unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(unionSelector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159160, 0x97f bytes: DPInterDataBody::visualize(int64_t, TdrWriteBuf&, int, char) const
TdrError::ErrorType DPInterDataBody::visualize(int64_t unionSelector, TdrWriteBuf& destBuf,
                                     int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (unionSelector == 1) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_code_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return client_code_.visualize(destBuf, indent + 1, separator);
        }
        return client_code_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 2) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dynamic_algorithm_data_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return dynamic_algorithm_data_.visualize(destBuf, indent + 1, separator);
        }
        return dynamic_algorithm_data_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 3) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_file_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return bin_file_.visualize(destBuf, indent + 1, separator);
        }
        return bin_file_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 4) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_list]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return cmd_list.visualize(destBuf, indent + 1, separator);
        }
        return cmd_list.visualize(destBuf, indent, separator);
    } else if (unionSelector == 5) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bin_count_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return bin_count_.visualize(destBuf, indent + 1, separator);
        }
        return bin_count_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 6) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[server_code_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return server_code_.visualize(destBuf, indent + 1, separator);
        }
        return server_code_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 9) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[request_code_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return request_code_.visualize(destBuf, indent + 1, separator);
        }
        return request_code_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 10) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[init_data_ptc_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return init_data_ptc_.visualize(destBuf, indent + 1, separator);
        }
        return init_data_ptc_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 11) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_stat_ptc_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return client_stat_ptc_.visualize(destBuf, indent + 1, separator);
        }
        return client_stat_ptc_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 12) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[decrypt_info_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return decrypt_info_.visualize(destBuf, indent + 1, separator);
        }
        return decrypt_info_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 13) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_log_ptc_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return client_log_ptc_.visualize(destBuf, indent + 1, separator);
        }
        return client_log_ptc_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 14) {
        ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cs_test_data_]", 10);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        for (uint32_t i = 0; i < 10; ++i) {
            ret = destBuf.textize(" %d", cs_test_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
        return destBuf.writeCharWithNull(separator);
    } else if (unionSelector == 15) {
        ret = TdrBufUtil::printArray(destBuf, indent, separator, "[sc_test_data_]", 10);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        for (uint32_t i = 0; i < 10; ++i) {
            ret = destBuf.textize(" %d", sc_test_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
        return destBuf.writeCharWithNull(separator);
    } else if (unionSelector == 17) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[fix_algorithm_data_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return fix_algorithm_data_.visualize(destBuf, indent + 1, separator);
        }
        return fix_algorithm_data_.visualize(destBuf, indent, separator);
    } else if (unionSelector == 20) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[config_data_]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return config_data_.visualize(destBuf, indent + 1, separator);
        }
        return config_data_.visualize(destBuf, indent, separator);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015c0b0, 0xc6 bytes: DPInterDataBody::visualize(int64_t, char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPInterDataBody::visualize(int64_t unionSelector, char* buffer,
                                     unsigned int size, unsigned int* usedSize,
                                     int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(unionSelector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ---------------------------------------------------------------------
// TClientFrameBody (anonymous union)
// ---------------------------------------------------------------------

// ORIG 0x001562a0, 0x23 bytes: TClientFrameBody::construct(int64_t)
TdrError::ErrorType TClientFrameBody::construct(int64_t unionSelector) {
    return framePiece.construct();
}

// ORIG 0x0015b960, 0x32 bytes: TClientFrameBody::pack(int64_t, TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TClientFrameBody::pack(int64_t unionSelector, TdrWriteBuf& destBuf,
                                unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = framePiece.pack(destBuf, 1);
    return ret;
}

// ORIG 0x0015d620, 0x93 bytes: TClientFrameBody::pack(int64_t, char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TClientFrameBody::pack(int64_t unionSelector, char* buffer, unsigned int size,
                                unsigned int* usedSize,
                                unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(unionSelector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160b90, 0x32 bytes: TClientFrameBody::unpack(int64_t, TdrReadBuf&, unsigned int)
TdrError::ErrorType TClientFrameBody::unpack(int64_t unionSelector, TdrReadBuf& srcBuf,
                                  unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = framePiece.unpack(srcBuf, 1);
    return ret;
}

// ORIG 0x00160cb0, 0x7a bytes: TClientFrameBody::unpack(int64_t, const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TClientFrameBody::unpack(int64_t unionSelector, const char* buffer,
                                  unsigned int size, unsigned int* usedSize,
                                  unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(unionSelector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158ab0, 0x82 bytes: TClientFrameBody::visualize(int64_t, TdrWriteBuf&, int, char) const
TdrError::ErrorType TClientFrameBody::visualize(int64_t unionSelector, TdrWriteBuf& destBuf,
                                     int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[framePiece]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        return framePiece.visualize(destBuf, indent + 1, separator);
    }
    return framePiece.visualize(destBuf, indent, separator);
}

// ORIG 0x0015d550, 0xc6 bytes: TClientFrameBody::visualize(int64_t, char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TClientFrameBody::visualize(int64_t unionSelector, char* buffer,
                                     unsigned int size, unsigned int* usedSize,
                                     int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(unionSelector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ---------------------------------------------------------------------
// TClientPkgBody (anonymous union)
// ---------------------------------------------------------------------

// ORIG 0x00156370, 0x7c bytes: TClientPkgBody::construct(int64_t)
TdrError::ErrorType TClientPkgBody::construct(int64_t unionSelector) {
    if (unionSelector == 309) {
        return reviseMouse.construct();
    } else if (unionSelector == 310) {
        return specialControl.construct();
    } else if (unionSelector == 311) {
        return safeMode.construct();
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a700, 0xab bytes: TClientPkgBody::pack(int64_t, TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TClientPkgBody::pack(int64_t unionSelector, TdrWriteBuf& destBuf,
                                unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (unionSelector == 309) {
        return reviseMouse.pack(destBuf, 1);
    } else if (unionSelector == 310) {
        return specialControl.pack(destBuf, 1);
    } else if (unionSelector == 311) {
        return safeMode.pack(destBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015c950, 0x93 bytes: TClientPkgBody::pack(int64_t, char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TClientPkgBody::pack(int64_t unionSelector, char* buffer, unsigned int size,
                                unsigned int* usedSize,
                                unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(unionSelector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160f70, 0xab bytes: TClientPkgBody::unpack(int64_t, TdrReadBuf&, unsigned int)
TdrError::ErrorType TClientPkgBody::unpack(int64_t unionSelector, TdrReadBuf& srcBuf,
                                  unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (unionSelector == 309) {
        return reviseMouse.unpack(srcBuf, 1);
    } else if (unionSelector == 310) {
        return specialControl.unpack(srcBuf, 1);
    } else if (unionSelector == 311) {
        return safeMode.unpack(srcBuf, 1);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00161120, 0x7a bytes: TClientPkgBody::unpack(int64_t, const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TClientPkgBody::unpack(int64_t unionSelector, const char* buffer,
                                  unsigned int size, unsigned int* usedSize,
                                  unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(unionSelector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00156fd0, 0x1b6 bytes: TClientPkgBody::visualize(int64_t, TdrWriteBuf&, int, char) const
TdrError::ErrorType TClientPkgBody::visualize(int64_t unionSelector, TdrWriteBuf& destBuf,
                                     int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (unionSelector == 309) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[reviseMouse]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return reviseMouse.visualize(destBuf, indent + 1, separator);
        }
        return reviseMouse.visualize(destBuf, indent, separator);
    } else if (unionSelector == 310) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[specialControl]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return specialControl.visualize(destBuf, indent + 1, separator);
        }
        return specialControl.visualize(destBuf, indent, separator);
    } else if (unionSelector == 311) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[safeMode]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            return safeMode.visualize(destBuf, indent + 1, separator);
        }
        return safeMode.visualize(destBuf, indent, separator);
    }
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015dd20, 0xc6 bytes: TClientPkgBody::visualize(int64_t, char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TClientPkgBody::visualize(int64_t unionSelector, char* buffer,
                                     unsigned int size, unsigned int* usedSize,
                                     int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(unionSelector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ---------------------------------------------------------------------
// DPInterData
// ---------------------------------------------------------------------

// ORIG 0x00155d80, 0x35 bytes: DPInterData::construct()
TdrError::ErrorType DPInterData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015bd90, 0x6c bytes: DPInterData::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType DPInterData::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body.pack(head.protocol_type_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015c8c0, 0x85 bytes: DPInterData::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType DPInterData::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161970, 0x6c bytes: DPInterData::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType DPInterData::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body.unpack(head.protocol_type_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001619e0, 0x66 bytes: DPInterData::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType DPInterData::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159ae0, 0xf7 bytes: DPInterData::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType DPInterData::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = head.visualize(destBuf, indent + 1, separator);
    } else {
        ret = head.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = body.visualize(head.protocol_type_, destBuf, indent + 1, separator);
    } else {
        ret = body.visualize(head.protocol_type_, destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015c800, 0xb6 bytes: DPInterData::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType DPInterData::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00159be0, 0x8b bytes: DPInterData::visualize_ex(TdrWriteBuf&, int, char) const
const char* DPInterData::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015c730, 0xc9 bytes: DPInterData::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* DPInterData::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TClientFrame
// ---------------------------------------------------------------------

// ORIG 0x00156200, 0x35 bytes: TClientFrame::construct()
TdrError::ErrorType TClientFrame::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015b9a0, 0x6c bytes: TClientFrame::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TClientFrame::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body.pack(head.protoId, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d4c0, 0x85 bytes: TClientFrame::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TClientFrame::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00160bd0, 0x6c bytes: TClientFrame::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TClientFrame::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body.unpack(head.protoId, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00160c40, 0x66 bytes: TClientFrame::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TClientFrame::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158b40, 0xf7 bytes: TClientFrame::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TClientFrame::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = head.visualize(destBuf, indent + 1, separator);
    } else {
        ret = head.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = body.visualize(head.protoId, destBuf, indent + 1, separator);
    } else {
        ret = body.visualize(head.protoId, destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015d400, 0xb6 bytes: TClientFrame::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TClientFrame::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00158c40, 0x8b bytes: TClientFrame::visualize_ex(TdrWriteBuf&, int, char) const
const char* TClientFrame::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015d330, 0xc9 bytes: TClientFrame::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TClientFrame::visualize_ex(char* buffer, unsigned int size,
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

// ---------------------------------------------------------------------
// TClientPkg
// ---------------------------------------------------------------------

// ORIG 0x001562d0, 0x37 bytes: TClientPkg::construct()
TdrError::ErrorType TClientPkg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x0015a8d0, 0xbe bytes: TClientPkg::pack(TdrWriteBuf&, unsigned int) const
TdrError::ErrorType TClientPkg::pack(TdrWriteBuf& destBuf,
                            unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    size_t startPos = destBuf.getUsedSize();
    ret = head.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    // The stamped value is the *body* length: ORIG 0x15a920 saves
    // pos_ right after the head pack (-0x10(%ebp)) and 0x15a96f stores
    // pos_after_body - pos_after_head at startPos + 10; the position itself
    // is the pre-head startPos.  (The differential oracle caught this:
    // stamping used-startPos wrote 20 instead of 8 for a 8-byte body.)
    size_t headEnd = destBuf.getUsedSize();
    ret = body.pack(head.sub_sln_id_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16((uint16_t)(destBuf.getUsedSize() - headEnd),
                            startPos + 10);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015dc90, 0x85 bytes: TClientPkg::pack(char*, unsigned int, unsigned int*, unsigned int) const
TdrError::ErrorType TClientPkg::pack(char* buffer, unsigned int size,
                            unsigned int* usedSize,
                            unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161020, 0x8f bytes: TClientPkg::unpack(TdrReadBuf&, unsigned int)
TdrError::ErrorType TClientPkg::unpack(TdrReadBuf& srcBuf,
                              unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (head.body_len_ > srcBuf.capacity() - srcBuf.size())
        return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    ret = body.unpack(head.sub_sln_id_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001610b0, 0x66 bytes: TClientPkg::unpack(const char*, unsigned int, unsigned int*, unsigned int)
TdrError::ErrorType TClientPkg::unpack(const char* buffer, unsigned int size,
                              unsigned int* usedSize,
                              unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001572d0, 0xf7 bytes: TClientPkg::visualize(TdrWriteBuf&, int, char) const
TdrError::ErrorType TClientPkg::visualize(TdrWriteBuf& destBuf, int indent,
                                 char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = head.visualize(destBuf, indent + 1, separator);
    } else {
        ret = head.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = body.visualize(head.sub_sln_id_, destBuf, indent + 1, separator);
    } else {
        ret = body.visualize(head.sub_sln_id_, destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x0015dbd0, 0xb6 bytes: TClientPkg::visualize(char*, unsigned int, unsigned int*, int, char) const
TdrError::ErrorType TClientPkg::visualize(char* buffer, unsigned int size,
                                 unsigned int* usedSize, int indent,
                                 char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00157a50, 0x8b bytes: TClientPkg::visualize_ex(TdrWriteBuf&, int, char) const
const char* TClientPkg::visualize_ex(TdrWriteBuf& destBuf, int indent,
                              char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x0015db00, 0xc9 bytes: TClientPkg::visualize_ex(char*, unsigned int, unsigned int*, int, char) const
const char* TClientPkg::visualize_ex(char* buffer, unsigned int size,
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
