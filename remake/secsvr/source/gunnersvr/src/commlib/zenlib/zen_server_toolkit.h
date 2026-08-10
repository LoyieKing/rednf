// ZEN_Server_Toolkit 还原头文件（基于 gunnersvr DWARF + 二进制反汇编，2026-08-10）
// 布局保持 DWARF 原样（sizeof = 488）；静态阈值在二进制中为编译期常量（无符号）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_TOOLKIT_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_TOOLKIT_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_process.h"
#include "src/commlib/zenlib/zen_os_adapt_sysinfo.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

// sizeof = 488
struct ZEN_Server_Toolkit { // line 31
protected:
static const int MAX_RECORD_MEMORY_NUMBER = 5;
static const size_t MEMORY_LEAK_THRESHOLD = 0x12bfffff;
static const size_t PROCESS_CPU_RATIO_THRESHOLD = 600;
static const size_t SYSTEM_CPU_RATIO_THRESHOLD = 750;
ZEN_HANDLE pid_handle_;
pid_t self_pid_;
int check_leak_times_;
size_t mem_checkpoint_size_;
size_t cur_mem_usesize_;
uint32_t process_cpu_ratio_;
uint32_t system_cpu_ratio_;
uint64_t can_use_size_;
uint32_t mem_use_ratio_;
ZEN_PROCESS_PERFORM last_process_perf_;
ZEN_PROCESS_PERFORM now_process_perf_;
ZEN_SYSTEM_PERFORMANCE last_system_perf_;
ZEN_SYSTEM_PERFORMANCE now_system_perf_;
ZEN_Server_Toolkit(); // line 34
~ZEN_Server_Toolkit(); // line 35
public:
int socket_init(); // line 40
int out_pid_file(const char *arg0, bool arg1); // line 44
int watch_dog_status(bool arg0); // line 47
uint32_t get_sys_cpu_ratio() const; // line 50
uint32_t get_app_cpu_ratio() const; // line 51
uint32_t get_sys_mem_ratio() const; // line 52
uint64_t get_can_use_mem_size() const; // line 53
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_TOOLKIT_H_H_
