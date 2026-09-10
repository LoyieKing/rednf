// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PerfMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// PerfMgr.cpp — 与 df_game_r ORIG 对照实现。
// Evidence: docs/class_func_reports/PerfMgr/{inc,get,clear}.md
//   inc  0x085c3c44: 读 (this+idx*4) → lea +1 → 写回
//   get  0x085c3c5c: 返回 (this+idx*4)
//   clear 0x085c3c6a: for(i=0; i<=3; ++i) (this+i*4) = 0（cmpl $0x3; setle）
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN7PerfMgr3incENS_8TASK_IDXE`（ORIG 0x85c3cd7）；inc 已是首强符号。
#include <iostream>

#include "PerfMgr.h"

#include "PerfMgr/inc.inc"
{
    ++m_count[idx];
}

#include "PerfMgr/get.inc"
{
    return m_count[idx];
}

#include "PerfMgr/clear.inc"
{
    for (int i = TASK_IDX_BEGIN; i < TASK_IDX_END; ++i)
    {
        m_count[i] = 0;
    }
}
