// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGuildAgitManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CGuildAgitManager（公会驻地分区管理器，G2-4 批次）。
// 无独立类报告：逐函数对照 ORIG 反汇编（ctor 0x856f2c6 / dtor 0x856f146 /
// AllocGuildAgitArea 0x82aa41c / SetGuildAgitAreaDomain 0x86d2642 /
// GetGuildAgitAreaIndex 0x86d2666 / ReleaseGuildAgitArea 0x86d268a）实现，
// 数据流参照 Inter_Notice*GuildAgit 系列报告。
// 布局（与 CSchoolMgr/CSchoolArea 同构）：
//   CGuildAgitManager：+0x00 std::map<uint,uint> m_guildCountMap
//                      +0x18 CGuildAgitArea m_area（sizeof 0x28）
//   CGuildAgitArea：+0x00 std::map<uint,int> m_areaMap
//                   +0x18 std::stack<int> m_areaStack
// 每个驻地占用 4 个连续区号（GetArea 一次弹 4 个，Release 回填 4 个）。
#include <map>
#include <stack>
#include <deque>

#include "CGuildAgitManager.h"

// CGuildAgitArea / CGuildAgitManager 类声明见 CGuildAgitManager.h（唯一声明点）

// ============================================================================
// CGuildAgitArea 实现
// ============================================================================

#include "CGuildAgitManager/CGuildAgitArea.inc"


#include "CGuildAgitManager/dtor_CGuildAgitArea.inc"


#include "CGuildAgitManager/AllocGuildAgitArea.inc"


#include "CGuildAgitManager/GetArea.inc"


#include "CGuildAgitManager/SetAreaMapping.inc"


#include "CGuildAgitManager/SetGuildAgitAreaDomain.inc"


#include "CGuildAgitManager/GetGuildAgitAreaIndex.inc"


#include "CGuildAgitManager/ReleaseGuildAgitArea.inc"


// ============================================================================
// CGuildAgitManager 实现
// ============================================================================

#include "CGuildAgitManager/CGuildAgitManager.inc"


#include "CGuildAgitManager/dtor_CGuildAgitManager.inc"


#include "CGuildAgitManager/AllocGuildAgitArea_1.inc"


#include "CGuildAgitManager/SetGuildAgitAreaDomain_1.inc"


#include "CGuildAgitManager/GetGuildAgitAreaIndex_1.inc"


#include "CGuildAgitManager/ReleaseGuildAgitArea_1.inc"

