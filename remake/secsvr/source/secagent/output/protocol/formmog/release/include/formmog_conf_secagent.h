// Reconstructed from secagent disassembly (TDR-generated config structures)
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_SECAGENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_SECAGENT_H_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrXml.h"

namespace tsf4g_tdr {
class TdrXmlReader;
class TdrXmlWriter;
}

namespace secagent_config {

#pragma pack(push, 1)


class SanlixChecker {
public:
    int32_t is_use_;
    int32_t is_distribute_tenparty_data_;
    char tp_data_path_[256];
    int32_t is_distribute_ts_data_;
    char ts_data_path_[256];
    int32_t is_distribute_te_data_;
    char te_data_path_[256];
    int32_t is_distribute_tk_data_;
    char tk_data_path_[256];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class CmdListInfo {
public:
    uint32_t cmd_list_count_;
    uint16_t cmd_[30];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class CryptCheckerConf {
public:
    int32_t is_use_;
    int32_t crypt_test_gap_;
    CmdListInfo up_cmdlist_info_;
    CmdListInfo down_cmdlist_info_;
    char dib_base_path_[256];
    char exe_path_[256];
    char exe_version_path_[256];
    uint32_t antibot_error_thread_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

class config {
public:
    uint8_t is_for_mmog_;
    uint8_t if_restore_gmsvr_channel_;
    uint8_t if_check_pthread_;
    uint8_t if_restore_dpsdk_channel_;
    uint8_t if_restore_user_info_;
    uint32_t log_priority_;
    uint32_t expire_time_;
    uint32_t table_node_num_;
    char dp_config_path_[256];
    uint32_t report_statistic_time_;
    char restrict_file_[256];
    CryptCheckerConf crypt_checker_conf_;
    uint32_t max_package_size_;
    uint32_t if_rpcode_with_key_;
    char rpcode_keyinfo_key_[64];
    uint8_t if_open_cs_channel_;
    SanlixChecker sanlix_checker_conf_;
    uint32_t total_max_send_count_;
    uint8_t if_check_rpcode_hash_;
    uint32_t gamesvr_channel_num_;
    uint8_t channel_recv_times_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

#pragma pack(pop)

} // namespace secagent_config

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_SECAGENT_H_H_
