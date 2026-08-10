// Restored header for zen_os_adapt_time.cpp (gunnersvr oracle).
// Constant values recovered from DWARF DW_AT_const_value of the original TU.
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_TIME_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_TIME_H_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

const time_t ONE_HOUR_SECONDS = 3600;      // line 28
const time_t ONE_QUARTER_SECONDS = 900;    // line 30
const time_t FIVE_MINUTE_SECONDS = 300;    // line 32
const time_t ONE_DAY_SECONDS = 86400;      // line 35

namespace ZEN_OS {
    // NB: glibc <time.h> declares a global variable 'timezone', so the struct
    // tag must be spelled explicitly in C++; the mangled name is unchanged.
    extern int gettimeofday(timeval *tv, struct timezone *tz);

    // Header-only constants (internal linkage per TU in the original build).
    const size_t LEN_OF_TIME_STAMP = 26;            // line 76
    const size_t LEN_OF_ISO_DAY_TIMESTRING = 10;    // line 80
    const size_t LEN_OF_ISO_SEC_TIMESTRING = 19;    // line 82
    const size_t LEN_OF_ISO_USEC_TIMESTRING = 26;   // line 84
    const size_t LEN_OF_US_DAY_TIMESTRING = 15;     // line 88
    const size_t LEN_OF_US_SEC_TIMESTRING = 24;     // line 90
    const size_t LEN_OF_US_USEC_TIMESTRING = 31;    // line 92

    extern const timeval timeval_zero();
    extern void timeval_clear(timeval &tv);
    extern uint64_t total_milliseconds(const timeval &tv);
    extern uint64_t total_microseconds(const timeval &tv);
    extern int timeval_compare(const timeval &left, const timeval &right);
    extern const timeval timeval_add(const timeval &left, const timeval &right);
    extern const timeval timeval_sub(const timeval &left, const timeval &right, bool safe);
    extern void timeval_adjust(timeval &tv);
    extern bool timeval_havetime(const timeval &tv);
    extern const timeval make_timeval(time_t sec, time_t usec);
    extern const timeval make_timeval(clock_t clock_value);
    extern const timeval make_timeval(const timespec *timespec_val);
    extern const timespec make_timespec(const timeval &timeval_val);
    extern uint64_t total_milliseconds(const timespec &tv);
    extern void usleep(unsigned long usec);
    extern int sleep(const timeval &tv);
    extern int sleep(uint32_t seconds);
    extern const char *timestamp(const timeval *timeval, char *str_date_time,
                                 size_t datetime_strlen);
    extern const char *timestamp(char *str_date_time, size_t datetime_strlen);
    extern const timeval get_uptime();
    extern const char *timestamp_ex(const timeval *timeval, char *str_date_time,
                                    size_t datetime_strlen, int fromat_type);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_TIME_H_H_
