// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG +
// docs/class_func_reports/RwSpinLock/{RwSpinLock,README}.md（11 函数报告）
// RwSpinLock：ORIG T 生命周期/读写锁族 @0x08ad35a6..0x08ad374f。
// ctor 报告：Uncopyable 空基类（GameTypes.h）+ this->state_ = 0（写 this+0x00，
// 即 state_ 为首个数据成员，空基类优化）；enterRead 报告为原子循环锁。
// isReadTaken/isWriteTaken 返回类型 demangled 缺失【推断】bool。
#ifndef DNF_GAME_RWSPINLOCK_H_
#define DNF_GAME_RWSPINLOCK_H_

#include "GameTypes.h"   // Uncopyable 基类（ORIG ctor 调 Uncopyable::Uncopyable）

class RwSpinLock : public Uncopyable
{
public:
    RwSpinLock();   // ORIG T @0x08ad35a6

    void enterRead();                              // ORIG T @0x08ad35c2
    void leaveRead();                              // ORIG T @0x08ad35d6
    void enterWrite();                             // ORIG T @0x08ad35ea
    void leaveWrite();                             // ORIG T @0x08ad35fe
    bool isReadTaken();                            // ORIG T @0x08ad3612【推断】返回 bool
    bool isWriteTaken();                           // ORIG T @0x08ad3626【推断】返回 bool
    static void enterReadToValue(long volatile* value);   // ORIG T @0x08ad363a；静态（Ghidra C 无 this + enterRead 单参数槽实证）
    static void leaveReadToValue(long volatile* value);   // ORIG T @0x08ad367a；静态（同上）
    static void enterWriteToValue(long volatile* value);  // ORIG T @0x08ad36be；静态（同上）
    static void leaveWriteToValue(long volatile* value);  // ORIG T @0x08ad372a；静态（同上）

    long state_;  // +0x00（ORIG ctor 写 0；读写状态字。long=32 位，enterReadToValue 直传 &state_ 无 cast，见 mangled EPVl）
};

#endif  // DNF_GAME_RWSPINLOCK_H_
