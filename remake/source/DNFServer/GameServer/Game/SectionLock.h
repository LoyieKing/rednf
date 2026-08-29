#ifndef DNF_GAME_SECTIONLOCK_H_
#define DNF_GAME_SECTIONLOCK_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/SectionLock/
// ORIG T 符号（0x08ad3742..0x08ad37ed）：C1 0x08ad3742 / D1 0x08ad3786 /
// enter 0x08ad37a4 / leave 0x08ad37d0 / tryEnter 0x08ad37b8 /
// isTaken 0x08ad37e4 / isTakenByCurrentThread 0x08ad37ee。

#include <pthread.h>
#include "GameTypes.h"   // Uncopyable 基类（ctor 反编译实证：Uncopyable::Uncopyable(this)）

// 互斥锁封装：本体即 pthread_mutex_t（ctor：pthread_mutexattr_settype(attr,1)
// → pthread_mutex_init(this,attr) → attr destroy；isTaken 恒 false【已报告】；
// tryEnter = pthread_mutex_trylock(this)!=0）。
class SectionLock : public Uncopyable
{
public:
    SectionLock();                      // T 0x08ad3742
    ~SectionLock();                     // T 0x08ad3786
    void enter();                       // T 0x08ad37a4（pthread_mutex_lock）
    bool tryEnter();                    // T 0x08ad37b8（返回 trylock!=0）
    void leave();                       // T 0x08ad37d0（pthread_mutex_unlock）
    bool isTaken();                     // T 0x08ad37e4（恒 false）
    bool isTakenByCurrentThread();      // T 0x08ad37ee

private:
    pthread_mutex_t m_mutex;   // 本体（ORIG this 即 mutex 指针）
};

#endif  // DNF_GAME_SECTIONLOCK_H_
