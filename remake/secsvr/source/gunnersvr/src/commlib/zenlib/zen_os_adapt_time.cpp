// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_time.cpp
// Original quirks preserved (verified against the oracle at 0x08092e10-0x080937c7):
//  - timeval_clear only clears tv_sec (tv_usec untouched).
//  - timeval_add only carries when usec > 1000000 (== 1000000 does NOT carry).
//  - total_milliseconds(timespec) divides tv_nsec by 1000000000 (not 1000000).
//  - timestamp_ex returns NULL (and sets errno=EINVAL) for unknown format bits;
//    required buffer sizes are checked with "> len" against the format length.

#include "src/commlib/zenlib/zen_os_adapt_time.h"

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// ZEN_Trace_LogMsg is restored in a later TU; the static vararg entry used by
// get_uptime is forward-declared here so this TU stays self-contained.
struct ZEN_Trace_LogMsg {
    static void debug_errorex(const char *str_format, ...);
};

namespace ZEN_OS {

const timeval timeval_zero() {
    timeval zero_time;
    zero_time.tv_sec = 0;
    zero_time.tv_usec = 0;
    return zero_time;
}

void timeval_clear(timeval &tv) {
    // Original quirk: only tv_sec is cleared.
    tv.tv_sec = 0;
}

uint64_t total_milliseconds(const timeval &tv) {
    // tv_usec / 1000 is an unsigned 32-bit division in the oracle.
    return (uint64_t)tv.tv_sec * 1000 + (uint32_t)tv.tv_usec / 1000;
}

uint64_t total_microseconds(const timeval &tv) {
    // 64-bit signed: tv_sec * 1000 + tv_usec (tv_usec sign-extended).
    return (uint64_t)((int64_t)tv.tv_sec * 1000 + tv.tv_usec);
}

int timeval_compare(const timeval &left, const timeval &right) {
    if (left.tv_sec == right.tv_sec) {
        return left.tv_usec - right.tv_usec;
    }
    return left.tv_sec - right.tv_sec;
}

const timeval timeval_add(const timeval &left, const timeval &right) {
    const uint32_t SEC_PER_USEC = 1000000;
    timeval plus_time_val;
    plus_time_val.tv_sec = left.tv_sec + right.tv_sec;
    plus_time_val.tv_usec = left.tv_usec + right.tv_usec;
    if (plus_time_val.tv_usec > SEC_PER_USEC) {
        plus_time_val.tv_sec += plus_time_val.tv_usec / SEC_PER_USEC;
        plus_time_val.tv_usec %= SEC_PER_USEC;
    }
    return plus_time_val;
}

const timeval timeval_sub(const timeval &left, const timeval &right, bool safe) {
    const uint32_t SEC_PER_USEC = 1000000;
    int64_t left_usec_val = left.tv_sec * SEC_PER_USEC + left.tv_usec;
    int64_t right_usec_val = right.tv_sec * SEC_PER_USEC + right.tv_usec;
    int64_t minus_usec_val = left_usec_val - right_usec_val;
    timeval minus_time_val;
    if (minus_usec_val < 0 && safe) {
        minus_time_val.tv_sec = 0;
        minus_time_val.tv_usec = 0;
        return minus_time_val;
    }
    minus_time_val.tv_sec = minus_usec_val / SEC_PER_USEC;
    minus_time_val.tv_usec = minus_usec_val % SEC_PER_USEC;
    return minus_time_val;
}

void timeval_adjust(timeval &tv) {
    const uint32_t SEC_PER_USEC = 1000000;
    int64_t tv_usec_val = tv.tv_sec * SEC_PER_USEC + tv.tv_usec;
    tv.tv_sec = tv_usec_val / SEC_PER_USEC;
    tv.tv_usec = tv_usec_val % SEC_PER_USEC;
}

bool timeval_havetime(const timeval &tv) {
    return (uint32_t)(tv.tv_sec * 1000000 + tv.tv_usec) != 0;
}

const timeval make_timeval(time_t sec, time_t usec) {
    timeval to_timeval;
    to_timeval.tv_sec = sec;
    to_timeval.tv_usec = usec;
    return to_timeval;
}

const timeval make_timeval(clock_t clock_value) {
    const uint32_t SEC_PER_USEC = 1000000;
    timeval to_timeval;
    clock_t remain_val = clock_value % SEC_PER_USEC;
    to_timeval.tv_sec = clock_value / SEC_PER_USEC;
    to_timeval.tv_usec = (uint32_t)(remain_val * SEC_PER_USEC) / SEC_PER_USEC;
    return to_timeval;
}

const timeval make_timeval(const timespec *timespec_val) {
    const uint32_t USEC_PER_NSEC = 1000;
    timeval to_timeval;
    to_timeval.tv_sec = timespec_val->tv_sec;
    to_timeval.tv_usec = (uint32_t)timespec_val->tv_nsec / USEC_PER_NSEC;
    return to_timeval;
}

const timespec make_timespec(const timeval &timeval_val) {
    const uint32_t USEC_PER_NSEC = 1000;
    timespec to_timespec;
    to_timespec.tv_sec = timeval_val.tv_sec;
    to_timespec.tv_nsec = timeval_val.tv_usec * USEC_PER_NSEC;
    return to_timespec;
}

uint64_t total_milliseconds(const timespec &tv) {
    // Original quirk: divides tv_nsec by 1000000000 instead of 1000000.
    return (uint64_t)tv.tv_sec * 1000 + (uint32_t)tv.tv_nsec / 1000000000;
}

void usleep(unsigned long usec) {
    ::usleep(usec);
}

int sleep(const timeval &tv) {
    const int ONE_SECOND_IN_USECS = 1000000;
    return ::usleep(tv.tv_sec * ONE_SECOND_IN_USECS + tv.tv_usec);
}

int sleep(uint32_t seconds) {
    return ::sleep(seconds);
}

const char *timestamp(const timeval *timeval, char *str_date_time,
                      size_t datetime_strlen) {
    time_t now_time = timeval->tv_sec;
    tm tm_data;
    localtime_r(&now_time, &tm_data);
    snprintf(str_date_time, datetime_strlen, "%4d-%02d-%02d %02d:%02d:%02d.%06ld",
             tm_data.tm_year + 1900, tm_data.tm_mon + 1, tm_data.tm_mday,
             tm_data.tm_hour, tm_data.tm_min, tm_data.tm_sec, timeval->tv_usec);
    return str_date_time;
}

const char *timestamp(char *str_date_time, size_t datetime_strlen) {
    timeval now_time_val;
    ::gettimeofday(&now_time_val, NULL);
    return ZEN_OS::timestamp(&now_time_val, str_date_time, datetime_strlen);
}

const timeval get_uptime() {
    timespec sp;
    timeval up_time;
    int ret = clock_gettime(CLOCK_MONOTONIC, &sp);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "::clock_gettime(CLOCK_MONOTONIC, &sp) ret != 0,fail.ret = %d lasterror = %d",
            ret, errno);
        up_time = timeval_zero();
    } else {
        up_time = make_timeval(&sp);
    }
    return up_time;
}

const char *timestamp_ex(const timeval *timeval, char *str_date_time,
                         size_t datetime_strlen, int fromat_type) {
    static const char *const MONTH_NAME[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static const char *const DAY_OF_WEEK_NAME[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    time_t now_time = timeval->tv_sec;
    tm tm_data;
    if (fromat_type & 0x1) {
        localtime_r(&now_time, &tm_data);
    } else if (fromat_type & 0x2) {
        gmtime_r(&now_time, &tm_data);
    } else {
        errno = EINVAL;
        return NULL;
    }

    if (fromat_type & 0x10) {
        if (fromat_type & 0x400) {
            // oracle: len <= 26 -> NULL
            if (datetime_strlen <= 26) {
                return NULL;
            }
            snprintf(str_date_time, datetime_strlen, "%4d-%02d-%02d %02d:%02d:%02d.%06ld",
                     tm_data.tm_year + 1900, tm_data.tm_mon + 1, tm_data.tm_mday,
                     tm_data.tm_hour, tm_data.tm_min, tm_data.tm_sec,
                     timeval->tv_usec);
            return str_date_time;
        }
        if (fromat_type & 0x200) {
            // oracle: len <= 19 -> NULL
            if (datetime_strlen <= 19) {
                return NULL;
            }
            snprintf(str_date_time, datetime_strlen, "%4d-%02d-%02d %02d:%02d:%02d",
                     tm_data.tm_year + 1900, tm_data.tm_mon + 1, tm_data.tm_mday,
                     tm_data.tm_hour, tm_data.tm_min, tm_data.tm_sec);
            return str_date_time;
        }
        if (!(fromat_type & 0x100)) {
            errno = EINVAL;
            return NULL;
        }
        // oracle: len <= 10 -> NULL
        if (datetime_strlen <= 10) {
            return NULL;
        }
        snprintf(str_date_time, datetime_strlen, "%4d-%02d-%02d",
                 tm_data.tm_year + 1900, tm_data.tm_mon + 1, tm_data.tm_mday);
        return str_date_time;
    }

    if (!(fromat_type & 0x20)) {
        errno = EINVAL;
        return NULL;
    }
    if (fromat_type & 0x400) {
        // oracle: len <= 31 -> NULL
        if (datetime_strlen <= 31) {
            return NULL;
        }
        snprintf(str_date_time, datetime_strlen,
                 "%3s %3s %2d %04d %02d:%02d:%02d.%06d",
                 DAY_OF_WEEK_NAME[tm_data.tm_wday], MONTH_NAME[tm_data.tm_mon],
                 tm_data.tm_mday, tm_data.tm_year + 1900,
                 tm_data.tm_hour, tm_data.tm_min, tm_data.tm_sec,
                 timeval->tv_usec);
        return str_date_time;
    }
    if (fromat_type & 0x200) {
        // oracle: len <= 24 -> NULL
        if (datetime_strlen <= 24) {
            return NULL;
        }
        snprintf(str_date_time, datetime_strlen,
                 "%3s %3s %2d %04d %02d:%02d:%02d",
                 DAY_OF_WEEK_NAME[tm_data.tm_wday], MONTH_NAME[tm_data.tm_mon],
                 tm_data.tm_mday, tm_data.tm_year + 1900,
                 tm_data.tm_hour, tm_data.tm_min, tm_data.tm_sec);
        return str_date_time;
    }
    if (!(fromat_type & 0x100)) {
        errno = EINVAL;
        return NULL;
    }
    // Oracle quirk: the US day format also requires len > 24 (same as the
    // US second format), not the 15 bytes the format itself needs.
    if (datetime_strlen <= 24) {
        return NULL;
    }
    snprintf(str_date_time, datetime_strlen, "%3s %3s %2d %04d",
             DAY_OF_WEEK_NAME[tm_data.tm_wday], MONTH_NAME[tm_data.tm_mon],
             tm_data.tm_mday, tm_data.tm_year + 1900);
    return str_date_time;
}

} // namespace ZEN_OS
