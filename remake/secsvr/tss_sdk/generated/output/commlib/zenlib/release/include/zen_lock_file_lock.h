// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_lock_file_lock.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_FILE_LOCK_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_FILE_LOCK_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include <sys/types.h>

// sizeof = 8
struct ZEN_File_Lock : public ZEN_NON_Copyable { // line 27
protected:
ZEN_HANDLE lock_file_hadle_;
bool open_by_self_;
public:
ZEN_File_Lock(ZEN_HANDLE arg0); // line 38
ZEN_File_Lock(); // line 40
~ZEN_File_Lock(); // line 41
int open(const char *arg0, int arg1, mode_t arg2); // line 48
int open(ZEN_HANDLE arg0); // line 51
int close(); // line 54
ZEN_HANDLE get_file_handle(); // line 57
void lock_read(); // line 62
bool try_lock_read(); // line 64
void lock_write(); // line 67
bool try_lock_write(); // line 69
void unlock(); // line 72
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_FILE_LOCK_H_H_
