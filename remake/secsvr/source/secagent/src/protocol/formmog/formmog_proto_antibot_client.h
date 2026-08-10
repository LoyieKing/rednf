// Reconstructed from secagent disassembly (TDR-generated protocol classes)
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_

#include <stdint.h>
#include <stddef.h>
#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {
class TdrWriteBuf;
class TdrReadBuf;
}

namespace sec_proto {

#pragma pack(push, 1)

class AB_CS_REPORT_ERROR {
public:
    uint16_t error_func_id_;
    uint8_t error_type_;
    uint32_t error_index_;
    uint32_t error_code_;
    uint32_t eip_;
    uint8_t error_env_[16];
    uint32_t reserved1_;
    uint32_t reserved2_;

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

class APCReportInfo {
public:
    char info_[1024];

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

class TAB_CS_DETESTPKG {
public:
    uint32_t test_pkg_crc_;
    uint8_t content_size_;
    uint8_t content_[256];

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

class TAB_SC_TVCHECK {
public:
    uint32_t tv_gap_;
    uint8_t tv_info_key_[16];
    uint8_t tv_check_flag_;

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

class TAB_CS_ANTICLTKEY {
public:
    uint16_t flag_;
    uint32_t uin_;
    uint32_t ip_;
    uint8_t signature_[16];
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TAB_SC_CHANNEL_SERVER_ID {
public:
    uint16_t channel_id_;
    uint16_t world_id_;
    uint32_t service_id_;

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

class TAB_SC_STOPLCHECK {
public:
    uint8_t stop_rand_[16];

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

class TAB_SC_CFGDATA {
public:
    uint8_t type_;
    uint8_t len_;
    uint8_t send_data_[32727];

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

class TAB_CS_CAPTCHAREQ {
public:
    uint16_t send_data_len_;
    uint8_t send_data_[32727];

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

class TAB_SC_FMSCMD {
public:
    uint16_t dumb_;
    uint16_t fms_scn_cmd_len_;
    uint8_t fms_scn_cmd_data_[32727];

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

class TAB_SC_DELAY_CHECK {
public:
    uint64_t secsvr_down_time_;
    uint64_t sdk_down_time_;

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

class TAB_CS_INITINFO {
public:
    uint8_t is_init_;
    uint8_t sig_flag_;
    uint8_t rccp_ver_[16];
    uint32_t module_handle_;
    uint32_t lib_base_;
    uint8_t os_ver_;
    uint8_t st_ver_[16];
    uint32_t proc_address_;
    uint8_t sf_ver_[16];
    uint16_t rc_id_;
    uint32_t client_ver_;
    uint8_t user_id_[16];
    uint8_t admin_;
    uint8_t dib_id_;
    uint32_t key_offset_;
    uint8_t key_len_;
    uint8_t rp_data_info_size_;
    uint8_t rp_data_info_[256];
    uint8_t cs_data_;
    uint8_t len_;
    uint8_t send_data_[256];

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

class TAB_CS_CSCHECK {
public:
    uint16_t rpcs_result_len_;
    uint8_t rpcs_result_[32727];

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

class TQD_CS_INITDATA {
public:
    uint32_t crc_;
    uint16_t data_len_;
    uint8_t mem_sign_ver_[16];
    uint8_t file_sign_ver_[16];
    uint8_t scan_engine_ver_[16];
    uint8_t clean_engine_ver_[16];

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

class SClientGUIDInfo {
public:
    uint8_t guid_[16];
    uint8_t reserve_[16];

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

class TAB_SC_CSCFGDATA {
public:
    uint8_t encrypt_key_[16];
    uint16_t rpcs_data_len_;
    uint8_t rpcs_data_[32727];

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

class TAB_CS_SYSMEMLOG {
public:
    uint8_t system_log_size_;
    uint8_t system_log_[256];
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TAB_SC_DE_CMDID_LIST {
public:
    uint8_t cmd_count_;
    uint16_t cmd_[200];

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

class TAB_CS_SUSMODINFO {
public:
    uint16_t sus_mod_info_len_;
    uint8_t sus_mod_info_data_[32727];
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TAB_SC_PKGCODE {
public:
    uint16_t rp_id_;
    uint16_t rpcode_data_size_;
    uint8_t rpcode_data_[32727];

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

class TAB_SC_TENPRTYDATA {
public:
    uint16_t tenpary_data_len_;
    uint8_t tenpary_data_[32727];

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

class TAB_CS_VERIFYRESULT {
public:
    uint16_t ext_size_;
    uint32_t uin_;
    uint8_t stamp_[16];
    uint8_t module_[16];
    uint8_t out_proc_[16];
    uint8_t trojan_[16];
    uint8_t game_id_;
    uint8_t bot_id_;
    uint8_t err_msg_size_;
    uint8_t err_msg_[256];
    uint8_t client_passitive_;
    uint32_t client_rand_value_;
    uint8_t send_data_len_;
    uint8_t send_data_[32727];
    uint8_t rand_[2];
    uint8_t local_check_flag_;
    uint8_t rc_check_[8];

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

class TAB_CS_CSCHANNEL_RESULT {
public:
    uint32_t error_code_;

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

class TAB_SC_REQCLIRESENDDATA {
public:
    uint8_t rand_data_[16];
    uint8_t type_;

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

class TAB_CS_DEREADY {
public:
    uint8_t id_;
    uint32_t key_crc_;
    uint8_t err_msg_size_;
    uint8_t err_msg_[256];

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

class TAB_CS_DELOG {
public:
    uint8_t error_id_;
    uint32_t error_;
    uint8_t pad_size_;
    uint8_t buf_[128];

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

class TMODULE_NAME {
public:
    uint8_t name_len_;
    uint8_t module_name_[256];

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

class TFILE_HASH_INFO_ITEM {
public:
    uint8_t id_;
    TMODULE_NAME file_name_;
    uint8_t file_type_;
    uint32_t offset_;
    uint32_t len_;

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

class TFILE_HASH_INFO {
public:
    uint16_t count_;
    TFILE_HASH_INFO_ITEM file_hash_item_[10];

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

class TAB_SC_FILE_HASH {
public:
    uint16_t buf_len_;
    TFILE_HASH_INFO file_hash_;

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

class TROJANINFO {
public:
    uint32_t virus_id_;
    uint32_t mod_size_;
    uint8_t path_name_[512];

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

class SLX_TROJAN_INFO {
public:
    uint32_t trojan_count_;
    TROJANINFO trojans_[32];

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

class TROJAN_REPORT {
public:
    union {
        SLX_TROJAN_INFO trojans_;
        uint32_t trojan_count_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class TSANLIX_CS_REPORT {
public:
    uint8_t type_;
    uint16_t len_;
    TROJAN_REPORT trojan_;

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

class TAB_CS_RPFMCSCANRESULT {
public:
    uint32_t hash_;
    uint16_t rpfmc_scan_result_len_;
    uint8_t rpfmc_scan_result_data_[32727];

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

class TAB_SC_LAPKGCMD {
public:
    uint16_t la_cmd_size_;
    uint8_t la_cmd_data_[32727];
    uint8_t pad_data_size_;
    uint8_t pad_data_[128];

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

class TAB_CS_RESENDDATA {
public:
    uint8_t data_flag_;
    uint8_t re_send_reason_;
    uint32_t resend_data_info_;

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

class TAB_CS_CLIENTINFO {
public:
    uint16_t client_info_msg_size_;
    uint8_t client_info_msg_[32727];

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

class TAB_SC_DEKEY {
public:
    uint8_t pad_data_size_;
    uint8_t pad_data_[128];
    uint32_t key_crc_;
    uint8_t key_len_;
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

class TAB_CS_ENCRYPT {
public:
    uint32_t test_pkg_crc_;
    uint16_t pkg_len_;
    uint8_t pkg_data_[32727];

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

class TAB_CS_FMCRESULT {
public:
    uint16_t fms_result_len_;
    uint8_t fms_result_data_[32727];
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TAB_CS_CAPTCHARESULT {
public:
    uint16_t send_data_len_;
    uint8_t send_data_[32727];

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

class TAB_SC_PKGCODE_KEY {
public:
    uint8_t key_info_len_;
    uint8_t key_info_[64];

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

class TAB_SC_DEDIBDATA {
public:
    uint8_t id_;
    uint8_t dib_;
    uint16_t dib_data_len_;
    uint8_t dib_data_[32727];

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

class FILE_HASH_VAL {
public:
    uint8_t file_id_;
    uint8_t file_hash_[16];

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

class TAB_CS_CLINETINFO {
public:
    uint8_t file_num_;
    FILE_HASH_VAL file_hash_data_[10];

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

class TAB_CS_IATCHECK {
public:
    uint8_t pad_len_;
    uint8_t pad_[256];
    uint8_t iat_check_result_;
    uint8_t iat_info_size_;
    uint8_t iat_info_[256];

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

class TAB_SC_RPFMCSCANCMD {
public:
    uint8_t clr_flag_;
    uint16_t rpfmc_scan_cmd_len_;
    uint8_t rpfmc_scan_cmd_data_[32727];

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

class TQD_SC_CMDDATA {
public:
    uint32_t crc_;
    uint16_t len_;
    uint8_t hash_[16];

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

class TAB_SC_SENDANTIBOT {
public:
    uint32_t init_crc_value_;
    uint8_t send_ver_[16];
    uint8_t data_flag_;
    uint16_t rcid_;
    uint16_t send_data_len_;
    uint8_t send_data_[32727];

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

class TAB_SC_ENCRYPT {
public:
    uint32_t test_pkg_crc_;
    uint16_t pkg_size_;
    uint8_t pkg_data_[32727];

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

class ThreadVerifyInfo {
public:
    uint16_t thread_id_;
    uint16_t status_;
    uint32_t tp_time_;
    uint32_t ts_time_;

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

class TAB_CS_TVINFO {
public:
    uint32_t key_crc_;
    uint32_t main_code_offset_;
    uint32_t main_code_size_;
    uint8_t tv_flag_;
    uint32_t thread_num_;
    ThreadVerifyInfo thread_verify_infos_[20];
    uint8_t pad_size_;
    uint8_t pad_buf_[32];

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

class TAB_CS_LAPKGRESULT {
public:
    uint32_t hash_;
    uint16_t la_result_len_;
    uint8_t aucLaResultData[32727];
    uint32_t serial_;
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TQD_CS_VIRUSINFO {
public:
    uint32_t process_type_;
    uint8_t virus_name_[64];
    uint8_t module_name_[512];

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

class TQD_CS_SCANRESULT {
public:
    uint32_t crc_;
    uint16_t data_len_;
    uint16_t data_len_temp_;
    uint8_t data_temp_[32727];
    uint32_t uin_;
    uint16_t virus_cnt_;
    TQD_CS_VIRUSINFO virus_info_[10];

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

class TAB_SC_RESTRICTUIN {
public:
    uint32_t ban_time_;
    uint8_t pad_data_size_;
    uint8_t pad_data_[128];

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

class TAB_CS_RPPKGRESULT {
public:
    uint32_t rpcode_data_hash_;
    uint16_t rpcode_result_data_size_;
    uint8_t rpcode_result_data_[32727];
    uint8_t rpcode_func_id_;
    uint32_t serial_;
    uint16_t hash_idx_;
    uint32_t hash_;
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TAB_CS_ANTITIME {
public:
    uint32_t uin_;
    uint32_t time_stamp_;
    uint32_t seq_;
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class SAB_MAC_INFO {
public:
    uint16_t send_data_len_;
    uint8_t ipcs_status_;
    uint8_t read_flag_;
    uint32_t read_status_;
    uint32_t qq_login_mac_crc_;
    uint32_t game_mac_crc_;
    uint32_t qq_login_uin_;
    uint32_t game_uin_;

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

class SRP_RPCODE_HASH {
public:
    uint16_t send_data_len_;
    uint8_t fun_id_;
    uint8_t pad_data_len_;
    uint8_t pad_[256];
    uint8_t hash_count_;
    uint32_t hash_[40];
    uint32_t times_;
    uint32_t send_func_addr_;
    uint32_t no_die_value_;
    uint8_t garbage_data_len_;
    uint8_t garbage_[256];

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

class SanlixScanResult {
public:
    uint16_t data_len_;
    int32_t scan_result_;
    uint32_t sanlix_dat_version_low_;
    uint32_t sanlix_dat_version_high_;
    uint32_t sanlix_id_;
    uint8_t flag_;
    char virus_name_[64];
    char scan_module_[512];

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

class TVMPFuncHashInfo {
public:
    uint32_t addr_;
    uint32_t hash_;

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

class STVMP_REPORT_HASH_INFO {
public:
    uint16_t send_data_len_;
    TVMPFuncHashInfo func_hash_info_;

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

class SSchemeClientGUIDInfo {
public:
    uint16_t send_data_len_;
    SClientGUIDInfo guid_;

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

class ShellModuleVerify {
public:
    uint16_t data_len_;
    uint32_t verify_type_;
    uint32_t verify_start_ptr_;
    uint32_t verify_len_;
    uint32_t load_hash_;
    uint32_t verify_hash_;

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

class STPKRReportInfo {
public:
    uint16_t send_data_len_;
    uint32_t major_code_;
    uint32_t minor_code_;
    int32_t status_code_;

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

class TVMPFuncCallInfo {
public:
    uint32_t call_addr_;
    uint32_t ret_addr_;

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

class STVMP_REPORT_CALL_INFO {
public:
    uint16_t send_data_len_;
    uint32_t func_call_info_size_;
    TVMPFuncCallInfo func_call_infos_[20];

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

class SCS_SLN_DATA_BUF {
public:
    uint16_t send_data_len_;
    uint8_t send_data_[32727];

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

class SAB_SIGN_INFO {
public:
    uint16_t send_data_len_;
    uint32_t mac_id_;
    uint32_t sig_size_;
    uint8_t sig_data_[10240];

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

class SLogReport {
public:
    uint16_t send_data_len_;
    int32_t data_source_;
    int32_t game_id_;
    uint32_t uin_;
    uint32_t log_type_;
    uint8_t int_count_;
    int32_t int_values_[256];
    char comment_[1024];

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

class STP_ERROR_INFO {
public:
    uint16_t send_data_len_;
    uint32_t type_;
    uint32_t number_;
    uint32_t reserved_;

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

class SSE_ERROR_INFO {
public:
    uint16_t send_data_len_;
    uint32_t error_;

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

class SRP_THREAD_EXP {
public:
    uint16_t send_data_len_;
    uint32_t eip_;
    uint32_t exception_code_;
    uint8_t except_code_array_[32];

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

class STP_CUSTOM_STRING {
public:
    uint16_t total_len_;
    uint16_t send_data_len_;
    uint8_t send_data_[32727];

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

class SAB_CS_SLN_DATA {
public:
    union {
        SSE_ERROR_INFO se_error_info_;
        SCS_SLN_DATA_BUF se_data_;
        ShellModuleVerify shell_module_verify_;
        SanlixScanResult sanlix_scan_result_;
        STP_ERROR_INFO tp_error_info_;
        STP_CUSTOM_STRING tp_custom_string_;
        SLogReport tp_bin_log_report_;
        SSchemeClientGUIDInfo client_guid_info_;
        STPKRReportInfo tp_kr_report_info_;
        APCReportInfo tp_apc_report_info_;
        SCS_SLN_DATA_BUF ab_auth_code_;
        SAB_MAC_INFO ab_mac_info_;
        SAB_SIGN_INFO ab_sign_info_;
        SCS_SLN_DATA_BUF ab_exp_data_;
        SCS_SLN_DATA_BUF rp_msg_result_;
        SRP_THREAD_EXP rp_thread_exp_;
        SCS_SLN_DATA_BUF rp_func_exp_;
        SRP_RPCODE_HASH rp_rpcode_hash_;
        STVMP_REPORT_CALL_INFO tvmp_report_call_info_;
        STVMP_REPORT_HASH_INFO tvmp_report_hash_info_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class TAB_CS_SCHEMEDATA {
public:
    uint16_t scheme_id_;
    uint16_t sub_protocol_id_;
    SAB_CS_SLN_DATA data_;

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

class TORDINAL {
public:
    uint8_t count_;
    uint32_t ordinal_;

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

class TORDINAL_OR_NAME {
public:
    union {
        TMODULE_NAME name_;
        TORDINAL ordinal_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class TMEM_RECOVER_ITEM {
public:
    uint8_t id_;
    TORDINAL_OR_NAME name_or_ordinal_;
    uint8_t mem_type_;
    uint32_t cur_times_;
    uint32_t total_times_;
    uint32_t offset_;
    uint32_t len_;
    uint8_t mem_[100];

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

class TMEM_RECOVER {
public:
    uint16_t count_;
    TMEM_RECOVER_ITEM mem_item_[10];

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

class TAB_SC_MEM_RECOVER {
public:
    uint16_t buf_len_;
    TMEM_RECOVER mem_info_;

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

class TAB_SC_TESTPKG {
public:
    uint32_t test_pkg_crc_;
    uint8_t content_size_;
    uint8_t content_[128];

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

class TAB_SC_RANDCHECK {
public:
    uint8_t rand_data_[16];
    uint8_t type_;

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

class TAB_CS_NUMINFO {
public:
    uint16_t num_info_size_;
    uint8_t num_info_msg_[256];

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

class TAB_CS_P2PSTATINFO {
public:
    int32_t crc_;
    uint16_t serial_;
    uint8_t end_flag_;
    uint16_t size_;

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

class TAB_CS_TP_INFO_MSG {
public:
    uint8_t msg_[260];

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

class TAB_CS_TPINFO {
public:
    uint16_t tp_info_msg_size_;
    TAB_CS_TP_INFO_MSG tp_info_msg_[64];

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

class SSC_SE_SCAN_DATA_ITEM {
public:
    uint8_t cfg_id_;
    uint8_t cmd_size_;
    uint8_t cmd_data_[128];

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

class SSC_SE_SCAN_DATA {
public:
    uint16_t temp_data_len_;
    SSC_SE_SCAN_DATA_ITEM se_scan_data_item_;

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

class SanlixScanJudgeResult {
public:
    uint16_t data_len_;
    uint32_t sanlix_id_;
    uint32_t result_;
    uint32_t duration_;

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

class SSC_SLN_DATA {
public:
    uint16_t data_len_;
    uint8_t data_[32727];

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

class SC_SCHEMEDATA {
public:
    union {
        SSC_SE_SCAN_DATA se_scan_data_;
        SanlixScanJudgeResult sx_judge_result_;
        SSC_SLN_DATA tp_svr_config_;
        SSC_SLN_DATA rp_cp_data_;
        SSC_SLN_DATA sx_cfg_info_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class TAB_SC_SCHEMEDATA {
public:
    uint16_t scheme_id_;
    uint16_t scheme_data_len_;
    SC_SCHEMEDATA scheme_data_;

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

class TAB_CS_ERRPACKET {
public:
    uint16_t snd_proto_id_;
    uint32_t snd_size_;
    uint8_t err_info_size_;
    uint8_t err_info_[256];

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

class TAB_SC_SLXSHAREINFO {
public:
    uint16_t slx_data_len_;
    uint8_t slx_data_[32727];

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

class TAB_SC_RESENDDATA {
public:
    uint16_t data_proto_id_;
    uint8_t data_type_;

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

class TAB_CS_DELAY_CHECK {
public:
    uint64_t secsvr_down_time_;
    uint64_t sdk_down_time_;
    uint64_t sdk_up_time_;
    uint32_t client_up_time_;

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

class TAB_CS_SYSMEM {
public:
    uint16_t system_error_len_;
    uint8_t system_error_data_[32727];
    uint8_t pad_len_;
    uint8_t pad_data_[256];

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

class TQD_SC_CFGDATA {
public:
    uint32_t crc_;
    uint16_t len_;
    uint8_t scan_type_;
    uint8_t prompt_;
    uint8_t action_on_memory_;
    uint8_t action_on_file_;
    uint8_t scan_sub_;

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

class TAB_SC_ANTICFGINFO {
public:
    uint32_t anti_scan_gap_;

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

class TAB_CS_LOCALCHECK {
public:
    uint16_t local_check_data_len_;
    uint8_t local_check_data_[32727];
    uint8_t num_msg_size_;
    uint8_t num_msg_[256];

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

class TAB_CS_EXTINFO {
public:
    uint16_t ext_info_len_;
    uint8_t ext_info_[32727];

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

class TAB_SC_SUSMODQUEST {
public:
    uint16_t sus_addr_data_len_;
    uint8_t sus_addr_data_[32727];

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

class AntibotPkgBody {
public:
    union {
        TAB_SC_SENDANTIBOT sc_send_antibot_;
        TAB_SC_RANDCHECK sc_rand_check_;
        TAB_CS_INITINFO cs_init_info_;
        TAB_CS_VERIFYRESULT cs_verify_result_;
        TAB_SC_STOPLCHECK sc_stop_l_check_;
        TAB_CS_RESENDDATA cs_resend_data_;
        TAB_SC_RESENDDATA sc_resend_data_;
        TAB_CS_P2PSTATINFO cs_p2p_stat_info_;
        TAB_SC_CFGDATA sc_cfg_data_;
        TAB_SC_REQCLIRESENDDATA sc_req_cli_resend_data_;
        TAB_CS_CLIENTINFO cs_client_info_;
        TAB_CS_TPINFO cs_tp_info_;
        TQD_SC_CFGDATA sc_qd_cfg_data_;
        TQD_SC_CMDDATA sc_qd_cmd_data_;
        TQD_CS_SCANRESULT cs_qd_scan_result_;
        TQD_CS_INITDATA cs_qd_init_data_;
        TSANLIX_CS_REPORT cs_sanlix_report_;
        TAB_SC_TENPRTYDATA sc_tenparty_data_;
        TAB_SC_FILE_HASH sc_file_hash_;
        TAB_CS_CLINETINFO cs_cli_net_info_;
        TAB_SC_MEM_RECOVER sc_mem_recover_;
        TAB_CS_EXTINFO cs_ext_info_;
        TAB_CS_LOCALCHECK cs_local_check_;
        TAB_CS_NUMINFO cs_num_info_;
        TAB_SC_DE_CMDID_LIST sc_de_cmd_id_list_;
        TAB_SC_DEDIBDATA sc_de_dib_data_;
        TAB_SC_DEKEY sc_de_key_;
        TAB_CS_DEREADY cs_de_ready_;
        TAB_CS_DETESTPKG cs_de_test_pkg_;
        TAB_SC_TESTPKG sc_test_pkg_;
        TAB_CS_DELOG cs_de_log_;
        TAB_SC_ENCRYPT sc_encrypt_;
        TAB_CS_ENCRYPT cs_encrypt_;
        TAB_CS_ERRPACKET cs_err_packet_;
        TAB_CS_SYSMEM cs_sys_mem_;
        TAB_CS_SYSMEMLOG cs_sys_mem_log_;
        TAB_SC_RESTRICTUIN sc_restrict_uin_;
        TAB_CS_IATCHECK cs_iat_check_;
        TAB_SC_CSCFGDATA sc_cs_cfg_data_;
        TAB_CS_CSCHECK cs_cs_check_;
        TAB_SC_SUSMODQUEST sc_susmod_quest_;
        TAB_CS_SUSMODINFO cs_sus_mod_info_;
        TAB_CS_TVINFO cs_tv_info_;
        TAB_SC_TVCHECK sc_tv_check_;
        TAB_SC_SLXSHAREINFO sc_slx_share_info_;
        TAB_SC_SCHEMEDATA sc_scheme_data_;
        TAB_CS_SCHEMEDATA cs_scheme_data_;
        TAB_SC_SLXSHAREINFO sc_slx_te_share_info_;
        TAB_SC_RPFMCSCANCMD sc_rpfmc_scan_cmd_;
        TAB_CS_RPFMCSCANRESULT cs_rp_fmc_scan_result_;
        TAB_SC_LAPKGCMD sc_la_pkg_cmd_;
        TAB_CS_LAPKGRESULT cs_la_pkg_result_;
        TAB_SC_FMSCMD sc_fms_cmd_;
        TAB_CS_FMCRESULT cs_fmc_result_;
        TAB_SC_ANTICFGINFO sc_anti_cfg_info_;
        TAB_CS_ANTITIME cs_anti_time_;
        TAB_CS_ANTICLTKEY cs_anti_clt_key_;
        TAB_CS_CAPTCHARESULT cs_captcha_result_;
        TAB_CS_CAPTCHAREQ cs_captcha_req_;
        SClientGUIDInfo cs_report_client_guid_;
        AB_CS_REPORT_ERROR cs_report_error_;
        TAB_SC_PKGCODE sc_pkg_code_;
        TAB_CS_RPPKGRESULT cs_rp_pkg_result_;
        TAB_SC_PKGCODE_KEY sc_pkg_code_key_;
        TAB_SC_CHANNEL_SERVER_ID sc_channel_server_id_;
        TAB_CS_CSCHANNEL_RESULT cs_cschannel_result_;
        TAB_SC_DELAY_CHECK sc_channel_delay_check_;
        TAB_CS_DELAY_CHECK cs_channel_delay_check_;
    };

    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;
};

class AntibotPkgHead {
public:
    uint16_t cmd_id_;
    uint32_t seq_;

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

class AntibotClientPkg {
public:
    AntibotPkgHead head_;
    AntibotPkgBody body_;

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

class PKGCODE_KEY_INFO {
public:
    uint8_t dib_id_;
    uint8_t key_size_;
    uint8_t key_data_[64];

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

class SCS_SX_KEY_INFO {
public:
    uint16_t send_data_len_;
    uint32_t version_;
    uint32_t content_info_size_;
    uint32_t num_info_size_;
    uint8_t content_[256];
    uint8_t content2_[256];

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

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_
