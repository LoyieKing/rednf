// Private protocol header reconstructed for
// src/protocol/formmog/formmog_conf_tsssdk_conf.cpp of
// libtss_sdk.so.1.0.394 (tag 394).
//
// Layout source: the DWARF of the ORIG translation unit
// (cu_layout.py / dwarf_full_extract.py on CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_conf_tsssdk_conf.cpp
// ), not the generated header:
//   AddrInfo            sizeof   38 : 4 / char[32] / uint16
//   CommConfig          sizeof 1226 : 4 / 4 / uint16 / AddrInfo[32]
//   LogConfig           sizeof    4 : 4
//   UserControlconfig   sizeof    8 : 4 / 4
//   UicConfig           sizeof  277 : 4 / int8 / char[32] / 4 / char[32] / 4 /
//                                     9 x 4 / char[128] / uint32 / 14 x 4 /
//                                     2 x uint32
//   config              sizeof 1515 : CommConfig(0) LogConfig(1226)
//                                     UserControlconfig(1230) UicConfig(1238)
//
// The members are byte-packed (#pragma pack(1)): DWARF puts
// CommConfig::server_addrs_info_ at offset 10 and config::log_config_ at
// offset 1226, which only hold with packing - the same packing the
// TDR-generated configuration headers of this generator family use.
//
// The 12 member functions per class are declared in the ORIG's declaration
// order (DW_AT_decl_line of each DW_TAG_subprogram of the class DIE).
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_H_H_

#include <stdint.h>

#include "tsf4g_tdr_compat_cfg_conf.h"

namespace tss_sdk_conf_desc {

#pragma pack(push, 1)

class AddrInfo {
public:
    int32_t plugin_id_;
    char ip_[32];
    uint16_t port_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class CommConfig {
public:
    int32_t use_ipc_;
    int32_t use_socket_;
    uint16_t server_num_;
    AddrInfo server_addrs_info_[32];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class LogConfig {
public:
    int32_t log_level_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class UserControlconfig {
public:
    int32_t use_idx_;
    int32_t max_client_obj_count_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class UicConfig {
public:
    int32_t use_backend_check_;
    int8_t user_input_code_;
    char master_tss_uic_ip_[32];
    int32_t master_tss_uic_port_;
    char slave_tss_uic_ip_[32];
    int32_t slave_tss_uic_port_;
    int32_t max_var_len_queue_len_;
    int32_t max_fix_len_queue_len_;
    int32_t use_dirty_file_flag_;
    char dirty_file_[128];
    uint32_t bus_id_;
    int32_t queue_timeout_;
    int32_t connect_timeout_;
    int32_t connect_intval_;
    int32_t recv_timeout_;
    int32_t send_timeout_;
    int32_t max_recv_count_onetime_;
    int32_t max_send_count_onetime_;
    int32_t max_recv_time_gap_onetime_;
    int32_t max_send_time_gap_onetime_;
    int32_t stop_timeout_;
    int32_t tcp_max_recv_buf_len_;
    int32_t tcp_max_send_buf_len_;
    uint32_t send_intval_;
    uint32_t recv_intval_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class config {
public:
    CommConfig comm_config_;
    LogConfig log_config_;
    UserControlconfig usercontrol_config_;
    UicConfig uic_config_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

#pragma pack(pop)

// Layout assertions: every value is a DW_AT_byte_size /
// DW_AT_data_member_location of the CU's DWARF.
typedef char AddrInfo_size_must_be_38[sizeof(AddrInfo) == 38 ? 1 : -1];
typedef char AddrInfo_plugin_id__offset_must_be_0[
    offsetof(AddrInfo, plugin_id_) == 0 ? 1 : -1];
typedef char AddrInfo_ip__offset_must_be_4[
    offsetof(AddrInfo, ip_) == 4 ? 1 : -1];
typedef char AddrInfo_port__offset_must_be_36[
    offsetof(AddrInfo, port_) == 36 ? 1 : -1];

typedef char CommConfig_size_must_be_1226[sizeof(CommConfig) == 1226 ? 1 : -1];
typedef char CommConfig_use_ipc__offset_must_be_0[
    offsetof(CommConfig, use_ipc_) == 0 ? 1 : -1];
typedef char CommConfig_use_socket__offset_must_be_4[
    offsetof(CommConfig, use_socket_) == 4 ? 1 : -1];
typedef char CommConfig_server_num__offset_must_be_8[
    offsetof(CommConfig, server_num_) == 8 ? 1 : -1];
typedef char CommConfig_server_addrs_info__offset_must_be_10[
    offsetof(CommConfig, server_addrs_info_) == 10 ? 1 : -1];

typedef char LogConfig_size_must_be_4[sizeof(LogConfig) == 4 ? 1 : -1];
typedef char LogConfig_log_level__offset_must_be_0[
    offsetof(LogConfig, log_level_) == 0 ? 1 : -1];

typedef char UserControlconfig_size_must_be_8[
    sizeof(UserControlconfig) == 8 ? 1 : -1];
typedef char UserControlconfig_use_idx__offset_must_be_0[
    offsetof(UserControlconfig, use_idx_) == 0 ? 1 : -1];
typedef char UserControlconfig_max_client_obj_count__offset_must_be_4[
    offsetof(UserControlconfig, max_client_obj_count_) == 4 ? 1 : -1];

typedef char UicConfig_size_must_be_277[sizeof(UicConfig) == 277 ? 1 : -1];
typedef char UicConfig_use_backend_check__offset_must_be_0[
    offsetof(UicConfig, use_backend_check_) == 0 ? 1 : -1];
typedef char UicConfig_user_input_code__offset_must_be_4[
    offsetof(UicConfig, user_input_code_) == 4 ? 1 : -1];
typedef char UicConfig_master_tss_uic_ip__offset_must_be_5[
    offsetof(UicConfig, master_tss_uic_ip_) == 5 ? 1 : -1];
typedef char UicConfig_master_tss_uic_port__offset_must_be_37[
    offsetof(UicConfig, master_tss_uic_port_) == 37 ? 1 : -1];
typedef char UicConfig_slave_tss_uic_ip__offset_must_be_41[
    offsetof(UicConfig, slave_tss_uic_ip_) == 41 ? 1 : -1];
typedef char UicConfig_slave_tss_uic_port__offset_must_be_73[
    offsetof(UicConfig, slave_tss_uic_port_) == 73 ? 1 : -1];
typedef char UicConfig_max_var_len_queue_len__offset_must_be_77[
    offsetof(UicConfig, max_var_len_queue_len_) == 77 ? 1 : -1];
typedef char UicConfig_max_fix_len_queue_len__offset_must_be_81[
    offsetof(UicConfig, max_fix_len_queue_len_) == 81 ? 1 : -1];
typedef char UicConfig_use_dirty_file_flag__offset_must_be_85[
    offsetof(UicConfig, use_dirty_file_flag_) == 85 ? 1 : -1];
typedef char UicConfig_dirty_file__offset_must_be_89[
    offsetof(UicConfig, dirty_file_) == 89 ? 1 : -1];
typedef char UicConfig_bus_id__offset_must_be_217[
    offsetof(UicConfig, bus_id_) == 217 ? 1 : -1];
typedef char UicConfig_queue_timeout__offset_must_be_221[
    offsetof(UicConfig, queue_timeout_) == 221 ? 1 : -1];
typedef char UicConfig_connect_timeout__offset_must_be_225[
    offsetof(UicConfig, connect_timeout_) == 225 ? 1 : -1];
typedef char UicConfig_connect_intval__offset_must_be_229[
    offsetof(UicConfig, connect_intval_) == 229 ? 1 : -1];
typedef char UicConfig_recv_timeout__offset_must_be_233[
    offsetof(UicConfig, recv_timeout_) == 233 ? 1 : -1];
typedef char UicConfig_send_timeout__offset_must_be_237[
    offsetof(UicConfig, send_timeout_) == 237 ? 1 : -1];
typedef char UicConfig_max_recv_count_onetime__offset_must_be_241[
    offsetof(UicConfig, max_recv_count_onetime_) == 241 ? 1 : -1];
typedef char UicConfig_max_send_count_onetime__offset_must_be_245[
    offsetof(UicConfig, max_send_count_onetime_) == 245 ? 1 : -1];
typedef char UicConfig_max_recv_time_gap_onetime__offset_must_be_249[
    offsetof(UicConfig, max_recv_time_gap_onetime_) == 249 ? 1 : -1];
typedef char UicConfig_max_send_time_gap_onetime__offset_must_be_253[
    offsetof(UicConfig, max_send_time_gap_onetime_) == 253 ? 1 : -1];
typedef char UicConfig_stop_timeout__offset_must_be_257[
    offsetof(UicConfig, stop_timeout_) == 257 ? 1 : -1];
typedef char UicConfig_tcp_max_recv_buf_len__offset_must_be_261[
    offsetof(UicConfig, tcp_max_recv_buf_len_) == 261 ? 1 : -1];
typedef char UicConfig_tcp_max_send_buf_len__offset_must_be_265[
    offsetof(UicConfig, tcp_max_send_buf_len_) == 265 ? 1 : -1];
typedef char UicConfig_send_intval__offset_must_be_269[
    offsetof(UicConfig, send_intval_) == 269 ? 1 : -1];
typedef char UicConfig_recv_intval__offset_must_be_273[
    offsetof(UicConfig, recv_intval_) == 273 ? 1 : -1];

typedef char config_size_must_be_1515[sizeof(config) == 1515 ? 1 : -1];
typedef char config_comm_config__offset_must_be_0[
    offsetof(config, comm_config_) == 0 ? 1 : -1];
typedef char config_log_config__offset_must_be_1226[
    offsetof(config, log_config_) == 1226 ? 1 : -1];
typedef char config_usercontrol_config__offset_must_be_1230[
    offsetof(config, usercontrol_config_) == 1230 ? 1 : -1];
typedef char config_uic_config__offset_must_be_1238[
    offsetof(config, uic_config_) == 1238 ? 1 : -1];

} // namespace tss_sdk_conf_desc

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_H_H_
