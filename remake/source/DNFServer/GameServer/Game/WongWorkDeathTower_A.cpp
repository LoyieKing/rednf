// ============================================================================
// df_game_r 还原 —— WongWork::CDeathTower 嵌套类（G4 批次，WongWorkDeathTower_A）
//
// 本 TU 承载 CPacketHandler 全部 make*/append* 包构造（ORIG 0x0845fc6c..0x084605a1）、
// CDungeonMgr C1/D1（ORIG W 0x082a74b0/0x082a74b6）、CPlayData
// generateRewardExp/generateRewardItem 与显式空 D1（ORIG T 0x08462538/0x08461fc0 /
// W 0x082a7572）以及 CPlayData C1 / stRewardItem_t C1（生命周期闭环所需，
// ORIG W 0x82a7538 / 0x82a7502：逐 Inven_Item()/stRewardItem_t() 子对象构造）。
// 头声明见 CDeathTower.h（唯一完整定义点；CGameManager.h 仅 include）。
// ============================================================================
#include "CDeathTower.h"

#include "CDungeon.h"
#include "CMap.h"
#include "CDataManager.h"          // G_CDataManager / GetChannelScript / getItemList
#include "GlobalData.h"            // GlobalData::s_event_manager
#include "CGuildServerProxy.h"     // class CEventManager（GetRepeatEvent）
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CServerEvent.h"
#include "CEnvironment.h"          // G_CEnvironment
#include "CItemGeneratorMgr.h"
#include "QuickParty.h"

namespace {

// ORIG fistpll / fistpl（默认 FPU 控制字 RC=00：就近舍入 ties-to-even）。
// 本 TU 两处奖励倍率取整均按该语义【推断：就近舍入】。
inline long long roundFpuNearest(double x)
{
    return (long long)(x + (x >= 0.0 ? 0.5 : -0.5));
}

} // namespace

namespace WongWork {

// ---- CPlayData::stRewardItem_t ----------------------------------------------
// ORIG W 0x82a7502：do-while 构造 10 个 Inven_Item（步长 0x3d），起点 this+4。
CDeathTower::CPlayData::stRewardItem_t::stRewardItem_t()
{
    for (int i = 0; i < 10; ++i)
        m_items[i].~Inven_Item();  // 定位释放后按 ORIG 形式重建，保持隐式与显式一致
    for (int i = 0; i < 10; ++i)
        new (&m_items[i]) Inven_Item();
}
CDeathTower::CPlayData::CPlayData()
    : m_lastRoutedTurn(0), m_playTime(0), m_lastPlayTime(0),
      m_startMemberCnt(0)
{
    // ORIG W 0x82a7538：仅按序构造 m_rewards[4]（编译器子对象循环）；其余标量无初始化。
}
CDeathTower::CPlayData::~CPlayData()
{
    // ORIG W 0x82a7572：push ebp/mov esp,ebp/pop ebp/ret —— 空函数。
    // m_rewards[4] 内 Inven_Item 为平凡析构，用户空析构体与 ORIG 一致。
}

// ==============================================================================
// CDungeonMgr 生命周期（ORIG W 0x082a74b0 C1/C2、0x082a74b6 D1/D2：全为空函数）
// ==============================================================================
CDeathTower::CDungeonMgr::CDungeonMgr() {}
CDeathTower::CDungeonMgr::~CDungeonMgr() {}

// ==============================================================================
// CPacketHandler 生命周期（ORIG W 0x08469a8c C1/C2 → PacketGuard C1；
//                                    0x08469aa0 D1/D2 → PacketGuard D1）
// ==============================================================================
CDeathTower::CPacketHandler::CPacketHandler() {}
CDeathTower::CPacketHandler::~CPacketHandler() {}

// ==============================================================================
// CPacketHandler 包构造族（基类 PacketGuard→InterfacePacketBuf 的 put_* 直调）
// ==============================================================================

// ORIG T 0x0845fc6c：put_header(0,0x27); put_short(b); put_short(a)；无 finalize。
void CDeathTower::CPacketHandler::makePickupItemHeader(unsigned short a, unsigned short b)
{
    put_header(0, 0x27);
    put_short((int)b);
    put_short((int)a);
}

// ORIG T 0x0845fcc4：对 4 个队伍槽位逐个写入金钱信息后 finalize(true)。
void CDeathTower::CPacketHandler::appendPickupMoney(int* moneyA, int* moneyB, CParty* party)
{
    for (int i = 0; i <= 3; ++i)
    {
        bool validSlot = false;
        if (party->get_user(i) != 0 && party->checkValidUser(i))
            validSlot = true;
        if (!validSlot)
        {
            put_byte(0);
            put_int(0);
        }
        else
        {
            put_byte(1);
            put_int(moneyA[i]);
            put_byte(1);
            put_int(moneyB[i]);
        }
    }
    finalize(true);
}

// ORIG T 0x0845fdc0：4 字节名字符串 + ushort + short + byte 后 finalize(true)。
void CDeathTower::CPacketHandler::appendPickupItem(char* name, unsigned short uid,
                                                   int v3, int v4)
{
    for (int i = 0; i < 4; ++i)
        put_byte((int)(signed char)name[i]);
    put_short((int)uid);
    put_short(v3);
    put_byte(v4);
    finalize(true);
}

// ORIG T 0x0845fe4c：put_header(1,cmd); put_byte(0); put_byte(err); finalize(true)。
void CDeathTower::CPacketHandler::makeCommonError(ENUM_CMDPACKET cmd, ENUM_ERROR err)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    finalize(true);
}

// ORIG T 0x0845fea6：put_header(1,cmd); put_byte(0); put_byte(err); put_byte(slot); finalize(true)。
void CDeathTower::CPacketHandler::makeDropItemError(ENUM_CMDPACKET cmd, ENUM_ERROR err, char slot)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    put_byte((int)(signed char)slot);
    finalize(true);
}

// ORIG T 0x0845ff1a：
//   header(0,0x28); short uid; short x; short y; short dropIdx;
//   int item.m_addInfo(+0x12 即 Inven_Item+0x02)；
//   byte item.GetItemAttr(); int item.get_add_info();
//   short item.m_fieldb(+0x1b 即 Inven_Item+0x0b endurance);
//   int item.m_fieldd(+0x1d 即 Inven_Item+0x0d);
//   byte amp.getAbilityType(); short amp.getAbilityValue();
//   put_packet(item(+0x10 子对象)); byte(item.m_count==0 ? 1:0); finalize(true)。
void CDeathTower::CPacketHandler::makeDropItemNotify(unsigned short uid, int x, int y,
                                                     unsigned short dropIdx,
                                                     const map_item& item)
{
    put_header(0, 0x28);
    put_short((int)uid);
    put_short(x);
    put_short(y);
    put_short((int)dropIdx);
    put_int(item.m_item.m_addInfo);
    put_byte((int)item.m_item.GetItemAttr());
    put_int(item.m_item.get_add_info());
    put_short((int)item.m_item.m_fieldb);
    put_int(item.m_item.m_fieldd);
    put_byte((int)item.m_item.m_amp.getAbilityType());
    put_short((int)item.m_item.m_amp.getAbilityValue());
    put_packet(item.m_item);
    put_byte(item.m_count == 0 ? 1 : 0);
    finalize(true);
}

// ORIG T 0x084600a6：header(1,0x32); byte 1; byte order; short xy; int key; finalize(true)。
void CDeathTower::CPacketHandler::makeDropItem(char order, int xy, int itemKey)
{
    put_header(1, 0x32);
    put_byte(1);
    put_byte((int)(signed char)order);
    put_short(xy);
    put_int(itemKey);
    finalize(true);
}

// ORIG T 0x0846012c：header(0,0x1e); int 0; finalize(true)。
void CDeathTower::CPacketHandler::makeFinishLoading()
{
    put_header(0, 0x1e);
    put_int(0);
    finalize(true);
}

// ORIG T 0x08460176：header(0,0x20); short uid; byte 0; finalize(true)。
void CDeathTower::CPacketHandler::makeDieState(unsigned short uid)
{
    put_header(0, 0x20);
    put_short((int)uid);
    put_byte(0);
    finalize(true);
}

// ORIG T 0x084601da：塔排行榜包。clearTime/playTime 在前，随后委托
// CDeathTowerRanking 写入个人最优 + 榜首记录（各函数以本对象为 PacketGuard&）。
void CDeathTower::CPacketHandler::makeDisplayRangking(unsigned int tableType,
                                                      unsigned int key,
                                                      unsigned int stage,
                                                      unsigned int clearTime,
                                                      bool flag,
                                                      unsigned int dungeonIdx,
                                                      unsigned int playTime)
{
    put_header(0, 0x90);
    put_int((int)clearTime);   // ebp+0x18 → param_4
    put_int((int)stage);       // ebp+0x14 → param_3
    put_byte(flag ? 1 : 0);    // ebp+0x1c → param_5
    put_int((int)playTime);    // ebp+0x24 → param_7
    const unsigned int towerIdx = CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    CDeathTowerRanking* table =
        (CDeathTowerRanking*)CDeathTowerRanking::getRankTable(tableType);
    table->makeMyBestRecordPacket(towerIdx, key, *this);       // ORIG 0x08468b8e
    table->makeTopRankerRecordPacket(towerIdx, *this);         // ORIG 0x08468c68
    finalize(true);
}

// ORIG T 0x084602c4：header(0,0x91); int v1; 之后 4 组 [byte cnt; cnt×{int itemIdx;
// int add_info}]；逐组用 Inven_Item 步长 0x3d 遍历；finalize(true)。
void CDeathTower::CPacketHandler::makeDisplayReward(int v1,
                                                    int cnt1, const Inven_Item* items1,
                                                    int cnt2, const Inven_Item* items2,
                                                    int cnt3, const Inven_Item* items3,
                                                    int cnt4, const Inven_Item* items4)
{
    put_header(0, 0x91);
    put_int(v1);
    put_byte(cnt1);
    for (int i = 0; i < cnt1; ++i)
    {
        put_int(items1[i].m_addInfo);
        put_int(items1[i].get_add_info());
    }
    put_byte(cnt2);
    for (int i = 0; i < cnt2; ++i)
    {
        put_int(items2[i].m_addInfo);
        put_int(items2[i].get_add_info());
    }
    put_byte(cnt3);
    for (int i = 0; i < cnt3; ++i)
    {
        put_int(items3[i].m_addInfo);
        put_int(items3[i].get_add_info());
    }
    put_byte(cnt4);
    for (int i = 0; i < cnt4; ++i)
    {
        put_int(items4[i].m_addInfo);
        put_int(items4[i].get_add_info());
    }
    finalize(true);
}

// ORIG T 0x084604b0：header(0,0x92); byte flag; finalize(true)。
void CDeathTower::CPacketHandler::makeDisplayEPLP(bool flag)
{
    put_header(0, 0x92);
    put_byte(flag ? 1 : 0);
    finalize(true);
}

// ORIG T 0x0846050e：header(0,0xe); byte space; short 1;
// inv.MakeItemPacket(GetInvenTypeFromItemSpace(space), slot, *this)；finalize(true)。
void CDeathTower::CPacketHandler::makeUpdateItemList(CUser* user, ENUM_ITEMSPACE space, int slot)
{
    // GetInvenTypeFromItemSpace：既有实现返回原值（CDeathTower.cpp 定义）。
    extern int GetInvenTypeFromItemSpace(int space);

    put_header(0, 0xe);
    put_byte((int)space);
    put_short(1);
    CInventory* inven = user->getCurCharacInvenR();
    inven->MakeItemPacket((INVEN_TYPE)GetInvenTypeFromItemSpace((int)space), slot, *this);
    finalize(true);
}

// ==============================================================================
// CPlayData::generateRewardExp（ORIG T 0x08462538）
// 流程：baseExp=stage×base×DataManager 浮点权重表（tier 按
// limitOfStackableItemInTower 选 0x1464/0x15f4）→ 各类加成系数收集（纯记录
// local，最终仅 getClearRewardBonusExp 结果写入 m_rewardExp[memberIdx]，>1100000
// 记日志并截断）。
// ==============================================================================
void CDeathTower::CPlayData::generateRewardExp(int memberIdx, int stage,
                                               unsigned int baseExp, CParty* party,
                                               CDungeonMgr& dungeonMgr)
{
    CUser* user = party->get_user(memberIdx);
    unsigned int reward = 0;

    CDungeon* dungeon = dungeonMgr.getDungeon();
    const char stackableLimitFlag = dungeon->limitOfStackableItemInTower();
    if (stackableLimitFlag == '\0')
    {
        //DataManager+0xc+(stage+0x15f4)*4 float 表项
        CDataManager* dm = G_CDataManager();
        float weight = *(float*)((char*)dm + 0xc +
                                 ((unsigned int)(stage + 0x15f4)) * 4);
        double v = (double)((float)((long long)stage * (long long)baseExp) * weight);
        reward = (unsigned int)roundFpuNearest(v);
    }
    else
    {
        // DataManager+0xc+(stage+0x1464)*4 float 表项
        CDataManager* dm = G_CDataManager();
        float weight = *(float*)((char*)dm + 0xc +
                                 ((unsigned int)(stage + 0x1464)) * 4);
        double v = (double)((float)((long long)stage * (long long)baseExp) * weight);
        reward = (unsigned int)roundFpuNearest(v);
    }

    if (1100000 < reward)
    {
        float logWeight = *(float*)((char*)G_CDataManager() + 0xc +
                                    ((unsigned int)(stage + 0x1464)) * 4);
        LogManager_LogFormatBlood(
            "BLOOD_LOG : EROOR DEATHTOWER REWARD EXP(%u), LAST STAGE(%d), BASE EXP(%u), EXP WEIGHT(%f)\n",
            0x2ff, reward, stage, baseExp, (double)logWeight);
    }

    stClearRewardExpParameter_t clearParam;
    stClearRewardBonusExp_t bonus;

    const float avatarBonus = party->getAvatarBonus();

    const float creatureBonus = party->getCreatureBonus(user);

    // 事件 8：PC 房间燃烧加成（整除 /100 —— ORIG imul 0x51eb851f 实证）
    CEventManager* evPcRoom = GlobalData::s_event_manager->GetRepeatEvent(8);
    if (evPcRoom != 0 && evPcRoom->IsEventing(user))
        pcRoomRate_ = 0.0f;  // 占位写入被下方真实赋值覆盖（保持指令序）
    evPcRoom = GlobalData::s_event_manager->GetRepeatEvent(8);
    if (evPcRoom != 0 && evPcRoom->IsEventing(user))
        pcRoomRate_ = (float)((static_cast<CPCRoomBurningEvent*>(evPcRoom)->GetExpFactor()) / 100);

    // 事件 3：双倍经验（浮点除法 /100.0f）
    CEventManager* evDouble = GlobalData::s_event_manager->GetRepeatEvent(3);
    if (evDouble != 0 && evDouble->IsEventing(0))
        expDoubleRate_ = (float)(static_cast<CExpDoubleEvent*>(evDouble)->GetExpFactor()) / 100.0f;

    // 事件 0xf：疲劳燃烧加成
    CEventManager* evFatigue = GlobalData::s_event_manager->GetRepeatEvent(0xf);
    if (evFatigue != 0 && evFatigue->IsEventing(0))
        burningBonusExp_ =
            static_cast<CBurningFatigueEvent*>(evFatigue)->getBonusExpRate(user->getCurCharacUsedFatigue());

    // 频道脚本 bonus rate
    channel_script_t* channelScript = (channel_script_t*)G_CDataManager()->GetChannelScript();
    unsigned int dungeonIdx = dungeon->get_index();
    int serverGroup = *(int*)((char*)G_CEnvironment() + 0x378); //m_serverGroup（SPremiumInfo.h 注释实证）
    unsigned int channelNo = (unsigned int)G_CEnvironment()->get_channel_no();
    channelBonusRate_ = channelScript->getBonusRate((unsigned char)serverGroup, channelNo, dungeonIdx);

    // 高级会员经验加成
    if (user->IsHavePremiumAdvantage())
        premiumAdvantageRate_ =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f;

    // 服务端事件经验倍率
    serverEventExpRate_ = (float)CServerEvent::GetExpRate() / 100.0f;

    // 管理奖励
    manageBonus_ = party->CalcurateManageBonus((ManageBonusType::T)0, user, reward);

    // 结算基础通关经验（exp 经引用传入并被内部调整）
    unsigned int expInOut = reward;
    reward = party->getClearRewardBonusExp(user, expInOut, clearParam, bonus);

    if (1100000 < reward)
    {
        LogManager_LogFormatBlood(
            "BLOOD_LOG : EROOR DEATHTOWER CLEAR REWARD EXP(%u)\n", 0x324, reward);
        LogManager_LogFormatBlood(
            "BLOOD_LOG : DEATHTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            0x32e,
            (double)avatarBonus_, (double)creatureBonus_, (double)pcRoomRate_,
            (double)expDoubleRate_, (double)channelBonusRate_,
            (double)premiumAdvantageRate_, (double)serverEventExpRate_,
            (int)burningBonusExp_);
        reward = 1100000;
    }

    m_rewardExp[memberIdx] = (int)reward;
}

} // namespace WongWork
