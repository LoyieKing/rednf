// df_game_r 还原 —— DB_InsertAccountFirstLoginDailyFlag（账号每日首次登录标记 DB 请求）。
// 逐函数对照 docs/class_func_reports/DB_InsertAccountFirstLoginDailyFlag/。
// 语义：makeRequest（0x0843ea64）发表头 0x342 + 4 参数；dispatch（0x0843e8be）
// 先 UPDATE（affected==0 则 INSERT）。
#include "DB_InsertAccountFirstLoginDailyFlag.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "CInventory.h"
#include "Stream.h"

// ============================================================================
// DB_InsertAccountFirstLoginDailyFlag::makeRequest @ 0x0843ea64
// mangled _ZN35DB_InsertAccountFirstLoginDailyFlag11makeRequestEijji
// ============================================================================
void DB_InsertAccountFirstLoginDailyFlag::makeRequest(int p1, unsigned int p2,
                                                      unsigned int p3, int p4)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x953d);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x342);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    guard.operator*()->operator<<(p3);
    guard.operator*()->operator<<(p4);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_InsertAccountFirstLoginDailyFlag::dispatch @ 0x0843e8be
// mangled _ZN35DB_InsertAccountFirstLoginDailyFlag8dispatchEiiP6Stream
// ============================================================================
int DB_InsertAccountFirstLoginDailyFlag::dispatch(int /*param1*/, int /*serverGroup*/,
                                                  Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int local_20 = 0;   // m_id
    unsigned int local_24 = 0;   // charac_no
    int          local_28 = 0;   // 事件记录类型（0x19c）
    *s >> local_20;
    *s >> local_24;
    *s >> local_28;

    CDataManager* dm = G_CDataManager();
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(dm->get_event_script_mng());
    if (local_28 != 0x19c)
        return 0;

    EventClassify::CEventEntity* ent = sm->get_event_entity(local_28);
    if (ent == 0)
        return 0;

    char* tableName = ent->get_db_table_name();
    db->set_query("upDate %s set charac_no=%u, occ_date=now() where m_id=%u",
                  tableName, local_24, local_20);
    if (!db->exec(true))
        return 0;

    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into %s(m_id, charac_no, occ_date) "
                      "values(%u, %u, now())",
                      tableName, local_20, local_24);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}
