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
#include <new>

#include "CParty.h"
#include "CDungeon.h"
#include "InterfacePacketBuf.h"

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ============================================================================
// 依赖子对象 / 外部函数（asm-label extern；链接桩由主 agent / 后续批次提供）
// ============================================================================
extern "C" void sub_cElection_ctor(void* thisp) asm("_ZN9cElectionIiLi4ELi4EEC1Ev");
extern "C" void sub_SECRET_SHOP_DATA_ctor(void* thisp) asm("_ZN10secretshop16SECRET_SHOP_DATAC1Ev");
extern "C" void sub_SECRET_SHOP_DATA_dtor(void* thisp) asm("_ZN10secretshop16SECRET_SHOP_DATAD1Ev");
extern "C" void sub_SECRET_SHOP_DATA_clear(void* thisp) asm("_ZN10secretshop16SECRET_SHOP_DATA5clearEv");
extern "C" void sub_BattleData_ctor(void* thisp) asm("_ZN10BattleDataC1Ev");
extern "C" void sub_BattleData_dtor(void* thisp) asm("_ZN10BattleDataD1Ev");
extern "C" void sub_BattleData_Reset(void* thisp) asm("_ZN10BattleData5ResetEv");
extern "C" void sub_BattleData_SetHellPartyValueTotal(void* thisp, int v) asm("_ZN10BattleData22SetHellPartyValueTotalEi");
extern "C" void sub_CBattle_Field_ctor(void* thisp) asm("_ZN13CBattle_FieldC1Ev");
extern "C" void sub_CBattle_Field_dtor(void* thisp) asm("_ZN13CBattle_FieldD1Ev");
extern "C" void sub_CBattle_Field_SetParty(void* thisp, void* party) asm("_ZN13CBattle_Field8SetPartyEP6CParty");
extern "C" void sub_CBattle_Field_check_end_point(void* thisp) asm("_ZN13CBattle_Field15check_end_pointEv");
extern "C" void sub_CBattle_Field_check_start_point(void* thisp) asm("_ZN13CBattle_Field17check_start_pointEv");
extern "C" void sub_CBattle_Field_pickup_item(void* thisp, int idx) asm("_ZN13CBattle_Field11pickup_itemEi");
extern "C" void sub_CBattle_Field_reset_hell_party_value(void* thisp) asm("_ZN13CBattle_Field22reset_hell_party_valueEv");
extern "C" void sub_CBattle_Field_reset_field(void* thisp) asm("_ZN13CBattle_Field11reset_fieldEv");
extern "C" void sub_GameResultSet_ctor(void* thisp) asm("_ZN13GameResultSetC1Ev");
extern "C" void sub_GameResultSet_dtor(void* thisp) asm("_ZN13GameResultSetD1Ev");
extern "C" void sub_std_map_ctor(void* thisp) asm("_ZNSt3mapIiiSt4lessIiESaISt4pairIKiiEEEC1Ev");
extern "C" void sub_std_map_dtor(void* thisp) asm("_ZNSt3mapIiiSt4lessIiESaISt4pairIKiiEEED1Ev");
extern "C" void sub_Secu_HackLogCheckByParty_ctor(void* thisp) asm("_ZN24Secu_HackLogCheckByPartyC1Ev");
extern "C" void sub_Secu_HackLogCheckByParty_dtor(void* thisp) asm("_ZN24Secu_HackLogCheckByPartyD1Ev");
extern "C" void sub_Secu_HackLogCheckByParty_Init(void* thisp, void* party) asm("_ZN24Secu_HackLogCheckByParty4InitEP6CParty");
extern "C" void sub_CPartyTelePort_ctor(void* thisp) asm("_ZN14CPartyTelePortC1Ev");
extern "C" void sub_CPartyTelePort_dtor(void* thisp) asm("_ZN14CPartyTelePortD1Ev");
extern "C" void sub_CPartyTelePort_init(void* thisp, void* party) asm("_ZN14CPartyTelePort4initEP6CParty");
extern "C" void sub_std_vector_MapInfo_ctor(void* thisp) asm("_ZNSt6vectorI7MapInfoSaIS0_EEC1Ev");
extern "C" void sub_std_vector_MapInfo_dtor(void* thisp) asm("_ZNSt6vectorI7MapInfoSaIS0_EED1Ev");

extern "C" void* sub_G_CGameManager() asm("_Z14G_CGameManagerv");
extern "C" void sub_CGameManager_PutParty(void* mgr, void* party) asm("_ZN12CGameManager8PutPartyEP6CParty");
extern "C" void sub_GameWorld_send_party_info_to_all(void* world, void* party, int v) asm("_ZN9GameWorld22send_party_info_to_allEP6CPartyi");
extern "C" void* sub_G_GameWorld() asm("_Z11G_GameWorldv");
extern "C" void sub_CHackAnalyzer_reportHackInfo(void* analyzer) asm("_ZN8WongWork13CHackAnalyzer14reportHackInfoEv");
extern "C" void sub_CHackAnalyzer_resetHackInfo(void* analyzer) asm("_ZN8WongWork13CHackAnalyzer13resetHackInfoEv");
extern "C" int sub_CDungeon_get_index(void* dungeon) asm("_ZNK8CDungeon9get_indexEv");
extern "C" void* sub_GetInstanceDungeonEntranceLog() asm("_Z29GetInstanceDungeonEntranceLogv");
extern "C" void sub_CDungeonEntranceLog_DecrementDungeonEntrance(void* log, int idx, bool flag) asm("_ZN19CDungeonEntranceLog24DecrementDungeonEntranceEib");

// CUserCharacInfo（CUser.h 权威头已有方法；无权威头的走 asm extern）
extern "C" int sub_CUserCharacInfo_get_charac_level(void* uci) asm("_ZNK15CUserCharacInfo16get_charac_levelEv");
extern "C" void* sub_CUserCharacInfo_getCurCharacR(void* uci) asm("_ZNK15CUserCharacInfo13getCurCharacREv");

// CUser（CUser.h 缺失方法 -> asm extern；调用点形态按 ORIG 压栈）
extern "C" void sub_CUser_SendPacket(void* user, int target, void* packet) asm("_ZN5CUser10SendPacketENS_11eSendTargetER11PacketGuard");
extern "C" void sub_CUser_SendNotiPacket(void* user, int target, int cmd, int param) asm("_ZN5CUser14SendNotiPacketENS_11eSendTargetE15ENUM_NOTIPACKETi");
extern "C" void sub_CUser_gain_exp_sp(void* user, int exp, int& a, int& b, int reason, int c, bool d) asm("_ZN5CUser10gain_exp_spEiRiS0_13eExpAddReasonib");

// 全局数据 / 自由函数
extern "C" void* sub_G_CDataManager() asm("_Z14G_CDataManagerv");
extern "C" void* sub_CDataManager_get_hellparty_script_values(void* mgr) asm("_ZN12CDataManager27get_hellparty_script_valuesEv");

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
    if (idx < 0 || idx > 3)
        return 0;
    return m_memberRoutingState[idx];
}

void CParty::CItemRoutingData::SetMemberRoutingState(int idx, char state)
{
    if (idx < 0 || idx > 3)
        return;
    m_memberRoutingState[idx] = state;
}

// ============================================================================
// CParty 构造 / 析构
// ============================================================================
CParty::CParty()
{
    sub_cElection_ctor(&m_padElection);
    sub_SECRET_SHOP_DATA_ctor(&m_padShop);
    sub_BattleData_ctor(&m_padBattleData);
    sub_CBattle_Field_ctor(&m_padBattleField1);
    sub_GameResultSet_ctor(&m_padResult);
    sub_std_map_ctor(&m_padMap);
    sub_Secu_HackLogCheckByParty_ctor(&m_padSecu);
    sub_CPartyTelePort_ctor(&m_pad1ad0);
    sub_std_vector_MapInfo_ctor(&m_padPassedMap);
    for (int i = 0xb; i <= 0x2f; ++i)
        m_field148[i - 0xb] = 0;
    m_usedCoinCount = 0;
    sub_CBattle_Field_SetParty(&m_padBattleField1, this);
    SetAssaultState(0);
    m_routingData.Reset();
    sub_CPartyTelePort_init(&m_pad1ad0, this);
    m_randomBuffType = 0xb;
    m_isQuickParty = 0;
    init_quick_party_data();
    sub_Secu_HackLogCheckByParty_Init(&m_padSecu, this);
    m_partyMemberCoinLimit = 0;
}

CParty::~CParty()
{
    sub_std_vector_MapInfo_dtor(&m_padPassedMap);
    sub_CPartyTelePort_dtor(&m_pad1ad0);
    sub_Secu_HackLogCheckByParty_dtor(&m_padSecu);
    sub_std_map_dtor(&m_padMap);
    sub_GameResultSet_dtor(&m_padResult);
    sub_CBattle_Field_dtor(&m_padBattleField1);
    sub_BattleData_dtor(&m_padBattleData);
    sub_SECRET_SHOP_DATA_dtor(&m_padShop);
}

void CParty::send_to_party(PacketGuard& packet) {}  // TODO(G2)

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

char CParty::getTitleIndex()
{
    return m_titleIndex;
}

void CParty::setTitleIndex(unsigned char idx)
{
    m_titleIndex = (char)idx;
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
    return m_autoCreated != 0;
}

void CParty::SetAutoCreated(bool flag)
{
    m_autoCreated = flag ? 1 : 0;
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
    m_recvResultFlag = flag ? 1 : 0;
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
    return m_firstMapClear != 0;
}

void CParty::SetFirstMapClear(bool flag)
{
    m_firstMapClear = flag ? 1 : 0;
}

bool CParty::IsPremiumGoldCardParty()
{
    return m_premiumGoldCardParty != 0;
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
    sub_CBattle_Field_check_end_point(&m_padBattleField1);
}

void CParty::checkStartRoom()
{
    sub_CBattle_Field_check_start_point(&m_padBattleField1);
}

void CParty::DeleteDungeonDropItem(int idx)
{
    sub_CBattle_Field_pickup_item(&m_padBattleField1, idx);
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
    return m_isQuickParty != 0;
}

void CParty::set_quick_party(bool flag)
{
    m_isQuickParty = flag ? 1 : 0;
}

void CParty::set_gm_random_buff_type(QuickParty::RandomBuffType type)
{
    m_randomBuffType = (int)type;
}

int CParty::get_random_buff_type()
{
    return m_randomBuffType;
}

bool CParty::IsWeekendEvent()
{
    return m_weekendEvent != 0;
}

void CParty::SetWeekendEvent(bool flag)
{
    m_weekendEvent = flag ? 1 : 0;
}

bool CParty::getDungeonMapSaving()
{
    return m_dungeonMapSaving != 0;
}

void CParty::setDungeonMapSaving(bool flag)
{
    m_dungeonMapSaving = flag ? 1 : 0;
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
    return m_routingData.GetRoutingState() != 0;
}

void CParty::SetStartRouting()
{
    if (m_routingData.GetRoutingState() != 1)
        m_routingData.SetRoutingState(1);
}

void CParty::SetEndRouting()
{
    m_routingData.SetRoutingState(0);
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
        if (_checkValidUser(i) == 1)
        {
            CUser* user = m_member[i].m_pUser;
            sub_CHackAnalyzer_reportHackInfo(user->getHackAnalyzer());
            sub_CHackAnalyzer_resetHackInfo(user->getHackAnalyzer());
        }
    }
    m_memberLevelGap = -1;
    m_recvResultFlag = 0;
    memset(m_title, 0, 0x20);
    m_titleIndex = (char)(get_rand_int(0xff) + 1);
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
    sub_BattleData_SetHellPartyValueTotal(
        &m_padBattleData,
        *(int*)((char*)sub_CDataManager_get_hellparty_script_values(sub_G_CDataManager()) + 8));
    sub_CBattle_Field_reset_hell_party_value(&m_padBattleField1);
    sub_BattleData_Reset(&m_padBattleData);
    sub_CBattle_Field_reset_field(&m_padBattleField1);
    reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)->Clear();
    m_usedCoinCount = 0;
    m_field1868 = 0;
    m_straightVictories = 0;
    sub_SECRET_SHOP_DATA_clear(&m_padShop);
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
        packet.put_header(9, 0);
        packet.put_short(1);
        packet.put_short(GetPartyIndex());
        packet.put_byte(3);
        packet.finalize(true);
        sub_CUser_SendPacket(m_manager, 1, &packet);
    }
    else
    {
        sub_GameWorld_send_party_info_to_all(sub_G_GameWorld(), this, 3);
    }
    init();
    sub_CGameManager_PutParty(sub_G_CGameManager(), this);
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
                if (_checkValidUser(i))
                {
                    int idx = sub_CDungeon_get_index(m_dungeon);
                    sub_CDungeonEntranceLog_DecrementDungeonEntrance(
                        sub_GetInstanceDungeonEntranceLog(), idx, false);
                }
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
    return m_tournamentVictory != 0;
}

char CParty::GetTournamentDungeonClearState()
{
    return m_tournamentDungeonClearState;
}

void CParty::SetTournamentDungeonClearState(char state)
{
    if (state <= 6)
    {
        m_tournamentDungeonClearState = state;
    }
}

int CParty::set_host()
{
    m_host = m_manager;
    return 1;
}

bool CParty::check_timer_key(TIMER_MESSAGE msg, int param)
{
    return ((int*)m_title)[(int)msg + 0x45] == param;
}

int CParty::gen_timer_key(TIMER_MESSAGE msg)
{
    int* key = &((int*)m_title)[(int)msg + 0x45];
    *key = *key + 1;
    return *key;
}

void CParty::resetFinishLoadingVar()
{
    for (int i = 0; i < 4; i++)
    {
        m_padBattleData[i + 0x50] = 0;
    }
}

bool CParty::CanDoPartyActionAssalutState()
{
    char state = GetAssaultState();
    if (state < 2 || 6 < state)
    {
        return true;
    }
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

int CParty::_IsCannotConnectP2P(int a, int b)
{
    if (m_member[a].m_fielde[b] != 0 && _checkValidUser(b) != 1)
    {
        return 0;
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
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i) == 1 && m_padBattleData[i + 0x54] != 0)
        {
            return false;
        }
    }
    return true;
}

int CParty::get_live_count()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i) == 1 && m_padBattleData[i + 0x54] != 0)
        {
            count++;
        }
    }
    return count;
}

int CParty::get_live_count_enter_map()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i) == 1 && m_padBattleData[i + 0x58] != 0)
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
        for (int i = 0; i < 4; i++)
        {
            if (_checkValidUser(i) == 1)
            {
                int level = ((CUserCharacInfo*)m_member[i].m_pUser)->get_charac_level();
                minLevel = std::min(minLevel, level);
                maxLevel = std::max(maxLevel, level);
            }
        }
        int gap = maxLevel - minLevel;
        if (gap < 0)
        {
            gap = -gap;
        }
        m_memberLevelGap = gap;
    }
    return m_memberLevelGap;
}

int CParty::getMemberLevelGap()
{
    return _getMemberLevelGap();
}

void CParty::increase_dungeon_coin_count(int param)
{
    if (m_dungeon != 0 &&
        m_dungeon->m_87c != -1)
    {
        SetUsedCoinCount(param);
    }
}

bool CParty::IsReturnUserParty() { return false; }
void CParty::send_invite_bluemarble(int) {}
int CParty::get_member_count() { return 0; }
int CParty::getMemberNames(char* buf) { buf[0] = 0; return 0; }
char CParty::_checkValidUser(int) { return 0; }
bool CParty::checkValidUser(int) { return false; }
unsigned int CParty::GetMemberSlotNo(CUser const*) const { return 0; }
void CParty::Reset_party_overlapped_drop_ratio() {}
void CParty::ReturnToVillage() {}
void CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE) {}
bool CParty::IsEventCharacParty() { return false; }
bool CParty::CheckEnterAncientDungeon(CDungeon const*, unsigned char&, Inven_Item*, int*) { return false; }
bool CParty::UseAncientDungeonItems(CDungeon const*, Inven_Item*, int*) { return false; }
