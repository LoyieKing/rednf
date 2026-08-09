// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_share_mem_posix.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SHARE_MEM_POSIX_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SHARE_MEM_POSIX_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include <bits/stringfwd.h>
#include <stddef.h>

// sizeof = 16
struct ZEN_ShareMem_Posix : public ZEN_NON_Copyable { // line 13
protected:
string shm_name_;
void *mmap_addr_;
ZEN_HANDLE mmap_handle_;
size_t shm_size_;
public:
ZEN_ShareMem_Posix(); // line 18
~ZEN_ShareMem_Posix(); // line 19
int open(const char *arg0, size_t arg1, int arg2, int arg3, const void *arg4, int arg5, int arg6, size_t arg7); // line 31
int open(const char *arg0, size_t arg1, bool arg2, bool arg3, bool arg4, const void *arg5, size_t arg6); // line 42
int close(); // line 46
int remove(); // line 49
int flush(); // line 52
void * addr(); // line 55
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SHARE_MEM_POSIX_H_H_
