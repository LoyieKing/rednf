#include "GameRand.h"
// df_game_r CParty（G2 队伍聚合根）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/CParty.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖子对象（CBattle_Field/BattleData/GameResultSet/...）为后续批次，
// 本 TU 以 asm-label extern 发调用、以 char 数组占位，主 agent 集成后替换。
#include <string.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
// 显式构造/析构调用带模板参数(含逗号)时需 typedef，否则 ~std::map<int,int>() 的逗号被解析为分隔符。
typedef std::map<int, int> MapII;
#include <vector>
#include <new>

#include "QuickParty.h"   // 必须先于 CDungeon.h/CParty.h（QUICKPARTY_H 守卫需先放开，避免 CDungeon 最小 CQuickPartySystemManager 与权威冲突）
#include "CParty.h"
#include "CDungeon.h"
#include "Party.h"       // 仅ORIG 类 Party（DungeonPartyManager 数据类族，worklist Party.txt）
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "CPartyTelePort.h"
#include "LogManager.h"

#include "BestClearTime.h"          // setNewRecord（dungeon_start 经 GetCharacExpandData(0x12)）
#include "CDungeonClear.h"          // getClearedDungeonDiff（dungeon_start 难度门槛）
#include "SkillSlot.h"              // checkskillSlotEmptyCount（dungeon_start 反外挂）
#include "HistoryLog.h"             // WriteDunStart
#include "CStatisticServerProxy.h"  // statistc_proxy::add（get_item 随机选项拾取统计）
#include "DB_AccountCargoSync.h"    // DB_InsertRandomOptionItemInform::makeRequest（get_item）
#include "CGenUniqueNo.h"           // genIPGNo（get_item 时装入库）
#include "CCirculationStatistic.h"  // AddCirculationStatistic（金币流水）
#include "DataManagerInitDepends.h" // IsLightServer
#include "Arad_DataManager.h"       // ARAD::Singleton<ServiceRestrictManager>
#include "PacketTypes.h"            // MSG_PARTY_SET_INFO 完整类型
#include "CSecretShop.h"            // secretshop::CSecretShop 完整类型
#include "CServerProxyMgr.h"        // CServerProxyMgr<CGuildServerProxy>
#include "CAssaultMgr.h"            // pvp_assault 单例（walkout_member）
#include "CSpecialItemRoutingManager.h" // StartRouting（get_item 史诗路由）
#include "CTradeSpace.h"            // check_trade_possibility/cancel_trade（game_start）
#include "CSystemTime.h"            // getCurSec/getCurTickCount/getCurDate
#include "ServiceRestrictManager.h" // isRestricted（drop_item 金币丢弃限制）

#include "CWorldMap.h"
#include "ServerParameterScript.h"

#include "BattleData.h"
#include "SECRET_SHOP_DATA.h"
#include "Secu_HackLogCheckByParty.h"   // Secu_HackLogCheckByParty::Init/ctor（§7）

#include "PvPTypes.h"
#include "CGameManager.h"
#include "CGuildServerProxy.h"
#include "GameResultSet.h"
#include "CVillageMonsterMgr.h"
#include "CDataManager.h"
#include "CDimensionActivationEvent.h"
#include "CBattle_Field.h"
#include "GameWorld.h"
#include "TimerQueue.h"   // insert_time_* 定时器注册（ORIG 0x8630e16 InsertTimer）
#include "CUserProc.h"   // APSystem::CUserProc::ClearActionAndSendtoUser（SetCardNumber）
#include "CPCRoomBurningEvent.h"   // generateBloodRewardExp 事件加成
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
// MapInfo 经 CBattle_Field.h→WarField.h 引入；显式析构/构造带模板参数需 typedef。
typedef std::vector<MapInfo> MapInfoVec;
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDungeonEntranceLog.h"
#include "GameTypes.h"

#include "EventNewCharacterReward.h"
#include "GlobalData.h"
#include "CServerEvent.h"   // CServerEvent::GetExpRate（sumGainedEachExp）

// ---- ClearRewardFinal 批次（clear_reward / set_clear_reward_exp_parameter）----
#include "CItemGeneratorMgr.h"       // WongWork::CItemGeneratorMgr::getGenerator(2)
#include "CPartyExpBonusEvent.h"     // repeat 事件 0x18 队伍经验加成
#include "HeroMissionEvent.h"        // ARAD::Singleton<HeroMissionEvent>::processMission
#include "CConditionEventManager.h"  // IsFreeGoldCard / ProcessCheckStepUp
#include "CGlobalEffectManager.h"    // s_globalEffectManager_ 效果 2
#include "CEnvironment.h"            // G_CEnvironment / get_channel_no
#include "STExpReward.h"             // set_clear_reward_exp_parameter 局部结构
#include "Quest.h"                   // check_clear_item/get_clear_itemlist（clear_reward）

// ============================================================================
// 所有 extern "C" asm 桥已删除（game_verify_guide.md §9/§10）：
// 改用 include 权威头 + 真实 C++ 调用；子对象以 reinterpret_cast +
// placement-new 等价替换原 asm 标签，ABI 与 ORIG 保持一致。
//
// ============================================================================
// CParty::cMember
// ============================================================================
void CParty::cMember::Init()
{
    m_pUser = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = (char)0xff;
    m_fieldd = (char)0xff;
    memset(m_fielde, 0, 4);
    m_field12 = (char)0xff;
    m_field14 = 0;
}

CParty::cMember::cMember()
{
    Init();
}

// ============================================================================
// CParty::CItemRoutingData
// ============================================================================
void CParty::CItemRoutingData::Reset()
{
    m_routingState = 0;
    m_item.reset();
    m_itemUniqueID = 0;
    for (int i = 0; i <= 3; ++i)
        m_memberRoutingState[i] = 0;
}

CParty::CItemRoutingData::CItemRoutingData()
{
    Reset();
}

CParty::CItemRoutingData::~CItemRoutingData()
{
}

int CParty::CItemRoutingData::GetRoutingState()
{
    return m_routingState;
}

void CParty::CItemRoutingData::SetRoutingState(char state)
{
    m_routingState = (int)state;
}

Inven_Item* CParty::CItemRoutingData::GetRoutingInvenItem()
{
    return &m_item;
}

unsigned int CParty::CItemRoutingData::GetRoutingItemIndex()
{
    return (unsigned int)m_item.m_addInfo;
}

void CParty::CItemRoutingData::SetRoutingItemIndex(Inven_Item& item, unsigned int idx)
{
    m_item.setCopy(item);
    m_itemUniqueID = idx;
}

unsigned int CParty::CItemRoutingData::GetRoutingItemUniqueID()
{
    return m_itemUniqueID;
}

char CParty::CItemRoutingData::GetMemberRoutingState(int idx)
{
    if (idx >= 0 && idx <= 3)
        return m_memberRoutingState[idx];
    return 0;
}

void CParty::CItemRoutingData::SetMemberRoutingState(int idx, char state)
{
    if (idx >= 0 && idx <= 3)
        m_memberRoutingState[idx] = state;
}

// ============================================================================
// CParty 构造 / 析构
// ============================================================================
CParty::CParty()
{
    new (&m_pad1dc) CPartyResultRecvFlag;
    new (&m_pad210) CTraceMobDieHack;
    new (&m_padShop) secretshop::SECRET_SHOP_DATA();
    new (&m_padBattleData) BattleData();
    new (&m_padBattleField1) CBattle_Field();
    new (&m_padResult) GameResultSet();
    new (&m_padSecu) Secu_HackLogCheckByParty();
    new (&m_pad1ad0) CPartyTelePort();
    new (&m_padPassedMap) std::vector<MapInfo>();
    for (int i = 0xb; i <= 0x2f; ++i)
        m_field148[i - 0xb] = 0;
    m_usedCoinCount = 0;
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->SetParty(this);
    SetAssaultState(0);
    m_routingData.Reset();
    reinterpret_cast<CPartyTelePort*>(&m_pad1ad0)->init(this);
    m_randomBuffType = 0xb;
    m_isQuickParty = 0;
    init_quick_party_data();
    reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu)->Init(this);
    m_partyMemberCoinLimit = 0;
}

CParty::~CParty()
{
    reinterpret_cast<MapInfoVec*>(&m_padPassedMap)->~MapInfoVec();
    reinterpret_cast<CPartyTelePort*>(&m_pad1ad0)->~CPartyTelePort();
    reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu)->~Secu_HackLogCheckByParty();
    reinterpret_cast<GameResultSet*>(&m_padResult)->~GameResultSet();
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->~CBattle_Field();
    reinterpret_cast<BattleData*>(&m_padBattleData)->~BattleData();
    reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop)->~SECRET_SHOP_DATA();
    reinterpret_cast<CTraceMobDieHack*>(&m_pad210)->~CTraceMobDieHack();
}

void CParty::send_to_party(PacketGuard& packet)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
            m_member[i].m_pUser->Send(packet);
    }
}

// ============================================================================
// 基础访问器
// ============================================================================
void CParty::SetIDX(int idx)
{
    m_partyIndex = idx;
    init();
}

int CParty::GetPartyIndex()
{
    return m_partyIndex;
}

void CParty::set_state(char s)
{
    m_state = s;
}

char CParty::get_state()
{
    return m_state;
}

bool CParty::IsSinglePlay()
{
    return m_singlePlay != 0;
}

void CParty::SetSinglePlay(bool flag)
{
    m_singlePlay = flag ? 1 : 0;
}

char CParty::GetEPLPState()
{
    return m_eplpState;
}

char CParty::GetSelectedEPLPCmd()
{
    return m_selectedEPLPCmd;
}

void CParty::SetSelectedEPLPCmd(char cmd)
{
    m_selectedEPLPCmd = cmd;
}

char CParty::GetAssaultState()
{
    return m_assaultState;
}

void CParty::SetAssaultState(char state)
{
    m_assaultState = state;
}

CParty::cMember* CParty::GetMember()
{
    return m_member;
}

CUser* CParty::get_user(int idx)
{
    return m_member[idx].m_pUser;
}

int CParty::get_party_type() const
{
    return m_partyType;
}

char CParty::getTitleIndex()
{
    return m_titleIndex;
}

void CParty::setTitleIndex(unsigned char idx)
{
    struct V { char m_pad[0x11b]; unsigned char m_f; };
    ((V*)this)->m_f = idx;
}

char* CParty::getTitle()
{
    return m_title;
}

void CParty::setTitle(const char* title)
{
    strncpy(m_title, title, 0x20);
}

bool CParty::IsAutoCreated()
{
    return m_autoCreated;
}

void CParty::SetAutoCreated(bool flag)
{
    m_autoCreated = flag;
}

unsigned char CParty::getUserMax()
{
    return m_userMax;
}

void CParty::setUserMax(unsigned char max)
{
    m_userMax = max;
}

unsigned short CParty::getDungIndex() const
{
    return m_dungIndex;
}

void CParty::setDungIndex(unsigned short idx)
{
    m_dungIndex = idx;
}

unsigned char CParty::getDungDiffi() const
{
    return m_dungDiffi;
}

void CParty::setDungDiffi(unsigned char diffi)
{
    m_dungDiffi = diffi;
}

char CParty::get_dungeon_clear_state()
{
    return m_dungeonClearState;
}

void CParty::set_dungeon_clear_state(char state)
{
    if (state <= 6)
        m_dungeonClearState = state;
}

void CParty::reset_recv_flag()
{
    reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)->Clear();
}

void CParty::SetRecvResultFlag(bool flag)
{
    m_recvResultFlag = flag;
}

unsigned short CParty::getStraightVictories()
{
    return m_straightVictories;
}

void CParty::setStraightVictories(unsigned short v)
{
    m_straightVictories = v;
}

void CParty::incStraightVictories()
{
    m_straightVictories += 1;
}

void CParty::resetStraightVictories()
{
    m_straightVictories = 0;
}

int CParty::getStartGamePartyCount()
{
    return m_startGamePartyCount;
}

void CParty::setStartGamePartyCount(int v)
{
    m_startGamePartyCount = v;
}

bool CParty::IsFirstMapClear()
{
    return m_firstMapClear;
}

void CParty::SetFirstMapClear(bool flag)
{
    m_firstMapClear = flag;
}

bool CParty::IsPremiumGoldCardParty()
{
    return m_premiumGoldCardParty;
}

void CParty::SetPremiumGoldCardParty()
{
    m_premiumGoldCardParty = 1;
}

void CParty::ResetPremiumGoldCardParty()
{
    m_premiumGoldCardParty = 0;
}

unsigned int CParty::GetPremiumGoldCardDefaultItem()
{
    return m_premiumGoldCardDefaultItem;
}

void CParty::SetPremiumGoldCardDefaultItem(unsigned int v)
{
    m_premiumGoldCardDefaultItem = v;
}

void CParty::ResetPremiumGoldCardDefaultItem()
{
    m_premiumGoldCardDefaultItem = 0;
}

void* CParty::GetSecretShopData()
{
    return &m_padShop;
}

bool CParty::checkBossRoom()
{   // ORIG W 0x0822d8f6：al 透传 check_end_point 返回值（check_allmember_loading
    // 以 && 消费，原 void 声明与 ORIG 不符）
    return reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->check_end_point();
}

bool CParty::checkStartRoom()
{   // ORIG 0x0826525e：al 直接透传 check_start_point 返回值
    return reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->check_start_point();
}

int CParty::DeleteDungeonDropItem(int idx)
{   // ORIG W 0x0860b62a：转发 CBattle_Field::pickup_item 并透传其返回值
    // （processRouting 以 `0 < 返回值` 判定删除失败）。
    return reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->pickup_item(idx);
}

int CParty::get_quick_party_index()
{
    return m_quickPartyIndex;
}

void CParty::set_quick_party_index(int idx)
{
    m_quickPartyIndex = idx;
}

bool CParty::is_quick_party()
{
    return m_isQuickParty;
}

void CParty::set_quick_party(bool flag)
{
    m_isQuickParty = flag;
}

void CParty::set_gm_random_buff_type(QuickParty::RandomBuffType type)
{
    m_randomBuffType = (int)type;
}

int CParty::get_random_buff_type()
{
    return m_field1ae8;
}

bool CParty::IsWeekendEvent()
{
    return m_weekendEvent;
}

void CParty::SetWeekendEvent(bool flag)
{
    m_weekendEvent = flag;
}

bool CParty::getDungeonMapSaving()
{
    return m_dungeonMapSaving;
}

void CParty::setDungeonMapSaving(bool flag)
{
    m_dungeonMapSaving = flag;
}

// ORIG 0x085be39a（_ZN6CParty25checkFreeRevivalConditionEPK8CDungeon）：
// 1) m_partyType==1 → true；2) dungeon 为空时按战场 dungeon_index 找回；
// 3) CDungeon::checkFreeRevivalCondition(diff) 命中 → true；
// 4) 否则需维度副本（get_dimension_possible>0）且重复事件槽 0x5e 在进行中，
//    再按 diff 查 CDimensionActivationEvent::checkFreeRevivalCondition。
bool CParty::checkFreeRevivalCondition(CDungeon const* dungeon)
{
    if (m_partyType == 1)
        return true;

    CDungeon* dg = const_cast<CDungeon*>(dungeon);
    if (dg == 0)
    {
        int idx = reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_index();
        dg = G_CDataManager()->find_dungeon(idx);
        if (dg == 0)
            return false;
    }

    int diff = reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_diff();
    if (dg->checkFreeRevivalCondition(diff))
        return true;

    if (dg->get_dimension_possible() <= 0)
        return false;

    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5e);
    if (event == 0)
        return false;
    if (!event->IsEventing(0))
        return false;

    return static_cast<CDimensionActivationEvent*>(event)->checkFreeRevivalCondition(diff);
}

void* CParty::GetPartyTelePort()
{
    return &m_pad1ad0;
}

void* CParty::getPassedMapInfoList()
{
    return &m_padPassedMap;
}

int CParty::GetUsedCoinCount()
{
    return m_usedCoinCount;
}

void CParty::SetUsedCoinCount(int v)
{
    m_usedCoinCount = v;
}

void CParty::IncreaseUsedCoinCount()
{
    m_usedCoinCount++;
}

int CParty::Get_party_overlapped_drop_ratio_rare() const
{
    return m_overlapRare;
}

int CParty::Get_party_overlapped_drop_ratio_unique() const
{
    return m_overlapUnique;
}

void CParty::ClearMapHitCount()
{
    m_mapHitCount = 0;
    m_mapHitSum = 0;
    memset(m_mapHitList1, 0, 0xb4);
    m_mapHitSum2 = 0;
    memset(m_mapHitList2, 0, 0xb4);
    m_mapHitSum3 = 0;
    memset(m_mapHitList3, 0, 0xb4);
    m_field1aa0 = 0;
    memset(m_field1aa4, 0, 0x10);
    m_field1ab4 = 0;
    m_field1ab8 = 0;
}

bool CParty::IsRoutingState()
{
    if ((char)m_routingData.GetRoutingState())
        return true;
    else
        return false;
}

void CParty::SetStartRouting()
{
    if ((char)m_routingData.GetRoutingState() != 1)
        m_routingData.SetRoutingState(1);
}

void CParty::SetEndRouting()
{
    m_routingData.Reset();
}

void CParty::SetRoutingItem(Inven_Item& item, unsigned int idx)
{
    m_routingData.SetRoutingItemIndex(item, idx);
}

int CParty::GetRoutingItemID()
{
    return m_routingData.GetRoutingItemIndex();
}

Inven_Item* CParty::GetRoutingInvenItem()
{
    return m_routingData.GetRoutingInvenItem();
}

unsigned int CParty::GetRoutingItemUniqueID()
{
    return m_routingData.GetRoutingItemUniqueID();
}

char CParty::GetMemberRoutingState(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser == user)
            return m_routingData.GetMemberRoutingState(i);
    }
    return (char)0xff;
}

void CParty::SetMemberRoutingState(CUser* user, char state)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser == user)
        {
            m_routingData.SetMemberRoutingState(i, state);
            break;
        }
    }
}

void CParty::SetRoutingTimerOver()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (m_routingData.GetMemberRoutingState(i) == 0)
            m_routingData.SetMemberRoutingState(i, 2);
    }
}

// ============================================================================
// 生命周期
// ============================================================================
void CParty::init_quick_party_data()
{
    m_quickPartyIndex = -1;
    m_field1ae8 = 0xb;
}

CUser* CParty::getManager()
{
    return m_manager;
}

void CParty::init()
{
    reinterpret_cast<CTraceMobDieHack*>(&m_pad210)->reportHackInfo();
    reinterpret_cast<CTraceMobDieHack*>(&m_pad210)->reset();
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        m_member[i].m_pUser->getHackAnalyzer()->reportHackInfo();
        m_member[i].m_pUser->getHackAnalyzer()->resetHackInfo();
    }
    m_memberLevelGap = -1;
    m_recvResultFlag = 0;
    memset(m_title, 0, 0x20);
    m_titleIndex = get_rand_int(0xff) + 1;
    m_userMax = 4;
    m_dungIndex = (unsigned short)0xffff;
    m_dungDiffi = 0xff;
    m_field68 = 0;
    m_state = 0;
    m_singlePlay = false;
    m_autoCreated = 0;
    SetEPLPState(0);
    m_selectedEPLPCmd = (char)0xff;
    SetAssaultState(0);
    for (int i = 0; i <= 3; ++i)
    {
        m_member[i].Init();
        for (int j = 0; j <= 1; ++j)
            m_field204[j][i] = (char)0xff;
        m_dungeonClearState = 0;
    }
    for (int i = 0; i <= 1; ++i)
        for (int j = 0; j <= 3; ++j)
            m_field1e4[i][j] = 0;
    reinterpret_cast<BattleData*>(&m_padBattleData)->SetHellPartyValueTotal(
        *(int*)((char*)G_CDataManager()->get_hellparty_script_values() + 8));
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->reset_hell_party_value();
    reinterpret_cast<BattleData*>(&m_padBattleData)->Reset();
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->reset_field();
    reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)->Clear();
    m_usedCoinCount = 0;
    m_hellWorldMap = 0;
    m_straightVictories = 0;
    reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop)->clear();
    ResetPremiumGoldCardParty();
    ResetPremiumGoldCardDefaultItem();
    m_weekendEvent = 0;
    m_randomBuffType = 0xb;
    init_quick_party_data();
    m_tournamentDungeonClearState = 0;
    m_dungeonMapSaving = 0;
}

void CParty::destroy()
{
    if (m_autoCreated)
    {
        PacketGuard packet;
        packet.put_header(0, 9);
        packet.put_short(1);
        packet.put_short(GetPartyIndex());
        packet.put_byte(3);
        packet.finalize(true);
        m_manager->SendPacket((CUser::eSendTarget)1, packet);
    }
    else
    {
        G_GameWorld()->send_party_info_to_all(this, 3);
    }
    init();
    G_CGameManager()->PutParty(this);
    m_usedCoinCount = 0;
}

void CParty::SetEPLPState(char state)
{
    m_eplpState = state;
    if (state == 2)
    {
        		if (m_dungeon != 0)
		{
			for (int i = 0; i <= 3; ++i)
			{
				if (!_checkValidUser(i))
					continue;
                int idx = m_dungeon->get_index();
                GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(idx, false);
            }
        }
    }
}

// ============================================================================
// 批次 A 补全（ORIG 0x85a932a / 0x85a21ca / 0x85b8f92 等；语义见 CParty.md）
// ============================================================================

void CParty::check_passive_skill()
{
}

bool CParty::isHelpAbuseParty(CUser* user)
{
    return false;  // ORIG T 0x085a21ca（0xa 字节）：恒 0。
}

// ORIG T 0x085a21d4（0x71 字节）：任一有效成员 isHelpAbuseParty 则真。
bool CParty::isHelpAbuseParty()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
        {
            continue;
        }
        CUser* user = get_user(i);
        if (isHelpAbuseParty(user))
        {
            return true;
        }
    }
    return false;
}

int CParty::MemberPenaltyRate()
{
    return 1;
}

int CParty::GetPartyMemberCoinLimit()
{
    return m_partyMemberCoinLimit;
}

void CParty::SetPartyMemberCoinLimit(int limit)
{
    m_partyMemberCoinLimit = limit;
}

int CParty::getStandardDimensionLevel()
{
    return m_standardDimensionLevel;
}

bool CParty::isTournamentVictory()
{
    return m_tournamentVictory;
}

char CParty::GetTournamentDungeonClearState()
{
    return m_tournamentDungeonClearState;
}

void CParty::SetTournamentDungeonClearState(char state)
{
    if (state <= 3)
    {
        m_tournamentDungeonClearState = state;
    }
}

int CParty::set_host()
{
    m_host = m_manager;
    return 1;
}

// ============================================================================
// STGainedEachExpData / getMonsterTotalExp / sumGainedEachExp
// ============================================================================
// ORIG _ZN19STGainedEachExpDataC1Ev @ 0x08151a56（W，C2 同址）：
//   movl $0x0,(%eax)           → m_exp = 0
//   movl $0x3f800000,0x4(%eax) → m_expPartyDivisor = 1.0f（0x3f800000）
// ORIG 无 D1/D2（平凡析构）。
STGainedEachExpData::STGainedEachExpData()
    : m_exp(0), m_expPartyDivisor(1.0f)
{
}

// ----------------------------------------------------------------------------
// CParty::getMonsterTotalExp @ 0x085a23dc（_ZN6CParty18getMonsterTotalExpEjciffb）
// 逐指令转写（docs/class_func_reports/CParty/getMonsterTotalExp.md + objdump）：
//   base = G_CDataManager() 返回值 + 0x68；
//   flag(char,有符号) <= 3 时 rate = *(float*)(base + 4*(flag+0xfc))，
//   否则 rate = 1.0f（0x3f800000 立即数）；
//   bool d 非 0：rate *= *(float*)0x08cbbbd0（字节 00 00 40 40 = 3.0f）；
//   x87 链（fildll/fdivrp/fmuls 就近舍入 fistpll）：
//     (double)(unsigned long long)exp / 2.0f（*(float*)0x08cbbbd4，
//     字节 00 00 00 40 = 2.0f）
//       * 参数 c(0x1c(%ebp)) * *(float*)(base + 4*(a+0xe4) + 8) * b * rate
unsigned int CParty::getMonsterTotalExp(unsigned int monsterIdx, char flag, int a,
                                        float b, float c, bool d)
{
    // ORIG base = G_CDataManager()+0x68 == &CDataManager::m_serverParameter
    //（CDataManager.h 权威布局 +0x68）。
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;
    float rate;
    if (flag <= 3)
    {
        // ORIG 地址 = &sp + (flag+0xfc)*4 = &m_flagExpRate3f0[flag]
        //（ServerParameterScript.h +0x3f0；flag<0 时与 ORIG 同址前越界读取）。
        rate = sp.m_flagExpRate3f0[static_cast<int>(flag)];
    }
    else
    {
        rate = 1.0f;  // .text 立即数 0x3f800000
    }
    if (d)
    {
        // .rodata 0x08cbbbd0: 00 00 40 40 → float 3.0f
        rate *= 3.0f;
    }
    // ORIG 经验表地址 = &sp + (a+0xe4)*4 + 8 = &m_monsterExpTable398[a]（+0x398）。
    long long total = static_cast<long long>(
        static_cast<double>(static_cast<unsigned long long>(monsterIdx)) / 2.0f *
        c * sp.m_monsterExpTable398[a] * b * rate);
    return static_cast<unsigned int>(total);
}

// ----------------------------------------------------------------------------
// CParty::sumGainedEachExp @ 0x085a2488（_ZN6CParty16sumGainedEachExpEP5CUserRK19STGainedEachExpData）
// 逐段转写（docs/class_func_reports/CParty/sumGainedEachExp.md + objdump）。
// 注：ORIG CUser::get_growth_power_exp_reward 为 sret 结构返回；本工程内该函数
// 以 int 返回同一 dword（CUser.cpp），故按 (v>>8)&0xff 取原结构 +1 字节，语义等价。
// CUser::getAddExpRateFromPremium(type)（ORIG W 0x085bfd76）=
// GetAddExpBonus(type)（ORIG W 0x085bf940）：type∈[0,0x69] 时返回
// g_SPremiumInfo[type].m_field44（表基 0x09483a84-0x09483a40=+0x44），否则 0；
// 该转发链尚未在本工程建模为成员函数，此处就地取权威表字段，不引入新 undefined。
// ----------------------------------------------------------------------------
unsigned int CParty::sumGainedEachExp(CUser* user, STGainedEachExpData const& data)
{
    int total = 0;
    unsigned int baseExp = static_cast<unsigned int>(data.m_exp);
    float expDivisor = data.m_expPartyDivisor;      // -0x30(%ebp)，fdivs 除数
    int fatigueBonus = 0;                           // -0x2c
    int seriaBonus = 0;                             // -0x28
    int weekendBonus = 0;                           // -0x24

    if (!reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->isWorldMapTowerArea())
    {
        // fildll(exp)*fldl 0x08cbbbd8（33 33 33 33 33 33 d3 3f = double 0.3）→ fistpl
        if (user->getCurCharacFatigueGrownUpBuff() != 0)
        {
            fatigueBonus = static_cast<int>(
                static_cast<double>(static_cast<unsigned long long>(baseExp)) * 0.3);
            total += fatigueBonus;
            user->set_charac_fatigue_buf_bonus_exp(fatigueBonus);
        }

        // 成长之力：ratio = 字节1(v>>8&0xff) / 2.0f（flds 0x08cbbbcc = 2.0f）
        int growthReward = user->get_growth_power_exp_reward();
        float seriaRatio =
            static_cast<float>((growthReward >> 8) & 0xff) / 2.0f;
        // fildll(exp)*fmuls(ratio) → fistpl
        seriaBonus = static_cast<int>(
            static_cast<double>(static_cast<unsigned long long>(baseExp)) *
            seriaRatio);
        user->set_charac_seria_buf_bonus_exp(seriaBonus);
        total += seriaBonus;

        CEventBase* repeatEvent =
            GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (repeatEvent->IsEventing(static_cast<CUser*>(0)))
        {
            // ORIG *(int*)(G_CDataManager()+0x520) == ServerParameterScript+0x4b8
            //   = stWeekendBouns.m_bonusExpPercent（importServerParameterScript.md
            //   0x8a8b95b "[weekend bonus exp]" mov %eax,0x4b8(%edx) 实证）。
            int weekendPercent =
                G_CDataManager()->m_serverParameter.m_weekendBouns4b4.m_bonusExpPercent;
            user->setWeekendBonusExp(baseExp, weekendPercent);
            weekendBonus = user->getWeekendBonusExp();
            total += weekendBonus;
        }

        if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
        {
            // getAddExpRateFromPremium(0x4f)：见文件头注释（g_SPremiumInfo[0x4f].m_field44）
            total += user->setPremiumBonusExp(
                baseExp, g_SPremiumInfo[0x4f].m_field44);
        }
    }

    // 尾段（两路径汇合 +0x1d2）：
    float premiumRate = 0.0f;                       // -0x20
    premiumRate = static_cast<float>(
        user->GetPremiumInfo()->GetAdvantageExpRate());
    premiumRate += static_cast<float>(CServerEvent::GetExpRate());

    // fildll(E); fildll(E); fmuls(premRate); flds 2.0f; fdivrp; faddp → fistpll
    long long newExp64 = static_cast<long long>(
        static_cast<double>(static_cast<unsigned long long>(baseExp)) *
            premiumRate / 2.0f +
        static_cast<double>(static_cast<unsigned long long>(baseExp)));
    unsigned int newExp = static_cast<unsigned int>(newExp64);  // 仅低 32 位回写
    total += static_cast<int>(newExp);

    // fildll(newExp)/fdivs(expDivisor) → fistpl（RC=就近舍入）；party 加成为差额
    int partyBonus = static_cast<int>(newExp) -
                     static_cast<int>(
                         static_cast<double>(static_cast<unsigned long long>(newExp)) /
                         expDivisor);
    user->set_charac_party_bonus_exp(partyBonus);

    total += user->gainExpAsUpperMember(static_cast<int>(newExp));
    total += user->gainGuildSkillExp(static_cast<int>(newExp));
    total += user->gainPowerWarRewardExp(static_cast<int>(newExp));
    return static_cast<unsigned int>(total);
}

// ============================================================================
// change_manager / IsExistInvisible / battle_data_reset / 发送族
// ============================================================================
// CParty::change_manager @ 0x859c832 —— 清空旧 manager 槽位，选举新 manager。
// 返回 1=成功 / 0=队伍已空（写日志）。
int CParty::change_manager()
{
    int i;
    for (i = 0; i <= 3; ++i)
    {
        if (m_member[i].m_pUser == m_manager)
        {
            m_manager->SetPartyIndex(-1);
            m_manager->set_state(ch_state3);
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            break;
        }
    }
    for (i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            m_manager = m_member[i].m_pUser;
            set_host();
            send_host_info();
            return 1;
        }
    }
    LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                          0x689, "there is no manager(party empty)");
    return 0;
}

// ==== CParty::change_manager @ 0x0859c978（两参版，change_manager() 无参版同族报告）====
// ORIG _ZN6CParty14change_managerEP5CUseri T：仅现任 manager 可换任；
// slot 越界/空槽→21、非本人→8、目标已是 manager→18；成功换 m_manager 并广播。
int CParty::change_manager(CUser* user, int slot)
{
    if (slot < 0 || 3 < slot)
        return 0x15;
    if (m_member[slot].m_pUser == 0)
        return 0x15;
    if (user != m_manager)
        return 8;
    if (m_member[slot].m_pUser == m_manager)
        return 0x12;
    m_manager = m_member[slot].m_pUser;
    set_host();
    send_host_info();
    G_GameWorld()->send_party_info_to_all(this, 2);
    return 0;
}

// CParty::IsExistInvisible @ 0x85b6410 —— 任一有效成员当前角色不可见则 true。
bool CParty::IsExistInvisible()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && !m_member[i].m_pUser->IsCurCharacVisible())
            return true;
    }
    return false;
}

// CParty::battle_data_reset @ 0x859aec2 —— 副本战斗数据与快捷队伍状态复位。
void CParty::battle_data_reset()
{
    reinterpret_cast<CTraceMobDieHack*>(&m_pad210)->reportHackInfo();
    reinterpret_cast<CTraceMobDieHack*>(&m_pad210)->reset();
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        m_member[i].m_pUser->getHackAnalyzer()->reportHackInfo();
        m_member[i].m_pUser->getHackAnalyzer()->resetHackInfo();
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        m_member[i].m_pUser->set_state(ch_state3);
        m_member[i].m_pUser->set_charac_party_bonus_exp(0);
        m_member[i].m_pUser->set_charac_member_bonus_exp(0);
        m_member[i].m_pUser->set_charac_fatigue_buf_bonus_exp(0);
        m_member[i].m_pUser->set_charac_seria_buf_bonus_exp(0);
    }
    reinterpret_cast<BattleData*>(&m_padBattleData)->Reset();
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->reset_field();
    m_state = 1;
    SetEPLPState(0);
    SetSelectedEPLPCmd(-1);
    m_dungeonClearState = 0;
    m_memberLevelGap = -1;
    m_killMobCounter.clear();
    reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop)->clear();
    SetEndRouting();
    ResetPremiumGoldCardParty();
    ResetPremiumGoldCardDefaultItem();
    m_tournamentDungeonClearState = 0;
    init_quick_party_data();
    m_dungeonMapSaving = 0;
    reinterpret_cast<MapInfoVec*>(&m_padPassedMap)->clear();
}

// CParty::isHelpAbuseParty 依赖的 send_host_info @ 0x859cdd2。
void CParty::send_host_info()
{
    PacketGuard packet;
    packet.put_header(0, 0x1a);
    for (int i = 0; i <= 3; ++i)
    {
        if (m_member[i].m_pUser == m_host)
        {
            packet.put_byte(i);
            packet.finalize(true);
            send_to_party(packet);
            return;
        }
    }
}

// CParty::get_party_realtime_info @ 0x859cbac —— 实时队伍信息（始终返回 1）。
int CParty::get_party_realtime_info(PacketGuard& packet)
{
    int startIndex = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);
            const _Charac_info* ci = m_member[i].m_pUser->getCurCharacR();
            if (ci == 0)
                packet.put_byte(0);
            else
                packet.put_byte((int)*(unsigned char*)((char*)ci + 0x75));  // ORIG +0x75
            packet.put_byte(isHelpAbuseParty(m_member[i].m_pUser));
            packet.put_byte(i);
            ++count;
        }
    }
    packet.put_byte(startIndex, count);
    return 1;
}

// CParty::send_party_realtime_info @ 0x859cd24。
void CParty::send_party_realtime_info()
{
    PacketGuard packet;
    packet.put_header(0, 0x99);
    if (get_party_realtime_info(packet))
    {
        packet.finalize(true);
        send_to_party(packet);
    }
}

// CParty::send_quick_party_matching_result @ 0x859b73e。
void CParty::send_quick_party_matching_result(bool flag, CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x17e);
    packet.put_byte(flag);
    packet.finalize(true);
    if (get_member_count() > 2 && user != 0)
        user->Send(packet);
    else
        send_to_party(packet);
}

// ORIG 反汇编为 for(i=0; i<=3; ++i) 结构（初始化后直接 jmp 到尾部条件测试）。

// CParty::CheckEnterEventDungeon @ 0x859ee66 —— 事件地下城条件（必备/奖励材料）。
bool CParty::CheckEnterEventDungeon(CDungeon const* dungeon, unsigned char& flag,
                                    Inven_Item* items, int* param)
{
    if (((dungeon->m_entryRequiredItem2.m_itemIdx != -1) ||
         (dungeon->m_entryRequiredItem.m_itemIdx != -1)) &&
        ((dungeon->m_entryRequiredItem2.m_itemIdx == -1) ||
         ((dungeon->m_entryRequiredItem2.m_enableFlag == 1) ||
          (dungeon->m_entryRequiredItem.m_itemIdx == -1) ||
          (dungeon->m_entryRequiredItem.m_enableFlag == 1))))
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i) != 1)
                continue;
            char local_d = 1;
            int* pi = &param[i];
            *pi = m_member[i].m_pUser->getCurCharacInvenR()
                      ->check_item_exist(dungeon->m_entryRequiredItem2.m_itemIdx);
            if (*pi == -1)
            {
                flag = (unsigned char)i;
                local_d = 0;
            }
            else
            {
                Inven_Item* piv = &items[i];
                *piv = m_member[i].m_pUser->getCurCharacInvenR()
                           ->GetInvenSlot(1, *pi);
                if (piv->m_addInfo2 < dungeon->m_entryRequiredItem2.m_itemCount)
                {
                    flag = (unsigned char)i;
                    local_d = 0;
                }
            }
            if (local_d != 1)
            {
                local_d = 1;
                pi = &param[i];
                *pi = m_member[i].m_pUser->getCurCharacInvenR()
                          ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
                if (*pi == -1)
                {
                    flag = (unsigned char)i;
                    local_d = 0;
                }
                else
                {
                    Inven_Item* piv = &items[i];
                    *piv = m_member[i].m_pUser->getCurCharacInvenR()
                               ->GetInvenSlot(1, *pi);
                    if (piv->m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
                    {
                        flag = (unsigned char)i;
                        local_d = 0;
                    }
                }
            }
            if (local_d != 1)
                return 0;
        }
    }
    return 1;
}

// CParty::checkInoutConditionDungeon @ 0x85ac59c —— 进入副本前的进出条件判定，
// 返回 RetryFailReason 值（10 无副本 / 10 禁止进入 / 1 疲劳 / 2 材料 / 4 维度 /
// 7 其它 / 8 封印门 / 9 可进入）。
// 依赖若干 CParty / CWorldMap / CWorldMapList 方法（ORIG 符号已声明，定义桩由
// 并行批次提供）：CheckEnterDimensionDungeon / check_dungeon_start /
// CheckEnterVillageAttackRevenge / CheckDestroyConditionSealDoor /
// CheckMemberFatigue / CWorldMapList::find_world_map /
// CWorldMap::IsInHellDungeon / CWorldMap::hasDeathTower。
int CParty::checkInoutConditionDungeon()
{
    if (m_dungeon == 0)
        return 10;
    if (GetEPLPState() <= (char)1)
        return 10;
    for (int i = 0; i < 4; ++i)
    {
        CUser* u = get_user(i);
        if (u == 0 || (char)m_dungeon->get_blood_dungeon_type() > 0)
            continue;
        if (u->getCurCharacTotalFatigue() ==
            u->getCurCharacTotalMaxFatigue())
            return 1;
    }
    unsigned char flag = 0;
    Inven_Item items[4];
    int param[4] = { 0, 0, 0, 0 };
    memset(items, 0, sizeof(items));
    if (CheckEnterAncientDungeon(m_dungeon, flag, items, param) == 1)
    {
        int r = CheckEnterDimensionDungeon(m_dungeon);
        if (r == 0xab || r == 0xad)
            return 4;
        r = check_dungeon_start(m_dungeon);
        if (!(r != 10 && (r < 10 || 4 < r - 0xf3u)))
            return 7;
        if (CheckEnterVillageAttackRevenge(m_dungeon) != 0)
            return 7;
        CWorldMap* worldMap = 0;
        int worldIdx = G_GameWorld()->GetWorldMapIndex(m_manager);
        if (worldIdx > 0)
        {
            worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        }
        CBattle_Field& bfView = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
        bool hellDun = false;
        if ((worldMap != 0 &&
             worldMap->IsInHellDungeon() != 0 &&
             bfView.IsEnableHellDungeon() != 0) &&
            bfView.m_hellPartyMode == 1)   // ORIG this+0xcd4（BF+0x1b0 m_hellPartyMode）
        {
            hellDun = true;
        }
        if (hellDun)
        {
            std::vector<int> vec;
            int sealR = CheckDestroyConditionSealDoor(
                m_dungeon, vec, worldMap->m_field34);
            if (sealR == 1)
                m_sealDoorFlag = 1;
            else
                m_sealDoorFlag = 0;
            if (sealR != 1)
                return 8;
        }
        bool noFat = true;
        if (m_partyType == 1 ||
            (worldMap != 0 && worldMap->hasDeathTower() == 1) ||
            m_dungeon->isNoFatigueDungeon() == 1 ||
            m_dungeon->m_advanceAltarSurvivalFlag == 1)
        {
            noFat = false;
        }
        if (noFat && CheckMemberFatigue() > 0)
            return 1;
        return 9;
    }
    return 2;
}

// ============================================================================
// 链接阻断方法（ORIG T/W）：6 个 CParty 方法 + depended helper（主 agent 授权一并实现）
// ============================================================================

// 全局：getHellPartyEntranceItemNeedCount（ORIG 0x8891be6，非成员函数）
//   汇编：eax = 40*n - 0x708；除以 100（magic 0x51eb851f, sar 5）；+10。
int getHellPartyEntranceItemNeedCount(int n)
{
    return ((40 * n - 0x708) / 100) + 10;
}

int CParty::CheckMemberFatigue()
{
    // ORIG 0x859d5e8：遍历 4 成员，返回"有效且未疲劳"的第一个槽位+1，否则 0。
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser &&
            !m_member[i].m_pUser->CheckFatigue())
            return i + 1;
    }
    return 0;
}

void CParty::sendInoutConditionDungeon(RetryFailReason::T reason)
{
    // ORIG 0x85ac95e：reason==10 不发；否则 PacketGuard header(0,0x104)+put_byte(reason)+send_to_party。
    if ((int)reason == 10)
        return;
    PacketGuard packet;
    packet.put_header(0, 0x104);
    packet.put_byte((int)reason);
    packet.finalize(true);
    send_to_party(packet);
}

// ORIG 0x859f6e2：遍历 4 成员，任一有效用户是 GM → 返回 1；否则 ServerParameterScript::isDungeonOpen(idx)。
int CParty::checkDugeonInoutTime(int dungeonIndex)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser &&
            m_member[i].m_pUser->isGMUser())
            return 1;
    }
    ServerParameterScript* sps = &G_CDataManager()->m_serverParameter;
    return sps->isDungeonOpen(dungeonIndex) ? 1 : 0;
}

// ORIG 0x859f75e：进入血腥地下城入场费 = lottoCost + price + (inc * price) / 100。
//   price = ServerParameterScript::GetPriceAverage(charac_level)
//   inc   = ServerParameterScript::GetAdmissionIncrease(CCharacBloodDungeon::GetEnterCount)
//   lotto = ServerParameterScript::GetLottoCost()
int CParty::get_blood_dungeon_admission_fee(CUser* user)
{
    int enterCount =
        *(int*)((char*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1) + 0x18);
    int level = user->get_charac_level();
    ServerParameterScript* sps = &G_CDataManager()->m_serverParameter;
    int price = sps->GetPriceAverage(level);
    int inc = sps->GetAdmissionIncrease(enterCount);
    int lotto = sps->GetLottoCost();
    return lotto + price + (inc * price) / 100;
}

int CParty::check_dungeon_start(CDungeon const* dungeon)
{
    // ORIG 0x859fb16
    int dtype = dungeon->get_blood_dungeon_type();
    if (dtype == 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            CUser* u = m_member[i].m_pUser;
            int fee = get_blood_dungeon_admission_fee(u);
            if (u->getCurCharacInvenR()->get_money() < fee)
                return 10;
        }
    }
    else if (dtype == 2)
    {
        if (checkDugeonInoutTime(dungeon->get_index()) != 1)
            return 0xf4;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
            return 0xf6;
        if (GlobalData::s_villageMonsterMgr->OnEvent())
            return 0xf7;
        int memberCount = get_member_count();
        if (dungeon->get_limit_party_count() < memberCount)
            return 0xf3;
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            char* charac = (char*)m_member[i].m_pUser->getCurCharacR();
            if (*(char*)(charac + 0xec2) < '\x01' && m_partyType != 1)
                return 0xf5;
        }
    }
    return 0;
}

int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon)
{
    // ORIG 0x859f3fc
    if (m_dungeon == 0 || m_dungeon->get_dimension_possible() <= 0)
        return 0;
    int memberCount = get_member_count();
    int dimPossible = dungeon->get_dimension_possible();
    int dimIdx = (char)dimPossible - 1;
    DimensionPartyCount dpc;
    dpc.m_0 = (char)m_partyType;
    dpc.m_1 = m_fieldCC4;
    dpc.m_2 = -1;
    if (dungeon->get_dimension_member_count(dpc))
    {
        if (memberCount < dpc.m_2)
            return 0xab;
    }
    else
    {
        if (memberCount < dungeon->get_dimension_min_partymem())
            return 0xab;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        char* charac = (char*)m_member[i].m_pUser->getCurCharacR();
        if (*(char*)(charac + 0xeb9 + dimIdx) < '\x01' &&
            m_partyType != 1 &&
            !m_member[i].m_pUser->isGMUser())
            return 0xad;
    }
    return 0;
}

// ORIG 0x85ba804：村庄攻击/复仇模式进入检查。
int CParty::CheckEnterVillageAttackRevenge(CDungeon const* dungeon)
{
    if (dungeon->m_villageAttackEventFlag == 0)
        return 0;
    int memberCount = get_member_count();
    if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
        return 0x15;
    if (dungeon->m_maxAttackPartyMember < memberCount || memberCount < dungeon->m_minAttackPartyMember)
        return 0xab;
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
            continue;
        CUser* u = get_user(i);
        unsigned char cur = u->GetCurRevengeDungeonCount();
        if (cur == 0 || (unsigned char)dungeon->m_maxRevengeCount < cur)
        {
            u->SetCurRevengeDungeonCount(0);
            return 0xad;
        }
    }
    return 0;
}

// ORIG 0x85bc86e：地狱肉鸽免票检查/扣除。
//   worldMap = this->m_hellWorldMap（缓存）；worldMap->m_hellPassItemList 为 pair<int,int> 列表。
bool CParty::CheckHellDungeonFreepassItemHaveAndDel(CUser* user, bool del)
{
    if (m_hellWorldMap == 0)
    {
        int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (0 < worldIdx)
            m_hellWorldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        if (m_hellWorldMap == 0)
            return false;
    }
    int slotResult = 0;
    for (unsigned int k = 0; k < m_hellWorldMap->m_hellPassItemList.size(); ++k)
    {
        int itemId = m_hellWorldMap->m_hellPassItemList[k].first;
        slotResult = user->getCurCharacInvenR()->check_item_exist(itemId);
        if (slotResult == -1)
            continue;
        if (!del)
            break;   // 仅检查存在性
        int count = m_hellWorldMap->m_hellPassItemList[k].second;
        if (user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slotResult, count, (eItemDelReason)0x17, true) == 1)
        {
            user->SendUpdateItem((CUser::eSendTarget)1,
                                 (ENUM_ITEMSPACE)0, slotResult);
            break;
        }
        // 删除失败：记录（ORIG 走 cMyTrace 日志，此处跳过日志对象，保留语义）
    }
    return 0 < slotResult;
}

int CParty::CheckDestroyConditionSealDoor(CDungeon const* dungeon,
                                          std::vector<int>& list1,
                                          std::vector<int>& list2)
{
    // ORIG 0x85a7d86
    int validCount = 0;      // local_20：有效成员数
    int passCount = 0;       // local_24：满足条件成员数
    int result = 0;          // local_11
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        ++validCount;
        CUser* u = m_member[i].m_pUser;
        if (CheckHellDungeonFreepassItemHaveAndDel(u, false) == 0)
        {
            if (dungeon->get_min_level() < 0x2d)
                return 0;
            int needCount = getHellPartyEntranceItemNeedCount(dungeon->get_min_level());
            int haveCount = 0;
            for (unsigned int k = 0; k < list2.size(); ++k)
            {
                int slot = u->getCurCharacInvenR()->check_item_exist(list2[k]);
                if (slot != -1 &&
                    u->getCurCharacInvenR()->GetInvenSlot(1, slot).m_addInfo2 >= needCount)
                    ++haveCount;
            }
            if (haveCount == (int)list2.size())
                ++passCount;
            else
                list1.push_back(i);
        }
        else
        {
            ++passCount;
        }
    }
    if (validCount <= passCount)
        result = 1;
    return result;
}

bool CParty::check_timer_key(TIMER_MESSAGE msg, int param)
{
    // ORIG: lea -0xb(%eax),%edx; add $0x50,%edx; mov 0x8(%eax,%edx,4)
    struct Keys { char m_pad[8]; int m_k[64]; };
    return ((Keys*)this)->m_k[(int)msg - 0xb + 0x50] == param;
}

int CParty::gen_timer_key(TIMER_MESSAGE msg)
{
    // ORIG: sub $0xb,%eax; lea 0x50(%eax),%ecx; mov 0x8(%edx,%ecx,4)…
    struct Keys { char m_pad[8]; int m_k[64]; };
    ++((Keys*)this)->m_k[(int)msg + 0x45];
    return ((Keys*)this)->m_k[(int)msg + 0x45];
}

void CParty::resetFinishLoadingVar()
{
    for (int i = 0; i <= 3; i++)
    {
        m_finishLoadingFlag[i] = 0;
    }
}

bool CParty::CanDoPartyActionAssalutState()
{
    int state = (char)GetAssaultState();
    if (state <= 1 || state > 6)
        return true;
    return false;
}

void CParty::resetMemberUseCoin()
{
    for (int i = 0; i < 4; i++)
    {
        m_member[i].m_field14 = 0;
    }
}

void CParty::checkBloodRenewMyRecord(int a, int b, int c, int d)
{
    ((CUserCharacInfo*)m_member[a].m_pUser)->setBloodBestRecord(
        (unsigned int)b, c, d, false);
}

bool CParty::IsSelectCard(char a, eClearRewardCardType_t type)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_field204[(int)type][i] == a)
        {
            return true;
        }
    }
    return false;
}

bool CParty::_IsCannotConnectP2P(int a, int b)
{
    if (m_member[a].m_fielde[b] != 0)
    {
        if (_checkValidUser(b) != 1)
        {
            return 0;
        }
    }
    return 1;
}

int CParty::_GetConnectP2PPoint(int slot)
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (slot != i)
        {
            if (_IsCannotConnectP2P(slot, i) != 1)
            {
                count++;
            }
        }
    }
    return count;
}

bool CParty::check_allmember_die()
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (m_liveFlag[i] != 0)
        {
            return false;
        }
    }
    return true;
}

int CParty::get_live_count()
{
    int count = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (m_liveFlag[i] != 0)
        {
            count++;
        }
    }
    return count;
}

int CParty::get_live_count_enter_map()
{
    int count = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (m_enterMapFlag[i] != 0)
        {
            count++;
        }
    }
    return count;
}

void CParty::GetValidPartyMember(std::vector<int>& list)
{
    list.clear();
    for (int i = 0; i < 4; i++)
    {
        if (checkValidUser(i))
        {
            list.push_back(i);
        }
    }
}

bool CParty::IsExistUser(CUser* user)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_member[i].m_pUser == user)
        {
            return true;
        }
    }
    return false;
}

int CParty::_getMemberLevelGap()
{
    if (m_memberLevelGap == -1)
    {
        int minLevel = 99;
        int maxLevel = 0;
        for (int i = 0; i <= 3; i++)
        {
            if (_checkValidUser(i) != 1)
                continue;
            int level = ((CUserCharacInfo*)m_member[i].m_pUser)->get_charac_level();
            minLevel = std::min(minLevel, level);
            level = ((CUserCharacInfo*)m_member[i].m_pUser)->get_charac_level();
            maxLevel = std::max(maxLevel, level);
        }
        m_memberLevelGap = (maxLevel - minLevel) < 0 ? -(maxLevel - minLevel)
                                                     : (maxLevel - minLevel);
    }
    return m_memberLevelGap;
}

int CParty::getMemberLevelGap()
{
    return _getMemberLevelGap();
}

void CParty::increase_dungeon_coin_count(int param)
{
    if (m_dungeon == 0)
        return;
    if (m_dungeon->m_87c == -1)
        return;
    SetUsedCoinCount(param);
}

bool CParty::IsReturnUserParty()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (checkValidUser(i) != 1)
            continue;
        if (m_member[i].m_pUser != 0)
        {
            if (m_member[i].m_pUser->IsReturnUser())
            {
                return true;
            }
        }
    }
    return false;
}
void CParty::send_invite_bluemarble(int mapIdx)
{
    PacketGuard packet;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            packet.clear();
            packet.put_header(0, 7);
            packet.put_short(mapIdx);
            packet.put_byte(9);
            packet.put_int((unsigned short)m_member[i].m_pUser->get_unique_id());
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
}
int CParty::get_member_count()
{
    int count = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
            count++;
    }
    return count;
}
// ORIG W 0x08145882：getMemberNames = 转发 _getMemberNames（call 0x85b4bde）。
char* CParty::getMemberNames(char* buf)
{
    return _getMemberNames(buf);
}

// ORIG T 0x085b9e20：循环 i=0..3 读 (this+0x78+i*0x18) 的 CUser* 与 user 比较，
// 命中返回 i，否则 -1。
int CParty::GetUserPosition(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (*reinterpret_cast<CUser**>(reinterpret_cast<char*>(this) +
                                       0x78 + i * 0x18) == user)
            return i;
    }
    return -1;
}

 char* CParty::_getMemberNames(char* buf)
{
    char nameBuf[34];
    // ORIG 反汇编：m_manager==0 与 getCurCharacName() 结果直接作 sprintf 实参
    // （无中间变量，je/jmp 合并后 eax 直接进 0x8(%esp)）→ 用三元内联。
    sprintf(buf, "\"%s\",",
            (m_manager == 0) ? "NoMember" : m_manager->getCurCharacName());
    for (int i = 0; i <= 3; ++i)
    {
        if (i == 3)
        {
            if (_checkValidUser(i))
                sprintf(nameBuf, "\"%s\"", m_member[i].m_pUser->getCurCharacName());
            else
                sprintf(nameBuf, "\"%s\"", "N/A");
        }
        else
        {
            if (_checkValidUser(i))
                sprintf(nameBuf, "\"%s\",", m_member[i].m_pUser->getCurCharacName());
            else
                sprintf(nameBuf, "\"%s\",", "N/A");
        }
        strncat(buf, nameBuf, 0xff);
    }
    buf[0xff] = '\0';
    return buf;
}

bool CParty::_checkValidUser(int idx)
{
    if (m_member[idx].m_pUser != 0)
    {
        if ((int)m_member[idx].m_pUser->GetUID() == m_member[idx].m_field8 &&
            reinterpret_cast<CUserCharacInfo*>(m_member[idx].m_pUser)->getCurCharacR() != 0)
        {
            return 1;
        }
        char log1[16];
        int cState = (char)m_state;
        int pIndex = m_partyIndex;
        unsigned short dungIndex = m_dungIndex;
        CUser* manager = m_manager;
        new (log1) cMyTrace(__PRETTY_FUNCTION__, 0x37cf, 5);
        (*reinterpret_cast<cMyTrace*>(log1))(
            "[PARTYPOOL] m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
            manager, (unsigned int)dungIndex, pIndex, cState);
        for (int i = 0; i <= 3; ++i)
        {
            if (m_member[i].m_pUser == 0)
            {
                char log2[16];
                new (log2) cMyTrace(__PRETTY_FUNCTION__, 0x37e5, 5);
                (*reinterpret_cast<cMyTrace*>(log2))("User[%d] NULL uid : %d", i, m_member[i].m_field8);
            }
            else
            {
                char log3[16];
                int characNo = m_member[i].m_pUser->getCurCharacNo();
                int uid2 = m_member[i].m_field8;
                int uid = (int)m_member[i].m_pUser->GetUID();
                int chState = m_member[i].m_pUser->get_state();
                CUser* user = m_member[i].m_pUser;
                new (log3) cMyTrace(__PRETTY_FUNCTION__, 0x37da, 5);
                (*reinterpret_cast<cMyTrace*>(log3))(
                    "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
                    i, user, chState, uid, uid2, characNo);
            }
        }
        char log4[16];
        int uid = (int)m_member[idx].m_pUser->GetUID();
        unsigned int accId = m_member[idx].m_pUser->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        new (log4) cMyTrace(__PRETTY_FUNCTION__, 0x37ed, 5);
        (*reinterpret_cast<cMyTrace*>(log4))("User missmatched in party (m_id: %s)(uid: %d)", accIdStr, uid);
        m_member[idx].m_pUser->DisConnSig((DISCONN_SIG)0x17, false, 0);
        m_member[idx].Init();
    }
    return 0;
}
bool CParty::checkValidUser(int idx) { return _checkValidUser(idx); }
unsigned int CParty::GetMemberSlotNo(CUser const* user) const
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_member[i].m_pUser == user)
            return (unsigned int)i;
    }
    return 0xffffffff;
}
void CParty::Reset_party_overlapped_drop_ratio()
{
    m_overlapRare = 0;
    m_overlapUnique = 0;
    int cntA = 0;   // item1 == 0x5e7f
    int cntB = 0;   // item1 0x5e80..0x5e87 区间 / ==0x5ed9
    int cntC = 0;   // item2 0x5693..0x569a 区间 / item1==0x5ed9
    int cntD = 0;   // item3 0x4ecb..0x4ed2 区间 / item1==0x5ed9
    char cntE = 0;  // 累计命中数
    char flagF = 0; // cntE == 3
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (m_liveFlag[i] == 0)
            continue;
        cntE = 0;
        Inven_Item item1 =
            m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0x13);
        Inven_Item item2 =
            m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0x12);
        Inven_Item item3 =
            m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0x11);
        if (item1.m_addInfo > 0x5e7f && item1.m_addInfo <= 0x5e87)
        {
            cntB++;
            cntE++;
        }
        else if (item1.m_addInfo == 0x5e7f)
        {
            cntA++;
        }
        else if (item1.m_addInfo == 0x5ed9)
        {
            cntB++;
            cntE++;
        }
        if (item2.m_addInfo > 0x5692 && item2.m_addInfo <= 0x569a)
        {
            cntC++;
            cntE++;
        }
        else if (item1.m_addInfo == 0x5ed9)
        {
            cntC++;
            cntE++;
        }
        if (item3.m_addInfo > 0x4eca && item3.m_addInfo <= 0x4ed2)
        {
            cntD++;
            cntE++;
        }
        else if (item1.m_addInfo == 0x5ed9)
        {
            cntD++;
            cntE++;
        }
        if (cntE == 3)
        {
            flagF = 1;
        }
    }
    if (cntA != 0)
        m_overlapRare += cntA + 2;
    if (cntB != 0)
    {
        m_overlapRare += (cntB + 2) * 2;
        m_overlapUnique += cntB + 2;
    }
    if (cntC != 0)
    {
        m_overlapRare += (cntC + 2) * 2;
        m_overlapUnique += cntC + 2;
    }
    if (cntD != 0)
    {
        m_overlapRare += (cntD + 2) * 2;
        m_overlapUnique += cntD + 2;
    }
    if (flagF != 0)
    {
        m_overlapRare += 5;
        m_overlapUnique += 2;
    }
}
void CParty::ReturnToVillage()
{
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->setBloodState((ENUM_BLOOD_STATE)0);
    PacketGuard packet;
    packet.put_header(0, 2);
    if (m_autoCreated == 0)
    {
        packet.put_byte(0);
        packet.put_short(get_member_count());
        char names[0x100];
        memset(names, 0, 0x100);
        _getMemberNames(names);
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i))
            {
                if (m_dungeon != 0)
                {
                    int pt = m_partyType;
                    int diff = reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_diff();
                    const char* dn = m_dungeon->GetDungeonName();
                    m_member[i].m_pUser->m_historyLog.LeaveDungeon(dn, diff, names, pt);
                }
                if (m_dungeon != 0)
                {
                    bool b;
                    if (!IsSinglePlay() || !m_manager->isDungeonClearHackCheck())
                    {
                        b = false;
                    }
                    else
                    {
                        b = true;
                    }
                    if (b)
                    {
                        CUser* mgr = m_manager;
                        m_manager->getHackAnalyzer()->addServerHackCnt(
                            mgr, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
                    }
                }
                CUser* u = m_member[i].m_pUser;
                G_GameWorld()->out_from_dungeon(u);
                m_member[i].m_pUser->make_basic_info((char*)&packet, 0);
            }
        }
        set_state(1);
        battle_data_reset();
        packet.finalize(true);
        if (IsExistInvisible())
        {
            send_to_party(packet);
        }
        else
        {
            G_GameWorld()->send_all(packet);
        }
    }
    else
    {
        if (m_dungeon != 0)
        {
            char names2[0x100];
            memset(names2, 0, 0x100);
            int pt = m_partyType;
            char* n = _getMemberNames(names2);
            int diff = reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_diff();
            const char* dn = m_dungeon->GetDungeonName();
            m_manager->m_historyLog.LeaveDungeon(dn, diff, n, pt);
            bool b;
            if (!IsSinglePlay() || !m_manager->isDungeonClearHackCheck())
            {
                b = false;
            }
            else
            {
                b = true;
            }
            if (b)
            {
                CUser* mgr = m_manager;
                m_manager->getHackAnalyzer()->addServerHackCnt(
                    mgr, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
            }
        }
        CUser* mgr2 = m_manager;
        G_GameWorld()->out_from_dungeon(mgr2);
        packet.put_byte(0);
        packet.put_short(1);
        short partyIdx = m_manager->GetPartyIndex();
        int userState = m_manager->get_state();
        unsigned int accId = m_manager->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        char log[16];
        new (log) cMyTrace(__PRETTY_FUNCTION__, 0x2714, 0);
        (*reinterpret_cast<cMyTrace*>(log))(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            accIdStr, userState, (int)partyIdx);
        m_manager->SetPartyIndex(-1);
        if (m_manager->getMoveSpace() != (UserSpace::T)1)
        {
            m_manager->set_state(ch_state3);
        }
        m_manager->make_basic_info((char*)&packet, 0);
        destroy();
        packet.finalize(true);
        CUser* gm = getManager();
        if (gm->getMoveSpace() == (UserSpace::T)1)
        {
            send_to_party(packet);
        }
        else
        {
            G_GameWorld()->send_all(packet);
        }
    }
    SetEPLPState(0);
}
int CParty::leave_user(CUser* user, ENUM_PARTY_INFO_TYPE type)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "party.cpp",
            __PRETTY_FUNCTION__, 0x56d,
            "CParty::leave_user pUser is NULL");
        return 1;
    }
    ((CPartyTelePort*)GetPartyTelePort())->process_leave_user_at_teleport(user);
    if (reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(GetSecretShopData())->IsOpen())
    {
        int pos = user->GetUserPosInParty();
        reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(GetSecretShopData())->SetBuying(pos, false);
    }
    if (m_dungeon != 0)
    {
        char names[0x100];
        memset(names, 0, 0x100);
        int partyType = m_partyType;
        char* memberNames = _getMemberNames(names);
        int diff = reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_diff();
        const char* dname = m_dungeon->GetDungeonName();
        user->m_historyLog.LeaveDungeon(dname, diff, memberNames, partyType);
        bool b;
        if (!IsSinglePlay() || !user->isDungeonClearHackCheck())
        {
            b = false;
        }
        else
        {
            b = true;
        }
        if (b)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
        }
    }
    if (get_quick_party_index() != -1)
    {
        G_CGameManager()->CheckOutQuickParty(this, false);
    }
    if (is_quick_party())
    {
        send_quick_party_matching_result(false, user);
        if (get_member_count() == 2)
        {
            set_quick_party(false);
        }
    }
    int count = get_member_count();
    if (count < 2)
    {
        short partyIdx = user->GetPartyIndex();
        int userState = user->get_state();
        unsigned int accId = user->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        char log[16];
        new (log) cMyTrace(__PRETTY_FUNCTION__, 0x5b8, 0);
        (*reinterpret_cast<cMyTrace*>(log))(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            accIdStr, userState, (int)partyIdx);
        if (count < 1)
        {
            int cState = (char)m_state;
            int pIndex = m_partyIndex;
            unsigned short dIdx = m_dungIndex;
            LogManager::logFormat(1, "party.cpp",
                __PRETTY_FUNCTION__, 0x5c5,
                "[PARTYPOOL] member_count : %d, m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
                count, m_manager, (unsigned int)dIdx, pIndex, cState);
            for (int i = 0; i <= 3; ++i)
            {
                if (m_member[i].m_pUser == 0)
                {
                    LogManager::logFormat(1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x5cd,
                        "User[%d] NULL, ch_state NONE, uid : %d, charac_no : 0",
                        i, m_member[i].m_field8);
                }
                else
                {
                    LogManager::logFormat(1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x5d8,
                        "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
                        i, m_member[i].m_pUser, m_member[i].m_pUser->get_state(),
                        m_member[i].m_pUser->GetUID(), m_member[i].m_field8,
                        m_member[i].m_pUser->getCurCharacNo());
                }
            }
        }
        user->SetPartyIndex(-1);
        user->set_charac_party_bonus_exp(0);
        user->set_charac_member_bonus_exp(0);
        user->set_charac_fatigue_buf_bonus_exp(0);
        user->set_charac_seria_buf_bonus_exp(0);
        if (user->getMoveSpace() != (UserSpace::T)1)
        {
            user->set_state(ch_state3);
        }
        destroy();
        return 0;
    }
    else
    {
        if (user != 0)
        {
            user->getHackAnalyzer()->reportHackInfo();
            user->getHackAnalyzer()->resetHackInfo();
        }
        if (m_manager == user)
        {
            change_manager();
            G_GameWorld()->send_party_info_to_all(this, (int)type);
            send_party_realtime_info();
            sendInoutConditionDungeon((RetryFailReason::T)checkInoutConditionDungeon());
            return 0;
        }
        else
        {
            for (int i = 0; i <= 3; ++i)
            {
                if (_checkValidUser(i) != 1)
                    continue;
                if (m_member[i].m_pUser == user)
                {
                    m_member[i].m_pUser = 0;
                    m_member[i].m_field8 = 0;
                    user->SetPartyIndex(-1);
                    user->set_charac_party_bonus_exp(0);
                    user->set_charac_member_bonus_exp(0);
                    user->set_charac_fatigue_buf_bonus_exp(0);
                    user->set_charac_seria_buf_bonus_exp(0);
                    if (user->getMoveSpace() != (UserSpace::T)1)
                    {
                        user->set_state(ch_state3);
                    }
                    G_GameWorld()->send_party_info_to_all(this, (int)type);
                    send_party_realtime_info();
                    sendInoutConditionDungeon((RetryFailReason::T)checkInoutConditionDungeon());
                    return 0;
                }
            }
            return 1;
        }
    }
}
bool CParty::IsEventCharacParty()
{
    CEventBase* reward = GlobalData::s_event_manager->GetRepeatEvent(0xa2);
    bool b;
    if (reward == 0)
    {
        b = true;
    }
    else
    {
        char (*fn)(void*, int) = *(char(**)(void*, int))((*(int*)reward) + 0x34);
        b = fn(reward, 0) != 1;
    }
    if (b)
    {
        return 0;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (m_member[i].m_pUser == 0)
        {
            b = false;
        }
        else
        {
            int job = m_member[i].m_pUser->get_charac_job();
            b = reinterpret_cast<EventNewCharacterReward*>(reward)->isEventCharacter(job) != 0;
        }
        if (b)
        {
            return 1;
        }
    }
    return 0;
}
bool CParty::CheckEnterAncientDungeon(CDungeon const* dungeon, unsigned char& flag,
                                      Inven_Item* items, int* param)
{
    if (dungeon == 0)
        return 1;
    if (dungeon->isTowerOfDespairDungeon())
        return 1;
    if ((unsigned char)dungeon->m_entryRestrictFlag != 0)   // ORIG dungeon+0x85c（set_dungeon 自
                                              //   STDungeonScript+0x88 复制的事件副本标志）
    {
        if (CheckEnterEventDungeon(dungeon, flag, items, param) != 1)
            return 0;
        return 1;
    }
    if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
        return 1;
    if (dungeon->m_entryRequiredItem.m_enableFlag != 1)
        return 1;
    if (m_partyType == 1)
        return 1;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        int dungeonIdx = dungeon->get_index();
        if (dungeonIdx == 0x5dc)
        {
            param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                ->check_vilmark_dungeon_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
            if (param[i] == -1)
            {
                flag = (unsigned char)i;
                return 0;
            }
        }
        else
        {
            param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
            if (param[i] == -1)
            {
                flag = (unsigned char)i;
                return 0;
            }
        }
        items[i] = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(1, param[i]);
        if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
        {
            flag = (unsigned char)i;
            return 0;
        }
    }
    return 1;
}
bool CParty::UseAncientDungeonItems(CDungeon const* dungeon, Inven_Item* items, int* param)
{
    if (m_partyType != 1 && dungeon->m_entryRequiredItem.m_enableFlag != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i) != 1)
                continue;
            int itemIdx = dungeon->m_entryRequiredItem.m_itemCount;
            int slot = param[i];
            CInventory* inv = m_member[i].m_pUser->getCurCharacInvenW();
            char r = (char)inv->delete_item((INVEN_TYPE)1, slot, itemIdx,
                                            (eItemDelReason)0xe, true);
            if (r != 1)
            {
                int itemIdx2 = dungeon->m_entryRequiredItem.m_itemCount;
                int slot2 = param[i];
                int characNo = m_member[i].m_pUser->getCurCharacNo();
                char log[16];
                new (log) cMyTrace(
                    __PRETTY_FUNCTION__,
                    0xa60, 5);
                (*reinterpret_cast<cMyTrace*>(log))(
                    "CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
                    characNo, slot2, itemIdx2);
                return 0;
            }
            PacketGuard packet;
            packet.put_header(0, 0xe);
            packet.put_byte(0);
            packet.put_short(1);
            packet.put_short(param[i]);
            int diff = items[i].m_addInfo2 - dungeon->m_entryRequiredItem.m_itemCount;
            if (diff == 0)
            {
                packet.put_int(-1);
            }
            else
            {
                packet.put_int(items[i].m_addInfo);
            }
            packet.put_int(diff);
            packet.put_byte(items[i].GetItemAttr());
            packet.put_short(items[i].m_fieldb);
            packet.put_byte(items[i].m_field0);
            packet.put_int(items[i].m_fieldd);
            packet.put_byte(items[i].m_amp.getAbilityType());
            packet.put_short(items[i].m_amp.getAbilityValue());
            packet.put_byte(0);
            packet.put_packet(items[i]);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
            m_member[i].m_pUser->SaveInventory();
        }
    }
    return 1;
}

// ============================================================================
// CParty::set_charac_live —— ORIG 0x085b29bc（0x195 字节）
//   报告：docs/class_func_reports/CParty/set_charac_live.md
//   找到 user 槽位后按 state 写 +0x380(live)/+0x384(enterMap)/+0x388 标志；
//   state==1 时 m_member[i].m_field14(+0x8c) 计数 +1；随后发包 0x20 广播。
// ============================================================================
void CParty::set_charac_live(CUser* user, ENUM_USER_DUNGEON_STATE state)
{
    int slot = 0;
    while (true)
    {
        if (slot > 3)
            return;
        if (_checkValidUser(slot) && m_member[slot].m_pUser == user)
            break;
        ++slot;
    }

    if (state == ENUM_USER_DUNGEON_STATE_0)
    {
        m_liveFlag[slot] = 0;            // +0x380
    }
    else if (state >= ENUM_USER_DUNGEON_STATE_0 && state < (ENUM_USER_DUNGEON_STATE)3)
    {
        m_liveFlag[slot] = 1;            // +0x380
        m_enterMapFlag[slot] = 1;        // +0x384
        m_field388[slot] = 1;            // BattleData 第三标志组首字节
        if (state == (ENUM_USER_DUNGEON_STATE)1)
            m_member[slot].m_field14 += 1;   // ORIG：*(int*)(this+slot*0x18+0x8c) += 1
    }

    PacketGuard packet;
    packet.put_header(0, 0x20);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_byte(state);
    packet.finalize(true);
    send_to_party(packet);
}

// ============================================================================
// stClearRewardExpParameter_t / stClearRewardBonusExp_t 构造
//   ORIG W 0x0814a8b8 / 0x0814a800（逐 dword 清零；参数结构 +0x54/+0x55 字节
//   清零、+0x58/+0x5c 置 -1）。
// ============================================================================
stClearRewardExpParameter_t::stClearRewardExpParameter_t()
{
    m_deadPenaltyRate = 0.0f;
    m_rankBonusWeight = 0.0f;
    m_avatarBonus = 0.0f;
    m_creatureBonus = 0.0f;
    m_pcRoomBurningRate = 0.0f;
    m_expDoubleRate = 0.0f;
    m_channelBonusRate = 0.0f;
    m_burningFatigueBonusExp = 0;
    m_serverChannelRate = 0.0f;
    m_premiumAdvantageRate = 0.0f;
    m_serverEventExpRate = 0.0f;
    m_periodExpItemRate = 0.0f;
    m_globalEffectRate = 0.0f;
    m_pcRoomSameUserBonus = 0;
    m_manageBonus = 0.0f;
    m_returnUserPartyBonus = 0.0f;
    m_eventCharacExpRatio = 0.0f;
    m_growthPowerExpRate = 0.0f;
    m_premiumAddExpRate = 0.0f;
    m_reserved4c = 0;
    m_reserved50 = 0;
    m_reserved54 = 0;
    m_reserved55 = 0;
    m_deadMemberCount = -1;
    m_rankBonusIdx = -1;
}

stClearRewardBonusExp_t::stClearRewardBonusExp_t()
{
    m_avatarBonusExp = 0;
    m_rankWeightExp = 0;
    m_bestType = 0;
    m_selectedBonusExp = 0;
    m_burningFatigueExp = 0;
    m_channelBonusExp = 0;
    m_premiumAdvExp = 0;
    m_serverEventExp = 0;
    m_serverChannelExp = 0;
    m_creatureBonusExp = 0;
    m_upperMemberExp = 0;
    m_periodItemExp = 0;
    m_pcRoomSameUserExp = 0;
    m_returnUserExp = 0;
    m_eventCharacExp = 0;
    m_manageExp = 0;
    m_growthPowerExp = 0;
    m_premiumAddExp = 0;
}

// ============================================================================
// CParty::CalcAvatarBonus / CalcCreatureBonus / getAvatarBonus / getCreatureBonus
//   ORIG 0x085acfde / 0x085ad084 / 0x0814a9c2(W) / 0x0814a9e2(W)
//   报告：docs/class_func_reports/CParty/CalcAvatarBonus.md、CalcCreatureBonus.md
// ============================================================================
// CalcAvatarBonus：任一有效成员装备了 avatar → 单人队伍 0.02f，否则 0.05f；无则 0.0f
float CParty::CalcAvatarBonus()
{
    float bonus = 0.0f;
    bool found = false;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser->IsEquipAvatar())
        {
            found = true;
            break;
        }
    }
    if (found)
        bonus = (get_member_count() <= 1) ? 0.02f : 0.05f;
    return bonus;
}

// CalcCreatureBonus：user 当前角色装备了 creature → 0.05f，否则 0.0f
float CParty::CalcCreatureBonus(CUser* user)
{
    if (user->getCurCharacInvenR()->GetCreatureMgrR()->IsEquippedCreature())
        return 0.05f;
    return 0.0f;
}

float CParty::getAvatarBonus()
{
    return CalcAvatarBonus();
}

float CParty::getCreatureBonus(CUser* user)
{
    return CalcCreatureBonus(user);
}

// ============================================================================
// CParty::getClearRewardBonusExp —— ORIG 0x085ad278（0x73b 字节）
//   报告：docs/class_func_reports/CParty/getClearRewardBonusExp.md
// ============================================================================
// ORIG 乘加成公式：fildll(经验) × flds(比率) 后 fistpll——x87 默认就近舍入。
// 源码直接写 (long long)((double)(long long)exp * rate) 即产生相同指令序列
//（-march=i386 下为 fucompp/fnstsw/fistpll 形态，与 ORIG 一致）。
unsigned int CParty::getClearRewardBonusExp(CUser* user, unsigned int& exp,
                                            stClearRewardExpParameter_t const& param,
                                            stClearRewardBonusExp_t& bonus)
{
    unsigned int expPerUser = exp;   // 扣减前备份（日志 ExpPerUser）
    unsigned int effectExp = 0;      // globalEffect 择优值（日志第 8 参数）

    if (param.m_deadPenaltyRate != 0.0f)
        exp = (unsigned int)(long long)((double)(long long)exp *
                                        (1.0 - param.m_deadPenaltyRate));

    bonus.m_rankWeightExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_rankBonusWeight);
    bonus.m_avatarBonusExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_avatarBonus);
    if (param.m_avatarBonus != 0.0f)
        bonus.m_avatarBonusExp = std::max(bonus.m_avatarBonusExp, 1u);
    bonus.m_creatureBonusExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_creatureBonus);
    if (param.m_creatureBonus != 0.0f)
        bonus.m_creatureBonusExp = std::max(bonus.m_creatureBonusExp, 1u);
    bonus.m_periodItemExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_periodExpItemRate);
    bonus.m_pcRoomSameUserExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_pcRoomSameUserBonus);
    bonus.m_returnUserExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_returnUserPartyBonus);
    bonus.m_eventCharacExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_eventCharacExpRatio);

    // pcRoomBurning(2)/expDouble(1) 先到者优先；globalEffect 仅在类型 0/1/2
    // 且更大时覆盖为 4（类型 3 保留，ORIG +0x34c 分支）。
    if (param.m_pcRoomBurningRate != 0.0f)
    {
        bonus.m_selectedBonusExp =
            (unsigned int)(long long)((double)(long long)exp * param.m_pcRoomBurningRate);
        bonus.m_bestType = 2;
    }
    else if (param.m_expDoubleRate != 0.0f)
    {
        bonus.m_selectedBonusExp =
            (unsigned int)(long long)((double)(long long)exp * param.m_expDoubleRate);
        bonus.m_bestType = 1;
    }

    if (param.m_globalEffectRate != 0.0f)
    {
        effectExp =
            (unsigned int)(long long)((double)(long long)exp * param.m_globalEffectRate);
        int type = bonus.m_bestType;
        if (type > 0 && type < 3)
        {
            if (bonus.m_selectedBonusExp < effectExp)
            {
                bonus.m_selectedBonusExp = effectExp;
                bonus.m_bestType = 4;
            }
        }
        else if (type != 3)
        {
            bonus.m_selectedBonusExp = effectExp;
            bonus.m_bestType = 4;
        }
    }

    bonus.m_channelBonusExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_channelBonusRate);

    // 注意 ORIG 次序：燃烧疲劳块在择优之后执行并会把 m_bestType 覆写为 1。
    if (param.m_burningFatigueBonusExp != 0)
    {
        bonus.m_burningFatigueExp =
            (unsigned int)((long long)param.m_burningFatigueBonusExp * (long long)exp / 100);
        bonus.m_bestType = 1;
    }

    bonus.m_serverChannelExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_serverChannelRate);
    bonus.m_premiumAdvExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_premiumAdvantageRate);
    bonus.m_serverEventExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_serverEventExpRate);
    bonus.m_manageExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_manageBonus);
    bonus.m_growthPowerExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_growthPowerExpRate);
    bonus.m_premiumAddExp =
        (unsigned int)(long long)((double)(long long)exp * param.m_premiumAddExpRate);

    unsigned int total =
        exp + bonus.m_rankWeightExp + bonus.m_avatarBonusExp +
        bonus.m_selectedBonusExp + bonus.m_channelBonusExp + bonus.m_serverEventExp +
        bonus.m_premiumAdvExp + bonus.m_burningFatigueExp + bonus.m_serverChannelExp +
        bonus.m_creatureBonusExp;

    bonus.m_upperMemberExp = user->gainExpAsUpperMember((int)exp);
    total += (unsigned int)bonus.m_upperMemberExp;
    total += (unsigned int)user->gainGuildSkillExp((int)exp);
    total += (unsigned int)user->gainPowerWarRewardExp((int)exp);
    total += bonus.m_periodItemExp + bonus.m_pcRoomSameUserExp + bonus.m_returnUserExp +
             bonus.m_eventCharacExp + bonus.m_manageExp + bonus.m_growthPowerExp +
             bonus.m_premiumAddExp;

    if (1000000 < total)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x28b1, "BLOOD_LOG : EXP(%u), ExpPerUser(%u)\n", total, expPerUser);
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x28c2,
            "BLOOD_LOG : BONUS EXP PARAMETER : %f, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u\n",
            (double)param.m_deadPenaltyRate,
            bonus.m_rankWeightExp, bonus.m_avatarBonusExp, bonus.m_creatureBonusExp,
            bonus.m_periodItemExp, bonus.m_pcRoomSameUserExp, bonus.m_selectedBonusExp,
            effectExp, bonus.m_channelBonusExp, bonus.m_burningFatigueExp,
            bonus.m_serverChannelExp, bonus.m_premiumAdvExp, bonus.m_serverEventExp,
            (unsigned int)bonus.m_upperMemberExp);
    }
    return total;
}

// ============================================================================
// Prod_CParty 批次（2026-08-24）：CParty producer 函数（9 个 ORIG 符号）
// 逐函数对照 docs/class_func_reports/CParty/{GetPartyType,SendWalkout,
// SetPartyInfoUI,create_party,dungeon_start,game_start,set_single_play,
// walkout_member,get_item,drop_item}.md 的 ## 汇编 + ## 反编译 C 全文转写；
// 常量 _DAT_08cbbbcc=100.0f / _DAT_08cbbbe0=2.5 取自 df_game_r .rodata。
// ============================================================================

// ---- CParty::GetPartyType（ORIG T 0x85b62c0）----
int CParty::GetPartyType()
{
    int guildKey = 0;
    bool sameGuild = false;
    int onlineCnt = 0;
    int validCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        int key = m_member[i].m_pUser->get_charac_guildkey();
        if (key != 0)
        {
            if (guildKey == 0)
            {
                guildKey = key;
                sameGuild = true;
            }
            else if (key != guildKey)
            {
                sameGuild = false;
            }
        }
        unsigned int connectId = m_member[i].m_pUser->get_connect_upper_member();
        if (IsExistCharacter((int)connectId))
            ++onlineCnt;
        ++validCnt;
    }
    if (sameGuild)
        return 0;
    if (onlineCnt < 1)
        return 2;
    if (validCnt - 1 == onlineCnt)
        return 1;
    return 4;
}

// ---- CParty::SendWalkout（ORIG T 0x859cb00）----
int CParty::SendWalkout(int param, ENUM_WALKOUT_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 10);
    packet.put_byte(param);
    packet.put_byte((int)type);
    packet.finalize(true);
    send_to_party(packet);
    return 1;
}

// ---- CParty::SetPartyInfoUI（ORIG T 0x859b234）----
void CParty::SetPartyInfoUI(MSG_PARTY_SET_INFO const& info)
{
    setTitle(info.m_title);
    setTitleIndex(info.m_titleIndex);
    setUserMax(info.m_userMax);
    setDungIndex(info.m_dungIndex);
    setDungDiffi(info.m_dungDiffi);
}

// ---- CParty::create_party（ORIG T 0x859b1be）----
int CParty::create_party(CUser* user)
{
    m_member[0].m_pUser = user;                    // +0x78
    m_member[0].m_field8 = (int)user->GetUID();    // +0x80 ← CUser::GetUID
    m_manager = user;                              // +0x74
    m_field68 = 1;                                 // +0x68
    m_state = 1;                                   // +0x6a
    user->SetPartyIndex((short)m_partyIndex);      // +0x64（cwtl 截断）
    user->resetStraightVictories();
    ((CPartyTelePort*)GetPartyTelePort())->reset_teleport_data();
    return 1;
}

// ---- CParty::set_single_play（ORIG T 0x859b142）----
int CParty::set_single_play(CUser* user)
{
    m_member[0].m_pUser = user;                    // +0x78
    m_member[0].m_field8 = (int)user->GetUID();    // +0x80
    m_manager = user;                              // +0x74
    m_singlePlay = 1;                              // +0x69
    m_field68 = 1;                                 // +0x68
    m_autoCreated = 1;                             // +0x13c
    m_state = 1;                                   // +0x6a
    user->SetPartyIndex((short)m_partyIndex);
    ((CPartyTelePort*)GetPartyTelePort())->reset_teleport_data();
    return 1;
}


// ---- CParty::walkout_member（ORIG T 0x859ca4a）----
int CParty::walkout_member(CUser* user, int slot)
{
    if (m_manager != user)
        return 8;
    if (m_member[slot].m_pUser != 0)
    {
        CUser* target = m_member[slot].m_pUser;
        int r = leave_user(target, (ENUM_PARTY_INFO_TYPE)2);
        if (r == 0)
        {
            pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();
            mgr->ForcedMoveAtPowerWarPlace(target);
            return 0;
        }
        if (r == 1)
            return 1;
    }
    return 4;
}

// ---- CParty::game_start（ORIG T 0x859d718）----
// 状态==2 直接返回；否则逐成员 make_basic_info/goto_dungeon/交易清理，
// 广播 0x2(基本)/0x3(进图名单)/0x1b(血宫信息) 三包；村庄/区域不一致走
// HACK_VILL_AREA 日志并 DisConnSig(0x18) 全员断线。
void CParty::game_start(CUser* user)
{
    if (m_state == 2)
        return;

    PacketGuard basicPacket;                                // -0x54
    basicPacket.clear();
    basicPacket.put_header(0, 2);
    basicPacket.put_byte(1);
    basicPacket.put_short(get_member_count());

    m_tournamentVictory = 0;                                // +0x11a
    m_routingDice = get_rand_int(get_member_count());          // +0x144 路由起始槽
    m_field185c = GlobalData::s_systemTime_.getCurSec();    // +0x185c

    PacketGuard enterPacket;                                // -0x60
    enterPacket.put_header(0, 3);
    int liveCount = 0;                                      // -0x40
    int countIdx = enterPacket.get_index();                 // -0x64
    enterPacket.put_byte(liveCount);

    char hackFlag = 0;                                      // -0x3a
    char hostVill = user->getCurCharacVill();               // -0x39
    int hostArea = user->get_area(false);                   // -0x38

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        char vill = m_member[i].m_pUser->getCurCharacVill();   // -0x1d
        int area = m_member[i].m_pUser->get_area(false);       // -0x1c
        bool mismatch = ((hostVill != vill) || (hostArea != area));
        if (mismatch && !is_quick_party() &&
            reinterpret_cast<CBattle_Field*>(&m_padBattleField1)
                    ->m_villageMonsterId == 0)                  // ORIG this+0xc4c
        {
            LogManager::logFormat(
                1, "party.cpp", __PRETTY_FUNCTION__, 0x84c,
                "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
                NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                (int)hostVill, (int)vill, hostArea, area);
            hackFlag = 1;
            break;
        }
        m_liveFlag[i] = 1;      // +0x380
        m_enterMapFlag[i] = 1;  // +0x384
        m_field388[i] = 0;      // +0x388
        m_field38c[i] = 0;      // +0x38c
        m_member[i].m_pUser->make_basic_info((char*)&basicPacket, 1);
        G_GameWorld()->goto_dungeon(m_member[i].m_pUser);
        if (m_member[i].m_pUser->get_state() == 5)
        {
            enterPacket.put_short(
                m_member[i].m_pUser->get_unique_id() & 0xffff);
            enterPacket.put_byte(1);
            ++liveCount;
        }
        int tradeIdx = m_member[i].m_pUser->GetTradeSpace();   // -0x48
        if (tradeIdx != -1)
        {
            CTradeSpace* ts = G_CGameManager()->GetTradeSpace(tradeIdx);
            if (ts != 0 && ts->check_trade_possibility() != 1)
            {
                ts->cancel_trade();
                G_CGameManager()->PutTradeSpace(ts);
            }
        }
        m_member[i].m_pUser->getCurCharacQuestW()->m_authenData.reset();
    }

    if (hackFlag != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = get_user(i);
            if (u != 0)
            {
                LogManager::logFormat(
                    1, "party.cpp", __PRETTY_FUNCTION__, 0x88a,
                    "[HACK_VILL_AREA] m_id : %s, Vill(%d), Area(%d)",
                    NumberToString(u->get_acc_id(), 0), (int)hostVill,
                    hostArea);
                u->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
        }
        return;
    }

    enterPacket.put_byte(countIdx, liveCount);
    enterPacket.finalize(true);
    if (!IsExistInvisible())
    {
        if (getManager()->getMoveSpace() != 1)
            G_GameWorld()->send_all(enterPacket);
        else
            send_to_party(enterPacket);
    }
    else
    {
        send_to_party(enterPacket);
    }

    basicPacket.finalize(true);
    send_to_party(basicPacket);

    set_host();
    send_host_info();

    m_field1864 = 0;
    m_hellWorldMap = 0;
    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);   // -0x34
    if (worldIdx > 0)
        m_hellWorldMap =
            G_CDataManager()->m_worldMapList.find_world_map(worldIdx);

    basicPacket.clear();
    basicPacket.put_header(0, 0x1b);

    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
    if (get_member_count() == 1)
    {
        bd.SetHellPartyValueTotal(
            *(int*)((char*)G_CDataManager()->get_hellparty_script_values() + 8));
    }

    int hellDiv = *(int*)(m_padcee + 0x1e);   // ORIG this+0xd0c（BF 未建模区）
    int hellVal = (int)(bd.m_hellPartyValueTotal * 100.0f / (float)hellDiv);
    basicPacket.put_int(hellVal);

    std::vector<int> hellQuests;                            // -0x70
    bool hellEnable =
        (m_hellWorldMap != 0 && m_hellWorldMap->IsInHellDungeon() &&
         reinterpret_cast<CBattle_Field*>(&m_padBattleField1)
             ->IsEnableHellDungeon());
    if (hellEnable)
    {
        CheckClearQuestHellParty(hellQuests);
        size_t qn = hellQuests.size();                      // -0x10
        if ((int)qn < 1)
        {
            basicPacket.put_byte(0);
            m_field1864 = 1;
        }
        else
        {
            basicPacket.put_byte((int)qn);
            for (size_t k = 0; k < qn; ++k)
                basicPacket.put_short(hellQuests[k]);
        }
    }
    else
    {
        basicPacket.put_byte(0);
    }

    if (!make_blood_dungeon_info(user, basicPacket))
        make_blood_dungeon_info_blank(basicPacket);

    bool deathTowerArea = false;                            // -0x29
    worldIdx = G_GameWorld()->GetWorldMapIndex(user);       // -0x28
    if (worldIdx > 0)
    {
        CWorldMap* wm2 =
            G_CDataManager()->m_worldMapList.find_world_map(worldIdx);  // -0xc
        (void)wm2;
        if (m_hellWorldMap != 0 && m_hellWorldMap->hasDeathTower())
            deathTowerArea = true;
    }

    if (isDungeonDifficultyInfromNotice(deathTowerArea))
    {
        basicPacket.put_byte(1);
        m_manager->setMemberDungeonFlag(true);
    }
    else
    {
        basicPacket.put_byte(0);
    }
    if (deathTowerArea)
        reinterpret_cast<CBattle_Field*>(&m_padBattleField1)
            ->setWorldMapTowerArea(deathTowerArea);

    basicPacket.finalize(true);
    send_to_party(basicPacket);

    m_recvResultFlag = 0;                                   // +0x118
    reinterpret_cast<CPartyTelePort*>(&m_pad1ad0)->reset_teleport_data();
}

// ---- CParty::dungeon_start（ORIG T 0x85a0954）----
// 返回：0=成功；1=SelectDungeon/封印门/血宫置态失败；0xe=等级不足；
// 0x13=未通关该难度或难度倒退；0x15=dungeon 查表失败；8=EPLP 状态异常；
// 其余为 GetDungeonIndex_isTOD/checkInoutConditionDungeon 透传。
int CParty::dungeon_start(int dungeonIdx, char type, ENUM_DUNGEON_TYPE dtype)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);

    resetMemberUseCoin();
    m_firstMapClear = 0;                                   // +0x324
    ClearMapHitCount();
    reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu)->startDungeon();

    int todApcIdx = -1;                                    // -0x78
    int err = GetDungeonIndex_isTOD(dungeonIdx, todApcIdx);  // -0x74
    if (err != 0)
        return err;

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);   // -0x70
    if (dungeon == 0)
        return 0x15;

    PacketGuard packet;                                    // -0x84
    int partyType = m_partyType;                           // -0x6c ← +0xcd8
    SetPartyMemberCoinLimit(get_member_count() *
                            dungeon->GetPartyMemberCoinLimit());
    reinterpret_cast<CTraceMobDieHack*>(&m_pad210)->setDungeonDiff(type);

    if (GetEPLPState() != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
            hack->addServerHackCnt(m_member[i].m_pUser,
                                   (WongWork::ENUM_HACKTYPE)0x195, 1, 0, 0);
        }
        return 8;
    }

    int minLevel = dungeon->get_min_level();               // -0x68
    if (minLevel >= 2 && IsLightServer() != 0)
        minLevel = dungeon->get_min_level();               // 两分支同值，保留结构

    if (m_manager->IsGameMasterMode() != 1 &&
        minLevel > m_manager->get_charac_level())
        return 0xe;

    if (!checkDungeonDifficulty(type, dungeon))
    {
        char clearedDiff =
            m_manager->getDungeonClearRefR()->getClearedDungeonDiff(
                (short)dungeonIdx);                        // -0x51
        if (clearedDiff == 0)
            return 0x13;
        if ((clearedDiff < (char)type) && ((char)type != 4))
            return 0x13;
    }

    int questStage = 0;                                    // -0x64
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (partyType == 1)
            continue;
        questStage = m_member[i].m_pUser->CheckQuestMap(dungeon->get_index());
        if (questStage > 0)
            break;
    }

    SetUsedCoinCount(0);

    if (!CheckHaveHellPartyPassItem(dungeon, dtype))
        return 0;

    bool randomHell = false;                               // -0x85
    int inoutErr =
        checkInoutConditionDungeon(dungeon, (ENUM_DUNGEON_MODE)partyType,
                                   (ENUM_CMDPACKET)0x10);  // -0x60
    if (inoutErr != 0)
        return 0;

    bf.check_random_appear_hell_dungeon(dungeon, (ENUM_DUNGEON_MODE)partyType,
                                        dtype, randomHell);

    int memberCount = get_member_count();
    if (!bf.SelectDungeon(dungeonIdx, dungeon, (int)(char)type, dtype,
                          memberCount, m_hellWorldMap, randomHell, questStage))
        return 1;

    choose_random_buff((QuickParty::RandomBuffDungeon)bf.getRandomBuffDungeonType(0));

    char names[256];                                       // -0x1cc
    memset(names, 0, sizeof(names));
    _getMemberNames(names);

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        reinterpret_cast<BestClearTime*>(m_member[i].m_pUser->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0x12))
            ->setNewRecord(false);

        int mode = partyType;                              // -0x70 复用槽
        char* names2 = _getMemberNames(names);
        const char* dunName = dungeon->GetDungeonName();
        m_member[i].m_pUser->m_historyLog.EnterDungeon(dunName, (int)(char)type,
                                                       names2, mode);
        m_member[i].m_pUser->resetDungeonClearHackCheck();

        if (m_member[i].m_pUser->getMoveSpace() == 0)
        {
            int firstEmpty = 0;                            // -0xb4
            SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                m_member[i].m_pUser->getCurCharacSkillW());
            int emptyCnt = skillW->checkskillSlotEmptyCount(firstEmpty);   // -0x44
            for (int k = 0; k < emptyCnt; ++k)
            {
                int set = m_member[i].m_pUser->SetETC(0x1a6, firstEmpty);
                WongWork::CHackAnalyzer* hack =
                    m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a6, 1,
                                       (unsigned int)set, 0);
            }

            int cntA = 0, cntB = 0, cntC = 0, cntD = 0, cntE = 0, cntF = 0;
            char growFirst = m_member[i].m_pUser->getCurCharFirstGrowType();
            int job = m_member[i].m_pUser->get_charac_job();
            CInventory* invenW = m_member[i].m_pUser->getCurCharacInvenW();
            invenW->checkEquipmentState(job, (int)growFirst, cntA, cntB, cntC,
                                        cntD, cntE, cntF);

            int lvl = m_member[i].m_pUser->get_charac_level();
            if (lvl > 0xf)
            {
                for (int k = 0; k < cntA; ++k)
                {
                    WongWork::CHackAnalyzer* hack =
                        m_member[i].m_pUser->getHackAnalyzer();
                    hack->addServerHackCnt(m_member[i].m_pUser,
                                           (WongWork::ENUM_HACKTYPE)0x1a4, 1, 0, 0);
                }
            }
            lvl = m_member[i].m_pUser->get_charac_level();
            if (lvl > 0x17)
            {
                for (int k = 0; k < cntB; ++k)
                {
                    int set = m_member[i].m_pUser->SetETC(0x1a5, cntC);
                    WongWork::CHackAnalyzer* hack =
                        m_member[i].m_pUser->getHackAnalyzer();
                    hack->addServerHackCnt(m_member[i].m_pUser,
                                           (WongWork::ENUM_HACKTYPE)0x1a5, 1,
                                           (unsigned int)set, 0);
                }
            }
            for (int k = 0; k < cntD; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a7, 1, 0, 0);
            }
            for (int k = 0; k < cntE; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a8, 1, 0, 0);
            }
            for (int k = 0; k < cntF; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a9, 1, 0, 0);
            }
        }
    }

    // 封印门判定：地狱图+类型1+旗标已立时必须消耗成功，否则整队失败。
    if (m_hellWorldMap != 0 && m_hellWorldMap->IsInHellDungeon() &&
        dtype == 1 &&
        m_sealDoorFlag != 0 &&
        UseSealDoorItems(m_hellWorldMap->m_field34) == 0)
        return 1;

    if ((char)dungeon->m_bloodDungeonType < 1)
    {
        if (!dungeon->isTournamentDungeon())
        {
            packet.clear();
            packet.put_header(0, 0x1c);
            packet.put_short(dungeonIdx);
            packet.put_byte((int)(char)type);
            bf.MakePacket(0x1c, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x1d, &packet);
            send_to_party(packet);
            _Send_TOD_APCInfo(todApcIdx);
        }
        else
        {
            packet.clear();
            packet.put_header(0, 0x182);
            packet.put_short(dungeonIdx);
            packet.put_byte((int)(char)type);
            bf.MakePacket(0x182, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x183, &packet);
            send_to_party(packet);
        }
    }
    else
    {
        packet.clear();
        packet.put_header(0, 0xd4);
        packet.put_short(dungeonIdx);
        packet.put_byte((int)(char)type);
        bf.MakePacket(0xd4, &packet);
        send_to_party(packet);
        packet.clear();
        bf.MakePacket(0xd3, &packet);
        send_to_party(packet);
        if (!bf.setBloodState((ENUM_BLOOD_STATE)2))
            return 1;
    }

    // 计时/奖励旗标区复位（BattleData +0x10..+0x38）
    bd.m_field20 = (int)GlobalData::s_systemTime_.getCurTickCount();   // +0x34c
    bd.m_field24 = 0;                                                  // +0x350
    bd.m_field10 = 0;                                                  // +0x33c
    bd.m_field14 = 0;                                                  // +0x340
    bd.m_field18 = 0;                                                  // +0x344
    bd.m_field1c = 0;                                                  // +0x348
    bd.m_field2c = 0;                                                  // +0x358
    bd.m_field30 = 0;                                                  // +0x35c
    bd.m_field34 = bd.m_field20;                                       // +0x360
    bd.m_field38 = bd.m_field24;                                       // +0x364

    set_state(2);
    SetEPLPState(1);
    gen_timer_key((TIMER_MESSAGE)0x2d);
    SetSelectedEPLPCmd(-1);
    int startCount = get_member_count();                   // -0x5c
    setStartGamePartyCount(startCount);

    secretshop::SECRET_SHOP_DATA& shop =
        *reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop);
    if (((char)dungeon->m_bloodDungeonType < 1) && m_manager->get_state() != 10 &&
        dungeon->isTowerOfDespairDungeon() == 0 &&
        dungeon->isTournamentDungeon() == 0)
    {
        shop.m_openCode = GlobalData::s_secret_shop->LotteryNpc(
            dungeon->get_index(), dungeon->get_standard_level(), startCount);
    }
    else
    {
        shop.m_openCode = 1000;
    }

    char premiumCard = 0;                                  // -0x55
    unsigned int cardItem = 0;                             // -0x8c
    CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
    if (ev41->IsEventing(0) && IsReturnUserParty())
    {
        premiumCard = isPremiumGoldCardReward(m_dungeon, cardItem) ? 1 : 0;
        if (premiumCard != 0)
        {
            SetPremiumGoldCardParty();
            SetPremiumGoldCardDefaultItem(cardItem);
        }
    }

    reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)->Clear();   // +0x1dc

    for (int i = 0; i <= 3; ++i)
    {
        for (int j = 0; j <= 1; ++j)
            m_field204[j][i] = (char)0xff;                 // +0x204[j][i]
        m_dungeonClearState = 0;                           // +0x1e0
        if (!_checkValidUser(i))
            continue;

        CUser* mu = m_member[i].m_pUser;
        GlobalData::s_secret_shop->SendSecretShopNpc(mu, shop.m_openCode);
        mu->getCurCharacQuestW()->set_authen_data((QUEST_CONDITION)0,
                                                  dungeonIdx, (int)(char)type);
        if (dungeon != 0)
            SetProperLevelDungeon(mu);

        if (partyType != 1)
        {
            bool fatigueSkip = (dungeon->m_villageAttackEventFlag == 1) || (dungeon->m_bloodDungeonType != 0) ||
                               (dungeonIdx == 10000) ||
                               (dungeon->isNoFatigueDungeon() == 1);
            if (!fatigueSkip)
            {
                if (!dungeon->isTournamentDungeon())
                {
                    mu->FatigueUp(0);
                }
                else
                {
                    mu->FatigueUp(dungeon->getTournamentRoundFatigue());
                    mu->SendFatigue();
                }
            }
        }

        mu->setPlayingDungeonIndex(dungeonIdx);
        mu->m_character.Reset();              // PlayInfo::Reset @ user+0x8e034
        mu->m_character.m_f20 = startCount;   // user+0x8e054
        mu->m_field8e074 = 0;                 // user+0x8e074（CUser 直属字段；Layout_D 布局修正）

        int stdLvl = m_dungeon->get_standard_level();
        int muLvl = mu->get_charac_level();
        bool properLevel =
            ((muLvl >= stdLvl - 5) && (muLvl <= stdLvl + 3));
        if (properLevel)
        {
            bd.m_flag28[0] = 1;                // +0x354
            bd.m_flag28[1] = 1;                // +0x355
        }

        int maxBefore = getMemberMaxLevelBeforeDungeon();
        int lvBefore = mu->get_charac_level_before_dungeon();
        if ((maxBefore - lvBefore >= 7) && (startCount != 1))
        {
            bd.m_flag28[2] = 1;                // +0x356
            if (isGuildParty() ||
                FindUserByCharId(mu->get_connect_upper_member()))
                bd.m_flag28[3] = 0;            // +0x357
            else
                bd.m_flag28[3] = 1;
        }
        else
        {
            bd.m_flag28[2] = 0;
            bd.m_flag28[3] = 0;
        }

        if (m_dungeon->get_dimension_possible() > 0)
        {
            bd.m_flag28[0] = 1;
            bd.m_flag28[1] = 1;
        }

        HistoryLog::WriteDunStart((_IO_FILE*)mu->m_field796f8,
                                  mu->get_charac_job(),
                                  mu->get_charac_level(),
                                  (int)mu->getCurCharacGrowType());

        if (getManager()->getMoveSpace() != 1)
        {
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                dungeonIdx, partyType == 1);
        }
        if (partyType != 1)
            mu->IncCurCharacDungeonPlayCount();
        if (partyType == 1)
        {
            user_creature::CCreatureMgr* cm =
                const_cast<CInventory*>(mu->getCurCharacInvenR())->GetCreatureMgrW();
            cm->TurnStomach(false);
        }
        mu->set_level_before_dungeon((short)mu->get_charac_level());

        if (dungeon != 0)
            SetProperDungeonEvent(mu, dungeonIdx);
        SetEndRouting();
        static_cast<XNuclear::CHades*>(mu->getHades())->StdCateUpdate(2, 0);
    }

    for (int a = 0; a <= 1; ++a)
        for (int b = 0; b <= 3; ++b)
            m_field1e4[a][b] = 0;                          // +0x1e4[a][b]

    Reset_party_overlapped_drop_ratio();
    check_passive_skill();
    send_loadingcheck_msg();

    CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
    SetWeekendEvent(ev57->IsEventing(0) ? true : false);

    QuickParty::RewardData_Param rewardParam(this);
    QuickParty::RewardData_Result rewardResult;
    set_random_reward((QuickParty::RandomBuffType)9, rewardParam, rewardResult);
    SendPartyMemberTagChracInfo();
    return 0;
}

// ---- CParty::get_item（ORIG T 0x85a3b98）----
// 返回 1=拾取成功；0=失败（错误码经 1/0x2e 包下发）。
int CParty::get_item(CUser* user, int itemIdx, bool flag, char c, int& outIdx,
                     unsigned char& outUpgrade, bool& outStackable)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);

    char routingEligible = 1;                       // -0xe9
    CUser* winner = 0;                              // -0xe8
    int weights[4] = { 0, 0, 0, 0 };                // -0x234
    int sellPrice = 0;                              // -0x244
    int typeMark = 0;                               // -0xe4

    PacketGuard packet;                             // -0x240

    int slotNo = (int)GetMemberSlotNo(user);        // -0xe0
    if (slotNo != -1 && m_liveFlag[slotNo] != 1)
    {
        const user_creature::CCreatureMgr* cm = user->getCurCharacInvenR()->GetCreatureMgrR();
        if (cm->GetCreatureItemId() != (int)0xf631)
        {
            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
            hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x132, 1, 0, 0);
        }
    }

    if (m_dungeon != 0 && m_dungeon->IsEnterEachMap())
        bf.SetCurSeatNo(slotNo);

    map_item mi;                                    // -0x2bc
    char found = bf.get_field_item(itemIdx, mi);    // -0xd9

    if (found == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        if ((char)c > 3)
            packet.put_byte(0x51);
        else
            packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);

        if (!IsAutoCreated() && get_member_count() != 1)
        {
            bool visited =
                (!bf.check_start_point()) || bf.IsVisitedCurrentMap();   // -0xc9
            if (!visited || !bf.is_last_field_item(itemIdx))
            {
                WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
                hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x12d, 1, 0, 0);
            }
        }
        return 0;
    }

    int addInfo = mi.m_item.m_addInfo;
    if (get_member_count() < 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    if (addInfo == 0 || addInfo == 0x4af)
    {
        bool goldGen = (addInfo == 0x4af);
        int liveCount = get_live_count_enter_map();                  // -0xc8
        if (liveCount < 1)
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(0x15);
            packet.finalize(true);
            user->Send(packet);
            return 0;
        }

        int base;                                    // 金币基数
        if (!goldGen)
        {
            int money = mi.m_item.get_add_info();
            if (money > 499999)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x165c, 4);
                trace("Dungeon(%d) drop money(%d) on %d' party member with difficulty %d",
                      m_dungeon->m_index, money, get_member_count(),
                      bf.get_dungeon_diff());
            }
            int share = mi.m_item.get_add_info() / liveCount;   // -0xc0
            int rem = mi.m_item.get_add_info() % liveCount;     // -0xbc
            base = share + rem;                                 // -0xb8
        }
        else
        {
            WongWork::CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
            WongWork::CItemGenerator* gen =
                mgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2);
            int lvl0 = m_member[0].m_pUser->get_charac_level();   // 占位：逐成员内取
            (void)lvl0;
            base = 0;
        }

        packet.put_header(0, 0x27);
        packet.put_short(itemIdx);
        packet.put_short(user->get_unique_id() & 0xffff);

        for (int i = 0; i <= 3; ++i)
        {
            int gain;
            if (!_checkValidUser(i))
            {
                packet.put_byte(0);
                packet.put_int(0);
                continue;
            }
            packet.put_byte(1);

            if (!goldGen)
            {
                gain = (m_member[i].m_pUser == user) ? base : base - (mi.m_item.get_add_info() % liveCount);
                if (base > 499999)
                {
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0x1688, 4);
                    trace("User(%s) Charac(%s) gain money(%d), old money is %d.",
                          NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                          m_member[i].m_pUser->getCurCharacName(), base,
                          m_member[i].m_pUser->getCurCharacInvenR()->get_money());
                }
                if (m_enterMapFlag[i] != 1)
                    gain = 0;
            }
            else
            {
                WongWork::CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
                WongWork::CItemGenerator* gen =
                    mgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2);
                unsigned char lv = (unsigned char)m_member[i].m_pUser->get_charac_level();
                int* table = gen->getGeneratorCommonData()->getGoldGenTable(lv);
                int rnd = GetRandomRange(-table[1], table[1]);
                gain = table[0] + (table[0] * rnd) / 100;
                if (gain > 499999)
                {
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0x1789, 4);
                    trace("User(%s) Charac(%s) gain money(%d), old money is %d.",
                          NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                          m_member[i].m_pUser->getCurCharacName(), gain,
                          m_member[i].m_pUser->getCurCharacInvenR()->get_money());
                }
                if (m_enterMapFlag[i] != 1)
                    gain = 0;
            }

            int bonusGold = 0;                   // 展示加成合计
            int guildFund = 0;
            if (mi.m_count != 0)
            {
                const user_creature::CCreatureMgr* cm =
                    m_member[i].m_pUser->getCurCharacInvenR()->GetCreatureMgrR();
                bonusGold += cm->GetExtraGold(gain);
                gain += cm->GetExtraGold(gain);
                bonusGold += cm->GetOverSkillExtraGold(gain);
                gain += cm->GetOverSkillExtraGold(gain);

                Inven_Item pet1 = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 11);
                int pct = 0;
                switch (pet1.m_addInfo)
                {
                case 0x6603: case 0x66e5: case 0x6802: case 0x28e4a3: case 0x28e4a6:
                    pct = 3; break;
                case 0x6622: case 0x66e6: case 0x6803: case 0x68a2:
                    pct = 4; break;
                case 0x28e4a8:
                    pct = 5; break;
                default:
                    pct = 0; break;
                }
                if (pct != 0)
                {
                    int extra = (gain * pct) / 100;
                    bonusGold += extra;
                    gain += extra;
                }

                Inven_Item pet2 = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 19);
                if (pet2.m_addInfo == 0x5e7f)
                {
                    int extra = (int)(2.5 * (double)gain);
                    bonusGold += extra;
                    gain += extra;
                }
            }
            if (mi.m_count != 0)
            {
                float mb = CalcurateManageBonus((ManageBonusType::T)1,
                                                m_member[i].m_pUser,
                                                (unsigned int)gain);
                int manage = (int)(long double)mb;
                bonusGold += manage;
                gain += manage;
                GetInstanceCirculationStatistic()->AddCirculationStatistic(
                    (CIRCULATION_STATISTIC_FIELD)0x11, m_member[i].m_pUser,
                    (unsigned int)manage);
            }
            if (mi.m_count != 0 &&
                m_member[i].m_pUser->get_charac_guildkey() != 0 &&
                m_member[i].m_pUser->IsCompleteGuildFund() == 0)
            {
                GuildParameterScript& gp = G_CDataManager()->m_guildParameter;
                bool fixedBand =
                    (gp.m_field104 <= base) && (gp.m_field108 >= gain);
                if (fixedBand)
                {
                    guildFund = gp.m_field10c;
                }
                else if (gp.m_field108 < base)
                {
                    guildFund = (int)((long double)gp.m_field110 * (double)base);
                }
                if (guildFund <= gain && guildFund > 0)
                {
                    gain -= guildFund;
                    GetInstanceCirculationStatistic()->AddCirculationStatistic(
                        (CIRCULATION_STATISTIC_FIELD)0x2a, m_member[i].m_pUser,
                        (unsigned int)guildFund);
                }
                if (guildFund > 0)
                {
                    CGuildServerProxy* proxy =
                        GlobalData::s_guild_proxy_mgr->GetServerProxy(
                            m_member[i].m_pUser->GetServerGroup());
                    proxy->SendAddGuildFund(
                        m_member[i].m_pUser->get_charac_no(-1),
                        m_member[i].m_pUser->get_charac_guildkey(), guildFund,
                        (Packet_Add_Guild_Fund::eFundType)0);
                }
            }

            CInventory* invenW = m_member[i].m_pUser->getCurCharacInvenW();
            int gained = invenW->gain_money(
                gain, goldGen ? (eMoneyAddReason)0x1a : (eMoneyAddReason)4, true,
                bonusGold);
            if (mi.m_count != 0)
                m_member[i].m_pUser->saveTaxMoneyForUpperMember(gain);
            if (gained < gain)
            {
                if (gained < 0)
                    gained = 0;
                m_member[i].m_pUser->SendMoneyFullReason(
                    (ENUM_MONEY_FULL_REASON)0, (unsigned long)gain,
                    (unsigned long)gained);
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)1, m_member[i].m_pUser,
                (unsigned int)gain);

            packet.put_int(gained);
            packet.put_byte(1);
            packet.put_int(bonusGold);
            packet.put_int(guildFund);
        }

        packet.finalize(true);
        send_to_party(packet);
        int picked = bf.pickup_item(itemIdx);
        if (picked >= 1)
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(picked);
            packet.finalize(true);
            send_to_party(packet);
            user->Send(packet);
            return 0;
        }
        if (mi.m_count != 0)
        {
            outIdx = mi.m_item.m_addInfo;
            outUpgrade = 0;
        }
        else
        {
            outIdx = -1;
            outUpgrade = 0;
        }
        return 1;
    }

    CItem* item = G_CDataManager()->find_item(addInfo);   // -0x40
    if (item == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x1878, "G_CDataManager()->find_item(%d) fail", addInfo);
        return 0;
    }

    if (item->is_stackable())
    {
        outStackable = true;
        if (item->GetItemType() == 9)
            typeMark = 7;
    }
    else
    {
        if (item->IsCreatureItem())
            typeMark = 7;
        else if (item->IsAvatarItem())
            typeMark = 1;
    }

    if (get_member_count() > 1)
    {
        if (mi.m_count == 0)
            routingEligible = 0;
        if (item->get_rarity() > 3)
            routingEligible = (char)flag;
        if (mi.m_count != 0 && item->isEpicRoutingItem())
            routingEligible = (char)flag;

        if (routingEligible != 0)
        {
            int diceNext = m_routingDice + 1;                 // -0x34
            if (diceNext >= get_member_count())
                diceNext = 0;
            int seq = 0;                                   // -0x30
            int ownerBias = 0;                             // -0x2c
            if (mi.m_ownerId != 0)
            {
                ownerBias = get_rand_int(0xb) + 0x59;
            }
            for (int i = 0; i <= 3; ++i)
            {
                if (!_checkValidUser(i))
                    continue;
                if (m_liveFlag[i] == 1)
                    continue;
                const CInventory* invR = m_member[i].m_pUser->getCurCharacInvenR();
                int w = invR->tryInsertItemIntoInventory(mi.m_item);
                if (w == -1)
                    continue;
                weights[i] += 1;
                if (seq == diceNext)
                {
                    weights[i] += 0x28;
                    m_routingDice = seq;
                }
                bool jobBonus;
                if (item->is_stackable())
                    jobBonus = false;
                else if (item->GetItemType() != 10)
                    jobBonus = false;
                else
                    jobBonus = item->check_job_type(
                        m_member[i].m_pUser->get_charac_job()) != 0;
                if (jobBonus)
                    weights[i] += 0x1e;
                weights[i] += get_rand_int(100 - weights[i]);
                if (mi.m_ownerId != 0)
                {
                    if (m_member[i].m_pUser == (CUser*)mi.m_ownerId)
                        weights[i] = ownerBias;
                    else if (weights[i] >= ownerBias)
                        weights[i] = ownerBias - 1;
                }
                ++seq;
            }

            int best = 0;                                  // -0x28
            for (int i = 0; i <= 3; ++i)
            {
                if (!_checkValidUser(i))
                    continue;
                if (m_liveFlag[i] == 1)
                    continue;
                if (weights[i] > best)
                {
                    best = weights[i];
                    winner = m_member[i].m_pUser;
                }
                else if (weights[i] == best && weights[i] > 0)
                {
                    weights[i] -= 1;
                }
            }
        }
        else
        {
            winner = user;
        }
    }
    else
    {
        winner = user;
    }

    if (winner == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x16);
        packet.finalize(true);
        send_to_party(packet);
        return 0;
    }

    G_Store()->GetSellItemPrice(mi.m_item, item, (short)mi.m_item.m_fieldb,
                                false, sellPrice);

    if (item->IsRandomOption() && !winner->getCheckPickUpRandomOptionItem())
    {
        winner->setCheckPickUpRandomOptionItem(true);
        DB_InsertRandomOptionItemInform::makeRequest(winner->GetUID(),
                                                     winner->get_acc_id(), 1);
        PacketGuard optPacket;
        optPacket.clear();
        optPacket.put_header(0, 0x171);
        optPacket.put_short(winner->get_unique_id() & 0xffff);
        optPacket.put_byte(1);
        optPacket.finalize(true);
        winner->Send(optPacket);
    }
    if (item->IsRandomOption())
    {
        statistc_proxy::add(
            winner, "log_random_option", "pick_cnt", 1u, "%d,'%s'",
            (int)winner->get_charac_10_level_section(),
            GlobalData::s_systemTime_.getCurDate());
    }

    int slot;                                            // -0xd4
    if (!mi.m_item.isAvatarItemType())
    {
        slot = winner->getCurCharacInvenW()->insertItemIntoInventory(
            mi.m_item, (eItemAddReason)4, true, true);
    }
    else
    {
        char ipg[24];                                    // -0x268
        WongWork::CGenUniqueNo::genIPGNo((WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                                         winner->getCurCharacNo(), ipg);
        slot = winner->addAvatarItem(mi.m_item.m_addInfo, 0,
                                     (char)mi.m_item.m_fieldb, ipg,
                                     (eAvatarItemAddReason)0xf);
    }

    if (slot == -1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)2,
                                                   winner,
                                                   (unsigned int)sellPrice);
    int picked = bf.pickup_item(itemIdx);                // -0xd0
    if (picked >= 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x19bb, 5);
        trace("m_field_data.pickup_item( %d/%d ) ", itemIdx, addInfo);
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(picked);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    _onGetItem(winner, (unsigned int)addInfo, (unsigned int)mi.m_item.m_addInfo2);

    packet.put_header(0, 0x27);
    packet.put_short(itemIdx);
    packet.put_short(user->get_unique_id() & 0xffff);
    for (int i = 0; i <= 3; ++i)
        packet.put_byte(weights[i]);
    packet.put_short(winner->get_unique_id() & 0xffff);
    packet.put_short(slot);
    packet.put_byte(typeMark);
    packet.finalize(true);
    send_to_party(packet);

    if (mi.m_count != 0)
    {
        outIdx = mi.m_item.m_addInfo;
        outUpgrade = mi.m_item.GetUpgrade();
    }
    else
    {
        outIdx = -1;
        outUpgrade = 0;
    }

    if (mi.m_count != 0 && mi.m_item.m_field1 == 2)
    {
        winner->SendAradEventItemLog((unsigned long)mi.m_item.m_addInfo,
                                     mi.m_item.get_add_info());
    }
    return 1;
}

// ---- CParty::drop_item（ORIG T 0x85a63f4）----
// 返回 0=成功；0xd=稀有度限制；0x11=删除/扣款失败；0x13=交易中；
// 0x16=落物失败；0x17=禁止丢弃；0xd1=服务限制；-1=item 查表失败。
int CParty::drop_item(CUser* user, int a2, int a3, char type, int slot,
                      int count)
{
    if (count < 0)
    {
        LogManager::logFormat(1, "party.cpp",
                              __PRETTY_FUNCTION__,
                              0x1a0c, "user - accid[%s] %s : count<0",
                              NumberToString(user->get_acc_id(), 0),
                              user->getCurCharacName());
        return 0x11;
    }
    if (user->CheckInTrade())
        return 0x13;

    map_item mi;                                   // -0x12c
    Inven_Item src;                                // -0xd5
    src.reset();

    char t = (char)type;
    if (t == 2)
    {
        src = user->getCurCharacCargoR()->get_cargo_slot(slot);
    }
    else if (t == 0)
    {
        if (slot == 0)
        {
            // 直接构造"金币掉落"项：m_count=0、ITEM_TYPE=2、addInfo=0
            mi.m_count = 0;
            Inven_Item& dst = mi.m_item;
            dst.m_field1 = 2;
            dst.m_addInfo = 0;
            dst.set_add_info(count);
            dst.m_fieldb = 0;
            dst.ResetItemAttr();
        }
        else
        {
            src = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        }
    }
    else if (t == 3)
    {
        src = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
    }
    else if (t == 7)
    {
        src = user->getCurCharacInvenR()->GetInvenSlot(3, slot);
        if (src.m_field1 == 5)
            return 0x17;
    }

    int sellPrice = 0;                             // -0x8c
    if (src.m_addInfo != 0)
    {
        CItem* item = G_CDataManager()->find_item(src.m_addInfo);   // -0x24
        if (item == 0)
        {
            LogManager::logFormat(
                1, "party.cpp",
                __PRETTY_FUNCTION__,
                0x1a48, "[CParty::drop_item] Null Item");
            return -1;
        }

        int attach = item->GetAttachType();        // -0x20
        switch (attach)
        {
        case 1:
        case 2:
        case 5:
            return 0x17;
        case 3:
            if (src.m_field0 != 1)
                return 0x17;
            break;
        default:
            break;
        }

        if (src.m_upgradeSep.IsTradeRestriction())
            return 0x17;

        if (item->get_rarity() > 2)
            return 0xd;

        if (src.isEquipableItemType() &&
            (*(int*)(reinterpret_cast<const char*>(item) + 0x234) == 0xb ||
             *(int*)(reinterpret_cast<const char*>(item) + 0x234) < 10))
            return 0x17;   // ORIG 直读 item+0x234（CItem 头未建模区）

        Inven_Item& dst = mi.m_item;
        mi.m_count = 0;
        dst.m_field1 = src.m_field1;
        dst.m_addInfo = src.m_addInfo;
        if (src.isEquipableItemType())
            dst.set_add_info(src.m_addInfo2);
        else
            dst.set_add_info(count);
        dst.SetItemAttr(src.GetItemAttr());
        dst.m_fieldb = src.m_fieldb;
        dst.m_field0 = src.m_field0;
        dst.m_addInfo2 = src.m_addInfo2;
        dst.m_amp = src.m_amp;                       // +0x11..0x1e 整段
        dst.m_random = src.m_random;                 // +0x1f..0x32 整段
        dst.m_upgradeSep.SetUpgradeSeparate(src.m_upgradeSep.GetUpgradeSeparate());

        G_Store()->GetSellItemPrice(src, item, (short)count, false, sellPrice);
    }

    if (t == 2)
    {
        CCargo* cargoW = const_cast<CCargo*>(user->getCurCharacCargoR());
        if (!cargoW->delete_item(slot, count, (eItemDelReason)6))
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1ac6, 5);
            trace("CParty::drop_item , delete_item failed 2, User ch =%d , %d %d",
                  user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                       user, (unsigned int)count);
    }
    else if (t == 0)
    {
        if (slot == 0)
        {
            ServiceRestrictManager* srm =
                ARAD::Singleton<ServiceRestrictManager>::Get();
            if (srm->isRestricted(user, (RestrictCategory::Enum)2, 1))
                return 0xd1;
            if (!user->getCurCharacInvenW()->use_money(count,
                                                       (eMoneySubReason)6, true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1aa6, 5);
                trace("CParty::drop_item , use_money failed, User ch =%d , %d %d",
                      user->getCurCharacNo(), slot, count);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
        }
        else
        {
            if (!user->getCurCharacInvenW()->delete_item(
                    (INVEN_TYPE)1, slot, count, (eItemDelReason)6, true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1ab5, 5);
                trace("CParty::drop_item , delete_item failed, User ch =%d , %d %d",
                      user->getCurCharacNo(), slot, count);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)count);
        }
    }
    else if (t == 3)
    {
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, slot, count, (eItemDelReason)6, true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1ae8, 5);
            trace("CParty::drop_item , delete_item failed 3, User ch =%d , %d",
                  user->getCurCharacNo(), slot);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                       user, (unsigned int)count);
    }
    else if (t == 7)
    {
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)3, slot, count, (eItemDelReason)6, true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1af9, 5);
            trace("CParty::drop_item , delete_item failed 4, User ch =%d , %d",
                  user->getCurCharacNo(), slot);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                       user, (unsigned int)count);
    }

    if (!src.isEquipableItemType() && mi.m_item.m_addInfo2 != 0 &&
        mi.m_item.get_add_info() > 0x7d0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1b0c, 4);
        trace("drop item(%d) stack size over %d add_info(%d)", mi.m_item.m_addInfo,
              0x7d0, mi.m_item.get_add_info());
    }

    int dropped = reinterpret_cast<CBattle_Field*>(&m_padBattleField1)
                      ->drop_item(mi);           // ORIG CBattle_Field::drop_item(map_item)
    if (dropped == -1)
        return 0x16;

    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(a2);
    packet.put_short(a3);
    packet.put_short(dropped);
    packet.put_int(mi.m_item.m_addInfo);
    packet.put_byte(mi.m_item.GetItemAttr());
    packet.put_int(mi.m_item.get_add_info());
    packet.put_short(mi.m_item.m_fieldb);
    packet.put_byte(mi.m_item.m_amp.getAbilityType());
    packet.put_short(mi.m_item.m_amp.getAbilityValue());
    packet.put_packet(mi.m_item);
    packet.put_byte(mi.m_count == 0 ? 1 : 0);
    packet.finalize(true);
    send_to_party(packet);

    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)t);
    packet.put_short(slot);
    packet.put_int(count);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ============================================================================
// insert_time_*（ORIG 0x85b2114..0x85b22e1）：gen_timer_key 后向 TimerQueue
// 注册 OBJ_TYPE_1 定时器，param1=队伍索引，sec=调用方参数，param3=0。
// ============================================================================
void CParty::insert_time_dungeon_clear_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0xd, param,
        gen_timer_key((TIMER_MESSAGE)0xd), 0);
}

void CParty::insert_time_card_select_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x10, param,
        gen_timer_key((TIMER_MESSAGE)0x10), 0);
}

void CParty::insert_time_clear_dungeon_reward_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x11, param,
        gen_timer_key((TIMER_MESSAGE)0x11), 0);
}

void CParty::insert_time_dungeon_score_scroll_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x12, param,
        gen_timer_key((TIMER_MESSAGE)0x12), 0);
}

void CParty::insert_time_card_select_right_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x13, param,
        gen_timer_key((TIMER_MESSAGE)0x13), 0);
}

// ============================================================================
// CParty::set_play_result —— ORIG 0x085b2068：仅队长发起时把全员
// setPlayingDungeonIndex(-1) 并置 state=3 / m_recvResultFlag=1。
// ============================================================================
int CParty::set_play_result(CUser* user, GameResultSet& result)
{
    int ret = 0;                                        // -0x10
    if (m_host == user && !m_recvResultFlag)            // +0x70 / +0x118
    {
        for (int i = 0; i <= 3; ++i)                    // -0xc
        {
            if (_checkValidUser(i))
                m_member[i].m_pUser->setPlayingDungeonIndex(-1);
        }
        m_state = 3;                                    // +0x6a（ORIG movb 直写）
        m_recvResultFlag = true;                        // +0x118（ORIG movb 直写）
    }
    return ret;
}

// ============================================================================
// CParty::forced_set_play_result —— ORIG 0x085b26e8：非 state==3 时向全队
// 广播 0x22 结算强制包头（byte0/int0/byte0/byte0）。
// ============================================================================
void CParty::forced_set_play_result()
{
    if ((unsigned char)m_state != 3)                    // +0x6a
    {
        PacketGuard packet;
        packet.put_header(0, 0x22);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        send_to_party(packet);
    }
}

// ============================================================================
// CParty::map_load_forced_finish —— ORIG 0x085b1e66：装载超时强制收尾；
// 空队走 destroy+日志，否则未完成装载的有效成员逐个 giveup_game。
// ============================================================================
void CParty::map_load_forced_finish()
{
    if (m_pad119 == 0)                                  // +0x119 处理守卫
    {
        int count = get_member_count();                 // -0x10
        if (count == 0)
        {
            char log1[16];
            new (log1) cMyTrace(__PRETTY_FUNCTION__, 0x3184, 0);
            (*reinterpret_cast<cMyTrace*>(log1))(
                "<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)");
            destroy();
            LogManager::logFormat(
                1, "party.cpp", __PRETTY_FUNCTION__, 0x3187,
                "CParty::map_load_forced_finish 1, Using: %d, Member = 0\n",
                (unsigned int)(unsigned char)m_field68);   // +0x68
        }
        else
        {
            for (int i = 0; i <= 3; ++i)                // -0xc
            {
                if (_checkValidUser(i) && m_finishLoadingFlag[i] != 1)
                    giveup_game(m_member[i].m_pUser, false, false, false);
            }
            if (get_member_count() > 0)
                check_allmember_loading(true);
        }
    }
}

// ============================================================================
// 卡片选择（ORIG 0x85b401c..0x85b494e）。m_field204[type][slot]：
// dungeon_reward_card 表，0xff=未选。
// ============================================================================
bool CParty::HaveAlreadySelectedCard(char a, eClearRewardCardType_t type)
{
    if ((int)type < 2)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_field204[(int)type][i] == a)
                return true;
        }
    }
    return false;
}

bool CParty::_checkGoldCardSelectable(CUser* user, eClearRewardCardType_t type,
                                      int param)
{
    if (user == 0)
        return false;
    if ((int)type < 0 || 1 < (int)type)
        return false;
    if (param < 0 || 3 < param)
        return false;
    if (m_field1e4[(int)type][param] == 0)              // +0x1e4 金卡费用表
        return true;

    CInventory* inven =
        const_cast<CInventory*>(
            reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenW());
    if (inven->use_money(m_field1e4[(int)type][param],
                         (eMoneySubReason)0x14, true) == 0)
        return false;
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, 0);
    return true;
}

void CParty::SendEachCardInfo()
{
    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x4a);
    packet.put_byte(1);
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        packet.put_byte((int)(char)m_field204[0][i]);   // +0x204[i]
        packet.put_byte((int)(char)m_field204[1][i]);   // +0x208[i]
        if ((unsigned char)m_field204[1][i] == 0xff)
        {
            packet.put_byte(0);
        }
        else
        {
            int countIdx = packet.get_index();
            Inven_Item& item0 = bd.m_itemGroups2[(unsigned char)m_field204[1][i]][0];
            Inven_Item& item1 = bd.m_itemGroups2[(unsigned char)m_field204[1][i]][1];
            packet.put_byte(1);
            packet.put_int(0);
            packet.put_int(item0.get_add_info());
            if (item1.m_addInfo != 0)                   // +0x7af int 读
            {
                packet.put_int(item1.m_addInfo);
                packet.put_int(item1.get_add_info());
                packet.put_byte(countIdx, 2);           // 回填选项组数
            }
        }
        packet.put_byte((unsigned char)m_field38c[i]);  // +0x38c
    }
    packet.finalize(true);
    send_to_party(packet);
}

bool CParty::SetCardNumber(CUser* user, char a, eClearRewardCardType_t type)
{
    int slot = GetMemberSlotNo(user);                   // -0x18
    if (slot == -1 || 3 < (int)a)
        return false;
    if (HaveAlreadySelectedCard((char)slot, type))
        return false;
    if ((unsigned char)m_field204[(int)type][(int)a] != 0xff)
        return false;
    if (!_checkGoldCardSelectable(user, type, slot))
        return false;

    m_field204[(int)type][(int)a] = (char)slot;
    if ((int)type != 1)
        return true;

    // 金卡：事件 0x41 进行中跳过发放
    CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
    if (ev41->IsEventing(0))
        return true;                                    // ORIG 跳共用尾部

    CUserCharacInfo* ci = reinterpret_cast<CUserCharacInfo*>(user);
    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
    Inven_Item& reward = bd.m_itemGroups2[slot][1];     // +0x7ad + slot*0x7a
    int putSlot = ci->getCurCharacInvenW()->tryInsertItemIntoInventory(reward);
    if (putSlot == -1)
    {
        reward.m_addInfo = 0;                           // +0x7af / +0x7b4 清零
        reward.m_addInfo2 = 0;
    }
    else
    {
        CEventBase* ev40 = GlobalData::s_event_manager->GetRepeatEvent(0x40);
        if ((!ev40->IsEventing(0) && !IsReturnUserParty() &&
             !IsEventCharacParty()) || reward.m_field1 != 1)
        {
            // 不清奖励槽
        }
        else if (ci->getCurCharacInvenW()->check_empty_count(
                     Inven_Item::ITEM_TYPE_EQUIP, 2) == 1)
        {
            // 空位充足，不清
        }
        else
        {
            reward.m_addInfo = 0;
            reward.m_addInfo2 = 0;
        }
    }

    if (user != 0)
    {
        APSystem::_ActionGroupIndex clearGroup;
        clearGroup.m_value = 8;
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, clearGroup, 0, (ENUM_PACKETCLASS)0);
        if (reward.m_addInfo != 0)
        {
            CItem* item = G_CDataManager()->find_item(reward.m_addInfo);
            if (item != 0)
            {
                int rarity = item->get_rarity();
                if (rarity >= 0 && rarity <= 4)
                {
                    APSystem::_ActionGroupIndex rewardGroup;
                    rewardGroup.m_value = 0x15;
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        user, rewardGroup, 0, (ENUM_PACKETCLASS)0);
                }
            }
        }
    }
    return true;
}

void CParty::CompleteCardSelect()
{
    for (int i = 0; i <= 3; ++i)                        // -0x14
    {
        if ((unsigned char)m_field204[0][i] == 0xff)
        {
            for (int z = 0; z <= 3; ++z)                // -0x10
            {
                if (_checkValidUser(z))
                {
                    if (!IsSelectCard((char)z, eClearRewardCardType_t_0))
                    {
                        m_field204[0][i] = (char)z;
                        if ((unsigned char)m_field204[0][i] == 0xff)
                        {
                            LogManager::logFormat(
                                1, "party.cpp",
                                __PRETTY_FUNCTION__, 0x36f8,
                                "INIT_CARD_NUM != dungeon_reward_card_[i]==>> "
                                "dungeon_reward_card_[i] = %d, i = %d, z = %d",
                                (int)(char)m_field204[0][i], i, z);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void CParty::SendCardSelectComplete()
{
    PacketGuard packet;
    packet.put_header(0, 0x43);
    packet.finalize(true);
    send_to_party(packet);
}

bool CParty::IsCompleteCardSelectOfMembers(eClearRewardCardType_t type)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_member[i].m_pUser != 0)
        {
            if ((int)type == 2)
            {
                for (int j = 0; j < 2; ++j)
                {
                    if ((unsigned char)m_field204[j][i] == 0xff)
                        return false;
                }
            }
            else if ((unsigned char)m_field204[(int)type][i] == 0xff)
            {
                return false;
            }
        }
    }
    return true;
}

// ============================================================================
// CParty::TournamentDungeonClear —— ORIG 0x85bd236：锦标赛结算；逐有效成员
// 发卡牌奖励，胜利路径追加卡型 1 与通关日志，随后注册 0x15 秘商定时器。
// ============================================================================
void CParty::TournamentDungeonClear(bool flag)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    if (m_dungeon == 0)                                 // +0xcac
    {
        LogManager::logFormat(
            1, "party.cpp", __PRETTY_FUNCTION__, 0x45bc,
            "Tournament Dungeon is NULL");
        return;
    }
    if (m_dungeon->isTournamentDungeon() != 1)
    {
        LogManager::logFormat(
            1, "party.cpp", __PRETTY_FUNCTION__, 0x45c2,
            "Dungeon(%d) is can not call function", bf.get_dungeon_index());
        return;
    }

    SaveCoin(flag ? (eCoinSubReason)1 : (eCoinSubReason)2);
    PacketGuard packet;
    for (int slot = 0; slot < 4; ++slot)                // -0x18
    {
        if (_checkValidUser(slot))
        {
            Inven_Item items[2];                        // -0xa6
            if (bf.m_tournamentReward.GetItemFromCard(
                    ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, slot, items, 2))
            {
                _putItemIntoUser(m_member[slot].m_pUser, items, packet);
            }
            if (flag)
            {
                for (int i = 0; i < 2; ++i)
                    items[i].reset();
                if (bf.m_tournamentReward.GetItemFromCard(
                        ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, slot, items, 2))
                {
                    _putItemIntoUser(m_member[slot].m_pUser, items, packet);
                }
                HistoryLog::WriteDunClear(
                    (FILE*)m_member[slot].m_pUser->m_field796f8);
                OnSuccessedDungeonClear(m_member[slot].m_pUser);
                if (m_member[slot].m_pUser != 0 &&
                    m_member[slot].m_pUser->IsProperLevelDungeonUser())
                {
                    m_member[slot].m_pUser->ProcProperLevelDungeonClear();
                }
            }
            else
            {
                OnFailedDungeonClear(m_member[slot].m_pUser);
            }
        }
    }

    SetEPLPState(2);
    int sec = 0x3c;                                     // -0x1c
    secretshop::SECRET_SHOP_DATA& shop =
        *reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop);
    if (shop.IsOpen())
        sec = *(int*)((char*)G_CDataManager() + 0xa784) * 0x3c;   // 秘商开放剩余基数（CDataManager 域待具名）
    int key = gen_timer_key((TIMER_MESSAGE)0x15);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                (TIMER_MESSAGE)0x15, sec, key, 0);
    set_state(1);
    sendInoutConditionDungeon((RetryFailReason::T)checkInoutConditionDungeon());
    CheckHackAverageHitCount();
    ClearMapHitCount();
    reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu)->dungeonClear();
}

// ============================================================================
// CParty::MakeTournamentReward —— ORIG 0x85bcb40：按当前轮次生成奖励与金币，
// 逐成员发经验/奖励包（0x184），置锦标赛清除状态并注册 0x2e 定时器。
// ============================================================================
void CParty::MakeTournamentReward(CTournamentDungeonReward& reward)
{
    bool victory = m_tournamentVictory;                 // +0x11a
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    int round = bf.GetTournamentCurRound();
    reward.MakeReward(&m_dungeon->m_map8ac, &m_dungeon->m_itemRate8c4, round,
                      victory);

    int dungIdx = bf.get_dungeon_index();
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungIdx);
    int memberCount = get_member_count();               // -0x2c
    if (dungeon == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x4540, "Unknown Dungeon %d", dungIdx);
        return;
    }
    if (memberCount < 1)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x4545, "No Member[%d] in party", memberCount);
        return;
    }

    int stdLevel = dungeon->get_standard_level();       // -0x28
    WongWork::stGenerateRefData_t ref;                  // -0xdc
    ref.m_field34 = (char)stdLevel;
    ref.m_field35 = (char)bf.get_dungeon_diff();
    ref.m_rateSlot = (char)memberCount;
    {
        int one = 1;
        memcpy(&ref.m_rankBonusIndex, &one, sizeof(one));         // ORIG local_a4=1（int 写）
        BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
        unsigned short v;
        v = (unsigned short)bd.m_field48;               // +0x374
        memcpy(&ref.m_goldBase, &v, sizeof(v));
        v = (unsigned short)bd.m_killedMonster1;        // +0x36c
        memcpy(&ref.m_goldFactor, &v, sizeof(v));
        v = 0;                                          // +0x40
        memcpy(&ref.m_dungeon, &v, sizeof(v));
        v = (unsigned short)bd.m_killedMonster2;        // +0x370
        memcpy((char*)&ref.m_dungeon + 2, &v, sizeof(v));
    }

    WongWork::stGenerateResult_t result;                // -0x94
    WongWork::CItemGeneratorMgr* genMgr = G_CDataManager()->getItemGenerator();
    genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
        ->generateRandomItem(ref, result);

    float goldRate = *(float*)((char*)dungeon + 0x8d4);   // 金币率（CDungeon 域待具名）
    if (goldRate < 0.0f)
        goldRate = 1.0f;
    reward.SetRewardGold(result, goldRate);

    int exp = 0;                                        // -0x18
    int expRound = bf.GetTournamentCurRound();          // -0x14
    if (!victory)
        expRound = expRound - 1;
    if (expRound < 0)
        expRound = 0;

    std::map<unsigned char, unsigned int>::const_iterator itExp =
        dungeon->m_map8d8.find((unsigned char)expRound);
    if (itExp != dungeon->m_map8d8.end())
        exp = (int)itExp->second;

    for (int slot = 0; slot < 4; ++slot)                // -0x10
    {
        PacketGuard packet;                             // -0x4c
        packet.clear();
        packet.put_header(0, 0x184);
        if (_checkValidUser(slot))
        {
            CUser* user = m_member[slot].m_pUser;
            user->setChattingMessageCount(0);
            bool abuse = isHelpAbuseParty(user);
            reinterpret_cast<CUserCharacInfo*>(user)->calcHelpAbuseRatio(
                (unsigned int)exp, abuse);
            reinterpret_cast<CUserCharacInfo*>(user)->reset_level_before_dungeon();

            int sp = 0;                                 // -0x50
            int sfp = 0;                                // -0x54
            if (user->gain_exp_sp(exp, sp, sfp,
                                  (CUser::eExpAddReason)2, 0, false))
            {
                PacketGuard infoPacket;                 // -0x60
                infoPacket.clear();
                infoPacket.put_header(0, 2);
                infoPacket.put_byte(1);
                infoPacket.put_short(1);
                user->make_basic_info((char*)&infoPacket, 1);
                infoPacket.finalize(true);
                user->Send(infoPacket);
            }
            packet.put_byte(expRound);
            packet.put_int(exp);
            if (!victory)
                packet.put_byte(0);
            else
                MakeQuestRewardPacket(&packet, user);
            reward.MakeRewardPacket(&packet, victory);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    SetTournamentDungeonClearState(1);
    InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2e);
}

bool IsCollectDungeonStatistic();   // ORIG T 0x08484a51（InitEnvHelpers.cpp 定义）
extern Inven_Item g_emptySlot;      // ORIG 全局 0x943ddc0（AccountCargo.cpp 定义）

// ============================================================================
// CParty::OnKilledMonster —— ORIG 0x85b5a4c：怪物击杀反外挂核查链
// （伤害占比/跨图伤害/秒杀检测）后按 check_grid_clear 收图。
// ============================================================================
void CParty::OnKilledMonster(CUser* user, int monsterIdx, map_monster monster,
                             MSG_MONSTER_DIE const& dieInfo)
{
    IsCollectDungeonStatistic();
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    bool skip = false;
    if (bf.IsClearDungeon() || dieInfo.m_fieldA22 != 0)
        skip = true;
    if (!skip && user != 0)
    {
        int addCnt =
            reinterpret_cast<CTraceMobDieHack*>(&m_pad210)
                ->addMonsterDamage(monsterIdx,
                                   (unsigned short)dieInfo.m_fieldF,
                                   (int)dieInfo.m_killValue2);
        if (2 < addCnt)
        {
            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
            hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0xc9, 1, 0, 0);
        }
    }

    bool doCrossCheck = true;
    int dungIdx = bf.get_dungeon_index();
    if (dungIdx == 0x3e || dungIdx == 0x3f || dungIdx == 0x40)
        doCrossCheck = false;

    if (doCrossCheck && dieInfo.m_fieldA22 == 0 &&
        monster.m_roleType < 4 &&
        (unsigned int)dieInfo.m_killValue2 <
            (unsigned int)monster.m_active.getHpMax() &&      // +0x38 子对象
        monsterIdx != 10 && monsterIdx != 0xc351 && monsterIdx != 0x40a)
    {
        for (int i = 0; i < 4; ++i)                     // -0x20
        {
            if (checkValidUser(i))
            {
                CUser* member = get_user(i);
                void* memberParty = member->GetParty();
                void* userParty = user->GetParty();
                if (memberParty != 0 && userParty != 0 && member != 0)
                {
                    CDungeon* mpDungeon = *(CDungeon**)((char*)memberParty + 0xcac);
                    CDungeon* upDungeon = *(CDungeon**)((char*)userParty + 0xcac);
                    if (mpDungeon == 0 || upDungeon == 0)
                    {
                        if (member == user)
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x70, 1, 0, 0);
                        else
                            member->getHackAnalyzer()->addServerHackCnt(
                                member, (WongWork::ENUM_HACKTYPE)0x71, 1, 0, 0);
                    }
                    else
                    {
                        bool special = mpDungeon->get_index() == 0x2afe ||
                                       upDungeon->get_index() == 0x2afe;
                        if (special)
                        {
                            if (member == user)
                                user->getHackAnalyzer()->addServerHackCnt(
                                    user, (WongWork::ENUM_HACKTYPE)0x70, 1, 2, 0);
                            else
                                member->getHackAnalyzer()->addServerHackCnt(
                                    member, (WongWork::ENUM_HACKTYPE)0x71, 1, 2, 0);
                        }
                        else if (member == user)
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x70, 1, 0, 0);
                        }
                        else
                        {
                            member->getHackAnalyzer()->addServerHackCnt(
                                member, (WongWork::ENUM_HACKTYPE)0x71, 1, 0, 0);
                        }
                    }
                }
            }
        }
    }

    bool instantCheck = false;
    if (dieInfo.m_fieldA22 == 0 && monster.m_roleType < 4)
    {
        MapInfo* mapInfo = bf.GetCurrentMapInfo();
        if (mapInfo->m_killChecker1.getUseSkillMaterialTime() == 0)
            instantCheck = true;
    }
    if (instantCheck)
    {
        int level = 0;                                  // -0x10
        unsigned int hpMax = (unsigned int)monster.m_active.getHpMax();  // +0x38 子对象
        if (hpMax * 4 < (unsigned int)dieInfo.m_killValue2)
            level = 3;
        else if (hpMax * 3 < (unsigned int)dieInfo.m_killValue2)
            level = 2;
        else if (hpMax * 2 < (unsigned int)dieInfo.m_killValue2)
            level = 1;
        if (level != 0)
        {
            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
            hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x86, level, 0, 0);
        }
    }
    if (bf.check_grid_clear())
        bf.onClearMap(true);
}

// ============================================================================
// CParty::OnFightVillageMonster —— ORIG 0x85b9596：村庄受攻战入场；管理器
// 受理后走 game_start 全流程并广播 0x1c/0x1d 地图包。
// ============================================================================
int CParty::OnFightVillageMonster(CUser* user, int param)
{
    int ret = 0;                                        // -0x34
    if (GlobalData::s_villageMonsterMgr->OnFightVillageMonster(
            user, (unsigned short)param) == 0)
    {
        ret = 0;
        game_start(user);
        CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
        int dungIdx = bf.get_dungeon_index();           // -0x30
        int diff = 0;                                   // -0x2c
        CDungeon* dungeon = G_CDataManager()->find_dungeon(dungIdx);   // -0x28
        if (dungeon == 0)
        {
            return 0x15;
        }
        GameWorld* world = G_GameWorld();
        int wmIdx = world->GetWorldMapIndex(user);      // -0x24
        if (0 < wmIdx)
        {
            // ORIG：CDataManager+0x8780 CWorldMapList::find_world_map(wmIdx)
            //       （CWorldMapList 成员待 CDataManager 域具名）
            typedef CWorldMap* (*FindWmFn)(void*, int);
            (void)(FindWmFn)0;   // 形态说明；实际调用见下
            CWorldMapList& wml = G_CDataManager()->m_worldMapList;  // +0x8780
            m_hellWorldMap = wml.find_world_map(wmIdx);   // +0x1868
        }
        if (bf.SelectDungeon(dungIdx, dungeon, diff, (ENUM_DUNGEON_TYPE)0,
                             get_member_count(), 0, 0, 0))
        {
            char names[0x100];                          // -0x140
            memset(names, 0, sizeof(names));
            _getMemberNames(names);
            for (int i = 0; i < 4; ++i)                 // -0x20
            {
                if (_checkValidUser(i))
                {
                    m_member[i].m_pUser->m_historyLog.EnterDungeon(
                        dungeon->GetDungeonName(), diff, names, 0);
                }
            }
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0x1c);
            packet.put_short((short)dungIdx);
            packet.put_byte(diff);
            bf.MakePacket(0x1c, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x1d, &packet);
            send_to_party(packet);

            BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
            bd.m_field20 = (int)GlobalData::s_systemTime_.getCurTickCount();
            bd.m_field24 = 0;
            bd.m_field10 = 0;
            bd.m_field14 = 0;
            bd.m_field18 = 0;
            bd.m_field1c = 0;
            bd.m_field2c = 0;
            bd.m_field30 = 0;
            bd.m_field34 = bd.m_field20;
            bd.m_field38 = bd.m_field24;
            set_state(2);
            SetEPLPState(1);
            send_loadingcheck_msg();
            PacketGuard ack;
            ack.clear();
            ack.put_header(1, 0x10b);
            ack.put_byte(1);
            ack.finalize(true);
            user->Send(ack);
            return 0;
        }
        return 1;
    }
    return ret;
}

// ============================================================================
// CParty::OnFinishVillageMonsterFighting —— ORIG 0x85b99ac：村庄战收尾；
// 结算胜负、giveup_game 并对全员发 0x1ec 复仇地下城资格包。
// ============================================================================
int CParty::OnFinishVillageMonsterFighting(CUser* user, bool flag)
{
    CUser* members[4] = {0, 0, 0, 0};                   // -0x54
    for (int i = 0; i < 4; ++i)                         // -0x3c
    {
        if (checkValidUser(i))
            members[i] = get_user(i);
    }
    if (user == 0)
    {
        for (int i = 0; i < 4; ++i)                     // -0x38
        {
            if (checkValidUser(i))
            {
                user = get_user(i);
                break;
            }
        }
    }
    if (user == 0)
        return 3;

    bool notClear;                                      // -0x42
    bool isClear;                                       // -0x41
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    if (!bf.check_grid_clear())
    {
        notClear = true;
        isClear = false;
    }
    else
    {
        notClear = false;
        isClear = true;
    }
    if (GlobalData::s_villageMonsterMgr->OnKillVillageMonster(user, notClear)
        != 0)
    {
        return 0;
    }

    giveup_game(user, false, isClear, flag);
    *(unsigned short*)((char*)this + 0xc4c) = 0;   // BF+0x128 村庄区（待 CBattle_Field 域具名）
    {
        PacketGuard ack;                                // -0x60
        ack.clear();
        ack.put_header(1, 0x10c);
        ack.put_byte(1);
        ack.finalize(true);
        user->Send(ack);
    }
    if (!GlobalData::s_villageMonsterMgr->OnEvent() &&
        GlobalData::s_villageMonsterMgr->GetRewardType() == 1)
    {
        int nowSec = GlobalData::s_systemTime_.getCurSec();          // -0x34
        int closeTime =
            GlobalData::s_revengeDungeonMgr->GetCloseTime();         // -0x30
        int remain = closeTime < nowSec ? 0 : closeTime - nowSec;    // -0x2c
        for (int i = 0; i < 4; ++i)                     // -0x28
        {
            if (members[i] != 0)
            {
                CUserCharacInfo* ci =
                    reinterpret_cast<CUserCharacInfo*>(members[i]);
                char body[10];                          // -0x6a
                memset(body, 0, sizeof(body));
                PacketGuard packet;                     // -0x78
                packet.put_header(0, 0x1ec);
                unsigned int attackCount =
                    (unsigned int)ci->GetCurVillageAttackCount();
                // m_serverParameter.m_revenge498（CDataManager+0x500）
                const std::vector<ConditionEnterRevenge>& conds =
                    G_CDataManager()->m_serverParameter.m_revenge498;
                packet.put_binary(body, 10);
                packet.finalize(true);
                members[i]->Send(packet);
                members[i]->SetVillageAttackedReward(false);
                for (std::vector<ConditionEnterRevenge>::const_iterator it =
                         conds.begin();
                     it != conds.end(); ++it)
                {
                    if ((unsigned int)it->m_field0 <= attackCount &&
                        attackCount <= (unsigned int)it->m_field4)
                    {
                        ci->SetCurRevengeDungeonCount(it->m_field8);
                        ci->VillageAttack_DBUpdate();
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

// ============================================================================
// CParty::prepare_dungeon_clear —— ORIG 0x85aa39a：清图前处理（追踪器、任务
// 认证、宠物经验）并视条件触发 ClearDungeon。
// ============================================================================
void CParty::prepare_dungeon_clear()
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    m_dungeonClearTracer.SetClearedDungeon(m_tournamentVictory != 0);
    m_dungeonClearTracer.SetEndPoint(bf.check_end_point());
    if (!bf.check_grid_clear())
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (_checkValidUser(i) && m_partyType != 1)     // +0xcd8
        {
            CUser* user = m_member[i].m_pUser;
            UserQuest* quest = user->getCurCharacQuestW();
            quest->set_authen_data((QUEST_CONDITION)1, bf.get_cur_map(), 0);
            if (0 < user->m_fatigueGain)
            {
                const user_creature::CCreatureMgr* mgrR =
                    reinterpret_cast<CUserCharacInfo*>(user)
                        ->getCurCharacInvenR()
                        ->GetCreatureMgrR();
                if (!mgrR->IsGrowCreature_Equipped_Creature())
                {
                    int exp = user->m_fatigueGain;
                    user_creature::CCreatureMgr* mgrW =
                        reinterpret_cast<CUserCharacInfo*>(user)
                            ->getCurCharacInvenW()
                            ->GetCreatureMgrW();
                    mgrW->GainExp(exp);
                }
            }
            user->m_fatigueGain = 0;
        }
    }

    int mapIdx = bf.GetCurrentMapIndex();
    if (bf.ClearCondition((ENUM_DUNGEON_CLEAR_CONDITION_TYPE)1, mapIdx))
        ClearDungeon();
    else if (bf.check_end_point())
        ClearDungeon();
}

// ============================================================================
// CParty::party_dungeon_fail —— ORIG 0x85b3670：全灭败北处理（计时差记录、
// 逐成员退场与惩罚、状态复位或解散；锦标赛路径改发奖励）。
// ============================================================================
void CParty::party_dungeon_fail()
{
    if (!(m_tournamentVictory == 0 && check_allmember_die() && m_dungeon != 0))
        return;

    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
    char skipPanalty = 0;                               // -0x31
    CDungeon* dungeon = m_dungeon;                      // -0x30
    if (dungeon != 0)
    {
        skipPanalty = (*((char*)dungeon + 0x89f) > 0) ? 1 : 0;   // 进度标志（CDungeon 域待具名）
        if (dungeon->isTowerOfDespairDungeon())
            skipPanalty = 1;
    }
    if (*(unsigned short*)((char*)this + 0xc4c) != 0)   // BF+0x128 村庄区（待具名）
        skipPanalty = 1;
    if (dungeon != 0 &&
        (dungeon->get_dimension_possible() >= 1 || *((char*)dungeon + 0x89c) != 0))
        skipPanalty = 1;

    if (dungeon->isTournamentDungeon())
    {
        bf.m_tournamentReward.ResetReward();
        MakeTournamentReward(bf.m_tournamentReward);
        return;
    }

    unsigned int now =
        (unsigned int)GlobalData::s_systemTime_.getCurTickCount();
    unsigned int elapsedLow = now - (unsigned int)bd.m_field20;
    bd.m_field0 = (int)(elapsedLow - (unsigned int)bd.m_field2c);
    bd.m_field4 =
        (int)((0u - (unsigned int)(now < (unsigned int)bd.m_field20)
               - (unsigned int)bd.m_field24 - (unsigned int)bd.m_field30)
              - (unsigned int)(elapsedLow < (unsigned int)bd.m_field2c));
    SaveCoin((eCoinSubReason)2);
    CheckHackAverageHitCount();
    reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu)->dungeonFail();

    PacketGuard packet;
    for (int slot = 0; slot < 4; ++slot)                // -0x2c
    {
        if (_checkValidUser(slot))
        {
            CUser* user = m_member[slot].m_pUser;
            OnFailedDungeonClear(user);
            G_GameWorld()->out_from_dungeon(user);
            if (user->getMoveSpace() != (UserSpace::T)1)
            {
                GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(
                    dungeon->get_index() != 0, true);
            }
            if (user->getMoveSpace() == (UserSpace::T)1)
            {
                BlueMarble* marble =
                    reinterpret_cast<BlueMarble*>(user->getBlueMarble());
                if (marble != 0)
                    marble->giveupDungeon(user);
            }
            if (m_partyType != 1)
            {
                char has85c = (dungeon != 0 && *((char*)dungeon + 0x85c) != 0) ? 1 : 0;
                char has87a = (dungeon != 0 && *((char*)dungeon + 0x87a) != 0) ? 1 : 0;
                if (skipPanalty != 1 && has85c != 1 && has87a != 1)
                    user->giveup_panalty();
            }
        }
    }

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short((short)get_member_count());
    if (m_autoCreated == 0)                             // +0x13c
    {
        for (int i = 0; i < 4; ++i)
        {
            if (_checkValidUser(i))
                m_member[i].m_pUser->make_basic_info((char*)&packet, 0);
        }
        set_state(1);
        battle_data_reset();
        packet.finalize(true);
        if (!IsExistInvisible())
            G_GameWorld()->send_all(packet);
        else
            send_to_party(packet);
    }
    else
    {
        short mgrIndex = m_manager->GetPartyIndex();
        int mgrState = m_manager->get_state();
        unsigned int accId = m_manager->get_acc_id();
        char* idStr = NumberToString(accId, 0);
        char log1[16];
        new (log1) cMyTrace(__PRETTY_FUNCTION__, 0x3541, 0);
        (*reinterpret_cast<cMyTrace*>(log1))(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            idStr, mgrState, (int)mgrIndex);
        m_manager->SetPartyIndex(-1);
        if (getManager()->getMoveSpace() != (UserSpace::T)1)
            m_manager->set_state((ch_state)3);
        m_manager->make_basic_info((char*)&packet, 0);
        destroy();
        packet.finalize(true);
        if (getManager()->getMoveSpace() == (UserSpace::T)1)
            send_to_party(packet);
        else
            G_GameWorld()->send_all(packet, m_host);
    }
}

// ============================================================================
// CParty::dungeon_clear —— ORIG 0x85ab736：胜利结算；发放三组奖励物品与金卡，
// 清空奖励槽并注册 0x15 秘商定时器。
// ============================================================================
void CParty::dungeon_clear()
{
    SaveCoin((eCoinSubReason)1);
    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
    PacketGuard packet;
    int memberCount = get_member_count();               // -0x20
    int goldCardCount = 0;                              // -0x24
    for (int slot = 0; slot <= 3; ++slot)               // -0x18
    {
        if (slot > 3)
        {
            SetEPLPState(2);
            int sec = 0x3c;                             // -0x1c
            secretshop::SECRET_SHOP_DATA& shop =
                *reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop);
            if (shop.IsOpen())
                sec = *(int*)((char*)G_CDataManager() + 0xa784) * 12;
            int key = gen_timer_key((TIMER_MESSAGE)0x15);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x15,
                                        sec, key, 0);
            set_state(1);
            sendInoutConditionDungeon(
                (RetryFailReason::T)checkInoutConditionDungeon());
            if (IsPremiumGoldCardParty())
                sendPremiumGoldCardStatistic(m_dungeon->get_standard_level(),
                                             memberCount, goldCardCount);
            CheckHackAverageHitCount();
            ClearMapHitCount();
            reinterpret_cast<Secu_HackLogCheckByParty*>(&m_padSecu)->dungeonClear();
            return;
        }
        if (_checkValidUser(slot))
        {
            CUser* user = m_member[slot].m_pUser;
            Inven_Item* g0 = bd.m_itemGroups0[slot];    // +0x3a0 组
            Inven_Item* g1 = bd.m_itemGroups1[slot];    // +0x588 组
            Inven_Item* g2 = bd.m_itemGroups2[slot];    // +0x770 组
            int put = _putItemIntoUser(user, &g0[0], packet);
            put += _putItemIntoUser(user, &g1[0], packet);
            if (HaveAlreadySelectedCard(
                    (char)slot, (eClearRewardCardType_t)1))
            {
                CItem* card = G_CDataManager()->find_item(g2[1].m_addInfo);
                if (card != 0)
                {
                    int price = 0;
                    G_Store()->GetSellItemPrice(g2[1], card,
                                                (short)g2[1].m_addInfo2,
                                                false, price);
                    GetInstanceValueStatistic()->AddValueStatistic(
                        (VALUE_STATISTIC_FIELD)5, user, (unsigned int)price);
                }
                put += _putItemIntoUser(user, &g2[0], packet);
                CEventBase* ev40 =
                    GlobalData::s_event_manager->GetRepeatEvent(0x40);
                bool giveGold = ev40->IsEventing(0) || IsReturnUserParty() ||
                                IsEventCharacParty();
                if (giveGold && !IsPremiumGoldCardParty())
                    put += _putItemIntoUser(user, &g2[0], packet);
                ++goldCardCount;
            }
            (void)put;
            g0[0].reset();
            g0[1].reset();
            g1[0].reset();
            g1[1].reset();
            g2[0].reset();
            g2[1].reset();
            bd.ResetEventDungeonClearPoint();
            HistoryLog::WriteDunClear((FILE*)user->m_field796f8);
            OnSuccessedDungeonClear(user);
        }
    }
}

// ============================================================================
// CParty::_putItemIntoUser —— ORIG 0x85aab84：双槽奖励发放（items[0] 金币/
// 主物、items[1] 装备），含背包失败邮件退还与幸运点扣减。
// ============================================================================
int CParty::_putItemIntoUser(CUser* user, Inven_Item* items, PacketGuard& packet)
{
    int count = 0;                                      // -0x48
    int pktType = 0;                                    // -0x40
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(pktType);
    int countIdx = packet.get_index();                  // -0x68
    packet.put_short(count);

    if (items[0].get_add_info() > 0)                    // 金币槽
    {
        if (499999 < items[0].get_add_info())
        {
            CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
            char log1[16];
            new (log1) cMyTrace(
                __PRETTY_FUNCTION__,
                0x232d, 4);
            (*reinterpret_cast<cMyTrace*>(log1))(
                "Dungeon(%d) reward money(%d) on %d' party member with difficulty %d",
                *(int*)((char*)this + 0xcb8), items[0].get_add_info(),
                get_member_count(), bf.get_dungeon_diff());
        }
        CInventory* inven =
            reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenW();
        int oldMoney = inven->get_money();
        int gained = inven->gain_money(items[0].get_add_info(),
                                       (eMoneyAddReason)5, true, false);
        if (gained < items[0].get_add_info())
            user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0,
                                      (unsigned long)items[0].get_add_info(),
                                      (unsigned long)gained);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)3, user, (unsigned int)gained);
        if (m_dungeon != 0)
        {
            // +0x89e：金币流水分类标志（CDungeon 域待具名）
            if (*((char*)m_dungeon + 0x89e) == 0)
                GetInstanceCirculationStatistic()->AddCirculationStatistic(
                    (CIRCULATION_STATISTIC_FIELD)1, user,
                    (unsigned int)gained);
            else
                GetInstanceCirculationStatistic()->AddCirculationStatistic(
                    (CIRCULATION_STATISTIC_FIELD)0xf, user,
                    (unsigned int)gained);
        }
        user->saveTaxMoneyForUpperMember(items[0].get_add_info());

        packet.put_short(0);
        packet.put_int(0);
        packet.put_int(inven->get_money());
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_packet(g_emptySlot);
        ++count;
    }

    int space = items[1].GetItemSpace();
    if (pktType != space)
    {
        if (0 < count)
        {
            packet.put_short(countIdx, count);
            packet.finalize(true);
            user->Send(packet);
        }
        count = 0;
        packet.clear();
        packet.put_header(0, 0xe);
        pktType = space;
        packet.put_byte(pktType);
        countIdx = packet.get_index();
        packet.put_short(count);
    }

    CUserCharacInfo* ci = reinterpret_cast<CUserCharacInfo*>(user);
    int slot = ci->getCurCharacInvenW()->insertItemIntoInventory(items[1],
                                                                (eItemAddReason)0xb,
                                                                true, true);
    bool isTournament = m_dungeon != 0 && m_dungeon->isTournamentDungeon() != 0;
    if (slot < 1)
    {
        CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
        bool ev41ing = ev41->IsEventing(0);
        bool hasItem = items[1].m_addInfo != 0 && items[1].m_addInfo != -1;
        const char* senderKey = 0;
        if (ev41ing && hasItem && !isTournament)
            senderKey = g_scriptStringManager_.findString(
                4, "game_server_msg_155", 0);       // 活动期获得
        else if (!ev41ing && hasItem && isTournament)
            senderKey = g_scriptStringManager_.findString(
                4, "game_server_msg_250", 0);       // 锦标赛背包已满退还
        if (senderKey != 0 &&
            G_CDataManager()->find_item(items[1].m_addInfo) != 0)
        {
            char title[21];                         // -0x89
            strncpy(title, senderKey, 0x14);
            char bodyText[256];                     // -0x19e
            strncpy(bodyText,
                    g_scriptStringManager_.findString(4, "game_server_msg_104", 0),
                    0xff);
            unsigned int serverGroup =
                (unsigned int)user->GetServerGroup();
            unsigned int characNo =
                (unsigned int)ci->getCurCharacNo();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, items[1], 0, characNo, bodyText,
                (int)strlen(bodyText), 8,
                (ENUM_SERVER_GROUP)serverGroup, false, false);
        }
    }
    else
    {
        packet.put_short((short)slot);
        const Inven_Item* ref =
            ci->getCurCharacInvenR()->GetInvenRef(
                items[1].GetInventoryType(), slot);
        packet.put_int(*reinterpret_cast<const int*>(reinterpret_cast<const char*>(ref) + 2));
        packet.put_int(ref->get_add_info());
        packet.put_byte((unsigned int)ref->GetItemAttr() & 0xff);
        packet.put_short((unsigned int)ref->m_fieldb & 0xffff);
        packet.put_byte((unsigned int)(unsigned char)*reinterpret_cast<const unsigned char*>(ref));
        packet.put_int(*(const int*)&ref->m_fieldd);
        packet.put_byte((unsigned int)ref->m_amp.getAbilityType() & 0xff);
        packet.put_short((unsigned int)ref->m_amp.getAbilityValue() & 0xffff);
        if (ref->isAvatarItemType())
        {
            packet.put_int(0x1e);
            const WongWork::CAvatarItemMgr* avMgrR =
                ci->getCurCharacInvenR()->GetAvatarItemMgrR();
            packet.put_binary(
                (char*)avMgrR->getJewelSocketData(ref->get_add_info()), 0x1e);
            void* expInfo = const_cast<WongWork::CAvatarItemMgr*>(avMgrR)
                                ->GetExpansionInfo(ref->get_add_info());
            packet.put_int(4);
            if (expInfo == 0)
            {
                stAvatarExpansionInfo_t init0;
                init0.init();
                packet.put_binary((char*)&init0, 4);
            }
            else
                packet.put_binary((char*)expInfo, 4);
        }
        packet.put_byte(0);
        packet.put_packet(*ref);
        ++count;

        CItem* card = G_CDataManager()->find_item(items[1].m_addInfo);
        if (card != 0)
        {
            int sellPrice = 0;
            G_Store()->GetSellItemPrice(items[1], card,
                                        (short)items[1].m_addInfo2, false,
                                        sellPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)4, user, (unsigned int)sellPrice);
        }
        // ORIG 越界读 items[1]+0x3e（Inven_Item 0x3d 后 1 字节；线格式遗留，保留）
        if ((unsigned char) *reinterpret_cast<unsigned char*>(
                reinterpret_cast<char*>(&items[1]) + 0x3e) ==
                1 &&
            card != 0)
        {
            CLuckPoint::UseLuckPoint(user, (int)(unsigned char) * reinterpret_cast<unsigned char*>(
                                         reinterpret_cast<char*>(&items[1]) + 0x3e),
                                     card->get_rarity(), card->get_grade());
        }
    }

    if (0 < count)
    {
        packet.put_short(countIdx, count);
        packet.finalize(true);
        user->Send(packet);
    }
    return count;
}

// ============================================================================
// CParty::onBloodClear —— ORIG 0x85b7c9e：血之副本单轮结算日志与成败回调。
// ============================================================================
void CParty::onBloodClear(bool flag, int param)
{
    if (flag)
    {
        unsigned int playTime = (unsigned int)GetMapPlayingTime();
        m_member[param].m_pUser->m_historyLog.MapClear(
            *(unsigned short*)((char*)this + 0x183e),
            (int)(playTime / 1000), 0);
        if (*(char*)(*(int**)&m_dungeon + 0x89f) == 1)   // 血图进度（CDungeon 域待具名）
        {
            APSystem::_ActionGroupIndex group;
            group.m_value = 0x1a;
            APSystem::CUserProc::ClearActionAndSendtoUser(
                m_member[param].m_pUser, group, 0, (ENUM_PACKETCLASS)0);
        }
        OnSuccessedDungeonClear(m_member[param].m_pUser);
    }
    else
    {
        OnFailedDungeonClear(m_member[param].m_pUser);
    }
}

// ============================================================================
// CParty::BloodClearRewardPacket —— ORIG 0x85b8bf8：0xd1 血轮奖励包
// （四组 int 对 + 组数，物品表按 0x3d 步进）。
// ============================================================================
void CParty::BloodClearRewardPacket(int a, int b, int c, Inven_Item const* items1,
                                    int d, Inven_Item const* items2, int e,
                                    Inven_Item const* items3, int f,
                                    Inven_Item const* items4)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    PacketGuard packet;
    packet.put_header(0, 0xd1);
    packet.put_byte(b);
    packet.put_byte(bf.getBloodMaxRound());
    packet.put_byte(c);
    const Inven_Item* groups[4] = {items1, items2, items3, items4};
    int counts[4] = {c, d, e, f};
    for (int g = 0; g < 4; ++g)
    {
        if (g > 0)
            packet.put_byte(counts[g]);
        for (int i = 0; i < counts[g]; ++i)
        {
            packet.put_int(
                *(const int*)(reinterpret_cast<const char*>(groups[g]) +
                              i * 0x3d + 2));
            packet.put_int(groups[g][i].get_add_info());
        }
    }
    packet.finalize(true);
    m_member[a].m_pUser->Send(packet);
}

// ============================================================================
// CParty::generateBloodRewardExp —— ORIG 0x85b7ea2：血轮经验生成
// （基础×轮次权重×进出次数权重 → getClearRewardBonusExp 加成聚合）。
// ============================================================================
void CParty::generateBloodRewardExp(int slot, int round, unsigned int baseExp,
                                    short inoutCnt)
{
    CUser* user = get_user(slot);                       // -0x1c
    unsigned int exp = 0;                               // -0x20
    char* dmBase = (char*)G_CDataManager();
    if (*(char*)(*(int**)&m_dungeon + 0x89f) == 2)      // 最终轮（CDungeon 域待具名）
    {
        long long raw = (long long)round * (long long)baseExp;
        float weight = *(float*)(dmBase + ((round + 0x178c) * 4));   // 轮权重表（待具名）
        double prod = 10.0f * weight * (double)raw;     // ORIG _DAT_08cbbbf8=10.0
        exp = (unsigned int)(long long)(prod + 0.5);
    }
    else
    {
        long long raw = (long long)round * (long long)baseExp;
        float weight = *(float*)(dmBase + ((round + 0x178c) * 4));
        // 进出次数权重 vector<float>（CDataManager 域字段待具名）
        float inoutWeight =
            *(float*)(*(char**)(dmBase + 0x6164) + (int)inoutCnt * 4);
        double prod = (double)(inoutWeight * (float)raw * weight);
        exp = (unsigned int)(long long)(prod + 0.5);
    }
    if (1000000 < exp)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3cec,
            "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u), BASE EXP (%u), "
            "EXP_WEIGHT(%f), EXP_INOUT_WEIGHT(%f), LAST_ROUND(%d), "
            "INOUT_CNT(%d)\n",
            exp, baseExp,
            *(float*)(dmBase + ((round + 0x178c) * 4)),
            *(float*)(*(char**)(dmBase + 0x6164) + (int)inoutCnt * 4),
            round, (int)inoutCnt);
    }

    stClearRewardBonusExp_t bonus;                      // -0x68
    stClearRewardExpParameter_t param;                  // -0xc8
    param.m_avatarBonus = getAvatarBonus();
    param.m_creatureBonus = getCreatureBonus(user);

    CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
    CPCRoomBurningEvent* pcr = static_cast<CPCRoomBurningEvent*>(ev8);
    if (pcr->IsEventing(user))
        param.m_pcRoomBurningRate = (float)(pcr->GetExpFactor() / 100);

    CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
    CExpDoubleEvent* expDouble = static_cast<CExpDoubleEvent*>(ev3);
    if (expDouble->IsEventing(0))
        param.m_expDoubleRate = (float)(expDouble->GetExpFactor() / 100.0f);

    CBurningFatigueEvent* fatigue =
        static_cast<CBurningFatigueEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xf));
    if (fatigue->IsEventing(0))
        param.m_burningFatigueBonusExp = fatigue->getBonusExpRate(
            reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacUsedFatigue());

    channel_script_t* chScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    unsigned int dungIdx = m_dungeon->get_index();
    CEnvironment* env = G_CEnvironment();
    unsigned int channelNo = (unsigned int)env->get_channel_no();
    unsigned char channelType = *(unsigned char*)((char*)env + 0x378);
    param.m_serverChannelRate =
        chScript->getBonusRate(channelType, channelNo, dungIdx);

    if (user->IsHavePremiumAdvantage())
        param.m_premiumAdvantageRate = (float)(
            user->GetPremiumInfo()->GetAdvantageExpRate() / 100);

    param.m_serverEventExpRate = (float)(CServerEvent::GetExpRate() / 100);
    param.m_manageBonus = CalcurateManageBonus(
        ManageBonusType::T_0, user, (unsigned int)exp);

    unsigned int bonusExp = getClearRewardBonusExp(user, exp, param, bonus);
    if (1000000 < bonusExp)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3d12, "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
            bonusExp);
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3d1c,
            "BLOOD_LOG : EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            param.m_avatarBonus, param.m_creatureBonus,
            param.m_pcRoomBurningRate, param.m_expDoubleRate,
            param.m_serverChannelRate, param.m_premiumAdvantageRate,
            param.m_serverEventExpRate, param.m_burningFatigueBonusExp);
    }

    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    bf.m_bloodReward.setRewardExp(slot, bonusExp);
}

// ============================================================================
// CParty::beginBloodClearProcess —— ORIG 0x85b75d4：血轮结算主流程
// （状态机推进 → 逐成员奖励/经验/记录包 0xd0 → 0x23 定时器）。
// ============================================================================
void CParty::beginBloodClearProcess(bool flag)
{
    // CCharacBloodDungeon 只读视图（类未建模；CUser.cpp 同款模式）
    struct BloodEnterCountView
    {
        char m_pad0[0xc];
        int m_enterCount;   // +0x0c
    };

    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    if (bf.getBloodState() >= 3)
        return;

    int playTime = 0;                                   // -0x34
    int round = 0;                                      // -0x40[0]
    int bestTime = 0;                                   // -0x30
    int bestRound = 0;                                  // -0x2c
    int ultimatePoint = 0;                              // -0x28
    playTime = bf.m_bloodReward.getPlayTime();
    if (flag)
    {
        round = bf.getBloodMaxRound();
        bf.m_bloodReward.setCurrentBloodRound(round);
    }
    else
    {
        round = bf.m_bloodReward.getCurrentBloodRound();
    }

    if (!bf.setBloodState((ENUM_BLOOD_STATE)3))
        return;

    if (flag && *(char*)(*(int**)&m_dungeon + 0x89f) == 2)
    {
        short maxRound = (short)bf.getBloodMaxRound();
        ultimatePoint =
            bf.getNextRoundDifficulty((short)(maxRound - 1));
        bf.m_bloodReward.incUltimateRoundClearPoint(ultimatePoint);
    }

    for (int slot = 0; slot < 4; ++slot)                // -0x20
    {
        if (!(get_user(slot) == 0 || !checkValidUser(slot)))
        {
            CUser* user = get_user(slot);
            user->setChattingMessageCount(0);
            onBloodClear(flag, slot);
            CUserCharacInfo* ci = reinterpret_cast<CUserCharacInfo*>(user);
            ci->set_charac_party_bonus_exp(0);
            ci->set_charac_member_bonus_exp(0);
            ci->set_charac_fatigue_buf_bonus_exp(0);
            ci->set_charac_seria_buf_bonus_exp(0);

            checkBloodRenewMyRecord(slot, playTime, round,
                                    bf.get_dungeon_index());

            int itemMax = round * 10 / bf.getBloodMaxRound();   // -0x18
            if (itemMax == 0)
                itemMax = 1;
            int itemLimit = 10;                         // -0x40[1]
            int itemCount = std::min(itemLimit, round);
            generateBloodRewardItem(slot, itemCount,
                                    (unsigned int)ci->get_charac_level(),
                                    (unsigned int)itemMax, flag);

            unsigned int mobReward = 0;                 // -0x44
            G_CDataManager()->get_mob_reward(ci->get_charac_level(),
                                             mobReward);
            QuickParty::RewardData_Param rp(this);
            QuickParty::RewardData_Result rr;
            rr.m_value00 = (int)mobReward;
            set_random_reward((QuickParty::RandomBuffType)0, rp, rr);
            mobReward = (unsigned int)rr.m_value00;

            char* dmBase = (char*)G_CDataManager();
            int inoutSize =
                (int)*(unsigned int*)(*(char**)dmBase + 0x6164 + 8);  // vector size
            BloodEnterCountView* bloodView =
                reinterpret_cast<BloodEnterCountView*>(
                    user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1));
            int enterCount = bloodView->m_enterCount;   // -0x6c
            int expLimit = inoutSize - 1;               // -0x40[2]
            int inoutCnt = std::min(expLimit, enterCount);
            generateBloodRewardExp(slot, round, mobReward,
                                   (short)inoutCnt);

            unsigned int rewardExp =
                bf.m_bloodReward.getRewardExp(slot);    // -0x10
            if (1000000 < rewardExp)
            {
                LogManager::logFormat(
                    1, "party.cpp",
                    __PRETTY_FUNCTION__, 0x3c79,
                    "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u)\n",
                    rewardExp);
                rewardExp = 1000000;
                bf.m_bloodReward.setRewardExp(slot, 1000000);
            }
            if (round == 0)
            {
                playTime = 0;
                round = 0;
            }
            bestTime = (int)ci->getBloodBestTimeRecord();
            bestRound = ci->getBloodBestRoundRecord();
            if (bestRound == -1)
            {
                bestTime = playTime;
                bestRound = round;
            }
            PacketGuard packet;                         // -0x78
            packet.put_header(0, 0xd0);
            packet.put_int(playTime);
            packet.put_int(round);
            packet.put_int(bestTime);
            packet.put_int(bestRound);
            packet.put_int(bf.getBloodMaxRound());
            packet.put_int((int)bf.m_bloodReward.getRewardExp(slot));
            packet.finalize(true);
            m_member[slot].m_pUser->Send(packet);

            int key = gen_timer_key((TIMER_MESSAGE)0x23);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x23,
                                        8, key, 0x23);
        }
    }
}

// ============================================================================
// CParty::generateBloodRewardItem —— ORIG 0x85b840e：血轮物品奖励
// （rand<1000 分稀有/普通两生成路径，终局轮追加终极奖励）。
// ============================================================================
void CParty::generateBloodRewardItem(int slot, int round, unsigned int param1,
                                     unsigned int param2, bool flag)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    bf.m_bloodReward.setRewardItem(slot, round);
    for (int i = 0; i < round; ++i)                     // -0x3c
    {
        WongWork::stGenerateRefData_t ref;              // -0x198
        WongWork::stGenerateResult_t result;            // -0xa4
        CLuckPoint luckPoint;                           // -0x98
        GetLuckPoint(&luckPoint, false);
        unsigned int roll = (unsigned int)get_rand_int(1000);   // -0x38
        char* dmBase = (char*)G_CDataManager();
        unsigned int rareLine = *(unsigned int*)(dmBase + 0x6154) * param2;   // -0x34
        unsigned int normalLine = *(unsigned int*)(dmBase + 0x6158) * param2;// -0x30

        if (roll < rareLine)
        {
            ref.m_field34 = (char)param1;               // +0x34
            ref.m_field35 = 0;                          // +0x35
            ref.m_rateSlot = 1;                         // +0x36
            int one = 1;
            memcpy(&ref.m_rankBonusIndex, &one, sizeof(one));     // +0x38 = 1（位型）
            unsigned int v15c = 0x10001;
            memcpy(&ref.m_goldBase, &v15c, sizeof(v15c));// +0x3c
            memcpy(&ref.m_dungeon, &v15c, sizeof(v15c));   // +0x40

            WongWork::stGenerateResult_t specific;      // -0xd4
            WongWork::CItemGeneratorMgr* genMgr =
                G_CDataManager()->getItemGenerator();
            genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateSpecificItem(ref, specific);
            const Inven_Item& item = *specific.m_dropItems.begin();
            bf.m_bloodReward.setRewarItemCopy(item, slot, i);

            int dungIdx =
                m_dungeon != 0 ? m_dungeon->get_index() : 0;   // -0x2c
            QuickParty::RewardData_Param rp(this);
            rp.m_param4 = dungIdx;
            QuickParty::RewardData_Result rr;
            rr.m_value00 = item.get_add_info();
            set_basic_reward((QuickParty::BasicRewardType)0xb, rp, rr);
            Inven_Item& mutable_item =
                const_cast<Inven_Item&>(*specific.m_dropItems.begin());
            mutable_item.m_addInfo2 = rr.m_value00;
            float addRate = *(float*)(dmBase + 0x6160);
            bf.m_bloodReward.setRewardItemAddInfo(addRate, slot, i);
        }
        else if (roll < normalLine)
        {
            ref.m_field34 = (char)param1;
            ref.m_field35 = 0;
            float oneF = 1.0f;
            memcpy(&ref.m_rankBonusIndex, &oneF, sizeof(oneF));   // +0x38 float=1.0（位型）
            unsigned int zero = 0;
            memcpy(&ref.m_goldBase, &zero, sizeof(zero));    // +0x3c..0x3f = 0
            int dungPtrBits = *(int*)&m_dungeon;
            memcpy(&ref.m_dungeon, &dungPtrBits, sizeof(dungPtrBits)); // +0x40

            WongWork::CItemGeneratorMgr* genMgr =
                G_CDataManager()->getItemGenerator();
            genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateRandomItem(ref, result);
            if (!result.m_dropItems.empty())
            {
                const Inven_Item& item = *result.m_dropItems.begin();
                bf.m_bloodReward.setRewarItemCopy(item, slot, i);
                CItem* card =
                    G_CDataManager()->find_item(item.m_addInfo);
                if (card != 0)
                    CLuckPoint::UseLuckPoint(m_member[slot].m_pUser,
                                             (int)(unsigned char)item.m_field1,
                                             card->get_rarity(),
                                             card->get_grade());
            }
        }
    }

    if (flag && *(char*)(*(int**)&m_dungeon + 0x89f) == 2)
    {
        int ultIdx = bf.m_bloodReward.getUltimateRewardItem();   // -0x24
        CItem* card = G_CDataManager()->find_item(ultIdx);       // -0x20
        if (card != 0)
        {
            WongWork::stGenerateResult_t specific;
            Inven_Item made;                        // -0x14e（默认构造）
            G_CDataManager()->getItemList()->create_item(
                ENUM_ITEM_CREATE_TYPE_1, made, 1);
            specific.m_dropItems.push_back(made);
            const Inven_Item& item = *specific.m_dropItems.begin();
            bf.m_bloodReward.setRewarItemCopy(item, slot, round - 1);
        }
    }
}

// ============================================================================
// CParty::onBloodRewardTimer —— ORIG 0x85b6a48：0x22/0x23/0x24/0x25 血轮
// 定时器状态机（奖励包→物品/经验发放→收尾）。
// ============================================================================
int CParty::onBloodRewardTimer(TIMER_MESSAGE msg)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);
    if (gen_timer_key(msg) == 0)
        return 0;
    if (bf.getBloodState() == 0)
        return 0;

    if ((int)msg == 0x23)
    {
        if (bf.getBloodState() < 3)
            return 0;
        for (int slot = 0; slot < 4; ++slot)            // -0x3c
        {
            if (!(get_user(slot) == 0 || !checkValidUser(slot)))
            {
                BloodClearRewardPacket(
                    slot,
                    bf.m_bloodReward.getCurrentBloodRound(),
                    bf.m_bloodReward.getRewardItemCount(3),
                    bf.m_bloodReward.getRewardItem(3),
                    bf.m_bloodReward.getRewardItemCount(2),
                    bf.m_bloodReward.getRewardItem(2),
                    bf.m_bloodReward.getRewardItemCount(1),
                    bf.m_bloodReward.getRewardItem(1),
                    bf.m_bloodReward.getRewardItemCount(0),
                    bf.m_bloodReward.getRewardItem(0));
            }
        }
        bf.setBloodState((ENUM_BLOOD_STATE)4);
        int key = gen_timer_key((TIMER_MESSAGE)0x24);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x24, 8, key, 0x24);
    }
    else if ((int)msg == 0x24)
    {
        if (bf.getBloodState() < 4)
            return 0;
        for (int slot = 0; slot < 4; ++slot)            // -0x38
        {
            if (!(get_user(slot) == 0 || !checkValidUser(slot)))
            {
                CUserCharacInfo* ci =
                    reinterpret_cast<CUserCharacInfo*>(get_user(slot));
                for (int i = 0; i < bf.m_bloodReward.getRewardItemCount(slot);
                     ++i)                               // -0x30
                {
                    Inven_Item* item = bf.m_bloodReward.getRewardItem(slot) + i;
                    int putSlot = -1;                   // -0x28
                    if (*(int*)((char*)item + 2) != -1)
                    {
                        if (*(int*)((char*)item + 2) == 0)   // 金币槽
                        {
                            CInventory* inven = ci->getCurCharacInvenW();
                            int gained =
                                inven->gain_money(item->get_add_info(),
                                                  (eMoneyAddReason)5, true,
                                                  false);
                            if (gained < item->get_add_info())
                                get_user(slot)->SendMoneyFullReason(
                                    (ENUM_MONEY_FULL_REASON)0,
                                    (unsigned long)item->get_add_info(),
                                    (unsigned long)gained);
                            putSlot = 0;
                            if (*(char*)(*(int**)&m_dungeon + 0x89f) == 1)
                                GetInstanceCirculationStatistic()
                                    ->AddCirculationStatistic(
                                        (CIRCULATION_STATISTIC_FIELD)0xb,
                                        get_user(slot),
                                        (unsigned int)gained);
                            else if (*(char*)(*(int**)&m_dungeon + 0x89f) ==
                                     2)
                                GetInstanceCirculationStatistic()
                                    ->AddCirculationStatistic(
                                        (CIRCULATION_STATISTIC_FIELD)0xc,
                                        get_user(slot),
                                        (unsigned int)gained);
                        }
                        else
                        {
                            putSlot = ci->getCurCharacInvenW()
                                          ->insertItemIntoInventory(
                                              *item, (eItemAddReason)0xb, true,
                                              true);
                        }
                    }
                    if (putSlot < 0)
                    {
                        int itemKey = *(int*)((char*)item + 2);
                        if (itemKey == 0x4e3 || itemKey == 0x4e4)
                        {
                            if (G_CDataManager()->find_item(itemKey) != 0)
                            {
                                char title[21];         // -0x65
                                strncpy(title,
                                        g_scriptStringManager_.findString(
                                            4, "game_server_msg_153", 0),
                                        0x14);
                                char bodyText[256];     // -0x165
                                strncpy(bodyText,
                                        g_scriptStringManager_.findString(
                                            4, "game_server_msg_154", 0),
                                        0xff);
                                unsigned int serverGroup =
                                    (unsigned int)get_user(slot)->GetServerGroup();
                                unsigned int characNo = (unsigned int)
                                    ci->getCurCharacNo();
                                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                    title, *item, 0, characNo, bodyText,
                                    (int)strlen(bodyText), 8,
                                    (ENUM_SERVER_GROUP)serverGroup, false,
                                    false);
                            }
                        }
                    }
                    else
                    {
                        PacketGuard packet;             // -0x50
                        packet.put_header(0, 0xe);
                        packet.put_byte(item->GetItemSpace());
                        packet.put_short(1);
                        INVEN_TYPE invenType =
                            (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)
                                item->GetItemSpace());
                        ci->getCurCharacInvenR()->MakeItemPacket(
                            invenType, putSlot, packet);
                        packet.finalize(true);
                        get_user(slot)->Send(packet);
                    }
                }

                unsigned int rewardExp =
                    (unsigned int)bf.m_bloodReward.getRewardExp(slot);   // -0x34
                if (1000000 < rewardExp)
                {
                    LogManager::logFormat(
                        1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x3bd4,
                        "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
                        rewardExp);
                    rewardExp = 1000000;
                }
                int sp = 0;                             // -0x40
                int sfp = 0;                            // -0x44
                if (get_user(slot)->gain_exp_sp(
                        (int)rewardExp, sp, sfp, (CUser::eExpAddReason)2, 0,
                        false))
                    get_user(slot)->SendNotiPacket((CUser::eSendTarget)2,
                                                   (ENUM_NOTIPACKET)2, 1);
            }
        }
        bf.setBloodState((ENUM_BLOOD_STATE)5);
        SetEPLPState(2);
        int key = gen_timer_key((TIMER_MESSAGE)0x25);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x25, 0x3c, key, 0x25);
        sendInoutConditionDungeon(
            (RetryFailReason::T)checkInoutConditionDungeon());
    }
    else if ((int)msg == 0x25)
    {
        if (GetEPLPState() != 2)
            return 0;
        if (GetSelectedEPLPCmd() == 3)
            return 0;
        finishBloodDungeon();
    }
    else if ((int)msg == 0x22)
    {
        if (bf.getBloodState() < 2)
            return 0;
        if (get_member_count() < 1)
            return 0;
        bool clearOk = bf.check_grid_clear() && bf.check_end_point() &&
                       bf.m_bloodReward.isBloodClear();
        if (!check_allmember_die())
        {
            if (clearOk)
                beginBloodClearProcess(true);
        }
        else
        {
            beginBloodClearProcess(clearOk);
        }
    }
    return 1;
}

// ============================================================================
// CParty::finishBloodDungeon —— ORIG 0x85b8f92：单行转发 ReturnToVillage。
// ============================================================================
void CParty::finishBloodDungeon()
{
    ReturnToVillage();
}
// ============================================================================
// CParty::set_clear_reward_exp_parameter —— ORIG T 0x085adb00（0x5d4 字节）
//   报告：docs/class_func_reports/CParty/set_clear_reward_exp_parameter.md
//   逐段对照 ## 汇编 + ## 反编译 C 转写。要点：
//   - PC 房间燃烧经验：GetExpFactor()/100 为【整数除法】（imul 0x51eb851f/sar 5）；
//     双倍经验 GetExpFactor()/100.0f 为浮点除（flds _DAT_08cbbbcc=100.0f）。
//   - m_pcRoomSameUserBonus(+0x34) 按原始 dword 位型写入（读取方按 float 解释）。
//   - 回归用户队伍奖励 0x3e4ccccd = 0.2f。
//   - 成长之力：get_growth_power_exp_reward 返回 dword，取 (v>>16)&0xff / 100。
//   - getAddExpRateFromPremium(0x4f)：ORIG 直呼 CUser 方法（W 0x085bfd76 =
//     WongWork::CUserPremium::GetAddExpBonus(type)：type∈[0,0x69] 时返回
//     g_SPremiumInfo[type].m_field44）；该转发链未建模为成员函数，
//     就地取权威表字段（与 sumGainedEachExp 同惯例）。
// ============================================================================
void CParty::set_clear_reward_exp_parameter(CUser* user, unsigned int exp,
                                            stClearRewardExpParameter_t& param)
{
    CDataManager* dm = G_CDataManager();                    // -0x38 local_3c
    int dungeonIndex =
        reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_index();
    CDungeon* dungeon = dm->find_dungeon(dungeonIndex);     // -0x34 local_38
    ServerParameterScript* spScript = &dm->m_serverParameter;  // -0x30 local_34（+0x68）

    if (dungeon == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x294a, "Unknown Dungeon %d",
            reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_index());
        return;
    }
    if (user == 0)
    {
        return;
    }

    // +0x096..+0x119：四项基础加成
    float avatarBonus = CalcAvatarBonus();                  // -0x2c local_30

    float pcRoomBurningRate = 0.0f;                         // -0x28 local_2c
    {
        CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
        if (ev8->IsEventing(user))
        {
            CPCRoomBurningEvent* burning = static_cast<CPCRoomBurningEvent*>(
                GlobalData::s_event_manager->GetRepeatEvent(8));
            pcRoomBurningRate = static_cast<float>(burning->GetExpFactor() / 100);
        }
    }

    float expDoubleRate = 0.0f;                             // -0x24 local_28
    {
        CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
        if (ev3->IsEventing(static_cast<CUser*>(0)))
        {
            CExpDoubleEvent* doubleEv = static_cast<CExpDoubleEvent*>(
                GlobalData::s_event_manager->GetRepeatEvent(3));
            expDoubleRate = static_cast<float>(doubleEv->GetExpFactor()) / 100.0f;
        }
    }

    float periodItemRate = 0.0f;                            // -0x20 local_24
    if (user->IsExpItemAffected() && user->EnablePeriodExpItem())
    {
        periodItemRate = user->GetExpAffectRate();
    }

    int pcRoomSameUserBonus = 0;                            // -0x1c local_20
    unsigned int pcbangNo = static_cast<unsigned int>(user->GetPCRoomNo());
    if (pcbangNo != 0 && existSamePCRoomUser(pcbangNo))
    {
        pcRoomSameUserBonus = G_CDataManager()->m_serverParameter.m_76c;  // +0x7d4
    }

    float returnUserBonus = 0.0f;                           // -0x18 local_1c
    if (IsReturnUserParty())
    {
        returnUserBonus = 0.2f;                             // ORIG 0x3e4ccccd
    }

    // +0x21a..+0x27c：param 基础字段回填
    param.m_deadPenaltyRate = calDeadUserDungeonPenalty(param.m_deadMemberCount);
    param.m_rankBonusWeight = spScript->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(param.m_rankBonusIdx));
    param.m_avatarBonus = avatarBonus;
    param.m_creatureBonus = CalcCreatureBonus(user);
    param.m_pcRoomBurningRate = pcRoomBurningRate;
    param.m_expDoubleRate = expDoubleRate;

    // +0x27f..+0x2d5：燃烧疲劳奖励
    CBurningFatigueEvent* fatigueEvent = static_cast<CBurningFatigueEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0xf));
    if (fatigueEvent->IsEventing(static_cast<CUser*>(0)))
    {
        param.m_burningFatigueBonusExp =
            fatigueEvent->getBonusExpRate(user->getCurCharacUsedFatigue());
    }

    // +0x2d8..+0x394：频道加成（服务器类型 1/2 走 getBonusRateEventServer）
    channel_script_t* channelScript =
        static_cast<channel_script_t*>(dm->GetChannelScript());
    CEnvironment* env = G_CEnvironment();
    int serverType = env->m_serverEnvir.m_serverType;          // CEnvironment+0x1a8
    unsigned char channelByte =
        env->m_serverEnvir.m_serverGroup;                       // CEnvironment+0x378
    if (serverType == 1 || serverType == 2)
    {
        param.m_serverChannelRate = channelScript->getBonusRateEventServer(
            channelByte, static_cast<unsigned int>(env->get_channel_no()));
    }
    else
    {
        param.m_serverChannelRate = channelScript->getBonusRate(
            channelByte, static_cast<unsigned int>(env->get_channel_no()),
            static_cast<unsigned int>(dungeon->get_index()));
    }

    // +0x397..+0x3cd：会员/服务器/全局效果加成
    if (user->IsHavePremiumAdvantage())
    {
        param.m_premiumAdvantageRate =
            static_cast<float>(user->GetPremiumInfo()->GetAdvantageExpRate()) /
            100.0f;
    }
    param.m_serverEventExpRate =
        static_cast<float>(CServerEvent::GetExpRate()) / 100.0f;
    if (GlobalData::s_globalEffectManager_->isAffectedEffect(2))
    {
        param.m_globalEffectRate =
            GlobalData::s_globalEffectManager_->getEffectValue(2);
    }

    // +0x41a..+0x429：网吧同号/回归用户奖励（dword 位型写入）
    *reinterpret_cast<int*>(&param.m_pcRoomSameUserBonus) = pcRoomSameUserBonus;
    param.m_returnUserPartyBonus = returnUserBonus;

    // +0x42c..+0x474：事件角色队伍经验比例
    if (!IsEventCharacParty())
    {
        param.m_eventCharacExpRatio = 0.0f;
    }
    else
    {
        EventNewCharacterReward* newCharEv = reinterpret_cast<EventNewCharacterReward*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xa2));
        if (newCharEv != 0)
        {
            param.m_eventCharacExpRatio = newCharEv->eventExpRatio();
        }
    }

    // +0x477..+0x498：管理加成
    param.m_manageBonus =
        CalcurateManageBonus(static_cast<ManageBonusType::T>(0), user, exp);

    // +0x49e..+0x4dd：成长之力（STExpReward 局部回填 dword0 后取字节 2）
    STExpReward growthTmp;                                     // -0x3c
    growthTmp.m_reward = user->get_growth_power_exp_reward();  // sret dword → +0x00
    param.m_growthPowerExpRate =
        static_cast<float>((growthTmp.m_reward >> 16) & 0xff) / 100.0f;

    // +0x4e0..+0x530：会员额外经验（premium type 0x4f）
    if (!user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
    {
        param.m_premiumAddExpRate = 0.0f;
    }
    else
    {
        param.m_premiumAddExpRate =
            static_cast<float>(g_SPremiumInfo[0x4f].m_field44) /
            100.0f;
    }

    // +0x533..+0x57b：周期经验道具（与重叠加成事件互斥清零）
    param.m_periodExpItemRate = periodItemRate;
    if (param.m_periodExpItemRate != 0.0f &&
        GlobalData::s_event_manager->IsExistOverlappedExpBonusEvent())
    {
        param.m_periodExpItemRate = 0.0f;
    }

    // +0x57b..+0x5c7：清零四项角色 bonus exp
    user->set_charac_party_bonus_exp(0);
    user->set_charac_member_bonus_exp(0);
    user->set_charac_fatigue_buf_bonus_exp(0);
    user->set_charac_seria_buf_bonus_exp(0);
}
// ============================================================================
// CParty::clear_reward —— ORIG T 0x085ae0d4（0x3119 字节）
//   报告：docs/class_func_reports/CParty/clear_reward.md（## 汇编 + ## 反编译 C
//   逐段对照转写）。浮点常量（df_game_r .rodata）：_DAT_08cbbbcc=100.0f、
//   _DAT_08cbbbec=10000.0f、0x08cbbbf0=double 100.0；fistp 就近舍入序列按工程
//   惯例以 (int)/(long long)(x+0.5) 转写。
//   stGenerateRefData_t 调用上下文区按 ORIG 原始偏移写入：
//     +0x34..+0x37 dword：第一遍 stdLevel/diff 打包，第二遍 getGoldCardCreateRate/
//     mapRatio 位型覆盖；+0x3c..+0x3f dword：金币字段打包与掉落概率组号复用；
//     +0x40 CDungeon*（第二遍填充）。
//   奖励槽视图：BattleData::m_itemGroups{0,1,2}[slot][2]（this+0x3a0/+0x588/+0x770）。
// ============================================================================
void CParty::clear_reward(int param)
{
    CDataManager* dm = G_CDataManager();
    int dungIdx =
        reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_index();
    CDungeon* dungeon = dm->find_dungeon(dungIdx);
    int memberCount = get_member_count();

    if (dungeon == 0)
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a07, "Unknown Dungeon %d",
                              reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_index());
        return;
    }
    if (memberCount < 1)
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a0c, "No Member[%d] in party", memberCount);
        return;
    }
    if (dungeon->isTournamentDungeon())
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a13, "TournamentDungeon(%d) can not call function",
                              reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->get_dungeon_index());
        return;
    }

    BattleData& bd = *reinterpret_cast<BattleData*>(&m_padBattleData);
    CBattle_Field& bfView = *reinterpret_cast<CBattle_Field*>(&m_padBattleField1);

    int rankBonusIdx = dm->getRankBonusIndex(param);
    ServerParameterScript* spScript =
        &dm->m_serverParameter;                              // dm+0x68 子对象

    int stdLevel = 0;
    if (dungeon->get_dimension_possible() > 0)
        stdLevel = getStandardDimensionLevel();
    else
        stdLevel = dungeon->get_standard_level();

    bd.m_field68c += 1;

    int gridCount = 0;
    MazeScript* maze = bfView.getMaze();
    for (int gy = 0; gy < maze->m_width; ++gy)
    {
        for (int gx = 0; gx < maze->m_height; ++gx)
        {
            if (maze->getGridR(gy, gx).m_gridValue != 0)
                gridCount += 1;
        }
    }
    float mapRatio =
        static_cast<float>(bd.m_field68c) / static_cast<float>(gridCount);

    WongWork::CClearReward* clearGen = static_cast<WongWork::CClearReward*>(
        dm->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_2));
    int ratePerMapMax = clearGen->getRewardItemRatePerMapMaxCount(gridCount);
    if (ratePerMapMax != 0)
    {
        mapRatio = static_cast<float>(
                       static_cast<unsigned long long>(ratePerMapMax)) *
                   mapRatio / 10000.0f;
    }

    WongWork::stGenerateRefData_t ref;
    ref.m_field35 = static_cast<char>(bfView.get_dungeon_diff());
    ref.m_field34 = static_cast<char>(stdLevel);
    ref.m_rateSlot = static_cast<char>(memberCount);
    ref.m_rankBonusIndex = rankBonusIdx;
    ref.m_goldBase = static_cast<unsigned short>(bd.m_field48);
    ref.m_goldFactor = static_cast<unsigned short>(bd.m_killedMonster1);
    // ORIG 死写 m_dungeon 低/高半字（随后 ref.m_dungeon = dungeon 覆盖）
    *reinterpret_cast<unsigned short*>(&ref.m_dungeon) = 0;
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&ref.m_dungeon) + 2) =
        static_cast<unsigned short>(bd.m_killedMonster2);

    WongWork::stGenerateResult_t result;
    if (dungeon->m_89e == 0)
    {
        clearGen->generateSpecificItem(ref, result);
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            if (_checkValidUser(i))
            {
                clearGen->power_war_dungeon_gold_clear_reward(
                    ref, result, m_member[i].m_pUser->get_charac_level());
            }
        }
    }
    increase_result_card_gold(result);

    ref.m_field35 = static_cast<char>(bfView.get_dungeon_diff());
    ref.m_field34 = static_cast<char>(stdLevel);
    *reinterpret_cast<float*>(&ref.m_rankBonusIndex) = mapRatio;  // float 位型
    ref.m_dungeon = dungeon;

    int minStdDropRate = 100;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            ServiceRestrictManager* restrictMgr =
                ARAD::Singleton<ServiceRestrictManager>::Get();
            if (restrictMgr->isRestricted(RestrictCategory::Enum_1, 1))
            {
                minStdDropRate =
                    std::min(m_member[i].m_pUser->getStdDropRate(), minStdDropRate);
            }
        }
    }

    char guildTodayFlag[4] = {0, 0, 0, 0};
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser->IsTodayGuildMember())
        {
            guildTodayFlag[i] = 1;
            for (int j = 0; j <= 3; ++j)
            {
                if (_checkValidUser(j) &&
                    m_member[i].m_pUser->get_charac_guildkey() ==
                        m_member[j].m_pUser->get_charac_guildkey())
                {
                    guildTodayFlag[j] = 1;
                }
            }
        }
    }

    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))
        {
            continue;
        }
        CUser* user = m_member[slot].m_pUser;

        if (dungeon->m_entryRestrictFlag != 0 &&
            user->IsDeleteEventDungeonInvitation())
        {
            user->EventDungeonClearReward(dungeon->m_vec86c);
        }
        unsigned int newRankIdx = dm->getNewRankIndex(param);
        if (newRankIdx < 3)
        {
            APSystem::_ActionGroupIndex group;
            group.m_value = 0x10;
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, group, 0, static_cast<ENUM_PACKETCLASS>(0));
        }

        Inven_Item* rewardItems = bd.m_itemGroups0[slot];
        if (dungeon->m_89e == 0)
        {
            int lvl = user->get_charac_level();
            double basePenalty = dm->BaseExpPenalty(lvl, stdLevel);
            if (basePenalty >= 1.0 || dungeon->isRiskDungeon())
            {
                unsigned short guildLvl = static_cast<unsigned short>(
                    static_cast<const STGuildDBInfoOnly*>(
                        user->GetGuildDBInfo())->m_memberCount);  // +0x22
                unsigned long long tempExp =
                    static_cast<unsigned long long>(user->get_guild_temp_exp());
                float weight = dm->m_guildParameter.getExpWeight(guildLvl);
                int addExp = static_cast<int>(
                    static_cast<double>(weight) *
                        static_cast<double>(tempExp) + 0.5);
                if (guildTodayFlag[slot] == 0)
                {
                    user->add_guild_exp(addExp);
                }
                else
                {
                    addExp += static_cast<int>(
                        dm->m_guildParameter.m_fieldFC / 100.0 *
                            static_cast<double>(addExp) + 0.5);
                    user->add_guild_today_exp(addExp);
                }
            }
        }
        else
        {
            int lvl = user->get_charac_level();
            ref.m_field34 = static_cast<char>(lvl);
            unsigned short guildLvl = static_cast<unsigned short>(
                static_cast<const STGuildDBInfoOnly*>(
                    user->GetGuildDBInfo())->m_memberCount);  // +0x22
            unsigned long long tempExp =
                static_cast<unsigned long long>(user->get_guild_temp_exp());
            float weight = dm->m_guildParameter.getExpWeight(guildLvl);
            int addExp = static_cast<int>(
                static_cast<double>(weight) * static_cast<double>(tempExp) + 0.5);
            if (guildTodayFlag[slot] == 0)
            {
                user->add_guild_exp(addExp);
            }
            else
            {
                addExp += static_cast<int>(
                    dm->m_guildParameter.m_fieldFC / 100.0 *
                        static_cast<double>(addExp) + 0.5);
                user->add_guild_today_exp(addExp);
            }
        }
        user->reset_guild_temp_exp();

        {
            ServiceRestrictManager* restrictMgr =
                ARAD::Singleton<ServiceRestrictManager>::Get();
            if (restrictMgr->isRestricted(RestrictCategory::Enum_1, 1))
            {
                ref.m_dropRate = user->getStdDropRate();
            }
        }

        {
            std::vector<Inven_Item>& items = result.m_dropItems;
            if (!items.empty())
            {
                rewardItems[0].setCopy(items.front());
            }
        }

        if (m_liveFlag[slot] != 1 && m_enterMapFlag[slot] != 1 &&
            m_field388[slot] != 1)
        {
            bd.m_arr64[slot] += 1;
        }

        float deadPenalty = calDeadUserDungeonPenalty(slot);
        if (deadPenalty > 0.0f)
        {
            int addInfo = rewardItems[0].get_add_info();
            rewardItems[0].set_add_info(static_cast<int>(
                (1.0f - deadPenalty) * static_cast<float>(addInfo) + 0.5f));
            m_field38c[slot] = 1;
        }

        {
            int addInfo = rewardItems[0].get_add_info();
            rewardItems[0].set_add_info(static_cast<int>(
                static_cast<double>(static_cast<float>(minStdDropRate) / 100.0f) *
                    static_cast<double>(addInfo) + 0.5));
        }

        *reinterpret_cast<int*>(&ref.m_goldBase) =               // +0x3c 起 4 字节
            user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 3 : 0;
        WongWork::stGenerateResult_t genResult;
        GetLuckPoint(reinterpret_cast<CLuckPoint*>(&genResult.m_luckPoint), false);
        clearGen->generateRandomItem(ref, genResult);
        if (!genResult.m_dropItems.empty())
        {
            rewardItems[1].setCopy(genResult.m_dropItems.front());
        }

        *reinterpret_cast<int*>(&ref.m_goldBase) =               // +0x3c 起 4 字节
            user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 4 : 1;
        Inven_Item* goldCardItems = bd.m_itemGroups2[slot];
        goldCardItems[0].setCopy(rewardItems[0]);
        goldCardItems[0].set_add_info(0);

        bool goldCardPath = true;
        if (clearGen == 0)
        {
            goldCardPath = false;
        }
        else
        {
            CEventBase* ev10 = GlobalData::s_event_manager->GetRepeatEvent(0x10);
            if (ev10->IsEventing(static_cast<CUser*>(0)))
                goldCardPath = false;
        }
        if (goldCardPath)
        {
            float commissionRate = 1.0f;
            CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
            if (ev41->IsEventing(static_cast<CUser*>(0)) && IsPremiumGoldCardParty())
            {
                commissionRate = *reinterpret_cast<float*>(
                    &G_CDataManager()->m_serverParameter
                         .m_premiumGoldCard4894.m_field0);   // +0x48fc float 位型
            }
            CGameManager* gameMgr = G_CGameManager();
            CConditionEventManager* condMgr = gameMgr->GetConditionEventManager();
            if (condMgr != 0)
            {
                int step = user->GetCurConditionEventStep();
                if (condMgr->IsFreeGoldCard(user, static_cast<char>(step + 1)))
                    commissionRate = 0.0f;
            }
            int commission = clearGen->getGoldCardCommission(stdLevel);
            m_field1e4[1][slot] = static_cast<int>(
                static_cast<float>(commission) * commissionRate + 0.5f);
            float createRate = clearGen->getGoldCardCreateRate();
            *reinterpret_cast<int*>(&ref.m_field34) =           // +0x34 起 4 字节
                *reinterpret_cast<const int*>(&createRate);
        }

        genResult.m_dropItems.clear();
        {
            CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
            if (ev41->IsEventing(static_cast<CUser*>(0)) && IsPremiumGoldCardParty())
            {
                clearGen->generatePremiumGoldCardItem(
                    ref, genResult, static_cast<ENUM_RARITY>(-1),
                    GetPremiumGoldCardDefaultItem());
            }
            else
            {
                clearGen->generateRandomItem(ref, genResult);
                if (!genResult.m_dropItems.empty() &&
                    (IsReturnUserParty() || IsEventCharacParty()))
                {
                    clearGen->generatePremiumGoldCardItem(
                        ref, genResult, static_cast<ENUM_RARITY>(1), 10000);
                }
            }
        }

        if (genResult.m_dropItems.empty())
        {
            goldCardItems[1].reset();
            CEventBase* ev13 = GlobalData::s_event_manager->GetRepeatEvent(0x13);
            if (ev13->IsEventing(static_cast<CUser*>(0)))
            {
                clearGen->generateGoldCardBlankItem(goldCardItems[1]);
            }
        }
        else
        {
            goldCardItems[1].setCopy(genResult.m_dropItems.front());
            if (goldCardItems[1].m_addInfo == -1 || dungeon->m_89e != 0)
            {
                goldCardItems[1].reset();
            }
        }

        if (dungeon->m_villageAttackEventFlag != 0)
        {
            Inven_Item eventItem;
            eventItem.m_addInfo =
                G_CDataManager()->m_serverParameter.m_field4a4;  // +0x50c
            dm->getItemList()->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), eventItem, 1);
            goldCardItems[0].reset();
            goldCardItems[1].reset();
            rewardItems[0].reset();
            rewardItems[1].reset();
            rewardItems[1].setCopy(eventItem);
            bd.ResetEventDungeonClearPoint();
        }

        *reinterpret_cast<float*>(&ref.m_field34) = mapRatio;   // +0x34 起 4 字节
        if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)))
        {
            Inven_Item* pcRoomItems = bd.m_itemGroups1[slot];
            pcRoomItems[0].setCopy(rewardItems[0]);
            pcRoomItems[0].set_add_info(0);
            *reinterpret_cast<int*>(&ref.m_goldBase) = 5;       // +0x3c 起 4 字节
            genResult.m_dropItems.clear();
            clearGen->generateRandomItem(ref, genResult);
            if (genResult.m_dropItems.empty())
            {
                pcRoomItems[1].reset();
                WongWork::CClearReward* pcRoomGen =
                    static_cast<WongWork::CClearReward*>(dm->getItemGenerator()->getGenerator(
                        WongWork::CItemGeneratorMgr::eGenerateType_2));
                if (pcRoomGen != 0)
                {
                    CEventBase* ev1d =
                        GlobalData::s_event_manager->GetRepeatEvent(0x1d);
                    if (ev1d->IsEventing(static_cast<CUser*>(0)))
                    {
                        pcRoomGen->generatePcRoomCardBlankItem(pcRoomItems[1]);
                    }
                }
            }
            else
            {
                pcRoomItems[1].setCopy(genResult.m_dropItems.front());
            }
        }
    }

    unsigned int mobReward = 0;
    if (dungeon->m_89e != 1)
    {
        dm->get_mob_reward(stdLevel, mobReward);
    }
    float partyBonusRate = spScript->m_expGroup368[memberCount - 1];
    partyBonusRate +=
        static_cast<CPartyExpBonusEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x18))
            ->getBonusRate(memberCount, isHelpAbuseParty());

    unsigned int totalMobExp = 0;
    if (dungeon->m_89e != 1)
    {
        int killCount = bd.GetTotalKilledMonsterCount();
        float baseExp = static_cast<float>(killCount) *
                        static_cast<float>(mobReward) / 2.0f * partyBonusRate;
        float diffWeight =
            spScript->m_monsterExpTable398[bfView.get_dungeon_diff()];
        double weighted =
            static_cast<double>(baseExp) * static_cast<double>(diffWeight);
        totalMobExp = static_cast<unsigned int>(static_cast<long long>(
            weighted * dungeon->get_exp_weight() + 0.5));
    }

    PacketGuard pkBasic;
    PacketGuard pkReward;
    std::list<map_item> questItemList;
    Inven_Item questItem;

    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))
        {
            continue;
        }
        CUser* user = m_member[slot].m_pUser;

        user->setChattingMessageCount(0);

        if (dungeon->m_89e != 0)
        {
            dm->get_mob_reward(user->get_charac_level(), mobReward);
            int killCount = bd.GetTotalKilledMonsterCount();
            float baseExp = static_cast<float>(killCount) *
                            static_cast<float>(mobReward) / 2.0f * partyBonusRate;
            float diffWeight =
                spScript->m_monsterExpTable398[bfView.get_dungeon_diff()];
            double weighted =
                static_cast<double>(baseExp) * static_cast<double>(diffWeight);
            totalMobExp = static_cast<unsigned int>(static_cast<long long>(
                weighted * dungeon->get_exp_weight() + 0.5));
        }

        int userLevel = user->get_charac_level();
        unsigned int personalExp = 0;
        if (dungeon->m_89e == 1)
        {
            personalExp = totalMobExp / memberCount;
        }
        else if (!dungeon->isRiskDungeon())
        {
            double penalty = dm->BaseExpPenalty(userLevel, stdLevel);
            double scaled = static_cast<double>(
                static_cast<float>(static_cast<double>(
                    static_cast<unsigned long long>(totalMobExp)) * penalty) *
                static_cast<float>(MemberPenaltyRate()));
            personalExp = static_cast<unsigned int>(static_cast<long long>(
                scaled / memberCount + 0.5));
        }
        else
        {
            double scaled = static_cast<double>(
                static_cast<float>(static_cast<double>(
                    static_cast<unsigned long long>(totalMobExp)) *
                    static_cast<float>(MemberPenaltyRate())));
            personalExp = static_cast<unsigned int>(static_cast<long long>(
                scaled / memberCount + 0.5));
        }

        QuickParty::RewardData_Param rewardParam(this);
        QuickParty::RewardData_Result rewardResult;
        rewardResult.m_value00 = static_cast<int>(personalExp);
        set_random_reward(static_cast<QuickParty::RandomBuffType>(0), rewardParam,
                          rewardResult);
        personalExp = static_cast<unsigned int>(rewardResult.m_value00);

        stClearRewardExpParameter_t expParam;
        expParam.m_deadMemberCount = slot;
        expParam.m_rankBonusIdx = rankBonusIdx;
        set_clear_reward_exp_parameter(user, personalExp, expParam);
        stClearRewardBonusExp_t bonus;
        unsigned int bonusTotal =
            getClearRewardBonusExp(user, personalExp, expParam, bonus);

        bool helpAbuse = isHelpAbuseParty(user);
        user->calcHelpAbuseRatio(bonusTotal, helpAbuse);
        user->reset_level_before_dungeon();

        int spPoints = 0;
        int sfpPoints = 0;
        bool leveledUp = user->gain_exp_sp(
            static_cast<int>(bonusTotal), spPoints, sfpPoints,
            static_cast<CUser::eExpAddReason>(2), 0, false);
        if (leveledUp)
        {
            pkBasic.clear();
            pkBasic.put_header(0, 2);
            pkBasic.put_byte(1);
            pkBasic.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&pkBasic), 1);
            pkBasic.finalize(true);
            user->Send(pkBasic);
        }

        if (user->IsHavePremiumAdvantage())
        {
            float premiumDeduction;   // ORIG quirk: flds -0x2cc has no writer
            int deduction = static_cast<int>(premiumDeduction + 0.5f);
            user->m_character.m_f28 -= deduction;
            user->m_character.m_f34 -= deduction;
        }

        questItemList.clear();

        pkReward.clear();
        pkReward.put_header(0, 0x23);
        pkReward.put_int(bonus.m_channelBonusExp + static_cast<int>(personalExp));
        pkReward.put_int(bonus.m_rankWeightExp);
        user->set_charac_party_bonus_exp(static_cast<int>(bonus.m_rankWeightExp));

        unsigned int extraExp = 0;
        {
            float denom = partyBonusRate;
            float bonusRate =
                static_cast<CPartyExpBonusEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0x18))
                    ->getBonusRate(memberCount, isHelpAbuseParty());
            if (personalExp != 0)
            {
                int sub = static_cast<int>(
                    static_cast<float>(personalExp) / (bonusRate + denom) +
                    0.5f);
                unsigned int sum = bonus.m_avatarBonusExp +
                                   bonus.m_rankWeightExp +
                                   bonus.m_selectedBonusExp +
                                   bonus.m_channelBonusExp +
                                   bonus.m_premiumAdvExp +
                                   bonus.m_serverEventExp +
                                   bonus.m_serverChannelExp +
                                   bonus.m_creatureBonusExp;
                extraExp = (personalExp - static_cast<unsigned int>(sub)) *
                           (sum / personalExp + 1);
            }
        }
        pkReward.put_int(static_cast<int>(extraExp));
        pkReward.put_int(bonus.m_avatarBonusExp);
        pkReward.put_byte(bonus.m_bestType);
        pkReward.put_int(bonus.m_selectedBonusExp + bonus.m_serverEventExp +
                         bonus.m_burningFatigueExp);
        pkReward.put_int(bonus.m_periodItemExp);
        pkReward.put_int(bonus.m_premiumAdvExp);
        pkReward.put_int(bonus.m_pcRoomSameUserExp);
        pkReward.put_int(bonus.m_serverChannelExp);
        pkReward.put_int(bonus.m_upperMemberExp);
        pkReward.put_int(bonus.m_creatureBonusExp);
        pkReward.put_int(bonus.m_returnUserExp);
        pkReward.put_int(bonus.m_eventCharacExp);
        pkReward.put_int(bonus.m_manageExp);
        pkReward.put_int(bonus.m_growthPowerExp);
        pkReward.put_int(bonus.m_premiumAddExp);
        int questCountPos = pkReward.get_index();
        pkReward.put_byte(0);

        int questPutCount = 0;
        for (int qi = 0; qi < 0x14; ++qi)
        {
            UserQuest* questRef = user->getCurCharacQuestRefR();
            if (questRef->m_quest[qi] == 0 || questRef->m_trigger[qi] == 0)
            {
                continue;
            }
            Quest* quest = dm->find_quest(questRef->m_quest[qi]);
            if (quest == 0)
            {
                continue;
            }
            short characClass = questRef->m_authenData.m_characClass;
            if (!quest->check_clear_item(bfView.get_dungeon_index(),
                                         static_cast<int>(characClass)))
            {
                continue;
            }
            int requiredCount = questRef->m_quest[qi];
            if (requiredCount <= 0)
            {
                continue;
            }
            std::vector<std::pair<int, int> > clearItems(
                quest->get_clear_itemlist(bfView.get_dungeon_index()));
            for (unsigned int ci = 0; ci < clearItems.size(); ++ci)
            {
                questItem.reset();
                std::pair<int, int> entry = clearItems.at(ci);
                dm->getItemList()->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(1), questItem,
                    entry.second);
                CInventory* inventory = user->getCurCharacInvenW();
                int putSlotNo = inventory->insertItemIntoInventory(
                    questItem, static_cast<eItemAddReason>(0xb), true, true);
                if (putSlotNo >= 0)
                {
                    pkReward.put_short(putSlotNo);
                    pkReward.put_int(entry.first);
                    pkReward.put_int(questItem.get_add_info());
                    pkReward.put_short(static_cast<int>(questItem.m_fieldb));
                    pkReward.put_byte(static_cast<int>(
                        static_cast<unsigned char>(
                            questItem.m_amp.getAbilityType())));
                    pkReward.put_short(
                        static_cast<int>(questItem.m_amp.getAbilityValue()));
                    questPutCount += 1;
                    if (questItem.m_field1 == 1)
                    {
                        CItem* itemDef = dm->find_item(entry.first);
                        if (itemDef != 0)
                        {
                            CLuckPoint::UseLuckPoint(
                                user,
                                static_cast<int>(questItem.m_field1),
                                itemDef->get_rarity(),
                                itemDef->get_grade());
                        }
                    }
                }
            }
        }
        if (questPutCount != 0)
        {
            pkReward.put_byte(questCountPos, questPutCount);
        }

        for (int j = 0; j <= 3; ++j)
        {
            if (_checkValidUser(j))
            {
                _makeRewardItemInfo(m_member[j].m_pUser, bd.m_itemGroups0[j],
                                    pkReward);
            }
            else
            {
                pkReward.put_byte(0);
            }
        }
        pkReward.put_int(m_field1e4[1][slot]);

        for (int j = 0; j <= 3; ++j)
        {
            (void)_checkValidUser(j);
            pkReward.put_byte(0);
        }

        for (int j = 0; j <= 3; ++j)
        {
            if (_checkValidUser(j))
            {
                if (!m_member[j].m_pUser->isAffectedPremium(
                        static_cast<ENUM_PREMIUM_TYPE>(0xc)))
                {
                    pkReward.put_byte(0);
                }
                else
                {
                    _makeRewardItemInfo(m_member[j].m_pUser,
                                        bd.m_itemGroups1[j], pkReward);
                }
            }
            else
            {
                pkReward.put_byte(0);
            }
        }

        int hellCoinValue = static_cast<int>(
            100.0f * bd.m_hellPartyValueTotal /
                static_cast<float>(bfView.m_hellPartyRatio0) + 0.5f);
        pkReward.put_int(hellCoinValue);

        bool returnFlag = false;
        if (user->GetParty() != 0)
        {
            returnFlag =
                reinterpret_cast<CParty*>(user->GetParty())->IsReturnUserParty();
        }
        pkReward.put_byte(returnFlag ? 1 : 0);

        bool eventFlag = false;
        if (user->GetParty() != 0)
        {
            eventFlag =
                reinterpret_cast<CParty*>(user->GetParty())->IsEventCharacParty();
        }
        pkReward.put_byte(eventFlag ? 1 : 0);

        pkReward.put_byte(expParam.m_deadPenaltyRate != 0.0f ? 1 : 0);

        pkReward.finalize(true);
        user->Send(pkReward);

        if (m_dungeon != 0 && user != 0 && m_partyType != 1 &&
            user->getCurCharacR() != 0)
        {
            bool properDungeon = dungeon->m_bloodDungeonType == 0 &&
                                 dungeon->get_dimension_possible() == 0 &&
                                 dungeon->m_89e != 1 &&
                                 !dungeon->isTowerOfDespairDungeon();
            if (properDungeon && user->IsProperDungeonUser())
            {
                HeroMissionEvent* heroMission =
                    ARAD::Singleton<HeroMissionEvent>::Get();
                heroMission->processMission(
                    user, static_cast<HeroMissionCondition::MissionNo::T>(1), 1);
                user->IncProperDungeonClearCount();
                CGameManager* gameMgr2 = G_CGameManager();
                CConditionEventManager* condMgr2 =
                    gameMgr2->GetConditionEventManager();
                if (condMgr2 != 0)
                {
                    condMgr2->ProcessCheckStepUp(user, 2);
                }
            }
        }
        if (m_dungeon != 0 && user != 0 && m_partyType != 1 &&
            user->getCurCharacR() != 0)
        {
            bool properLevel = dungeon->m_bloodDungeonType == 0 &&
                               dungeon->m_89e != 1 &&
                               !dungeon->isTowerOfDespairDungeon();
            if (properLevel && user->IsProperLevelDungeonUser())
            {
                user->ProcProperLevelDungeonClear();
            }
        }
    }

    if (IsPremiumGoldCardParty())
    {
        PacketGuard goldCardPacket;
        goldCardPacket.put_header(0, 0x12a);
        goldCardPacket.finalize(true);
        send_to_party(goldCardPacket);
    }
    if (dungeon->m_entryRestrictFlag == 0)
    {
        DungeonPermission(param);
    }
}

Party::Party()
    : IData(), PlayInfo()
{
    Reset();
}

Party::~Party()
{
}

void Party::Reset()
{
    PlayInfo::Reset();
    m_f3c = 0;
}
