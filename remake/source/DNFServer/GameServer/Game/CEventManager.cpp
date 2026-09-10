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
// 全局 events[] 静态初始化（ORIG `D events @0x0936fa60`，79 项 .data+sid_ 实证）专用：
#include "CAssaultOnOffEvent.h"
#include "CBloodDungeonRewardFirstEvent.h"
#include "CBloodDungeonRewardSecondEvent.h"
#include "CPCRoomWorldDropEvent2nd.h"
#include "CPvPExpPenaltyEvent.h"
#include "CPvPLiveEvent.h"
#include "CIntegratedPvPServerMatchEvent.h"
#include "COnlinePreliminaryEvent.h"
#include "CDnFLeaguePromoteFirstEvent.h"
#include "CDnFLeaguePromoteSecondEvent.h"
#include "CDoubleGoldCardEvent.h"
#include "CPremiumGoldCard.h"
#include "CGmRegistEvent.h"
#include "CPCRoomPlayTimeEvent.h"
#include "LevelUpBefore70LvEvent.h"
#include "CDimensionActivationEvent.h"
#include "AttendanceEvent.h"
#include "GrowthEquipEvent.h"
#include "CFatigueGiveItemEvent.h"
#include "NewAccountLevelUpEventToJob.h"
#include "BingoEvent.h"
#include "W8LT_EventTail.h"          // GiveGrowCreatureEvent / SeriaRoomAniDecoEvent / OneADayItemShopEvent
#include "CEventCeraShopRewardPoint.h"
#include "EventNewCharacterReward.h"
#include "HeroMissionEvent.h"        // ARAD::Singleton<HeroMissionEvent>::GetPtr（events[76]）
#include "LevelupSupport2ndEvent.h"

CEventManager::CEventManager()
{
    std::memset(m_events, 0, sizeof(m_events));
    // ORIG 0x08114ce4：memset 0x298，operator new(0x18) 后调 CDelivery::CDelivery()
    // （0x08116386），存 CDelivery* 于 +0x298。此处用真实 CDelivery 构造。
    // [b24] 直接 `m_delivery = new CDelivery`（ORIG 寄存器直传 %ebx→+0x298，
    // 无具名局部栈槽 -0x1c 中转；具名局部在 GCC11 生成 -0x1c(%ebp) 槽，A 类 1 insn 差）。
    m_delivery = new CDelivery;
}

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

// b28d-solo-CEventManager: [N7 封顶] BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹
// （P220）；ORIG 0x08116682，144=144 助记符序列全等，唯一残差=间接 call 目标寄存器
// （ORIG %ecx vs 4.4.7 恒 %eax，P230 限定 4.4.7；4.4.6 后端可翻但禁改路由表）。
// 逐条归一化比对确认无其它差异；P282 复核 vtable 槽 +0x30=delivery 与头声明序一致。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
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




CEventManager::~CEventManager()
{
    // b28d-solo-CEventManager: [N7 封顶] P183/P0 修正——ORIG 0x08114d4e 无 event/
    // delivery 具名局部（每次重读 m_events[i]；delete 自带判空），缓存局部使槽
    // -0xc→-0x10 平移（已删）。44=44 助记符序列全等，唯一残差=虚析构调用目标寄存器
    // （ORIG add $0x4 槽 %ecx vs 4.4.7 %eax，P230/N7）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
            delete m_events[i];
    }
    delete m_delivery;
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

// b28d-solo-CEventManager: [N7 封顶] ORIG 0x0811630e——虚调用目标 %ecx + 索引
// %edx/%eax vs 4.4.7 目标恒 %eax + 索引 %ecx/%edx（42=42 助记符序列全等，仅寄存器
// 角色轮转）；P282 复核 +0x34=IsEventing、+0x24=dailyresetData 槽与头声明序一致。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
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

// b28d-solo-CEventManager: [N 封顶] P292 档① 已修——ORIG 两处调用点均
// `call; test %al,%al; je`（无 setne）⇒ 返回类型改 bool（TriggerEventStart/End
// 同步归位）。本体 ORIG 0x08115bf4：78 vs 82（本轮 84→82）。P222 正向包裹
// `if (channel <= 0x10) { … } return 0;` 已复现 ORIG 的 `ja <共享尾 mov$0>` 方向
// （旧 `if (channel >= 0x11) return 0;` 生反极 `jbe` + 独立 mov$0 块）。
// 残余 N 三项：①ORIG 的 A/C/B 体置函数尾部（`and/test; jne A`）vs 4.4 fall-through
// 内联（`je 跳过`）——跨版本块排布（N3/N93附族；goto 尾标签形实测 87 条/8 个
// nop;jmp 岛反劣，已回退）；②ORIG 内部 return 0 全共享函数尾 `mov $0` 块 vs 4.4
// 各自内联（N93附尾部 return 块发射序）；③channel/bit ORIG 全程无栈槽且 channel 留
// %eax、bit 经 %edx 中转 and——4.4 落 channel=%ebx/bit 重载（N7 寄存器分配）。
// P56 直分支形 + P133 独立守卫已应用。
// b28d-solo 双后端实测：4.4.7 DIFF（78 vs 82）/ 4.4.6 后端 DIFF（后端不敏感）。
bool filterEvent(int type)
{
    if (type == 2)
        return 0;
    // P231 纯 register 存储类提示（无 asm 绑定，P0 合规）：ORIG 0x08115bf4 中
    // channel/bit 全程无栈槽（cmp $0x10,%eax / shl %cl,%ebx），4.4 裸形恒槽化。
    register unsigned int channel = (unsigned int)G_GameWorld()->GetChannelType();
    if (channel <= 0x10)
    {
        register unsigned int bit = 1U << channel;
        if ((bit & 0x1a100U) != 0)
        {
            // P251/P286：ORIG 0x5d 段是**寄存器**比较决策树（`mov 0x8(%ebp),%eax;
            // cmp $0x39,%eax; jg …`，首键 0x39 = 8 键索引中点）⇒ 原码为
            // switch(type)；if 链在 4.4 恒退化为内存直比 `cmpl $imm,0x8(%ebp)`。
            switch (type)
            {
            case 0xb:
            case 0x36: case 0x37: case 0x38: case 0x39:
            case 0x3d: case 0x3e:
            case 0x5c:
                return 0;   // 全 case 共享同一 body ⇒ 单个 TRUE 岛（ORIG +0x80）
            }
            return 1;
        }
        else if ((bit & 0x4000U) != 0)
        {
            switch (type)
            {
            case 0xb:
            case 0x39:
            case 0x3d: case 0x3e:
                break;
            default:
                return 1;
            }
            return 0;
        }
        else if ((bit & 0x600U) != 0)
        {
            switch (type)
            {
            case 9:
                break;
            default:
                return 1;
            }
            return 0;
        }
    }
    return 0;
}
// ORIG 0x08115de2 实证（push_back 4/2/2 字节 store + 迭代 deEv 读 +0/+4/+6）：
// 权威类型名 stEVENTADDINFO（vector 实例 mangled St6vectorI14stEVENTADDINFO…），
// 旧重建发明的 EventAddInfo 属发明符号（N114/§4.4）。
struct stEVENTADDINFO
{
    int eventId;             // +0x00（push_back 4 字节 store）
    unsigned short info0;    // +0x04
    unsigned short info1;    // +0x06
};

void CEventManager::MakeNotiEventInfo(PacketGuard& packet, CUser* user)
{
    // b28d-solo-CEventManager: [N7] 本轮修——两循环 `if (event == 0) continue;`
    // 改正向包裹 `if (event != 0) { if (event->IsEventing(user)) {…} }`（P296）
    // ⇒ 消 2 处 if 体尾 `jmp+nop` 中继岛（旧判「三形不消」作废：continue 岛随
    // 包裹消失），237→233（gap 0，§6.2）。ORIG 0x08115de2 实证——两循环各由
    // `if (GlobalData::s_event_manager != 0)` 包裹；`packet.put_short(activeCount)`
    // （非字面量 0）；尾部 if(!extra.empty())…else put_byte(0)；const_iterator +
    // `(*it).field`（ORIG 调 deEv 非 ptEv，P167 判据）；put_short 实参 (short) 截断。
    // 类型名 stEVENTADDINFO 已按 ORIG vector 实例 mangled 归位（原发明名 EventAddInfo
    // 违反 N114）。p167 30/30 CLEAR；4.4.7 口径 NEAR（残差=4 处虚调用目标寄存器
    // ORIG %edx/%ecx vs 4.4.7 恒 %eax，N7；P282 复核 +0x34=IsEventing/+0x1c=
    // GetAddInfo/+0x18=AppendInfo 槽序与头一致）；4.4.6 后端口径 AE（233=233，
    // 双后端读数见 /tmp/b28d_solo_CEventManager_result.tsv）。迭代器段
    // （begin/end 各带转换构造 + operator*/++/!=）与 ORIG 逐条同构。
    std::vector<stEVENTADDINFO> extra;
    packet.clear();
    packet.put_header(0, 0x6c);
    int countIndex = 0;
    int activeCount = 0;
    countIndex = packet.get_index();
    packet.put_short(activeCount);
    if (GlobalData::s_event_manager != 0)
    {
        for (int id = 0; id <= 0xa5; ++id)
        {
            CEventBase* event = GetRepeatEvent(id);
            if (event != 0)
            {
                if (event->IsEventing(user))
                {
                    packet.put_short(id);
                    ++activeCount;
                    // GetAddInfo 返回 stADDINFO（ORIG sret ABI，2026-08-28 OrigEventLeaf8f 修正）。
                    stADDINFO add = event->GetAddInfo();
                    if ((short)add.m_info0 >= 0)
                    {
                        stEVENTADDINFO item;
                        item.eventId = id;
                        item.info0 = add.m_info0;
                        item.info1 = add.m_info1;
                        extra.push_back(item);
                    }
                }
            }
        }
    }
    packet.put_short(countIndex, activeCount);
    if (GlobalData::s_event_manager != 0)
    {
        for (int id = 0; id <= 0xa5; ++id)
        {
            CEventBase* event = GetRepeatEvent(id);
            if (event != 0)
            {
                if (event->IsEventing(user))
                    event->AppendInfo(packet);
            }
        }
    }
    if (!extra.empty())
    {
        packet.put_byte((int)extra.size());
        // P13：ORIG begin()/end() 各求值一次（end 缓存 -0x48 槽），循环内不再重求。
        std::vector<stEVENTADDINFO>::const_iterator it = extra.begin();
        std::vector<stEVENTADDINFO>::const_iterator itEnd = extra.end();
        for (; it != itEnd; ++it)
        {
            packet.put_short((short)(*it).eventId);
            packet.put_short((short)(*it).info0);
            packet.put_short((short)(*it).info1);
        }
    }
    else
    {
        packet.put_byte(0);
    }
    packet.finalize(true);
}

void CEventManager::BroadcastEventInfo()
{
    // b28d-solo-CEventManager: [AE] ORIG 0x08116108 实证——PacketGuard 构造在循环外一次
    // （-0x20 槽，循环内复用）；无 world==0 / user==0 防御（P140/P171 照抄）；
    // begin 一次求值、end 每轮经 G_GameWorld() 重求值；迭代器后置 `it++`（P28，
    // ORIG 增量段 ppEi）。80=80 逐条归一化后仅剩 call/全局地址重定位伪差（P264）。
    PacketGuard packet;
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            MakeNotiEventInfo(packet, user);
            user->Send(packet);
        }
    }
}

bool CEventManager::TriggerEventStart(int idx, Word_Param param)
{
    // b28d-solo-CEventManager: [N7 封顶] ORIG 0x08115cc6——`je <mov $0 尾>` 布局，
    // 正向包裹 `if (event != 0) { body; return 1; } return 0;`（P222/P194）+
    // filterEvent bool 化后 52=52 助记符序列全等；唯一残差=3 处虚调用目标寄存器
    // （ORIG %edx vs 4.4.7 %eax，N7）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event != 0)
    {
        event->StartEvent(param);
        BroadcastEventInfo();
        event->deliveryStart(m_delivery);
        event->StartAction();
        return 1;
    }
    return 0;
}

bool CEventManager::TriggerEventEnd(int idx)
{
    // b28d-solo-CEventManager: [N7 封顶] ORIG 0x08115d60——同 TriggerEventStart 的
    // 正向包裹 `je <mov $0 尾>` 布局 + filterEvent bool 化后 43=43 全等；唯一残差=
    // 2 处虚调用目标寄存器（ORIG %edx vs 4.4.7 %eax，N7）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event != 0)
    {
        event->deliveryEnd(m_delivery);
        BroadcastEventInfo();
        event->EndEvent();
        return 1;
    }
    return 0;
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
    // b28d-solo-CEventManager: [N82/N80 封顶] §6.2 三件套已验——①gap=0（927=927）；
    // ②P167 调用点差集 153/153 CLEAR（0 缺 0 多）；③残差=全帧槽号平移 + cMyTrace
    // 变参保活顺序（P227 全表达式链已试）。ORIG 0x08114dcc。
    // b28d-solo 本轮复核：归一化后唯一实质差异 = 尾部 7 条（LoadEventFromDB 调用点
    // 的 `xor $0x1/test/je` bool 物化 + __cxa_begin/end_catch 块相对序，N104/P292；
    // ①`if(call())return true;return false;` 形 gap=0 但缺 xor 段，②否定形 928 条
    // 且 catch 块序反劣，取 ①）。双后端：4.4.7 DIFF / 4.4.6 后端 DIFF（后端免疫）。
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
    // ORIG @0x08115950：`call; xor $0x1,%eax; test %al,%al; je <mov $1 块>;
    // mov $0,%ebx; jmp end; <mov $1>`（4.1 对 bool callee 的 xor 物化 + je 到
    // ret1 块）。本轮实测三形：①正向 `if(call()) return true;` = 927=927 gap0、
    // 无 xor 段（4.4 直测 jne）；②`if(!call()) return false;` 与 ③`== false`
    // 同形 = 928 条（有 xor 段但 4.4 恒 jne 到 ret0 块 + 多 1 条 jmp，反劣）。
    // 取 ① 保 gap=0；xor 段与 je 极性属 N93附（尾部双 return 块发射序）+
    // N104（EH 尾块相对序）跨版本封顶。
    // [round3 复测] `bool ok = (LoadEventFromDB() != 0); return ok;`（P158 汇聚
    // 变量形）实测 **923 条**（4.4 恒槽化物化 `mov %al,-0x19; movzbl`），比 ORIG
    // 的寄存器物化 `xor $1; test; je; mov $0/%1,%ebx`（927 条）少 4 条且更远 ⇒
    // 按 P168a③/P292（bool 死物化）回退取直分支形（gap=0）。
    if (LoadEventFromDB())
        return true;
    return false;
}

bool CEventManager::LoadEventFromDB()
{
    // b28d-solo-CEventManager: [N 封顶] ORIG 0x081159b6 实证——fetch 在循环体内
    // （`ok = db->fetch(); if (!ok) break;`，非 for 条件）；复用同一 bool ok 局部
    // （P102，ORIG 槽 -0x25 反复赋值）；get_int/get_ushort 直接写出参
    // （type 无初始化、param 成员直接传址）；type/param 传值 TriggerEventStart。
    // 143=143（助记符序列全等）、p167 14/14 CLEAR。残差 = 聚合对象优先低槽
    // （N82附）：ORIG trace -0x38 / db -0x24 / ok -0x25 vs 4.4 trace -0x48 /
    // db -0x28 / ok -0x21（rows -0x20、i -0x1c、type -0x4c、param -0x50 两侧一致）；
    // 4.1 按生命周期穿插 vs 4.4 类对象沉底 + bool 紧贴下邻标量，声明序/纯 register
    // 均不可翻（P182/N95）。变参寄存器已对齐 %edi/%esi/%ebx（P87）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 NEAR（槽平移非后端敏感，
    // 与 N7 不同族）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect event_type, parameter1, parameter2 from dnf_event_log where "
        "end_time = 0 and server_id in (0,%d) and unix_timestamp() >= start_time "
        "order by start_time",
        G_CEnvironment()->m_serverEnvir.m_serverGroup);
    cMyTrace traceEnter(__PRETTY_FUNCTION__, 0x296, 9, true, true);
    traceEnter("[!] Load Event From DB");
    bool ok;
    ok = db->exec(true);
    if (!ok)
        return false;
    const int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        ok = db->fetch();
        if (!ok)
            break;
        int type;
        Word_Param param;
        ok = db->get_int(0, type);
        if (!ok)
            return false;
        ok = db->get_ushort(1, param.m_duration);
        if (!ok)
            return false;
        ok = db->get_ushort(2, param.m_field2);
        if (!ok)
            return false;
        if (!TriggerEventStart(type, param))
            return false;
        // P227 全表达式临时链：ORIG 0x08115af2 在 ctor 前右起求 varargs 并以
        // %edi/%esi/%ebx 跨 ctor 保活（4.4 具名对象形恒用 caller-saved）。
        cMyTrace(__PRETTY_FUNCTION__, 0x305, 9, true, true)(
            "\t- Event Code(%d) , Parameter1(%d), Parameter2(%d)",
            type, (int)param.m_duration, (int)param.m_field2);
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
// ORIG `nm -S $O` = `0936fa60 0000062c D events`：本 TU 文件作用域**全局**对象（裸名
// `events`，非 `_ZL`/匿名 namespace 局部），且位于 .data（**非 BSS**）——79 项的
// id/apply_type/name/explain 是编译期静态初值（objdump -j .data 逐字复核，与
// local://events_table.md 0 差异）；base(+0x08) 在 .data 全 0，由同 TU
// __static_initialization_and_destruction_0 @0x0811a7a4 启动期填充：实证恰 79 次
// `mov %eax,<events+off>`，off 全部 ≡8 (mod 0x14)、下标 0..78 连续无缺；78 项为
// `operator new(sizeof(CxxxEvent))` + `CxxxEvent::CxxxEvent()`（new 尺寸∈{12,16,20,24,
// 32,96}，逐条等于对应类 sizeof），idx76 唯一例外 = ARAD::Singleton<HeroMissionEvent>::
// GetPtr() 写 events[76].base（0x9370058）。ORIG sid_ 全程仅 1 次 __cxa_atexit（宿主为
// std::ios_base::Init），即 78 个 new 出的事件对象**不登记析构**（进程退出不清理）。
// ⇒ 以聚合初始化器内联 new 的形态复现：静态字段落 .data，base 动态初始化按声明序
// （下标 0..78）在 sid_ 逐项存储。explain 与 id **不一一对应**（如 idx23/24 为
// "event_msg_00"、idx27 "event_msg_33"、idx68 "event_msg_190"），必须逐条照抄。
EventTableEntry events[0x4f] =
{
    { 1, 0, new CUnlimitFatigueEvent, "CUnlimitFatigueEvent", "event_msg_01" },
    { 2, 1, new CMaxFatigueFactorEvent, "CMaxFatigueFactorEvent", "event_msg_02" },
    { 3, 1, new CExpDoubleEvent, "CExpDoubleEvent", "event_msg_03" },
    { 4, 4, new CCoinEventPerDay, "CCoinEventPerDay", "event_msg_04" },
    { 5, 1, new CCoinEventOnCharCreate, "CCoinEventOnCharCreate", "event_msg_05" },
    { 6, 0, new CLeadingChannelEvent, "CLeadingChannelEvent", "event_msg_06" },
    { 7, 1, new CItemDropRatioEvent, "CItemDropRatioEvent", "event_msg_07" },
    { 8, 1, new CPCRoomBurningEvent, "CPCRoomBurningEvent", "event_msg_08" },
    { 9, 3, new CSchoolMatchEvent, "CSchoolMatchEvent", "event_msg_09" },
    { 10, 0, new CPCRoomFatigueEvent, "CPCRoomFatigueEvent", "event_msg_10" },
    { 12, 0, new CReformingDanjinEvent, "CReformingDanjinEvent", "event_msg_12" },
    { 13, 2, new CCoinRefillEvent, "CCoinRefillEvent", "event_msg_13" },
    { 15, 0, new CBurningFatigueEvent, "CBurningFatigueEvent", "event_msg_15" },
    { 16, 0, new CClearRewardCardEvent, "CClearRewardCardEvent", "event_msg_16" },
    { 17, 1, new CCeraShopBonusItemEvent, "CCeraShopBonusItemEvent", "event_msg_17" },
    { 18, 0, new CTournamentPvPEvent, "CTournamentPvPEvent", "event_msg_18" },
    { 19, 0, new CGoldCardBlankItemEvent, "CGoldCardBlankItemEvent", "event_msg_19" },
    { 21, 0, new CCollectArchieveEventLog, "CCollectArchieveEventLog", "event_msg_21" },
    { 22, 0, new CPCRoomWorldDropEvent, "CPCRoomWorldDropEvent", "event_msg_22" },
    { 24, 1, new CPartyExpBonusEvent, "CPartyExpBonusEvent", "event_msg_24" },
    { 29, 0, new CPcRoomCardBlankItemEvent, "CPcRoomCardBlankItemEvent", "event_msg_29" },
    { 30, 0, new CPowerWarEvent, "CPowerWarEvent", "event_msg_30" },
    { 32, 0, new CStabToDeathEvent, "CStabToDeathEvent", "event_msg_32" },
    { 33, 0, new CGuildWarEvent, "CGuildWarEvent", "event_msg_00" },
    { 34, 0, new CAutoMarketConditionsControlEvent, "CAutoMarketConditionsControlEvent", "event_msg_00" },
    { 35, 0, new CVendingMachineBonusEvent, "CVendingMachineBonusEvent", "event_msg_35" },
    { 36, 0, new CBurnigGoldMonsterEvent, "CBurnigGoldMonsterEvent", "event_msg_36" },
    { 38, 0, new CNoNeedGoldOnGuildCreateEvent, "CNoNeedGoldOnGuildCreateEvent", "event_msg_33" },
    { 39, 1, new CDeathTowerWinPointEvent, "CDeathTowerWinPointEvent", "event_msg_34" },
    { 40, 0, new CNotApplyBalkeunEvent, "CNotApplyBalkeunEvent", "event_msg_37" },
    { 41, 1, new CCharacterDayEvent, "CCharacterDayEvent", "event_msg_38" },
    { 42, 0, new CAssaultOnOffEvent, "CAssaultOnOffEvent", "event_msg_39" },
    { 43, 0, new CFatigueBuffEvent, "CFatigueBuffEvent", "event_msg_40" },
    { 45, 1, new CBloodDungeonRewardFirstEvent, "CBloodDungeonRewardFirstEvent", "event_msg_42" },
    { 46, 1, new CBloodDungeonRewardSecondEvent, "CBloodDungeonRewardSecondEvent", "event_msg_43" },
    { 48, 0, new CPCRoomWorldDropEvent2nd, "CPCRoomWorldDropEvent2nd", "event_msg_45" },
    { 49, 1, new CRestrictCharacCreationEvent, "CRestrictCharacCreationEvent", "event_msg_46" },
    { 50, 1, new CReduceUpgradeItemPay, "CReduceUpgradeItemPay", "event_msg_47" },
    { 51, 2, new COnTimeEvent, "COnTimeEvent", "event_msg_48" },
    { 52, 0, new CBreakAwayPreventEvent, "CBreakAwayPreventEvent", "event_msg_49" },
    { 53, 0, new CPowerWarVictoriousEvent, "CPowerWarVictoriousEvent", "event_msg_50" },
    { 54, 0, new CPvPExpPenaltyEvent, "CPvPExpPenaltyEvent", "event_msg_51" },
    { 55, 0, new CPvPLiveEvent, "CPvPLiveEvent", "event_msg_52" },
    { 56, 0, new CIntegratedPvPServerMatchEvent, "CIntegratedPvPServerMatchEvent", "event_msg_53" },
    { 58, 0, new CSecretShopEvent, "CSecretShopEvent", "event_msg_54" },
    { 57, 0, new COnlinePreliminaryEvent, "COnlinePreliminaryEvent", "event_msg_55" },
    { 61, 0, new CDnFLeaguePromoteFirstEvent, "CDnFLeaguePromoteFirstEvent", "event_msg_57" },
    { 62, 0, new CDnFLeaguePromoteSecondEvent, "CDnFLeaguePromoteSecondEvent", "event_msg_58" },
    { 64, 0, new CDoubleGoldCardEvent, "CDoubleGoldCardEvent", "event_msg_60" },
    { 65, 0, new CPremiumGoldCard, "CPremiumGoldCard", "event_msg_61" },
    { 67, 0, new CGmRegistEvent, "CGmRegistEvent", "event_msg_63" },
    { 86, 0, new CFatigueAttendance, "CFatigueAttendance", "event_msg_86" },
    { 87, 0, new CWeekendBonusEvent, "CWeekendBonusEvent", "event_msg_87" },
    { 91, 0, new CUXGameLogEvent, "CUXGameLogEvent", "event_msg_91" },
    { 92, 0, new CPCRoomPlayTimeEvent, "CPCRoomPlayTimeEvent", "event_msg_92" },
    { 93, 0, new LevelUpBefore70LvEvent, "LevelUpBefore70LvEvent", "event_msg_93" },
    { 94, 0, new CDimensionActivationEvent, "CDimensionActivationEvent", "event_msg_94" },
    { 95, 0, new BlueMarbleDungeonEvent, "BlueMarbleDungeonEvent", "event_msg_95" },
    { 96, 1, new AttendanceEvent, "AttendanceEvent", "event_msg_96" },
    { 100, 0, new GrowthEquipEvent, "GrowthEquipEvent", "event_msg_100" },
    { 101, 0, new CFatigueGiveItemEvent, "CFatigueGiveItemEvent", "event_msg_101" },
    { 102, 0, new CStopOverlabExpEvent, "CStopOverlabExpEvent", "event_msg_102" },
    { 103, 0, new GiveGrowCreatureEvent, "GiveGrowCreatureEvent", "event_msg_103" },
    { 104, 0, new NewAccountLevelUpEventToJob, "NewAccountLevelUpEventToJob", "event_msg_104" },
    { 105, 0, new SeriaRoomAniDecoEvent, "SeriaRoomAniDecoEvent", "event_msg_105" },
    { 106, 0, new BingoEvent, "BingoEvent", "event_msg_106" },
    { 109, 0, new OneADayItemShopEvent, "OneADayItemShopEvent", "event_msg_109" },
    { 113, 0, new CConditionEvent, "CConditionEvent", "event_msg_67" },
    { 116, 2, new CAradRyosikaEvent, "CAradRyosikaEvent", "event_msg_190" },
    { 119, 0, new CEventAdvanceAltarOpen, "CEventAdvanceAltarOpen", "event_msg_103" },
    { 155, 4, new Arad_MomijiEvent, "Arad_MomijiEvent", "event_msg_196" },
    { 158, 0, new LevelupSupportEvent, "LevelupSupportEvent", "event_msg_158" },
    { 159, 2, new CEventStayTime, "CEventStayTime", "event_msg_159" },
    { 160, 4, new CEventCreateDnf, "CEventCreateDnf", "event_msg_160" },
    { 161, 2, new CEventCeraShopRewardPoint, "CEventCeraShopRewardPoint", "event_msg_161" },
    { 162, 2, new EventNewCharacterReward, "EventNewCharacterReward", "event_msg_162" },
    { 164, 0, ARAD::Singleton<HeroMissionEvent>::GetPtr(), "HeroMissionEvent", "event_msg_163" },
    { 163, 0, new LevelupSupport2ndEvent, "LevelupSupport2ndEvent", "event_msg_164" },
    { 165, 0, new EventGiveMeBox, "EventGiveMeBox", "event_msg_165" },
};

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
    // b28d-solo-CEventManager: [AE] P0/P183——ORIG 0x0811a15d 无 tblName 具名局部、
    // strlen(name) 无条件调用（无 name/tblName 判空守卫），逐次重读 events[i].name；
    // P222 正向包裹 + 单出口（dup 块 fall-through 到 return i，ORIG 0x0811a1c1）
    // ⇒ 43=43；逐条归一化仅剩 events 绝对寻址/重定位伪差（N102/P264）。
    for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
    {
        if (events[i].id == id)
        {
            if (strncmp(events[i].name, name, strlen(name)) != 0)
                dup = true;
            return i;   // ORIG 0x0811a1c1 单出口：dup 块 fall-through 到 return i
        }
    }
    return -1;
}

// ORIG 0x811a1e0（0x2f4 字节）：经 DBMgr::GetDBHandle 查询
// dnf_event_info(event_id, event_name)，逐行 _FindEventTable 校验；把校验通过
// 的事件表索引记入集合，最后将「未在 DB 中」的事件表索引追加到 out，返回是否成功。
bool CEventTable::VerifyEventTable(std::vector<int>& out)
{
    // b28d-solo-CEventManager: [N 封顶] ORIG 0x0811a1e0 实证——局部全函数头声明（P212）：
    // ok(-0x1d,=false)/id(-0x48,=0)/name[0xff](-0x16f,={0})/dup(-0x49,=false)/
    // idx(-0x50,=-1)/found map(-0x68)；i 为函数级变量，两个 for 复用（P191 双初始化
    // `int i=0; for(i=0;…)` 两次清零）；fetch 失败 break。name 清零为 4.1 内联
    // rep stos（N78 封顶）。
    // 本轮修：P293 第二循环改具名迭代器 `iterator it = found.find(i); if (it ==
    // found.end())`（ORIG 0x0811a427 先 find sret -0x70 后 end sret -0x28；旧单表达式
    // 形 4.4 反序且令 id/dup/idx/found/i 全组深 4）⇒ 全部槽位归位（-0x48/-0x49/
    // -0x50/-0x68/-0x6c/-0x70/-0x2c/-0x34/-0x3c/-0x44 与 ORIG 逐一对齐）。
    // b28d-solo 本轮修（DIFF→AE）：唯一实质残差 = ORIG 0x0811a395 `mov -0x50(%ebp),
    // %eax; not %eax; shr $0x1f,%eax; test %al,%al; je`（4.1 对 `idx >= 0` 的寄存器
    // bool 物化）vs 4.4 `test %eax,%eax; js` 直跳。旧 `(unsigned int)~idx >> 31`
    // 表达式形被折叠（无效）；**改 `while (idx >= 0) { …; break; }` 后 4.4 精确复现
    // not/shr/test/je 四段**（P225 推广：while+break 对 4.1 寄存器 bool 物化同样有效，
    // 不只 setne 族），217→220 与 ORIG 219 在 compare_tu clean 口径下 AE（数据地址
    // 伪差豁免，P264）。槽位/帧 0x17c 与 ORIG 全等。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE);
    bool ok = false;
    int id = 0;
    char name[0xff] = {0};
    bool dup = false;
    int idx = -1;
    std::map<int, int> found;
    db->set_query("seLect event_id, event_name from dnf_event_info");
    ok = db->exec(true);
    if (!ok)
        return false;

    const int rows = db->get_n_rows();
    int i = 0;
    for (i = 0; i < rows; ++i)
    {
        ok = db->fetch();
        if (!ok)
            break;
        ok = db->get_int(0, id);
        if (!ok)
            return false;
        ok = db->get_str(1, name, 0xff);
        if (!ok)
            return false;
        idx = _FindEventTable(id, name, dup);
        // P197④ 镜像：ORIG 0x0811a395 `not %eax; shr $0x1f,%eax; test %al,%al; je`
        //（4.1 对 `idx >= 0` 的寄存器 bool 物化）vs 4.4 `test %eax,%eax; js` 直跳。
        // 等价位形 `(unsigned int)~idx >> 31` 实测仍被 4.4 折叠为 js（无收益），
        // 保留可读正向形；2 条差归 N93附/P197④ 封顶。
        while (idx >= 0)
        {
            if (dup)
                return false;
            // P167：ORIG 用 map::insert(make_pair(idx,1))（首值保留），非 operator[]
            found.insert(std::make_pair(idx, 1));
            break;
        }
    }

    for (i = 0; i < GetEventTableCount(); ++i)
    {
        // P293：ORIG 0x0811a427 先 find（sret -0x70）后 end（sret -0x28）⇒ 原码
        // 为具名迭代器形；单表达式 `find(i)==end()` 在 4.4 恒反序（end 先）。
        std::map<int, int>::iterator it = found.find(i);
        if (it == found.end())
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
    // b28d-solo-CEventManager: [AE] ORIG 0x0811a4d4 实证——buf[0xff]={0} 声明在
    // ids.empty() 之后；while + `++it; if (it != ids.end()) sql += ",";`
    // （无 operator+，ORIG 两次 end()/ne() 调用）；events[*it].field 逐次重读
    // （P157/P183，无 idx/e 引用缓存）；set_query 单格式串 "…values%s"
    // （ORIG .rodata 0x8b50858 实读）；bool ok 局部 + getDBError()==0x426 判定。
    // P214① 正向包裹 `if (!ids.empty()){…} return true;` 复现 `call empty; xor $1;
    // test; je <成功尾>` ⇒ 208=208；逐条归一化仅剩重定位伪差（P264）。
    // ORIG 0x0811a4d4 实证：`call empty; xor $1; test; je <成功尾>` ⇒ 正向包裹
    // `if (!ids.empty()) { … } return true;`（P214①/P168a），非 `if(empty)return true`。
    if (!ids.empty())
    {
        char buf[0xff] = {0};
        std::string sql;
        std::vector<int>::iterator it = ids.begin();
        while (it != ids.end())
        {
            std::sprintf(buf, "(%d,%d,'%s','%s')", events[*it].id,
                         events[*it].apply_type, events[*it].name,
                         g_scriptStringManager_.findString(4, events[*it].explain, 0));
            sql += buf;
            ++it;
            if (it != ids.end())                           // ORIG 末位元素不加逗号
                sql += ",";
        }

        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE);
        bool ok = false;
        db->set_query(
            "inSert into dnf_event_info(event_id, apply_type, event_name, event_explain) values%s",
            sql.c_str());
        ok = db->exec(true);
        if (!ok)
        {
            if (db->getDBError() == 0x426)
                return false;
            return false;
        }
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
// b28d-solo-CEventManager: [N7 封顶] 同 dailyresetData——50=50 助记符序列全等，
// 残余=虚调用目标 %ecx vs 4.4.7 %eax + 索引寄存器轮转；P282 复核 +0x34=IsEventing、
// +0x20=changeCharacName 槽与头声明序一致。ORIG 0x08116276。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 IDENTICAL（N7 敏感族）。
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
