//   docs/class_func_reports/DB_LoadCerashopAddRestrictType/{makeRequest,dispatch}.md
//   docs/class_func_reports/DB_SaveCerashopAddRestrictType/{makeRequest,dispatch}.md
// 语义：makeRequest 向 DB 队列发商城限购请求（包体 CStreamGuard）；
// dispatch 在 DBThread 侧读流执行 SQL（LOAD 组包回投 DISPATCH_Q）。
// 两类继承 DBDispatcher（ORIG vtable 0x8b2db30/0x8b2db48，虚槽 2 = dispatch）。
// ============================================================================
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "CUser.h"
#include "DBDispatcher.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "Stream.h"
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP

// ORIG T @ 0x080e1ff6（mangled _ZN30DB_LoadCerashopAddRestrictType11makeRequestEP5CUser）
// dispatch @ 0x080e1aca（_ZN30DB_LoadCerashopAddRestrictType8dispatchEiiP6Stream）
class DB_LoadCerashopAddRestrictType : public DBDispatcher
{
public:
    DB_LoadCerashopAddRestrictType() {}   // ORIG C1 0x844ff82（sz 0x4）
    virtual bool dispatch(int a, int b, Stream* stream);   // ORIG T 0x080e1aca
    static void makeRequest(CUser* user);
};

// ORIG T @ 0x080e17ba
//（mangled _ZN30DB_SaveCerashopAddRestrictType11makeRequestEP5CUserRN19CerashopAddRestrict11DBInputDataE）
// dispatch @ 0x080e14f6（_ZN30DB_SaveCerashopAddRestrictType8dispatchEiiP6Stream）
class DB_SaveCerashopAddRestrictType : public DBDispatcher
{
public:
    DB_SaveCerashopAddRestrictType() {}   // ORIG C1 0x844ff66（sz 0x4）
    virtual bool dispatch(int a, int b, Stream* stream);   // ORIG T 0x080e14f6
    static void makeRequest(CUser* user, CerashopAddRestrict::DBInputData& data);
};
// ============================================================================
// DB_LoadCerashopAddRestrictType::makeRequest @ 0x080e1ff6
// 包体：0x341 | GetUID() | get_acc_id() | getCurCharacNo()
// ============================================================================
void DB_LoadCerashopAddRestrictType::makeRequest(CUser* user)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("CerashopAddRestrictType.cpp", 500);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x341);                      // SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
    guard.operator*()->operator<<((int)user->GetUID());                     // operator<<(int)
    guard.operator*()->operator<<(user->get_acc_id());                      // operator<<(uint)
    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());    // operator<<(uint)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SaveCerashopAddRestrictType::makeRequest @ 0x080e17ba
// 按 data.m_pInfo->m_type（op）分派：
//   op==0（登记）：包体 0x340 | -1 | m_type | m_ipgNo | head[+4] | head[+8](long)
//                  | m_pInfo[+0x10](long) | get_acc_id()
//   op==1（删除）：同上，末字段为 getCurCharacNo()
// 注：ORIG DBInputData 的第 0 个指针字段（head 记录，slot +0x00）在源 CUser.h 的
// DBInputData 中未建模（仅 m_pInfo @ +0x04 与 m_pad[0xc]），本 TU 按 ORIG 槽位
// 直接解释，保持 ABI 与 ORIG 字节布局一致。
// ============================================================================
void DB_SaveCerashopAddRestrictType::makeRequest(
    CUser* user, CerashopAddRestrict::DBInputData& data)
{
    CerashopAddRestrict::InfoDaily* info = data.m_pInfo;
    CerashopAddRestrict::paramDaily* head =
        *reinterpret_cast<CerashopAddRestrict::paramDaily**>(&data);
    if (info->m_type == 0)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x18c);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<(info->m_type);
        guard.operator*()->operator<<(info->m_ipgNo);
        guard.operator*()->operator<<(head->m_count);
        guard.operator*()->operator<<((long)head->m_nextDate);
        guard.operator*()->operator<<((long)info->m_end);
        guard.operator*()->operator<<(user->get_acc_id());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else if (info->m_type == 1)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x19a);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<(info->m_type);
        guard.operator*()->operator<<(info->m_ipgNo);
        guard.operator*()->operator<<(head->m_count);
        guard.operator*()->operator<<((long)head->m_nextDate);
        guard.operator*()->operator<<((long)info->m_end);
        guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ============================================================================
// DB_SaveCerashopAddRestrictType::dispatch @ 0x080e14f6（报告 dispatch.md 全文转写）
// 读流：type | id | ipgNo | nextDate(long) | endDate(long)；
// type==0（账号）：再读 m_id；UPDATE，affected==0 时 INSERT。
// type==1（角色）：再读 characNo；同构。失败返回 false，成功 true。
// ============================================================================
bool DB_SaveCerashopAddRestrictType::dispatch(int /*param1*/, int /*param2*/,
                                              Stream* stream)
{
    unsigned int type = 0;    // -0x14
    unsigned int id = 0;      // -0x18
    unsigned int ipgNo = 0;   // -0x1c
    long nextDate = 0;        // -0x20
    long endDate = 0;         // -0x24
    *stream >> type;
    *stream >> id;
    *stream >> ipgNo;
    *stream >> nextDate;
    *stream >> endDate;

    if (type == 0) {
        unsigned int mId = 0;   // -0x28
        *stream >> mId;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            return false;
        db->set_query(
            "upDate account_cerashop_restrict set count=%u, next_date=%d, "
            "last_access_date=UNIX_TIMESTAMP() where m_id = %u and ipg_no=%u",
            ipgNo, nextDate, mId, id);
        if (!db->exec(true))
            return false;
        if (db->getAffectedRowCount() == 0) {
            db->set_query(
                "inSert into account_cerashop_restrict(m_id, ipg_no, count, "
                "next_date, end_date, last_access_date) values(%u, %u, %u, %d, "
                "%d, UNIX_TIMESTAMP())",
                mId, id, ipgNo, nextDate, endDate);
            if (!db->exec(true))
                return false;
        }
    } else if (type == 1) {
        unsigned int characNo = 0;   // -0x2c
        *stream >> characNo;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            return false;
        db->set_query(
            "upDate charac_cerashop_restrict set count=%u, next_date=%d, "
            "last_access_date=UNIX_TIMESTAMP() where charac_no = %u and ipg_no=%u",
            ipgNo, nextDate, characNo, id);
        if (!db->exec(true))
            return false;
        if (db->getAffectedRowCount() == 0) {
            db->set_query(
                "inSert into charac_cerashop_restrict(charac_no, ipg_no, count, "
                "next_date, end_date, last_access_date) values(%u, %u, %u, %d, "
                "%d, UNIX_TIMESTAMP())",
                characNo, id, ipgNo, nextDate, endDate);
            if (!db->exec(true))
                return false;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadCerashopAddRestrictType::dispatch @ 0x080e1aca（报告 dispatch.md 全文转写）
// 读流：m_id | characNo；账号表（handle 1）查询 limit 10，n_rows>0 时组包
// 0x341 | param2 | SIG（count + type=0 + 每行 3 uint 自 +8 步长 0xc）投 DISPATCH_Q；
// 角色表（handle 2）同构（type=1，行号 0x1da）。失败 false，成功 true。
// ============================================================================
bool DB_LoadCerashopAddRestrictType::dispatch(int /*param1*/, int param2,
                                              Stream* stream)
{
    unsigned int mId = 0;      // -0x30
    unsigned int characNo = 0; // -0x34
    *stream >> mId;
    *stream >> characNo;

    // ---- 账号表 ----
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
    if (db == 0)
        return false;
    db->set_query(
        "seLect ipg_no, count, next_date from account_cerashop_restrict where "
        "m_id = %u and end_date > UNIX_TIMESTAMP() limit 10", mId);
    if (!db->exec(true))
        return false;
    int nRows = db->get_n_rows();   // -0x28
    if (nRows > 0) {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x1bc);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x341);   // SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
        guard.operator*()->operator<<(param2);
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig =
            guard.GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        *(int*)((char*)sig + 4) = 0;      // +0x04 type = account
        for (int i = 0; i < nRows; ++i) {
            if (!db->fetch())
                return false;
            int col = 1;   // -0x18 列号推进（0 基）
            if (!db->get_int(0, *(unsigned int*)((char*)sig + 8 + i * 0xc)))
                return false;
            col = 2;
            if (!db->get_int(1, *(unsigned int*)((char*)sig + 0xc + i * 0xc)))
                return false;
            col = 3;
            if (!db->get_int(2, *(unsigned int*)((char*)sig + 0x10 + i * 0xc)))
                return false;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }

    // ---- 角色表 ----
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    if (db2 == 0)
        return false;
    db2->set_query(
        "seLect ipg_no, count, next_date from charac_cerashop_restrict where "
        "charac_no = %u and end_date > UNIX_TIMESTAMP() limit 10", characNo);
    if (!db2->exec(true))
        return false;
    nRows = db2->get_n_rows();
    if (nRows > 0) {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x1da);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x341);
        guard.operator*()->operator<<(param2);
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig =
            guard.GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        *(int*)((char*)sig + 4) = 1;      // +0x04 type = charac
        for (int i = 0; i < nRows; ++i) {
            if (!db2->fetch())
                return false;
            int col = 1;
            if (!db2->get_int(0, *(unsigned int*)((char*)sig + 8 + i * 0xc)))
                return false;
            col = 2;
            if (!db2->get_int(1, *(unsigned int*)((char*)sig + 0xc + i * 0xc)))
                return false;
            col = 3;
            if (!db2->get_int(2, *(unsigned int*)((char*)sig + 0x10 + i * 0xc)))
                return false;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return true;
}