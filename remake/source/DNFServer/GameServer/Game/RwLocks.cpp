// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RwLocks/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "RwLocks/RwSpinLock.inc"

#include "RwLocks/enterRead.inc"

#include "RwLocks/leaveRead.inc"

#include "RwLocks/enterWrite.inc"

#include "RwLocks/leaveWrite.inc"

#include "RwLocks/isReadTaken.inc"

#include "RwLocks/isWriteTaken.inc"

#include "RwLocks/enterReadToValue.inc"
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

#include "RwLocks/leaveReadToValue.inc"
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

#include "RwLocks/enterWriteToValue.inc"
{
    while (true)
    {
        long ostate = *value;
        // [b28d-41 判词·N3] DIFF：唯一残差 = ORIG CAS 后 `cmp;jne/mov $1;jmp/mov $0;
        // test %al,%al` 的 4.1 寄存器 select 物化（P56 分支返回形）vs 4.4 `sete %al;
        // test`；帧/跳转布局与其余 28 条已全等。b28c-lane25 探针 /tmp/l25probe/rw.cpp
        // fA/fB/fC 三形全败 + 本轮复核（N3 choose-without-store 负例，非 O0 可复现；
        // 无类型旋钮可试，比较两侧均 long）。
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

#include "RwLocks/leaveWriteToValue.inc"
{
    __sync_lock_test_and_set(value, *value & 0x40000000);
}

// ---- SectionLock（ORIG T 0x08ad3742..0x08ad37f7）----
#include "RwLocks/SectionLock.inc"
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

#include "RwLocks/dtor_SectionLock.inc"

#include "RwLocks/enter.inc"

#include "RwLocks/tryEnter.inc"

#include "RwLocks/leave.inc"

#include "RwLocks/isTaken.inc"

#include "RwLocks/isTakenByCurrentThread.inc"

// ---- RwSectionLock（ORIG T 0x08ad37f8..0x08ad389b；报告缺失，objdump 转写）----
#include "RwLocks/RwSectionLock.inc"
{
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlock_init(&m_rwlock, &attr);
    pthread_rwlockattr_destroy(&attr);
}

#include "RwLocks/dtor_RwSectionLock.inc"

#include "RwLocks/enterRead_1.inc"

#include "RwLocks/leaveRead_1.inc"

#include "RwLocks/enterWrite_1.inc"

#include "RwLocks/leaveWrite_1.inc"
