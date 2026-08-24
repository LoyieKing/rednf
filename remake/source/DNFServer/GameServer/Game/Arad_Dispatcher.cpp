// ============================================================================
// df_game_r 还原 —— ARAD::Arad_Dispatcher / Arad_Dispatcher_Manager /
// Arad_Dispatcher_BuyItemCheck_Impl + ARAD::DATABASE::ITEM_QUERY 查询。
// 依据 docs/class_func_reports/ARAD__Arad_Dispatcher{,_Manager}/、
// ARAD__Arad_Dispatcher_BuyItemCheck_Impl/、ARAD__DATABASE__ITEM_QUERY/ 报告逐函数转写。
// C++98 / -m32 / O0 基线。
// ============================================================================

#include "Arad_Dispatcher.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr / s_pCeraShop / s_pItemVendingMachine
#include "Stream.h"            // Stream operator>> / GetOutBuffer<T>
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBMgr.h"             // DBMgr 完整类型（GlobalData::s_db_mgr->GetDBHandle）
#include <stddef.h>
#include "MySQL.h"
#include "LogManager.h"
#include "SigTypes.h"          // MSG_BUY_CERASHOP_ITEM（0x3f84）
#include "MoreTypes.h"         // ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK
#include "CCeraShop.h"         // WongWork::CCeraShop::ProcessError
#include "ItemVendingMachine.h"
#include "CUser.h"

// 布局断言：ORIG Manager ctor 0x0818fdd4 两组各 0x354 指针数组，size = 0x1AA0。
typedef char AradDispatcherManagerSizeAssert[
    sizeof(ARAD::Arad_Dispatcher_Manager) == 0x354 * 4 * 2 ? 1 : -1];

namespace ARAD
{

// ============================================================================
// Arad_Dispatcher —— ORIG C1 @0x0818edd0：vptr 由编译器写；m_code = code（+0x04）。
// ============================================================================
Arad_Dispatcher::Arad_Dispatcher(int code)
    : m_code(code)
{
}

// ORIG D1 @0x0818e900：仅写基类 vptr；D0 条件 operator delete(this)。
Arad_Dispatcher::~Arad_Dispatcher()
{
}

int Arad_Dispatcher::db_dispatch(int /*code*/, int /*uid*/, Stream* /*stream*/)
{
    // ORIG 0x0818e84c：mov $0x0,%eax; ret
    return 0;
}

int Arad_Dispatcher::inter_dispatch(CUser* /*user*/, char* /*body*/, int /*len*/)
{
    // ORIG 0x0818e856：mov $0x0,%eax; ret
    return 0;
}

void Arad_Dispatcher::register_dispatcher(Arad_Dispatcher* d)
{
    // ORIG 0x0818e860：GlobalInstance<Manager>::inst()->register_dispatcher(d)
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(d);
}

void Arad_Dispatcher::unregister_dispatcher(int code)
{
    // ORIG 0x0818e87c：GlobalInstance<Manager>::inst()->unregister_dispatcher(code)
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->unregister_dispatcher(code);
}

// ============================================================================
// Arad_Dispatcher_Manager —— ORIG C1 @0x0818fdd4：两个 0x354 项指针数组清零。
// ============================================================================
Arad_Dispatcher_Manager::Arad_Dispatcher_Manager()
{
    for (int i = 0; i < 0x354; ++i)
        m_interDispatchers[i] = 0;
    for (int i = 0; i < 0x354; ++i)
        m_dbDispatchers[i] = 0;
}

Arad_Dispatcher_Manager::~Arad_Dispatcher_Manager()
{
    // ORIG D1 @0x0818fe32：空。
}

bool Arad_Dispatcher_Manager::open()
{
    // ORIG 0x0818fe38：operator new(8) → BuyItemCheck_Impl ctor → 注册 → return 0。
    Arad_Dispatcher_BuyItemCheck_Impl* impl =
        new Arad_Dispatcher_BuyItemCheck_Impl();
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(impl);
    return false;
}

bool Arad_Dispatcher_Manager::register_dispatcher(Arad_Dispatcher* d)
{
    // ORIG 0x0818fe98：code=dispathc_code()；inter[code]=d；db[code]=d；返回 0。
    const int code = d->dispathc_code();
    m_interDispatchers[code] = d;
    m_dbDispatchers[code] = d;
    return false;
}

bool Arad_Dispatcher_Manager::unregister_dispatcher(int code)
{
    // ORIG 0x0818fed6：inter[code] 判空→虚析构调用块（Ghidra unreachable）→置空返回 0；
    // 否则 db[code] 同样处理；两处均非空返回 1。按报告反编译 C 转写：
    if (m_interDispatchers[code] == 0)
    {
        m_interDispatchers[code] = 0;
        return false;
    }
    if (m_dbDispatchers[code] == 0)
    {
        m_dbDispatchers[code] = 0;
        return false;
    }
    return true;
}

bool Arad_Dispatcher_Manager::inter_dispatch(int code, CUser* user,
                                             char* body, int len)
{
    // ORIG 0x0818ff6c：查 inter 组，虚槽3 调用；err!=0 时记日志并对 user 断连。
    Arad_Dispatcher* d = m_interDispatchers[code];
    if (d == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            "bool ARAD::Arad_Dispatcher_Manager::inter_dispatch("
            "ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)",
            0x7d, "Not found Inter_Dispatch_Code[%d]", code);
        return false;
    }
    const int err = d->inter_dispatch(user, body, len);
    if (err == 0)
        return true;

    unsigned int accId = 0;
    if (user != 0)
    {
        accId = user->get_acc_id();
        user->DisConnSig((DISCONN_SIG)2, 1, err + 1000000);
    }
    LogManager::logFormat(
        1, "localjapan/Arad_Dispatch_Manager.cpp",
        "bool ARAD::Arad_Dispatcher_Manager::inter_dispatch("
        "ARAD::Arad_Dispatcher_Manager::INTERNAL_PACKET_CODE, CUser*, char*, int)",
        0x8d, "Inter Dispatch Result Error Inter_Dispatch_Code[%d] user_acc_id[%d]",
        code, accId);
    return false;
}

bool Arad_Dispatcher_Manager::db_dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08190080：查 db 组（+0x354），虚槽2 调用；ErrLine!=0 记日志；找到即返回 1。
    Arad_Dispatcher* d = m_dbDispatchers[code];
    if (d == 0)
        return false;
    const int errLine = d->db_dispatch(code, uid, stream);
    if (errLine != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            "bool ARAD::Arad_Dispatcher_Manager::db_dispatch(int, int, Stream*)",
            0xab, "[ERROR] DB Dispatch Result dipCode[%d] uid[%d], ErrLine(%d)",
            code, uid, errLine);
    }
    return true;
}

// ============================================================================
// Arad_Dispatcher_BuyItemCheck_Impl —— ORIG C1 @0x0818e9cc：基类码 0x1fa。
// ============================================================================
Arad_Dispatcher_BuyItemCheck_Impl::Arad_Dispatcher_BuyItemCheck_Impl()
    : Arad_Dispatcher(0x1fa)
{
}

Arad_Dispatcher_BuyItemCheck_Impl::~Arad_Dispatcher_BuyItemCheck_Impl()
{
}

int Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch(CUser* user,
                                                      char* body, int /*len*/)
{
    // ORIG 0x0818ea3e：
    //   body[0]==1 → ItemVendingMachine::BuyItem(user, *(MSG*)(body+1))；
    //               成功返回 0，失败 logFormat(…0x40…) 返回 -1。
    //   body[0]!=1 → CCeraShop::ProcessError(s_pCeraShop,user,0x74,0,0,1,1,0)，返回 0。
    if (body[0] != '\x01')
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x74, 0, 0, 1, 1, '\0');
        return 0;
    }
    if (!GlobalData::s_pItemVendingMachine->BuyItem(
            user, *(const MSG_BUY_CERASHOP_ITEM*)(body + 1)))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch("
            "CUser*, char*, int)", 0x40,
            "[ERROR] BuyItem() Result Error ItemIdx(%d)",
            *(unsigned int*)(body + 0x3a));
        return -1;
    }
    return 0;
}

int Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch(int code, int uid,
                                                   Stream* stream)
{
    // ORIG 0x0818eb0c：读 BuyItemInfo ← stream；GetOutBuffer<MSG_BUY_CERASHOP_ITEM>；
    // arad_sp_buy_item_check 通过后组装 PURCHASE_COUNT_CHECK 内部包入 DB 队列。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)7,
                                                  (ENUM_SERVER_GROUP)0);
    ARAD_DATAS::BuyItemInfo info;
    memset(&info, 0, sizeof(info));
    *stream >> info.mid;
    *stream >> info.characNo;
    *stream >> info.ipgNo;
    *stream >> info.checkType;
    *stream >> info.limitCount;
    MSG_BUY_CERASHOP_ITEM* msg = stream->GetOutBuffer<MSG_BUY_CERASHOP_ITEM>();
    if (msg == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch("
            "int, int, Stream*)", 0x66,
            "[ERROR] MSG_BUY_CERASHOP_ITEM GetOutBuffer => MID[%d], CHARIDX[%d], "
            "ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    if (DATABASE::ITEM_QUERY::arad_sp_buy_item_check(info) != 0)
        return -1;

    Stream* out = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp", 0x71);
    CStreamGuard guard(out, true);
    guard.operator*()->operator<<(code);
    guard.operator*()->operator<<(uid);
    typedef ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK PurchaseCountCheck;
    PurchaseCountCheck* chk =
        guard.operator->()->GetInBuffer<PurchaseCountCheck>();
    if (chk == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch("
            "int, int, Stream*)", 0x7b,
            "ITEM_PURCHASE_COUNT_CHECK MID[%d], CHARIDX[%d], ITEMIDX[%d], "
            "CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
    }
    else
    {
        *(char*)chk = info.buyable;
        memcpy((char*)chk + 1, msg, 0x3f84);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 0;
}

// ============================================================================
// ARAD::GetQuarterOfYear —— ORIG T 0x0819e8d0。
// time/localtime 后 quarter=tm_mon/3+1（0x55555556 除魔数）、year=(tm_year+1900)%10000
// （0x10624dd3/sar7 除魔数 + imul 0x7d0 取余），sprintf("%02d%d", year, quarter)。
// ============================================================================
void GetQuarterOfYear(char* buf)
{
    time_t t = time(0);
    struct tm* tmp = localtime(&t);
    const int quarter = tmp->tm_mon / 3 + 1;
    const int yearFull = tmp->tm_year + 1900;
    const int year = yearFull - (yearFull / 10000) * 10000;
    sprintf(buf, "%02d%d", year, quarter);
}

int DATABASE::ITEM_QUERY::arad_sp_buy_item_check(ARAD_DATAS::BuyItemInfo& info)
{
    // ORIG T 0x0818cfab：prod_sale_entry_<quarter> count(*) 与 limitCount 比较。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)7,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x454, "%s", "Database null!!");
        return -1;
    }
    char quarter[5];
    quarter[0] = quarter[1] = quarter[2] = quarter[3] = quarter[4] = 0;
    GetQuarterOfYear(quarter);
    if (info.checkType == 1)
    {
        db->set_query(
            "select count(*) from prod_sale_entry_%s where  ipg_no = %d and "
            " m_id = %d   and  charac_no = %d",
            quarter, info.ipgNo, info.mid, info.characNo);
    }
    else
    {
        db->set_query(
            "select count(*) from prod_sale_entry_%s where  ipg_no = %d and "
            " m_id = %d",
            quarter, info.mid, info.ipgNo);
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x472,
            "[ERROR] DB Exec Error => MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    if (!db->fetch())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x47b,
            "[ERROR] DB Fetch Error => MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    int count = 0;
    if (!db->get_int(0, count))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            "int ARAD::DATABASE::ITEM_QUERY::arad_sp_buy_item_check("
            "ARAD_DATAS::BuyItemInfo&)", 0x485,
            "RESULT_COUNT MID[%d], CHARIDX[%d], ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        return -1;
    }
    info.buyable = 1;
    if (info.limitCount <= count)
        info.buyable = 0;
    return 0;
}

}  // namespace ARAD
