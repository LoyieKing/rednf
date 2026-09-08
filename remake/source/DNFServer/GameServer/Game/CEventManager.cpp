// df_game_r Game/ CEventManager 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "GameWorld.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "GlobalData.h"
#include "CGlobalEffectManager.h"
#include "CEnvironment.h"
#include "InterfacePacketBuf.h"
#include "CGuildServerProxy.h"

#include <cstring>
#include <cstdio>

#include <new>
#include <algorithm>
#include <map>
#include <vector>
#include <boost/foreach.hpp>           // ORIG attach/notify 用 BOOST_FOREACH 遍历 list
#include "RDARScriptStringManager.h"   // g_scriptStringManager_/findString
#include "CEventTable.h"              // CEventTable 权威声明（唯一）
#include "PacketGuard.h"
// InitEventManager 事件类声明（ORIG InitEventManager 0x08114dcc 槽位映射实证）。
#include "CUnlimitFatigueEvent.h"
#include "CMaxFatigueFactorEvent.h"
#include "CExpDoubleEvent.h"
#include "CCoinEventPerDay.h"
#include "ARAD_Database.h"           // CCoinEventOnCharCreate
#include "CLeadingChannelEvent.h"
#include "CItemDropRatioEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CGuildWarEvent.h"
#include "CPCRoomFatigueEvent.h"
#include "CReformingDanjinEvent.h"
#include "CCoinRefillEvent.h"
#include "CBurningFatigueEvent.h"
#include "CClearRewardCardEvent.h"
#include "CCeraShopBonusItemEvent.h"
#include "CTournamentPvPEvent.h"
#include "CGoldCardBlankItemEvent.h"
#include "CPcRoomCardBlankItemEvent.h"
#include "CPowerWarEvent.h"
#include "CCollectArchieveEventLog.h"
#include "CPCRoomWorldDropEvent.h"
#include "CPartyExpBonusEvent.h"
#include "CStabToDeathEvent.h"
#include "CSchoolMatchEvent.h"
#include "W9_EventSmall.h"           // CAutoMarketConditionsControlEvent / CWeekendBonusEvent
#include "CVendingMachineBonusEvent.h"
#include "CBurnigGoldMonsterEvent.h"
#include "CNoNeedGoldOnGuildCreateEvent.h"
#include "CDeathTowerWinPointEvent.h"
#include "CNotApplyBalkeunEvent.h"
#include "CCharacterDayEvent.h"
#include "CRestrictCharacCreationEvent.h"
#include "CReduceUpgradeItemPay.h"
#include "COnTimeEvent.h"
#include "CBreakAwayPreventEvent.h"
#include "CPowerWarVictoriousEvent.h"
#include "CSecretShopEvent.h"
#include "CFatigueAttendance.h"
#include "CConditionEvent.h"
#include "CUXGameLogEvent.h"
#include "BlueMarbleDungeonEvent.h"
#include "CStopOverlabExpEvent.h"
#include "CEventAdvanceAltarOpen.h"
#include "CAradRyosikaEvent.h"
#include "Arad_MomijiEvent.h"
#include "LevelupSupportEvent.h"
#include "CEventStayTime.h"
#include "CEventCreateDnf.h"
#include "EventGiveMeBox.h"

CDelivery::CDelivery()
    : m_eventMap()
{
}

CDelivery::~CDelivery()
{
    m_eventMap.clear();
}

bool CDelivery::checkAddress(int type)
{
    return type >= 1 && type <= 2;
}

// b28d-40: BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹（P220）；ORIG 0x081163fa，AE。
void CDelivery::attach(int type, CEventBase* event)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
    {
        BOOST_FOREACH(CEventBase*& e, it->second)
        {
            if (e == event)
                return;
        }
    }
    m_eventMap[type].push_back(event);
}

void CDelivery::detach(int type, CEventBase* event)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
        it->second.remove(event);
}

// b28d-40: BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹（P220）；ORIG 0x08116682，AE。
// checkAddress 校验 → map.find → 遍历该 type 的 list，对非空 CEventBase 经
// vtable+0x30（= delivery 虚槽 12）调用 delivery(msg)。
void CDelivery::notify(int type, DeliveryMsg& msg)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
    {
        BOOST_FOREACH(CEventBase*& e, it->second)
        {
            if (e != 0)
                e->delivery(msg);
        }
    }
}



CEventManager::CEventManager()
{
    std::memset(m_events, 0, sizeof(m_events));
    // ORIG 0x08114ce4：memset 0x298，operator new(0x18) 后调 CDelivery::CDelivery()
    // （0x08116386），存 CDelivery* 于 +0x298。此处用真实 CDelivery 构造。
    // [b24] 直接 `m_delivery = new CDelivery`（ORIG 寄存器直传 %ebx→+0x298，
    // 无具名局部栈槽 -0x1c 中转；具名局部在 GCC11 生成 -0x1c(%ebp) 槽，A 类 1 insn 差）。
    m_delivery = new CDelivery;
}



CEventManager::~CEventManager()
{
    // b28d-40: N7/N82 封顶——遍历槽 -0xc/-0x10 平移 + 虚调用寄存器轮转，语义闭合。
    // ORIG 0x08114d4e。
    for (int i = 0; i <= 0xa5; ++i)
    {
        CEventBase* event = m_events[i];
        if (event != 0)
            delete event;
    }
    CDelivery* delivery = m_delivery;
    if (delivery != 0)
    {
        delete delivery;
    }
}

int CEventManager::AddEvent(int type, CEventBase* event)
{
    m_events[type] = event;
    return 1;
}

CEventBase* CEventManager::GetRepeatEvent(int type)
{
    if (type > 0xa5)
        return 0;
    return m_events[type];
}

// ORIG T 0x08116212 _ZN13CEventManager30IsExistOverlappedExpBonusEventEv
// 事件 0x66（经验加成类）进行中，或全局效果管理器效果 2 生效 → 视为存在
// 重叠经验加成。
bool CEventManager::IsExistOverlappedExpBonusEvent()
{
    bool eventExp = GlobalData::s_event_manager->GetRepeatEvent(0x66)->IsEventing(0);
    bool effect2 = GlobalData::s_globalEffectManager_->isAffectedEffect(2);
    if (eventExp || effect2)
        return true;
    return false;
}

// b28d-40: N7 封顶——ORIG 虚调用目标 %ecx + 索引 %edx/%eax vs 4.4.7 目标恒 %eax
// + 索引 %ecx/%edx（助记符序列全等，仅寄存器角色轮转）。ORIG 0x0811630e。
void CEventManager::dailyresetData()
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
        {
            if (m_events[i]->IsEventing(0))
                m_events[i]->dailyresetData();
        }
    }
}

int filterEvent(int type)
{
    // b28d-40: N82 封顶——帧 0x14 vs 0x24 + 实参槽中转（getChannelType 结果具名槽），
    // 语义闭合。ORIG 0x08115bf4。
    if (type == 2)
        return 0;
    GameWorld* world = G_GameWorld();
    unsigned int channel = (unsigned int)world->GetChannelType();
    if (channel >= 0x11)
        return 0;
    unsigned int bit = 1U << channel;
    if ((bit & 0x1a100U) == 0)
    {
        if ((bit & 0x4000U) == 0)
            return ((bit & 0x600U) != 0 && type != 9) ? 1 : 0;
        if (type == 0x39) return 0;
        if (type < 0x3a)
            return type == 0xb ? 0 : 1;
        return (type - 0x3dU <= 1) ? 0 : 1;
    }
    if (type < 0x3a)
        return (type < 0x36 && type != 0xb) ? 1 : 0;
    if (type < 0x3d)
        return 1;
    return (type > 0x3e && type != 0x5c) ? 1 : 0;
}
struct EventAddInfo
{
    unsigned short eventId;
    unsigned short info0;
    unsigned short info1;
};

void CEventManager::MakeNotiEventInfo(PacketGuard& packet, CUser* user)
{
    // b28d-40: N82 封顶——帧/槽平移 + 中间对象槽中转，语义闭合。ORIG 0x08115de2。
    std::vector<EventAddInfo> extra;
    packet.clear();
    packet.put_header(0, 0x6c);
    int countIndex = packet.get_index();
    int activeCount = 0;
    packet.put_short(0);
    for (int id = 0; id <= 0xa5; ++id)
    {
        CEventBase* event = GetRepeatEvent(id);
        if (event == 0 || !event->IsEventing(user))
            continue;
        packet.put_short(id);
        ++activeCount;
        // GetAddInfo 返回 stADDINFO（ORIG sret ABI，2026-08-28 OrigEventLeaf8f 修正）。
        stADDINFO add = event->GetAddInfo();
        unsigned short info0 = add.m_info0;
        if ((short)info0 >= 0)
        {
            EventAddInfo item;
            item.eventId = (unsigned short)id;
            item.info0 = info0;
            item.info1 = add.m_info1;
            extra.push_back(item);
        }
    }
    packet.put_short(countIndex, activeCount);
    for (int id = 0; id <= 0xa5; ++id)
    {
        CEventBase* event = GetRepeatEvent(id);
        if (event != 0 && event->IsEventing(user))
        {
            event->AppendInfo(packet);
        }
    }
    packet.put_byte((int)extra.size());
    for (size_t i = 0; i < extra.size(); ++i)
    {
        packet.put_short(extra[i].eventId);
        packet.put_short(extra[i].info0);
        packet.put_short(extra[i].info1);
    }
    packet.finalize(true);
}

void CEventManager::BroadcastEventInfo()
{
    // b28d-40: N82 封顶——帧/槽平移（对象槽 -0x10/-0x18 vs -0x20/-0x24），语义闭合。
    // ORIG 0x08116108。
    GameWorld* world = G_GameWorld();
    if (world == 0)
        return;
    for (std::map<unsigned short, CUser*>::iterator it =
             world->m_UserInWorld.begin(); it != world->m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0 && user->get_state() > 2)
        {
            PacketGuard packet;
            MakeNotiEventInfo(packet, user);
            user->Send(packet);
        }
    }
}

bool CEventManager::TriggerEventStart(int idx, Word_Param param)
{
    // b28d-40: N82 封顶——帧/槽平移 + bool 物化段，语义闭合。ORIG 0x08115cc6。
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event == 0)
        return 0;
    event->StartEvent(param);
    BroadcastEventInfo();
    event->deliveryStart(m_delivery);
    event->StartAction();
    return 1;
}

bool CEventManager::TriggerEventEnd(int idx)
{
    // b28d-40: N82 封顶——帧/槽平移 + setne 物化段，语义闭合。ORIG 0x08115d60。
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event == 0)
        return 0;
    event->deliveryEnd(m_delivery);
    BroadcastEventInfo();
    event->EndEvent();
    return 1;
}

// ORIG 0x08114dcc（InitEventManager.md 反汇编逐槽核对）：按槽位映射堆构造 50
// 个事件对象（new + C1 + 写槽），成功后调 LoadEventFromDB() 并透传其返回值；
// 任何构造异常经 catch 直接返回 0（ORIG catch @0x08115965 只 begin/end_catch，
// 不回收已构造对象）。GLOBAL 键控槽（21/24/35/36/39/50/58/86/87/91/102/119/
// 158）在 ORIG 为静态单例 getter 返回指针，析构同样 delete——语义等价堆构造，
// 统一 new。槽位 = 偏移/4，映射如下：
//   1 CUnlimitFatigueEvent 2 CMaxFatigueFactorEvent 3 CExpDoubleEvent
//   4 CCoinEventPerDay 5 CCoinEventOnCharCreate 6 CLeadingChannelEvent
//   7 CItemDropRatioEvent 8 CPCRoomBurningEvent 33 CGuildWarEvent
//   10 CPCRoomFatigueEvent 12 CReformingDanjinEvent 13 CCoinRefillEvent
//   15 CBurningFatigueEvent 16 CClearRewardCardEvent 17 CCeraShopBonusItemEvent
//   18 CTournamentPvPEvent 19 CGoldCardBlankItemEvent 29 CPcRoomCardBlankItemEvent
//   30 CPowerWarEvent 21 CCollectArchieveEventLog 22 CPCRoomWorldDropEvent
//   24 CPartyExpBonusEvent 32 CStabToDeathEvent 9 CSchoolMatchEvent
//   34 CAutoMarketConditionsControlEvent 35 CVendingMachineBonusEvent
//   36 CBurnigGoldMonsterEvent 38 CNoNeedGoldOnGuildCreateEvent
//   39 CDeathTowerWinPointEvent 40 CNotApplyBalkeunEvent 41 CCharacterDayEvent
//   49 CRestrictCharacCreationEvent 50 CReduceUpgradeItemPay 51 COnTimeEvent
//   52 CBreakAwayPreventEvent 53 CPowerWarVictoriousEvent 58 CSecretShopEvent
//   86 CFatigueAttendance 87 CWeekendBonusEvent 113 CConditionEvent
//   91 CUXGameLogEvent 95 BlueMarbleDungeonEvent 102 CStopOverlabExpEvent
//   119 CEventAdvanceAltarOpen 116 CAradRyosikaEvent 155 Arad_MomijiEvent
//   158 LevelupSupportEvent 159 CEventStayTime 160 CEventCreateDnf
//   165 EventGiveMeBox
// §6.2 巨件核销：P167 153/153 [0缺0多]；gap 927→927；残差归因 N 类
// （4.1 bool 取反物化 xor$1/test/je+块序 fall-else 在前 vs 4.4 test%al+jne，
// 详见 docs/asm_translation_patterns.md 负例表；try/catch joiner 与槽位一致）。
bool CEventManager::InitEventManager()
{
    // b28d-40: N82/N80 封顶——927=927 指令，残差为槽号平移与 cMyTrace 变参保活顺序
    // （P227 全表达式链已试）；ORIG 0x08114dcc。
    try
    {
        m_events[1]  = new CUnlimitFatigueEvent;
        m_events[2]  = new CMaxFatigueFactorEvent;
        m_events[3]  = new CExpDoubleEvent;
        m_events[4]  = new CCoinEventPerDay;
        m_events[5]  = new CCoinEventOnCharCreate;
        m_events[6]  = new CLeadingChannelEvent;
        m_events[7]  = new CItemDropRatioEvent;
        m_events[8]  = new CPCRoomBurningEvent;
        m_events[33] = new CGuildWarEvent;
        m_events[10] = new CPCRoomFatigueEvent;
        m_events[12] = new CReformingDanjinEvent;
        m_events[13] = new CCoinRefillEvent;
        m_events[15] = new CBurningFatigueEvent;
        m_events[16] = new CClearRewardCardEvent;
        m_events[17] = new CCeraShopBonusItemEvent;
        m_events[18] = new CTournamentPvPEvent;
        m_events[19] = new CGoldCardBlankItemEvent;
        m_events[29] = new CPcRoomCardBlankItemEvent;
        m_events[30] = new CPowerWarEvent;
        m_events[21] = new CCollectArchieveEventLog;
        m_events[22] = new CPCRoomWorldDropEvent;
        m_events[24] = new CPartyExpBonusEvent;
        m_events[32] = new CStabToDeathEvent;
        m_events[9]  = new CSchoolMatchEvent;
        m_events[34] = new CAutoMarketConditionsControlEvent;
        m_events[35] = new CVendingMachineBonusEvent;
        m_events[36] = new CBurnigGoldMonsterEvent;
        m_events[38] = new CNoNeedGoldOnGuildCreateEvent;
        m_events[39] = new CDeathTowerWinPointEvent;
        m_events[40] = new CNotApplyBalkeunEvent;
        m_events[41] = new CCharacterDayEvent;
        m_events[49] = new CRestrictCharacCreationEvent;
        m_events[50] = new CReduceUpgradeItemPay;
        m_events[51] = new COnTimeEvent;
        m_events[52] = new CBreakAwayPreventEvent;
        m_events[53] = new CPowerWarVictoriousEvent;
        m_events[58] = new CSecretShopEvent;
        m_events[86] = new CFatigueAttendance;
        m_events[87] = new CWeekendBonusEvent;
        m_events[113] = new CConditionEvent;
        m_events[91] = new CUXGameLogEvent;
        m_events[95] = new BlueMarbleDungeonEvent;
        m_events[102] = new CStopOverlabExpEvent;
        m_events[119] = new CEventAdvanceAltarOpen;
        m_events[116] = new CAradRyosikaEvent;
        m_events[155] = new Arad_MomijiEvent;
        m_events[158] = new LevelupSupportEvent;
        m_events[159] = new CEventStayTime;
        m_events[160] = new CEventCreateDnf;
        m_events[165] = new EventGiveMeBox;
    }
    catch (...)
    {
        return false;
    }
    // ORIG @0x08115950：call 后 xor $0x1/test/je→mov$1（x 真）+ fall mov$0。
    // 4.4 对 bool 返回物化 test%al+jne（无 xor1），==true/三目均被前端折叠成
    // 直传（922）；此形条数 927=927 且分支语义/块集一致，残余归 N 类跨版本物化。
    if (LoadEventFromDB())
        return true;
    return false;
}

bool CEventManager::LoadEventFromDB()
{
    // b28d-40: N82 封顶——帧 0x6c vs 0x68 + callee-saved 分配差，语义闭合。ORIG 0x081159b6。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    CEnvironment* env = G_CEnvironment();
    db->set_query(
        "seLect event_type, parameter1, parameter2 from dnf_event_log where "
        "end_time = 0 and server_id in (0,%d) and unix_timestamp() >= start_time "
        "order by start_time",
        env->m_serverEnvir.m_serverGroup);
    cMyTrace traceEnter(__PRETTY_FUNCTION__, 0x296, 9, true, true);
    traceEnter("[!] Load Event From DB");
    if (!db->exec(true))
        return false;
    const int rows = db->get_n_rows();
    for (int i = 0; i < rows && db->fetch(); ++i)
    {
        int type = 0;
        unsigned short p1 = 0;
        unsigned short p2 = 0;
        if (!db->get_int(0, type) || !db->get_ushort(1, p1) ||
            !db->get_ushort(2, p2))
            return false;
        Word_Param param;
        param.m_duration = p1;
        param.m_field2 = p2;
        if (!TriggerEventStart(type, param))
            return false;
        cMyTrace traceRow(__PRETTY_FUNCTION__, 0x305, 9, true, true);
        traceRow("\t- Event Code(%d) , Parameter1(%d), Parameter2(%d)",
                 type, (int)p1, (int)p2);
    }
    return true;
}

void CEventManager::eventDeliveryNotify(int type, DeliveryMsg& msg)
{
    // ORIG 0x08116774：无局部槽，条件与调用各重读 m_delivery（+0x298）
    // 两次（P0 表达式直读）；body 尾 `jmp +1; nop` 中继需 P223 goto 尾空标签形。
    if (m_delivery == 0)
        goto done;
    m_delivery->notify(type, msg);
done:;
}


namespace
{
// ORIG 每项 0x14 字节（_FindEventTable 0x811a15d / InsertEventTable 0x811a4d4 证据）：
//   +0x00 event_id / +0x04 apply_type / +0x08 CEventBase* / +0x0c name(char*) /
//   +0x10 explain(char*)。
struct EventTableEntry
{
    int id;              // +0x00
    int apply_type;      // +0x04
    CEventBase* base;    // +0x08
    const char* name;    // +0x0c（event_name）
    const char* explain; // +0x10（event_explain key/string）
};
// §4.5 正解：events 为本 TU 独占定义（匿名命名空间，无外部引用，ORIG BSS
// 0x936fa60）。extern "C" 于此无链接信号作用，移除改正常 C++ 链接。
EventTableEntry events[0x4f];  // ORIG 0x936fa60（BSS 零初始化）
}  // namespace

CEventTable::CEventTable() {}

// ORIG 0x0811a0ce：空析构（T D1/D2 同址）。
CEventTable::~CEventTable() {}

int CEventTable::GetEventTableCount()
{
    return 0x4f;
}

CEventBase* CEventTable::GetEventBase(int idx)
{
    if (idx < 0 || GetEventTableCount() <= idx)
        return 0;
    return events[idx].base;
}

int CEventTable::GetEventId(int idx)
{
    if (idx < 0 || GetEventTableCount() <= idx)
        return 0;
    return events[idx].id;
}

int _FindEventTable(int id, const char* name, bool& dup)
{
    // b28d-40: N 封顶——槽序差 + 全局事件表访问形（ORIG 0x936fa60 绝对寻址 vs
    // 4.4 成员 disp），语义闭合。ORIG 0x0811a15d。
    for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
    {
        if (events[i].id == id)
        {
            const char* tblName = events[i].name;
            const std::size_t n = name ? strlen(name) : 0;
            if (tblName != 0 && name != 0 && strncmp(tblName, name, n) == 0)
                return i;
            dup = true;
            return i;
        }
    }
    return -1;
}

// ORIG 0x811a1e0（0x2f4 字节）：经 DBMgr::GetDBHandle 查询
// dnf_event_info(event_id, event_name)，逐行 _FindEventTable 校验；把校验通过
// 的事件表索引记入集合，最后将「未在 DB 中」的事件表索引追加到 out，返回是否成功。
bool CEventTable::VerifyEventTable(std::vector<int>& out)
{
    // b28d-40: N78/N82 封顶——帧 0x17c vs 0x170 + memset 展开形差，语义闭合。
    // ORIG 0x0811a1e0。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE);
    db->set_query("seLect event_id, event_name from dnf_event_info");
    if (!db->exec(true))
        return false;

    std::map<int, int> found;
    const int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        if (!db->fetch())
            return false;
        int id = 0;
        if (!db->get_int(0, id))
            return false;
        char name[0xff];
        if (!db->get_str(1, name, 0xff))
            return false;
        bool dup = false;
        int idx = _FindEventTable(id, name, dup);
        if (idx < 0)
            continue;
        if (dup)
            return false;
        // P167：ORIG 用 map::insert(make_pair(idx,1))（首值保留），非 operator[]
        found.insert(std::make_pair(idx, 1));
    }

    for (int i = 0; i < GetEventTableCount(); ++i)
    {
        if (found.find(i) == found.end())
            out.push_back(i);
    }
    return true;
}

// ORIG 0x811a4d4（0x2d0 字节）：为空则直接返回 1；否则遍历 ids 中各事件表索引，
// 用事件表 name/explain 字段构造 "(%d,%d,'%s','%s')" 插入串（explain 经
// g_scriptStringManager_.findString(4, ...) 解析），一次性 INSERT 到
// dnf_event_info，exec 成功返回 1，出错码 0x426（诱骗重复）或失败返回 0。
bool CEventTable::InsertEventTable(std::vector<int>& ids)
{
    // b28d-40: N78/N82 封顶——帧 0x15c vs 0x150 + memset 展开形差，语义闭合。
    // ORIG 0x0811a4d4。
    if (ids.empty())
        return true;

    std::string sql;
    // ORIG 0x811a4d4：begin() 非 const → std::vector<int>::iterator。
    for (std::vector<int>::iterator it = ids.begin(); it != ids.end(); ++it)
    {
        const int idx = *it;
        const EventTableEntry& e = events[idx];
        // ORIG：explain/name 字段直接使用（无空指针 fallback），
        // 仅 explain 经 findString(4, explain, 0) 解析后使用。
        const char* resolved = g_scriptStringManager_.findString(4, e.explain, 0);
        char buf[0xff];                                    // ORIG 缓冲 0xff 字节
        std::memset(buf, 0, sizeof(buf));                  // ORIG 清 0xff 字节
        std::sprintf(buf, "(%d,%d,'%s','%s')", e.id, e.apply_type, e.name,
                     resolved);
        sql += buf;
        if (it + 1 != ids.end())                           // ORIG 末位元素不加逗号
            sql += ",";
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE);

    std::string query = "inSert into dnf_event_info(event_id, apply_type, event_name, event_explain) values";
    query += sql;
    db->set_query("%s", query.c_str());
    if (!db->exec(true))
    {
        (void)db->getDBError();
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：changeCharacName（ORIG T 0x08116276 sz 0x98，
// 报告 docs/class_func_reports/CEventManager/changeCharacName.md 全文已读）：
// 遍历事件数组 0..0xa5：槽非空且 IsEventing(0)（ORIG vptr+0x34 槽）为真 →
// 调该事件 vptr+0x20 槽（ORIG changeCharacName 槽，头内声明序与 ORIG vtable
// 对位：_ZTV10CEventBase @0x08b4bae0 实测 +0x20=changeCharacName、
// +0x34=IsEventing），实参 (user, key, name, flag)。
// b28d-40: N7 封顶（同 dailyresetData，虚调用目标/索引寄存器轮转）。ORIG 0x08116276。
void CEventManager::changeCharacName(CUser* user, int key, char* name, int flag)
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
        {
            if (m_events[i]->IsEventing(0))
                m_events[i]->changeCharacName(user, key, name, flag);
        }
    }
}
