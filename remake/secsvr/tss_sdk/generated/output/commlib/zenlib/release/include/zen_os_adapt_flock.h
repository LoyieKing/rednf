// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_os_adapt_flock.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_FLOCK_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_FLOCK_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <bits/fcntl.h>

// sizeof = 32
struct zen_flock_t { // line 28
public:
flock lock_;
ZEN_HANDLE handle_;
bool open_by_self_;
zen_flock_t(); // line 30
~zen_flock_t(); // line 35
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_OS_ADAPT_FLOCK_H_H_
