// Single shared layout of the process-wide `g_config` (sizeof 1515) for the
// reconstructed plugin translation units.
//
// The ORIG declaration is the TDR-generated
// src/protocol/formmog/formmog_conf_tsssdk_conf.h (namespace
// tss_sdk_conf_desc).  That header pulls the rapidxml/TDR XML runtime and
// cannot be co-included with src/commlib/framework/comm_cfgsvr_sdk.h, whose
// TdrXmlFormat guard it does not share.  The plugin TUs need both, so the
// data-member layout is declared once here, with the exact ORIG member names,
// order and packing:
//   AddrInfo            sizeof   38 : int32 / char[32] / uint16
//   CommConfig          sizeof 1226 : int32 / int32 / uint16 / AddrInfo[32]
//   LogConfig           sizeof    4 : int32
//   UserControlconfig   sizeof    8 : int32 / int32
//   UicConfig           sizeof  277 : see below (DWARF offsets in comments)
//   config              sizeof 1515 : CommConfig(0) LogConfig(1226)
//                                      UserControlconfig(1230) UicConfig(1238)
//
// g_config itself is defined by tss_sdk_framework_lifecycle_agent.cpp as
// tss_sdk_conf_desc::config; both declarations describe the same unmangled
// symbol and the same 1515 bytes.  No TDR member functions are repeated here.
#ifndef TSS_SDK_RECON_PLUGINS_TSS_SDK_CONFIG_SHARED_H_
#define TSS_SDK_RECON_PLUGINS_TSS_SDK_CONFIG_SHARED_H_

#include <stdint.h>

#include "src/formmog/tss_sdk/layout/layout_contract.h"

struct TssSdkServerAddrInfo {
    int32_t plugin_id_;          // +0
    char ip_[32];                // +4
    uint16_t port_;              // +36
} __attribute__((packed));

struct TssSdkCommConfig {
    int32_t use_ipc_;                        // +0
    int32_t use_socket_;                     // +4
    uint16_t server_num_;                    // +8
    TssSdkServerAddrInfo server_addrs_info_[32];  // +10
} __attribute__((packed));

struct TssSdkUserControlConfig {
    int32_t use_idx_;                  // +0
    int32_t max_client_obj_count_;     // +4
};

struct TssSdkUicConfig {
    int32_t use_backend_check_;          // +0
    int8_t user_input_code_;             // +4
    char master_tss_uic_ip_[32];         // +5
    int32_t master_tss_uic_port_;        // +37
    char slave_tss_uic_ip_[32];          // +41
    int32_t slave_tss_uic_port_;         // +73
    int32_t max_var_len_queue_len_;      // +77
    int32_t max_fix_len_queue_len_;      // +81
    int32_t use_dirty_file_flag_;        // +85
    char dirty_file_[128];               // +89
    uint32_t bus_id_;                    // +217
    int32_t queue_timeout_;              // +221
    int32_t connect_timeout_;            // +225
    int32_t connect_intval_;             // +229
    int32_t recv_timeout_;               // +233
    int32_t send_timeout_;               // +237
    int32_t max_recv_count_onetime_;     // +241
    int32_t max_send_count_onetime_;     // +245
    int32_t max_recv_time_gap_onetime_;  // +249
    int32_t max_send_time_gap_onetime_;  // +253
    int32_t stop_timeout_;               // +257
    int32_t tcp_max_recv_buf_len_;       // +261
    int32_t tcp_max_send_buf_len_;       // +265
    uint32_t send_intval_;               // +269
    uint32_t recv_intval_;               // +273
} __attribute__((packed));

struct TssSdkConfig {
    TssSdkCommConfig comm_config_;              // +0
    int32_t log_config_;                       // +1226
    TssSdkUserControlConfig usercontrol_config_;  // +1230
    TssSdkUicConfig uic_config_;               // +1238, ends at 1515
} __attribute__((packed));

extern TssSdkConfig g_config;

TSS_SDK_STATIC_ASSERT(sizeof(TssSdkServerAddrInfo) == 38,
                      tss_sdk_server_addr_info_size_must_be_38);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkCommConfig) == 1226,
                      tss_sdk_comm_config_size_must_be_1226);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkUicConfig) == 277,
                      tss_sdk_uic_config_size_must_be_277);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkConfig) == 1515,
                      tss_sdk_config_size_must_be_1515);

#endif  // TSS_SDK_RECON_PLUGINS_TSS_SDK_CONFIG_SHARED_H_
