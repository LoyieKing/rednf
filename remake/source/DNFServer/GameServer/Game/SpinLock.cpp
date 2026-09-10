// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SpinLock/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "SpinLock/SpinLock.inc"
#include "SpinLock/Uncopyable.inc"

#include "SpinLock/dtor_SpinLock.inc"
#include "SpinLock/dtor_Uncopyable.inc"

#include "SpinLock/enter.inc"
{
    while (__sync_lock_test_and_set(&m_lock, 1) == 1)
        YieldProcessor();
}

#include "SpinLock/leave.inc"
{
    __sync_lock_test_and_set(&m_lock, 0);
}

#include "SpinLock/isTaken.inc"

#include "SpinLock/tryEnter.inc"
