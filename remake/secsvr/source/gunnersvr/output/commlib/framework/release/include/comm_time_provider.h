// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_time_provider.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TIME_PROVIDER_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TIME_PROVIDER_H_H_

#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include <bits/time.h>
#include <time.h>

// sizeof = 8
struct Comm_Time_Provider { // line 14
private:
ZEN_Time_Value cur_time_;
static Comm_Time_Provider *instance_;
public:
static Comm_Time_Provider * instance(); // line 17
static void clean_instance(); // line 18
time_t time() const; // line 21
timeval gettimeofday() const; // line 27
void update(const ZEN_Time_Value &arg0); // line 33
private:
Comm_Time_Provider(); // line 39
~Comm_Time_Provider(); // line 40
};

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TIME_PROVIDER_H_H_
