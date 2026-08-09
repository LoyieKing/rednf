// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_lock_null_lock.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_NULL_LOCK_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_NULL_LOCK_H_H_

// sizeof = 8
struct ZEN_Null_Condition : public ZEN_Condition_Base { // line 101
protected:
int lock_;
public:
ZEN_Null_Condition(const ZEN_Null_Condition &arg0);
ZEN_Null_Condition(); // line 101
virtual ~ZEN_Null_Condition(); // line 106
private:
virtual void wait(ZEN_Null_Mutex *arg0); // line 112
virtual bool systime_wait(ZEN_Null_Mutex *arg0, const ZEN_Time_Value &arg1); // line 118
virtual bool duration_wait(ZEN_Null_Mutex *arg0, const ZEN_Time_Value &arg1); // line 124
virtual void signal(); // line 131
virtual void broadcast(); // line 137
};
// sizeof = 8
struct ZEN_Null_Mutex : public ZEN_Lock_Base { // line 35
protected:
int lock_;
public:
ZEN_Null_Mutex(const ZEN_Null_Mutex &arg0);
ZEN_Null_Mutex(const char *arg0); // line 10
virtual ~ZEN_Null_Mutex(); // line 15
virtual void lock(); // line 20
virtual bool try_lock(); // line 26
virtual void unlock(); // line 32
virtual bool systime_lock(const ZEN_Time_Value &arg0); // line 38
virtual bool duration_lock(const ZEN_Time_Value &arg0); // line 44
virtual void lock_read(); // line 51
virtual bool try_lock_read(); // line 57
virtual bool timed_lock_read(const ZEN_Time_Value &arg0); // line 63
virtual bool duration_lock_read(const ZEN_Time_Value &arg0); // line 69
virtual void lock_write(); // line 75
virtual bool try_lock_write(); // line 81
virtual bool timed_lock_write(const ZEN_Time_Value &arg0); // line 87
virtual bool duration_lock_write(const ZEN_Time_Value &arg0); // line 93
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_NULL_LOCK_H_H_
