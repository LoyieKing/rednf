// 还原自 gunnersvr 二进制（zen_lock_base.cpp，GCC 4.1.0 SUSE -O2，2026-08-10）
// ZEN_Lock_Base / ZEN_Condition_Base 为纯虚接口的"默认成功"实现：
//   lock/unlock/lock_read/lock_write/wait/signal/broadcast 空实现；
//   try_lock/systime_lock/duration_lock/timed_lock_* 等返回 true；
//   仅 duration_wait 返回 false。
#include "src/commlib/zenlib/zen_lock_base.h"

ZEN_Lock_Base::ZEN_Lock_Base(const char *) {}
ZEN_Lock_Base::~ZEN_Lock_Base() {}

void ZEN_Lock_Base::lock() {}
bool ZEN_Lock_Base::try_lock() { return true; }
void ZEN_Lock_Base::unlock() {}
bool ZEN_Lock_Base::systime_lock(const ZEN_Time_Value &) { return true; }
bool ZEN_Lock_Base::duration_lock(const ZEN_Time_Value &) { return true; }
void ZEN_Lock_Base::lock_read() {}
bool ZEN_Lock_Base::try_lock_read() { return true; }
bool ZEN_Lock_Base::timed_lock_read(const ZEN_Time_Value &) { return true; }
bool ZEN_Lock_Base::duration_lock_read(const ZEN_Time_Value &) { return true; }
void ZEN_Lock_Base::lock_write() {}
bool ZEN_Lock_Base::try_lock_write() { return true; }
bool ZEN_Lock_Base::timed_lock_write(const ZEN_Time_Value &) { return true; }
bool ZEN_Lock_Base::duration_lock_write(const ZEN_Time_Value &) { return true; }

ZEN_Condition_Base::ZEN_Condition_Base() {}
ZEN_Condition_Base::~ZEN_Condition_Base() {}

void ZEN_Condition_Base::wait(ZEN_Lock_Base *) {}
bool ZEN_Condition_Base::systime_wait(ZEN_Lock_Base *, const ZEN_Time_Value &) { return true; }
bool ZEN_Condition_Base::duration_wait(ZEN_Lock_Base *, const ZEN_Time_Value &) { return false; }
void ZEN_Condition_Base::signal() {}
void ZEN_Condition_Base::broadcast() {}
