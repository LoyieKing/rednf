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

// ============================================================================
// DB_UpdateInventoryCapacity::dispatch @ 0x08444bea
// mangled _ZN26DB_UpdateInventoryCapacity8dispatchEiiP6Stream
// ============================================================================
// 报告 docs/class_func_reports/DB_UpdateInventoryCapacity/dispatch.md 全文：
// 解包 serverGroup(int) → GetDBHandle(DB_HANDLE_3, serverGroup) → 解包
// characNo(uint)/capacity(int) → set_query("upDate inventory set
// inventory_capacity=%d where charac_no=%u", capacity, characNo) → exec(true)；
// 失败或 affectedRowCount==0 → cMyTrace(__PRETTY_FUNCTION__, 0xabc8, 5) 记录同
// 款 SQL 后返 0；成功返 1。
bool DB_UpdateInventoryCapacity::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    int serverGroup = 0;      // -0x20（先于 GetDBHandle 解包）
    *stream >> serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_3, static_cast<ENUM_SERVER_GROUP>(serverGroup));
    unsigned int characNo = 0;  // -0x24
    int capacity = 0;           // -0x28
    *stream >> characNo;
    *stream >> capacity;
    db->set_query("upDate inventory set inventory_capacity=%d where charac_no=%u",
                  capacity, characNo);
    if (db->exec(true) == 1 && db->getAffectedRowCount() != 0)
        return 1;
    cMyTrace trace(__PRETTY_FUNCTION__, 0xabc8, 5);
    trace("upDate inventory set inventory_capacity=%d where charac_no=%u",
          capacity, characNo);
    return 0;
}
