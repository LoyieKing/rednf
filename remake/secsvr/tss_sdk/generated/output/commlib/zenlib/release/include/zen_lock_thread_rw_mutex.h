// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_lock_thread_rw_mutex.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_THREAD_RW_MUTEX_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_THREAD_RW_MUTEX_H_H_

#include <bits/pthreadtypes.h>

// sizeof = 36
struct ZEN_Thread_RW_Mutex : public ZEN_Lock_Base { // line 20
protected:
._19 rw_lock_;
public:
ZEN_Thread_RW_Mutex(const ZEN_Thread_RW_Mutex &arg0);
ZEN_Thread_RW_Mutex(); // line 36
virtual ~ZEN_Thread_RW_Mutex(); // line 63
virtual void lock_read(); // line 76
virtual bool try_lock_read(); // line 90
virtual bool systime_lock_read(const ZEN_Time_Value &abs_time); // line 104
virtual bool duration_lock_read(const ZEN_Time_Value &relative_time); // line 120
virtual void lock_write(); // line 128
virtual bool try_lock_write(); // line 141
virtual bool systime_lock_write(const ZEN_Time_Value &abs_time); // line 155
virtual bool duration_lock_write(const ZEN_Time_Value &relative_time); // line 172
virtual void unlock(); // line 181
._19 * get_lock(); // line 195
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_LOCK_THREAD_RW_MUTEX_H_H_
