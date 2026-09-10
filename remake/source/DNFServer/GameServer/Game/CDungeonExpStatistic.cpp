// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeonExpStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
