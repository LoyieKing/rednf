// df_game_r 还原 —— DB_EventUpdateGrowthWeaponCharac（成长武器活动角色选择 DB 请求）。
// 逐函数对照 docs/class_func_reports/DB_EventUpdateGrowthWeaponCharac/makeRequest.md
// （ORIG 0x084495c6）。语义：向 DB 队列发一条请求，表头 0x314 + -1，正文为
// accId(uint)、characNo(uint)、addInfo(int)、serverGroup(short)；
// Acquire 行号取 ORIG 常量 "DBThread.cpp" 0xb914。队列 put 的 QUEUE_IDX = 2（DB_Q）。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DB_EventUpdateGrowthWeaponCharac.h"

// ============================================================================
// DB_EventUpdateGrowthWeaponCharac::makeRequest @ 0x084495c6
// ============================================================================
void DB_EventUpdateGrowthWeaponCharac::makeRequest(unsigned int accId,
                                                   unsigned int characNo,
                                                   int addInfo, short serverGroup)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb914), true);
    guard.operator*()->operator<<(0x314);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(characNo);    // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(addInfo);     // operator<<(int) 0x080c8c56
    guard.operator*()->operator<<(serverGroup); // operator<<(short) 0x0822ad66
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
