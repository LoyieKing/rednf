// df_game_r Game/ EventClassify 事件实体第一组实现。
// 建模并实现以下 CEventEntity 派生类（ORIG 均为 0x20 字节：
//   基类 CEventEntity 0x1c + InGameEventScript* m_script1c(+0x1c)）。
//   - CLevelUpReward      ← 事件记录类型 0x194
//   - CUsedFatigueGiveItem← 事件记录类型 0x65
//   - CAttendance         ← 事件记录类型 0x56
//   - CPcRoomPlayTime     ← 事件记录类型 0x5c
//   - CAccountFirstLogin  ← 事件记录类型 0x191/0x192
//   - CAddItemCountDB     ← 事件记录类型 0x193
//
// 脚本绑定：m_script1c 指向的具体脚本类型由记录类型唯一确定（ORIG nm W 符号：
// 六个派生类 ctor 均为隐式生成，仅调基类 C1 + 设 vptr；对象由 create_event_entity
// value-init：operator new(0x20) → 整块清零 → C1 thunk）。除 CAccountFirstLogin
// （entitiesB 的派生类需显式调用基类 ctor，故保留声明并补 m_script1c=0 对齐工厂
// 清零终态）外均不声明 ctor，由编译器同样产生清零循环 + 弱 C1。
//
// 脚本访问一律经 InGameEventScripts.h 权威布局的具名字段（禁止裸偏移）：
//   0x194→LevelUpRewardScript        0x65→UsedFatigueGiveItemScript
//   0x56→AttendanceScript            0x5c→PcBangPlayTimeScript
//   0x191/0x192→AccountFirstLoginScript    0x193→AddItemCountDBScript

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

#include "InGameEventScripts.h"         // 具体事件脚本类型（权威布局）
// 依赖类由事件系统提供（FixEventScriptDeps）：
#include "EventClassify_Action.h"       // EventClassify::Action_SendMail / CEventActionMng
#include "DB_InsertAccountFirstLoginFlag.h"
#include "GameTypes.h"                  // cMyTrace

// ORIG T 0x0889232e（定义 BingoEventLink4.cpp）：
bool isSetBit_Uint(unsigned int value, int bit);
// ORIG T 0x0810904b（声明 CInventory.h/SkillSlot.h）：
char* NumberToString(unsigned int value, int radix);

namespace EventClassify
{

// process_action_send_mail 形参用的是 EventClassify_Action.h 的命名空间内
// Action_SendMail；脚本子对象 m_mail 为全局 ::Action_SendMail（ORIG 符号口径，
// 见 InGameEventScripts.h）。两者逐字段同布局（sizeof 均 0x44）。用宏桥接：
// -O0 下 inline 函数会产生多余调用帧（ORIG 无）。
typedef char dnf_mail_layout_assert_[
    (sizeof(::Action_SendMail) == sizeof(Action_SendMail)) ? 1 : -1];
#define EVENT_MAIL_REF(mail) (*reinterpret_cast<Action_SendMail*>(&(mail)))

// ============ CLevelUpReward（0x194 → LevelUpRewardScript）============
// ORIG T 0x810e4ea：以绑定脚本 m_mail（+0x28）为 Action_SendMail&，
// flag=true 走 process_action_send_mail 发奖。
void CLevelUpReward::do_action(CUser* user)
{
    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<LevelUpRewardScript*>(m_script1c)->m_mail),
        true);
}
 
 void CLevelUpReward::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810e168：call vtable+0x18（set_event_record_type(key)），再写 +0x1c。
    set_event_record_type(key);
    m_script1c = script;
}

std::string& CLevelUpReward::get_db_table_name()
{
    // ORIG 0x810e2d6：return &m_dbTableName（调用方按 std::string& 解释）。
    return static_cast<LevelUpRewardScript*>(m_script1c)->m_dbTableName;
}

char CLevelUpReward::is_recv_check_item()
{
    // ORIG 0x810e2f2：return m_flag08。
    return static_cast<char>(
        static_cast<LevelUpRewardScript*>(m_script1c)->m_flag08);
}

char* CLevelUpReward::get_mail_data()
{
    // ORIG 0x810e2e4：return &m_mail。
    return reinterpret_cast<char*>(
        &static_cast<LevelUpRewardScript*>(m_script1c)->m_mail);
}

// ORIG 0x810e51a：不缓存脚本/items/item/mail 局部——条件与循环每次内联
// 解引用 it 与 m_script1c；循环计数为 signed int（setg 比较）。
int CLevelUpReward::send_reward_from_recv_level(CUser* user, int level,
                                                int growType)
{
    int job = user->get_charac_job();
    int curGrowType = static_cast<int>(user->getCurCharFirstGrowType());

    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it == static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end())
        return 1;

    if (it->second.m_filter[1] != 0xff &&
        static_cast<int>(static_cast<signed char>(it->second.m_filter[1])) != curGrowType)
        return 1;

    for (int i = 0; i < (int)it->second.m_items.size(); ++i)
    {
        if (growType < it->second.m_items[i].m_level &&
            it->second.m_items[i].m_level <= level)
        {
            static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.reset();
            static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.set_item(
                it->second.m_items[i].m_itemIndex,
                it->second.m_items[i].m_itemCount);
            static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field30 =
                it->second.m_items[i].m_title;
            static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field34 =
                it->second.m_items[i].m_content;
            do_action(user);
        }
    }
    return 1;
}
// ORIG T 0x811056e（vtable slot 2）：基类 is_eventing 且
// GetRepeatEvent(0x65) 非空且其 IsEventing(0) 为真才返回 true。
bool CUsedFatigueGiveItem::is_eventing()
{
    if (CEventEntity::is_eventing())
    {
        CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x65);
        if (evt->IsEventing(0))
            return true;
    }
    return false;
}

// ORIG T 0x08110756：脚本 job 过滤（-1 通配），随后按 [user type] 分派：
//   0=全适用；1=仅 premium（ENUM_PREMIUM_TYPE 0xc）；其它=不适用。
int CUsedFatigueGiveItem::verifyApplyUser(CUser* user)
{
    UsedFatigueGiveItemScript* script =
        static_cast<UsedFatigueGiveItemScript*>(m_script1c);
    int job = script->m_jobType;
    if (job >= 0 && job != static_cast<int>(user->get_charac_job()))
        return 0;

    switch (script->m_userType)
    {
        case 0:
            return 1;
        case 1:
            return user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 1 : 0;
        default:
            return 0;
    }
}

std::string& CUsedFatigueGiveItem::get_db_table_name()
{
    // ORIG 0x81105e8：return &m_dbTableName。
    return static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_dbTableName;
}

void CUsedFatigueGiveItem::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x81105be。
    set_event_record_type(key);
    m_script1c = script;
}

bool CUsedFatigueGiveItem::getIsCharacEvent(bool& out) const
{
    // ORIG 0x81107ea：m_script == 0 直接返回 false；否则按 bool 读
    // m_charcEventFlag（+0x14 字节）后返回 true。
    if (m_script1c == 0)
        return false;
    *(unsigned char*)&out =
        static_cast<const UsedFatigueGiveItemScript*>(m_script1c)->m_charcEventFlag;
    return true;
}

void CUsedFatigueGiveItem::sendEventItemFromFatigue(CUser* user, unsigned int fatigue)
{
    // ORIG 0x81105f6：按已用疲劳档位周期发放奖励邮件。
    // 档位间隔 m_usedFatigue、上限 m_fatigueLimit（<0 不限）、邮件 m_mail；
    // 循环先判当前已用疲劳/上限，再 ++step 后按档位取模并发信（ORIG do-while），
    // 发信成功读 m_mail.m_itemIdx 记日志收尾。全程不缓存脚本指针（与 ORIG 同）。
    if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_usedFatigue == 0)
        return;
    if (user->GetCurCharacUsedGiftFatigueQuantity() == 0)
        return;
    unsigned int step =
        (unsigned int)(user->GetCurCharacUsedGiftFatigueQuantity() - fatigue);
    // 入口护栏：m_fatigueLimit >= 0 且 step >= 上限 → 一档都不发。
    if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit >= 0 &&
        (unsigned int)static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit <= step)
        return;

    CEventActionMng mng;
    while ((unsigned int)user->GetCurCharacUsedGiftFatigueQuantity() > step)
    {
        if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit >= 0 &&
            (unsigned int)static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit <= step)
            return;
        ++step;                                   // 先自增再判档位/发信（ORIG do-while）
        if (step % (unsigned int)static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_usedFatigue != 0u)
            continue;
        int sent = mng.process_action_send_mail(
            user,
            EVENT_MAIL_REF(static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_mail),
            true);
        if (sent == 0)
            continue;
        LogManager::logFormat(1, "EventClassify.cpp",
            __PRETTY_FUNCTION__,
            0x83f,
            "CUsedFatigueGiveItem::sendEventItemFromFatigue(m_id:%d, charac_no:%d, reward:%d)",
            user->get_acc_id(), user->getCurCharacNo(),
            static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_mail.m_itemIdx);
    }
}

// ============ CAttendance（0x56 → AttendanceScript）============

bool CAttendance::is_eventing()
{
    // ORIG 0x810e75c：GetRepeatEvent(0x56) 判空后经 vptr+0x34 判 IsEventing(0)，
    // 结果与判空合成一个 bool 条件（双重归一化形态）。
    if (CEventEntity::is_eventing())
    {
        CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x56);
        if (evt != 0 && evt->IsEventing(0))
            return true;
    }
    return false;
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
    // 两计数以单表达式按 int 相加（前者经寄存器缓存）后整体传入；return 1。
    packet.put_short(key);
    packet.put_short(
        user->get_server_fatigue_count(count) +
        user->get_fatigue_count_db(count));
    return true;
}

std::string& CAttendance::get_db_table_name()
{
    // ORIG 0x810e7cc：return &m_dbTableName。
    return static_cast<AttendanceScript*>(m_script1c)->m_dbTableName;
}

char* CAttendance::get_mail_data()
{
    // ORIG 0x810e7da：return &m_mail。
    return reinterpret_cast<char*>(
        &static_cast<AttendanceScript*>(m_script1c)->m_mail);
}

int CAttendance::get_fatigue_condition()
{
    // ORIG 0x810e7e8：return m_field0c。
    return static_cast<AttendanceScript*>(m_script1c)->m_field0c;
}

int CAttendance::get_final_day_condition()
{
    // ORIG 0x810e7f6：return m_field24。
    return static_cast<AttendanceScript*>(m_script1c)->m_field24;
}

// ORIG 0x810e878：day 越界（<0 或 ≥ m_attends.size()）直接返回；命中则把
// m_attends[day] 写入 mail 的 itemIdx/addInfo，正文 key 取 m_str70 后发奖邮件。
void CAttendance::send_attend_reward_item(CUser* user, int day)
{
    // 判断段 ORIG 为三个独立 if+return（跳转目标各异）：size()==0 判空、
    // day<0、signed size<=day，每次内联解引用脚本（不缓存）；m_attends[day]
    // 两次独立取用。
    if (static_cast<AttendanceScript*>(m_script1c)->m_attends.size() == 0)
        return;
    if (day < 0)
        return;
    if ((int)static_cast<AttendanceScript*>(m_script1c)->m_attends.size() <= day)
        return;

    char* mid = NumberToString(user->get_acc_id(), 0);
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x5ad, 0);
    trace("send_attend_reward_item m_id:%s day:%d", mid, day);

    AttendanceScript* script = static_cast<AttendanceScript*>(m_script1c);
    script->m_mail.m_itemIdx = script->m_attends[day].first;
    script->m_mail.m_addInfo = script->m_attends[day].second;
    script->m_mail.m_field34 = script->m_str70;

    CEventActionMng mng;
    mng.process_action_send_mail(user, EVENT_MAIL_REF(script->m_mail), true);
}

// ORIG 0x810e9c0：统计 attendFlags 置位数（bit 0..31），等于 final 记录数才发：
// mail 写入 m_pair1c（全勤终奖），正文 key 取 m_str74。
void CAttendance::send_attend_final_rward_item(CUser* user, int attendFlags,
                                               int record)
{
    char* mid = NumberToString(user->get_acc_id(), 0);
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x5ba, 0);
    trace("send_attend_final_rward_item m_id:%s day:%d record:%u",
          mid, record, static_cast<unsigned int>(attendFlags));

    int setBits = 0;
    for (int bit = 0; bit < 0x20; ++bit)
    {
        if (isSetBit_Uint(static_cast<unsigned int>(attendFlags), bit))
            ++setBits;
    }
    if (record != setBits)
    {
        return;
    }

    // ORIG 发奖段每次内联解引用脚本（不缓存局部）。
    static_cast<AttendanceScript*>(m_script1c)->m_mail.m_itemIdx =
        static_cast<AttendanceScript*>(m_script1c)->m_pair1c.first;
    static_cast<AttendanceScript*>(m_script1c)->m_mail.m_addInfo =
        static_cast<AttendanceScript*>(m_script1c)->m_pair1c.second;
    static_cast<AttendanceScript*>(m_script1c)->m_mail.m_field34 =
        static_cast<AttendanceScript*>(m_script1c)->m_str74;

    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<AttendanceScript*>(m_script1c)->m_mail),
        true);
}

// ============ CPcRoomPlayTime（0x5c → PcBangPlayTimeScript）============

bool CPcRoomPlayTime::is_eventing()
{
    // ORIG 0x8110814：GetRepeatEvent(0x5c) 直接解引用判 IsEventing(0)
    // （ORIG 无空检）。
    if (CEventEntity::is_eventing())
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x5c)->IsEventing(0))
            return true;
    }
    return false;
}

void CPcRoomPlayTime::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x8110872。
    set_event_record_type(key);
    m_script1c = script;
}

std::string& CPcRoomPlayTime::get_db_table_name()
{
    // ORIG 0x8110864：return &m_dbTableName。
    return static_cast<PcBangPlayTimeScript*>(m_script1c)->m_dbTableName;
}

int CPcRoomPlayTime::verifyUserPcRoomPlayEvent(CUser* user)
{
    // ORIG 0x8110a38：未开事件 → 0xb2；非 0xc 类 premium → 7；
    // 领奖次数达到 m_numberOfTime（先判 >=）→ 0x12，否则 0。
    if (!is_eventing())
        return 0xb2;
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        return 7;
    if ((unsigned int)user->getPcRoomPlayTimeRewardCnt() >=
        (unsigned int)static_cast<PcBangPlayTimeScript*>(m_script1c)->m_numberOfTime)
        return 0x12;
    return 0;
}

// ============ CAccountFirstLogin（0x191/0x192 → AccountFirstLoginScript）============

CAccountFirstLogin::CAccountFirstLogin()
    : CEventEntity()
{
    // Fix6：ORIG 0x810e168 隐式 ctor 不清 +0x1c（工厂 new 后整块清零），
    // m_script1c 保持垃圾值以字节一致。
}

void CAccountFirstLogin::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810deda。
    set_event_record_type(key);
    m_script1c = script;
}

std::string& CAccountFirstLogin::get_db_table_name()
{
    // ORIG 0x810debe：return &m_dbTableName。
    return static_cast<AccountFirstLoginScript*>(m_script1c)->m_dbTableName;
}

char* CAccountFirstLogin::get_mail_data()
{
    // ORIG 0x810decc：return &m_mail。
    return reinterpret_cast<char*>(
        &static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail);
}

void CAccountFirstLogin::do_action(int flags, CUser* user, std::bitset<32u> /*bs*/)
{
    // ORIG 0x810df04：发首登奖励邮件并请求 DB 落首登标记（仅判 user 空指针）。
    // 服务器组：PvP 统合频道取 GetServerGroup 低字节，否则取 CEnvironment +0x378
    // 低字节（共享头暂无命名字段，用组件视图复现 ORIG 单指令 disp 加载）。
    if (user == 0)
        return;
    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail),
        true);

    struct CEnvironment378View
    {
        char         m_pad378[0x378];
        unsigned int m_serverGroup378;   // +0x378
    };
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = (unsigned char)user->GetServerGroup();
    else
        serverGroup = (unsigned char)
            ((CEnvironment378View*)G_CEnvironment())->m_serverGroup378;

    DB_InsertAccountFirstLoginFlag::makeRequest(
        (int)user->GetUID(), user->get_acc_id(), serverGroup,
        (unsigned int)user->getCurCharacNo(), flags);
}

// ============ CAddItemCountDB（0x193 → AddItemCountDBScript）============

void CAddItemCountDB::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810e064。
    set_event_record_type(key);
    m_script1c = script;
}

std::string& CAddItemCountDB::get_db_table_name()
{
    // ORIG 0x810e15a：return &m_dbTableName。
    return static_cast<AddItemCountDBScript*>(m_script1c)->m_dbTableName;
}

bool CAddItemCountDB::is_event_item(int itemId)
{
    // ORIG 0x810e08e：size() 后按下标遍历 m_itemIdxs（每次内联解引用脚本）。
    int count = static_cast<AddItemCountDBScript*>(m_script1c)->m_itemIdxs.size();
    for (int i = 0; i < count; ++i)
    {
        if (static_cast<AddItemCountDBScript*>(m_script1c)->m_itemIdxs[i] == itemId)
            return 1;
    }
    return 0;
}

bool CAddItemCountDB::is_event_quest(int questId)
{
    // ORIG 0x810e0f4：size() 后按下标遍历 m_questIds（每次内联解引用脚本）。
    int count = static_cast<AddItemCountDBScript*>(m_script1c)->m_questIds.size();
    for (int i = 0; i < count; ++i)
    {
        if (static_cast<AddItemCountDBScript*>(m_script1c)->m_questIds[i] == questId)
            return 1;
    }
    return 0;
}

// ============================================================================
// create_event_entity @ 0x0810b402（CEventScriptMng 实体工厂）。
// flag=true → 仅基类实体（operator new 0x1c + CEventEntity ctor，无清零）。
// 否则按记录类型 value-init 对应派生类：operator new(0x20) → 整块清零循环 →
// C1 thunk（隐式 ctor 下 gcc 与 ORIG 同形；组 B/CAccountFirstLogin 为显式
// ctor，成员终态一致）。
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

    // 分支体按 ORIG 代码布局顺序排列（ctor 地址升序，gcc -O0 同序生成）。
    switch (recordType)
    {
    case 0x191:                                     // ORIG 08110f88
    case 0x192:
        entity = new CAccountFirstLogin();
        break;
    case 0x193:                                     // ORIG 08110fa4
        entity = new CAddItemCountDB();
        break;
    case 0x194:                                     // ORIG 08110fc0
        entity = new CLevelUpReward();
        break;
    case 0x56:                                      // ORIG 081110fdc
        entity = new CAttendance();
        break;
    case 0x196:                                     // ORIG 08110ff8
        entity = new CExchangeRandomItemReward();
        break;
    case 0x197:                                     // ORIG 08111014
        entity = new CAvatarDisjointRandomReward();
        break;
    case 0x199:                                     // ORIG 08111030
        entity = new CFatigueQuantity();
        break;
    case 0x19a:                                     // ORIG 0811104c
        entity = new CPurchaseCashItemBonus();
        break;
    case 0x19b:                                     // ORIG 08111068
        entity = new CObjectBringUp();
        break;
    case 0x5c:                                      // ORIG 08111084
        entity = new CPcRoomPlayTime();
        break;
    case 0x65:                                      // ORIG 081110a0
        entity = new CUsedFatigueGiveItem();
        break;
    case 0x19c:                                     // ORIG 0811110a
        entity = new CAccountFirstLoginDaily();
        break;
    default:                                        // ORIG 未匹配返回 0（无分配）
        break;
    }
    return entity;
}
}  // namespace EventClassify
