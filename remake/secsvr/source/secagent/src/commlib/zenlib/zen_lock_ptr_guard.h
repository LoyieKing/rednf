// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_lock_ptr_guard.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_PTR_GUARD_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_PTR_GUARD_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"

// sizeof = 4
struct ZEN_Lock_Ptr_Guard : public ZEN_NON_Copyable { // line 29
protected:
ZEN_Lock_Base *lock_ptr_;
public:
ZEN_Lock_Ptr_Guard(ZEN_Lock_Base *arg0); // line 33
ZEN_Lock_Ptr_Guard(ZEN_Lock_Base *arg0, bool arg1); // line 40
~ZEN_Lock_Ptr_Guard(); // line 50
void lock(); // line 56
bool try_lock(); // line 62
void unlock(); // line 68
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_PTR_GUARD_H_H_
