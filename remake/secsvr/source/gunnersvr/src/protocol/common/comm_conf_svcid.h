// Restored from DWARF + disassembly (gunnersvr oracle), semantics verified.
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_SVCID_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_SVCID_H_H_

#include <stdint.h>
#include <stdio.h>

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrXml.h"

namespace conf_svcid {

#pragma pack(push, 1)

// sizeof 0x3c = 60 (packed)
struct ServiceInfo {
    uint16_t svr_type_;   // +0
    uint32_t svr_id_;     // +2
    char svr_ip_[16];     // +6
    uint16_t svr_port_;   // +0x16
    uint32_t svr_idc_;    // +0x18
    char svr_info_[32];   // +0x1c

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

// sizeof 4 + 2048*0x3c = 0x1e004 = 122884
struct svcid_config {
    uint32_t service_num_;               // +0
    ServiceInfo service_info_[2048];     // +4

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

} // namespace conf_svcid

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_SVCID_H_H_
