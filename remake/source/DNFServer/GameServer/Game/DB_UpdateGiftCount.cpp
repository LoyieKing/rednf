// DB_UpdateGiftCount / DB_UpdateBreakAwayAccruedCera 实现（W7 链接收敛）。
// 逐函数按 class_func_reports/{DB_UpdateGiftCount,DB_UpdateBreakAwayAccruedCera}/
// makeRequest.md、dispatch.md 转写；包构建约定与 DB_AccountCargoSync.cpp 一致：
// StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard → MsgQueueMgr::put(DB_Q)。

#include "DB_UpdateGiftCount.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CInventory.h"      // NumberToString 声明（定义 LinkFinalDeps.cpp）

// ============================================================================
// DB_UpdateGiftCount::makeRequest ORIG @ 0x084271ec
//   Acquire("DBThread.cpp", 0x6841) | opcode 0xd1 | -1 | accId(uint) | count(int)
// ============================================================================
void DB_UpdateGiftCount::makeRequest(unsigned int accId, int count)
{
    // [b28d-53 P259] ORIG 无 stream 具名局部：Acquire 返回值 eax 直传 guard ctor
    // 的 0x4(%esp)；具名缓存多 1 store+1 load 并使 guard 槽 -0x10→-0x14。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6841), true);
    guard.operator*()->operator<<(0xd1);              // SIG_UPDATE_GIFT_COUNT
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(count);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateGiftCount::DB_UpdateGiftCount ORIG W C1/C2 @ 0x0844e756：
// 仅调用基类 ctor 并设 vptr —— 与空派生体一致。
// ============================================================================
DB_UpdateGiftCount::DB_UpdateGiftCount()
{
}

// ============================================================================
// DB_UpdateGiftCount::~DB_UpdateGiftCount ORIG D1 @ 0x0845eac4 / D0 @0x0845eaf4：
// 设 vptr + 析构基类（无成员），delete 分支按 ORIG 同型由编译器展开。
// ============================================================================
DB_UpdateGiftCount::~DB_UpdateGiftCount()
{
}

// ============================================================================
// DB_UpdateGiftCount::dispatch ORIG T @ 0x08427112 [dispatch.md]
//   read u32 cnt + i32 id → MySQL(6, group==0)：
//   "upDate member_login set last_gift_time=unix_timestamp(now()),
//    gift_cnt=gift_cnt+%d where m_id=%s" % (id, NumberToString(cnt,0))
//   → exec(true)；返回 1。
// ============================================================================
bool DB_UpdateGiftCount::dispatch(int userIdx, int slot, Stream* stream)
{
    (void)userIdx;
    (void)slot;
    unsigned int mId = 0;      // local_14：Stream >> (uint&)
    int giftCnt = 0;           // local_18：Stream >> (int&)

    stream->operator>>(mId);
    stream->operator>>(giftCnt);

    MySQL* handle = GlobalData::s_db_mgr->GetDBHandle(
        static_cast<ENUM_DB_HANDLE_IDX>(6), static_cast<ENUM_SERVER_GROUP>(0));

    char midBuf[20];                       // local_2c[20]
    memset(midBuf, 0, sizeof(midBuf));
    memcpy(midBuf, "member_login", 13);    // 汇编 memcpy(local_2c,"member_login",0xd)
    // [b28d-53 P134] ORIG 无 cntStr 具名局部：NumberToString 结果 %eax 直落
    // set_query 的 0xc(%esp)，且先物化 giftCnt 入 %edx 跨 call 保活。
    handle->set_query(
        "upDate member_login set last_gift_time=unix_timestamp(now()),gift_cnt=gift_cnt+%d where m_id=%s",
        giftCnt, NumberToString(mId, 0));
    handle->exec(true);
    return true;
}

// ============================================================================
// DB_UpdateBreakAwayAccruedCera::makeRequest ORIG @ 0x0843856a
//   Acquire("DBThread.cpp", 0x85fe) | opcode 0x146 | -1 | param_1 | param_2
// ============================================================================
void DB_UpdateBreakAwayAccruedCera::makeRequest(unsigned int param_1,
                                                unsigned int param_2)
{
    // b28c-33：ORIG 无 stream 具名局部（Acquire 返回值 eax 直传 guard ctor 的
    // 0x4(%esp)），值缓存局部多 1 store+1 load 并使 guard 槽 -0x10→-0x14 ⇒ 内联实参。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x85fe), true);
    guard.operator*()->operator<<(0x146);             // SIG_UPDATE_BREAKAWAY_ACCRUED_CERA
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
