#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r BattleData 还原（G2-5 批次）。
// CParty +0x32c，总尺寸 0x7f8。符号（nm -C，W/T 全实现）：
//   C1 0x082a4cea / D1 0x082a54d6 / Reset 0x082a4e8a /
//   GetHellPartyValueTotal 0x0822d7c4 / SetHellPartyValueTotal 0x0822d7b2 /
//   GetTotalKilledMonsterCount 0x085bf456 /
//   IncEventDungeonDestoryObjectPoint 0x085be6e2 /
//   ResetEventDungeonClearPoint 0x085be6ba /
//   ClearEventDungeonRoom 0x085be7aa / IsClearEventDungeonRoom 0x085be862
// 布局（由 Reset/构造/析构反汇编推导）：
//   +0x00 int[10] 字段区、+0x28 char[4]、+0x2c int[9]（含 3 个击杀计数）、
//   +0x50 char[4]x5、+0x64 int[4]、
//   +0x74 Inven_Item[4][2]（0x3d 每项）、+0x25c Inven_Item[4][2]、
//   +0x444 Inven_Item[4][2]、+0x62c stEventClearData[12]（0x8 每项）、
//   +0x68c int、+0x690 GameResultType[4]（0x50 每项）、+0x7d0 int[4]、
//   +0x7e0 float m_hellPartyValueTotal、+0x7e4 char[4]、+0x7e8 int[4]。
// Inven_Item/GameResultType 跨类方法以 extern asm 标签引用（ORIG 真实符号）。

#include <string.h>

#include "BattleData.h"
#include "Inven_Item.h"
#include "GameResultType.h"

// ============================================================================
// 构造 / 析构 / Reset
// ============================================================================

#include "BattleData/BattleData.inc"

#include "BattleData/dtor_BattleData.inc"

// ORIG 0x82a4e90 头部清零为 4.1 dest 缓存形：每对字段一次 this 载入+双 store
//（0x0..0x24 五对、0x2c..0x48 四对、0x4c 单、0x28..0x2b 字节逐条、0x68c 单）。
// 4.4 函数体逐条重载 this 不可造（探针 /tmp/l0_probe2.cpp 逐句/链式/逗号三形
// 实测全不复现）——经 8 字节清零视图折叠复现（探针 /tmp/l0_probe4.cpp ResetG
// 与 /tmp/l0_probe3.cpp ResetF 同形实证）。视图仅表达布局，字段语义仍以头内
// m_fieldN/m_killedMonsterN 为权威。
namespace {
struct BattleDataZeroView {
    long long a[5];   // +0x00 m_field0/4 .. m_field20/24
    char pad28[4];    // +0x28 m_flag28[0..3]（字节域另逐字节清）
    long long b[4];   // +0x2c m_field2c/30 .. m_killedMonster2/m_field48
    int c;            // +0x4c m_field4c
};
} // namespace

#include "BattleData/Reset.inc"

#include "BattleData/GetHellPartyValueTotal.inc"

#include "BattleData/SetHellPartyValueTotal.inc"

#include "BattleData/GetTotalKilledMonsterCount.inc"

#include "BattleData/IncEventDungeonDestoryObjectPoint.inc"

#include "BattleData/ResetEventDungeonClearPoint.inc"

#include "BattleData/ClearEventDungeonRoom.inc"

#include "BattleData/IsClearEventDungeonRoom.inc"
