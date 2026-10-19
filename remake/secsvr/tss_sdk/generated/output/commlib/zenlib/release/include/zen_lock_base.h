// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_lock_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_BASE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_BASE_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"

// sizeof = 4
struct ZEN_Condition_Base : public ZEN_NON_Copyable { // line 96
public:
int (***_vptr.ZEN_Condition_Base)(...);
ZEN_Condition_Base(const ZEN_Condition_Base &arg0);
protected:
ZEN_Condition_Base(); // line 100
public:
virtual ~ZEN_Condition_Base(); // line 104
private:
virtual void wait(ZEN_Lock_Base *arg0); // line 110
virtual bool systime_wait(ZEN_Lock_Base *arg0, const ZEN_Time_Value &arg1); // line 116
virtual bool duration_wait(ZEN_Lock_Base *arg0, const ZEN_Time_Value &arg1); // line 122
virtual void signal(); // line 129
virtual void broadcast(); // line 135
};
// sizeof = 4
struct ZEN_Lock_Base : public ZEN_NON_Copyable { // line 35
public:
int (***_vptr.ZEN_Lock_Base)(...);
ZEN_Lock_Base(const ZEN_Lock_Base &arg0);
protected:
ZEN_Lock_Base(const char *arg0); // line 10
public:
virtual ~ZEN_Lock_Base(); // line 14
private:
virtual void lock(); // line 19
virtual bool try_lock(); // line 25
virtual void unlock(); // line 31
virtual bool systime_lock(const ZEN_Time_Value &arg0); // line 37
virtual bool duration_lock(const ZEN_Time_Value &arg0); // line 43
virtual void lock_read(); // line 50
virtual bool try_lock_read(); // line 56
virtual bool timed_lock_read(const ZEN_Time_Value &arg0); // line 62
virtual bool duration_lock_read(const ZEN_Time_Value &arg0); // line 68
virtual void lock_write(); // line 74
virtual bool try_lock_write(); // line 80
virtual bool timed_lock_write(const ZEN_Time_Value &arg0); // line 86
virtual bool duration_lock_write(const ZEN_Time_Value &arg0); // line 92
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_BASE_H_H_
