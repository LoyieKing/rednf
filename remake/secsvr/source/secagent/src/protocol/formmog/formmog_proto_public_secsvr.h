// Reconstructed from secagent disassembly
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_SECSVR_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_SECSVR_H_H_

#include <stdint.h>
#include <stddef.h>
#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {
class TdrWriteBuf;
class TdrReadBuf;
}

namespace sec_proto {

#pragma pack(push, 1)

class AddUserReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t time_stamp_;
uint32_t client_version_;
uint32_t client_ip_;
uint32_t server_ip_;
    char player_name_[256];

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

class ConfirmRpcodeInfo {
public:
uint16_t world_id_;
uint8_t dib_id_;
uint32_t key_offset_;

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

class DelUserReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t time_stamp_;

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

class RecvDataFromSDKReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t time_stamp_;
uint16_t to_busi_id_;
uint16_t from_busi_id_;
uint32_t to_secsvr_seq_;
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

class ReportErrReq {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t error_id_;

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

class ReportHeartbeatReq {
public:
uint16_t room_id_;

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

class ReportLostAntibotPkg {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t last_seq_;
uint32_t now_seq_;

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

class ReportPkgTime {
public:
uint16_t channel_id_;
uint16_t room_id_;
uint32_t last_report_time_;
uint32_t now_report_time_;
int32_t now_cmd_;

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

class ReqSecsvrForwardData {
public:
uint16_t channel_id_;
uint16_t room_id_;
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

class ReqSecsvrFreeLock {
public:
uint32_t world_id_;

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

class RspSecsvrFreeLock {
public:
uint32_t world_id_;

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

class SecsvrForwardDataToClientReq {
public:
uint16_t world_id_;
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

class SecsvrNotifyIsChangeSvrRsp {
public:
uint16_t world_id_;
uint8_t is_change_svr_;

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

class SecsvrNotifyKeyOffsetRsp {
public:
uint16_t world_id_;
uint16_t channel_id_;
uint8_t dib_id_;
uint32_t key_offset_;
uint8_t key_size_;

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

class SecsvrNotifyMACRsp {
public:
uint16_t world_id_;
uint32_t src_id_;
uint32_t client_ip_;
uint32_t login_time_;
    uint8_t mac_[16];
uint8_t first_login_;
uint8_t report_from_;

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

class SecsvrNotifyTcjHeartbeatTimeoutRsp {
public:
uint16_t world_id_;
uint32_t src_id_;
uint32_t login_time_;
uint32_t check_time_;
uint32_t timeout_;

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

class SecsvrNotifyUserLoginRsp {
public:
uint16_t world_id_;
uint32_t src_id_;
uint32_t client_ip_;
uint32_t login_time_;

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

class SecsvrNotifyUserLogoutRsp {
public:
uint16_t world_id_;
uint32_t src_id_;
uint32_t client_ip_;
uint32_t login_time_;
uint32_t logout_time_;

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

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_PUBLIC_SECSVR_H_H_
