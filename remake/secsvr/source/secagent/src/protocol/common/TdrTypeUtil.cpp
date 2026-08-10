// Reconstructed from gunnersvr disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrTypeUtil.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrTime.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::tdrIp2Str(char *dest, unsigned int size, tdr_ip_t src) {
    struct in_addr addr;
    addr.s_addr = src;
    if (inet_ntop(AF_INET, &addr, dest, size) == NULL) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    dest[size - 1] = '\0';
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::tdrIp2Str(TdrWriteBuf &buf, tdr_ip_t src) {
    struct in_addr addr;
    char dotIP[32];
    addr.s_addr = src;
    if (inet_ntop(AF_INET, &addr, dotIP, sizeof(dotIP)) == NULL) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    dotIP[sizeof(dotIP) - 1] = '\0';
    return buf.textize("%s", dotIP);
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::UTC2TdrDateTime(tdr_datetime_t &dest, time_t utcTime) {
    struct tm stTm;
    memset(&stTm, 0, sizeof(stTm));
    struct tm *pstTm = localtime_r(&utcTime, &stTm);
    if (pstTm == NULL) {
        return TdrError::TDR_ERR_FUNC_LOCALTIME_FAILED;
    }

    TdrDateTime tdrDateTime;
    tdrDateTime.year = pstTm->tm_year + 1900;
    tdrDateTime.month = pstTm->tm_mon + 1;
    tdrDateTime.day = pstTm->tm_mday;
    tdrDateTime.hour = pstTm->tm_hour;
    tdrDateTime.minute = pstTm->tm_min;
    tdrDateTime.second = pstTm->tm_sec;
    TdrDate datePart;
    datePart.year = tdrDateTime.year;
    datePart.month = tdrDateTime.month;
    datePart.day = tdrDateTime.day;
    if (!datePart.isValid()) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }
    if (tdrDateTime.hour + 999 > 1998 || tdrDateTime.minute > 59 || tdrDateTime.second > 59) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }

    TdrDateTime tmp;
    tmp.year = tdrDateTime.year;
    tmp.month = tdrDateTime.month;
    tmp.day = tdrDateTime.day;
    tmp.hour = tdrDateTime.hour;
    tmp.minute = tdrDateTime.minute;
    tmp.second = tdrDateTime.second;
    dest = *reinterpret_cast<tdr_datetime_t *>(&tmp);
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::str2TdrIP(tdr_ip_t &dest, const char *pszIP) {
    struct in_addr addr;
    if (inet_aton(pszIP, &addr) == 0) {
        return TdrError::TDR_ERR_INVALID_TDRIP_VALUE;
    }
    dest = addr.s_addr;
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::str2TdrTime(tdr_time_t &dest, const char *pszTime) {
    struct tm stTm;
    memset(&stTm, 0, sizeof(stTm));
    if (strptime(pszTime, "%H:%M:%S", &stTm) == NULL) {
        return TdrError::TDR_ERR_INVALID_TDRTIME_VALUE;
    }

    TdrTime tdrTime;
    tdrTime.hour = stTm.tm_hour;
    tdrTime.minute = stTm.tm_min;
    tdrTime.second = stTm.tm_sec;
    if (tdrTime.hour + 999 > 1998 || tdrTime.minute > 59 || tdrTime.second > 59) {
        return TdrError::TDR_ERR_INVALID_TDRTIME_VALUE;
    }

    TdrTime tmp;
    tmp.hour = tdrTime.hour;
    tmp.minute = tdrTime.minute;
    tmp.second = tdrTime.second;
    dest = *reinterpret_cast<tdr_time_t *>(&tmp);
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::str2TdrDate(tdr_date_t &dest, const char *pszDate) {
    struct tm stTm;
    memset(&stTm, 0, sizeof(stTm));
    if (strptime(pszDate, "%Y-%m-%d", &stTm) == NULL) {
        return TdrError::TDR_ERR_INVALID_TDRDATE_VALUE;
    }

    TdrDate tdrDate;
    tdrDate.year = stTm.tm_year + 1900;
    tdrDate.month = stTm.tm_mon + 1;
    tdrDate.day = stTm.tm_mday;
    if (!tdrDate.isValid()) {
        return TdrError::TDR_ERR_INVALID_TDRDATE_VALUE;
    }

    TdrDate tmp;
    tmp.year = tdrDate.year;
    tmp.month = tdrDate.month;
    tmp.day = tdrDate.day;
    dest = *reinterpret_cast<tdr_date_t *>(&tmp);
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::str2TdrDateTime(tdr_datetime_t &dest, const char *pszDateTime) {
    struct tm stTm;
    memset(&stTm, 0, sizeof(stTm));
    if (strptime(pszDateTime, "%Y-%m-%d %H:%M:%S", &stTm) == NULL) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }

    TdrDateTime tdrDateTime;
    tdrDateTime.year = stTm.tm_year + 1900;
    tdrDateTime.month = stTm.tm_mon + 1;
    tdrDateTime.day = stTm.tm_mday;
    tdrDateTime.hour = stTm.tm_hour;
    tdrDateTime.minute = stTm.tm_min;
    tdrDateTime.second = stTm.tm_sec;
    TdrDate datePart;
    datePart.year = tdrDateTime.year;
    datePart.month = tdrDateTime.month;
    datePart.day = tdrDateTime.day;
    if (!datePart.isValid()) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }
    if (tdrDateTime.hour + 999 > 1998 || tdrDateTime.minute > 59 || tdrDateTime.second > 59) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }

    TdrDateTime tmp;
    tmp.year = tdrDateTime.year;
    tmp.month = tdrDateTime.month;
    tmp.day = tdrDateTime.day;
    tmp.hour = tdrDateTime.hour;
    tmp.minute = tdrDateTime.minute;
    tmp.second = tdrDateTime.second;
    dest = *reinterpret_cast<tdr_datetime_t *>(&tmp);
    return TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrTypeUtil::tdrDateTime2UTC(time_t &dest, tdr_datetime_t src) {
    struct tm stTm;
    memset(&stTm, 0, sizeof(stTm));

    TdrDateTime tdrDateTime = *reinterpret_cast<TdrDateTime *>(&src);
    TdrDate datePart;
    datePart.year = tdrDateTime.year;
    datePart.month = tdrDateTime.month;
    datePart.day = tdrDateTime.day;
    if (!datePart.isValid()) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }
    if (tdrDateTime.hour + 999 > 1998 || tdrDateTime.minute > 59 || tdrDateTime.second > 59) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }

    stTm.tm_year = tdrDateTime.year - 1900;
    stTm.tm_mon = tdrDateTime.month - 1;
    stTm.tm_mday = tdrDateTime.day;
    stTm.tm_hour = tdrDateTime.hour;
    stTm.tm_min = tdrDateTime.minute;
    stTm.tm_sec = tdrDateTime.second;
    time_t utcTime = mktime(&stTm);
    if (utcTime == (time_t)-1) {
        return TdrError::TDR_ERR_INVALID_TDRDATETIME_VALUE;
    }
    dest = utcTime;
    return TdrError::TDR_NO_ERROR;
}

int tsf4g_tdr::TdrTypeUtil::compareTdrTime(tdr_time_t left, tdr_time_t right) {
    TdrTime l = *reinterpret_cast<TdrTime *>(&left);
    TdrTime r = *reinterpret_cast<TdrTime *>(&right);
    if (l.hour < r.hour) {
        return -1;
    }
    if (l.hour > r.hour) {
        return 1;
    }
    if (l.minute < r.minute) {
        return -1;
    }
    if (l.minute > r.minute) {
        return 1;
    }
    if (l.second < r.second) {
        return -1;
    }
    if (l.second > r.second) {
        return 1;
    }
    return 0;
}

int tsf4g_tdr::TdrTypeUtil::compareTdrDate(tdr_date_t left, tdr_date_t right) {
    TdrDate l = *reinterpret_cast<TdrDate *>(&left);
    TdrDate r = *reinterpret_cast<TdrDate *>(&right);
    if (l.year < r.year) {
        return -1;
    }
    if (l.year > r.year) {
        return 1;
    }
    if (l.month < r.month) {
        return -1;
    }
    if (l.month > r.month) {
        return 1;
    }
    if (l.day < r.day) {
        return -1;
    }
    if (l.day > r.day) {
        return 1;
    }
    return 0;
}

int tsf4g_tdr::TdrTypeUtil::compareTdrDateTime(tdr_datetime_t left, tdr_datetime_t right) {
    TdrDateTime l = *reinterpret_cast<TdrDateTime *>(&left);
    TdrDateTime r = *reinterpret_cast<TdrDateTime *>(&right);
    if (l.year < r.year) {
        return -1;
    }
    if (l.year > r.year) {
        return 1;
    }
    if (l.month < r.month) {
        return -1;
    }
    if (l.month > r.month) {
        return 1;
    }
    if (l.day < r.day) {
        return -1;
    }
    if (l.day > r.day) {
        return 1;
    }
    if (l.hour < r.hour) {
        return -1;
    }
    if (l.hour > r.hour) {
        return 1;
    }
    if (l.minute < r.minute) {
        return -1;
    }
    if (l.minute > r.minute) {
        return 1;
    }
    if (l.second < r.second) {
        return -1;
    }
    if (l.second > r.second) {
        return 1;
    }
    return 0;
}

unsigned int tsf4g_tdr::TdrTypeUtil::wstrlen(const tdr_wchar_t *wstr) {
    unsigned int len = 0;
    while (wstr[len] != 0) {
        ++len;
    }
    return len;
}
