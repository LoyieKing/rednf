// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_CerashopAddRestrict/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "DB_CerashopAddRestrict/makeRequest.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CerashopAddRestrictType.cpp", 500), true);
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
// 注：ORIG DBInputData 的第 0 个字段（slot +0x00，DBInputData::m_result）实际存
// head 记录（paramDaily*）指针。m_result 建模为 int，本 TU 将其 reinterpret_cast 为
// paramDaily* 解释 m_count(+4)/m_nextDate(+8)，保持 ABI 与 ORIG 字节布局一致。
// ============================================================================
#include "DB_CerashopAddRestrict/makeRequest_1.inc"
    CUser* user, CerashopAddRestrict::DBInputData& data)
{
    // [b28d-46 判词·N/N18a] 沿用 b28c-33：213 vs 215，p167 CLEAR；两处
    // `mov 0x4(%eax),%ebx` 单段直落 vs 4.4 `mov 0x4(%eax),%eax; mov %eax,%ebx`
    // 两段（N18a；字段/callee 返回类型两旋钮已试）⇒ 封顶。ORIG 0x080e17ba。
    // [b28c-33 判词 NEAR-N18a] 两处 `operator<<(reinterpret_cast<paramDaily*>(
    // data.m_result)->m_count/m_nextDate)`：ORIG 为 `mov 0x4(%eax),%ebx` 单段直落
    // callee-saved；4.4 恒 `mov 0x4(%eax),%eax; mov %eax,%ebx`（+1×2）。其余全等
    // （213 vs 215，实质差异仅此两处，余为重定位/注释）。
    switch (data.m_pInfo->m_type)
    {
    case 0:
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x18c), true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_type);
        guard.operator*()->operator<<(data.m_pInfo->m_ipgNo);
        guard.operator*()->operator<<((unsigned int)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_count);
        guard.operator*()->operator<<((long)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_nextDate);
        guard.operator*()->operator<<((long)data.m_pInfo->m_end);
        guard.operator*()->operator<<(user->get_acc_id());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        break;
    }
    case 1:
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x19a), true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_type);
        guard.operator*()->operator<<(data.m_pInfo->m_ipgNo);
        guard.operator*()->operator<<((unsigned int)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_count);
        guard.operator*()->operator<<((long)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_nextDate);
        guard.operator*()->operator<<((long)data.m_pInfo->m_end);
        guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        break;
    }
    default:
        break;
    }
}

// ============================================================================
// DB_SaveCerashopAddRestrictType::dispatch @ 0x080e14f6（报告 dispatch.md 全文转写）
// 读流：type | id | ipgNo | nextDate(long) | endDate(long)；
// type==0（账号）：再读 m_id；UPDATE，affected==0 时 INSERT。
// type==1（角色）：再读 characNo；同构。失败返回 false，成功 true。
// ============================================================================
#include "DB_CerashopAddRestrict/dispatch.inc"
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

    switch (type) {
    case 0: {
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
        break;
    }
    case 1: {
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
        break;
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
#include "DB_CerashopAddRestrict/dispatch_1.inc"
                                              Stream* stream)
{
    // [b28d-46 判词·N/N82] ORIG 0x080e1aca：354 vs 358；帧 0x50 vs 0x60 与
    // 槽号整体平移（N82/P112）⇒ 封顶。
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
            guard.operator->()->GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        // [P6S6 2026-09-11 复核·仍 N] 试 `((int*)sig)[1] = 0;`（期望 disp 折叠
        //   `movl $0x0,0x4(%eax)`）实测我方仍 `add $0x4,%eax; movl $0x0,(%eax)`，
        //   读数恒 358/354 ⇒ 4.4 对 int* 索引 cast 不折叠（N82 族）⇒ 回滚原形。
        *(int*)((char*)sig + 4) = 0;      // +0x04 type = account
        for (int i = 0; i < nRows; ++i) {
            if (!db->fetch())
                return false;
            // [P6S6 2026-09-11 负结论·回滚] ORIG 0x080e1c48 段为 `col++` 形
            //   （`mov -0x18,%eax; addl $0x1,-0x18` ×3 直证），但本波实测
            //   `int col = 0; get_int(col++, …)` 反而劣化：ours 358→**366**、
            //   difflib 块 22→25（4.4 对后置自增实参多 8 条）⇒ 回滚维持常量形
            //   （语义等价：两侧都传 0/1/2 列号）。
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
            guard.operator->()->GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
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