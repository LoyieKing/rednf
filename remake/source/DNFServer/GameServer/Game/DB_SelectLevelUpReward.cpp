// df_game_r 还原 —— DB_SelectLevelUpReward（升级奖励领取确认 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_SelectLevelUpReward/makeRequest.md。
// 语义：makeRequest（0x0844271a）向 DB 队列 2 发一条请求，表头 0x28e + 3 参数
// （uid、characNo、level）；流来源 StreamPool::Acquire("DBThread.cpp", 0xa2c6)。
#include "DB_SelectLevelUpReward.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "Stream.h"

// ============================================================================
// DB_SelectLevelUpReward::makeRequest @ 0x0844271a
// mangled _ZN22DB_SelectLevelUpReward11makeRequestEiji
// ============================================================================
void DB_SelectLevelUpReward::makeRequest(int uid, unsigned int characNo,
                                         int level)
{
    // ORIG：Acquire 返回值直接作为 CStreamGuard 构造实参（无中间变量）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa2c6), true);
    guard.operator*()->operator<<(0x28e);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(level);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(,2,)
}
