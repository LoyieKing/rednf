#ifndef DNF_GAME_RWSECTIONLOCK_H_
#define DNF_GAME_RWSECTIONLOCK_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/RwSectionLock/
// ORIG T 符号（0x08ad37f8..0x08ad388e）：C1 0x08ad37f8 / D1 0x08ad3834 /
// enterRead 0x08ad3852 / leaveRead 0x08ad3866 / enterWrite 0x08ad387a /
// leaveWrite 0x08ad388e。

#include <pthread.h>
#include "GameTypes.h"   // Uncopyable 基类（ctor 反编译实证：Uncopyable::Uncopyable(this)）

// 读写锁封装：本体即 pthread_rwlock_t（ctor：pthread_rwlockattr_init
// → pthread_rwlock_init(this,attr) → attr destroy）。
class RwSectionLock : public Uncopyable
{
public:
    RwSectionLock();                    // T 0x08ad37f8
    ~RwSectionLock();                   // T 0x08ad3834
    void enterRead();                   // T 0x08ad3852
    void leaveRead();                   // T 0x08ad3866
    void enterWrite();                  // T 0x08ad387a
    void leaveWrite();                  // T 0x08ad388e

private:
    pthread_rwlock_t m_rwlock;   // 本体（ORIG this 即 rwlock 指针）
};

#endif  // DNF_GAME_RWSECTIONLOCK_H_
