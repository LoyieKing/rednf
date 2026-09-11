// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_UpdateInventoryCapacity/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_UpdateInventoryCapacity（背包容量更新 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_UpdateInventoryCapacity/makeRequest.md
// （ORIG 0x08444d12）与 dispatch.md（ORIG 0x08444bea）。
// 语义：makeRequest 向 DB 队列发一条请求，表头 0x2ae + -1，正文为 serverGroup、
// characNo(uint)、capacity；dispatch 读 serverGroup→GetDBHandle(3)→执行
// "upDate inventory set inventory_capacity=%d where charac_no=%u"。makeRequest 的
// 第 2 个 int 入参（uid）在 ORIG 汇编中不写入流，按 ORIG mangled 签名保留。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "DBMgr.h"           // DBMgr::GetDBHandle（dispatch）
#include "GlobalData.h"
#include "DB_UpdateInventoryCapacity.h"
#include "MySQL.h"           // MySQL::{set_query,exec,getAffectedRowCount}
#include "Stream.h"          // Stream::operator>>
#include "GameTypes.h"       // cMyTrace

#include "DB_UpdateInventoryCapacity/makeRequest.inc"


#include "DB_UpdateInventoryCapacity/dispatch.inc"

