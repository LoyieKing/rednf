// Reconstructed from gunnersvr disassembly (DWARF offsets + mnemonic check).
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_LOGSVR_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_LOGSVR_H_H_

#include <stddef.h>

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBuf.h"

namespace sec_proto {

class LogSvrCommBillRecord {
public:
    char string_buffer_[2048]; // sizeof 2048

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

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_PROTO_PUBLIC_LOGSVR_H_H_
