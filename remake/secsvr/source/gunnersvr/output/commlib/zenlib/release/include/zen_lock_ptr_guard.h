// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_lock_ptr_guard.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_PTR_GUARD_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_PTR_GUARD_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"

// sizeof = 4
struct ZEN_Lock_Ptr_Guard : public ZEN_NON_Copyable { // line 21
protected:
ZEN_Lock_Base *lock_ptr_;
public:
ZEN_Lock_Ptr_Guard(ZEN_Lock_Base *arg0); // line 25
ZEN_Lock_Ptr_Guard(ZEN_Lock_Base *arg0, bool arg1); // line 32
~ZEN_Lock_Ptr_Guard(); // line 42
void lock(); // line 48
bool try_lock(); // line 54
void unlock(); // line 60
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_PTR_GUARD_H_H_
