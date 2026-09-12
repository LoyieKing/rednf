// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHackLog_OverDamage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CHackLog_OverDamage.h"
#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE 权威枚举
#include "CUser.h"
#include "CParty.h"     // m_dungeon / m_battleField 视图
#include "CDungeon.h"   // get_min_level/get_index
#include "CBattle_Field.h"   // get_dungeon_diff

// [R12fixG2 修正 · ④#5] ORIG @0x08c10c68 raw 逐项（7×u32）：
//   58 02 00 00 | 08 07 00 00 | 10 0e 00 00 | a8 61 00 00 | d8 d6 00 00 | 70 11 01 00 | 60 ae 0a 00
//   = 600 | 1800 | 3600 | 25000 | 55000 | 70000 | 700000
// 旧 [5]=70064(0x111b0) / [6]=712640(0xadfc0) 与 raw 不符。索引与 ORIG 一致：
// GetBossTowerLimitDamage[5]（ORIG 0x82869af `mov 0x8c10c7c`）、GetDeathTowerLimitDamage[6]
// （0x8286987 `mov 0x8c10c80`）、GetDungeonLimitMaxDamage[6]、GetDungeonLimitDamageAndHackType
// sectionIdx∈[0,6]（ORIG 初值 6 + LEVEL_SECTION_TABLE 值域 0..5）。
const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_CONST[7] = {
    600, 1800, 3600, 25000, 55000, 70000, 700000
};   // hex {258,708,e10,61a8,d6d8,11170,aae60}

// ORIG _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO（objdump 实测）：
// {2.0,1.8,1.7,1.6,1.0}——第 3/4 项原抄错为 0.075/0.025，已按 ORIG 修正。
const double _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[5] = {
    2.0, 1.8, 1.7, 1.6, 1.0
};

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[7] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b
};

// [R12fixG2 修正 · ④#6] ORIG @0x08c10d00 raw 全 89 项逐值核对 = {0×9,1×10,2×10,3×18,4×8,5×34}；
// 旧版显式仅 88 项（5 只写 33 个）⇒ [88] 被隐式补 0，与 ORIG 末项 5 分叉（level 89 档 sectionIdx：
// ORIG=5 / 旧=0；raw 末 16 字节 = `05 00 00 00` ×4）。补足第 89 项。
const int LEVEL_SECTION_TABLE[89] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

#include "CHackLog_OverDamage/GetDungeonLimitDamageAndHackType.inc"


#include "CHackLog_OverDamage/GetDeathTowerLimitDamage.inc"


#include "CHackLog_OverDamage/GetDeathTowerOverDamageHackType.inc"


#include "CHackLog_OverDamage/GetDungeonLimitMaxDamage.inc"


#include "CHackLog_OverDamage/GetHackTypeOverLimitMaxDamage.inc"


#include "CHackLog_OverDamage/GetBossTowerLimitDamage.inc"


#include "CHackLog_OverDamage/GetBossTowerOverDamageHackType.inc"


