// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkGlobalsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN12advancealtar16isValidRidableIdENS_9RidableId1TE`（ORIG 0x889b937）；
// 该函数已是本 TU 首强符号。
#include <iostream>


#include "advancealtar.h"

namespace advancealtar
{

// ORIG 0x08898ff0（报告 docs/class_func_reports/advancealtar/isValidRidableId.md
// ## 汇编：mov 0x8(%ebp),%eax; cmp $0x1,%eax; sete）：
// 仅 RidableId::RIDEABLE（值 1）有效。ORIG 比较为 reg 形（mov 0x8(%ebp),%eax;
// cmp $0x1,%eax），按 P209 源侧保持 unsigned 语义触发 4.4 载寄存器。
#include "LinkGlobalsB2/isValidRidableId.inc"
{
    if ((unsigned int)ridableId == 1u)
        return true;
    return false;
}

}  // namespace advancealtar
