// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_share_mem_mmap.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHARE_MEM_MMAP_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHARE_MEM_MMAP_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include <bits/stringfwd.h>
#include <stddef.h>

// sizeof = 16
struct ZEN_ShareMem_Mmap : public ZEN_NON_Copyable { // line 14
protected:
string mmap_file_name_;
void *mmap_addr_;
ZEN_HANDLE mmap_handle_;
size_t shm_size_;
public:
ZEN_ShareMem_Mmap(); // line 17
~ZEN_ShareMem_Mmap(); // line 18
int open(const char *arg0, size_t arg1, int arg2, int arg3, const void *arg4, int arg5, int arg6, size_t arg7); // line 32
int open(const char *arg0, size_t arg1, bool arg2, bool arg3, bool arg4, const void *arg5, size_t arg6); // line 43
int close(); // line 47
int remove(); // line 50
int flush(); // line 53
void * addr(); // line 56
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHARE_MEM_MMAP_H_H_
