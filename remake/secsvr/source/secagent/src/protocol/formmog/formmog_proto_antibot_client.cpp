// Reconstructed from secagent disassembly (TDR-generated protocol classes)

#include "src/protocol/formmog/formmog_proto_antibot_client.h"
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
        char c;
        ret = destBuf.readChar(c);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        dest[i] = c;
    }
    if (dest[size - 1] != '\0') return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    if (strlen(dest) + 1 != size) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

} // anonymous namespace

namespace sec_proto {

using namespace tsf4g_tdr;


TdrError::ErrorType AB_CS_REPORT_ERROR::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AB_CS_REPORT_ERROR::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(error_func_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(error_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(error_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(error_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(eip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(error_env_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(reserved1_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(reserved2_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AB_CS_REPORT_ERROR::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType AB_CS_REPORT_ERROR::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(error_func_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(error_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(error_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(error_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(eip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        error_env_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt32(reserved1_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(reserved2_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AB_CS_REPORT_ERROR::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType AB_CS_REPORT_ERROR::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_func_id_]", "%d", error_func_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_type_]", "0x%02x", error_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_index_]", "%u", error_index_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_code_]", "%u", error_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[eip_]", "%u", eip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[error_env_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", error_env_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[reserved1_]", "%u", reserved1_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[reserved2_]", "%u", reserved2_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AB_CS_REPORT_ERROR::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* AB_CS_REPORT_ERROR::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* AB_CS_REPORT_ERROR::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType APCReportInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType APCReportInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ((char*)info_)[sizeof(info_) / sizeof(info_[0]) - 1] = '\0';
    ret = destBuf.writeString(info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType APCReportInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType APCReportInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = readBoundedString(srcBuf, info_, sizeof(info_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType APCReportInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType APCReportInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[info_]", info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType APCReportInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* APCReportInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* APCReportInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_DETESTPKG::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DETESTPKG::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < content_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < content_size_; ++i) {
        ret = destBuf.writeUInt8(content_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DETESTPKG::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DETESTPKG::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < content_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < content_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        content_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DETESTPKG::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DETESTPKG::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[test_pkg_crc_]", "%u", test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[content_size_]", "0x%02x", content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[content_]", content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ != 0) {
        for (uint32_t i = 0; i < content_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", content_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DETESTPKG::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_DETESTPKG::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_DETESTPKG::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_TVCHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TVCHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(tv_gap_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(tv_info_key_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(tv_check_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TVCHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_TVCHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(tv_gap_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        tv_info_key_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(tv_check_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TVCHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_TVCHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tv_gap_]", "%u", tv_gap_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[tv_info_key_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", tv_info_key_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tv_check_flag_]", "0x%02x", tv_check_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TVCHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_TVCHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_TVCHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_ANTICLTKEY::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTICLTKEY::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(signature_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTICLTKEY::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ANTICLTKEY::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        signature_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTICLTKEY::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ANTICLTKEY::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[flag_]", "%d", flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u", uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ip_]", "%u", ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[signature_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", signature_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTICLTKEY::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_ANTICLTKEY::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_ANTICLTKEY::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(service_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(service_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[service_id_]", "%u", service_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CHANNEL_SERVER_ID::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_CHANNEL_SERVER_ID::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_CHANNEL_SERVER_ID::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_STOPLCHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_STOPLCHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(stop_rand_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_STOPLCHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_STOPLCHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        stop_rand_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_STOPLCHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_STOPLCHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[stop_rand_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", stop_rand_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_STOPLCHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_STOPLCHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_STOPLCHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_CFGDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CFGDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CFGDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_CFGDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CFGDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_CFGDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[type_]", "0x%02x", type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "0x%02x", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ != 0) {
        for (uint32_t i = 0; i < len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CFGDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_CFGDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_CFGDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_CAPTCHAREQ::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHAREQ::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHAREQ::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CAPTCHAREQ::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHAREQ::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CAPTCHAREQ::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ != 0) {
        for (uint32_t i = 0; i < send_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHAREQ::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_CAPTCHAREQ::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_CAPTCHAREQ::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_FMSCMD::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FMSCMD::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(dumb_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(fms_scn_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_scn_cmd_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < fms_scn_cmd_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < fms_scn_cmd_len_; ++i) {
        ret = destBuf.writeUInt8(fms_scn_cmd_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FMSCMD::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_FMSCMD::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(dumb_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(fms_scn_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_scn_cmd_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < fms_scn_cmd_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < fms_scn_cmd_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        fms_scn_cmd_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FMSCMD::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_FMSCMD::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dumb_]", "%d", dumb_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[fms_scn_cmd_len_]", "%d", fms_scn_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_scn_cmd_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[fms_scn_cmd_data_]", fms_scn_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_scn_cmd_len_ != 0) {
        for (uint32_t i = 0; i < fms_scn_cmd_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", fms_scn_cmd_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FMSCMD::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_FMSCMD::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_FMSCMD::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_DELAY_CHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DELAY_CHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt64(secsvr_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt64(sdk_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DELAY_CHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DELAY_CHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt64(secsvr_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt64(sdk_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DELAY_CHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DELAY_CHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[secsvr_down_time_]", "%llu", secsvr_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sdk_down_time_]", "%llu", sdk_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DELAY_CHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_DELAY_CHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_DELAY_CHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_INITINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_INITINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(is_init_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(sig_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(rccp_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(module_handle_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(lib_base_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(os_ver_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(st_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(proc_address_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(sf_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt16(rc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ver_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(user_id_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(admin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(key_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(rp_data_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rp_data_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rp_data_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rp_data_info_size_; ++i) {
        ret = destBuf.writeUInt8(rp_data_info_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(cs_data_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_INITINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_INITINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(is_init_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(sig_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rccp_ver_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt32(module_handle_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(lib_base_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(os_ver_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        st_ver_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt32(proc_address_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        sf_ver_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt16(rc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ver_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        user_id_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(admin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(key_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(rp_data_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rp_data_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rp_data_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rp_data_info_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rp_data_info_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(cs_data_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_INITINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_INITINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[is_init_]", "0x%02x", is_init_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sig_flag_]", "0x%02x", sig_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rccp_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", rccp_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[module_handle_]", "%u", module_handle_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[lib_base_]", "%u", lib_base_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[os_ver_]", "0x%02x", os_ver_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[st_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", st_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[proc_address_]", "%u", proc_address_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[sf_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", sf_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rc_id_]", "%d", rc_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ver_]", "%u", client_ver_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[user_id_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", user_id_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[admin_]", "0x%02x", admin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dib_id_]", "0x%02x", dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_offset_]", "%u", key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_len_]", "0x%02x", key_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_data_info_size_]", "0x%02x", rp_data_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rp_data_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rp_data_info_]", rp_data_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rp_data_info_size_ != 0) {
        for (uint32_t i = 0; i < rp_data_info_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", rp_data_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_data_]", "0x%02x", cs_data_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "0x%02x", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (len_ != 0) {
        for (uint32_t i = 0; i < len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_INITINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_INITINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_INITINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_CSCHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(rpcs_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rpcs_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rpcs_result_len_; ++i) {
        ret = destBuf.writeUInt8(rpcs_result_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CSCHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(rpcs_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rpcs_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rpcs_result_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rpcs_result_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CSCHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpcs_result_len_]", "%d", rpcs_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rpcs_result_]", rpcs_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_result_len_ != 0) {
        for (uint32_t i = 0; i < rpcs_result_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpcs_result_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_CSCHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_CSCHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TQD_CS_INITDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_INITDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(mem_sign_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(file_sign_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(scan_engine_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(clean_engine_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_INITDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TQD_CS_INITDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        mem_sign_ver_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        file_sign_ver_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        scan_engine_ver_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        clean_engine_ver_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_INITDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TQD_CS_INITDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc_]", "%u", crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[mem_sign_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", mem_sign_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[file_sign_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", file_sign_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[scan_engine_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", scan_engine_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[clean_engine_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", clean_engine_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_INITDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TQD_CS_INITDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TQD_CS_INITDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SClientGUIDInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SClientGUIDInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(guid_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(reserve_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SClientGUIDInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SClientGUIDInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        guid_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        reserve_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SClientGUIDInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SClientGUIDInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[guid_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", guid_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[reserve_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", reserve_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SClientGUIDInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SClientGUIDInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SClientGUIDInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_CSCFGDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CSCFGDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(encrypt_key_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt16(rpcs_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rpcs_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rpcs_data_len_; ++i) {
        ret = destBuf.writeUInt8(rpcs_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CSCFGDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_CSCFGDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        encrypt_key_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt16(rpcs_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rpcs_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rpcs_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rpcs_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CSCFGDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_CSCFGDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[encrypt_key_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", encrypt_key_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpcs_data_len_]", "%d", rpcs_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rpcs_data_]", rpcs_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcs_data_len_ != 0) {
        for (uint32_t i = 0; i < rpcs_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpcs_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_CSCFGDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_CSCFGDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_CSCFGDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_SYSMEMLOG::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEMLOG::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(system_log_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_log_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < system_log_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < system_log_size_; ++i) {
        ret = destBuf.writeUInt8(system_log_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEMLOG::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SYSMEMLOG::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(system_log_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_log_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < system_log_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < system_log_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        system_log_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEMLOG::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SYSMEMLOG::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[system_log_size_]", "0x%02x", system_log_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_log_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[system_log_]", system_log_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_log_size_ != 0) {
        for (uint32_t i = 0; i < system_log_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", system_log_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEMLOG::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_SYSMEMLOG::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_SYSMEMLOG::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_count_ > 100) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < cmd_count_; ++i) {
        ret = destBuf.writeUInt16(cmd_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_count_ > 100) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < cmd_count_; ++i) {
        ret = srcBuf.readUInt16(cmd_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_count_]", "0x%02x", cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_count_ > 100) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cmd_]", cmd_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_count_ != 0) {
        for (uint32_t i = 0; i < cmd_count_; ++i) {
            ret = destBuf.textize(" %d", cmd_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DE_CMDID_LIST::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_DE_CMDID_LIST::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_DE_CMDID_LIST::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_SUSMODINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SUSMODINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(sus_mod_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_mod_info_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < sus_mod_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < sus_mod_info_len_; ++i) {
        ret = destBuf.writeUInt8(sus_mod_info_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SUSMODINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SUSMODINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(sus_mod_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_mod_info_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < sus_mod_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < sus_mod_info_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        sus_mod_info_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SUSMODINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SUSMODINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sus_mod_info_len_]", "%d", sus_mod_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_mod_info_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[sus_mod_info_data_]", sus_mod_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_mod_info_len_ != 0) {
        for (uint32_t i = 0; i < sus_mod_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", sus_mod_info_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SUSMODINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_SUSMODINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_SUSMODINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_PKGCODE::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(rp_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rpcode_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_data_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rpcode_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rpcode_data_size_; ++i) {
        ret = destBuf.writeUInt8(rpcode_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_PKGCODE::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(rp_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rpcode_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_data_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rpcode_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rpcode_data_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rpcode_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_PKGCODE::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_id_]", "%d", rp_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpcode_data_size_]", "%d", rpcode_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_data_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rpcode_data_]", rpcode_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_data_size_ != 0) {
        for (uint32_t i = 0; i < rpcode_data_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpcode_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_PKGCODE::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_PKGCODE::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_TENPRTYDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TENPRTYDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(tenpary_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tenpary_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < tenpary_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < tenpary_data_len_; ++i) {
        ret = destBuf.writeUInt8(tenpary_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TENPRTYDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_TENPRTYDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(tenpary_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tenpary_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < tenpary_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < tenpary_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        tenpary_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TENPRTYDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_TENPRTYDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tenpary_data_len_]", "%d", tenpary_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tenpary_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[tenpary_data_]", tenpary_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tenpary_data_len_ != 0) {
        for (uint32_t i = 0; i < tenpary_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", tenpary_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TENPRTYDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_TENPRTYDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_TENPRTYDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_VERIFYRESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_VERIFYRESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(ext_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(stamp_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(module_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(out_proc_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(trojan_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(bot_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < err_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < err_msg_size_; ++i) {
        ret = destBuf.writeUInt8(err_msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(client_passitive_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_rand_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 2) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 2; ++i) {
        ret = destBuf.writeUInt8(rand_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(local_check_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 8) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 8; ++i) {
        ret = destBuf.writeUInt8(rc_check_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_VERIFYRESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_VERIFYRESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(ext_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        stamp_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        module_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        out_proc_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        trojan_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(bot_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < err_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < err_msg_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        err_msg_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(client_passitive_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_rand_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 2) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 2; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rand_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(local_check_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 8) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 8; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rc_check_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_VERIFYRESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_VERIFYRESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ext_size_]", "%d", ext_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u", uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[stamp_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", stamp_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[module_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", module_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[out_proc_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", out_proc_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[trojan_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", trojan_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_id_]", "0x%02x", game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[bot_id_]", "0x%02x", bot_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[err_msg_size_]", "0x%02x", err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[err_msg_]", err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ != 0) {
        for (uint32_t i = 0; i < err_msg_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", err_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_passitive_]", "0x%02x", client_passitive_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_rand_value_]", "%u", client_rand_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "0x%02x", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ != 0) {
        for (uint32_t i = 0; i < send_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rand_]", 2);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 2; ++i) {
        ret = destBuf.textize(" 0x%02x", rand_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[local_check_flag_]", "0x%02x", local_check_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rc_check_]", 8);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 8; ++i) {
        ret = destBuf.textize(" 0x%02x", rc_check_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_VERIFYRESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_VERIFYRESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_VERIFYRESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(error_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(error_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_code_]", "%u", error_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CSCHANNEL_RESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_CSCHANNEL_RESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_CSCHANNEL_RESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(rand_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rand_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rand_data_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", rand_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[type_]", "0x%02x", type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_REQCLIRESENDDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_REQCLIRESENDDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_REQCLIRESENDDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_DEREADY::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DEREADY::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < err_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < err_msg_size_; ++i) {
        ret = destBuf.writeUInt8(err_msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DEREADY::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DEREADY::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < err_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < err_msg_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        err_msg_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DEREADY::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DEREADY::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[id_]", "0x%02x", id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_crc_]", "%u", key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[err_msg_size_]", "0x%02x", err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[err_msg_]", err_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_msg_size_ != 0) {
        for (uint32_t i = 0; i < err_msg_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", err_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DEREADY::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_DEREADY::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_DEREADY::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_DELOG::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELOG::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(error_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_size_; ++i) {
        ret = destBuf.writeUInt8(buf_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELOG::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DELOG::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(error_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        buf_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELOG::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DELOG::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_id_]", "0x%02x", error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_]", "%u", error_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_size_]", "0x%02x", pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[buf_]", pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ != 0) {
        for (uint32_t i = 0; i < pad_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", buf_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELOG::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_DELOG::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_DELOG::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TMODULE_NAME::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMODULE_NAME::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (name_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < name_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < name_len_; ++i) {
        ret = destBuf.writeUInt8(module_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMODULE_NAME::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TMODULE_NAME::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (name_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < name_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < name_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        module_name_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMODULE_NAME::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TMODULE_NAME::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[name_len_]", "0x%02x", name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (name_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[module_name_]", name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (name_len_ != 0) {
        for (uint32_t i = 0; i < name_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", module_name_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMODULE_NAME::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TMODULE_NAME::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TMODULE_NAME::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TFILE_HASH_INFO_ITEM::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO_ITEM::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = file_name_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(file_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO_ITEM::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TFILE_HASH_INFO_ITEM::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = file_name_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(file_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO_ITEM::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TFILE_HASH_INFO_ITEM::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[id_]", "0x%02x", id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_name_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = file_name_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = file_name_.visualize(destBuf, indent, separator);
    }
    return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_type_]", "0x%02x", file_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[offset_]", "%u", offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "%u", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO_ITEM::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TFILE_HASH_INFO_ITEM::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TFILE_HASH_INFO_ITEM::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TFILE_HASH_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = file_hash_item_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TFILE_HASH_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = file_hash_item_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TFILE_HASH_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[count_]", "%d", count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return ret;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_hash_item_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = file_hash_item_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = file_hash_item_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TFILE_HASH_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TFILE_HASH_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TFILE_HASH_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_FILE_HASH::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FILE_HASH::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = file_hash_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FILE_HASH::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_FILE_HASH::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = file_hash_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FILE_HASH::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_FILE_HASH::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[buf_len_]", "%d", buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_hash_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = file_hash_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = file_hash_.visualize(destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_FILE_HASH::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_FILE_HASH::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_FILE_HASH::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TROJANINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TROJANINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(virus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(mod_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 512) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 512; ++i) {
        ret = destBuf.writeUInt8(path_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TROJANINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TROJANINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(virus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(mod_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 512) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 512; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        path_name_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TROJANINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TROJANINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[virus_id_]", "%u", virus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mod_size_]", "%u", mod_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[path_name_]", 512);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 512; ++i) {
        ret = destBuf.textize(" 0x%02x", path_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TROJANINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TROJANINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TROJANINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SLX_TROJAN_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLX_TROJAN_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(trojan_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (trojan_count_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < trojan_count_; ++i) {
        ret = trojans_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLX_TROJAN_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SLX_TROJAN_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(trojan_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (trojan_count_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < trojan_count_; ++i) {
        ret = trojans_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLX_TROJAN_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SLX_TROJAN_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[trojan_count_]", "%u", trojan_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (trojan_count_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (trojan_count_ == 0) return ret;
    for (uint32_t i = 0; i < trojan_count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[trojans_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = trojans_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = trojans_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLX_TROJAN_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SLX_TROJAN_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SLX_TROJAN_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TROJAN_REPORT::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = trojans_.construct();
    break;
    case 1:
    trojan_count_ = 0;
    ret = TdrError::TDR_NO_ERROR;
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TROJAN_REPORT::pack(int64_t union_selector, TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = trojans_.pack(destBuf, 1);
    break;
    case 1:
    ret = destBuf.writeUInt32(trojan_count_);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TROJAN_REPORT::pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TROJAN_REPORT::unpack(int64_t union_selector, TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = trojans_.unpack(srcBuf, 1);
    break;
    case 1:
    ret = srcBuf.readUInt32(trojan_count_);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TROJAN_REPORT::unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TROJAN_REPORT::visualize(int64_t union_selector, TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[trojans_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = trojans_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = trojans_.visualize(destBuf, indent, separator);
    }
    break;
    case 1:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[trojan_count_]", "%u", trojan_count_);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TROJAN_REPORT::visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TSANLIX_CS_REPORT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TSANLIX_CS_REPORT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = trojan_.pack((int64_t)type_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TSANLIX_CS_REPORT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TSANLIX_CS_REPORT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = trojan_.unpack((int64_t)type_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TSANLIX_CS_REPORT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TSANLIX_CS_REPORT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[type_]", "0x%02x", type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "%d", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[trojan_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = trojan_.visualize((int64_t)type_, destBuf, indent + 1, separator);
    } else {
        ret = trojan_.visualize((int64_t)type_, destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TSANLIX_CS_REPORT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TSANLIX_CS_REPORT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TSANLIX_CS_REPORT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rpfmc_scan_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rpfmc_scan_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rpfmc_scan_result_len_; ++i) {
        ret = destBuf.writeUInt8(rpfmc_scan_result_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rpfmc_scan_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rpfmc_scan_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rpfmc_scan_result_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rpfmc_scan_result_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[hash_]", "%u", hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpfmc_scan_result_len_]", "%d", rpfmc_scan_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rpfmc_scan_result_data_]", rpfmc_scan_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_result_len_ != 0) {
        for (uint32_t i = 0; i < rpfmc_scan_result_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpfmc_scan_result_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPFMCSCANRESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_RPFMCSCANRESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_RPFMCSCANRESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_LAPKGCMD::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_LAPKGCMD::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(la_cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_cmd_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < la_cmd_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < la_cmd_size_; ++i) {
        ret = destBuf.writeUInt8(la_cmd_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_data_size_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_LAPKGCMD::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_LAPKGCMD::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(la_cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_cmd_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < la_cmd_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < la_cmd_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        la_cmd_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_data_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_LAPKGCMD::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_LAPKGCMD::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[la_cmd_size_]", "%d", la_cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_cmd_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[la_cmd_data_]", la_cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_cmd_size_ != 0) {
        for (uint32_t i = 0; i < la_cmd_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", la_cmd_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_data_size_]", "0x%02x", pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ != 0) {
        for (uint32_t i = 0; i < pad_data_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_LAPKGCMD::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_LAPKGCMD::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_LAPKGCMD::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_RESENDDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RESENDDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(data_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(re_send_reason_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(resend_data_info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RESENDDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_RESENDDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(data_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(re_send_reason_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(resend_data_info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RESENDDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_RESENDDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_flag_]", "0x%02x", data_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[re_send_reason_]", "0x%02x", re_send_reason_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[resend_data_info_]", "%u", resend_data_info_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RESENDDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_RESENDDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_RESENDDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_CLIENTINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLIENTINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(client_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_info_msg_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < client_info_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < client_info_msg_size_; ++i) {
        ret = destBuf.writeUInt8(client_info_msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLIENTINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CLIENTINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(client_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_info_msg_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < client_info_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < client_info_msg_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        client_info_msg_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLIENTINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CLIENTINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_info_msg_size_]", "%d", client_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_info_msg_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[client_info_msg_]", client_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (client_info_msg_size_ != 0) {
        for (uint32_t i = 0; i < client_info_msg_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", client_info_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLIENTINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_CLIENTINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_CLIENTINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_DEKEY::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEKEY::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_data_size_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(key_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEKEY::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DEKEY::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_data_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt32(key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(key_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEKEY::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DEKEY::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_data_size_]", "0x%02x", pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ != 0) {
        for (uint32_t i = 0; i < pad_data_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_crc_]", "%u", key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_len_]", "0x%02x", key_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_offset_]", "%u", key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEKEY::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_DEKEY::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_DEKEY::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_ENCRYPT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ENCRYPT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pkg_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pkg_len_; ++i) {
        ret = destBuf.writeUInt8(pkg_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ENCRYPT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ENCRYPT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pkg_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pkg_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pkg_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ENCRYPT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ENCRYPT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[test_pkg_crc_]", "%u", test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pkg_len_]", "%d", pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pkg_data_]", pkg_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_len_ != 0) {
        for (uint32_t i = 0; i < pkg_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pkg_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ENCRYPT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_ENCRYPT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_ENCRYPT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_FMCRESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_FMCRESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(fms_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < fms_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < fms_result_len_; ++i) {
        ret = destBuf.writeUInt8(fms_result_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_FMCRESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_FMCRESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(fms_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < fms_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < fms_result_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        fms_result_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_FMCRESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_FMCRESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[fms_result_len_]", "%d", fms_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[fms_result_data_]", fms_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (fms_result_len_ != 0) {
        for (uint32_t i = 0; i < fms_result_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", fms_result_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_FMCRESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_FMCRESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_FMCRESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_CAPTCHARESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHARESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHARESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CAPTCHARESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHARESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CAPTCHARESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ != 0) {
        for (uint32_t i = 0; i < send_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CAPTCHARESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_CAPTCHARESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_CAPTCHARESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_PKGCODE_KEY::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE_KEY::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(key_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_info_len_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < key_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < key_info_len_; ++i) {
        ret = destBuf.writeUInt8(key_info_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE_KEY::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_PKGCODE_KEY::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(key_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_info_len_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < key_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < key_info_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        key_info_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE_KEY::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_PKGCODE_KEY::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_info_len_]", "0x%02x", key_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_info_len_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[key_info_]", key_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_info_len_ != 0) {
        for (uint32_t i = 0; i < key_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", key_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_PKGCODE_KEY::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_PKGCODE_KEY::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_PKGCODE_KEY::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_DEDIBDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEDIBDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(dib_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(dib_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (dib_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < dib_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < dib_data_len_; ++i) {
        ret = destBuf.writeUInt8(dib_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEDIBDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DEDIBDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(dib_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(dib_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (dib_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < dib_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < dib_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        dib_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEDIBDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_DEDIBDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[id_]", "0x%02x", id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dib_]", "0x%02x", dib_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dib_data_len_]", "%d", dib_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (dib_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[dib_data_]", dib_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (dib_data_len_ != 0) {
        for (uint32_t i = 0; i < dib_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", dib_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_DEDIBDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_DEDIBDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_DEDIBDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType FILE_HASH_VAL::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType FILE_HASH_VAL::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(file_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(file_hash_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType FILE_HASH_VAL::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType FILE_HASH_VAL::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(file_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        file_hash_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType FILE_HASH_VAL::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType FILE_HASH_VAL::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_id_]", "0x%02x", file_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[file_hash_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", file_hash_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType FILE_HASH_VAL::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* FILE_HASH_VAL::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* FILE_HASH_VAL::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_CLINETINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLINETINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(file_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (file_num_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < file_num_; ++i) {
        ret = file_hash_data_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLINETINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CLINETINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(file_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (file_num_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < file_num_; ++i) {
        ret = file_hash_data_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLINETINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_CLINETINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_num_]", "0x%02x", file_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (file_num_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (file_num_ == 0) return ret;
    for (uint32_t i = 0; i < file_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[file_hash_data_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = file_hash_data_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = file_hash_data_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_CLINETINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_CLINETINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_CLINETINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_IATCHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_IATCHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(iat_check_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(iat_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (iat_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < iat_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < iat_info_size_; ++i) {
        ret = destBuf.writeUInt8(iat_info_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_IATCHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_IATCHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(iat_check_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(iat_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (iat_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < iat_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < iat_info_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        iat_info_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_IATCHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_IATCHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[iat_check_result_]", "0x%02x", iat_check_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[iat_info_size_]", "0x%02x", iat_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (iat_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[iat_info_]", iat_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (iat_info_size_ != 0) {
        for (uint32_t i = 0; i < iat_info_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", iat_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_IATCHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_IATCHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_IATCHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(clr_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rpfmc_scan_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_cmd_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rpfmc_scan_cmd_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rpfmc_scan_cmd_len_; ++i) {
        ret = destBuf.writeUInt8(rpfmc_scan_cmd_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(clr_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rpfmc_scan_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_cmd_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rpfmc_scan_cmd_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rpfmc_scan_cmd_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rpfmc_scan_cmd_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[clr_flag_]", "0x%02x", clr_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpfmc_scan_cmd_len_]", "%d", rpfmc_scan_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_cmd_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rpfmc_scan_cmd_data_]", rpfmc_scan_cmd_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpfmc_scan_cmd_len_ != 0) {
        for (uint32_t i = 0; i < rpfmc_scan_cmd_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpfmc_scan_cmd_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RPFMCSCANCMD::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_RPFMCSCANCMD::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_RPFMCSCANCMD::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TQD_SC_CMDDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CMDDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(hash_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CMDDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TQD_SC_CMDDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        hash_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CMDDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TQD_SC_CMDDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc_]", "%u", crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "%d", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[hash_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", hash_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CMDDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TQD_SC_CMDDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TQD_SC_CMDDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_SENDANTIBOT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SENDANTIBOT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(init_crc_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(send_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(data_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rcid_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 32727) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 32727; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SENDANTIBOT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SENDANTIBOT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(init_crc_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_ver_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(data_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rcid_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 32727) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 32727; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SENDANTIBOT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SENDANTIBOT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[init_crc_value_]", "%u", init_crc_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_ver_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", send_ver_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_flag_]", "0x%02x", data_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rcid_]", "%d", rcid_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", 32727);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 32727; ++i) {
        ret = destBuf.textize(" 0x%02x", send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SENDANTIBOT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_SENDANTIBOT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_SENDANTIBOT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_ENCRYPT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ENCRYPT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(pkg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pkg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pkg_size_; ++i) {
        ret = destBuf.writeUInt8(pkg_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ENCRYPT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_ENCRYPT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(pkg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pkg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pkg_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pkg_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ENCRYPT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_ENCRYPT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[test_pkg_crc_]", "%u", test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pkg_size_]", "%d", pkg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pkg_data_]", pkg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pkg_size_ != 0) {
        for (uint32_t i = 0; i < pkg_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", pkg_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ENCRYPT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_ENCRYPT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_ENCRYPT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ThreadVerifyInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ThreadVerifyInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(thread_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(tp_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(ts_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ThreadVerifyInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ThreadVerifyInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(thread_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(tp_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(ts_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ThreadVerifyInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ThreadVerifyInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[thread_id_]", "%d", thread_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[status_]", "%d", status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_time_]", "%u", tp_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ts_time_]", "%u", ts_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ThreadVerifyInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ThreadVerifyInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ThreadVerifyInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_TVINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TVINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(main_code_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(main_code_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(tv_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(thread_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (thread_num_ > 20) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < thread_num_; ++i) {
        ret = thread_verify_infos_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_size_; ++i) {
        ret = destBuf.writeUInt8(pad_buf_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TVINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_TVINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(main_code_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(main_code_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(tv_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(thread_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (thread_num_ > 20) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < thread_num_; ++i) {
        ret = thread_verify_infos_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = srcBuf.readUInt8(pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_buf_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TVINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_TVINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_crc_]", "%u", key_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[main_code_offset_]", "%u", main_code_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[main_code_size_]", "%u", main_code_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tv_flag_]", "0x%02x", tv_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[thread_num_]", "%u", thread_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (thread_num_ > 20) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (thread_num_ == 0) return ret;
    for (uint32_t i = 0; i < thread_num_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[thread_verify_infos_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = thread_verify_infos_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = thread_verify_infos_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_size_]", "0x%02x", pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_buf_]", pad_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_size_ != 0) {
        for (uint32_t i = 0; i < pad_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_buf_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TVINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_TVINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_TVINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_LAPKGRESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LAPKGRESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(la_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < la_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < la_result_len_; ++i) {
        ret = destBuf.writeUInt8(aucLaResultData[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LAPKGRESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_LAPKGRESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(la_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < la_result_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < la_result_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        aucLaResultData[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt32(serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LAPKGRESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_LAPKGRESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[hash_]", "%u", hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[la_result_len_]", "%d", la_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_result_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[aucLaResultData]", la_result_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (la_result_len_ != 0) {
        for (uint32_t i = 0; i < la_result_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", aucLaResultData[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[serial_]", "%u", serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LAPKGRESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_LAPKGRESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_LAPKGRESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TQD_CS_VIRUSINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_VIRUSINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(process_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 64) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 64; ++i) {
        ret = destBuf.writeUInt8(virus_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 512) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 512; ++i) {
        ret = destBuf.writeUInt8(module_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_VIRUSINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TQD_CS_VIRUSINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(process_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 64) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 64; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        virus_name_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 512) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 512; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        module_name_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_VIRUSINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TQD_CS_VIRUSINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[process_type_]", "%u", process_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[virus_name_]", 64);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 64; ++i) {
        ret = destBuf.textize(" 0x%02x", virus_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[module_name_]", 512);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 512; ++i) {
        ret = destBuf.textize(" 0x%02x", module_name_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_VIRUSINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TQD_CS_VIRUSINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TQD_CS_VIRUSINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TQD_CS_SCANRESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_SCANRESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(data_len_temp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_temp_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_temp_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < data_len_temp_; ++i) {
        ret = destBuf.writeUInt8(data_temp_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(virus_cnt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (virus_cnt_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < virus_cnt_; ++i) {
        ret = virus_info_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_SCANRESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TQD_CS_SCANRESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(data_len_temp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_temp_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < data_len_temp_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < data_len_temp_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        data_temp_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(virus_cnt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (virus_cnt_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < virus_cnt_; ++i) {
        ret = virus_info_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_SCANRESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TQD_CS_SCANRESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc_]", "%u", crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_temp_]", "%d", data_len_temp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_temp_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_temp_]", data_len_temp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_temp_ != 0) {
        for (uint32_t i = 0; i < data_len_temp_; ++i) {
            ret = destBuf.textize(" 0x%02x", data_temp_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u", uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[virus_cnt_]", "%d", virus_cnt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (virus_cnt_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (virus_cnt_ == 0) return ret;
    for (uint32_t i = 0; i < virus_cnt_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[virus_info_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = virus_info_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = virus_info_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_CS_SCANRESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TQD_CS_SCANRESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TQD_CS_SCANRESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_RESTRICTUIN::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESTRICTUIN::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(ban_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_data_size_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESTRICTUIN::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RESTRICTUIN::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(ban_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_data_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESTRICTUIN::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RESTRICTUIN::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ban_time_]", "%u", ban_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_data_size_]", "0x%02x", pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_size_ != 0) {
        for (uint32_t i = 0; i < pad_data_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESTRICTUIN::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_RESTRICTUIN::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_RESTRICTUIN::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_RPPKGRESULT::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPPKGRESULT::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(rpcode_data_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(rpcode_result_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < rpcode_result_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < rpcode_result_data_size_; ++i) {
        ret = destBuf.writeUInt8(rpcode_result_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(rpcode_func_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(hash_idx_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPPKGRESULT::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_RPPKGRESULT::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(rpcode_data_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(rpcode_result_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < rpcode_result_data_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < rpcode_result_data_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rpcode_result_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(rpcode_func_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(hash_idx_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPPKGRESULT::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_RPPKGRESULT::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpcode_data_hash_]", "%u", rpcode_data_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpcode_result_data_size_]", "%d", rpcode_result_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_size_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rpcode_result_data_]", rpcode_result_data_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (rpcode_result_data_size_ != 0) {
        for (uint32_t i = 0; i < rpcode_result_data_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", rpcode_result_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rpcode_func_id_]", "0x%02x", rpcode_func_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[serial_]", "%u", serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[hash_idx_]", "%d", hash_idx_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[hash_]", "%u", hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_RPPKGRESULT::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_RPPKGRESULT::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_RPPKGRESULT::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_ANTITIME::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTITIME::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTITIME::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ANTITIME::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTITIME::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ANTITIME::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u", uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]", "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_]", "%u", seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ANTITIME::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_ANTITIME::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_ANTITIME::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SAB_MAC_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_MAC_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(ipcs_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(read_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(read_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(qq_login_mac_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_mac_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(qq_login_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(game_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_MAC_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SAB_MAC_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(ipcs_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(read_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(read_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(qq_login_mac_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(game_mac_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(qq_login_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(game_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_MAC_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SAB_MAC_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ipcs_status_]", "0x%02x", ipcs_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[read_flag_]", "0x%02x", read_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[read_status_]", "%u", read_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[qq_login_mac_crc_]", "%u", qq_login_mac_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_mac_crc_]", "%u", game_mac_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[qq_login_uin_]", "%u", qq_login_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_uin_]", "%u", game_uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_MAC_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SAB_MAC_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SAB_MAC_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SRP_RPCODE_HASH::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_RPCODE_HASH::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(fun_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(pad_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_data_len_; ++i) {
        ret = destBuf.writeUInt8(pad_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(hash_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (hash_count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < hash_count_; ++i) {
        ret = destBuf.writeUInt32(hash_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt32(times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(send_func_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(no_die_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(garbage_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (garbage_data_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < garbage_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < garbage_data_len_; ++i) {
        ret = destBuf.writeUInt8(garbage_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_RPCODE_HASH::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SRP_RPCODE_HASH::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(fun_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(pad_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(hash_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (hash_count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < hash_count_; ++i) {
        ret = srcBuf.readUInt32(hash_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = srcBuf.readUInt32(times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(send_func_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(no_die_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(garbage_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (garbage_data_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < garbage_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < garbage_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        garbage_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_RPCODE_HASH::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SRP_RPCODE_HASH::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[fun_id_]", "0x%02x", fun_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_data_len_]", "0x%02x", pad_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_]", pad_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_data_len_ != 0) {
        for (uint32_t i = 0; i < pad_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[hash_count_]", "0x%02x", hash_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (hash_count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[hash_]", hash_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (hash_count_ != 0) {
        for (uint32_t i = 0; i < hash_count_; ++i) {
            ret = destBuf.textize(" %u", hash_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[times_]", "%u", times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_func_addr_]", "%u", send_func_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[no_die_value_]", "%u", no_die_value_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[garbage_data_len_]", "0x%02x", garbage_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (garbage_data_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[garbage_]", garbage_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (garbage_data_len_ != 0) {
        for (uint32_t i = 0; i < garbage_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", garbage_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_RPCODE_HASH::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SRP_RPCODE_HASH::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SRP_RPCODE_HASH::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SanlixScanResult::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanResult::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)scan_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(sanlix_dat_version_low_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(sanlix_dat_version_high_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(sanlix_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ((char*)virus_name_)[sizeof(virus_name_) / sizeof(virus_name_[0]) - 1] = '\0';
    ret = destBuf.writeString(virus_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ((char*)scan_module_)[sizeof(scan_module_) / sizeof(scan_module_[0]) - 1] = '\0';
    ret = destBuf.writeString(scan_module_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanResult::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SanlixScanResult::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    uint32_t tmp4scan_result_;
    ret = srcBuf.readUInt32(tmp4scan_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    scan_result_ = (int32_t)tmp4scan_result_;
    ret = srcBuf.readUInt32(sanlix_dat_version_low_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(sanlix_dat_version_high_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(sanlix_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = readBoundedString(srcBuf, virus_name_, sizeof(virus_name_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = readBoundedString(srcBuf, scan_module_, sizeof(scan_module_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanResult::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SanlixScanResult::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[scan_result_]", "%d", scan_result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sanlix_dat_version_low_]", "%u", sanlix_dat_version_low_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sanlix_dat_version_high_]", "%u", sanlix_dat_version_high_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sanlix_id_]", "%u", sanlix_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[flag_]", "0x%02x", flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[virus_name_]", virus_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[scan_module_]", scan_module_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanResult::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SanlixScanResult::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SanlixScanResult::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TVMPFuncHashInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncHashInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncHashInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TVMPFuncHashInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncHashInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TVMPFuncHashInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[addr_]", "%u", addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[hash_]", "%u", hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncHashInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TVMPFuncHashInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TVMPFuncHashInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType STVMP_REPORT_HASH_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_HASH_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = func_hash_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_HASH_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType STVMP_REPORT_HASH_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = func_hash_info_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_HASH_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType STVMP_REPORT_HASH_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[func_hash_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = func_hash_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = func_hash_info_.visualize(destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_HASH_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* STVMP_REPORT_HASH_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* STVMP_REPORT_HASH_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SSchemeClientGUIDInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSchemeClientGUIDInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = guid_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSchemeClientGUIDInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SSchemeClientGUIDInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = guid_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSchemeClientGUIDInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SSchemeClientGUIDInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[guid_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = guid_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = guid_.visualize(destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSchemeClientGUIDInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SSchemeClientGUIDInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SSchemeClientGUIDInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ShellModuleVerify::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ShellModuleVerify::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(verify_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(verify_start_ptr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(verify_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(load_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(verify_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ShellModuleVerify::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ShellModuleVerify::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(verify_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(verify_start_ptr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(verify_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(load_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(verify_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ShellModuleVerify::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ShellModuleVerify::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[verify_type_]", "%u", verify_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[verify_start_ptr_]", "%u", verify_start_ptr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[verify_len_]", "%u", verify_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[load_hash_]", "%u", load_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[verify_hash_]", "%u", verify_hash_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ShellModuleVerify::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ShellModuleVerify::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ShellModuleVerify::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType STPKRReportInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STPKRReportInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(major_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(minor_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)status_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STPKRReportInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType STPKRReportInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(major_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(minor_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    uint32_t tmp4status_code_;
    ret = srcBuf.readUInt32(tmp4status_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    status_code_ = (int32_t)tmp4status_code_;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STPKRReportInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType STPKRReportInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[major_code_]", "%u", major_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[minor_code_]", "%u", minor_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[status_code_]", "%d", status_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STPKRReportInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* STPKRReportInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* STPKRReportInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TVMPFuncCallInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncCallInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(call_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(ret_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncCallInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TVMPFuncCallInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(call_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(ret_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncCallInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TVMPFuncCallInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[call_addr_]", "%u", call_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ret_addr_]", "%u", ret_addr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TVMPFuncCallInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TVMPFuncCallInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TVMPFuncCallInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType STVMP_REPORT_CALL_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_CALL_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(func_call_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (func_call_info_size_ > 20) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < func_call_info_size_; ++i) {
        ret = func_call_infos_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_CALL_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType STVMP_REPORT_CALL_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(func_call_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (func_call_info_size_ > 20) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < func_call_info_size_; ++i) {
        ret = func_call_infos_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_CALL_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType STVMP_REPORT_CALL_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[func_call_info_size_]", "%u", func_call_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (func_call_info_size_ > 20) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (func_call_info_size_ == 0) return ret;
    for (uint32_t i = 0; i < func_call_info_size_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[func_call_infos_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = func_call_infos_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = func_call_infos_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STVMP_REPORT_CALL_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* STVMP_REPORT_CALL_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* STVMP_REPORT_CALL_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SCS_SLN_DATA_BUF::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SLN_DATA_BUF::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SLN_DATA_BUF::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SCS_SLN_DATA_BUF::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SLN_DATA_BUF::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SCS_SLN_DATA_BUF::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ != 0) {
        for (uint32_t i = 0; i < send_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SLN_DATA_BUF::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SCS_SLN_DATA_BUF::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SCS_SLN_DATA_BUF::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SAB_SIGN_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_SIGN_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(mac_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(sig_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_size_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < sig_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < sig_size_; ++i) {
        ret = destBuf.writeUInt8(sig_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_SIGN_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SAB_SIGN_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(mac_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(sig_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_size_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < sig_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < sig_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        sig_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_SIGN_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SAB_SIGN_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mac_id_]", "%u", mac_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sig_size_]", "%u", sig_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_size_ > 10240) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[sig_data_]", sig_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sig_size_ != 0) {
        for (uint32_t i = 0; i < sig_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", sig_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SAB_SIGN_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SAB_SIGN_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SAB_SIGN_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SLogReport::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLogReport::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)data_source_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(log_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(int_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (int_count_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < int_count_; ++i) {
        ret = destBuf.writeUInt32(int_values_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ((char*)comment_)[sizeof(comment_) / sizeof(comment_[0]) - 1] = '\0';
    ret = destBuf.writeString(comment_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLogReport::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SLogReport::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    uint32_t tmp4data_source_;
    ret = srcBuf.readUInt32(tmp4data_source_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    data_source_ = (int32_t)tmp4data_source_;
    uint32_t tmp4game_id_;
    ret = srcBuf.readUInt32(tmp4game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    game_id_ = (int32_t)tmp4game_id_;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(log_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(int_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (int_count_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < int_count_; ++i) {
        uint32_t tmp4int_values_;
        ret = srcBuf.readUInt32(tmp4int_values_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        int_values_[i] = (int32_t)tmp4int_values_;
    }
    ret = readBoundedString(srcBuf, comment_, sizeof(comment_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLogReport::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SLogReport::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_source_]", "%d", data_source_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[game_id_]", "%d", game_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u", uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[log_type_]", "%u", log_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[int_count_]", "0x%02x", int_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (int_count_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[int_values_]", int_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (int_count_ != 0) {
        for (uint32_t i = 0; i < int_count_; ++i) {
            ret = destBuf.textize(" %d", int_values_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[comment_]", comment_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SLogReport::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SLogReport::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SLogReport::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType STP_ERROR_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_ERROR_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(number_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(reserved_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_ERROR_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType STP_ERROR_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(number_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(reserved_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_ERROR_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType STP_ERROR_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[type_]", "%u", type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[number_]", "%u", number_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[reserved_]", "%u", reserved_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_ERROR_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* STP_ERROR_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* STP_ERROR_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SSE_ERROR_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSE_ERROR_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(error_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSE_ERROR_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SSE_ERROR_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(error_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSE_ERROR_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SSE_ERROR_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_]", "%u", error_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSE_ERROR_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SSE_ERROR_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SSE_ERROR_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SRP_THREAD_EXP::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_THREAD_EXP::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(eip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(exception_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 32) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 32; ++i) {
        ret = destBuf.writeUInt8(except_code_array_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_THREAD_EXP::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SRP_THREAD_EXP::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(eip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(exception_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 32) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 32; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        except_code_array_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_THREAD_EXP::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SRP_THREAD_EXP::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[eip_]", "%u", eip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[exception_code_]", "%u", exception_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[except_code_array_]", 32);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 32; ++i) {
        ret = destBuf.textize(" 0x%02x", except_code_array_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SRP_THREAD_EXP::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SRP_THREAD_EXP::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SRP_THREAD_EXP::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType STP_CUSTOM_STRING::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_CUSTOM_STRING::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(total_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        ret = destBuf.writeUInt8(send_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_CUSTOM_STRING::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType STP_CUSTOM_STRING::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(total_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < send_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < send_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        send_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_CUSTOM_STRING::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType STP_CUSTOM_STRING::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[total_len_]", "%d", total_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[send_data_]", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (send_data_len_ != 0) {
        for (uint32_t i = 0; i < send_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", send_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType STP_CUSTOM_STRING::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* STP_CUSTOM_STRING::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* STP_CUSTOM_STRING::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SAB_CS_SLN_DATA::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    ret = se_data_.construct();
    break;
    case 1:
    ret = se_error_info_.construct();
    break;
    case 19:
    ret = shell_module_verify_.construct();
    break;
    case 20:
    ret = sanlix_scan_result_.construct();
    break;
    case 100:
    ret = tp_error_info_.construct();
    break;
    case 101:
    ret = tp_custom_string_.construct();
    break;
    case 106:
    ret = tp_bin_log_report_.construct();
    break;
    case 107:
    ret = client_guid_info_.construct();
    break;
    case 108:
    ret = tp_kr_report_info_.construct();
    break;
    case 109:
    ret = tp_apc_report_info_.construct();
    break;
    case 201:
    ret = ab_auth_code_.construct();
    break;
    case 202:
    ret = ab_mac_info_.construct();
    break;
    case 203:
    ret = ab_sign_info_.construct();
    break;
    case 204:
    ret = ab_exp_data_.construct();
    break;
    case 302:
    ret = rp_msg_result_.construct();
    break;
    case 304:
    ret = rp_thread_exp_.construct();
    break;
    case 305:
    ret = rp_func_exp_.construct();
    break;
    case 306:
    ret = rp_rpcode_hash_.construct();
    break;
    case 601:
    ret = tvmp_report_call_info_.construct();
    break;
    case 602:
    ret = tvmp_report_hash_info_.construct();
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SAB_CS_SLN_DATA::pack(int64_t union_selector, TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    ret = se_data_.pack(destBuf, 1);
    break;
    case 1:
    ret = se_error_info_.pack(destBuf, 1);
    break;
    case 19:
    ret = shell_module_verify_.pack(destBuf, 1);
    break;
    case 20:
    ret = sanlix_scan_result_.pack(destBuf, 1);
    break;
    case 100:
    ret = tp_error_info_.pack(destBuf, 1);
    break;
    case 101:
    ret = tp_custom_string_.pack(destBuf, 1);
    break;
    case 106:
    ret = tp_bin_log_report_.pack(destBuf, 1);
    break;
    case 107:
    ret = client_guid_info_.pack(destBuf, 1);
    break;
    case 108:
    ret = tp_kr_report_info_.pack(destBuf, 1);
    break;
    case 109:
    ret = tp_apc_report_info_.pack(destBuf, 1);
    break;
    case 201:
    ret = ab_auth_code_.pack(destBuf, 1);
    break;
    case 202:
    ret = ab_mac_info_.pack(destBuf, 1);
    break;
    case 203:
    ret = ab_sign_info_.pack(destBuf, 1);
    break;
    case 204:
    ret = ab_exp_data_.pack(destBuf, 1);
    break;
    case 302:
    ret = rp_msg_result_.pack(destBuf, 1);
    break;
    case 304:
    ret = rp_thread_exp_.pack(destBuf, 1);
    break;
    case 305:
    ret = rp_func_exp_.pack(destBuf, 1);
    break;
    case 306:
    ret = rp_rpcode_hash_.pack(destBuf, 1);
    break;
    case 601:
    ret = tvmp_report_call_info_.pack(destBuf, 1);
    break;
    case 602:
    ret = tvmp_report_hash_info_.pack(destBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SAB_CS_SLN_DATA::pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SAB_CS_SLN_DATA::unpack(int64_t union_selector, TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    ret = se_data_.unpack(srcBuf, 1);
    break;
    case 1:
    ret = se_error_info_.unpack(srcBuf, 1);
    break;
    case 19:
    ret = shell_module_verify_.unpack(srcBuf, 1);
    break;
    case 20:
    ret = sanlix_scan_result_.unpack(srcBuf, 1);
    break;
    case 100:
    ret = tp_error_info_.unpack(srcBuf, 1);
    break;
    case 101:
    ret = tp_custom_string_.unpack(srcBuf, 1);
    break;
    case 106:
    ret = tp_bin_log_report_.unpack(srcBuf, 1);
    break;
    case 107:
    ret = client_guid_info_.unpack(srcBuf, 1);
    break;
    case 108:
    ret = tp_kr_report_info_.unpack(srcBuf, 1);
    break;
    case 109:
    ret = tp_apc_report_info_.unpack(srcBuf, 1);
    break;
    case 201:
    ret = ab_auth_code_.unpack(srcBuf, 1);
    break;
    case 202:
    ret = ab_mac_info_.unpack(srcBuf, 1);
    break;
    case 203:
    ret = ab_sign_info_.unpack(srcBuf, 1);
    break;
    case 204:
    ret = ab_exp_data_.unpack(srcBuf, 1);
    break;
    case 302:
    ret = rp_msg_result_.unpack(srcBuf, 1);
    break;
    case 304:
    ret = rp_thread_exp_.unpack(srcBuf, 1);
    break;
    case 305:
    ret = rp_func_exp_.unpack(srcBuf, 1);
    break;
    case 306:
    ret = rp_rpcode_hash_.unpack(srcBuf, 1);
    break;
    case 601:
    ret = tvmp_report_call_info_.unpack(srcBuf, 1);
    break;
    case 602:
    ret = tvmp_report_hash_info_.unpack(srcBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SAB_CS_SLN_DATA::unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SAB_CS_SLN_DATA::visualize(int64_t union_selector, TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[se_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = se_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = se_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 1:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[se_error_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = se_error_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = se_error_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 19:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[shell_module_verify_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = shell_module_verify_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = shell_module_verify_.visualize(destBuf, indent, separator);
    }
    break;
    case 20:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sanlix_scan_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sanlix_scan_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sanlix_scan_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 100:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_error_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tp_error_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tp_error_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 101:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_custom_string_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tp_custom_string_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tp_custom_string_.visualize(destBuf, indent, separator);
    }
    break;
    case 106:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_bin_log_report_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tp_bin_log_report_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tp_bin_log_report_.visualize(destBuf, indent, separator);
    }
    break;
    case 107:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_guid_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = client_guid_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = client_guid_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 108:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_kr_report_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tp_kr_report_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tp_kr_report_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 109:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_apc_report_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tp_apc_report_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tp_apc_report_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 201:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ab_auth_code_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = ab_auth_code_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = ab_auth_code_.visualize(destBuf, indent, separator);
    }
    break;
    case 202:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ab_mac_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = ab_mac_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = ab_mac_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 203:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ab_sign_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = ab_sign_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = ab_sign_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 204:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ab_exp_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = ab_exp_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = ab_exp_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 302:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_msg_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = rp_msg_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = rp_msg_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 304:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_thread_exp_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = rp_thread_exp_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = rp_thread_exp_.visualize(destBuf, indent, separator);
    }
    break;
    case 305:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_func_exp_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = rp_func_exp_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = rp_func_exp_.visualize(destBuf, indent, separator);
    }
    break;
    case 306:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_rpcode_hash_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = rp_rpcode_hash_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = rp_rpcode_hash_.visualize(destBuf, indent, separator);
    }
    break;
    case 601:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tvmp_report_call_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tvmp_report_call_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tvmp_report_call_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 602:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tvmp_report_hash_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tvmp_report_hash_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tvmp_report_hash_info_.visualize(destBuf, indent, separator);
    }
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SAB_CS_SLN_DATA::visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(scheme_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(sub_protocol_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = data_.pack((int64_t)sub_protocol_id_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(scheme_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(sub_protocol_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = data_.unpack((int64_t)sub_protocol_id_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[scheme_id_]", "%d", scheme_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sub_protocol_id_]", "%d", sub_protocol_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = data_.visualize((int64_t)sub_protocol_id_, destBuf, indent + 1, separator);
    } else {
        ret = data_.visualize((int64_t)sub_protocol_id_, destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SCHEMEDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_SCHEMEDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_SCHEMEDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TORDINAL::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TORDINAL::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(ordinal_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TORDINAL::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TORDINAL::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(ordinal_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TORDINAL::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TORDINAL::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[count_]", "0x%02x", count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ordinal_]", "%u", ordinal_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TORDINAL::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TORDINAL::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TORDINAL::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TORDINAL_OR_NAME::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = name_.construct();
    break;
    case 1:
    ret = ordinal_.construct();
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TORDINAL_OR_NAME::pack(int64_t union_selector, TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = name_.pack(destBuf, 1);
    break;
    case 1:
    ret = ordinal_.pack(destBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TORDINAL_OR_NAME::pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TORDINAL_OR_NAME::unpack(int64_t union_selector, TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = name_.unpack(srcBuf, 1);
    break;
    case 1:
    ret = ordinal_.unpack(srcBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TORDINAL_OR_NAME::unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TORDINAL_OR_NAME::visualize(int64_t union_selector, TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 0:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[name_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = name_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = name_.visualize(destBuf, indent, separator);
    }
    break;
    case 1:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ordinal_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = ordinal_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = ordinal_.visualize(destBuf, indent, separator);
    }
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType TORDINAL_OR_NAME::visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = name_or_ordinal_.pack((int64_t)id_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(mem_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(cur_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(total_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 100) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 100; ++i) {
        ret = destBuf.writeUInt8(mem_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = name_or_ordinal_.unpack((int64_t)id_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(mem_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(cur_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(total_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 100) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 100; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        mem_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[id_]", "0x%02x", id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[name_or_ordinal_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = name_or_ordinal_.visualize((int64_t)id_, destBuf, indent + 1, separator);
    } else {
        ret = name_or_ordinal_.visualize((int64_t)id_, destBuf, indent, separator);
    }
    return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mem_type_]", "0x%02x", mem_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cur_times_]", "%u", cur_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[total_times_]", "%u", total_times_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[offset_]", "%u", offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "%u", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[mem_]", 100);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 100; ++i) {
        ret = destBuf.textize(" 0x%02x", mem_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER_ITEM::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TMEM_RECOVER_ITEM::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TMEM_RECOVER_ITEM::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TMEM_RECOVER::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = mem_item_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TMEM_RECOVER::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = mem_item_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TMEM_RECOVER::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[count_]", "%d", count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 10) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return ret;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mem_item_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = mem_item_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = mem_item_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TMEM_RECOVER::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TMEM_RECOVER::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TMEM_RECOVER::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_MEM_RECOVER::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_MEM_RECOVER::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = mem_info_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_MEM_RECOVER::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_MEM_RECOVER::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = mem_info_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_MEM_RECOVER::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_MEM_RECOVER::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[buf_len_]", "%d", buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mem_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = mem_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = mem_info_.visualize(destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_MEM_RECOVER::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_MEM_RECOVER::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_MEM_RECOVER::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_TESTPKG::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TESTPKG::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < content_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < content_size_; ++i) {
        ret = destBuf.writeUInt8(content_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TESTPKG::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_TESTPKG::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < content_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < content_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        content_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TESTPKG::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_TESTPKG::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[test_pkg_crc_]", "%u", test_pkg_crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[content_size_]", "0x%02x", content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[content_]", content_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (content_size_ != 0) {
        for (uint32_t i = 0; i < content_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", content_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_TESTPKG::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_TESTPKG::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_TESTPKG::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_RANDCHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RANDCHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(rand_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RANDCHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RANDCHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        rand_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RANDCHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RANDCHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[rand_data_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", rand_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[type_]", "0x%02x", type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RANDCHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_RANDCHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_RANDCHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_NUMINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_NUMINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < num_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < num_info_size_; ++i) {
        ret = destBuf.writeUInt8(num_info_msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_NUMINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_NUMINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < num_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < num_info_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        num_info_msg_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_NUMINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_NUMINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[num_info_size_]", "%d", num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[num_info_msg_]", num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_info_size_ != 0) {
        for (uint32_t i = 0; i < num_info_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", num_info_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_NUMINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_NUMINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_NUMINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_P2PSTATINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_P2PSTATINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(end_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_P2PSTATINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_P2PSTATINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp4crc_;
    ret = srcBuf.readUInt32(tmp4crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    crc_ = (int32_t)tmp4crc_;
    ret = srcBuf.readUInt16(serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(end_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_P2PSTATINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_P2PSTATINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc_]", "%d", crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[serial_]", "%d", serial_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[end_flag_]", "0x%02x", end_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[size_]", "%d", size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_P2PSTATINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_P2PSTATINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_P2PSTATINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_TP_INFO_MSG::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TP_INFO_MSG::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (destBuf.capacity() - destBuf.size() < 260) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 260; ++i) {
        ret = destBuf.writeUInt8(msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TP_INFO_MSG::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_TP_INFO_MSG::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (srcBuf.capacity() - srcBuf.size() < 260) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 260; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        msg_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TP_INFO_MSG::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_TP_INFO_MSG::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[msg_]", 260);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 260; ++i) {
        ret = destBuf.textize(" 0x%02x", msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TP_INFO_MSG::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_TP_INFO_MSG::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_TP_INFO_MSG::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_TPINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TPINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(tp_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tp_info_msg_size_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < tp_info_msg_size_; ++i) {
        ret = tp_info_msg_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TPINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_TPINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(tp_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tp_info_msg_size_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    for (uint32_t i = 0; i < tp_info_msg_size_; ++i) {
        ret = tp_info_msg_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TPINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_TPINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_info_msg_size_]", "%d", tp_info_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (tp_info_msg_size_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (tp_info_msg_size_ == 0) return ret;
    for (uint32_t i = 0; i < tp_info_msg_size_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_info_msg_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = tp_info_msg_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = tp_info_msg_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_TPINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_TPINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_TPINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(cfg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < cmd_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < cmd_size_; ++i) {
        ret = destBuf.writeUInt8(cmd_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(cfg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < cmd_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < cmd_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        cmd_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cfg_id_]", "0x%02x", cfg_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_size_]", "0x%02x", cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_size_ > 128) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[cmd_data_]", cmd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (cmd_size_ != 0) {
        for (uint32_t i = 0; i < cmd_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", cmd_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA_ITEM::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SSC_SE_SCAN_DATA_ITEM::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SSC_SE_SCAN_DATA_ITEM::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SSC_SE_SCAN_DATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(temp_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = se_scan_data_item_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SSC_SE_SCAN_DATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(temp_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = se_scan_data_item_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SSC_SE_SCAN_DATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[temp_data_len_]", "%d", temp_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[se_scan_data_item_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = se_scan_data_item_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = se_scan_data_item_.visualize(destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SE_SCAN_DATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SSC_SE_SCAN_DATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SSC_SE_SCAN_DATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SanlixScanJudgeResult::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanJudgeResult::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(sanlix_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(duration_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanJudgeResult::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SanlixScanJudgeResult::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(sanlix_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(duration_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanJudgeResult::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SanlixScanJudgeResult::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sanlix_id_]", "%u", sanlix_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%u", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[duration_]", "%u", duration_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixScanJudgeResult::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SanlixScanJudgeResult::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SanlixScanJudgeResult::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SSC_SLN_DATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SLN_DATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 32727) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 32727; ++i) {
        ret = destBuf.writeUInt8(data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SLN_DATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SSC_SLN_DATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 32727) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 32727; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SLN_DATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SSC_SLN_DATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%d", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_]", 32727);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 32727; ++i) {
        ret = destBuf.textize(" 0x%02x", data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SSC_SLN_DATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SSC_SLN_DATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SSC_SLN_DATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SC_SCHEMEDATA::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 3:
    ret = se_scan_data_.construct();
    break;
    case 21:
    ret = sx_judge_result_.construct();
    break;
    case 105:
    ret = tp_svr_config_.construct();
    break;
    case 308:
    ret = rp_cp_data_.construct();
    break;
    case 402:
    ret = sx_cfg_info_.construct();
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SC_SCHEMEDATA::pack(int64_t union_selector, TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 3:
    ret = se_scan_data_.pack(destBuf, 1);
    break;
    case 21:
    ret = sx_judge_result_.pack(destBuf, 1);
    break;
    case 105:
    ret = tp_svr_config_.pack(destBuf, 1);
    break;
    case 308:
    ret = rp_cp_data_.pack(destBuf, 1);
    break;
    case 402:
    ret = sx_cfg_info_.pack(destBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SC_SCHEMEDATA::pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SC_SCHEMEDATA::unpack(int64_t union_selector, TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 3:
    ret = se_scan_data_.unpack(srcBuf, 1);
    break;
    case 21:
    ret = sx_judge_result_.unpack(srcBuf, 1);
    break;
    case 105:
    ret = tp_svr_config_.unpack(srcBuf, 1);
    break;
    case 308:
    ret = rp_cp_data_.unpack(srcBuf, 1);
    break;
    case 402:
    ret = sx_cfg_info_.unpack(srcBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SC_SCHEMEDATA::unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SC_SCHEMEDATA::visualize(int64_t union_selector, TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 3:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[se_scan_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = se_scan_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = se_scan_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 21:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sx_judge_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sx_judge_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sx_judge_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 105:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[tp_svr_config_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = tp_svr_config_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = tp_svr_config_.visualize(destBuf, indent, separator);
    }
    break;
    case 308:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[rp_cp_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = rp_cp_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = rp_cp_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 402:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sx_cfg_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sx_cfg_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sx_cfg_info_.visualize(destBuf, indent, separator);
    }
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType SC_SCHEMEDATA::visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(scheme_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(scheme_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = scheme_data_.pack((int64_t)scheme_data_len_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(scheme_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(scheme_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = scheme_data_.unpack((int64_t)scheme_data_len_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[scheme_id_]", "%d", scheme_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[scheme_data_len_]", "%d", scheme_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = scheme_data_.visualize((int64_t)scheme_data_len_, destBuf, indent + 1, separator);
    } else {
        ret = scheme_data_.visualize((int64_t)scheme_data_len_, destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SCHEMEDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_SCHEMEDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_SCHEMEDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_ERRPACKET::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ERRPACKET::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(snd_proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(snd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(err_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < err_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < err_info_size_; ++i) {
        ret = destBuf.writeUInt8(err_info_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ERRPACKET::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ERRPACKET::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(snd_proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(snd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(err_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < err_info_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < err_info_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        err_info_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ERRPACKET::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_ERRPACKET::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[snd_proto_id_]", "%d", snd_proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[snd_size_]", "%u", snd_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[err_info_size_]", "0x%02x", err_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_info_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[err_info_]", err_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (err_info_size_ != 0) {
        for (uint32_t i = 0; i < err_info_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", err_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_ERRPACKET::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_ERRPACKET::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_ERRPACKET::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_SLXSHAREINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SLXSHAREINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(slx_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (slx_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < slx_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < slx_data_len_; ++i) {
        ret = destBuf.writeUInt8(slx_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SLXSHAREINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SLXSHAREINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(slx_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (slx_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < slx_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < slx_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        slx_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SLXSHAREINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SLXSHAREINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[slx_data_len_]", "%d", slx_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (slx_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[slx_data_]", slx_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (slx_data_len_ != 0) {
        for (uint32_t i = 0; i < slx_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", slx_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SLXSHAREINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_SLXSHAREINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_SLXSHAREINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_RESENDDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESENDDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(data_proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(data_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESENDDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RESENDDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(data_proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(data_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESENDDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_RESENDDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_proto_id_]", "%d", data_proto_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_type_]", "0x%02x", data_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_RESENDDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_RESENDDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_RESENDDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_DELAY_CHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELAY_CHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt64(secsvr_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt64(sdk_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt64(sdk_up_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_up_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELAY_CHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DELAY_CHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt64(secsvr_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt64(sdk_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt64(sdk_up_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_up_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELAY_CHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_DELAY_CHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[secsvr_down_time_]", "%llu", secsvr_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sdk_down_time_]", "%llu", sdk_down_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sdk_up_time_]", "%llu", sdk_up_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_up_time_]", "%u", client_up_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_DELAY_CHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_DELAY_CHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_DELAY_CHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_SYSMEM::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEM::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(system_error_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_error_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < system_error_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < system_error_len_; ++i) {
        ret = destBuf.writeUInt8(system_error_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        ret = destBuf.writeUInt8(pad_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEM::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SYSMEM::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(system_error_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_error_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < system_error_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < system_error_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        system_error_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < pad_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < pad_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        pad_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEM::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_SYSMEM::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[system_error_len_]", "%d", system_error_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_error_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[system_error_data_]", system_error_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (system_error_len_ != 0) {
        for (uint32_t i = 0; i < system_error_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", system_error_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[pad_len_]", "0x%02x", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[pad_data_]", pad_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (pad_len_ != 0) {
        for (uint32_t i = 0; i < pad_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", pad_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_SYSMEM::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_SYSMEM::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_SYSMEM::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TQD_SC_CFGDATA::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CFGDATA::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(scan_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(prompt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(action_on_memory_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(action_on_file_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(scan_sub_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CFGDATA::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TQD_SC_CFGDATA::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(scan_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(prompt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(action_on_memory_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(action_on_file_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(scan_sub_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CFGDATA::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TQD_SC_CFGDATA::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[crc_]", "%u", crc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[len_]", "%d", len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[scan_type_]", "0x%02x", scan_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[prompt_]", "0x%02x", prompt_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[action_on_memory_]", "0x%02x", action_on_memory_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[action_on_file_]", "0x%02x", action_on_file_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[scan_sub_]", "0x%02x", scan_sub_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TQD_SC_CFGDATA::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TQD_SC_CFGDATA::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TQD_SC_CFGDATA::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_ANTICFGINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ANTICFGINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(anti_scan_gap_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ANTICFGINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_ANTICFGINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(anti_scan_gap_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ANTICFGINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_ANTICFGINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[anti_scan_gap_]", "%u", anti_scan_gap_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_ANTICFGINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_ANTICFGINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_ANTICFGINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_LOCALCHECK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LOCALCHECK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(local_check_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (local_check_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < local_check_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < local_check_data_len_; ++i) {
        ret = destBuf.writeUInt8(local_check_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(num_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < num_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < num_msg_size_; ++i) {
        ret = destBuf.writeUInt8(num_msg_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LOCALCHECK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_LOCALCHECK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(local_check_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (local_check_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < local_check_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < local_check_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        local_check_data_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(num_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < num_msg_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < num_msg_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        num_msg_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LOCALCHECK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_LOCALCHECK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[local_check_data_len_]", "%d", local_check_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (local_check_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[local_check_data_]", local_check_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (local_check_data_len_ != 0) {
        for (uint32_t i = 0; i < local_check_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", local_check_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[num_msg_size_]", "0x%02x", num_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_msg_size_ > 256) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[num_msg_]", num_msg_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (num_msg_size_ != 0) {
        for (uint32_t i = 0; i < num_msg_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", num_msg_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_LOCALCHECK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_LOCALCHECK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_LOCALCHECK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_CS_EXTINFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_EXTINFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(ext_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (ext_info_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < ext_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < ext_info_len_; ++i) {
        ret = destBuf.writeUInt8(ext_info_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_EXTINFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_EXTINFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(ext_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (ext_info_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < ext_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < ext_info_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ext_info_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_EXTINFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_CS_EXTINFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ext_info_len_]", "%d", ext_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (ext_info_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[ext_info_]", ext_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (ext_info_len_ != 0) {
        for (uint32_t i = 0; i < ext_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", ext_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_CS_EXTINFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_CS_EXTINFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_CS_EXTINFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType TAB_SC_SUSMODQUEST::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SUSMODQUEST::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(sus_addr_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_addr_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < sus_addr_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < sus_addr_data_len_; ++i) {
        ret = destBuf.writeUInt8(sus_addr_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SUSMODQUEST::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SUSMODQUEST::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(sus_addr_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_addr_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < sus_addr_data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < sus_addr_data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        sus_addr_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SUSMODQUEST::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType TAB_SC_SUSMODQUEST::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sus_addr_data_len_]", "%d", sus_addr_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_addr_data_len_ > 32727) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[sus_addr_data_]", sus_addr_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (sus_addr_data_len_ != 0) {
        for (uint32_t i = 0; i < sus_addr_data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", sus_addr_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TAB_SC_SUSMODQUEST::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* TAB_SC_SUSMODQUEST::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* TAB_SC_SUSMODQUEST::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType AntibotPkgBody::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 1:
    ret = sc_send_antibot_.construct();
    break;
    case 2:
    ret = sc_rand_check_.construct();
    break;
    case 3:
    ret = cs_init_info_.construct();
    break;
    case 4:
    ret = cs_verify_result_.construct();
    break;
    case 5:
    ret = sc_stop_l_check_.construct();
    break;
    case 6:
    ret = cs_resend_data_.construct();
    break;
    case 8:
    ret = sc_resend_data_.construct();
    break;
    case 10:
    ret = cs_p2p_stat_info_.construct();
    break;
    case 11:
    ret = sc_cfg_data_.construct();
    break;
    case 12:
    ret = sc_req_cli_resend_data_.construct();
    break;
    case 13:
    ret = cs_client_info_.construct();
    break;
    case 15:
    ret = cs_tp_info_.construct();
    break;
    case 16:
    ret = sc_qd_cfg_data_.construct();
    break;
    case 17:
    ret = sc_qd_cmd_data_.construct();
    break;
    case 18:
    ret = cs_qd_scan_result_.construct();
    break;
    case 19:
    ret = cs_qd_init_data_.construct();
    break;
    case 20:
    ret = cs_sanlix_report_.construct();
    break;
    case 21:
    ret = sc_tenparty_data_.construct();
    break;
    case 22:
    ret = sc_file_hash_.construct();
    break;
    case 23:
    ret = cs_cli_net_info_.construct();
    break;
    case 24:
    ret = sc_mem_recover_.construct();
    break;
    case 25:
    ret = cs_ext_info_.construct();
    break;
    case 26:
    ret = cs_local_check_.construct();
    break;
    case 27:
    ret = cs_num_info_.construct();
    break;
    case 28:
    ret = sc_de_cmd_id_list_.construct();
    break;
    case 29:
    ret = sc_de_dib_data_.construct();
    break;
    case 30:
    ret = sc_de_key_.construct();
    break;
    case 31:
    ret = cs_de_ready_.construct();
    break;
    case 32:
    ret = cs_de_test_pkg_.construct();
    break;
    case 33:
    ret = sc_test_pkg_.construct();
    break;
    case 34:
    ret = cs_de_log_.construct();
    break;
    case 37:
    ret = sc_encrypt_.construct();
    break;
    case 38:
    ret = cs_encrypt_.construct();
    break;
    case 39:
    ret = cs_err_packet_.construct();
    break;
    case 40:
    ret = cs_sys_mem_.construct();
    break;
    case 41:
    ret = cs_sys_mem_log_.construct();
    break;
    case 42:
    ret = sc_restrict_uin_.construct();
    break;
    case 43:
    ret = cs_iat_check_.construct();
    break;
    case 44:
    ret = sc_cs_cfg_data_.construct();
    break;
    case 45:
    ret = cs_cs_check_.construct();
    break;
    case 46:
    ret = sc_susmod_quest_.construct();
    break;
    case 47:
    ret = cs_sus_mod_info_.construct();
    break;
    case 48:
    ret = cs_tv_info_.construct();
    break;
    case 49:
    ret = sc_tv_check_.construct();
    break;
    case 50:
    ret = sc_slx_share_info_.construct();
    break;
    case 51:
    ret = sc_scheme_data_.construct();
    break;
    case 52:
    ret = cs_scheme_data_.construct();
    break;
    case 53:
    ret = sc_slx_te_share_info_.construct();
    break;
    case 54:
    ret = sc_rpfmc_scan_cmd_.construct();
    break;
    case 55:
    ret = cs_rp_fmc_scan_result_.construct();
    break;
    case 56:
    ret = sc_la_pkg_cmd_.construct();
    break;
    case 57:
    ret = cs_la_pkg_result_.construct();
    break;
    case 58:
    ret = sc_fms_cmd_.construct();
    break;
    case 59:
    ret = cs_fmc_result_.construct();
    break;
    case 60:
    ret = sc_anti_cfg_info_.construct();
    break;
    case 61:
    ret = cs_anti_time_.construct();
    break;
    case 62:
    ret = cs_anti_clt_key_.construct();
    break;
    case 64:
    ret = cs_captcha_result_.construct();
    break;
    case 65:
    ret = cs_captcha_req_.construct();
    break;
    case 66:
    ret = cs_report_client_guid_.construct();
    break;
    case 70:
    ret = cs_report_error_.construct();
    break;
    case 100:
    ret = sc_pkg_code_.construct();
    break;
    case 101:
    ret = cs_rp_pkg_result_.construct();
    break;
    case 102:
    ret = sc_pkg_code_key_.construct();
    break;
    case 104:
    ret = sc_channel_server_id_.construct();
    break;
    case 105:
    ret = cs_cschannel_result_.construct();
    break;
    case 120:
    ret = sc_channel_delay_check_.construct();
    break;
    case 121:
    ret = cs_channel_delay_check_.construct();
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType AntibotPkgBody::pack(int64_t union_selector, TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 1:
    ret = sc_send_antibot_.pack(destBuf, 1);
    break;
    case 2:
    ret = sc_rand_check_.pack(destBuf, 1);
    break;
    case 3:
    ret = cs_init_info_.pack(destBuf, 1);
    break;
    case 4:
    ret = cs_verify_result_.pack(destBuf, 1);
    break;
    case 5:
    ret = sc_stop_l_check_.pack(destBuf, 1);
    break;
    case 6:
    ret = cs_resend_data_.pack(destBuf, 1);
    break;
    case 8:
    ret = sc_resend_data_.pack(destBuf, 1);
    break;
    case 10:
    ret = cs_p2p_stat_info_.pack(destBuf, 1);
    break;
    case 11:
    ret = sc_cfg_data_.pack(destBuf, 1);
    break;
    case 12:
    ret = sc_req_cli_resend_data_.pack(destBuf, 1);
    break;
    case 13:
    ret = cs_client_info_.pack(destBuf, 1);
    break;
    case 15:
    ret = cs_tp_info_.pack(destBuf, 1);
    break;
    case 16:
    ret = sc_qd_cfg_data_.pack(destBuf, 1);
    break;
    case 17:
    ret = sc_qd_cmd_data_.pack(destBuf, 1);
    break;
    case 18:
    ret = cs_qd_scan_result_.pack(destBuf, 1);
    break;
    case 19:
    ret = cs_qd_init_data_.pack(destBuf, 1);
    break;
    case 20:
    ret = cs_sanlix_report_.pack(destBuf, 1);
    break;
    case 21:
    ret = sc_tenparty_data_.pack(destBuf, 1);
    break;
    case 22:
    ret = sc_file_hash_.pack(destBuf, 1);
    break;
    case 23:
    ret = cs_cli_net_info_.pack(destBuf, 1);
    break;
    case 24:
    ret = sc_mem_recover_.pack(destBuf, 1);
    break;
    case 25:
    ret = cs_ext_info_.pack(destBuf, 1);
    break;
    case 26:
    ret = cs_local_check_.pack(destBuf, 1);
    break;
    case 27:
    ret = cs_num_info_.pack(destBuf, 1);
    break;
    case 28:
    ret = sc_de_cmd_id_list_.pack(destBuf, 1);
    break;
    case 29:
    ret = sc_de_dib_data_.pack(destBuf, 1);
    break;
    case 30:
    ret = sc_de_key_.pack(destBuf, 1);
    break;
    case 31:
    ret = cs_de_ready_.pack(destBuf, 1);
    break;
    case 32:
    ret = cs_de_test_pkg_.pack(destBuf, 1);
    break;
    case 33:
    ret = sc_test_pkg_.pack(destBuf, 1);
    break;
    case 34:
    ret = cs_de_log_.pack(destBuf, 1);
    break;
    case 37:
    ret = sc_encrypt_.pack(destBuf, 1);
    break;
    case 38:
    ret = cs_encrypt_.pack(destBuf, 1);
    break;
    case 39:
    ret = cs_err_packet_.pack(destBuf, 1);
    break;
    case 40:
    ret = cs_sys_mem_.pack(destBuf, 1);
    break;
    case 41:
    ret = cs_sys_mem_log_.pack(destBuf, 1);
    break;
    case 42:
    ret = sc_restrict_uin_.pack(destBuf, 1);
    break;
    case 43:
    ret = cs_iat_check_.pack(destBuf, 1);
    break;
    case 44:
    ret = sc_cs_cfg_data_.pack(destBuf, 1);
    break;
    case 45:
    ret = cs_cs_check_.pack(destBuf, 1);
    break;
    case 46:
    ret = sc_susmod_quest_.pack(destBuf, 1);
    break;
    case 47:
    ret = cs_sus_mod_info_.pack(destBuf, 1);
    break;
    case 48:
    ret = cs_tv_info_.pack(destBuf, 1);
    break;
    case 49:
    ret = sc_tv_check_.pack(destBuf, 1);
    break;
    case 50:
    ret = sc_slx_share_info_.pack(destBuf, 1);
    break;
    case 51:
    ret = sc_scheme_data_.pack(destBuf, 1);
    break;
    case 52:
    ret = cs_scheme_data_.pack(destBuf, 1);
    break;
    case 53:
    ret = sc_slx_te_share_info_.pack(destBuf, 1);
    break;
    case 54:
    ret = sc_rpfmc_scan_cmd_.pack(destBuf, 1);
    break;
    case 55:
    ret = cs_rp_fmc_scan_result_.pack(destBuf, 1);
    break;
    case 56:
    ret = sc_la_pkg_cmd_.pack(destBuf, 1);
    break;
    case 57:
    ret = cs_la_pkg_result_.pack(destBuf, 1);
    break;
    case 58:
    ret = sc_fms_cmd_.pack(destBuf, 1);
    break;
    case 59:
    ret = cs_fmc_result_.pack(destBuf, 1);
    break;
    case 60:
    ret = sc_anti_cfg_info_.pack(destBuf, 1);
    break;
    case 61:
    ret = cs_anti_time_.pack(destBuf, 1);
    break;
    case 62:
    ret = cs_anti_clt_key_.pack(destBuf, 1);
    break;
    case 64:
    ret = cs_captcha_result_.pack(destBuf, 1);
    break;
    case 65:
    ret = cs_captcha_req_.pack(destBuf, 1);
    break;
    case 66:
    ret = cs_report_client_guid_.pack(destBuf, 1);
    break;
    case 70:
    ret = cs_report_error_.pack(destBuf, 1);
    break;
    case 100:
    ret = sc_pkg_code_.pack(destBuf, 1);
    break;
    case 101:
    ret = cs_rp_pkg_result_.pack(destBuf, 1);
    break;
    case 102:
    ret = sc_pkg_code_key_.pack(destBuf, 1);
    break;
    case 104:
    ret = sc_channel_server_id_.pack(destBuf, 1);
    break;
    case 105:
    ret = cs_cschannel_result_.pack(destBuf, 1);
    break;
    case 120:
    ret = sc_channel_delay_check_.pack(destBuf, 1);
    break;
    case 121:
    ret = cs_channel_delay_check_.pack(destBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType AntibotPkgBody::pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType AntibotPkgBody::unpack(int64_t union_selector, TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 1:
    ret = sc_send_antibot_.unpack(srcBuf, 1);
    break;
    case 2:
    ret = sc_rand_check_.unpack(srcBuf, 1);
    break;
    case 3:
    ret = cs_init_info_.unpack(srcBuf, 1);
    break;
    case 4:
    ret = cs_verify_result_.unpack(srcBuf, 1);
    break;
    case 5:
    ret = sc_stop_l_check_.unpack(srcBuf, 1);
    break;
    case 6:
    ret = cs_resend_data_.unpack(srcBuf, 1);
    break;
    case 8:
    ret = sc_resend_data_.unpack(srcBuf, 1);
    break;
    case 10:
    ret = cs_p2p_stat_info_.unpack(srcBuf, 1);
    break;
    case 11:
    ret = sc_cfg_data_.unpack(srcBuf, 1);
    break;
    case 12:
    ret = sc_req_cli_resend_data_.unpack(srcBuf, 1);
    break;
    case 13:
    ret = cs_client_info_.unpack(srcBuf, 1);
    break;
    case 15:
    ret = cs_tp_info_.unpack(srcBuf, 1);
    break;
    case 16:
    ret = sc_qd_cfg_data_.unpack(srcBuf, 1);
    break;
    case 17:
    ret = sc_qd_cmd_data_.unpack(srcBuf, 1);
    break;
    case 18:
    ret = cs_qd_scan_result_.unpack(srcBuf, 1);
    break;
    case 19:
    ret = cs_qd_init_data_.unpack(srcBuf, 1);
    break;
    case 20:
    ret = cs_sanlix_report_.unpack(srcBuf, 1);
    break;
    case 21:
    ret = sc_tenparty_data_.unpack(srcBuf, 1);
    break;
    case 22:
    ret = sc_file_hash_.unpack(srcBuf, 1);
    break;
    case 23:
    ret = cs_cli_net_info_.unpack(srcBuf, 1);
    break;
    case 24:
    ret = sc_mem_recover_.unpack(srcBuf, 1);
    break;
    case 25:
    ret = cs_ext_info_.unpack(srcBuf, 1);
    break;
    case 26:
    ret = cs_local_check_.unpack(srcBuf, 1);
    break;
    case 27:
    ret = cs_num_info_.unpack(srcBuf, 1);
    break;
    case 28:
    ret = sc_de_cmd_id_list_.unpack(srcBuf, 1);
    break;
    case 29:
    ret = sc_de_dib_data_.unpack(srcBuf, 1);
    break;
    case 30:
    ret = sc_de_key_.unpack(srcBuf, 1);
    break;
    case 31:
    ret = cs_de_ready_.unpack(srcBuf, 1);
    break;
    case 32:
    ret = cs_de_test_pkg_.unpack(srcBuf, 1);
    break;
    case 33:
    ret = sc_test_pkg_.unpack(srcBuf, 1);
    break;
    case 34:
    ret = cs_de_log_.unpack(srcBuf, 1);
    break;
    case 37:
    ret = sc_encrypt_.unpack(srcBuf, 1);
    break;
    case 38:
    ret = cs_encrypt_.unpack(srcBuf, 1);
    break;
    case 39:
    ret = cs_err_packet_.unpack(srcBuf, 1);
    break;
    case 40:
    ret = cs_sys_mem_.unpack(srcBuf, 1);
    break;
    case 41:
    ret = cs_sys_mem_log_.unpack(srcBuf, 1);
    break;
    case 42:
    ret = sc_restrict_uin_.unpack(srcBuf, 1);
    break;
    case 43:
    ret = cs_iat_check_.unpack(srcBuf, 1);
    break;
    case 44:
    ret = sc_cs_cfg_data_.unpack(srcBuf, 1);
    break;
    case 45:
    ret = cs_cs_check_.unpack(srcBuf, 1);
    break;
    case 46:
    ret = sc_susmod_quest_.unpack(srcBuf, 1);
    break;
    case 47:
    ret = cs_sus_mod_info_.unpack(srcBuf, 1);
    break;
    case 48:
    ret = cs_tv_info_.unpack(srcBuf, 1);
    break;
    case 49:
    ret = sc_tv_check_.unpack(srcBuf, 1);
    break;
    case 50:
    ret = sc_slx_share_info_.unpack(srcBuf, 1);
    break;
    case 51:
    ret = sc_scheme_data_.unpack(srcBuf, 1);
    break;
    case 52:
    ret = cs_scheme_data_.unpack(srcBuf, 1);
    break;
    case 53:
    ret = sc_slx_te_share_info_.unpack(srcBuf, 1);
    break;
    case 54:
    ret = sc_rpfmc_scan_cmd_.unpack(srcBuf, 1);
    break;
    case 55:
    ret = cs_rp_fmc_scan_result_.unpack(srcBuf, 1);
    break;
    case 56:
    ret = sc_la_pkg_cmd_.unpack(srcBuf, 1);
    break;
    case 57:
    ret = cs_la_pkg_result_.unpack(srcBuf, 1);
    break;
    case 58:
    ret = sc_fms_cmd_.unpack(srcBuf, 1);
    break;
    case 59:
    ret = cs_fmc_result_.unpack(srcBuf, 1);
    break;
    case 60:
    ret = sc_anti_cfg_info_.unpack(srcBuf, 1);
    break;
    case 61:
    ret = cs_anti_time_.unpack(srcBuf, 1);
    break;
    case 62:
    ret = cs_anti_clt_key_.unpack(srcBuf, 1);
    break;
    case 64:
    ret = cs_captcha_result_.unpack(srcBuf, 1);
    break;
    case 65:
    ret = cs_captcha_req_.unpack(srcBuf, 1);
    break;
    case 66:
    ret = cs_report_client_guid_.unpack(srcBuf, 1);
    break;
    case 70:
    ret = cs_report_error_.unpack(srcBuf, 1);
    break;
    case 100:
    ret = sc_pkg_code_.unpack(srcBuf, 1);
    break;
    case 101:
    ret = cs_rp_pkg_result_.unpack(srcBuf, 1);
    break;
    case 102:
    ret = sc_pkg_code_key_.unpack(srcBuf, 1);
    break;
    case 104:
    ret = sc_channel_server_id_.unpack(srcBuf, 1);
    break;
    case 105:
    ret = cs_cschannel_result_.unpack(srcBuf, 1);
    break;
    case 120:
    ret = sc_channel_delay_check_.unpack(srcBuf, 1);
    break;
    case 121:
    ret = cs_channel_delay_check_.unpack(srcBuf, 1);
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType AntibotPkgBody::unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType AntibotPkgBody::visualize(int64_t union_selector, TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
switch (union_selector) {
    case 1:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_send_antibot_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_send_antibot_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_send_antibot_.visualize(destBuf, indent, separator);
    }
    break;
    case 2:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_rand_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_rand_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_rand_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 3:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_init_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_init_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_init_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 4:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_verify_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_verify_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_verify_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 5:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_stop_l_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_stop_l_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_stop_l_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 6:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_resend_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_resend_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_resend_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 8:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_resend_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_resend_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_resend_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 10:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_p2p_stat_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_p2p_stat_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_p2p_stat_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 11:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_cfg_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_cfg_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_cfg_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 12:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_req_cli_resend_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_req_cli_resend_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_req_cli_resend_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 13:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_client_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_client_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_client_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 15:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_tp_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_tp_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_tp_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 16:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_qd_cfg_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_qd_cfg_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_qd_cfg_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 17:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_qd_cmd_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_qd_cmd_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_qd_cmd_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 18:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_qd_scan_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_qd_scan_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_qd_scan_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 19:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_qd_init_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_qd_init_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_qd_init_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 20:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_sanlix_report_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_sanlix_report_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_sanlix_report_.visualize(destBuf, indent, separator);
    }
    break;
    case 21:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_tenparty_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_tenparty_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_tenparty_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 22:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_file_hash_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_file_hash_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_file_hash_.visualize(destBuf, indent, separator);
    }
    break;
    case 23:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_cli_net_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_cli_net_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_cli_net_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 24:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_mem_recover_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_mem_recover_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_mem_recover_.visualize(destBuf, indent, separator);
    }
    break;
    case 25:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_ext_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_ext_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_ext_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 26:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_local_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_local_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_local_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 27:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_num_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_num_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_num_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 28:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_de_cmd_id_list_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_de_cmd_id_list_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_de_cmd_id_list_.visualize(destBuf, indent, separator);
    }
    break;
    case 29:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_de_dib_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_de_dib_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_de_dib_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 30:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_de_key_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_de_key_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_de_key_.visualize(destBuf, indent, separator);
    }
    break;
    case 31:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_de_ready_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_de_ready_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_de_ready_.visualize(destBuf, indent, separator);
    }
    break;
    case 32:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_de_test_pkg_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_de_test_pkg_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_de_test_pkg_.visualize(destBuf, indent, separator);
    }
    break;
    case 33:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_test_pkg_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_test_pkg_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_test_pkg_.visualize(destBuf, indent, separator);
    }
    break;
    case 34:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_de_log_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_de_log_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_de_log_.visualize(destBuf, indent, separator);
    }
    break;
    case 37:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_encrypt_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_encrypt_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_encrypt_.visualize(destBuf, indent, separator);
    }
    break;
    case 38:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_encrypt_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_encrypt_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_encrypt_.visualize(destBuf, indent, separator);
    }
    break;
    case 39:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_err_packet_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_err_packet_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_err_packet_.visualize(destBuf, indent, separator);
    }
    break;
    case 40:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_sys_mem_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_sys_mem_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_sys_mem_.visualize(destBuf, indent, separator);
    }
    break;
    case 41:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_sys_mem_log_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_sys_mem_log_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_sys_mem_log_.visualize(destBuf, indent, separator);
    }
    break;
    case 42:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_restrict_uin_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_restrict_uin_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_restrict_uin_.visualize(destBuf, indent, separator);
    }
    break;
    case 43:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_iat_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_iat_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_iat_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 44:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_cs_cfg_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_cs_cfg_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_cs_cfg_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 45:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_cs_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_cs_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_cs_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 46:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_susmod_quest_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_susmod_quest_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_susmod_quest_.visualize(destBuf, indent, separator);
    }
    break;
    case 47:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_sus_mod_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_sus_mod_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_sus_mod_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 48:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_tv_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_tv_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_tv_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 49:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_tv_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_tv_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_tv_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 50:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_slx_share_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_slx_share_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_slx_share_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 51:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_scheme_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_scheme_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_scheme_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 52:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_scheme_data_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_scheme_data_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_scheme_data_.visualize(destBuf, indent, separator);
    }
    break;
    case 53:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_slx_te_share_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_slx_te_share_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_slx_te_share_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 54:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_rpfmc_scan_cmd_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_rpfmc_scan_cmd_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_rpfmc_scan_cmd_.visualize(destBuf, indent, separator);
    }
    break;
    case 55:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_rp_fmc_scan_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_rp_fmc_scan_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_rp_fmc_scan_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 56:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_la_pkg_cmd_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_la_pkg_cmd_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_la_pkg_cmd_.visualize(destBuf, indent, separator);
    }
    break;
    case 57:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_la_pkg_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_la_pkg_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_la_pkg_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 58:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_fms_cmd_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_fms_cmd_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_fms_cmd_.visualize(destBuf, indent, separator);
    }
    break;
    case 59:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_fmc_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_fmc_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_fmc_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 60:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_anti_cfg_info_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_anti_cfg_info_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_anti_cfg_info_.visualize(destBuf, indent, separator);
    }
    break;
    case 61:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_anti_time_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_anti_time_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_anti_time_.visualize(destBuf, indent, separator);
    }
    break;
    case 62:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_anti_clt_key_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_anti_clt_key_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_anti_clt_key_.visualize(destBuf, indent, separator);
    }
    break;
    case 64:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_captcha_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_captcha_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_captcha_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 65:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_captcha_req_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_captcha_req_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_captcha_req_.visualize(destBuf, indent, separator);
    }
    break;
    case 66:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_report_client_guid_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_report_client_guid_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_report_client_guid_.visualize(destBuf, indent, separator);
    }
    break;
    case 70:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_report_error_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_report_error_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_report_error_.visualize(destBuf, indent, separator);
    }
    break;
    case 100:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_pkg_code_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_pkg_code_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_pkg_code_.visualize(destBuf, indent, separator);
    }
    break;
    case 101:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_rp_pkg_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_rp_pkg_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_rp_pkg_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 102:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_pkg_code_key_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_pkg_code_key_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_pkg_code_key_.visualize(destBuf, indent, separator);
    }
    break;
    case 104:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_channel_server_id_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_channel_server_id_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_channel_server_id_.visualize(destBuf, indent, separator);
    }
    break;
    case 105:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_cschannel_result_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_cschannel_result_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_cschannel_result_.visualize(destBuf, indent, separator);
    }
    break;
    case 120:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[sc_channel_delay_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = sc_channel_delay_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = sc_channel_delay_check_.visualize(destBuf, indent, separator);
    }
    break;
    case 121:
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cs_channel_delay_check_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = cs_channel_delay_check_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = cs_channel_delay_check_.visualize(destBuf, indent, separator);
    }
    break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

TdrError::ErrorType AntibotPkgBody::visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType AntibotPkgHead::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotPkgHead::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotPkgHead::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType AntibotPkgHead::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotPkgHead::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType AntibotPkgHead::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_id_]", "%d", cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[seq_]", "%u", seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotPkgHead::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* AntibotPkgHead::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* AntibotPkgHead::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType AntibotClientPkg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotClientPkg::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head_.pack(destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.pack((int64_t)head_.cmd_id_, destBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotClientPkg::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType AntibotClientPkg::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = head_.unpack(srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.unpack((int64_t)head_.cmd_id_, srcBuf, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotClientPkg::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType AntibotClientPkg::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[head_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = head_.visualize(destBuf, indent + 1, separator);
    } else {
        ret = head_.visualize(destBuf, indent, separator);
    }
    return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body_]", true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = body_.visualize((int64_t)head_.cmd_id_, destBuf, indent + 1, separator);
    } else {
        ret = body_.visualize((int64_t)head_.cmd_id_, destBuf, indent, separator);
    }
    return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AntibotClientPkg::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* AntibotClientPkg::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* AntibotClientPkg::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType PKGCODE_KEY_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PKGCODE_KEY_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_size_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < key_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint32_t i = 0; i < key_size_; ++i) {
        ret = destBuf.writeUInt8(key_data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PKGCODE_KEY_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType PKGCODE_KEY_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_size_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < key_size_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint32_t i = 0; i < key_size_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        key_data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PKGCODE_KEY_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType PKGCODE_KEY_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dib_id_]", "0x%02x", dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_size_]", "0x%02x", key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_size_ > 64) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[key_data_]", key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (key_size_ != 0) {
        for (uint32_t i = 0; i < key_size_; ++i) {
            ret = destBuf.textize(" 0x%02x", key_data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PKGCODE_KEY_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* PKGCODE_KEY_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* PKGCODE_KEY_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SCS_SX_KEY_INFO::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SX_KEY_INFO::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(content_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 256) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 256; ++i) {
        ret = destBuf.writeUInt8(content_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (destBuf.capacity() - destBuf.size() < 256) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 256; ++i) {
        ret = destBuf.writeUInt8(content2_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SX_KEY_INFO::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SCS_SX_KEY_INFO::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(content_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 256) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 256; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        content_[i] = (uint8_t)c;
    }
    if (srcBuf.capacity() - srcBuf.size() < 256) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 256; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        content2_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SX_KEY_INFO::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SCS_SX_KEY_INFO::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[send_data_len_]", "%d", send_data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[version_]", "%u", version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[content_info_size_]", "%u", content_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[num_info_size_]", "%u", num_info_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[content_]", 256);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 256; ++i) {
        ret = destBuf.textize(" 0x%02x", content_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[content2_]", 256);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 256; ++i) {
        ret = destBuf.textize(" 0x%02x", content2_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SCS_SX_KEY_INFO::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SCS_SX_KEY_INFO::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SCS_SX_KEY_INFO::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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