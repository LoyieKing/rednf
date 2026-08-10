// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRTYPEUTIL_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRTYPEUTIL_H_H_

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include <stdint.h>

namespace tsf4g_tdr {

class TdrWriteBuf;

class TdrTypeUtil {
public:
    static TdrError::ErrorType str2TdrDate(tdr_date_t &date, const char *str);
    static TdrError::ErrorType str2TdrTime(tdr_time_t &time, const char *str);
    static TdrError::ErrorType str2TdrDateTime(tdr_datetime_t &datetime, const char *str);
    static TdrError::ErrorType str2TdrIP(tdr_ip_t &ip, const char *str);
    static TdrError::ErrorType UTC2TdrDateTime(tdr_datetime_t &datetime, long utc);
    static TdrError::ErrorType tdrDateTime2UTC(long &utc, tdr_datetime_t datetime);
    static TdrError::ErrorType tdrIp2Str(char *dest, unsigned int size, tdr_ip_t src);
    static TdrError::ErrorType tdrIp2Str(TdrWriteBuf &buf, tdr_ip_t ip);
    static unsigned int wstrlen(const tdr_wchar_t *str);
    static int compareTdrDate(tdr_date_t a, tdr_date_t b);
    static int compareTdrTime(tdr_time_t a, tdr_time_t b);
    static int compareTdrDateTime(tdr_datetime_t a, tdr_datetime_t b);
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRTYPEUTIL_H_H_
