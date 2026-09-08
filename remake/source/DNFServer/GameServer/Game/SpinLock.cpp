// df_game_r Game/ SpinLock + Uncopyable 实现（独立 TU，从 GameStubs.cpp 拆出）。
// SpinLock 声明在 CEnvironment.h；Uncopyable 声明在 GameTypes.h。
// 实现与 df_game_r 反编译逐函数比对（identical）：
//   enter 用 xchg 自旋（__sync_lock_test_and_set），leave 用 xchg 清零。
#include "CEnvironment.h"
#include "GameTypes.h"

// ---- YieldProcessor（ORIG 0x8ad351c，空函数）----
static void YieldProcessor() {}

// ---- SpinLock ----
// P210 前置：本 TU 的 Uncopyable ctor/dtor 定义放在 SpinLock C1/D1 之后，
// 使 4.4 对未可见 callee 采用保守帧（ORIG sub $0x18）。
SpinLock::SpinLock() : Uncopyable() { m_lock = 0; }
Uncopyable::Uncopyable() {}

SpinLock::~SpinLock() {}
Uncopyable::~Uncopyable() {}

void SpinLock::enter()
{
    while (__sync_lock_test_and_set(&m_lock, 1) == 1)
        YieldProcessor();
}

void SpinLock::leave()
{
    __sync_lock_test_and_set(&m_lock, 0);
}

bool SpinLock::isTaken() { return m_lock == 1; }

bool SpinLock::tryEnter() { return __sync_lock_test_and_set(&m_lock, 1) == 0; }
