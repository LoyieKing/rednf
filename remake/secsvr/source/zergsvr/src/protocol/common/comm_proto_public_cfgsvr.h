// Reconstructed from gunnersvr disassembly
// Original path: src/protocol/common/comm_proto_public_cfgsvr.h
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_CFGSVR_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_CFGSVR_H_H_

#include <stdint.h>
#include <stddef.h>
#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {
class TdrWriteBuf;
class TdrReadBuf;
}

namespace sec_proto {

#pragma pack(push, 1)

class ConfigBaseInfo;
class ConfigMD5Info;
class SvcIpInfoReq;
class SvcIpInfoRsp;
class SvcIdReq;
class SvcIdRsp;
class CfgsvrInfoRsp;
class DeployInfo;
class DeployInfoReq;
class SvrHeartbeatReq;
class StartSynCfgFileReq;
class StartSynSvrInfoRsp;
class SvrInfo;
class SynSvrInfoReq;
class ConfigStorageInfo;
class SvrUpdateInfo;
class CfgFileReq;
class CfgFileRsp;
class CheckCfgFileUpdateReq;
class CheckCfgFileUpdateRsp;
class DeleteConfigReq;
class DeployInfoRsp;
class NotifySynCfgFileReq;
class StartSynCfgFileRsp;
class SynSvrInfoRsp;
class UpdateCfgFileReq;
class UpdateSvrInfoReq;

class ConfigBaseInfo {
public:
    uint16_t svc_type_;
    uint32_t svc_id_;
    char cfg_name_[128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class ConfigMD5Info {
public:
    char cfg_name_[128];
    unsigned char cfg_md5_[16];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvcIpInfoReq {
public:
    uint16_t svc_type_;
    uint32_t svc_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvcIpInfoRsp {
public:
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t bind_ip_;
    uint16_t bind_port_;
    uint32_t idc_no_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvcIdReq {
public:
    uint16_t svc_type_;
    uint16_t svc_index_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvcIdRsp {
public:
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t game_id_;
    uint32_t idc_no_;
    uint32_t world_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class CfgsvrInfoRsp {
public:
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t bind_ip_;
    uint16_t bind_port_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class DeployInfo {
public:
    uint32_t game_id_;
    uint32_t idc_no_;
    uint16_t world_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class DeployInfoReq {
public:
    uint16_t svc_type_;
    uint32_t svc_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvrHeartbeatReq {
public:
    uint16_t svc_type_;
    uint32_t svc_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class StartSynCfgFileReq {
public:
    int32_t expect_cfg_info_index_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class StartSynSvrInfoRsp {
public:
    uint32_t total_svr_num_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvrInfo {
public:
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint16_t svc_index_;
    uint32_t internal_ip_;
    uint32_t bind_ip_;
    uint16_t bind_port_;
    uint32_t idc_no_;
    uint32_t world_;
    uint32_t game_id_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SynSvrInfoReq {
public:
    uint32_t curr_svr_num_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class ConfigStorageInfo {
public:
    ConfigBaseInfo cfg_base_info_;
    uint32_t storage_index_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SvrUpdateInfo {
public:
    uint16_t update_type_;
    SvrInfo svr_info_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class CfgFileReq {
public:
    ConfigBaseInfo cfg_base_info_;
    unsigned char cfg_md5_[16];
    uint32_t recv_len_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class CfgFileRsp {
public:
    int32_t result_;
    ConfigBaseInfo cfg_base_info_;
    uint32_t cfg_len_;
    unsigned char cfg_md5_[16];
    uint32_t data_len_;
    char data_buf_[64000];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class CheckCfgFileUpdateReq {
public:
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t cfg_num_;
    ConfigMD5Info cfg_md5_info_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class CheckCfgFileUpdateRsp {
public:
    uint32_t cfg_num_;
    char cfg_name_[2048 * 128];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class DeleteConfigReq {
public:
    uint32_t del_num_;
    ConfigBaseInfo cfg_info_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class DeployInfoRsp {
public:
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t deploy_info_num_;
    DeployInfo deploy_info_[10];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class NotifySynCfgFileReq {
public:
    uint32_t cfg_num_;
    ConfigStorageInfo cfg_info_[2048];
    int32_t seq_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class StartSynCfgFileRsp {
public:
    ConfigStorageInfo cfg_storage_info_;
    int32_t cfg_info_index_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class SynSvrInfoRsp {
public:
    uint32_t curr_svr_num_;
    uint32_t syn_svr_num_;
    SvrInfo svr_info_list_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class UpdateCfgFileReq {
public:
    uint32_t cfg_num_;
    ConfigStorageInfo cfg_info_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

class UpdateSvrInfoReq {
public:
    uint32_t svr_num_;
    SvrUpdateInfo svr_update_info_list_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, size_t size, size_t* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, size_t size, size_t* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    const char* visualize_ex(char* buffer, size_t size, size_t* usedSize, int indent, char separator) const;
};

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_CFGSVR_H_H_
