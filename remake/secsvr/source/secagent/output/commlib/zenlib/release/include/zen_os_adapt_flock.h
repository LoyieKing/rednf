// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_os_adapt_flock.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_FLOCK_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_FLOCK_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <bits/fcntl.h>

// sizeof = 32
struct zen_flock_t { // line 36
public:
flock lock_;
ZEN_HANDLE handle_;
bool open_by_self_;
zen_flock_t(); // line 38
~zen_flock_t(); // line 43
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_FLOCK_H_H_
