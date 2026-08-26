// df_game_r Game/ ARAD::DISPATCHER 内部流派发器体系实现。
// 依据 docs/class_func_reports/ARAD__DISPATCHER__*/ 各报告 + ORIG nm/objdump。
#include "Arad_InternalDispatcher.h"
#include "CStreamGuard.h"
#include <algorithm>
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "LogManager.h"
#include "Stream.h"
#include "MoreTypes.h"      // arad::Sig* 线格式结构

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
bool Arad_DB_kLoadRewardEventItem::dispatch(int code, int uid, Stream* stream) { return false; }
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
bool Arad_DB_kLoadServerState::dispatch(int code, int uid, Stream* stream) { return false; }
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
bool Arad_DB_kAvatarEnduranceSave::dispatch(int code, int uid, Stream* stream) { return false; }
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

}  // namespace DISPATCHER
}  // namespace ARAD
