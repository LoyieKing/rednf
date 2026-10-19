// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_lock_guard.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_GUARD_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_GUARD_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"

// sizeof = 4
struct ZEN_Lock_Guard<ZEN_Thread_Mutex> : public ZEN_NON_Copyable { // line 21
protected:
ZEN_Thread_Mutex *lock_;
public:
void ZEN_Lock_Guard(ZEN_Thread_Mutex &arg0); // line 25
void ZEN_Lock_Guard(ZEN_Thread_Mutex &arg0, bool arg1); // line 32
void ~ZEN_Lock_Guard(); // line 42
void lock(); // line 48
bool try_lock(); // line 54
void unlock(); // line 60
};
// sizeof = 4
struct ZEN_Write_Guard<ZEN_File_Lock> : public ZEN_NON_Copyable { // line 148
protected:
ZEN_File_Lock *lock_;
public:
void ZEN_Write_Guard(ZEN_File_Lock &arg0); // line 151
void ZEN_Write_Guard(ZEN_File_Lock &arg0, bool arg1); // line 158
void ~ZEN_Write_Guard(); // line 168
void lock_write(); // line 174
bool try_lock_write(); // line 179
void unlock(); // line 185
};
// sizeof = 4
struct ZEN_Write_Guard<ZEN_Thread_RW_Mutex> : public ZEN_NON_Copyable { // line 148
protected:
ZEN_Thread_RW_Mutex *lock_;
public:
void ZEN_Write_Guard(ZEN_Thread_RW_Mutex &arg0); // line 151
void ZEN_Write_Guard(ZEN_Thread_RW_Mutex &arg0, bool arg1); // line 158
void ~ZEN_Write_Guard(); // line 168
void lock_write(); // line 174
bool try_lock_write(); // line 179
void unlock(); // line 185
};
// sizeof = 4
struct ZEN_Read_Guard<ZEN_Thread_RW_Mutex> : public ZEN_NON_Copyable { // line 86
protected:
ZEN_Thread_RW_Mutex *lock_;
public:
void ZEN_Read_Guard(ZEN_Thread_RW_Mutex &arg0); // line 89
void ZEN_Read_Guard(ZEN_Thread_RW_Mutex &arg0, bool arg1); // line 96
void ~ZEN_Read_Guard(); // line 106
void lock_read(); // line 112
bool try_lock_read(); // line 117
void unlock(); // line 123
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_GUARD_H_H_
