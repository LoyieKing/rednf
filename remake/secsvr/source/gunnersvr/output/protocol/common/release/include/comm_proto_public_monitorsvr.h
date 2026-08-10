// Reconstructed from gunnersvr disassembly (DWARF offsets + mnemonic check).
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_MONITORSVR_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_MONITORSVR_H_H_

#include <stddef.h>
#include <stdint.h>

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBuf.h"

namespace sec_proto {

#pragma pack(push, 1)

class MonitorItem {
public:
    uint32_t feature_id_;     // sizeof 20
    uint32_t app_id_;
    uint32_t classify_id_;
    uint64_t feature_value_;

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf &destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf &srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size, size_t *usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const;
    const char * visualize_ex(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const;
    const char * visualize_ex(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const;
};

class MonitorSvrReport {
public:
    uint16_t version_;      // sizeof 40980
    uint32_t report_time_;
    uint32_t service_type_;
    uint32_t service_id_;
    uint32_t send_ip_;
    uint16_t count_;
    MonitorItem item_[2048];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf &destBuf, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf &srcBuf, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size, size_t *usedSize, unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const;
    tsf4g_tdr::TdrError::ErrorType visualize(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const;
    const char * visualize_ex(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const;
    const char * visualize_ex(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const;
};

#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_MONITORSVR_H_H_
