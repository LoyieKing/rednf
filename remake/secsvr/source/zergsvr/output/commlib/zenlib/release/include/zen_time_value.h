// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_time_value.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIME_VALUE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIME_VALUE_H_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

// sizeof = 8
struct ZEN_Time_Value { // line 8
protected:
timeval zen_time_value_;
public:
static const ZEN_Time_Value ZERO_TIME_VALUE;
static const ZEN_Time_Value MAX_TIME_VALUE;
ZEN_Time_Value(); // line 12
~ZEN_Time_Value(); // line 13
ZEN_Time_Value(const timeval &arg0); // line 16
ZEN_Time_Value(const timespec &arg0); // line 19
ZEN_Time_Value(time_t arg0, time_t arg1); // line 22
ZEN_Time_Value(time_t arg0); // line 25
void set(const timeval &arg0); // line 39
void set(const timespec &arg0); // line 41
void set(time_t arg0, time_t arg1); // line 43
void set(time_t arg0); // line 45
void set_by_clock_t(clock_t arg0); // line 48
time_t sec() const; // line 61
void sec(time_t arg0); // line 63
time_t usec() const; // line 66
void usec(time_t arg0); // line 68
uint64_t total_msec() const; // line 71
void total_msec(uint64_t arg0); // line 73
uint64_t total_usec() const; // line 76
void total_usec(uint64_t arg0); // line 78
const char * timestamp(char *arg0, size_t arg1, int arg2) const; // line 83
operator timespec() const; // line 86
operator timeval() const; // line 89
operator const timeval*() const; // line 91
operator timeval*(); // line 93
void gettimeofday(); // line 97
ZEN_Time_Value & operator+=(const ZEN_Time_Value &arg0); // line 100
ZEN_Time_Value & operator=(const ZEN_Time_Value &arg0); // line 102
ZEN_Time_Value & operator-=(const ZEN_Time_Value &arg0); // line 104
bool operator<(const ZEN_Time_Value &arg0); // line 108
bool operator>(const ZEN_Time_Value &arg0); // line 110
bool operator<=(const ZEN_Time_Value &arg0); // line 113
bool operator>=(const ZEN_Time_Value &arg0); // line 116
bool operator==(const ZEN_Time_Value &arg0); // line 119
bool operator!=(const ZEN_Time_Value &arg0); // line 122
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIME_VALUE_H_H_
