// Reconstructed from gunnersvr disassembly
// Original source: src/protocol/common/comm_proto_public_cfgsvr.cpp

#include "src/protocol/common/comm_proto_public_cfgsvr.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <string.h>
#include <stdint.h>

namespace {

// 读取带长度前缀的字符串（长度字段含结尾 NUL），单个元素最大 maxLen 字节。
tsf4g_tdr::TdrError::ErrorType readBoundedString(tsf4g_tdr::TdrReadBuf& destBuf, char* dest, size_t maxLen) {
    uint32_t size;
    tsf4g_tdr::TdrError::ErrorType ret = destBuf.readUInt32(size);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < size) return tsf4g_tdr::TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    if (size > maxLen) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
    if (size == 0) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_SMALL;
    if (dest == NULL) return tsf4g_tdr::TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    for (uint32_t i = 0; i < size; ++i) {
        ret = destBuf.readChar(dest[i]);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    if (dest[size - 1] != '\0') return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    if (strlen(dest) + 1 != size) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

} // anonymous namespace

namespace sec_proto {

using namespace tsf4g_tdr;

// ConfigBaseInfo

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const size_t sizePos4cfg_name_ = destBuf.size();
    ((char*)cfg_name_)[sizeof(cfg_name_) / sizeof(cfg_name_[0]) - 1] = '\0';
    size_t realSize4cfg_name_ = strlen(cfg_name_) + 1;
    ret = destBuf.writeString(cfg_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    uint32_t size4cfg_name_;
    size_t realSize4cfg_name_;
    ret = readBoundedString(destBuf, cfg_name_, sizeof(cfg_name_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[cfg_name_]", cfg_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigBaseInfo::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ConfigBaseInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* ConfigBaseInfo::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// ConfigMD5Info

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const size_t sizePos4cfg_name_ = destBuf.size();
    ((char*)cfg_name_)[sizeof(cfg_name_) / sizeof(cfg_name_[0]) - 1] = '\0';
    size_t realSize4cfg_name_ = strlen(cfg_name_) + 1;
    ret = destBuf.writeString(cfg_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < sizeof(cfg_md5_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (size_t i = 0; i < sizeof(cfg_md5_); ++i) {
        ret = destBuf.writeChar((char)cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t size4cfg_name_;
    size_t realSize4cfg_name_;
    ret = readBoundedString(destBuf, cfg_name_, sizeof(cfg_name_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < sizeof(cfg_md5_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (size_t i = 0; i < sizeof(cfg_md5_); ++i) {
        ret = destBuf.readUInt8(cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[cfg_name_]", cfg_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cfg_md5_]", (int64_t)(sizeof(cfg_md5_)));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (size_t i = 0; i < (size_t)(sizeof(cfg_md5_)); ++i) {
        ret = destBuf.textize(" 0x%02x", (int)cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() <= 1) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ((char*)destBuf.data())[destBuf.size()] = '\0';
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigMD5Info::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ConfigMD5Info::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* ConfigMD5Info::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvcIpInfoReq

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvcIpInfoReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvcIpInfoReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvcIpInfoRsp

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    result_ = (int32_t)tmp;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt16(bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bind_ip_]", "%u", bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bind_port_]", "%d", bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[idc_no_]", "%u", idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIpInfoRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvcIpInfoRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvcIpInfoRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvcIdReq

tsf4g_tdr::TdrError::ErrorType SvcIdReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(svc_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIdReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt16(svc_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIdReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_index_]", "%d", svc_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvcIdReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvcIdReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvcIdRsp

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    result_ = (int32_t)tmp;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_id_]", "%u", game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[idc_no_]", "%u", idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%u", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvcIdRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvcIdRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvcIdRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// CfgsvrInfoRsp

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    result_ = (int32_t)tmp;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt16(bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bind_ip_]", "%u", bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bind_port_]", "%d", bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgsvrInfoRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* CfgsvrInfoRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* CfgsvrInfoRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// DeployInfo

tsf4g_tdr::TdrError::ErrorType DeployInfo::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt16(world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_id_]", "%u", game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[idc_no_]", "%u", idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_]", "%d", world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DeployInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* DeployInfo::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// DeployInfoReq

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DeployInfoReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* DeployInfoReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvrHeartbeatReq

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrHeartbeatReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvrHeartbeatReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvrHeartbeatReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// StartSynCfgFileReq

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)expect_cfg_info_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    expect_cfg_info_index_ = (int32_t)tmp;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[expect_cfg_info_index_]", "%d", expect_cfg_info_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* StartSynCfgFileReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* StartSynCfgFileReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// StartSynSvrInfoRsp

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(total_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(total_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[total_svr_num_]", "%u", total_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynSvrInfoRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* StartSynSvrInfoRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* StartSynSvrInfoRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvrInfo

tsf4g_tdr::TdrError::ErrorType SvrInfo::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrInfo::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(svc_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(internal_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrInfo::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvrInfo::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt16(svc_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(internal_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt16(bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrInfo::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvrInfo::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_index_]", "%d", svc_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[internal_ip_]", "%u", internal_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bind_ip_]", "%u", bind_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bind_port_]", "%d", bind_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[idc_no_]", "%u", idc_no_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_]", "%u", world_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_id_]", "%u", game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrInfo::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvrInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvrInfo::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SynSvrInfoReq

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(curr_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(curr_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[curr_svr_num_]", "%u", curr_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SynSvrInfoReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SynSvrInfoReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// ConfigStorageInfo

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = cfg_base_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(storage_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = cfg_base_info_.unpack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(storage_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_base_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cfg_base_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cfg_base_info_.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[storage_index_]", "%u", storage_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigStorageInfo::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ConfigStorageInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* ConfigStorageInfo::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SvrUpdateInfo

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(update_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = svr_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(update_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = svr_info_.unpack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[update_type_]", "%d", update_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svr_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = svr_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = svr_info_.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SvrUpdateInfo::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SvrUpdateInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SvrUpdateInfo::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// ConfigInfo

tsf4g_tdr::TdrError::ErrorType ConfigInfo::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigInfo::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = cfg_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(cfg_info_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigInfo::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigInfo::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = cfg_info_.unpack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(cfg_info_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigInfo::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ConfigInfo::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cfg_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cfg_info_.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_info_index_]", "%u", cfg_info_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ConfigInfo::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ConfigInfo::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* ConfigInfo::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// CfgFileReq

tsf4g_tdr::TdrError::ErrorType CfgFileReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = cfg_base_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < sizeof(cfg_md5_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (size_t i = 0; i < sizeof(cfg_md5_); ++i) {
        ret = destBuf.writeChar((char)cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(recv_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CfgFileReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = cfg_base_info_.unpack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < sizeof(cfg_md5_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (size_t i = 0; i < sizeof(cfg_md5_); ++i) {
        ret = destBuf.readUInt8(cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.readUInt32(recv_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CfgFileReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_base_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cfg_base_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cfg_base_info_.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cfg_md5_]", (int64_t)(sizeof(cfg_md5_)));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (size_t i = 0; i < (size_t)(sizeof(cfg_md5_)); ++i) {
        ret = destBuf.textize(" 0x%02x", (int)cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() <= 1) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ((char*)destBuf.data())[destBuf.size()] = '\0';
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[recv_len_]", "%u", recv_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* CfgFileReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* CfgFileReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// CfgFileRsp

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = cfg_base_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(cfg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < sizeof(cfg_md5_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (size_t i = 0; i < sizeof(cfg_md5_); ++i) {
        ret = destBuf.writeChar((char)cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 64000) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (size_t i = 0; i < data_len_; ++i) {
        ret = destBuf.writeChar(data_buf_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    result_ = (int32_t)tmp;
    ret = cfg_base_info_.unpack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(cfg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < sizeof(cfg_md5_)) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (size_t i = 0; i < sizeof(cfg_md5_); ++i) {
        ret = destBuf.readUInt8(cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.readUInt32(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 64000) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (size_t i = 0; i < data_len_; ++i) {
        ret = destBuf.readChar(data_buf_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_base_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cfg_base_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cfg_base_info_.visualize(destBuf, indent, separator);
    }
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_len_]", "%u", cfg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cfg_md5_]", (int64_t)(sizeof(cfg_md5_)));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (size_t i = 0; i < (size_t)(sizeof(cfg_md5_)); ++i) {
        ret = destBuf.textize(" 0x%02x", (int)cfg_md5_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() <= 1) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ((char*)destBuf.data())[destBuf.size()] = '\0';
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%u", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_buf_]", (int64_t)(data_len_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (size_t i = 0; i < (size_t)(data_len_); ++i) {
        ret = destBuf.textize(" 0x%02x", (int)data_buf_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() <= 1) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ((char*)destBuf.data())[destBuf.size()] = '\0';
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CfgFileRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* CfgFileRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* CfgFileRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// CheckCfgFileUpdateReq

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = cfg_md5_info_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = cfg_md5_info_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_num_]", "%u", cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_md5_info_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = cfg_md5_info_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = cfg_md5_info_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* CheckCfgFileUpdateReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* CheckCfgFileUpdateReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// CheckCfgFileUpdateRsp

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < cfg_num_; ++i) {
        ((char*)cfg_name_)[i * 128 + 127] = '\0';
        ret = destBuf.writeString(cfg_name_ + i * 128);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < cfg_num_; ++i) {
        ret = readBoundedString(destBuf, cfg_name_ + i * 128, 128);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_num_]", "%u", cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < cfg_num_; ++i) {
        ret = TdrBufUtil::printString(destBuf, indent, separator, "[cfg_name_]", i, cfg_name_ + i * 128);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType CheckCfgFileUpdateRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* CheckCfgFileUpdateRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* CheckCfgFileUpdateRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// DeleteConfigReq

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)del_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (del_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)del_num_; ++i) {
        ret = cfg_info_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(del_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (del_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)del_num_; ++i) {
        ret = cfg_info_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[del_num_]", "%u", del_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (del_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)del_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_info_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = cfg_info_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = cfg_info_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeleteConfigReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DeleteConfigReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* DeleteConfigReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// DeployInfoRsp

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)deploy_info_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (deploy_info_num_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)deploy_info_num_; ++i) {
        ret = deploy_info_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    result_ = (int32_t)tmp;
    ret = destBuf.readUInt16(svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(deploy_info_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (deploy_info_num_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)deploy_info_num_; ++i) {
        ret = deploy_info_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_type_]", "%d", svc_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svc_id_]", "%u", svc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[deploy_info_num_]", "%u", deploy_info_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (deploy_info_num_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)deploy_info_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[deploy_info_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = deploy_info_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = deploy_info_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfoRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DeployInfoRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* DeployInfoRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// NotifySynCfgFileReq

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = cfg_info_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32((uint32_t)seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = cfg_info_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    seq_ = (int32_t)tmp;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_num_]", "%u", cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_info_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = cfg_info_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = cfg_info_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_]", "%d", seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* NotifySynCfgFileReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* NotifySynCfgFileReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// NotifySynCfgFileRsp

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    result_ = (int32_t)tmp;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType NotifySynCfgFileRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* NotifySynCfgFileRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* NotifySynCfgFileRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// StartSynCfgFileRsp

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (svr_num_ < 0) return TdrError::TDR_ERR_MINUS_REFER_VALUE;
    if (svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)svr_num_; ++i) {
        ret = cfg_storage_infos_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp;
    ret = destBuf.readUInt32(tmp);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    svr_num_ = (int32_t)tmp;
    if (svr_num_ < 0) return TdrError::TDR_ERR_MINUS_REFER_VALUE;
    if (svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)svr_num_; ++i) {
        ret = cfg_storage_infos_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svr_num_]", "%d", svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (svr_num_ < 0) return TdrError::TDR_ERR_MINUS_REFER_VALUE;
    if (svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)svr_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_storage_infos_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = cfg_storage_infos_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = cfg_storage_infos_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType StartSynCfgFileRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* StartSynCfgFileRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* StartSynCfgFileRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// SynSvrInfoRsp

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(curr_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)syn_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (syn_svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)syn_svr_num_; ++i) {
        ret = svr_info_list_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(curr_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.readUInt32(syn_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (syn_svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)syn_svr_num_; ++i) {
        ret = svr_info_list_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[curr_svr_num_]", "%u", curr_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[syn_svr_num_]", "%u", syn_svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (syn_svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)syn_svr_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svr_info_list_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = svr_info_list_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = svr_info_list_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType SynSvrInfoRsp::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SynSvrInfoRsp::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* SynSvrInfoRsp::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// UpdateCfgFileReq

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = cfg_info_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = cfg_info_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_num_]", "%u", cfg_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cfg_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)cfg_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_info_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = cfg_info_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = cfg_info_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateCfgFileReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* UpdateCfgFileReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* UpdateCfgFileReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

// UpdateSvrInfoReq

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::construct() {
    memset(this, 0, sizeof(*this));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)svr_num_; ++i) {
        ret = svr_update_info_list_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::unpack(tsf4g_tdr::TdrReadBuf& destBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.readUInt32(svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)svr_num_; ++i) {
        ret = svr_update_info_list_[i].unpack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svr_num_]", "%u", svr_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (svr_num_ > 2048) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < (uint32_t)svr_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[svr_update_info_list_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = svr_update_info_list_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = svr_update_info_list_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType UpdateSvrInfoReq::visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* UpdateSvrInfoReq::visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    ((char*)destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char* UpdateSvrInfoReq::visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.size();
    return buffer;
}

} // namespace sec_proto
