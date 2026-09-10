// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_ReqItemUpgradeLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_ReqItemUpgradeLog（强化/增幅 DB 日志请求，LOGDB 派发）。
// 逐函数对照 docs/class_func_reports/DB_ReqItemUpgradeLog/makeRequest.md。
// 语义：向 LOGDB 队列发一条强化日志请求，表头 0xc0，正文为
// (a,b,c) 三个 uint、d uint、e bool、str char*（ORIG 0x08425ece）。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DB_ReqItemUpgradeLog.h"

// ============================================================================
// DB_ReqItemUpgradeLog::makeRequest @ 0x08425ece
// ============================================================================
#include "DB_ReqItemUpgradeLog/makeRequest.inc"
                                       unsigned int c, const char* str,
                                       unsigned int d, bool e)
{
    // [b28c-L29] ORIG 0x08425ece：Acquire 返回值直接作 CStreamGuard ctor 实参
    // （无具名 stream 局部槽，P0）——guard 槽 -0x10 与 ORIG 一致。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63f7),
                       true);
    guard.operator*()->operator<<(0xc0);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(a);
    guard.operator*()->operator<<(b);
    guard.operator*()->operator<<(c);
    guard.operator*()->operator<<(d);
    guard.operator*()->operator<<(e);
    // ORIG：param_4 为 char*，直接写入（const_cast 适配本声明）。
    guard.operator*()->operator<<(const_cast<char*>(str));
    // ORIG 队列 idx=4（LOGDB_Q）；MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)。
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
}
