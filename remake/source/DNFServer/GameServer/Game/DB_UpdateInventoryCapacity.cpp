// df_game_r 还原 —— DB_UpdateInventoryCapacity（背包容量更新 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_UpdateInventoryCapacity/makeRequest.md（ORIG 0x08444d12）。
// 语义：向 DB 队列发一条请求，表头 0x2ae + -1，正文为 serverGroup、characNo(uint)、
// capacity；dispatch（0x08444bea）读 serverGroup→GetDBHandle(3)→执行
// "upDate inventory set inventory_capacity=%d where charac_no=%u"。第 2 个 int 入参在
// ORIG 汇编中不写入流，按 ORIG mangled 签名保留。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DB_UpdateInventoryCapacity.h"

// ============================================================================
// DB_UpdateInventoryCapacity::makeRequest @ 0x08444d12
// ============================================================================
void DB_UpdateInventoryCapacity::makeRequest(ENUM_SERVER_GROUP serverGroup,
                                             int /*uid*/, unsigned int characNo,
                                             int capacity)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xabd0);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x2ae);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(static_cast<int>(serverGroup));
    guard.operator*()->operator<<(characNo);   // 走 operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(capacity);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
