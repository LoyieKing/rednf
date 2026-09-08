// df_game_r 还原 —— DB_UpdateChuseokPackagePurchase（2010 秋夕包购买计数 DB
// 请求，DBThread 派发）。逐函数对照
// docs/class_func_reports/DB_UpdateChuseokPackagePurchase/{makeRequest,dispatch}.md。
// 语义：makeRequest（0x0843c8b6）向 DB 队列发一条请求，表头 0x213 +
// characNo(-1) + m_id；dispatch（0x0843c7bc）读流取 m_id 后对硬编码表
// event_1009_package_count 执行
// "upDate event_1009_package_count set purchase_cnt = purchase_cnt+1 where
// m_id=%s"（无行则 insert）。与 DB_UpdatePurchaseCashItemBonus 不同，
// ORIG 表名不走 get_db_table_name 且无 select 回发。
#include "DB_UpdateChuseokPackagePurchase.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "SkillSlot.h"          // NumberToString
#include "Stream.h"

// ============================================================================
// DB_UpdateChuseokPackagePurchase::makeRequest @ 0x0843c8b6
// mangled _ZN31DB_UpdateChuseokPackagePurchase11makeRequestEj
// ============================================================================
void DB_UpdateChuseokPackagePurchase::makeRequest(unsigned int mId)
{
    // b28c-33：ORIG 0x0843c8b6 无 stream 具名局部（Acquire 返回值 eax 直传 guard
    // ctor 0x4(%esp)，guard 槽 -0x10）；值缓存局部多 store/load 且槽平移 -0x14。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x90bd), true);
    guard.operator*()->operator<<(0x213);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(mId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateChuseokPackagePurchase::dispatch @ 0x0843c7bc
// mangled _ZN31DB_UpdateChuseokPackagePurchase8dispatchEiiP6Stream
// ============================================================================
bool DB_UpdateChuseokPackagePurchase::dispatch(int /*userIdx*/, int /*slot*/,
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int mId = 0;
    *s >> mId;

    // ORIG：NumberToString(local_14, 0) 后直接拼入硬编码表名 SQL。
    db->set_query(
        "upDate event_1009_package_count set purchase_cnt = purchase_cnt+1 "
        "where m_id=%s",
        NumberToString(mId, 0));
    if (!db->exec(true))
        return false;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1009_package_count(m_id, purchase_cnt) "
            "values(%s, %u)",
            NumberToString(mId, 0), 1);
        if (!db->exec(true))
            return false;
    }
    return true;
}
