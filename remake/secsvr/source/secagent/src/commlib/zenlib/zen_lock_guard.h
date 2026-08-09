// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_lock_guard.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_GUARD_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_GUARD_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"

// sizeof = 4
struct ZEN_Lock_Guard<ZEN_Null_Mutex> : public ZEN_NON_Copyable { // line 28
protected:
ZEN_Null_Mutex *lock_;
public:
void ZEN_Lock_Guard(ZEN_Null_Mutex &arg0); // line 32
void ZEN_Lock_Guard(ZEN_Null_Mutex &arg0, bool arg1); // line 39
void ~ZEN_Lock_Guard(); // line 49
void lock(); // line 55
bool try_lock(); // line 61
void unlock(); // line 67
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_GUARD_H_H_
