// df_game_r 还原 —— DB_EventSaveGrowthWeapon（成长武器活动礼包领取 DB 请求）。
// 逐函数对照 docs/class_func_reports/DB_EventSaveGrowthWeapon/makeRequest.md
// （ORIG 0x084493e8）。语义：向 DB 队列发一条请求，表头 0x313 + -1，正文为
// accId(uint)、serverGroup(short)；Acquire 行号取 ORIG 常量 "DBThread.cpp" 0xb8ef。
// 队列 put 的 QUEUE_IDX = 2（DB_Q）。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DB_EventSaveGrowthWeapon.h"

// ============================================================================
// DB_EventSaveGrowthWeapon::makeRequest @ 0x084493e8
// ============================================================================
void DB_EventSaveGrowthWeapon::makeRequest(unsigned int accId, short serverGroup)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb8ef), true);
    guard.operator*()->operator<<(0x313);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(serverGroup); // operator<<(short) 0x0822ad66
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
