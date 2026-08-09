// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_os_adapt_process.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_PROCESS_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_PROCESS_H_H_

#include <bits/time.h>
#include <stddef.h>
#include <unistd.h>

// sizeof = 64
struct ZEN_PROCESS_PERFORM { // line 30
public:
pid_t process_id_;
timeval running_time_;
timeval start_time_;
timeval run_stime_;
timeval run_utime_;
int priority_;
int nice_;
size_t vm_size_;
size_t resident_set_;
size_t shared_size_;
size_t text_size_;
size_t data_size_;
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_PROCESS_H_H_
