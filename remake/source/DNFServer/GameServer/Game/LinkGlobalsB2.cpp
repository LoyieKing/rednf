// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinkGlobalsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkGlobalsB2.cpp —— df_game_r 还原（ScriptFileAndGlobalsB2 批次独占 TU）。
//
// 本 TU 独家提供（勿在他处重复定义；Main 统一登记 CMake）：
//   advancealtar::isValidRidableId(advancealtar::RidableId::T)
//     ORIG T _ZN12advancealtar16isValidRidableIdENS_9RidableId1TE @ 0x08898ff0
//     （原定义在未登记的 advancealtar.cpp，本批次迁出为唯一定义点。）
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
}

}  // namespace advancealtar
