// 还原自 gunnersvr 二进制（zen_lock_null_lock.cpp，GCC 4.1.0 SUSE -O2，2026-08-10）
// 空锁/空条件变量：所有锁操作 no-op，查询类返回 true，duration_wait 返回 false。
#include "src/commlib/zenlib/zen_lock_null_lock.h"

ZEN_Null_Mutex::ZEN_Null_Mutex(const char *) : ZEN_Lock_Base(0), lock_(0) {}
ZEN_Null_Mutex::~ZEN_Null_Mutex() {}

void ZEN_Null_Mutex::lock() {}
bool ZEN_Null_Mutex::try_lock() { return true; }
void ZEN_Null_Mutex::unlock() {}
bool ZEN_Null_Mutex::systime_lock(const ZEN_Time_Value &) { return true; }
bool ZEN_Null_Mutex::duration_lock(const ZEN_Time_Value &) { return true; }
void ZEN_Null_Mutex::lock_read() {}
bool ZEN_Null_Mutex::try_lock_read() { return true; }
bool ZEN_Null_Mutex::timed_lock_read(const ZEN_Time_Value &) { return true; }
bool ZEN_Null_Mutex::duration_lock_read(const ZEN_Time_Value &) { return true; }
void ZEN_Null_Mutex::lock_write() {}
bool ZEN_Null_Mutex::try_lock_write() { return true; }
bool ZEN_Null_Mutex::timed_lock_write(const ZEN_Time_Value &) { return true; }
bool ZEN_Null_Mutex::duration_lock_write(const ZEN_Time_Value &) { return true; }

ZEN_Null_Condition::ZEN_Null_Condition() : ZEN_Condition_Base(), lock_(0) {}
ZEN_Null_Condition::~ZEN_Null_Condition() {}

void ZEN_Null_Condition::wait(ZEN_Null_Mutex *) {}
bool ZEN_Null_Condition::systime_wait(ZEN_Null_Mutex *, const ZEN_Time_Value &) { return true; }
bool ZEN_Null_Condition::duration_wait(ZEN_Null_Mutex *, const ZEN_Time_Value &) { return false; }
void ZEN_Null_Condition::signal() {}
void ZEN_Null_Condition::broadcast() {}
