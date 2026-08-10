// Reconstructed from secagent disassembly

#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"
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


TdrError::ErrorType SendToSDK::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SendToSDK::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(to_busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(from_busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32767) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint16_t i = 0; i < data_len_; ++i) {
        ret = destBuf.writeUInt8(data_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SendToSDK::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SendToSDK::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(to_busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(from_busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ > 32767) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < data_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint16_t i = 0; i < data_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        data_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SendToSDK::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SendToSDK::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]", "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[to_busi_id_]", "%d", to_busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[from_busi_id_]", "%d", from_busi_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[data_len_]", "%u", data_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[data_]", 0);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (data_len_ != 0) {
        for (uint16_t i = 0; i < data_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", data_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SendToSDK::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SendToSDK::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SendToSDK::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType PunishClientReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(punish_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(punish_duration_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(punish_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (punish_info_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < punish_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint16_t i = 0; i < punish_info_len_; ++i) {
        ret = destBuf.writeUInt8(punish_info_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt16(punish_reason_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (punish_reason_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < punish_reason_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint16_t i = 0; i < punish_reason_len_; ++i) {
        ret = destBuf.writeUInt8(punish_reason_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType PunishClientReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(punish_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(punish_duration_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(punish_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (punish_info_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < punish_info_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint16_t i = 0; i < punish_info_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        punish_info_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt16(punish_reason_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (punish_reason_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < punish_reason_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint16_t i = 0; i < punish_reason_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        punish_reason_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType PunishClientReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]", "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[punish_type_]", "%u", punish_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[punish_duration_]", "%u", punish_duration_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[punish_info_len_]", "%u", punish_info_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[punish_info_]", 0);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (punish_info_len_ != 0) {
        for (uint16_t i = 0; i < punish_info_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", punish_info_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[punish_reason_len_]", "%u", punish_reason_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[punish_reason_]", 0);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (punish_reason_len_ != 0) {
        for (uint16_t i = 0; i < punish_reason_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", punish_reason_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* PunishClientReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* PunishClientReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType PunishClientRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType PunishClientRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType PunishClientRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]", "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "%d", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType PunishClientRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* PunishClientRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* PunishClientRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType DpBanModeDetail::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeDetail::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(mode_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(keep_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeDetail::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType DpBanModeDetail::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(mode_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(keep_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeDetail::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType DpBanModeDetail::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[mode_id_]", "%u", mode_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[keep_time_]", "%u", keep_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeDetail::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DpBanModeDetail::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* DpBanModeDetail::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType DpBanModeReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return TdrError::TDR_NO_ERROR;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = modes_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType DpBanModeReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return TdrError::TDR_NO_ERROR;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = modes_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType DpBanModeReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[count_]", "%u", count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 32) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return ret;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[modes_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = modes_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = modes_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpBanModeReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DpBanModeReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* DpBanModeReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType DpDropCmdlistInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(cmdlist_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(keep_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType DpDropCmdlistInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(cmdlist_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(keep_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType DpDropCmdlistInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmdlist_id_]", "%u", cmdlist_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[keep_time_]", "%u", keep_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DpDropCmdlistInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* DpDropCmdlistInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType DpDropCmdlistReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 100) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return TdrError::TDR_NO_ERROR;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = cmd_lists_[i].pack(destBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType DpDropCmdlistReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 100) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return TdrError::TDR_NO_ERROR;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = cmd_lists_[i].unpack(srcBuf, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType DpDropCmdlistReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[count_]", "%u", count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (count_ > 100) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (count_ == 0) return ret;
    for (uint32_t i = 0; i < count_; ++i) {
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_lists_]", i, true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = cmd_lists_[i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = cmd_lists_[i].visualize(destBuf, indent, separator);
        }
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DpDropCmdlistReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DpDropCmdlistReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* DpDropCmdlistReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SafemodeStatusRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeStatusRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(ipc_days_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(stat_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(has_verify_mb_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(event_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 32) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 32; ++i) {
        ret = destBuf.writeUInt8(reserve_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeStatusRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SafemodeStatusRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(ipc_days_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(stat_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(has_verify_mb_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(event_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 32) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 32; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        reserve_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeStatusRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SafemodeStatusRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ipc_days_]", "%d", ipc_days_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[stat_level_]", "0x%02x", stat_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[has_verify_mb_]", "0x%02x", has_verify_mb_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[event_type_]", "%d", event_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[reserve_]", 32);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 32; ++i) {
        ret = destBuf.textize(" 0x%02x", reserve_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeStatusRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SafemodeStatusRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SafemodeStatusRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SafemodeLimitStatusRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeLimitStatusRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32((uint32_t)ret_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)limit_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(unlock_url_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (unlock_url_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (destBuf.capacity() - destBuf.size() < unlock_url_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (uint16_t i = 0; i < unlock_url_len_; ++i) {
        ret = destBuf.writeUInt8(unlock_url_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeLimitStatusRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SafemodeLimitStatusRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    uint32_t tmp4ret_;
    ret = srcBuf.readUInt32(tmp4ret_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret_ = (int32_t)tmp4ret_;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    uint32_t tmp4limit_status_;
    ret = srcBuf.readUInt32(tmp4limit_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    limit_status_ = (int32_t)tmp4limit_status_;
    ret = srcBuf.readUInt16(unlock_url_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (unlock_url_len_ > 1024) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (srcBuf.capacity() - srcBuf.size() < unlock_url_len_) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (uint16_t i = 0; i < unlock_url_len_; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        unlock_url_[i] = (uint8_t)c;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeLimitStatusRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SafemodeLimitStatusRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[ret_]", "%d", ret_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin_]", "%u", uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[limit_status_]", "%d", limit_status_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[unlock_url_len_]", "%u", unlock_url_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[unlock_url_]", 0);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (unlock_url_len_ != 0) {
        for (uint16_t i = 0; i < unlock_url_len_; ++i) {
            ret = destBuf.textize(" 0x%02x", unlock_url_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeChar(separator);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeChar('\0');
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SafemodeLimitStatusRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SafemodeLimitStatusRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SafemodeLimitStatusRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType DPCaptchaResultRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPCaptchaResultRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPCaptchaResultRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType DPCaptchaResultRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPCaptchaResultRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType DPCaptchaResultRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[result_]", "0x%02x", result_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPCaptchaResultRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DPCaptchaResultRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* DPCaptchaResultRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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