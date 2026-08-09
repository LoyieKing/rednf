// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_os_adapt_sysinfo.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SYSINFO_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SYSINFO_H_H_

#include <bits/time.h>
#include <sys/types.h>

// sizeof = 160
struct ZEN_SYSTEM_PERFORMANCE { // line 61
public:
int64_t totalram_size_;
int64_t freeram_size_;
int64_t shared_size_;
int64_t bufferram_size_;
int64_t cachedram_size_;
int64_t totalswap_size_;
int64_t freeswap_size_;
int64_t swapcached_size_;
timeval up_time_;
timeval user_time_;
timeval nice_time_;
timeval system_time_;
timeval idle_time_;
timeval iowait_time_;
timeval hardirq_time_;
timeval softirq_time_;
double sys_loads_[];
int processes_num_;
int running_num_;
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_SYSINFO_H_H_
