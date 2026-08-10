// Reconstructed from gunnersvr disassembly (conf_zerg config structures)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_ZERG_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_ZERG_H_H_

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrXml.h"
#include <stdint.h>
#include <stdio.h>

namespace conf_zerg {

struct SvrInfo {
    uint16_t svr_type;
    uint32_t svr_id;
    uint8_t use_encrypt;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct SelfCfg {
    SvrInfo self_svr_info;
    uint8_t slave_svr_count;
    SvrInfo slave_svrs[3];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct LogCfg {
    char log_level[32];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct CommCfg {
    uint32_t max_frame_len;
    uint32_t accept_send_buf_size;
    uint32_t connect_send_deque_size;
    uint32_t connect_timeout;
    uint32_t recv_timeout;
    uint8_t is_proxy;
    uint32_t max_accept_svr;
    uint8_t is_lock_pipe;
    uint8_t retry_error;
    uint32_t recv_pipe_len;
    uint32_t send_pipe_len;
    uint32_t error_pipe_len;
    uint8_t check_pthread;
    uint8_t insurance;
    char opt_key1[128];
    char opt_key2[128];
    char get_svr_info_type[64];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct CfgSvrCfg {
    SvrInfo svr_info;
    char svr_ip[16];
    uint16_t svr_port;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct ConsoleCfg {
    uint8_t use_console;
    char console_ip[16];
    uint16_t console_port;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct CheckCfg {
    uint8_t check_frame;
    uint8_t check_rcvid;
    uint8_t num_snd_type;
    uint32_t allow_snd_types[10];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct AutoConnectSvr {
    SvrInfo main_svr;
    SvrInfo backup_svr;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct AutoConnectCfg {
    uint32_t auto_connect_num;
    AutoConnectSvr auto_connect_svrs[100];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct RestrictCfg {
    char allow_ips[20480];
    char reject_ips[20480];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct MonitorCfg {
    uint32_t monitor_num;
    uint32_t monitor_cmds[100];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

struct zerg_config {
    SelfCfg self_cfg;
    LogCfg log_cfg;
    CommCfg comm_cfg;
    CfgSvrCfg cfgsvr_cfg;
    ConsoleCfg console_cfg;
    CheckCfg check_cfg;
    AutoConnectCfg auto_connect_cfg;
    RestrictCfg restrict_cfg;
    MonitorCfg monitor_cfg;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *str, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

} // namespace conf_zerg

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_ZERG_H_H_
