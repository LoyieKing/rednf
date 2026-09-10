// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDungeonExpStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CDungeonExpStatistic::InsertDatabase。
// ORIG 0x0860f110 T（_ZN20CDungeonExpStatistic14InsertDatabaseEv）。
// 报告：docs/class_func_reports/CDungeonExpStatistic/InsertDatabase.md
// 语义：统计表空则直接返回；否则从 s_stream_pool 取流（Statistics.cpp:0x59c），
//   写消息头 0x116、-1、表 size，再遍历每节点输出
//   (level, m_exp, m_count, m_clearTimes) 四字段，
//   最后 put(LOGDB_Q) 入库并 clear 统计表。
// IncreaseDungeonExp 实现在 PartyClosureC2.cpp（ORIG 0x0860efca）。
// ============================================================================
#include "CDungeonExpStatistic.h"

#include "AdditionalDungeonInfo.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ============================================================================
// AdditionalDungeonInfo::~AdditionalDungeonInfo —— ORIG D1/D2 0x08a6887c（W，空体）。
// 非多态 POD（8 字节两 int，无 vtable/typeinfo）；属主 TU 提供真实空析构，
// 供 vector<AdditionalDungeonInfo>（QuestScript::m_field16c）析构实例化引用。
// ============================================================================


#include "CDungeonExpStatistic/InsertDatabase.inc"
}
