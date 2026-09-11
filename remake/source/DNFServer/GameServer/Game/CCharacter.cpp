// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCharacter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CCharacter.h"

#include "CCharacter/STSecondGrowTypeInfo.inc"


#include "CCharacter/dtor_STSecondGrowTypeInfo.inc"


#include "CCharacter/CCharacter.inc"


#include "CCharacter/dtor_CCharacter.inc"


#include "CCharacter/get_give_skill_firstGrowtype.inc"


#include "CCharacter/get_give_skill_secondGrowtype.inc"


#include "CCharacter/get_growtype_stat.inc"


#include "CCharacter/get_give_skill.inc"


// ============================================================================
// 以下为 CharacterAIA2 批次追加（报告 docs/class_func_reports/CCharacter/
// {CCharacter.md,~CCharacter.md} 完整转写；destroy/set_data/clear 报告缺失
// → objdump 转写，ORIG nm：destroy T 0x083485c4 / clear T 0x0834852e /
//   set_data T 0x08348894）。
// ============================================================================
#include "STCharacterScript.h"

#include "CCharacter/clear.inc"


#include "CCharacter/destroy.inc"


#include "CCharacter/clear_1.inc"


#include "CCharacter/set_data.inc"

