// Reconstructed from secagent disassembly
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_TSSSDK_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_TSSSDK_H_H_

#include <stdint.h>
#include <stddef.h>
#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {
class TdrWriteBuf;
class TdrReadBuf;
}

namespace sec_proto {

#pragma pack(push, 1)

class SendToSDK {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t time_stamp_;
uint16_t to_busi_id_;
uint16_t from_busi_id_;
    uint16_t data_len_;
    uint8_t data_[32767];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class PunishClientReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t time_stamp_;
uint32_t punish_type_;
uint32_t punish_duration_;
    uint16_t punish_info_len_;
    uint8_t punish_info_[1024];
    uint16_t punish_reason_len_;
    uint8_t punish_reason_[1024];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class PunishClientRsp {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t time_stamp_;
uint16_t result_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class DpBanModeDetail {
public:
uint32_t mode_id_;
uint32_t keep_time_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class DpBanModeReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t count_;
    DpBanModeDetail modes_[32];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class DpDropCmdlistInfo {
public:
uint32_t cmdlist_id_;
uint32_t keep_time_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class DpDropCmdlistReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t count_;
    DpDropCmdlistInfo cmd_lists_[100];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class SafemodeStatusRsp {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint16_t ipc_days_;
uint8_t stat_level_;
uint8_t has_verify_mb_;
uint16_t event_type_;
    uint8_t reserve_[32];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class SafemodeLimitStatusRsp {
public:
int32_t ret_;
uint32_t uin_;
int32_t limit_status_;
    uint16_t unlock_url_len_;
    uint8_t unlock_url_[1024];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class DPCaptchaResultRsp {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint8_t result_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_TSSSDK_H_H_
