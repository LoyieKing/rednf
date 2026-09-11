#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 IsEquipable/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CUser.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "CSkill.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include <algorithm>   // std::max<int>(int const&, int const&) ORIG 808e218

// [B01 铁律1] 原 `int _S_CHARAC_JOB_MASTARY[0xC0]`（外部链接 D 768B）为发明符号：
// ORIG `nm -S --defined-only` 无裸名，真形是 489 份 TU-local `_ZL21_S_CHARAC_JOB_MASTARY`
// （r 220B = 55×int，如 0x8ae5b60 / 0x8b126e0 …），成因即 CInventory.h 内 file-scope
// static 被多 TU include。本 TU 的 IsEquipable()（ORIG 0x085003be）全程不引用该表
// （索引点 checkEquipmentState 在 ORIG 同 TU 0x0850cfbe/0x0850cfe6，我方落 CInventory.cpp）
// ⇒ 此处删除；TU-local 副本由 CInventory.h:580 的静态定义在引用者 CInventory.cpp 发射。

#include "IsEquipable/IsEquipable.inc"
