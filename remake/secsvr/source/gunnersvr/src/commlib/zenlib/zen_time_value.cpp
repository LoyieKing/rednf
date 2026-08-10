// ZEN_Time_Value 还原实现（语义对照 gunnersvr oracle）。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_time_value.cpp
// 说明：
//  - ZEN_OS::timeval_add/timeval_sub/make_timeval/timestamp_ex 定义于
//    zen_os_adapt_time.cpp（本工程其他 TU），此处仅按 oracle 符号签名局部声明并转发。
//  - operator<= 原版只比较秒字段（oracle 0x0809a260 反汇编仅 setle 秒），予以保留。
//  - 自由 operator+/- 因 zen_time_value_ 为 protected，采用 C 风格引用转换传参，
//    生成的调用序列与 oracle（直接透传 this 指针）一致。

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include <iostream>

#include "zen_time_value.h"

namespace ZEN_OS {
const timeval timeval_add(const timeval &left, const timeval &right);
const timeval timeval_sub(const timeval &left, const timeval &right, bool safe);
const timeval make_timeval(clock_t clock_value);
const timeval make_timeval(const timespec *timespec_val);
const char * timestamp_ex(const timeval *timeval, char *str_date_time,
                          size_t datetime_strlen, int fromat_type);
}

// 类静态成员：ZERO(0,0) / MAX(0x7fffffff, 0x7fffffff)，
// 由 TU 全局构造器调用 C1(long,long) 动态初始化（.bss，与 oracle 一致）。
const ZEN_Time_Value ZEN_Time_Value::ZERO_TIME_VALUE(0, 0);
const ZEN_Time_Value ZEN_Time_Value::MAX_TIME_VALUE(0x7fffffff, 0x7fffffff);

ZEN_Time_Value::ZEN_Time_Value() {
    zen_time_value_.tv_sec = 0;
    zen_time_value_.tv_usec = 0;
}

ZEN_Time_Value::~ZEN_Time_Value() {
}

ZEN_Time_Value::ZEN_Time_Value(const timeval &time_data) {
    zen_time_value_ = time_data;
}

ZEN_Time_Value::ZEN_Time_Value(const timespec &timespec_val) {
    zen_time_value_ = ZEN_OS::make_timeval(&timespec_val);
}

ZEN_Time_Value::ZEN_Time_Value(time_t sec, time_t usec) {
    zen_time_value_.tv_sec = sec;
    zen_time_value_.tv_usec = usec;
}

ZEN_Time_Value::ZEN_Time_Value(time_t sec) {
    zen_time_value_.tv_sec = sec;
    zen_time_value_.tv_usec = 0;
}

void ZEN_Time_Value::set(const timeval &time_data) {
    zen_time_value_ = time_data;
}

void ZEN_Time_Value::set(const timespec &timespec_val) {
    zen_time_value_ = ZEN_OS::make_timeval(&timespec_val);
}

void ZEN_Time_Value::set(time_t sec, time_t usec) {
    zen_time_value_.tv_sec = sec;
    zen_time_value_.tv_usec = usec;
}

void ZEN_Time_Value::set(time_t sec) {
    zen_time_value_.tv_sec = sec;
    zen_time_value_.tv_usec = 0;
}

void ZEN_Time_Value::set_by_clock_t(clock_t time) {
    zen_time_value_ = ZEN_OS::make_timeval(time);
}

uint64_t ZEN_Time_Value::total_msec() const {
    return (uint64_t)zen_time_value_.tv_sec * 1000
        + (uint64_t)(zen_time_value_.tv_usec / 1000);
}

void ZEN_Time_Value::total_msec(uint64_t set_msec) {
    const int SEC_PER_MESC = 1000;
    const long int MSEC_PER_USEC = 1000;
    zen_time_value_.tv_sec = (time_t)(set_msec / SEC_PER_MESC);
    zen_time_value_.tv_usec = (time_t)((time_t)(set_msec % SEC_PER_MESC) * MSEC_PER_USEC);
}

uint64_t ZEN_Time_Value::total_usec() const {
    return (uint64_t)zen_time_value_.tv_sec * 1000000
        + (uint64_t)zen_time_value_.tv_usec;
}

void ZEN_Time_Value::total_usec(uint64_t set_usec) {
    const int SEC_PER_UESC = 1000000;
    zen_time_value_.tv_sec = (time_t)(set_usec / SEC_PER_UESC);
    zen_time_value_.tv_usec = (time_t)(set_usec % SEC_PER_UESC);
}

time_t ZEN_Time_Value::sec() const {
    return zen_time_value_.tv_sec;
}

void ZEN_Time_Value::sec(time_t sec) {
    zen_time_value_.tv_sec = sec;
}

time_t ZEN_Time_Value::usec() const {
    return zen_time_value_.tv_usec;
}

void ZEN_Time_Value::usec(time_t usec) {
    zen_time_value_.tv_usec = usec;
}

ZEN_Time_Value & ZEN_Time_Value::operator=(const ZEN_Time_Value &tv) {
    zen_time_value_ = tv.zen_time_value_;
    return *this;
}

ZEN_Time_Value & ZEN_Time_Value::operator+=(const ZEN_Time_Value &tv) {
    zen_time_value_ = ZEN_OS::timeval_add(zen_time_value_, tv.zen_time_value_);
    return *this;
}

ZEN_Time_Value & ZEN_Time_Value::operator-=(const ZEN_Time_Value &tv) {
    zen_time_value_ = ZEN_OS::timeval_sub(zen_time_value_, tv.zen_time_value_, true);
    return *this;
}

bool ZEN_Time_Value::operator<(const ZEN_Time_Value &tv) {
    return zen_time_value_.tv_sec < tv.zen_time_value_.tv_sec
        || (zen_time_value_.tv_sec == tv.zen_time_value_.tv_sec
            && zen_time_value_.tv_usec < tv.zen_time_value_.tv_usec);
}

bool ZEN_Time_Value::operator>(const ZEN_Time_Value &tv) {
    return zen_time_value_.tv_sec > tv.zen_time_value_.tv_sec
        || (zen_time_value_.tv_sec == tv.zen_time_value_.tv_sec
            && zen_time_value_.tv_usec > tv.zen_time_value_.tv_usec);
}

bool ZEN_Time_Value::operator<=(const ZEN_Time_Value &tv) {
    // 原版怪癖：只比较秒字段。
    return zen_time_value_.tv_sec <= tv.zen_time_value_.tv_sec;
}

bool ZEN_Time_Value::operator>=(const ZEN_Time_Value &tv) {
    return zen_time_value_.tv_sec > tv.zen_time_value_.tv_sec
        || (zen_time_value_.tv_sec == tv.zen_time_value_.tv_sec
            && zen_time_value_.tv_usec >= tv.zen_time_value_.tv_usec);
}

bool ZEN_Time_Value::operator==(const ZEN_Time_Value &tv) {
    return zen_time_value_.tv_sec == tv.zen_time_value_.tv_sec
        && zen_time_value_.tv_usec == tv.zen_time_value_.tv_usec;
}

bool ZEN_Time_Value::operator!=(const ZEN_Time_Value &tv) {
    return !(*this == tv);
}

ZEN_Time_Value operator+(const ZEN_Time_Value &tv1, const ZEN_Time_Value &tv2) {
    return ZEN_Time_Value(ZEN_OS::timeval_add((const timeval &)tv1, (const timeval &)tv2));
}

ZEN_Time_Value operator-(const ZEN_Time_Value &tv1, const ZEN_Time_Value &tv2) {
    return ZEN_Time_Value(ZEN_OS::timeval_sub((const timeval &)tv1, (const timeval &)tv2, true));
}

ZEN_Time_Value::operator timeval() const {
    return zen_time_value_;
}

ZEN_Time_Value::operator const timeval*() const {
    return &zen_time_value_;
}

ZEN_Time_Value::operator timeval*() {
    return &zen_time_value_;
}

const char * ZEN_Time_Value::timestamp(char *str_date_time, size_t datetime_strlen,
                                       int fromat_type) const {
    return ZEN_OS::timestamp_ex(&zen_time_value_, str_date_time, datetime_strlen, fromat_type);
}

void ZEN_Time_Value::gettimeofday() {
    timeval tv;
    ::gettimeofday(&tv, NULL);
    zen_time_value_ = tv;
}
