// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CLevelDungeonPlayStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [b28d-38 判词] CLevelDungeonPlayStatistic::InsertDatabase 0x0860edf4 DIFF：
//  p167 CLEAR 28/28（语义完成）；残差 N18a 单段直落（ORIG mov 0x8;mov 0x4;mov 双段）。
// df_game_r 还原 —— CLevelDungeonPlayStatistic::InsertDatabase。
// ORIG 0x0860edf4 T（_ZN26CLevelDungeonPlayStatistic14InsertDatabaseEv）。
// 报告：docs/class_func_reports/CLevelDungeonPlayStatistic/InsertDatabase.md
// 语义：统计表空则直接返回；否则从 s_stream_pool 取流（Statistics.cpp:0x569），
//   写消息头 0x107、-1、表 size，再遍历每节点输出
//   (level, m_userCount, m_playCount) 三字段，
//   最后 put(LOGDB_Q) 入库并 clear 统计表。
// IncreaseLevelDungeonPlay 实现在 GlobalData.cpp（ORIG 0x0860ecc6）。
// ============================================================================
#include "CLevelDungeonPlayStatistic.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// [b28c-32] verdict=DIFF：N18a（ORIG it->second 成员读发 8 字节两段载入，4.4 恒折单条）。
#include "CLevelDungeonPlayStatistic/InsertDatabase.inc"
}
