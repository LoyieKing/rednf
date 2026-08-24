// df_game_r Game/ EventClassify 事件实体第一组实现。
// 建模并实现以下 CEventEntity 派生类（ORIG 均为 0x20 字节：
//   基类 CEventEntity 0x1c + InGameEventScript* m_script1c(+0x1c)）。
//   - CLevelUpReward      ← 事件记录类型 0x194
//   - CUsedFatigueGiveItem← 事件记录类型 0x65
//   - CAttendance         ← 事件记录类型 0x56
//   - CPcRoomPlayTime     ← 事件记录类型 0x5c
//   - CAccountFirstLogin  ← 事件记录类型 0x191/0x192
//   - CAddItemCountDB     ← 事件记录类型 0x193
// create_event_entity（CEventScriptMng，0x810b402）按记录类型 new 各派生类
// （0x20 先清零再调 ctor）；每个 ctor 只调基类 ctor + 设 vptr。
//
// 依赖外部类（CEventActionMng::process_action_send_mail、DB_InsertAccountFirstLoginFlag）
// 当前由 FixEventScriptDeps / EventClassify 事件系统补全，本 TU 仅声明引用。

#include "CEventScriptMng.h"

#include "PacketGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CEnvironment.h"
#include "LogManager.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager（GetRepeatEvent）

#include <cstring>
#include <vector>

// 依赖类由事件系统提供（FixEventScriptDeps）：
#include "EventClassify_Action.h"       // EventClassify::Action_SendMail / CEventActionMng
#include "DB_InsertAccountFirstLoginFlag.h"

namespace EventClassify
{

// ============ CLevelUpReward ============

CLevelUpReward::CLevelUpReward()
    : CEventEntity(), m_script1c(0)
{
}

void CLevelUpReward::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810e168：call vtable+0x18（set_event_record_type(key)），再写 +0x1c。
    set_event_record_type(key);
    m_script1c = script;
}

char* CLevelUpReward::get_db_table_name()
{
    // ORIG 0x810e2d6：return m_script + 0xc。
    return reinterpret_cast<char*>(m_script1c) + 0xc;
}

char CLevelUpReward::is_recv_check_item()
{
    // ORIG 0x810e2f2：return *(char*)(m_script + 8)。
    return *(reinterpret_cast<char*>(m_script1c) + 8);
}

char* CLevelUpReward::get_mail_data()
{
    // ORIG 0x810e2e4：return m_script + 0x28（Action_SendMail）。
    return reinterpret_cast<char*>(m_script1c) + 0x28;
}

void CLevelUpReward::do_action(CUser* user)
{
    // ORIG 0x810e4ea：CEventActionMng::process_action_send_mail(mail=script+0x28, true)。
    if (m_script1c == 0)
        return;
    CEventActionMng mng;
    mng.process_action_send_mail(user, *reinterpret_cast<Action_SendMail*>(get_mail_data()), true);
}

// ============ CUsedFatigueGiveItem ============

CUsedFatigueGiveItem::CUsedFatigueGiveItem()
    : CEventEntity(), m_script1c(0)
{
}

bool CUsedFatigueGiveItem::is_eventing()
{
    // ORIG 0x811056e：基类 is_eventing && GetRepeatEvent(0x65) && IsEventing(0)。
    if (!CEventEntity::is_eventing())
        return false;
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x65);
    if (evt == 0)
        return false;
    return evt->IsEventing(0);
}

void CUsedFatigueGiveItem::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x81105be。
    set_event_record_type(key);
    m_script1c = script;
}

char* CUsedFatigueGiveItem::get_db_table_name()
{
    // ORIG 0x81105e8：return m_script + 8。
    return reinterpret_cast<char*>(m_script1c) + 8;
}

bool CUsedFatigueGiveItem::getIsCharacEvent(bool& out) const
{
    // ORIG 0x81107ea：m_script != 0 → out = *(bool*)(m_script+0x14)；返回 m_script!=0。
    if (m_script1c != 0)
        out = *(reinterpret_cast<bool*>(reinterpret_cast<char*>(m_script1c) + 0x14));
    return m_script1c != 0;
}

void CUsedFatigueGiveItem::sendEventItemFromFatigue(CUser* user, unsigned int fatigue)
{
    // ORIG 0x81105f6：按已用疲劳档位周期发放奖励邮件。
    if (m_script1c == 0 || user == 0)
        return;
    char* scr = reinterpret_cast<char*>(m_script1c);
    if (*(int*)(scr + 0x10) == 0)                 // 间隔档位为 0 → 不发
        return;
    int used = user->GetCurCharacUsedGiftFatigueQuantity();
    if (used == 0)
        return;
    unsigned int step = used - fatigue;
    // 当 (step) 越过档位边界时发放（ORIG 循环逻辑）。
    for (;; ++step)
    {
        unsigned int cur = (unsigned int)user->GetCurCharacUsedGiftFatigueQuantity();
        if (cur <= step)
            return;
        if (*(int*)(scr + 0x1c) >= 0 && *(unsigned int*)(scr + 0x1c) <= step)
            return;
        if (step % *(unsigned int*)(scr + 0x10) != 0)
            continue;
        CEventActionMng mng;
        if (mng.process_action_send_mail(user,
                *reinterpret_cast<Action_SendMail*>(scr + 0x20), true) == 0)
            continue;
        int reward = *(int*)(scr + 0x40);
        int characNo = user->getCurCharacNo();
        unsigned int accId = user->get_acc_id();
        LogManager::logFormat(1, "EventClassify.cpp",
            "void EventClassify::CUsedFatigueGiveItem::sendEventItemFromFatigue(CUser*, unsigned int)",
            0x83f,
            "CUsedFatigueGiveItem::sendEventItemFromFatigue(m_id:%d, charac_no:%d, reward:%d)",
            accId, characNo, reward);
        return;
    }
}

// ============ CAttendance ============

CAttendance::CAttendance()
    : CEventEntity(), m_script1c(0)
{
}

bool CAttendance::is_eventing()
{
    // ORIG 0x810e75c：GetRepeatEvent(0x56) && IsEventing(0)。
    if (!CEventEntity::is_eventing())
        return false;
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x56);
    if (evt == 0)
        return false;
    return evt->IsEventing(0);
}

void CAttendance::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810eadc。
    set_event_record_type(key);
    m_script1c = script;
}

bool CAttendance::send_init_data(CUser* user, PacketGuard& packet,
                                 short key, unsigned char count, bool flag)
{
    // ORIG 0x810e804：packet.put_short(key)；put_short(get_server_fatigue_count(count)
    //   + get_fatigue_count_db(count))；return 1。
    packet.put_short(key);
    short s = user->get_server_fatigue_count(count);
    s = (short)(s + user->get_fatigue_count_db(count));
    packet.put_short(s);
    return true;
}

char* CAttendance::get_db_table_name()
{
    // ORIG 0x810e7cc：return m_script + 8。
    return reinterpret_cast<char*>(m_script1c) + 8;
}

char* CAttendance::get_mail_data()
{
    // ORIG 0x810e7da：return m_script + 0x2c。
    return reinterpret_cast<char*>(m_script1c) + 0x2c;
}

int CAttendance::get_fatigue_condition()
{
    // ORIG 0x810e7e8：return *(int*)(m_script + 0xc)。
    return *(reinterpret_cast<int*>(reinterpret_cast<char*>(m_script1c) + 0xc));
}

int CAttendance::get_final_day_condition()
{
    // ORIG 0x810e7f6：return *(int*)(m_script + 0x24)。
    return *(reinterpret_cast<int*>(reinterpret_cast<char*>(m_script1c) + 0x24));
}

// ============ CPcRoomPlayTime ============

CPcRoomPlayTime::CPcRoomPlayTime()
    : CEventEntity(), m_script1c(0)
{
}

bool CPcRoomPlayTime::is_eventing()
{
    // ORIG 0x8110814：GetRepeatEvent(0x5c) && IsEventing(0)。
    if (!CEventEntity::is_eventing())
        return false;
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x5c);
    if (evt == 0)
        return false;
    return evt->IsEventing(0);
}

void CPcRoomPlayTime::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x8110872。
    set_event_record_type(key);
    m_script1c = script;
}

char* CPcRoomPlayTime::get_db_table_name()
{
    // ORIG 0x8110864：return m_script + 8。
    return reinterpret_cast<char*>(m_script1c) + 8;
}

int CPcRoomPlayTime::verifyUserPcRoomPlayEvent(CUser* user)
{
    // ORIG 0x8110a38：is_eventing()==1 → 若 premium(0xc) 且 次数>=档位 → 0x12 else 0；
    //   非 premium → 7；未开 → 0xb2。
    if (is_eventing())
    {
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        {
            unsigned char cnt = user->getPcRoomPlayTimeRewardCnt();
            if (cnt < *(unsigned char*)(reinterpret_cast<char*>(m_script1c) + 0x10))
                return 0;
            return 0x12;
        }
        return 7;
    }
    return 0xb2;
}

// ============ CAccountFirstLogin ============

CAccountFirstLogin::CAccountFirstLogin()
    : CEventEntity(), m_script1c(0)
{
}

void CAccountFirstLogin::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810deda。
    set_event_record_type(key);
    m_script1c = script;
}

char* CAccountFirstLogin::get_db_table_name()
{
    // ORIG 0x810debe：return m_script + 8。
    return reinterpret_cast<char*>(m_script1c) + 8;
}

char* CAccountFirstLogin::get_mail_data()
{
    // ORIG 0x810decc：return m_script + 0xc。
    return reinterpret_cast<char*>(m_script1c) + 0xc;
}

void CAccountFirstLogin::do_action(int flags, CUser* user, std::bitset<32u> /*bs*/)
{
    // ORIG 0x810df04：发首登奖励邮件并写 DB 首登标志。
    if (user == 0)
        return;
    if (m_script1c == 0)
        return;
    CEventActionMng mng;
    mng.process_action_send_mail(user, *reinterpret_cast<Action_SendMail*>(get_mail_data()), true);

    int serverGroup = 0;
    GameWorld* gw = G_GameWorld();
    if (gw->IsIntegratedPvPBaseChannel())
        serverGroup = user->GetServerGroup();
    else
        serverGroup = *(int*)((char*)G_CEnvironment() + 0x378);
    serverGroup &= 0xff;

    int characNo = user->getCurCharacNo();
    unsigned int accId = user->get_acc_id();
    unsigned int uid = user->GetUID();
    DB_InsertAccountFirstLoginFlag::makeRequest(uid, accId, serverGroup, characNo, flags);
}

// ============ CAddItemCountDB ============

CAddItemCountDB::CAddItemCountDB()
    : CEventEntity(), m_script1c(0)
{
}

void CAddItemCountDB::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810e064。
    set_event_record_type(key);
    m_script1c = script;
}

char* CAddItemCountDB::get_db_table_name()
{
    // ORIG 0x810e15a：return m_script + 0x20。
    return reinterpret_cast<char*>(m_script1c) + 0x20;
}

int CAddItemCountDB::is_event_item(int itemId)
{
    // ORIG 0x810e08e：遍历 vector<int>(m_script+8) 判 itemId。
    std::vector<int>* vec = reinterpret_cast<std::vector<int>*>(
        reinterpret_cast<char*>(m_script1c) + 8);
    for (std::vector<int>::iterator it = vec->begin(); it != vec->end(); ++it)
        if (*it == itemId)
            return 1;
    return 0;
}

int CAddItemCountDB::is_event_quest(int questId)
{
    // ORIG 0x810e0f4：遍历 vector<int>(m_script+0x14) 判 questId。
    std::vector<int>* vec = reinterpret_cast<std::vector<int>*>(
        reinterpret_cast<char*>(m_script1c) + 0x14);
    for (std::vector<int>::iterator it = vec->begin(); it != vec->end(); ++it)
        if (*it == questId)
            return 1;
    return 0;
}


// ============================================================================
// create_event_entity @ 0x0810b402（CEventScriptMng 实体工厂）。
// flag=true → 仅基类实体（operator new 0x1c + CEventEntity ctor，无清零循环）。
// 否则按记录类型分配 0x20 字节、先整块清零再调用对应派生类构造
// （ORIG 每分支均为 operator new(0x20) → 8×dword 清零 → 派生 C1 thunk）：
//   0x56→CAttendance        0x5c→CPcRoomPlayTime       0x65→CUsedFatigueGiveItem
//   0x191/0x192→CAccountFirstLogin    0x193→CAddItemCountDB   0x194→CLevelUpReward
//   0x196→CExchangeRandomItemReward   0x197→CAvatarDisjointRandomReward
//   0x199→CFatigueQuantity  0x19a→CPurchaseCashItemBonus
//   0x19b→CObjectBringUp    0x19c→CAccountFirstLoginDaily   其它→0
// ============================================================================
CEventEntity* CEventScriptMng::create_event_entity(int recordType, bool flag)
{
    CEventEntity* entity = 0;

    if (flag)
    {
        // ORIG 0810b41e..450：new 0x1c + 基类 ctor。
        entity = new CEventEntity();
        return entity;
    }


    // ORIG 每分支内联重复：operator new(0x20) → 8×dword 清零循环 → 派生 ctor
    // （value-init 语义；ORIG 无 memset 调用）。宏不产生新符号。
#define DNF_NEW_ZEROED_ENTITY(CTOR_EXPR)                                     \
    do                                                                       \
    {                                                                        \
        void* mem = ::operator new(0x20);                                    \
        unsigned int off = 0;                                                \
        do                                                                   \
        {                                                                    \
            *reinterpret_cast<unsigned int*>(                                \
                static_cast<char*>(mem) + off) = 0;                          \
            off += 4;                                                        \
        } while (off < 0x20);                                                \
        entity = new (mem) CTOR_EXPR;                                        \
    } while (0)

    switch (recordType)
    {
    case 0x56:                                      // ORIG 08110fdc
        DNF_NEW_ZEROED_ENTITY(CAttendance());
        break;
    case 0x5c:                                      // ORIG 08111084
        DNF_NEW_ZEROED_ENTITY(CPcRoomPlayTime());
        break;
    case 0x65:                                      // ORIG 081110a0
        DNF_NEW_ZEROED_ENTITY(CUsedFatigueGiveItem());
        break;
    case 0x191:                                     // ORIG 08110f88
    case 0x192:
        DNF_NEW_ZEROED_ENTITY(CAccountFirstLogin());
        break;
    case 0x193:                                     // ORIG 08110fa4
        DNF_NEW_ZEROED_ENTITY(CAddItemCountDB());
        break;
    case 0x194:                                     // ORIG 08110fc0
        DNF_NEW_ZEROED_ENTITY(CLevelUpReward());
        break;
    case 0x196:                                     // ORIG 08110ff8
        DNF_NEW_ZEROED_ENTITY(CExchangeRandomItemReward());
        break;
    case 0x197:                                     // ORIG 08111014
        DNF_NEW_ZEROED_ENTITY(CAvatarDisjointRandomReward());
        break;
    case 0x199:                                     // ORIG 08111030
        DNF_NEW_ZEROED_ENTITY(CFatigueQuantity());
        break;
    case 0x19a:                                     // ORIG 0811104c
        DNF_NEW_ZEROED_ENTITY(CPurchaseCashItemBonus());
        break;
    case 0x19b:                                     // ORIG 08111068
        DNF_NEW_ZEROED_ENTITY(CObjectBringUp());
        break;
    case 0x19c:                                     // ORIG 0811110a
        DNF_NEW_ZEROED_ENTITY(CAccountFirstLoginDaily());
        break;
    default:                                        // ORIG 未匹配返回 0（无分配）
        return 0;
    }
#undef DNF_NEW_ZEROED_ENTITY
    return entity;
}
}  // namespace EventClassify
