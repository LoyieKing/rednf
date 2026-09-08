// df_game_r 还原 —— DB_UpdatePurchaseCashItemBonus（商城消费奖励购买计数 DB
// 请求，DBThread 派发）。逐函数对照
// docs/class_func_reports/DB_UpdatePurchaseCashItemBonus/{makeRequest,dispatch}.md。
// 语义：makeRequest（0x0843e560）向 DB 队列发一条请求，表头 0x23c + m_id + acc_id；
// dispatch（0x0843e252）读流取 m_id 后对 0x19a 事件表执行
// "upDate %s set purchase_cnt = purchase_cnt + 1 where m_id=%s"（无行则 insert），
// 再 select 回读 purchase_cnt 经 DISPATCH_Q 回发游戏侧。
#include "DB_UpdatePurchaseCashItemBonus.h"

#include <string>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "SigTypes.h"
#include "SkillSlot.h"          // NumberToString
#include "SyncScriptDeps.h"     // toTString
#include "Stream.h"

// ============================================================================
// DB_UpdatePurchaseCashItemBonus::makeRequest @ 0x0843e560
// mangled _ZN30DB_UpdatePurchaseCashItemBonus11makeRequestEij
// ============================================================================
void DB_UpdatePurchaseCashItemBonus::makeRequest(int p1, unsigned int p2)
{
    // b28c-33：同 DB_UpdateBreakAwayAccruedCera::makeRequest，Acquire 返回值直传
    // guard ctor（ORIG 无 stream 局部、guard 槽 -0x10）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9415), true);
    guard.operator*()->operator<<(0x23c);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdatePurchaseCashItemBonus::dispatch @ 0x0843e252
// mangled _ZN30DB_UpdatePurchaseCashItemBonus8dispatchEiiP6Stream
// ============================================================================
bool DB_UpdatePurchaseCashItemBonus::dispatch(int /*userIdx*/, int slot,
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int mId = 0;
    *s >> mId;

    CDataManager* dm = G_CDataManager();
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(dm->get_event_script_mng());
    // ORIG：std::string local_1c(get_db_table_name(0x19a))。
    std::string tableStr(sm->get_db_table_name(0x19a));
    const char* tableName = toTString(tableStr);

    db->set_query("upDate %s set purchase_cnt = purchase_cnt + 1 where m_id=%s",
                  tableName, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into %s(m_id, purchase_cnt) values(%s, %u)",
                      tableName, NumberToString(mId, 0), 1);
        if (!db->exec(true))
            return 0;
    }
    db->set_query("seLect purchase_cnt from %s where m_id=%s",
                  tableName, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
        return 0;
    if (!db->fetch())
        return 0;
    unsigned int purchaseCnt = 0;
    if (!db->get_uint(0, purchaseCnt))
        return 0;

    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9407);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x23c);
    guard.operator*()->operator<<(slot);   // ORIG 0x843e4c0：第二个 lsEi 参数 = 0x10(%ebp) = slot
    SIG_PURCHASE_CASHITEM_BONUS* sig = guard.GetInBuffer<SIG_PURCHASE_CASHITEM_BONUS>();
    *reinterpret_cast<unsigned int*>(sig) = purchaseCnt;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}
