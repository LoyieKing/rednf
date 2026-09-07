// df_game_r 还原 —— DB_InsertAccountFirstLoginFlag（账号首次登录标记 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_InsertAccountFirstLoginFlag/。
// 语义：makeRequest（0x0843e79a）向 DB 队列发一条请求，表头 0x25e + 5 参数；
// dispatch（0x0843e636）读流并据此向事件表执行 "insert into ...(m_id,server_id,
// charac_no, occ_date) values(...)"。
#include "DB_InsertAccountFirstLoginFlag.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "SyncScriptDeps.h"         // toTString(const std::string&)
#include "CInventory.h"
#include "Stream.h"

#include <cstdio>

// ============================================================================
// DB_InsertAccountFirstLoginFlag::makeRequest @ 0x0843e79a
// mangled _ZN30DB_InsertAccountFirstLoginFlag11makeRequestEijiji
// ============================================================================
void DB_InsertAccountFirstLoginFlag::makeRequest(int p1, unsigned int p2, int p3,
                                                 unsigned int p4, int p5)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9503), true);
    guard.operator*()->operator<<(0x25e);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    guard.operator*()->operator<<(p3);
    guard.operator*()->operator<<(p4);
    guard.operator*()->operator<<(p5);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_InsertAccountFirstLoginFlag::dispatch @ 0x0843e636
// mangled _ZN30DB_InsertAccountFirstLoginFlag8dispatchEiiP6Stream
// ============================================================================
bool DB_InsertAccountFirstLoginFlag::dispatch(int /*userIdx*/, int /*slot*/,
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int local_2c = 0;   // m_id（NumberToString）
    int          local_34 = 0;   // server_id
    unsigned int local_30 = 0;   // charac_no
    int          local_38 = 0;   // 事件记录类型（0x191/0x192）
    *s >> local_2c;
    *s >> local_34;
    *s >> local_30;
    *s >> local_38;

    CDataManager* dm = G_CDataManager();
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(dm->get_event_script_mng());
    if (local_38 != 0x191 && local_38 != 0x192)
        return 0;

    EventClassify::CEventEntity* ent = sm->get_event_entity(local_38);
    if (ent == 0)
        return 0;

    char* numStr = NumberToString(local_2c, 0);
    // ORIG 0x0843e744：toTString(const std::string&) 转换表名（get_db_table_name
    // 返回 &m_dbTableName，调用方按 std::string& 解释）。
    std::string& tableName = ent->get_db_table_name();
    const char* tableNameStr = toTString(tableName);
    db->set_query("inSert into %s(m_id,server_id, charac_no, occ_date) "
                  "values(%s,%d, %u, now())",
                  tableNameStr, numStr, local_34, local_30);
    return db->exec(true);
}
