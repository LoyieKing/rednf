// ============================================================================
// df_game_r 还原 —— DB_UpdateCeraShopPurchaseCount（塞拉商城购买计数 DB 请求）。
// 归属批次：OrigCeraCount（仅 ORIG 4 符号：dispatch/_select/_update/_insert；
// makeRequest 为 LinkE2.JpnExImpl 批次既有实现）。
// ORIG 符号（df_game_r 80）：
//   T 0x0844b0cc _ZN30DB_UpdateCeraShopPurchaseCount8dispatchEiiP6Stream
//   T 0x0844b450 _ZN30DB_UpdateCeraShopPurchaseCount20_selectPurchaseCountEjRt
//   T 0x0844b680 _ZN30DB_UpdateCeraShopPurchaseCount20_updatePurchaseCountEjt
//   T 0x0844b808 _ZN30DB_UpdateCeraShopPurchaseCount20_insertPurchaseCountEj
//   T 0x0844b31c _ZN30DB_UpdateCeraShopPurchaseCount11makeRequestEjmii（既有）
// 语义：dispatch 读流 SIG（m_id + purchase_cnt）→ select 计数 → 无则 insert(1) /
// 有则 update(+1) → 组 0x2d9 回发 DISPATCH_Q。DB 异常统一 throw const char*。
// vtable @0x8c5e020 = [offtop][typeinfo][dispatch][~D1][~D0]。
// ============================================================================

#include "GlobalData.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "SigTypes.h"  // SIG_CERASHOP_PURCHASE_COUNT（0x10 POD，SigTypes_A.h）
#include "GameTypes.h"  // cMyTrace
#include "DBMgr.h"
#include "MySQL.h"
#include "SkillSlot.h"  // NumberToString
#include "DBDispatcher.h"

#include <string.h>

class DB_UpdateCeraShopPurchaseCount : public DBDispatcher
{
public:
    DB_UpdateCeraShopPurchaseCount() {}  // ORIG W C1 0x844fbe6（0x4 基类 ctor 形态）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(unsigned int accId, unsigned long itemUid,
                            int count, int param4);

private:
    bool _selectPurchaseCount(unsigned int mId, unsigned short& out);
    void _updatePurchaseCount(unsigned int mId, unsigned short count);
    void _insertPurchaseCount(unsigned int mId);
};

// P1 组件视图：SIG_CERASHOP_PURCHASE_COUNT 的 purchase_cnt 字段（+0x8 ushort）
// 必须经此视图访问才能在 O0 折叠为单指令 disp 寻址（movzwl 0x8(%eax)）。
struct CeraShopCntView
{
    char m_pad[8];
    unsigned short m_cnt;
    int m_param;
};

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::dispatch @ 0x0844b0cc
// 逐指令转写：GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT> → null 则 trace(0xbbac)
//   "ptrData is null." 返 false；_selectPurchaseCount(m_id, &purchase_cnt) 失败
//   trace(0xbbb2) "select query fail." 返 false；purchase_cnt==0 →
//   cnt=1 + _insertPurchaseCount(m_id)，否则 cnt+1 + _updatePurchaseCount(m_id,cnt)；
//   共用出口：Acquire("DBThread.cpp",0xbbc1) → CStreamGuard(s,true) → *guard<<0x2d9
//   → *guard<<slot → guard->GetInBuffer<SIG>()（null 则 trace(0xbbc9) 返 false）
//   → 16B 逐字拷贝 → MsgQueueMgr::put(DISPATCH_Q=1, guard) → 返 true。
// ============================================================================
bool DB_UpdateCeraShopPurchaseCount::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CERASHOP_PURCHASE_COUNT* sig =
        stream->GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (sig == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbbac, 5);
        trace("[PURCAHSE BONUS] ptrData is null.");
        return false;
    }

    if (!_selectPurchaseCount(
            *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x0),
            reinterpret_cast<CeraShopCntView*>(sig)->m_cnt))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbbb2, 5);
        trace("[PURCAHSE BONUS] select query fail.");
        return false;
    }

    if (reinterpret_cast<CeraShopCntView*>(sig)->m_cnt == 0)
    {
        reinterpret_cast<CeraShopCntView*>(sig)->m_cnt = 1;
        _insertPurchaseCount(
            *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x0));
    }
    else
    {
        reinterpret_cast<CeraShopCntView*>(sig)->m_cnt++;
        _updatePurchaseCount(
            *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x0),
            reinterpret_cast<CeraShopCntView*>(sig)->m_cnt);
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbc1), true);
    guard.operator*()->operator<<((int)0x2d9);   // ORIG operator*() → operator<<(int)
    guard.operator*()->operator<<(slot);         // ORIG 0x10(%ebp) 第 3 形参
    SIG_CERASHOP_PURCHASE_COUNT* replySig =
        guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (replySig == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbbc9, 5);
        trace("[PURCAHSE BONUS] ptrData is null");
        return false;
    }

    *replySig = *sig;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_selectPurchaseCount @ 0x0844b450
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→
//   set_query(" seLect purchase_cnt from event_purchase_cnt where m_id = %s ",
//   NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)（失败 throw
//   "exec"）→ get_n_rows()==1 时 fetch()（失败 throw "fetch"）+
//   get_ushort(0,out)（失败 throw "get_ushort"）；否则 out=0；返 true。
//   catch(const char* e) → trace(0xbc0b) "select purchase count (error:%s)" → false。
// ============================================================================
bool DB_UpdateCeraShopPurchaseCount::_selectPurchaseCount(unsigned int mId,
                                                          unsigned short& out)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        if (!db->set_query(
                " seLect purchase_cnt from event_purchase_cnt where m_id = %s ",
                NumberToString(mId, 0)))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        if (db->get_n_rows() == 1)
        {
            if (!db->fetch())
                throw "fetch";
            if (!db->get_ushort(0, out))
                throw "get_ushort";
        }
        else
        {
            out = 0;
        }
        return true;
    }
    catch (const char* e)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbc0b, 5);
        trace("[PURCAHSE BONUS] select purchase count (error:%s)", e);
        return false;
    }
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_updatePurchaseCount @ 0x0844b680
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→ set_query(
//   " upDate event_purchase_cnt set purchase_cnt=%d where m_id = %s ",
//   count, NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)
//   （失败 throw "exec"）。catch(const char* e) → trace(0xbc22)
//   "update purchase count (error:%s)"。void 返回。
// ============================================================================
void DB_UpdateCeraShopPurchaseCount::_updatePurchaseCount(unsigned int mId,
                                                          unsigned short count)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        if (!db->set_query(
                " upDate event_purchase_cnt set purchase_cnt=%d where m_id = %s ",
                count, NumberToString(mId, 0)))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbc22, 5);
        trace("[PURCAHSE BONUS] update purchase count (error:%s)", e);
    }
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_insertPurchaseCount @ 0x0844b808
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→ set_query(
//   " inSert into event_purchase_cnt(m_id, purchase_cnt) values(%s, 1) ",
//   NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)
//   （失败 throw "exec"）。catch(const char* e) → trace(0xbc39)
//   "update purchase count (error:%s)"。void 返回。
// ============================================================================
void DB_UpdateCeraShopPurchaseCount::_insertPurchaseCount(unsigned int mId)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        if (!db->set_query(
                " inSert into event_purchase_cnt(m_id, purchase_cnt) values(%s, 1) ",
                NumberToString(mId, 0)))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbc39, 5);
        trace("[PURCAHSE BONUS] update purchase count (error:%s)", e);
    }
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::makeRequest @ 0x0844b31c
// 逐指令转写：Acquire("DBThread.cpp",0xbbd6) → CStreamGuard(s,true) →
//   *guard<<0x2d9 → *guard<<param4 → guard->GetInBuffer<SIG>()（null 则
//   trace(0xbbde) "ptrData is null" 返）→ sig+0xc=count；+0=accId；+4(8B)=itemUid；
//   +8(2B)=0 → MsgQueueMgr::put(DB_Q=2, guard)。
// ============================================================================
void DB_UpdateCeraShopPurchaseCount::makeRequest(unsigned int accId,
                                                 unsigned long itemUid,
                                                 int count, int param4)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbd6), true);
    guard.operator*()->operator<<((int)0x2d9);
    guard.operator*()->operator<<(param4);
    SIG_CERASHOP_PURCHASE_COUNT* sig =
        guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (sig == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbbde, 5);
        trace("[PURCAHSE BONUS] ptrData is null");
        return;  // ORIG jmp → ~CStreamGuard 后 ret（无入队）
    }

    *(int*)((char*)sig + 0xc) = count;
    *(unsigned int*)((char*)sig + 0x0) = accId;
    *(unsigned long long*)((char*)sig + 0x4) = itemUid;
    *(unsigned short*)((char*)sig + 0x8) = 0;

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ============================================================================
// DB_SavePuUserCheck::makeRequest —— ORIG T 0x0844c098
// _ZN18DB_SavePuUserCheck11makeRequestEj（LinkE2.JpnExImpl 批次追加：
// ItemVendingMachine::BillingRecord 调用链 producer）。
// 逐指令转写：Acquire("DBThread.cpp", 0xbcf4) → guard(s,true) → << 0x2e2 →
//   << -1 → << accId → MsgQueueMgr::put(QUEUE_IDX 2, guard)。
// ============================================================================
class DB_SavePuUserCheck
{
public:
    DB_SavePuUserCheck() {}  // ORIG W C1 0x844fc1e（0x4 基类 ctor 形态）
    static void makeRequest(unsigned int accId);
};

void DB_SavePuUserCheck::makeRequest(unsigned int accId)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbcf4);
    CStreamGuard guard(stream, true);
    guard << (int)0x2e2;   // SIG opcode
    guard << (int)-1;
    guard << accId;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}
