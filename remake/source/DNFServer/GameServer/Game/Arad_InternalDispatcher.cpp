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

// [A31-GI · P327] Arad_InternalDispatcher::C1 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN4ARAD10DISPATCHER23Arad_InternalDispatcherC2Ev`（ORIG 0x819a31a）。
// 零语义/机器码变化，仅发射序。
ARAD::DISPATCHER::Arad_InternalDispatcher::Arad_InternalDispatcher()
{
    // ORIG 0x081984d4：map C1 后跟一次显式 map::clear。
    m_dispatchers.clear();
}

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN4ARAD10DISPATCHER23Arad_DatabaseDispatcherC2Ev`
// （P327：GI 名 = TU 内第一个「强符号·外部链接·本 TU 定义」实体；全限定定义
//  可写在命名空间块之外，发射序因此提前）。**语义与机器码零变化，仅发射序。**
// ---------------------------------------------------------------------------
// ORIG ctor 0x08184c7c：map C1 后跟一次显式 map::clear。
ARAD::DISPATCHER::Arad_DatabaseDispatcher::Arad_DatabaseDispatcher()
{
    m_dispatchers.clear();
}

// ============================================================================
// arad::SigSaveRewardEventItem 生命周期（置于全局作用域；MoreTypes.h namespace arad 声明）（声明于 MoreTypes.h；ORIG 见其注释）
// ============================================================================
// ORIG 0x08195ce8：6 dword 清零（+0x00..+0x14）。
void arad::SigSaveRewardEventItem::init()
{
    // ORIG：6 dword 清零通过 set(0,0,0,0,0,0) 栈传参（0x08195ce8）。
    set(0, 0, 0, 0, 0, 0);
}

void arad::SigSaveRewardEventItem::set(int a, unsigned int b, unsigned int c,
                                       unsigned long d, short e, unsigned int f)
{
    // ORIG 0x08195d18 栈槽：0xc→+0x00、0x10→+0x04、0x14→+0x08、
    //   0x18(d 低32位)→+0x10、0x1c(e 低16位)→+0x14、0x20(f)→+0x0c。
    m_field00 = a;
    m_field04 = b;
    m_field08 = c;
    m_field10 = (unsigned int)d;
    m_field14 = (short)e;
    m_field0c = f;
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
//   bool 归一化(0/1) 后 put_header(typeFlag, (int)code) + put_byte(normalized)；
//   cmd 流 typeFlag=1，noti 流 typeFlag=0。
// ============================================================================
void make_cmd_packetheader_jpn(PacketGuard& guard, ENUM_CMDPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header(1, (int)code);
    guard.put_byte((int)normalized);
}

void make_noti_packetheader_jpn(PacketGuard& guard, ENUM_NOTIPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header(0, (int)code);
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
// （C1 定义已按 P327 前移至 TU 首 [A31-GI] 段）
// ============================================================================
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
    // ORIG 0x08184d40：单槽 `Arad_IDatabaseDispatcher* d`(-0xc) 跨块复用、
    //   每块头 movl$0,-0xc 重新置空；d==0（0x08184d6b je）跳 regist 继续下块。
    Arad_IDatabaseDispatcher* d = 0;
    // ORIG new@08184d55 + ctor 0x8186836（code=K_ARAD_EVENT_ITEM_LOG）
    d = new Arad_DB_kAradEventItemLog();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x81868a8（code=K_SET_CHARAC_CREATE_TIME）
    d = new Arad_DB_kSetCharacCreateTime();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x818691a（code=K_LOAD_REWARD_EVENT_ITEM）
    d = new Arad_DB_kLoadRewardEventItem();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x818698c（code=K_SAVE_REWARD_EVENT_ITEM）
    d = new Arad_DB_kSaveRewardEventItem();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x81869fe（code=K_SAVE_SERVER_STATE）
    d = new Arad_DB_kSaveServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186a70（code=K_LOAD_SERVER_STATE）
    d = new Arad_DB_kLoadServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186ae2（code=K_DB_AVATAR_HIDDEN_OPTION_SAVE）
    d = new Arad_DB_kAvatarHiddenOptionSave();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186b54（code=K_DB_AVATAR_ENDURANCE_SAVE）
    d = new Arad_DB_kAvatarEnduranceSave();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186bc6（code=K_RESET_LIMITED_CREATE_CHARAC_PER_MID）
    d = new Arad_DB_kResetLimitedCreateCharacPerMid();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    return true;
}

// ============================================================================
// Init —— ORIG Arad_InternalDispatcher::Init 0x08198598 T
//   （ARAD__DISPATCHER__Arad_InternalDispatcher/Init.md）：同构，10 个 INTER 流。
// ============================================================================
bool Arad_InternalDispatcher::Init()
{
    // ORIG 0x08198598：同构，单槽 d 复用（-0xc）。
    Arad_IInterDispatcher* d = 0;
    // ORIG new@081985ad + ctor 0x819a46e（code=K_LOAD_REWARD_EVENT_ITEM）
    d = new Arad_INTER_kLoadRewardEventItem();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a4e0（code=K_SAVE_SERVER_STATE）
    d = new Arad_INTER_kSaveServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a552（code=K_LOAD_SERVER_STATE）
    d = new Arad_INTER_kLoadServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a5c4（code=K_DELETE_SERVER_STATE）
    d = new Arad_INTER_kDeleteServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a3fc（code=K_AVATAR_ROULETTE）
    d = new Arad_INTER_kAvatarRoulette();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a636（code=K_ARAD_JUMPING_CHARACTER）
    d = new Arad_INTER_kAradJumpingCharacter();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a6a8（code=K_AVATAR_HIDDEN_OPTION_CHANGE）
    d = new Arad_INTER_kAvatarHiddenOptionChange();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a71a（code=K_USE_AVATAR_RECHARGE）
    d = new Arad_INTER_kUseAvatarRecharge();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a78c（code=K_EMBLEM_COMPOUND）
    d = new Arad_INTER_kEmblemCompound();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a7fe（code=K_AVATAR_CONVERT）
    d = new Arad_INTER_kAvatarConvert();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    return true;
}


// ============================================================================
// 具体 DB 流派发器 —— ctor 形态（ORIG 各 ctor 0x24 字节反汇编）：
//   调基类 Arad_IDatabaseDispatcher(enum 立即数) 后由编译器写自身 vptr；
//   析构隐式（ORIG W D1/D0，仅置基类 vptr + 调基类析构）。
//   ORIG 各派生 ctor 帧为 sub $0x18（例 kAradEventItemLog@0x08186836、
//   kAvatarRoulette@0x0819a3fc），call outgoing 仅占 0x8 ⇒ 需 0x10 dead pad 撑帧。
// ============================================================================
#define ARAD_DB_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IDatabaseDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) \
    { char pad[0x10]; (void)pad; }

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
    //   返回值经 %ebx 缓存（4.1 EH 形）：早退 0x08185224 mov$0,%ebx、
    //   成功 0x0818535e? mov$1,%ebx、catch 0x08185343 mov$0,%ebx。
    register bool ret;
    arad::SigAradEventItemLog* sig = stream->GetOutBuffer<arad::SigAradEventItemLog>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x643, "EVENT ITEM LOG, SigAradEventItemLog is null.");
        ret = false;
        goto done;
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
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x661,
                              "[EVENT_LOG] DB_DSP Error, insert log to log_arad_event_stat. (error:%s)",
                              e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}
bool Arad_DB_kSetCharacCreateTime::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081853b2：upDate charac_info set create_time=from_unixtime(%u) where charac_no=%u。
    //   %ebx 缓存返回（catch 0x08185544 / 成功 0x08185565 / merge 0x0818556a）。
    register bool ret;
    arad::SigSetCharacCreateTime* sig = stream->GetOutBuffer<arad::SigSetCharacCreateTime>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x773, "SigSetCharacCreateTime is null.");
        ret = false;
        goto done;   // ORIG 0x08185346：`mov$0,%ebx; jmp merge`（共享 merge，勿复制）
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
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x788, "[SetCharacCreateTime] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}
// ORIG 0x08185574：select event_id, charac_no ... limit 10 →
//   SigLoadRewardEventItem(count + eventId[10] + characNo[10]) →
//   make_internal_stream_jpn(0x26, uid) + put_binary(0x56) →
//   MsgQueueMgr::put(DISPATCH_Q)。返回值 = put 的返回值。
// b28d-solo-Arad_InternalDispatcher: AE（clean 口径）——ORIG 0x08186f7c，204/204 条
//   助记符逐条全等，残差仅 .o 未解析重定位伪差；p167 调用点差集 CLEAR；
//   P0 已清寄存器绑定（N96；保留纯 register 存储类提示，P231 合规）。
bool Arad_DB_kLoadRewardEventItem::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08186f7c：GetOutBuffer<arad::SigLoadRewardEventItem> 模板实例。
    //   %ebx 缓存返回（4.1 EH 形）：put 结果 0x0818583e `mov %eax,%ebx`，
    //   catch 0x081858b0 `mov$0,%ebx`，merge 0x081858d1 `mov%ebx,%eax`。
    register bool ret;
    arad::SigLoadRewardEventItem* sig = stream->GetOutBuffer<arad::SigLoadRewardEventItem>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x796, "SigRewardEventItem is null.");
        ret = false;
        goto done;
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
        // ORIG 0x081856e0/0x08185738：get_int/get_uint 目标 = lea out+4*i(+0x28)+6/0xe
        //   直填字段（技巧37 无具名中间槽）；m_count++ 0x08185794（movzwl+add+mov%ax）。
        int i = 0;
        arad::SigLoadRewardEventItem out;
        while (db->fetch())
        {
            if (!db->get_int(0, *(int*)&(out.m_eventCodes[i])))
                throw "EventCode";
            // ORIG 0x08185746：i++ 发生在 get_uint 地址算好后、call 前
            //   ⇒ 源形为 get_uint(1, m_characNos[i++])（技巧4 后缀 ++）。
            if (!db->get_uint(1, *(unsigned int*)&(out.m_characNos[i++])))
                throw "Character";
            ++out.m_count;
        }

        // ORIG 0x081857ca：Acquire 结果直落 ctor 实参（无 acquired 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_DatabaseDispatcher.cpp", 0x7b8), true);
        make_internal_stream_jpn(guard, (ENUM_INTERNALPACKET_JPN)0x26, uid);
        // ORIG 0x08185806/0x0818581d：`call CStreamGuard::operator->` 后再
        //   `call CStreamGuard::put_binary`（p167 差集：ORIG 无 operator*）⇒
        //   源形为 `guard->put_binary(...)`（P167 判据）。
        guard->put_binary(&out, 0x56);

        ret = GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7c0,
                              "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
    }
done:
    return ret;
}
bool Arad_DB_kSaveRewardEventItem::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081858de：inSert into event_reward_item_arad values (%d,%u,%u,%u,from_unixtime(%u),now())。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigSaveRewardEventItem* sig = stream->GetOutBuffer<arad::SigSaveRewardEventItem>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7cc, "SigRewardEventItem is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " inSert into event_reward_item_arad values (%d, %u, %u, %u, from_unixtime(%u), now())",
                sig->m_field00, sig->m_field04, sig->m_field08,
                sig->m_field10, sig->m_field0c))   // ORIG 0x08185985：第5实参 = +0x0c dword 直存
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e2)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7e1, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e2);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}
// ORIG 0x08185d20：select category, code, state where end_time >= now() →
//   SigServerState（m_flag/m_count + tagData[20]，逐行填 m_category/m_code/
//   get_binary(msg,0xc)）→ make_internal_stream_jpn(0x29, -1) + put_binary(0x195)
//   → DISPATCH_Q，成功返回 true。
// b28d-solo-Arad_InternalDispatcher: AE（clean 口径）——ORIG 0x08187024，224/224 条
//   助记符逐条全等，残差仅 .o 未解析重定位伪差；p167 调用点差集 CLEAR；
//   P0 已清寄存器绑定（N96；保留纯 register 存储类提示，P231 合规）。
bool Arad_DB_kLoadServerState::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08187024：GetOutBuffer<arad::SigServerState> 模板实例。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigServerState* sig = stream->GetOutBuffer<arad::SigServerState>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x81e, "SigServerState is null.");
        ret = false;
        goto done;
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

        // ORIG 0x08185e53：out 在 exec 后构造；循环索引 = out.m_count 本身
        //   （0x08185e66 `mov -0x1b0(%ebp),%edx` = out+1 dword，无独立 i 槽）；
        //   m_count++ 尾置 0x08185fa0。
        arad::SigServerState out;
        while (db->fetch())
        {
            // ORIG 0x08185e6c/0x08185ee2：get_int 目标 = lea out+idx*0x14(+4) 直填
            //   字段（无具名中间槽，技巧37）。
            if (!db->get_int(0, *(int*)&(out.m_data[out.m_count].m_category)))
                throw "category";
            if (!db->get_int(1, *(int*)&(out.m_data[out.m_count].m_code)))
                throw "category";
            if (!db->get_binary(2, &out.m_data[out.m_count].m_msg, 0xc))
                throw "state";
            ++out.m_count;
        }

        // ORIG 0x08185fda：Acquire 结果 %eax 直落 guard ctor 实参（无 acquired 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_DatabaseDispatcher.cpp", 0x83f), true);

        make_internal_stream_jpn(guard, (ENUM_INTERNALPACKET_JPN)0x29, -1);
        // ORIG 0x08186017/0x08186031：`call CStreamGuard::operator->` 后再
        //   `call CStreamGuard::put_binary`（p167 差集：ORIG 无 operator*）⇒
        //   源形为 `guard->put_binary(...)`（P167 判据）。
        guard->put_binary(&out, 0x195);

        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x846,
                              "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    // ORIG 0x08186069/0x08186074：guard dtor 后 fall 入 `mov$1,%ebx` → merge
    //   ⇒ 成功赋值在 try-catch 之后（E 形，技巧23 系）。
    ret = true;
done:
    return ret;
}
bool Arad_DB_kSaveServerState::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08185abe：flag==0 → deLete by category/code；否则 inSert(msg blob→str)。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigServerState* sig = stream->GetOutBuffer<arad::SigServerState>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7ef, "SigServerState is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x08185b65 `movzbl (%eax),%eax; test; je →delete 岛`：flag 真值测试、
        //   fall-through = inSert 分支在前（技巧17）；blob_to_str/endSec 内联实参
        //   （技巧37，ORIG 0x08185b73 endSec→%ebx 先载、0x08185b96 结果 %eax 直存
        //   0x10(%esp)，无具名槽）。
        if (sig->m_flag)
        {
            if (!db->set_query(
                    " inSert into server_state_info values (%d, %d, '%s', now(), from_unixtime(%u)) ",
                    sig->m_data[0].m_category, sig->m_data[0].m_code,
                    db->blob_to_str(0, &sig->m_data[0].m_msg, 0xc),
                    sig->m_data[0].m_msg.m_endSec))
                throw "set_query";
        }
        else
        {
            if (!db->set_query(" deLete from server_state_info where category=%d and code=%d ",
                               sig->m_data[0].m_category, sig->m_data[0].m_code))
                throw "set_query";
        }
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x812, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}
bool Arad_DB_kAvatarHiddenOptionSave::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081860f4：upDate user_items set hidden_option where charac_no/slot/it_id。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigAradAvatarHiddenOptionSave* sig =
        stream->GetOutBuffer<arad::SigAradAvatarHiddenOptionSave>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x854, "SigAradAvatarHiddenOptionSave is null.");
        ret = false;
        goto done;
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
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x868, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}
bool Arad_DB_kAvatarEnduranceSave::dispatch(int code, int uid, Stream* stream)
{
    (void)code;
    (void)uid;
    // %ebx 缓存返回（4.1 EH 形，ORIG 0x081862d0 同族）。
    register bool ret;
    arad::SigAradAvatarEnduranceSave* sig =
        stream->GetOutBuffer<arad::SigAradAvatarEnduranceSave>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x876,
                              "SigAradAvatarEnduranceSave is null.");
        ret = false;
        goto done;
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
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x88a,
                              "[SigAradAvatarEnduranceSave] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}

bool Arad_DB_kResetLimitedCreateCharacPerMid::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08186492：truncate limit_create_character。
    // ORIG 4.1 EH 函数 bool 返回值经 %ebx 缓存、尾部 `mov %ebx,%eax` 出栈
    //   （成功 0x081865e8 `mov $0x1,%ebx`；catch 0x081865c7 `mov $0,%ebx`）。
    register bool ret;
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query("truncate limit_create_character"))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        // ORIG 0x08186492 布局：成功块 `mov$1,%ebx` 落在 cleanup 之后 fall 入
        //   merge（0x081865e8）⇒ 成功赋值须在 try-catch 之后（E 形，技巧23 系）。
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x8e1, "[ResetLimitedCreateCharacPerMid] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
    return ret;   // ORIG 0x081865ed 直存 `mov %ebx,%eax`（技巧22 同族：bool 绑 ebx 免归一）
}

// ============================================================================
// 具体 INTER 流派发器
// ============================================================================
#define ARAD_INTER_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IInterDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) \
    { char pad[0x10]; (void)pad; }   // ORIG 派生 ctor 帧 0x18（见 DB 宏注释）

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
//   否则 Singleton<AvatarConvertServer>::Get()->CheckCondition(user,&sig)；
//   尾部 ORIG 0x081a00a8 形 `call; xor $0x1,%eax; test; je →mov$1 岛` = !boolcall()（技巧1）。
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
    if (!Singleton<AvatarConvertServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
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
    // ORIG 0x0819fd68 !boolcall 尾形（技巧1）。
    if (!Singleton<EmblemCompoundServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
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
    // ORIG 0x0819fa40 !boolcall 尾形（技巧1）。
    if (!Singleton<AvatarRechargeServer>::Get()->CheckConditionUseRecharge(user, &sig))
        return false;
    return true;
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
    // ORIG 0x0819f956 族同尾形（技巧1）。
    if (!Singleton<AvatarFixedHiddenOptionServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
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
    // 注意：ORIG 0x0819eec9 `add $0x658,%eax`（char* 字节偏移、无具名 this 槽，
    //   技巧37）；0x0819eed6 `xor$1;test;je` = !boolcall 直出（技巧1）。
    if (!((const AvatarCoin*)((char*)user->getCurCharacInvenR() + 0x658))->IsUsable())
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
    // [b23S3C] P48：m_eventKind 三用点各重取（ORIG 无 jumpKind 槽）；hasLevel
    // 声明预清提前至函数头（ORIG `movb $0,-0x15` 在 Check call 前）；isEmpty
    // 正形早返（ORIG test;je else 岛）。残余 N 封顶：`Inven_Item item =
    // GetInvenSlot()` 4.4 copy-elision 直构 sret（N86，ORIG 4.1 先 ctor+temp+
    // 15 mov 逐字段拷贝）；各 err-return 岛位 4.4 jump-threading 差。
    bool hasLevelOver0x3b = false;                                // ORIG -0x15
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, (unsigned char)secuErr);
        return false;
    }

    if (sig.m_eventKind == 2)
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
    Inven_Item item;                 // ORIG `call Inven_ItemC1Ev` 默认构造
    item = inv->GetInvenSlot(1, (int)(unsigned short)sig.m_itemIndex);
    if (item.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
        return false;
    }
    if (sig.m_eventKind == 1)
    {
        // b28：ORIG 0x0819f25x 双臂 `cmp $K,%eax; je <err>`——命中不等即直落错误体
        // （无真岛）。4.4 正向 == 形产 jne+真值岛（+2 jmp）；改负判 `!=`：je 直落
        // err、真臂 fall-through 至统一尾 return true，与 ORIG 布局同构。
        if (item.m_addInfo != 0x28d468)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
            return false;
        }
    }
    else if (sig.m_eventKind == 2)
    {
        if (item.m_addInfo != 0x28d469)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
            return false;
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
        return false;
    }
    return true;
}
}  // namespace DISPATCHER
}  // namespace ARAD

