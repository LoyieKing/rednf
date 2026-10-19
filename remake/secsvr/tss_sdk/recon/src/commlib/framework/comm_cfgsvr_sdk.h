// CfgSvrSdk, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_cfgsvr_sdk.cpp
//
// ORIG DWARF layout (sizeof 344), verified member by member against
// cu_layout.py for tag394:
//   +0x00 Zen_Thread_Task (16)
//   +0x10 self_info_ (SERVICES_ID 6)  +0x16 master_cfgsvr_info_
//   +0x1c slave_cfgsvr_info_          +0x22 svc_index_
//   +0x24 master_cfgsvr_ip_ (28)      +0x40 slave_cfgsvr_ip_ (28)
//   +0x5c cfgsdk_config_ (154)        +0xf8 cfg_name_list_ (12)
//   +0x104 config_update_list_ (12)   +0x110 file_buf_
//   +0x114 recv_len_                  +0x118 curr_cfg_name_ (4)
//   +0x11c local_cfg_md5_ (16)        +0x12c recv_cfg_md5_ (16)
//   +0x13c cfg_md5_map_ (24)          +0x154 is_pthread_start_
//   +0x155 is_updade_config_
//
// The header is self-contained: every external type used by this CU is
// modelled with the ORIG ABI so the generated calls/vtables match the
// shared object's mangled symbols.
#ifndef TSS_RECON_COMM_CFGSVR_SDK_H_
#define TSS_RECON_COMM_CFGSVR_SDK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>

#include "src/commlib/framework/comm_socket_channel_tcp_handler.h" // ZEN_Sockaddr_In, ZEN_Socket_Stream
#include "src/commlib/zenlib/zen_recon_support.h"
// Shared framework declarations (recon-side tag-394 models; the reuse-tree
// copies carry the same ORIG layouts but pull vendor TDR/zen headers, so the
// recon TUs use the shadows resolved by -I recon):
//   SERVICES_ID       (comm_service_info.h, pack(1), 6 bytes)
//   Comm_App_Frame    (comm_app_frame.h, 50 + flexible appdata)
//   Zen_Thread_Task   (zen_thread_task.h, 16 bytes)
//   Comm_Svrd_Config  (comm_svrd_config.h, 166692 bytes)
// comm_app_frame.h is included after zen_recon_support.h so the recon
// ZEN_LOG_PRIORITY enum (which keeps the legacy ZEN_LOG_PRIORITY_LEVEL_*
// spellings for not-yet-migrated TUs) is declared first; both headers share
// the TSS_ZEN_LOG_PRIORITY_DECLARED guard.
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/zen_thread_task.h"
#include "src/commlib/framework/comm_svrd_config.h"

// ---------------------------------------------------------------------------
// zenlib / framework externals
// ---------------------------------------------------------------------------
typedef int ZEN_HANDLE;
typedef unsigned long ZEN_THREAD_ID;


using namespace std;

namespace ZEN_OS {
int open(const char *filename, int open_mode, mode_t perms);
int close(ZEN_HANDLE handle);
int read(ZEN_HANDLE handle, void *buffer, unsigned int buf_len);
int write(ZEN_HANDLE handle, const void *buffer, unsigned int buf_len);
int sleep(unsigned int seconds);
unsigned long pthread_self();
int unlink(const char *pathname);
}

// ORIG 0x001b8960: _Z6TenMd5PhjS_ (non-const source pointer, returns 16).
unsigned int TenMd5(unsigned char *src, unsigned int len, unsigned char *dst);



// ---------------------------------------------------------------------------
// TDR layer (only what the config/protocol structs reference).
// ---------------------------------------------------------------------------
namespace tsf4g_tdr {

#ifndef TSF4G_TDR_TDRERROR_DECLARED
#define TSF4G_TDR_TDRERROR_DECLARED
class TdrError {
public:
    enum ErrorType {
        TDR_NO_ERROR = 0,
        TDR_ERR_SHORT_BUF_FOR_WRITE = -1,
        TDR_ERR_SHORT_BUF_FOR_READ = -2,
        TDR_ERR_STR_LEN_TOO_BIG = -3,
        TDR_ERR_STR_LEN_TOO_SMALL = -4,
        TDR_ERR_STR_LEN_CONFLICT = -5,
        TDR_ERR_MINUS_REFER_VALUE = -6,
        TDR_ERR_REFER_SURPASS_COUNT = -7,
        TDR_ERR_ARG_POINTER_IS_NULL = -8,
        TDR_ERR_CUTVER_TOO_SMALL = -9,
        TDR_ERR_CUTVER_CONFILICT = -10,
        TDR_ERR_PARSE_TDRIP_FAILED = -11,
        TDR_ERR_INVALID_TDRIP_VALUE = -12,
        TDR_ERR_INVALID_TDRTIME_VALUE = -13,
        TDR_ERR_INVALID_TDRDATE_VALUE = -14,
        TDR_ERR_INVALID_TDRDATETIME_VALUE = -15,
        TDR_ERR_FUNC_LOCALTIME_FAILED = -16,
        TDR_ERR_INVALID_HEX_STR_LEN = -17,
        TDR_ERR_INVALID_HEX_STR_FORMAT = -18,
        TDR_ERR_NULL_POINTER_PARAMETER = -19,
        TDR_ERR_NET_CUTVER_INVALID = -20,
        TDR_ERR_VALUE_UNDER_OR_OVER_FLOW = -21,
        TDR_ERR_OPEN_FILE_READ_FAILED = -22,
        TDR_ERR_OPEN_FILE_WRITE_FAILED = -23,
        TDR_ERR_READ_FILE_FAILED = -24,
        TDR_ERR_WRITE_FILE_FAILED = -25,
        TDR_ERR_ALLOC_MEMORY_FAILED = -26,
        TDR_ERR_PARSE_XML_FAILED = -27,
        TDR_ERR_INVALID_ROOT_NODE = -28,
        TDR_ERR_FAILED_TO_PARSE_NUMERIC = -29,
        TDR_ERR_UNDEFINED_MACRO_NAME = -30
    };
};
#endif

class TdrWriteBuf;
class TdrReadBuf;
// The protocol TdrXml.h (pulled in by comm_svrd_config.h) declares the same
// enum without an inner guard; skip ours when that header is already in.
#if !defined(TSF4G_TDR_TDRXMLFORMAT_DECLARED) && \
    !defined(SECSVR_SRC_PROTOCOL_COMMON_TDRXML_H_H_)
#define TSF4G_TDR_TDRXMLFORMAT_DECLARED
// ORIG DWARF: LIST_ENTRY = 0, ATTR_ENTRY = 1.
enum TdrXmlFormat { LIST_ENTRY = 0, ATTR_ENTRY = 1 };
#endif
}

namespace conf_cfgsdk {

#pragma pack(push, 1)

struct DeployInfo {
    uint32_t game_id_;
    uint32_t idc_no_;
    uint16_t world_;
};

// sizeof 0x9a
struct cfgsdk_config {
    uint16_t slave_cfgsvr_type_;    // +0x00
    uint32_t slave_cfgsvr_id_;      // +0x02
    char slave_cfgsvr_ip_[32];      // +0x06
    uint16_t slave_cfgsvr_port_;    // +0x26
    uint32_t game_id_;              // +0x28
    uint32_t idc_no_;               // +0x2c
    uint16_t world_;                // +0x30
    uint32_t deploy_info_num_;      // +0x32
    DeployInfo deploy_info_[10];    // +0x36

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
};

#pragma pack(pop)

} // namespace conf_cfgsdk

// ---------------------------------------------------------------------------
// sec_proto messages used by this CU (TDR-generated, pack(1)).
// ---------------------------------------------------------------------------
namespace sec_proto {

#pragma pack(push, 1)

#define TSS_RECON_MSG(name)                                                  \
    struct name {                                                            \
        tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,       \
                                            size_t *usedSize,                \
                                            unsigned int cutVer) const;      \
        tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,\
                                              size_t *usedSize,              \
                                              unsigned int cutVer);          \
    }

struct SvcIpInfoReq {
    uint16_t svc_type_;
    uint32_t svc_id_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct SvcIdReq {
    uint16_t svc_type_;
    uint16_t svc_index_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct DeployInfoReq {
    uint16_t svc_type_;
    uint32_t svc_id_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct ConfigBaseInfo {
    uint16_t svc_type_;
    uint32_t svc_id_;
    char cfg_name_[128];
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct ConfigMD5Info {
    char cfg_name_[128];
    unsigned char cfg_md5_[16];
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct CfgFileReq {
    ConfigBaseInfo cfg_base_info_;
    unsigned char cfg_md5_[16];
    uint32_t recv_len_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct CheckCfgFileUpdateReq {
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t cfg_num_;
    ConfigMD5Info cfg_md5_info_[2048];
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct SvcIpInfoRsp {
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t bind_ip_;
    uint16_t bind_port_;
    uint32_t idc_no_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct SvcIdRsp {
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t game_id_;
    uint32_t idc_no_;
    uint32_t world_id_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct CfgsvrInfoRsp {
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t bind_ip_;
    uint16_t bind_port_;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct CfgFileRsp {
    int32_t result_;
    ConfigBaseInfo cfg_base_info_;
    uint32_t cfg_len_;
    unsigned char cfg_md5_[16];
    uint32_t data_len_;
    char data_buf_[64000];
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct CheckCfgFileUpdateRsp {
    uint32_t cfg_num_;
    char cfg_name_[2048 * 128];
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

struct DeployInfoRsp {
    int32_t result_;
    uint16_t svc_type_;
    uint32_t svc_id_;
    uint32_t deploy_info_num_;
    conf_cfgsdk::DeployInfo deploy_info_[10];
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size,
                                        size_t *usedSize,
                                        unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size,
                                          size_t *usedSize,
                                          unsigned int cutVer);
};

#pragma pack(pop)

} // namespace sec_proto

// ---------------------------------------------------------------------------
// Zulu_SendRecv_Package (comm_sndrcv_zulu.cpp + header-inline templates).
// ---------------------------------------------------------------------------
struct Tibetan_SendRecv_Package {
    void ***vptr_;                       // +0x00
    SERVICES_ID tibetan_recv_service_;   // +0x04
    SERVICES_ID tibetan_send_service_;   // +0x0a
    SERVICES_ID tibetan_proxy_service_;  // +0x10
    size_t test_frame_len_;              // +0x18
    unsigned int trans_id_builder_;      // +0x1c
    Comm_App_Frame *tibetan_send_appframe_;  // +0x20
    Comm_App_Frame *tibetan_recv_appframe_;  // +0x24
    unsigned int recv_trans_id_;         // +0x28

    Tibetan_SendRecv_Package();
    virtual ~Tibetan_SendRecv_Package();
};

struct Zulu_SendRecv_Package : public Tibetan_SendRecv_Package {
protected:
    bool zulu_connected_;         // +0x2c
    ZEN_Sockaddr_In zulu_svc_ip_;  // +0x30
    ZEN_Socket_Stream zulu_stream_; // +0x4c

public:
    Zulu_SendRecv_Package();
    virtual ~Zulu_SendRecv_Package();

    int set_zulu_svcinfo(const char *svc_ip, unsigned short svc_port,
                         const SERVICES_ID &recv_service,
                         const SERVICES_ID &send_service,
                         const SERVICES_ID &proxy_service, size_t frame_len);
    int connect_zulu_server(ZEN_Time_Value *time_wait);
    void close();
    int receive_svc_package(ZEN_Time_Value *time_wait);
    int send_svc_package(ZEN_Time_Value *time_wait);

    // ORIG keeps these instantiations out of line (each call site is a PLT
    // call to the weak template copy); noinline reproduces that code shape.
    template <typename T>
    __attribute__((noinline))
    int send_svc_package(unsigned int qquin, unsigned int cmd, const T &info,
                         ZEN_Time_Value *time_wait, unsigned int app_id,
                         unsigned int backfill_trans_id) {
        int ret = 0;
        if (!zulu_connected_) {
            ret = connect_zulu_server(time_wait);
            if (ret != 0) {
                return ret;
            }
        }
        tibetan_send_appframe_->frame_command_ = cmd;
        tibetan_send_appframe_->backfill_trans_id_ = backfill_trans_id;
        tibetan_send_appframe_->app_id_ = app_id;
        tibetan_send_appframe_->frame_uin_ = qquin;
        ret = tibetan_send_appframe_->appdata_encode<T>(0xffbd, info, 0, 0);
        if (ret != 0) {
            return ret;
        }
        ret = send_svc_package(time_wait);
        if (ret != 0) {
            zulu_stream_.close();
            zulu_connected_ = false;
        }
        return ret;
    }

    template <typename T>
    __attribute__((noinline))
    int receive_svc_package(unsigned int cmd, T &info, bool if_loop,
                            ZEN_Time_Value *time_out) {
        int ret = 0;
        for (;;) {
            ret = receive_svc_package(time_out);
            if (ret != 0) {
                zulu_stream_.close();
                zulu_connected_ = false;
                return ret;
            }
            unsigned int recv_cmd = tibetan_recv_appframe_->frame_command_;
            if (recv_cmd != cmd) {
                if (if_loop) {
                    continue;
                }
                ZEN_Trace_LogMsg::debug_errorex(
                    "[framework] recv a error or unexpect frame,expect cmd =%u,"
                    "recv cmd =%u.",
                    cmd, recv_cmd);
                zulu_stream_.close();
                zulu_connected_ = false;
                return 0x13e7;
            }
            if (tibetan_recv_appframe_->frame_length_ > 0x32) {
                ret = tibetan_recv_appframe_->appdata_decode<T>(info, 0, 0);
            }
            return ret;
        }
    }
};


// ---------------------------------------------------------------------------
// CfgSvrSdk
// ---------------------------------------------------------------------------
struct CfgSvrSdk : public Zen_Thread_Task {
public:
    struct CFG_MD5 {
        unsigned char md5_value_[16];
    };

    enum GET_CONFIG_RESULT {
        CONFIG_IS_NEWEST = 1,
        CONFIG_IS_GET_OVER = 2,
        CONFIG_NEED_GET_MORE = 3,
    };

private:
    static const unsigned int MAX_FILE_LENGTH = 52428800;
    static const unsigned short MASTER_CFGSVR_PORT = 9000;
    static const unsigned int MASTER_CFGSVR_ID = 1;
    static const int CHECK_UPDATE_TIME = 30;
    static CfgSvrSdk *instance_;

    SERVICES_ID self_info_;                  // 0x10
    SERVICES_ID master_cfgsvr_info_;         // 0x16
    SERVICES_ID slave_cfgsvr_info_;          // 0x1c
    unsigned short svc_index_;               // 0x22
    ZEN_Sockaddr_In master_cfgsvr_ip_;       // 0x24
    ZEN_Sockaddr_In slave_cfgsvr_ip_;        // 0x40
    conf_cfgsdk::cfgsdk_config cfgsdk_config_; // 0x5c (154)
    std::vector<std::string> cfg_name_list_;       // 0xf8
    std::vector<std::string> config_update_list_;   // 0x104
    unsigned char *file_buf_;                // 0x110
    unsigned int recv_len_;                  // 0x114
    std::string curr_cfg_name_;              // 0x118
    CFG_MD5 local_cfg_md5_;                  // 0x11c
    CFG_MD5 recv_cfg_md5_;                   // 0x12c
    std::map<std::string, CFG_MD5> cfg_md5_map_; // 0x13c
    bool is_pthread_start_;                  // 0x154
    bool is_updade_config_;                  // 0x155

public:
    CfgSvrSdk();
    virtual ~CfgSvrSdk();
    static CfgSvrSdk *instance();
    static void clean_instance();
    void add_config(const char *cfg_name);
    void set_service_type(unsigned short svc_type);
    int init();
    unsigned int get_game_id();
    int get_idc_no();
    unsigned short get_world_id();
    void get_deploy_world_list(std::vector<unsigned short> &world_list);
    int get_ip_info(const SERVICES_ID &req_info, ZEN_Sockaddr_In &svr_inetadd,
                    unsigned int &svr_idc_no);
    int start_task();
    bool is_need_update_file();
    void clear_file_update_flags();
    int get_slave_cfgsvr_info();
    int init_zulu_ex(Zulu_SendRecv_Package &zulu_ex,
                     const ZEN_Sockaddr_In &ip_info,
                     const SERVICES_ID &recv_info);
    int get_svc_id();
    int get_svc_deploy_info();
    int get_config_file(Zulu_SendRecv_Package &zulu_ex, const char *cfg_name);
    int get_config_file();
    void insert_config_md5(const std::string &cfg_name, const CFG_MD5 &cfg_md5);
    void caculate_config_md5();
    void get_config_md5(const std::string &cfg_name, CFG_MD5 &cfg_md5);
    int send_config_file_req(Zulu_SendRecv_Package &zulu_ex);
    int recv_config_file_rsp(Zulu_SendRecv_Package &zulu_ex,
                             GET_CONFIG_RESULT &get_config_result);
    int read_local_file(int &length);
    bool check_config_md5();
    int write_local_file();
    int uncompress_file(const char *cfg_name);
    int get_config_update_list(Zulu_SendRecv_Package &zulu_ex);
    int update_config_file(Zulu_SendRecv_Package &zulu_ex);
    void update_config_name_list();

protected:
    virtual int svc();
};

#endif // TSS_RECON_COMM_CFGSVR_SDK_H_
