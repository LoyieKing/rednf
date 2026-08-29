// df_game_r Game/ ARAD::DISPATCHER 内部流派发器体系实现。
// 依据 docs/class_func_reports/ARAD__DISPATCHER__*/ 各报告 + ORIG nm/objdump。
#include "Arad_InternalDispatcher.h"
#include "CStreamGuard.h"
#include "PacketGuard.h"
#include "GameEnums.h"
#include <algorithm>
#include "GlobalData.h"
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get
#include "MySQL.h"
#include "AvatarCoin.h"
#include "DBMgr.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "Stream.h"
#include "MoreTypes.h"      // arad::Sig* 线格式结构
#include "StreamPool.h"    // GlobalData::s_stream_pool->Acquire（LoadRewardEventItem/LoadServerState）
#include "MsgQueueMgr.h"   // MsgQueueMgr::DISPATCH_Q / put（LoadRewardEventItem/LoadServerState）

// ============================================================================
// arad::SigSaveRewardEventItem 生命周期（置于全局作用域；MoreTypes.h namespace arad 声明）（声明于 MoreTypes.h；ORIG 见其注释）
// ============================================================================
void arad::SigSaveRewardEventItem::init()
{
    set(0, 0, 0, 0, 0, 0);
}

void arad::SigSaveRewardEventItem::set(int a, unsigned int b, unsigned int c,
                                       unsigned long d, short e, unsigned int f)
{
    // 按参数名走【字段语义】写入；ORIG 栈槽读位见 MoreTypes.h 注释。
    m_field00 = a;
    m_field04 = b;
    m_field08 = c;
    m_field10 = f;          // 【推断】ORIG 写入第4个 dword（栈槽 +0x18）
    m_field14 = (short)e;   // word（ORIG 槽 +0x1c 的低16位）
    m_field0c = (unsigned int)e >> 16;  // 保持写入动作覆盖（值语义不可证）
}


namespace ARAD
{

namespace DISPATCHER
{

// ============================================================================
// make_internal_stream_jpn —— ORIG 0x081977b6 T（ARAD__DISPATCHER/make_internal_stream_jpn.md）
// guard->0x2dc(魔数) → param → code；写入序按报告 Ghidra C 还原。
// ============================================================================
void make_internal_stream_jpn(CStreamGuard& guard, ENUM_INTERNALPACKET_JPN code, int param)
{
    guard.operator*()->operator<<((int)0x2dc);
    guard.operator*()->operator<<((int)param);
    guard.operator*()->operator<<((int)code);
}

// ============================================================================
// packetheader_jpn 【objdump】ORIG make_cmd_packetheader_jpn 0x08197812 T /
// make_noti_packetheader_jpn 0x08197862 T（无独立 md 报告，直接反汇编转写）：
//   bool 归一化(0/1) 后 put_header((int)code, typeFlag) + put_byte(normalized)；
//   cmd 流 typeFlag=1，noti 流 typeFlag=0。
// ============================================================================
void make_cmd_packetheader_jpn(PacketGuard& guard, ENUM_CMDPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header((int)code, 1);
    guard.put_byte((int)normalized);
}

void make_noti_packetheader_jpn(PacketGuard& guard, ENUM_NOTIPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header((int)code, 0);
    guard.put_byte((int)normalized);
}

// ============================================================================
// Arad_IInterDispatcher —— ORIG ctor 0x0819a36a / D1 0x0819a382 / get_code 0x819a3c6
// ============================================================================
Arad_IInterDispatcher::Arad_IInterDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    m_code = code;
}

Arad_IInterDispatcher::~Arad_IInterDispatcher()
{
}

// ============================================================================
// Arad_IDatabaseDispatcher —— ORIG ctor 0x081867a4 / D1 0x081867bc / get_code 0x8186800
// ============================================================================
Arad_IDatabaseDispatcher::Arad_IDatabaseDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    m_code = code;
}

Arad_IDatabaseDispatcher::~Arad_IDatabaseDispatcher()
{
}

// ============================================================================
// Arad_DatabaseDispatcher —— 容器（ARAD__DISPATCHER__Arad_DatabaseDispatcher 各报告）
// ============================================================================

// ORIG ctor 0x08184c7c：map C1 后跟一次显式 map::clear。
Arad_DatabaseDispatcher::Arad_DatabaseDispatcher()
{
    m_dispatchers.clear();
}

// ORIG ~D1 0x08184cbe：for_each(begin,end,DeleteDispatcher 删除各派发器) 后析构 map。
Arad_DatabaseDispatcher::~Arad_DatabaseDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), DeleteDispatcher());
}

// ORIG 0x081850e0：find(code)；!=(end) 返回 second，否则 NULL。
Arad_IDatabaseDispatcher* Arad_DatabaseDispatcher::GetDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>::iterator it = m_dispatchers.find(code);
    if (it != m_dispatchers.end())
        return it->second;
    return 0;
}

// ORIG 0x08185144：GetDispatcher 为空则 map[code]=d；返回"之前不存在"。
bool Arad_DatabaseDispatcher::_registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IDatabaseDispatcher* d)
{
    // ORIG 0x08185144：已存在则立即 false 早退；否则注册并 true。
    if (GetDispatcher(code) != 0)
        return false;
    m_dispatchers[code] = d;
    return true;
}


// ORIG 0x0818518a：不存在则直接返回；命中则 erase。
// ORIG 0x0818518a 命中路径：erase(find(code))。
void Arad_DatabaseDispatcher::_unregistDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    if (GetDispatcher(code) != 0)
        m_dispatchers.erase(m_dispatchers.find(code));
}

// ORIG 0x0818680c W：delete pair.second。
void Arad_DatabaseDispatcher::DeleteDispatcher::operator()(
    std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>& p)
{
    delete p.second;
}

// ============================================================================
// Arad_InternalDispatcher —— 容器（与 Database 同构；ORIG 0x081984d4 起）
// ============================================================================
Arad_InternalDispatcher::Arad_InternalDispatcher()
{
    // ORIG 0x081984d4：map C1 后跟一次显式 map::clear。
    m_dispatchers.clear();
}

Arad_InternalDispatcher::~Arad_InternalDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), DeleteDispatcher());
}

Arad_IInterDispatcher* Arad_InternalDispatcher::GetDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*>::iterator it = m_dispatchers.find(code);
    if (it != m_dispatchers.end())
        return it->second;
    return 0;
}

bool Arad_InternalDispatcher::_registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IInterDispatcher* d)
{
    // ORIG 0x08198a02：已存在则立即 false 早退；否则注册并 true。
    if (GetDispatcher(code) != 0)
        return false;
    m_dispatchers[code] = d;
    return true;
}

void Arad_InternalDispatcher::_unregistDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    // ORIG 0x08198a48：命中则 erase(find(code))。
    if (GetDispatcher(code) != 0)
        m_dispatchers.erase(m_dispatchers.find(code));
}

void Arad_InternalDispatcher::DeleteDispatcher::operator()(
    std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*>& p)
{
    delete p.second;
}

// ============================================================================
// Init —— ORIG Arad_DatabaseDispatcher::Init 0x08184d40 T
//   （ARAD__DISPATCHER__Arad_DatabaseDispatcher/Init.md）：
//   顺序 new 9 个 DB 流派发器并注册 get_code()；任一步 new 失败或注册冲突即
//   return false（ORIG 未对失败对象做清理），全部成功返回 true。
// ============================================================================
bool Arad_DatabaseDispatcher::Init()
{
    {
        // ORIG new@08184d55 + ctor 0x8186836（code=K_ARAD_EVENT_ITEM_LOG）
        Arad_DB_kAradEventItemLog* d = new Arad_DB_kAradEventItemLog();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x81868a8（code=K_SET_CHARAC_CREATE_TIME）
        Arad_DB_kSetCharacCreateTime* d = new Arad_DB_kSetCharacCreateTime();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x818691a（code=K_LOAD_REWARD_EVENT_ITEM）
        Arad_DB_kLoadRewardEventItem* d = new Arad_DB_kLoadRewardEventItem();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x818698c（code=K_SAVE_REWARD_EVENT_ITEM）
        Arad_DB_kSaveRewardEventItem* d = new Arad_DB_kSaveRewardEventItem();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x81869fe（code=K_SAVE_SERVER_STATE）
        Arad_DB_kSaveServerState* d = new Arad_DB_kSaveServerState();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x8186a70（code=K_LOAD_SERVER_STATE）
        Arad_DB_kLoadServerState* d = new Arad_DB_kLoadServerState();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x8186ae2（code=K_DB_AVATAR_HIDDEN_OPTION_SAVE）
        Arad_DB_kAvatarHiddenOptionSave* d = new Arad_DB_kAvatarHiddenOptionSave();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x8186b54（code=K_DB_AVATAR_ENDURANCE_SAVE）
        Arad_DB_kAvatarEnduranceSave* d = new Arad_DB_kAvatarEnduranceSave();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x8186bc6（code=K_RESET_LIMITED_CREATE_CHARAC_PER_MID）
        Arad_DB_kResetLimitedCreateCharacPerMid* d =
            new Arad_DB_kResetLimitedCreateCharacPerMid();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    return true;
}

// ============================================================================
// Init —— ORIG Arad_InternalDispatcher::Init 0x08198598 T
//   （ARAD__DISPATCHER__Arad_InternalDispatcher/Init.md）：同构，10 个 INTER 流。
// ============================================================================
bool Arad_InternalDispatcher::Init()
{
    {
        // ORIG new@081985ad + ctor 0x819a46e（code=K_LOAD_REWARD_EVENT_ITEM）
        Arad_INTER_kLoadRewardEventItem* d = new Arad_INTER_kLoadRewardEventItem();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a4e0（code=K_SAVE_SERVER_STATE）
        Arad_INTER_kSaveServerState* d = new Arad_INTER_kSaveServerState();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a552（code=K_LOAD_SERVER_STATE）
        Arad_INTER_kLoadServerState* d = new Arad_INTER_kLoadServerState();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a5c4（code=K_DELETE_SERVER_STATE）
        Arad_INTER_kDeleteServerState* d = new Arad_INTER_kDeleteServerState();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a3fc（code=K_AVATAR_ROULETTE）
        Arad_INTER_kAvatarRoulette* d = new Arad_INTER_kAvatarRoulette();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a636（code=K_ARAD_JUMPING_CHARACTER）
        Arad_INTER_kAradJumpingCharacter* d = new Arad_INTER_kAradJumpingCharacter();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a6a8（code=K_AVATAR_HIDDEN_OPTION_CHANGE）
        Arad_INTER_kAvatarHiddenOptionChange* d =
            new Arad_INTER_kAvatarHiddenOptionChange();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a71a（code=K_USE_AVATAR_RECHARGE）
        Arad_INTER_kUseAvatarRecharge* d = new Arad_INTER_kUseAvatarRecharge();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a78c（code=K_EMBLEM_COMPOUND）
        Arad_INTER_kEmblemCompound* d = new Arad_INTER_kEmblemCompound();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    {
        // ORIG ctor 0x819a7fe（code=K_AVATAR_CONVERT）
        Arad_INTER_kAvatarConvert* d = new Arad_INTER_kAvatarConvert();
        if (d == 0 || !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
            return false;
    }
    return true;
}


// ============================================================================
// 具体 DB 流派发器 —— ctor 形态（ORIG 各 ctor 0x24 字节反汇编）：
//   调基类 Arad_IDatabaseDispatcher(enum 立即数) 后由编译器写自身 vptr；
//   析构隐式（ORIG W D1/D0，仅置基类 vptr + 调基类析构）。
// ============================================================================
#define ARAD_DB_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IDatabaseDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kAradEventItemLog, 0x20)
Arad_DB_kAradEventItemLog::~Arad_DB_kAradEventItemLog() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kSetCharacCreateTime, 0x24)
Arad_DB_kSetCharacCreateTime::~Arad_DB_kSetCharacCreateTime() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kLoadRewardEventItem, 0x26)
Arad_DB_kLoadRewardEventItem::~Arad_DB_kLoadRewardEventItem() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kSaveRewardEventItem, 0x27)
Arad_DB_kSaveRewardEventItem::~Arad_DB_kSaveRewardEventItem() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kLoadServerState, 0x29)
Arad_DB_kLoadServerState::~Arad_DB_kLoadServerState() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kSaveServerState, 0x2a)
Arad_DB_kSaveServerState::~Arad_DB_kSaveServerState() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kAvatarHiddenOptionSave, 0x2f)
Arad_DB_kAvatarHiddenOptionSave::~Arad_DB_kAvatarHiddenOptionSave() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kAvatarEnduranceSave, 0x31)
Arad_DB_kAvatarEnduranceSave::~Arad_DB_kAvatarEnduranceSave() {}

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kResetLimitedCreateCharacPerMid, 0x37)
Arad_DB_kResetLimitedCreateCharacPerMid::~Arad_DB_kResetLimitedCreateCharacPerMid() {}

// dispatch 函数体逐个按报告转写（本文件后续区段）。
bool Arad_DB_kAradEventItemLog::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081851dc：inSert into log_arad_event_stat (...) values (now(), %u, %u, %u, %d)。
    arad::SigAradEventItemLog* sig = stream->GetOutBuffer<arad::SigAradEventItemLog>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x643, "EVENT ITEM LOG, SigAradEventItemLog is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " inSert into log_arad_event_stat ( occ_time, m_id, charac_no, item_id, item_cnt )  values "
                " ( now(), %u, %u, %u, %d ) ",
                sig->m_id, sig->m_characNo, sig->m_itemId, sig->m_itemCount))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x661,
                              "[EVENT_LOG] DB_DSP Error, insert log to log_arad_event_stat. (error:%s)",
                              e);
    }
    return false;
}
bool Arad_DB_kSetCharacCreateTime::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081853b2：upDate charac_info set create_time=from_unixtime(%u) where charac_no=%u。
    arad::SigSetCharacCreateTime* sig = stream->GetOutBuffer<arad::SigSetCharacCreateTime>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x773, "SigSetCharacCreateTime is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " upDate charac_info set create_time = from_unixtime(%u)  where charac_no = %u ",
                sig->m_createTime, sig->m_characNo))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x788, "[SetCharacCreateTime] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}
// ORIG 0x08185574：select event_id, charac_no ... limit 10 →
//   SigLoadRewardEventItem(count + eventId[10] + characNo[10]) →
//   make_internal_stream_jpn(0x26, uid) + put_binary(0x56) →
//   MsgQueueMgr::put(DISPATCH_Q)。返回值 = put 的返回值。
bool Arad_DB_kLoadRewardEventItem::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08186f7c：GetOutBuffer<arad::SigLoadRewardEventItem> 模板实例。
    arad::SigLoadRewardEventItem* sig = stream->GetOutBuffer<arad::SigLoadRewardEventItem>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x796, "SigRewardEventItem is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";

        int curSec = GlobalData::s_systemTime_.getCurSec();

        if (!db->set_query(
                " seLect event_id, charac_no from event_reward_item_arad "
                " where (m_id = %u and end_date > from_unixtime(%u))  "
                " order by occ_date desc limit 10 ",
                sig->m_field0, curSec))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";

        // 行循环：col0=event_id(int) → m_eventCodes[i]；
        //         col1=charac_no(uint) → m_characNos[i]；每行 m_count++。
        int i = 0;
        arad::SigLoadRewardEventItem out;
        while (db->fetch())
        {
            int eventId = 0;
            if (!db->get_int(0, eventId))
                throw "EventCode";
            unsigned int characNo = 0;
            if (!db->get_uint(1, characNo))
                throw "Character";
            out.m_eventCodes[i] = eventId;
            out.m_characNos[i] = characNo;
            ++i;
            ++out.m_count;
        }

        Stream* acquired = GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_DatabaseDispatcher.cpp", 0x7b8);
        CStreamGuard guard(acquired, true);
        make_internal_stream_jpn(guard, (ENUM_INTERNALPACKET_JPN)0x26, uid);
        guard.put_binary(&out, 0x56);

        bool putRet = GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return putRet;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7c0,
                              "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}
bool Arad_DB_kSaveRewardEventItem::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081858de：inSert into event_reward_item_arad values (%d,%u,%u,%u,from_unixtime(%u),now())。
    arad::SigSaveRewardEventItem* sig = stream->GetOutBuffer<arad::SigSaveRewardEventItem>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7cc, "SigRewardEventItem is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " inSert into event_reward_item_arad values (%d, %u, %u, %u, from_unixtime(%u), now())",
                sig->m_field00, sig->m_field04, sig->m_field08,
                sig->m_field10, (unsigned short)sig->m_field14))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e2)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7e1, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e2);
    }
    return false;
}
// ORIG 0x08185d20：select category, code, state where end_time >= now() →
//   SigServerState（m_flag/m_count + tagData[20]，逐行填 m_category/m_code/
//   get_binary(msg,0xc)）→ make_internal_stream_jpn(0x29, -1) + put_binary(0x195)
//   → DISPATCH_Q，成功返回 true。
bool Arad_DB_kLoadServerState::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08187024：GetOutBuffer<arad::SigServerState> 模板实例。
    arad::SigServerState* sig = stream->GetOutBuffer<arad::SigServerState>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x81e, "SigServerState is null.");
        return false;
    }
    (void)sig;   // 取出仅作非空校验（ORIG 同样不再读该缓冲字段）

    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";

        if (!db->set_query(
                " seLect category, code, state from server_state_info where end_time >= now()"))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";

        int i = 0;
        arad::SigServerState out;
        while (db->fetch())
        {
            int category = 0;
            if (!db->get_int(0, category))
                throw "category";
            int stateCode = 0;
            if (!db->get_int(1, stateCode))
                throw "category";
            out.m_data[i].m_category = category;
            out.m_data[i].m_code = stateCode;
            if (!db->get_binary(2, &out.m_data[i].m_msg, 0xc))
                throw "state";
            i = i + 1;
        }

        Stream* acquired = GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_DatabaseDispatcher.cpp", 0x83f);
        CStreamGuard guard(acquired, true);

        make_internal_stream_jpn(guard, (ENUM_INTERNALPACKET_JPN)0x29, -1);
        guard.put_binary(&out, 0x195);

        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x846,
                              "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}
bool Arad_DB_kSaveServerState::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08185abe：flag==0 → deLete by category/code；否则 inSert(msg blob→str)。
    arad::SigServerState* sig = stream->GetOutBuffer<arad::SigServerState>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7ef, "SigServerState is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (sig->m_flag == 0)
        {
            if (!db->set_query(" deLete from server_state_info where category=%d and code=%d ",
                               sig->m_data[0].m_category, sig->m_data[0].m_code))
                throw "set_query";
        }
        else
        {
            unsigned int endSec = sig->m_data[0].m_msg.m_endSec;
            char* msgStr = db->blob_to_str(0, &sig->m_data[0].m_msg, 0xc);
            if (!db->set_query(
                    " inSert into server_state_info values (%d, %d, '%s', now(), from_unixtime(%u)) ",
                    sig->m_data[0].m_category, sig->m_data[0].m_code, msgStr, endSec))
                throw "set_query";
        }
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x812, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}
bool Arad_DB_kAvatarHiddenOptionSave::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081860f4：upDate user_items set hidden_option where charac_no/slot/it_id。
    arad::SigAradAvatarHiddenOptionSave* sig =
        stream->GetOutBuffer<arad::SigAradAvatarHiddenOptionSave>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x854, "SigAradAvatarHiddenOptionSave is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                "upDate user_items set hidden_option=%d where charac_no=%d and slot=%d and it_id=%d",
                (int)sig->m_hiddenOption, sig->m_characNo, (int)sig->m_slot, sig->m_itId))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x868, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}
bool Arad_DB_kAvatarEnduranceSave::dispatch(int code, int uid, Stream* stream)
{
    (void)code;
    (void)uid;
    arad::SigAradAvatarEnduranceSave* sig =
        stream->GetOutBuffer<arad::SigAradAvatarEnduranceSave>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x876,
                              "SigAradAvatarEnduranceSave is null.");
        return false;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";

        // ORIG set_query(h, fmt, +0x04=emblem_endurance, +0x00=ui_id)
        if (!db->set_query("upDate user_items set emblem_endurance=%d where ui_id=%d",
                           sig->endurance, sig->uiId))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x88a,
                              "[SigAradAvatarEnduranceSave] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}

bool Arad_DB_kResetLimitedCreateCharacPerMid::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08186492：truncate limit_create_character。
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query("truncate limit_create_character"))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        return true;
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x8e1, "[ResetLimitedCreateCharacPerMid] DB_DSP Error,  (error:%s)", e);
    }
    return false;
}

// ============================================================================
// 具体 INTER 流派发器
// ============================================================================
#define ARAD_INTER_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IInterDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarRoulette, 0x2c)
Arad_INTER_kAvatarRoulette::~Arad_INTER_kAvatarRoulette() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kLoadRewardEventItem, 0x26)
Arad_INTER_kLoadRewardEventItem::~Arad_INTER_kLoadRewardEventItem() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kSaveServerState, 0x2a)
Arad_INTER_kSaveServerState::~Arad_INTER_kSaveServerState() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kLoadServerState, 0x29)
Arad_INTER_kLoadServerState::~Arad_INTER_kLoadServerState() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kDeleteServerState, 0x2b)
Arad_INTER_kDeleteServerState::~Arad_INTER_kDeleteServerState() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAradJumpingCharacter, 0x2d)
Arad_INTER_kAradJumpingCharacter::~Arad_INTER_kAradJumpingCharacter() {}
// dispatch_sig/clearEpicQuest/sendHelperBox 真实现见 Arad_InternalDispatcher_INTER.cpp。

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarHiddenOptionChange, 0x2e)
Arad_INTER_kAvatarHiddenOptionChange::~Arad_INTER_kAvatarHiddenOptionChange() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kUseAvatarRecharge, 0x30)
Arad_INTER_kUseAvatarRecharge::~Arad_INTER_kUseAvatarRecharge() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kEmblemCompound, 0x32)
Arad_INTER_kEmblemCompound::~Arad_INTER_kEmblemCompound() {}

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarConvert, 0x33)
Arad_INTER_kAvatarConvert::~Arad_INTER_kAvatarConvert() {}

// ============================================================================
// cmd 流业务派发器 checkWorkState —— ORIG 各 W 函数逐条转写。
//   依赖：GlobalData::s_pSecuProtectionField、ARAD::Singleton<T>::Get、
//         各 Server::CheckCondition / AvatarRechargeServer::
//         CheckConditionUseRecharge（定义见各 Server 权威 TU）。
// ============================================================================
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get

// ORIG Dispatcher_AradAvatarConvert::checkWorkState 0x081a003e W：
//   Secu Check(user,0x2a) 非 0 → SendCmdErrorPacket(0x24f,err) 返回 false；
//   否则 Singleton<AvatarConvertServer>::Get()->CheckCondition(user,&sig)。
bool Dispatcher_AradAvatarConvert::checkWorkState(CUser* user,
                                                  ::arad::SigAradAvatarConvert& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, (unsigned char)secuErr);
        return false;
    }
    return Singleton<AvatarConvertServer>::Get()->CheckCondition(user, &sig);
}

// ORIG Dispatcher_AradEmblemCompound::checkWorkState 0x0819fcb4 W：
//   ServiceRestrictManager::isRestricted(user,Enum_1,0x15) → 错误 0xd1；
//   Secu Check(user,0x2a)；最后 EmblemCompoundServer::CheckCondition。
bool Dispatcher_AradEmblemCompound::checkWorkState(CUser* user,
                                                   ::arad::SigAradEmblemCompound& sig)
{
    if (Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x15))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd1);
        return false;
    }
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, (unsigned char)secuErr);
        return false;
    }
    return Singleton<EmblemCompoundServer>::Get()->CheckCondition(user, &sig);
}

// ORIG Dispatcher_AradUseAvatarRecharge::checkWorkState 0x0819f9d6 W。
bool Dispatcher_AradUseAvatarRecharge::checkWorkState(CUser* user,
                                                      ::arad::SigAradUseAvatarRecharge& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, (unsigned char)secuErr);
        return false;
    }
    return Singleton<AvatarRechargeServer>::Get()->CheckConditionUseRecharge(user, &sig);
}

// ORIG Dispatcher_AvartarHiddenOptionChange::checkWorkState 0x0819f956 W。
bool Dispatcher_AvartarHiddenOptionChange::checkWorkState(CUser* user,
                                                          ::arad::SigAvatarHiddenOptionChange& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, (unsigned char)secuErr);
        return false;
    }
    return Singleton<AvatarFixedHiddenOptionServer>::Get()->CheckCondition(user, &sig);
}

// ORIG Dispatcher_AvartarRoulette::checkWorkState 0x0819ee6c W：
//   Secu Check(user,0x45) → err 透传；AvatarCoin(+0x658 ABI 镜像) 不可用 →
//   错误 0x16；GetRemainCapacity(INVEN 2, ITEM_TYPE_0)<=0 → 错误 4。
bool Dispatcher_AvartarRoulette::checkWorkState(CUser* user,
                                                ::arad::SigAvatarRoulette& sig)
{
    (void)sig;   // ORIG 不读 Sig 字段
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x45);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, (unsigned char)secuErr);
        return false;
    }
    // AvatarCoin 位于 Inven 数组尾部固定偏移 +0x658【ABI 镜像】（同
    // Arad_InternalDispatcher_INTER.cpp kAvatarRoulette 段惯例）。
    const AvatarCoin* coin =
        (const AvatarCoin*)(user->getCurCharacInvenR() + 0x658);
    if (!coin->IsUsable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 0x16);
        return false;
    }
    if (user->getCurCharacInvenR()->GetRemainCapacity(
            (INVEN_TYPE)2, Inven_Item::ITEM_TYPE_0) <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 4);
        return false;
    }
    return true;
}

// ORIG Dispatcher_AradJumpingCharacter::checkWorkState 0x0819f44c W：
//   Secu Check(user,0x2a)；eventKind==2 时要求存在 m_level>0x3b 的角色，
//   否则错误 8；当前角色 level>0x11 → 错误 0x12；取 (INVEN 1,
//   sig.m_itemIndex 槽) 物品：空 → 错误 0x11；eventKind==1 须 item idx
//   ==0x28d468、eventKind==2 须 ==0x28d469，否则错误 0x11；其余 eventKind
//   → 错误 0x11。全部通过返回 true。
bool Dispatcher_AradJumpingCharacter::checkWorkState(CUser* user,
                                                     ::arad::SigAradJumpingCharacter& sig)
{
    unsigned char jumpKind = (unsigned char)sig.m_eventKind;      // ORIG movzbl +9
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, (unsigned char)secuErr);
        return false;
    }

    bool hasLevelOver0x3b = false;                                // ORIG -0x15
    if (jumpKind == 2)
    {
        for (std::size_t i = 0; i < user->m_characList.size(); ++i)
        {
            if (user->m_characList[i].m_level > 0x3b)             // elem +0x27 word
            {
                hasLevelOver0x3b = true;
                break;
            }
        }
        if (!hasLevelOver0x3b)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 8);
            return false;
        }
    }

    // 当前角色等级门（ORIG cmp $0x11 / setg：level>0x11 报错 0x12）。
    if (user->get_charac_level() > 0x11)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x12);
        return false;
    }

    CInventory* inv = user->getCurCharacInvenW();
    Inven_Item item = inv->GetInvenSlot(1, (int)(unsigned short)sig.m_itemIndex);
    if (!item.isEmpty())
    {
        if (jumpKind == 1)
        {
            if (item.m_addInfo != 0x28d468)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
                return false;
            }
        }
        else
        {
            if (jumpKind != 2)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
                return false;
            }
            if (item.m_addInfo != 0x28d469)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
                return false;
            }
        }
        return true;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
    return false;
}
}  // namespace DISPATCHER
}  // namespace ARAD

