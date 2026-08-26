// df_game_r 还原 —— DB_UpdateInGameEventHistory（游戏内事件历史更新 DB 请求，
// DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_UpdateInGameEventHistory/makeRequest.md。
// 语义：makeRequest（ORIG 0x08444ac4）向 DB 队列 2 发一条请求，流内容为
// 表头 opcode 0x2ad + 固定 -1 + accountId、eventType、history1、history2；
// 流来源 StreamPool::Acquire("DBThread.cpp", 0xaba8)，guard 持有所有权（RAII 析构归还）。
#include "DB_UpdateInGameEventHistory.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"

// ============================================================================
// DB_UpdateInGameEventHistory::makeRequest @ 0x08444ac4
// mangled _ZN27DB_UpdateInGameEventHistory11makeRequestEjiii
// ============================================================================
void DB_UpdateInGameEventHistory::makeRequest(unsigned int accountId,
                                              int eventType, int history1,
                                              int history2)
{
    // ORIG：Acquire 返回值直接作为 CStreamGuard 构造实参（无中间变量）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xaba8), true);
    guard.operator*()->operator<<(0x2ad);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accountId);
    guard.operator*()->operator<<(eventType);
    guard.operator*()->operator<<(history1);
    guard.operator*()->operator<<(history2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(,2,)
}
