// Reconstructed from gunnersvr disassembly (TDR-generated config structures).
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_CFGSDK_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_CFGSDK_H_H_

#include <stdint.h>
#include <stdio.h>

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrXml.h"

namespace conf_cfgsdk {

#pragma pack(push, 1)

// sizeof 10 (packed)
struct DeployInfo {
    uint32_t game_id_;   // +0
    uint32_t idc_no_;    // +4
    uint16_t world_;     // +8

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

// sizeof 0x9a = 154 (packed)
struct cfgsdk_config {
    uint16_t slave_cfgsvr_type_;   // +0
    uint32_t slave_cfgsvr_id_;     // +2
    char slave_cfgsvr_ip_[32];     // +6
    uint16_t slave_cfgsvr_port_;   // +0x26
    uint32_t game_id_;             // +0x28
    uint32_t idc_no_;              // +0x2c
    uint16_t world_;               // +0x30
    uint32_t deploy_info_num_;     // +0x32
    DeployInfo deploy_info_[10];   // +0x36

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

} // namespace conf_cfgsdk

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_CFGSDK_H_H_
