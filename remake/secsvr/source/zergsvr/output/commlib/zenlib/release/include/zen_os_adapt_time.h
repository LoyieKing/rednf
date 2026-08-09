// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_os_adapt_time.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_TIME_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_TIME_H_H_

extern const time_t ONE_HOUR_SECONDS; // line 27
extern const time_t ONE_QUARTER_SECONDS; // line 29
extern const time_t FIVE_MINUTE_SECONDS; // line 31
extern const time_t ONE_DAY_SECONDS; // line 33
extern const time_t SERVER_STATUS_CHECK_INTERVAL; // line 36
extern const timeval now_time; // line 308
namespace ZEN_OS {
    extern int gettimeofday(timeval *arg0, timezone *arg1); // line 245
    extern const size_t LEN_OF_TIME_STAMP; // line 77
    extern const size_t LEN_OF_ISO_DAY_TIMESTRING; // line 81
    extern const size_t LEN_OF_ISO_SEC_TIMESTRING; // line 83
    extern const size_t LEN_OF_ISO_USEC_TIMESTRING; // line 85
    extern const size_t LEN_OF_US_DAY_TIMESTRING; // line 89
    extern const size_t LEN_OF_US_SEC_TIMESTRING; // line 91
    extern const size_t LEN_OF_US_USEC_TIMESTRING; // line 93
} // namespace ZEN_OS

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_TIME_H_H_
