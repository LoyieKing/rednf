// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRBUFUTIL_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRBUFUTIL_H_H_

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include <stdarg.h>
#include <stdint.h>

namespace tsf4g_tdr {

class TdrWriteBuf;

class TdrBufUtil {
public:
    static TdrError::ErrorType printMultiStr(TdrWriteBuf &buf, const char *str, int count);
    static TdrError::ErrorType printVariable(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, const char *format, ...);
    static TdrError::ErrorType printVariable(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, bool value);
    static TdrError::ErrorType printVariable(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, unsigned int arrIdx,
                                             const char *format, ...);
    static TdrError::ErrorType printVariable(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, unsigned int arrIdx, bool value);
    static TdrError::ErrorType printString(TdrWriteBuf &buf, int indent, char sep,
                                           const char *variable, const char *str);
    static TdrError::ErrorType printString(TdrWriteBuf &buf, int indent, char sep,
                                           const char *variable, unsigned int arrIdx, const char *str);
    static TdrError::ErrorType printWString(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, const tdr_wchar_t *str);
    static TdrError::ErrorType printWString(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, unsigned int arrIdx, const tdr_wchar_t *str);
    static TdrError::ErrorType printArray(TdrWriteBuf &buf, int indent, char sep,
                                          const char *variable, int64_t count);
    static TdrError::ErrorType printTdrDate(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, tdr_date_t date);
    static TdrError::ErrorType printTdrDate(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, unsigned int arrIdx, tdr_date_t date);
    static TdrError::ErrorType printTdrTime(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, tdr_time_t time);
    static TdrError::ErrorType printTdrTime(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, unsigned int arrIdx, tdr_time_t time);
    static TdrError::ErrorType printTdrDateTime(TdrWriteBuf &buf, int indent, char sep,
                                                const char *variable, tdr_datetime_t datetime);
    static TdrError::ErrorType printTdrDateTime(TdrWriteBuf &buf, int indent, char sep,
                                                const char *variable, unsigned int arrIdx,
                                                tdr_datetime_t datetime);
    static TdrError::ErrorType printTdrIP(TdrWriteBuf &buf, int indent, char sep,
                                          const char *variable, tdr_ip_t ip);
    static TdrError::ErrorType printTdrIP(TdrWriteBuf &buf, int indent, char sep,
                                          const char *variable, unsigned int arrIdx, tdr_ip_t ip);
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRBUFUTIL_H_H_
