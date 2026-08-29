// df_game_r Game/ RwSpinLock + SectionLock + RwSectionLock 实现（独立 TU，仅 ORIG 符号）。
// 证据：docs/class_func_reports/RwSpinLock/（11 函数报告）、
// docs/class_func_reports/SectionLock/（7 函数报告）；
// RwSectionLock 无报告目录 → nm 定址后 objdump -d 0x08ad37f8..0x08ad38a0 转写（§3.2 标注报告缺失）。
// RwSpinLock state 位布局：0x80000000=写持有，0x40000000=写等待，低 30 位=读计数；
// enter/leave 原子 CAS 自旋，失败 YieldProcessor 重试（ORIG 0x8ad363a/0x8ad36be 实证）。
#include "RwSpinLock.h"
#include "RwSectionLock.h"
#include "SectionLock.h"

// ---- YieldProcessor（ORIG 0x8ad351c，空函数；static 每 TU 一份，与 SpinLock.cpp 同做法）----
static void YieldProcessor() {}

// ---- RwSpinLock（ORIG T 0x08ad35a6..0x08ad3741）----
RwSpinLock::RwSpinLock() : Uncopyable() { state_ = 0; }

void RwSpinLock::enterRead() { enterReadToValue(&state_); }

void RwSpinLock::leaveRead() { leaveReadToValue(&state_); }

void RwSpinLock::enterWrite() { enterWriteToValue(&state_); }

void RwSpinLock::leaveWrite() { leaveWriteToValue(&state_); }

bool RwSpinLock::isReadTaken() { return (state_ & 0x3fffffff) != 0; }

bool RwSpinLock::isWriteTaken() { return (state_ & 0xc0000000) != 0; }

void RwSpinLock::enterReadToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        if ((ostate & 0xc0000000) == 0)
        {
            if (__sync_val_compare_and_swap(value, ostate, ostate + 1) == ostate)
                return;
        }
        YieldProcessor();
    }
}

void RwSpinLock::leaveReadToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        if (__sync_val_compare_and_swap(
                value, ostate, ((ostate & 0x3fffffff) - 1) | (ostate & 0x40000000)) == ostate)
            return;
        YieldProcessor();
    }
}

void RwSpinLock::enterWriteToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        // ORIG CAS 后用 cmp;jne/mov $1/jmp/mov $0/test 寄存器 select 物化；
        // -O0 只能产 sete 物化（负例 N3 choose-without-store，非 O0 可复现），帧/跳转布局已对齐。
        if (ostate == 0 || ostate == 0x40000000)
        {
            if (__sync_val_compare_and_swap(value, ostate, 0x80000000) == ostate)
                break;
        }
        if ((ostate & 0x40000000) == 0)
            __sync_val_compare_and_swap(value, ostate, ostate | 0x40000000);
        YieldProcessor();
    }
}

void RwSpinLock::leaveWriteToValue(long volatile* value)
{
    __sync_lock_test_and_set(value, *value & 0x40000000);
}

// ---- SectionLock（ORIG T 0x08ad3742..0x08ad37f7）----
SectionLock::SectionLock() : Uncopyable()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

SectionLock::~SectionLock() { pthread_mutex_destroy(&m_mutex); }

void SectionLock::enter() { pthread_mutex_lock(&m_mutex); }

bool SectionLock::tryEnter() { return pthread_mutex_trylock(&m_mutex) != 0; }

void SectionLock::leave() { pthread_mutex_unlock(&m_mutex); }

bool SectionLock::isTaken() { return false; }

bool SectionLock::isTakenByCurrentThread() { return false; }

// ---- RwSectionLock（ORIG T 0x08ad37f8..0x08ad389b；报告缺失，objdump 转写）----
RwSectionLock::RwSectionLock() : Uncopyable()
{
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlock_init(&m_rwlock, &attr);
    pthread_rwlockattr_destroy(&attr);
}

RwSectionLock::~RwSectionLock() { pthread_rwlock_destroy(&m_rwlock); }

void RwSectionLock::enterRead() { pthread_rwlock_rdlock(&m_rwlock); }

void RwSectionLock::leaveRead() { pthread_rwlock_unlock(&m_rwlock); }

void RwSectionLock::enterWrite() { pthread_rwlock_wrlock(&m_rwlock); }

void RwSectionLock::leaveWrite() { pthread_rwlock_unlock(&m_rwlock); }
