// Reconstructed from gunnersvr DWARF + disassembly (2026-08-10).
// sizeof = 344：继承 Zen_Thread_Task(16)，成员自 +0x10 起，偏移已核对。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_CFGSVR_SDK_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_CFGSVR_SDK_H_H_

#include "output/protocol/common/release/include/comm_conf_cfgsdk.h"
#include "output/commlib/zenlib/release/include/zen_thread_task.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "src/commlib/framework/comm_service_info.h"
#include <map>
#include <string>
#include <vector>

struct Zulu_SendRecv_Package;

// sizeof = 344
struct CfgSvrSdk : public Zen_Thread_Task { // line 14
public:
    // sizeof = 16
    struct CFG_MD5 { // line 21
        unsigned char md5_value_[16];
    };

    enum GET_CONFIG_RESULT { // line 25
        CONFIG_IS_NEWEST = 1,
        CONFIG_IS_GET_OVER = 2,
        CONFIG_NEED_GET_MORE = 3,
    };

private:
    static const unsigned int MAX_FILE_LENGTH = 52428800; // line 39
    static const unsigned short MASTER_CFGSVR_PORT = 9000; // line 42
    static const unsigned int MASTER_CFGSVR_ID = 1; // line 45
    static const int CHECK_UPDATE_TIME = 30; // line 48
    static CfgSvrSdk *instance_; // line 19

    SERVICES_ID self_info_;                  // 0x10
    SERVICES_ID master_cfgsvr_info_;         // 0x16
    SERVICES_ID slave_cfgsvr_info_;          // 0x1c
    unsigned short svc_index_;               // 0x22
    ZEN_Sockaddr_In master_cfgsvr_ip_;       // 0x24
    ZEN_Sockaddr_In slave_cfgsvr_ip_;        // 0x40
    conf_cfgsdk::cfgsdk_config cfgsdk_config_; // 0x5c (154)
    std::vector<std::string> cfg_name_list_;       // 0xf8
    std::vector<std::string> config_update_list_;  // 0x104
    unsigned char *file_buf_;                // 0x110
    unsigned int recv_len_;                  // 0x114
    std::string curr_cfg_name_;              // 0x118
    CFG_MD5 local_cfg_md5_;                  // 0x11c
    CFG_MD5 recv_cfg_md5_;                   // 0x12c
    std::map<std::string, CFG_MD5> cfg_md5_map_; // 0x13c
    bool is_pthread_start_;                  // 0x154
    bool is_updade_config_;                  // 0x155

public:
    CfgSvrSdk(); // line 17
    virtual ~CfgSvrSdk(); // line 18
    static CfgSvrSdk * instance(); // line 20
    static void clean_instance(); // line 22
    void add_config(const char *cfg_name); // line 71
    void set_service_type(unsigned short svc_type); // line 82
    int init(); // line 23
    unsigned int get_game_id(); // line 86
    int get_idc_no(); // line 91
    unsigned short get_world_id(); // line 96
    void get_deploy_world_list(std::vector<unsigned short> &world_list); // line 101
    int get_ip_info(const SERVICES_ID &req_info, ZEN_Sockaddr_In &svr_inetadd,
                    unsigned int &svr_idc_no); // line 109
    int start_task(); // line 117
    bool is_need_update_file(); // line 52
    void clear_file_update_flags(); // line 58
    int get_slave_cfgsvr_info(); // line 124
    int init_zulu_ex(Zulu_SendRecv_Package &zulu_ex, const ZEN_Sockaddr_In &ip_info,
                     const SERVICES_ID &recv_info); // line 145
    int get_svc_id(); // line 133
    int get_svc_deploy_info(); // line 139
    int get_config_file(Zulu_SendRecv_Package &zulu_ex, const char *cfg_name); // line 155
    int get_config_file(); // line 181
    void insert_config_md5(const std::string &cfg_name, const CFG_MD5 &cfg_md5); // line 192
    void caculate_config_md5(); // line 212
    void get_config_md5(const std::string &cfg_name, CFG_MD5 &cfg_md5); // line 221
    int send_config_file_req(Zulu_SendRecv_Package &zulu_ex); // line 242
    int recv_config_file_rsp(Zulu_SendRecv_Package &zulu_ex,
                             GET_CONFIG_RESULT &get_config_result); // line 265
    int read_local_file(int &length); // line 296
    bool check_config_md5(); // line 306
    int write_local_file(); // line 319
    int uncompress_file(const char *cfg_name); // line 336
    int get_config_update_list(Zulu_SendRecv_Package &zulu_ex); // line 357
    int update_config_file(Zulu_SendRecv_Package &zulu_ex); // line 383
    void update_config_name_list(); // line 412

protected:
    virtual int svc(); // line 63
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_CFGSVR_SDK_H_H_
