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
void DB_ReqItemUpgradeLog::makeRequest(unsigned int a, unsigned int b,
                                       unsigned int c, const char* str,
                                       unsigned int d, bool e)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63f7);
    CStreamGuard guard(stream, true);
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
