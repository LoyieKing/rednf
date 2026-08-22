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
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "CPartyTelePort.h"
#include "LogManager.h"

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
#include "CBattle_Field.h"
#include "GameWorld.h"
// MapInfo 经 CBattle_Field.h→WarField.h 引入；显式析构/构造带模板参数需 typedef。
typedef std::vector<MapInfo> MapInfoVec;
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDungeonEntranceLog.h"
#include "GameTypes.h"

#include "EventNewCharacterReward.h"
#include "GlobalData.h"

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
    new (&m_padElection) cElection<int,4,4>();
    new (&m_pad1dc) CPartyResultRecvFlag;
    new (&m_pad210) CTraceMobDieHack;
    new (&m_padShop) secretshop::SECRET_SHOP_DATA();
    new (&m_padBattleData) BattleData();
    new (&m_padBattleField1) CBattle_Field();
    new (&m_padResult) GameResultSet();
    new (&m_padMap) std::map<int,int>();
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
    reinterpret_cast<MapII*>(&m_padMap)->~MapII();
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
    m_titleIndex = idx;
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

void CParty::checkBossRoom()
{
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->check_end_point();
}

void CParty::checkStartRoom()
{
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->check_start_point();
}

void CParty::DeleteDungeonDropItem(int idx)
{
    reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->pickup_item(idx);
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
                if (_checkValidUser(i) != 1)
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
    LogManager::logFormat(1, "party.cpp", "bool CParty::change_manager()",
                          0x689, "there is no manager(party empty)");
    return 0;
}

// CParty::IsExistInvisible @ 0x85b6410 —— 任一有效成员当前角色不可见则 true。
bool CParty::IsExistInvisible()
{
    int i = 0;
    while (true)
    {
        if (i > 3)
            return false;
        if (_checkValidUser(i) && m_member[i].m_pUser->IsCurCharacVisible() != 1)
            return true;
        ++i;
    }
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
    reinterpret_cast<MapII*>(&m_padMap)->clear();
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

// CParty::GetUserPosition @ 0x85b9e20 —— 返回 user 所在成员槽位，找不到返回 -1。
int CParty::GetUserPosition(CUser* user)
{
    int i = 0;
    while (true)
    {
        if (i > 3)
            return -1;
        if (_checkValidUser(i) && m_member[i].m_pUser == user)
            return i;
        ++i;
    }
}

// CParty::CheckEnterEventDungeon @ 0x859ee66 —— 事件地下城条件（必备/奖励材料）。
bool CParty::CheckEnterEventDungeon(CDungeon const* dungeon, unsigned char& flag,
                                    Inven_Item* items, int* param)
{
    if (((dungeon->m_requiredItem2.m_0 != -1) ||
         (dungeon->m_requiredItem1.m_0 != -1)) &&
        ((dungeon->m_requiredItem2.m_0 == -1) ||
         ((dungeon->m_requiredItem2.m_8 == 1) ||
          (dungeon->m_requiredItem1.m_0 == -1) ||
          (dungeon->m_requiredItem1.m_8 == 1))))
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i) != 1)
                continue;
            char local_d = 1;
            int* pi = &param[i];
            *pi = m_member[i].m_pUser->getCurCharacInvenR()
                      ->check_item_exist(dungeon->m_requiredItem2.m_0);
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
                if (piv->m_addInfo2 < dungeon->m_requiredItem2.m_4)
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
                          ->check_item_exist(dungeon->m_requiredItem1.m_0);
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
                    if (piv->m_addInfo2 < dungeon->m_requiredItem1.m_4)
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
        bool hellDun = false;
        if ((worldMap != 0 &&
             worldMap->IsInHellDungeon() != 0 &&
             reinterpret_cast<CBattle_Field*>(&m_padBattleField1)->IsEnableHellDungeon() != 0) &&
            *(int*)((char*)this + 0xcd4) == 1)
        {
            hellDun = true;
        }
        if (hellDun)
        {
            std::vector<int> vec;
            int sealR = CheckDestroyConditionSealDoor(
                m_dungeon, vec, worldMap->m_field34);
            if (sealR == 1)
                *(char*)((char*)this + 0x1865) = 1;   // ORIG +0x1865
            else
                *(char*)((char*)this + 0x1865) = 0;   // ORIG +0x1865
            if (sealR != 1)
                return 8;
        }
        bool noFat = true;
        if (m_partyType == 1 ||
            (worldMap != 0 && worldMap->hasDeathTower() == 1) ||
            m_dungeon->isNoFatigueDungeon() == 1 ||
            m_dungeon->m_800 == 1)
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
    if (dungeon->m_87a == 0)
        return 0;
    int memberCount = get_member_count();
    if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
        return 0x15;
    if (dungeon->m_879 < memberCount || memberCount < dungeon->m_878)
        return 0xab;
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
            continue;
        CUser* u = get_user(i);
        unsigned char cur = u->GetCurRevengeDungeonCount();
        if (cur == 0 || (unsigned char)dungeon->m_87b < cur)
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
    // ORIG: mov 0x8(%eax,%edx,4)，索引 (msg-0xb+0x50)，基址 this+8
    int idx = (int)msg - 0xb;
    idx += 0x50;
    return ((int*)((char*)this + 8))[idx] == param;
}

int CParty::gen_timer_key(TIMER_MESSAGE msg)
{
    // ORIG: 同 check_timer_key 地址形态（timer key 数组，位于 m_title 区）
    int idx = (int)msg - 0xb;
    idx += 0x50;
    int* key = &((int*)((char*)this + 8))[idx];
    *key = *key + 1;
    return *key;
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
    char ch = GetAssaultState();
    int state = ch;
    if (state <= 1 || state > 6)
        return true;
    else
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
int CParty::getMemberNames(char* buf) { return (int)_getMemberNames(buf); }
char* CParty::_getMemberNames(char* buf)
{
    char nameBuf[34];
    const char* name;
    if (m_manager == 0)
    {
        name = "NoMember";
    }
    else
    {
        name = m_manager->getCurCharacName();
    }
    sprintf(buf, "\"%s\",", name);
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
        new (log1) cMyTrace("bool CParty::_checkValidUser(int)", 0x37cf, 5);
        (*reinterpret_cast<cMyTrace*>(log1))(
            "[PARTYPOOL] m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
            manager, (unsigned int)dungIndex, pIndex, cState);
        for (int i = 0; i <= 3; ++i)
        {
            if (m_member[i].m_pUser == 0)
            {
                char log2[16];
                new (log2) cMyTrace("bool CParty::_checkValidUser(int)", 0x37e5, 5);
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
                new (log3) cMyTrace("bool CParty::_checkValidUser(int)", 0x37da, 5);
                (*reinterpret_cast<cMyTrace*>(log3))(
                    "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
                    i, user, chState, uid, uid2, characNo);
            }
        }
        char log4[16];
        int uid = (int)m_member[idx].m_pUser->GetUID();
        unsigned int accId = m_member[idx].m_pUser->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        new (log4) cMyTrace("bool CParty::_checkValidUser(int)", 0x37ed, 5);
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
        new (log) cMyTrace("void CParty::ReturnToVillage()", 0x2714, 0);
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
            "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)", 0x56d,
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
        new (log) cMyTrace("int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)", 0x5b8, 0);
        (*reinterpret_cast<cMyTrace*>(log))(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            accIdStr, userState, (int)partyIdx);
        if (count < 1)
        {
            int cState = (char)m_state;
            int pIndex = m_partyIndex;
            unsigned short dIdx = m_dungIndex;
            LogManager::logFormat(1, "party.cpp",
                "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)", 0x5c5,
                "[PARTYPOOL] member_count : %d, m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
                count, m_manager, (unsigned int)dIdx, pIndex, cState);
            for (int i = 0; i <= 3; ++i)
            {
                if (m_member[i].m_pUser == 0)
                {
                    LogManager::logFormat(1, "party.cpp",
                        "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)", 0x5cd,
                        "User[%d] NULL, ch_state NONE, uid : %d, charac_no : 0",
                        i, m_member[i].m_field8);
                }
                else
                {
                    LogManager::logFormat(1, "party.cpp",
                        "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)", 0x5d8,
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
    if (*(unsigned char*)((char*)dungeon + 0x85c) != 0)   // ORIG +0x85c（SurvivalRewardData 后间隙）
    {
        if (CheckEnterEventDungeon(dungeon, flag, items, param) != 1)
            return 0;
        return 1;
    }
    if (dungeon->m_requiredItem1.m_0 == -1)
        return 1;
    if (dungeon->m_requiredItem1.m_8 != 1)
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
                ->check_vilmark_dungeon_item_exist(dungeon->m_requiredItem1.m_0);
            if (param[i] == -1)
            {
                flag = (unsigned char)i;
                return 0;
            }
        }
        else
        {
            param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                ->check_item_exist(dungeon->m_requiredItem1.m_0);
            if (param[i] == -1)
            {
                flag = (unsigned char)i;
                return 0;
            }
        }
        items[i] = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(1, param[i]);
        if (items[i].m_addInfo2 < dungeon->m_requiredItem1.m_4)
        {
            flag = (unsigned char)i;
            return 0;
        }
    }
    return 1;
}
bool CParty::UseAncientDungeonItems(CDungeon const* dungeon, Inven_Item* items, int* param)
{
    if (m_partyType != 1 && dungeon->m_requiredItem1.m_8 != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i) != 1)
                continue;
            int itemIdx = dungeon->m_requiredItem1.m_4;
            int slot = param[i];
            CInventory* inv = m_member[i].m_pUser->getCurCharacInvenW();
            char r = (char)inv->delete_item((INVEN_TYPE)1, slot, itemIdx,
                                            (eItemDelReason)0xe, true);
            if (r != 1)
            {
                int itemIdx2 = dungeon->m_requiredItem1.m_4;
                int slot2 = param[i];
                int characNo = m_member[i].m_pUser->getCurCharacNo();
                char log[16];
                new (log) cMyTrace(
                    "bool CParty::UseAncientDungeonItems(const CDungeon*, Inven_Item*, int*)",
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
            int diff = items[i].m_addInfo2 - dungeon->m_requiredItem1.m_4;
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
