// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_AccountCargoSync/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// DB 同步 stub 类实现（batch-5 迁移）。
// 6 个 makeRequest 依据 class_func_reports 对应 md + 同族 DB packet 模式实现：
// 包构建约定一致——StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard
// （写 src/字段/header opcode）→ MsgQueueMgr::put(MsgQueueMgr::DB_Q(==2), guard)。
// 各函数 ORIG 地址见注释；字段顺序/长度直接来自反编译 C 与汇编。

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "DB_AccountCargoSync.h"
#include "MySQL.h"
#include "CInventory.h"       // NumberToString(unsigned int, int)
#include "CEnvironment.h"     // DB_HANDLE_2 / E_SERVER_GROUP_NONE
#include <cstring>
#include "GameBasicsScripts.h"   // DB_UpdateAuraAvatarOption
#include "DBMgr.h"           // DBMgr::GetDBHandle
#include "DB_AccountCargoSync/dispatch.inc"


#include "DB_AccountCargoSync/makeRequest.inc"


#include "DB_AccountCargoSync/makeRequest_1.inc"


#include "DB_AccountCargoSync/makeRequest_2.inc"


#include "DB_AccountCargoSync/makeRequest_3.inc"


#include "DB_AccountCargoSync/makeRequest_4.inc"


#include "DB_AccountCargoSync/makeRequest_5.inc"


#include "DB_AccountCargoSync/makeRequest_6.inc"


#include "DB_AccountCargoSync/makeRequest_7.inc"


// DB_UpdateAuraAvatarOption::makeRequest 权威实现见 W8LT_DBTask.cpp（FixCall_D 统一）。

namespace APSystem
{
#include "DB_AccountCargoSync/makeRequest_8.inc"
  // TODO(G2)
}