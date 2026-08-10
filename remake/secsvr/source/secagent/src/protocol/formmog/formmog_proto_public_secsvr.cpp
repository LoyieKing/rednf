// Reconstructed from secagent disassembly

#include "src/protocol/formmog/formmog_proto_public_secsvr.h"
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


TdrError::ErrorType AddUserReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AddUserReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(server_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const size_t sizePos4player_name_ = destBuf.size();
    ((char*)player_name_)[sizeof(player_name_) / sizeof(player_name_[0]) - 1] = '\0';
    size_t realSize4player_name_ = strlen(player_name_) + 1;
    ret = destBuf.writeString(player_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AddUserReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType AddUserReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(server_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = readBoundedString(srcBuf, player_name_, sizeof(player_name_));
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AddUserReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType AddUserReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]", "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_version_]", "%u", client_version_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]", "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[server_ip_]", "%u", server_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printString(destBuf, indent, separator, "[player_name_]", player_name_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AddUserReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* AddUserReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* AddUserReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ConfirmRpcodeInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ConfirmRpcodeInfo::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ConfirmRpcodeInfo::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ConfirmRpcodeInfo::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ConfirmRpcodeInfo::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ConfirmRpcodeInfo::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dib_id_]", "0x%02x", dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_offset_]", "%u", key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ConfirmRpcodeInfo::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ConfirmRpcodeInfo::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ConfirmRpcodeInfo::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType DelUserReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DelUserReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DelUserReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType DelUserReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DelUserReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType DelUserReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[time_stamp_]", "%u", time_stamp_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DelUserReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* DelUserReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* DelUserReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType RecvDataFromSDKReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType RecvDataFromSDKReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
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
    ret = destBuf.writeUInt32(to_secsvr_seq_);
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

TdrError::ErrorType RecvDataFromSDKReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType RecvDataFromSDKReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
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
    ret = srcBuf.readUInt32(to_secsvr_seq_);
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

TdrError::ErrorType RecvDataFromSDKReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType RecvDataFromSDKReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
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
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[to_secsvr_seq_]", "%u", to_secsvr_seq_);
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

TdrError::ErrorType RecvDataFromSDKReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* RecvDataFromSDKReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* RecvDataFromSDKReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ReportErrReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportErrReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportErrReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ReportErrReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportErrReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ReportErrReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[error_id_]", "%u", error_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportErrReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ReportErrReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ReportErrReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ReportHeartbeatReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportHeartbeatReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportHeartbeatReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ReportHeartbeatReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportHeartbeatReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ReportHeartbeatReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportHeartbeatReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ReportHeartbeatReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ReportHeartbeatReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ReportLostAntibotPkg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportLostAntibotPkg::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(last_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(now_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportLostAntibotPkg::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ReportLostAntibotPkg::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(last_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(now_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportLostAntibotPkg::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ReportLostAntibotPkg::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[last_seq_]", "%u", last_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[now_seq_]", "%u", now_seq_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportLostAntibotPkg::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ReportLostAntibotPkg::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ReportLostAntibotPkg::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ReportPkgTime::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportPkgTime::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(last_report_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(now_report_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32((uint32_t)now_cmd_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportPkgTime::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ReportPkgTime::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(last_report_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(now_report_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    uint32_t tmp4now_cmd_;
    ret = srcBuf.readUInt32(tmp4now_cmd_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    now_cmd_ = (int32_t)tmp4now_cmd_;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportPkgTime::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ReportPkgTime::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[last_report_time_]", "%u", last_report_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[now_report_time_]", "%u", now_report_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[now_cmd_]", "%d", now_cmd_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReportPkgTime::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ReportPkgTime::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ReportPkgTime::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ReqSecsvrForwardData::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReqSecsvrForwardData::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(room_id_);
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

TdrError::ErrorType ReqSecsvrForwardData::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ReqSecsvrForwardData::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(room_id_);
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

TdrError::ErrorType ReqSecsvrForwardData::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ReqSecsvrForwardData::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id_]", "%d", room_id_);
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

TdrError::ErrorType ReqSecsvrForwardData::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ReqSecsvrForwardData::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ReqSecsvrForwardData::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType ReqSecsvrFreeLock::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReqSecsvrFreeLock::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReqSecsvrFreeLock::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType ReqSecsvrFreeLock::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReqSecsvrFreeLock::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType ReqSecsvrFreeLock::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%u", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ReqSecsvrFreeLock::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* ReqSecsvrFreeLock::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* ReqSecsvrFreeLock::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType RspSecsvrFreeLock::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType RspSecsvrFreeLock::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType RspSecsvrFreeLock::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType RspSecsvrFreeLock::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType RspSecsvrFreeLock::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType RspSecsvrFreeLock::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%u", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType RspSecsvrFreeLock::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* RspSecsvrFreeLock::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* RspSecsvrFreeLock::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrForwardDataToClientReq::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrForwardDataToClientReq::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
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

TdrError::ErrorType SecsvrForwardDataToClientReq::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrForwardDataToClientReq::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
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

TdrError::ErrorType SecsvrForwardDataToClientReq::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrForwardDataToClientReq::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
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

TdrError::ErrorType SecsvrForwardDataToClientReq::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrForwardDataToClientReq::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrForwardDataToClientReq::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(is_change_svr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(is_change_svr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[is_change_svr_]", "0x%02x", is_change_svr_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyIsChangeSvrRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrNotifyIsChangeSvrRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrNotifyIsChangeSvrRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt16(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id_]", "%d", channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[dib_id_]", "0x%02x", dib_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_offset_]", "%u", key_offset_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[key_size_]", "0x%02x", key_size_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyKeyOffsetRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrNotifyKeyOffsetRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrNotifyKeyOffsetRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrNotifyMACRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyMACRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.writeUInt8(mac_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = destBuf.writeUInt8(first_login_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(report_from_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyMACRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyMACRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (srcBuf.capacity() - srcBuf.size() < 16) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    for (unsigned int i = 0; i < 16; ++i) {
        char c;
        ret = srcBuf.readChar(c);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        mac_[i] = (uint8_t)c;
    }
    ret = srcBuf.readUInt8(first_login_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(report_from_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyMACRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyMACRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]", "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]", "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[login_time_]", "%u", login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[mac_]", 16);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    for (unsigned int i = 0; i < 16; ++i) {
        ret = destBuf.textize(" 0x%02x", mac_[i]);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[first_login_]", "0x%02x", first_login_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[report_from_]", "0x%02x", report_from_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyMACRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrNotifyMACRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrNotifyMACRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(check_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(check_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]", "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[login_time_]", "%u", login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[check_time_]", "%u", check_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[timeout_]", "%u", timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyTcjHeartbeatTimeoutRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrNotifyTcjHeartbeatTimeoutRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrNotifyTcjHeartbeatTimeoutRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrNotifyUserLoginRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLoginRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLoginRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyUserLoginRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLoginRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyUserLoginRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]", "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]", "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[login_time_]", "%u", login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLoginRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrNotifyUserLoginRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrNotifyUserLoginRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(logout_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(logout_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.size();
    return ret;
}

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[world_id_]", "%d", world_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[src_id_]", "%u", src_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[client_ip_]", "%u", client_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[login_time_]", "%u", login_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[logout_time_]", "%u", logout_time_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SecsvrNotifyUserLogoutRsp::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.size();
    return ret;
}

const char* SecsvrNotifyUserLogoutRsp::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos != totalSize) {
        ((char*)destBuf.data())[targetPos] = '\0';
    }
    return destBuf.data();
}

const char* SecsvrNotifyUserLogoutRsp::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {
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