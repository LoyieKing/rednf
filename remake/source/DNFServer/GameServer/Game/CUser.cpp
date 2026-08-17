// ============================================================================
// df_game_r 还原 —— CUser（G1-3 批次，在线用户会话聚合根）
// 布局见 CUser.h；本文件按 ORIG 反汇编（docs/class_func_reports/CUser.md）
// 逐函数转录，identical 口径验证：source/toolchain/check_tu_game_orig.sh。
// ============================================================================

#include <string.h>

#include "CUser.h"
#include "CInventory.h"
#include "SkillSlot.h"
#include "WarRoom.h"
#include "PacketBuf.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "CItemAmplifier.h"
#include <algorithm>

// ---- 外部符号声明（对应 TU 翻译后移除） ----
extern unsigned char _NS_PI_2ND_GetDefaultRandomHashKey();

extern void GetPacketName(unsigned char area, unsigned short packetId);

class CGameManager
{
public:
    static void* GetParty(CGameManager* mgr);
    static void* GetWarRoom(CGameManager* mgr);
};

extern CGameManager* G_CGameManager();

namespace ARAD
{
namespace DISPATCHER
{
void make_cmd_packetheader_jpn(PacketGuard& packet, int cmd, int param);
}
}

namespace item_lock
{
class CItemLock
{
public:
    static int CheckItemLock(CExpandEquipslot* data);
};
}

// ---- G1-3 临时外部桩（对应 TU 翻译后移除） ----
unsigned char _NS_PI_2ND_GetDefaultRandomHashKey() { return 0; }
void GetPacketName(unsigned char, unsigned short) {}
void ARAD::DISPATCHER::make_cmd_packetheader_jpn(PacketGuard&, int, int) {}
void* CGameManager::GetParty(CGameManager* mgr) { return 0; }
void* CGameManager::GetWarRoom(CGameManager* mgr) { return 0; }
void WongWork::CDungeonClear::clear() {}
void WongWork::CMCAPManager::resetExposedCount() {}
int CUser::AntibotSend(PacketBuf& packet) { return 1; }
void Secu_AccountHacking::resetInfo() {}
int item_lock::CItemLock::CheckItemLock(CExpandEquipslot* data) { return 0; }
short WongWork::CUserPremium::GetAdvantageFatigueRate() const { return 0; }
void cUserHistoryLog::SetUser(CUser* user) {}
bool mission_list_is_clear(void* data) { return false; }
void CInventory_MakeItemPacket(CInventory* inven, int space, int slot,
                               PacketGuard& packet)
{
}

void CUser::_379::reset()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
}

void CUser::resetInformNoticeFlag()
{
    memset(&m_field8ebb1, 0, 1);
}

void CUser::disableSaveInformNoticeFlag()
{
    m_informNoticeFlag = 0;
}

void CUser::enableSaveInformNoticeFlag()
{
    m_informNoticeFlag = 1;
}

namespace WongWork
{
class CDeathTowerRanking
{
public:
    static unsigned int getRankTable(unsigned int type);
    static void unregistBestRecord(unsigned int table);
    static void unregistRanking(unsigned int table);
};
class CMailBox
{
public:
    void Init();
};
}

namespace Sanicova
{
class CPad_impl
{
public:
    int getCancelCnt();
    void setCancelCnt(int count);
};
}

namespace APSystem
{
struct _SIG_LOAD_ACTION_POINT
{
    char m_pad[4];
};
class DB_UpdateActionPoint
{
public:
    static void makeRequest(unsigned int uid, const _SIG_LOAD_ACTION_POINT* data,
                            bool save);
};
class CUserProc
{
public:
    static void SetTodayActionAndCheckMedalReward(CUser* user);
};
}

class DB_LogBuyItem
{
public:
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            unsigned int itemIdx, unsigned int type,
                            unsigned int value);
};
void DB_LogBuyItem::makeRequest(unsigned int, unsigned int, unsigned int,
                                unsigned int, unsigned int)
{
}

class DB_SecurityCardUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt);
};

class DB_PassPadUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt,
                            const char* webAddress);
};

namespace CerashopAddRestrict
{
}

namespace WongWork
{
void CMailBoxHelper::FreeMailBox(CMailBox* box) {}
void CUserPremium::InitPremium() {}
void CHackAnalyzer::resetHackInfo() {}
void CHackAnalyzer::resetServerHackAccumulatedCnt(ENUM_HACKTYPE type) {}
void CSecurityCard::resetCancelCnt() {}
int CSecurityCard::getCancelCnt() { return 0; }
void CAvatarItemMgr_Reset(CUser*) {}
void CDeathTowerRanking_unreg(unsigned int) {}
}

void UserMercenaryInfoMgr::AddCharac(const CHARAC_LOAD_MERCENARY& info) {}
void UserMercenaryInfoMgr::RemoveCharac(unsigned int characNo) {}
void UserMercenaryInfoMgr::SetCharac(unsigned int characNo,
                                     const CHARAC_LOAD_MERCENARY& info)
{
}
bool UserMercenaryInfoMgr::IsCompetition(unsigned int characNo) const
{
    return false;
}
bool UserMercenaryInfoMgr::IsReturnCharac(unsigned int characNo) const
{
    return false;
}

CodeHackCheckStorage::CodeHackCheckStorage() {}
CodeHackCheckStorage::~CodeHackCheckStorage() {}
charac_expand::CDataMgr::CDataMgr() {}
charac_expand::CDataMgr::~CDataMgr() {}
advancealtar::CharacAdvanceAltarManager::CharacAdvanceAltarManager() {}
advancealtar::CharacAdvanceAltarManager::~CharacAdvanceAltarManager() {}
PIReceiverManager::PIReceiverManager() {}
PIReceiverManager::~PIReceiverManager() {}
PISenderManager::PISenderManager() {}
PISenderManager::~PISenderManager() {}
void CCharacterView::reset() {}
void user_creature::CCreatureMgr::TurnStomach(bool fighting) {}
CExpandEquipslot* charac_expand::CDataMgr::GetData(ENUM_CHARAC_EXPAND_TYPE type) const
{
    return 0;
}
CExpandEquipslot* charac_expand::CDataMgr::GetDataR(ENUM_CHARAC_EXPAND_TYPE& type) const
{
    return 0;
}
void CodeHackCheckStorage::reset() {}
void charac_expand::CDataMgr::reset() {}
void BingoData::clear() {}
void APSystem::CActionPointManager::Reset() {}
GrowthCreatureEvent::GrowthCreatureEvent() {}
CharacLevelUpGiftAccountOnce::CharacLevelUpGiftAccountOnce() {}
void PlayInfo::Reset()
{
    m_f00 = 0;
    m_f04 = 0;
    m_f08 = 0;
    m_f0c = 0;
    m_f10 = 0;
    m_f14 = 0;
    m_f18 = 0;
    m_f1c = 0;
    m_f20 = 0;
    m_f24 = 0;
    m_f28 = 0;
    m_f2c = 0;
    m_f30 = 0;
    m_f34 = 0;
    m_f38 = 0;
}
unsigned int WongWork::CDeathTowerRanking::getRankTable(unsigned int type)
{
    return 0;
}
void WongWork::CDeathTowerRanking::unregistBestRecord(unsigned int table) {}
void WongWork::CDeathTowerRanking::unregistRanking(unsigned int table) {}
void WongWork::CMailBox::Init() {}

// ============================================================================
// CUser 实现
// ============================================================================

CUser::CUser()
    : m_field704a8(0), m_field79670(0), m_field796d8(0), m_field8cfc4(0)
{
    m_field8ec24 = 0;
    m_field8ec2c = 0;
    m_premium.m_mailBox = 0;
    m_field796d9 = 0;
    m_field8cfd0 = 300;
    m_field8cfd4 = 0;
    m_field8cfd8 = 0;
    setAntibotKey(-1);
    setClientVersion(0);
    m_mcap.m_field84 = 0;
    m_mcap.m_field90 = 0;
    m_codeHackCheck.m_field54 = 0;
    m_field796f8 = 0;
    reset();
    m_historyLog.SetUser(this);
    m_field705d8 = 0;
    m_field8e094 = 0;
    ResetUsedCoinInDungeon();
    m_piSender.m_field10 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_field8eb18 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_accountCargo.Clear();
    m_field8eb1c = 0;
    ClearItemSellCount();
    m_field8eb44 = 0;
    m_field8eb48 = 0;
    m_field8eb99 = 0;
    resetInformNoticeFlag();
    disableSaveInformNoticeFlag();
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_rentalInfo.clear();
    m_field8cfcc = (UserSpace::T)0;
    m_codeHackCheck.m_field30 = 0;
    m_field8d012 = 0xffff;
    m_field8eba8 = 0;
    m_field8ebac = 0;
    m_field8ebb0 = 0;
    m_field8ec29 = 0;
    m_rentalFlag3 = 0;
}

CUser::~CUser()
{
    m_expReward.m_field6 = 0;
}

void CUser::reset()
{
    ResetCurCharac();
    m_field796cc = 0;
    m_field796d0 = 0;
    m_field8cf00 = 0;
    m_field8ead8 = 0;
    m_premium.InitPremium();
    m_characList.clear();
    m_characList.reserve(8);
    m_guildAgitDB.m_pad[2] = 0;
    m_characterView.reset();
    m_hackAnalyzer.resetHackInfo();
    for (int i = 0x65; i < 0xfa1; ++i)
        getHackAnalyzer()->resetServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)i);
    m_accHacking.resetInfo();
    if (m_premium.m_mailBox != 0)
    {
        WongWork::CMailBoxHelper::FreeMailBox((WongWork::CMailBox*)m_premium.m_mailBox);
        m_premium.m_mailBox = 0;
    }
    m_list711ec.clear();
    InitFinishPoint();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    m_guildAgitDB.m_pad[1] = 0;
    m_codeHackCheck.reset();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
    setUserEventCharacterFlag(0);
    m_vec8cef4.clear();
    m_field8d0f0 = 0;
    m_field8d0f1 = 0;
    m_field8d0f2 = 0;
    m_character.m_f40 = 0;
    m_character.Reset();
    m_field8d0f8 = 0;
    m_field8d0f4 = 0;
    m_379.reset();
    setAntibotKey(-1);
    setClientVersion(0);
    m_dungeonGainedGold.reset();
    m_codeHackCheck.m_field44 = 0;
    m_character.m_f44 = 0;
    m_field8cfc0 = 0xffffffff;
    m_codeHackCheck.m_field46 = 0;
    m_levelupEvents.clear();
    m_characExpand.reset();
    m_premium.m_field98c = 0;
    m_limitItemUsage.clear();
    m_quest.m_field761c = 0;
    m_field704ac = 0;
    m_codeHackCheck.m_field59 = 0;
    m_accountCargo.Clear();
    m_codeHackCheck.m_field5c = 0;
    m_codeHackCheck.m_field62 = 0;
    m_codeHackCheck.m_field64 = 0;
    m_str79664[0].clear();
    m_str79664[1].clear();
    m_codeHackCheck.m_field5a = 0;
    m_codeHackCheck.m_field60 = 0;
    ResetUsedCoinInDungeon();
    DBUpdateDBLogItem();
    ResetDBLogItem();
    m_actionPoint.Reset();
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
    reset_server_fatigue_data();
    reset_ingame_event_history();
    reset_ingame_event_history_update();
    m_mercenaryMgr.m_characs.clear();
    m_network.m_padTail[0] = 0;
    m_network.m_padTail[1] = 0xff;
    resetMoneyLog();
    m_expReward.m_field4 = 0;
    m_expReward.m_field5 = 0;
    m_field8ec28 = 0xff;
    m_field796d8 = 0;
    m_quest.reset();
    m_field8ebd8 = 0;
    m_field8ebd4 = m_field8ebd8;
    m_field8ebd0 = m_field8ebd4;
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_bingo.clear();
    m_levelUpGift.m_map.clear();
    m_growthCreature.m_vec.clear();
    GetGameOptionRef()->reset_load_hotkey_flag();
    m_rentalFlag3 = 0;
    m_field8ec31 = 0;
    m_field8ec32 = 0;
}

void CUser::ResetCurCharac()
{
    getPVPResultRefW()->Clear();
    getBattleRecordRefW()->clear();
    m_list8cf80.clear();
    getDungeonClearRefW()->clear();
    m_field796cc = 0;
    m_field796d0 = 0;
    if (getCurCharacR() != 0)
    {
        ((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrW()->Reset();
        ((SkillSlot*)getCurCharacSkillR())->clear_all_skills_both();
        ((CCargo*)getCurCharacCargoR())->reset();
        getCurCharacQuestR()->reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->Reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SetUser(this);
        if (isDisguiseCharac() != 0)
            setDisguiseCharac(false, 0, 0);
    }
    reset_guild_temp_exp();
    reset_guild_exp();
    reset_level_before_dungeon();
    for (int i = 1; i < 5; ++i)
    {
        getCurCharacNo();
        WongWork::CDeathTowerRanking::unregistBestRecord(
            WongWork::CDeathTowerRanking::getRankTable(i));
        getCurCharacNo();
        WongWork::CDeathTowerRanking::unregistRanking(
            WongWork::CDeathTowerRanking::getRankTable(i));
    }
    resetCurCharacSchoolPoint();
    if (getCurCharacR() != 0)
    {
        GameWorld* pWorld = G_GameWorld();
        int job = get_charac_job();
        int* pCnt = (int*)((char*)pWorld + 4 + (job + 0x58) * 4);
        *pCnt = *pCnt - 1;
    }
    setCurCharac(0);
    setTagCharac(0);
    m_field8d002 = 0xffff;
    m_field8d004 = 0xffff;
    m_field8d006 = 0xffff;
    m_field8d008 = 0xffff;
    m_field8d00c = 0xffff;
    m_field8cfcc = (UserSpace::T)0;
    m_clientSpec.m_field5fc = 0;
    m_network.m_padTail[0] = 0;
    m_network.m_padTail[1] = 0xff;
    m_guildAgitDB.m_pad[2] = 0;
    m_field8cf04 = 0xffffffff;
    m_clientSpec.m_field608 = 0;
    m_codeHackCheck.m_field48 = 0;
    m_codeHackCheck.m_field4c = 0;
    if (m_premium.m_mailBox != 0)
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    resetGuildCreateFlag();
    resetTempGuildNameAddr();
    reset_pvp_masterid_walkingout_me();
    m_mcap.resetExposedCount();
    m_field8d1dc = 0;
    m_field8d1e0 = 0;
    m_quest.m_field761c = 0;
    m_quest.m_field761d = 0;
    memset(&m_expReward, 0, 4);
    setForceDropFlag(false);
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_field8ec29 = 0;
}

void CUser::reset_server_fatigue_data()
{
    m_field8eb4d = 0;
    m_field8eb4c = 0;
    m_map8eb50.clear();
}

void CUser::reset_ingame_event_history()
{
    m_map8eb68.clear();
}

void CUser::reset_ingame_event_history_update()
{
    m_map8eb80.clear();
}

void CUser::reset_pvp_masterid_walkingout_me()
{
    m_set8cfe4.clear();
}

void CUser::set_guildwar_point_per_pvpplay(int point)
{
    m_guildWarPoint = point;
}

void CUser::resetMoneyLog()
{
    for (int i = 0; i <= 6; ++i)
        m_moneyLogs[i].reset();
    m_field796c8 = 0;
}

void CUser::ResetDBLogItem()
{
    m_dbLogItems.clear();
}

void CUser::DBUpdateDBLogItem() const
{
    for (std::vector<_DBLogItemInfo>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); ++it)
    {
        DB_LogBuyItem::makeRequest(get_acc_id(), getCurCharacNo(), it->m_itemIdx,
                                   it->m_type, it->m_value);
    }
}

void* CUser::GetParty()
{
    if (m_field8d004 < 0)
        return 0;
    return CGameManager::GetParty(G_CGameManager());
}

void* CUser::GetWarRoom()
{
    if (m_field8d008 < 0)
        return 0;
    return CGameManager::GetWarRoom(G_CGameManager());
}

int CUser::get_charac_no(int param)
{
    if (param != -1)
    {
        if (param < 0 || (int)m_characList.size() <= param)
            return 0;
        return *((unsigned int*)&m_characList[param]);
    }
    if (getCurCharacR() == 0)
        return 0;
    return *((unsigned int*)getCurCharacR());
}

bool CUser::CheckItemLock(int a, int b) const
{
    if (getCurCharacR() == 0)
        return 0;
    CInventory* pInven = (CInventory*)getCurCharacInvenR();
    if (pInven->GetInvenRef(a, b) == 0)
        return 1;
    if (((stAmplifyOption_t*)((char*)pInven->GetInvenRef(a, b) + 0x11))->GetLock() == 0)
        return 0;
    if (item_lock::CItemLock::CheckItemLock((CExpandEquipslot*)GetCharacExpandDataR(
            (ENUM_CHARAC_EXPAND_TYPE)2)) == 0)
        return 0;
    return 1;
}

unsigned short CUser::getCurCharacTotalFatigue() const
{
    unsigned short total = getCurCharacFatigue();
    short rate = m_premium.GetAdvantageFatigueRate();
    if (rate != 0)
    {
        unsigned short maxTotal = getCurCharacTotalMaxFatigue();
        unsigned short maxPremium = getCurCharacMaxPremiumFatigue();
        unsigned short premiumFatigue = getCurCharacPremiumFatigue();
        unsigned short added = (unsigned short)(std::min(premiumFatigue, maxPremium) + total);
        total = std::min(added, maxTotal);
    }
    return total;
}

unsigned short CUser::getCurCharacTotalMaxFatigue() const
{
    unsigned short total = getCurCharacMaxFatigue();
    short rate = m_premium.GetAdvantageFatigueRate();
    if (rate != 0)
        total = (unsigned short)(total + getCurCharacMaxPremiumFatigue());
    return total;
}

void CUser::make_basic_info(char* buf, char type) {}
void CUser::send_equip(int slot) {}

void CUser::AddDBLogItem(unsigned int itemIdx, unsigned int type,
                         ENUM_DBLOG_ITEM_TYPE value)
{
    for (std::vector<_DBLogItemInfo>::iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); ++it)
    {
        if (it->m_itemIdx == itemIdx)
        {
            it->m_type += type;
            return;
        }
    }
    _DBLogItemInfo info;
    memset(&info, 0, 0xc);
    info.m_itemIdx = itemIdx;
    info.m_type = type;
    info.m_value = value;
    m_dbLogItems.push_back(info);
}

int CUser::addAvatarItem(unsigned long itemIdx, char param2, char param3,
                         const char* ipgAgency, eAvatarItemAddReason reason)
{
    CDataManager* pMgr = G_CDataManager();
    CEquipItem* pItem = (CEquipItem*)pMgr->find_item(itemIdx);
    if (pItem == 0)
        return -1;
    std::vector<stAvatarTypeSelect_t>* pSelect =
        (std::vector<stAvatarTypeSelect_t>*)pItem->getAvatarTypeSelect();
    int usablePeriod = pItem->getUsablePeriod();
    if (param3 < (int)pSelect->size())
        usablePeriod = (int)(*pSelect)[param3].m_field0;
    CInventory* pInven = (CInventory*)getCurCharacInvenW();
    return pInven->AddAvatarItem(itemIdx, usablePeriod, 0, param2, param3,
                                 ipgAgency, reason, 0, 0);
}

int CUser::SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE space, int slot)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    CInventory_MakeItemPacket((CInventory*)getCurCharacInvenR(), (int)space, slot,
                              packet);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return SendPacket(target, packet);
}

int CUser::GetCurExpertJobLevel(int exp)
{
    return 0;  // TODO(G1-3)：按 CUser.md GetCurExpertJobLevel 精修
}

bool CUser::is_clear_stealingSkillMission() const
{
    return mission_list_is_clear(GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
}

void CUser::makeGuildSkillMessage(const char* skillName, int count)
{
    // TODO(G1-3)：按 CUser.md makeGuildSkillMessage 精修
}

void CUser::set_grow_type(unsigned char firstGrow, unsigned char secondGrow,
                          char* name, eChangeGrowTypeReason reason)
{
    // TODO(G1-3)：按 CUser.md set_grow_type 精修
}

// ---- 基础访问器（G1-3 桩替换） ----
unsigned int CUser::get_acc_id() const
{
    return m_field704ac;
}

const char* CUser::get_acc_name() const
{
    return m_accName;
}

int CUser::get_state()
{
    return m_field8cfc4;
}

void CUser::set_state(ch_state state)
{
    m_field8cfc4 = state;
    if (m_field8cfc4 == 5)
        ClearItemSellCount();
    if (getCurCharacR() != 0)
    {
        bool bFight = (bool)is_fighting();
        CInventory* pInven = (CInventory*)getCurCharacInvenR();
        user_creature::CCreatureMgr* pCreature = pInven->GetCreatureMgrW();
        pCreature->TurnStomach(bFight);
    }
}

unsigned int CUser::GetUID() const
{
    return ((int)(short)m_field704a4 << 16) | (int)(unsigned short)m_field704a6;
}

int CUser::get_unique_id() const
{
    return (unsigned short)m_field704a8;
}

Secu_GoldControl* CUser::GetGoldControl()
{
    return &m_goldControl;
}

CDungeonGainedGold* CUser::getDungeonGainedGold()
{
    return &m_dungeonGainedGold;
}

WongWork::CHackAnalyzer* CUser::getHackAnalyzer()
{
    return &m_hackAnalyzer;
}

unsigned char CUser::getGmQuestFlag()
{
    return m_gameOption.m_gmQuestFlag;
}

int CUser::GetAccountLastPlayTime()
{
    return m_premium.m_lastPlayTime;
}

ENUM_SERVER_GROUP CUser::GetServerGroup() const
{
    return *(ENUM_SERVER_GROUP*)((char*)this + 0x79630);
}

CExpandEquipslot* CUser::GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const
{
    return m_characExpand.GetData(type);
}

CExpandEquipslot* CUser::GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const
{
    return m_characExpand.GetDataR(type);
}

CExpandEquipslot* CUser::GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE& type) const
{
    return m_characExpand.GetDataR(type);
}

WongWork::CUserPremium* CUser::GetPremiumInfo() const
{
    return (WongWork::CUserPremium*)&m_premium;
}

UserQuest* CUser::getCurCharacQuestR() const
{
    return (UserQuest*)&m_quest;
}

void CUser::DecreaseGuildPowerWarPoint(unsigned int point)
{
    m_guildDB.m_powerWarPoint -= point;
}

unsigned char CUser::is_fighting()
{
    int state = m_field8cfc4;
    if ((unsigned)state > 10)
        return false;
    int bit = 1 << state;
    if ((bit & 0x720) != 0)
        return true;
    if ((bit & 0xdf) == 0)
        return false;
    return false;
}

void CUser::set_before_area(int area)
{
    m_field8cfb8 = area;
}

const char* CUser::getWebAddress()
{
    return m_gameOption.m_webAddress;
}

WongWork::CSecurityCard* CUser::getSecurityCard()
{
    return &m_securityCard;
}

CGameOption* CUser::GetGameOptionRef()
{
    return &m_gameOption;
}

Sanicova::CPad* CUser::getPad()
{
    return &m_cpad;
}

long CUser::getBeforeAreaTime() const
{
    return m_field79670;
}

void CUser::setBeforeAreaTime(long t)
{
    m_field79670 = t;
}

int CUser::getLastPacketID()
{
    return m_lastPacketID;
}

void CUser::setLastPacketID(int id)
{
    m_lastPacketID = id;
}

int CUser::getPlayingDungeonIndex()
{
    return m_field8cf04;
}

void CUser::setPlayingDungeonIndex(int index)
{
    m_field8cf04 = index;
}

UserSpace::T CUser::getMoveSpace() const
{
    return (UserSpace::T)m_field8cfcc;
}

void CUser::setMoveSpace(UserSpace::T space)
{
    m_field8cfcc = space;
}

unsigned char CUser::isUseCraneStart() const
{
    return m_field8eb98;
}

void CUser::setUseCraneStart(bool flag)
{
    m_field8eb98 = flag;
}

unsigned char CUser::getForceDropFlag()
{
    return m_gameOption.m_forceDropFlag;
}

void CUser::setForceDropFlag(bool flag)
{
    m_gameOption.m_forceDropFlag = flag;
}

unsigned char CUser::isDungeonClearHackCheck()
{
    if (m_mcap.m_field54c == 3)
        return 1;
    return 0;
}

void CUser::setDungeonClearHackCheck(bool flag)
{
    if (flag)
        m_mcap.m_field54c = (char)((unsigned char)m_mcap.m_field54c | 1);
    else
        m_mcap.m_field54c = (char)((unsigned char)m_mcap.m_field54c | 2);
}

unsigned char CUser::isAllowLocationGeoState()
{
    return m_rentalFlag3;
}

void CUser::setAllowLocationGeoState(bool flag)
{
    m_rentalFlag3 = flag;
}

unsigned char CUser::isComboSkillSet()
{
    return m_field8ec29;
}

void CUser::setComboSkillSet(bool flag)
{
    m_field8ec29 = flag;
}

unsigned char CUser::isLocked4DataLoad()
{
    return m_field8d0f0;
}

bool CUser::lock4DataLoad()
{
    char c = isLocked4DataLoad();
    if (c == 0)
        m_field8d0f0 = 1;
    return c == 0;
}

bool CUser::unlock4DataLoad()
{
    char c = isLocked4DataLoad();
    if (c == 1)
        m_field8d0f0 = 0;
    return c == 1;
}

unsigned char CUser::isLockedCreateCharac()
{
    return m_field8d0f2;
}

bool CUser::lockCreateCharac()
{
    char c = isLockedCreateCharac();
    if (c == 0)
        m_field8d0f2 = 1;
    return c == 0;
}

bool CUser::unlockCreateCharac()
{
    char c = isLockedCreateCharac();
    if (c == 1)
        m_field8d0f2 = 0;
    return c == 1;
}

unsigned char CUser::isLockedDeleteCharac()
{
    return m_field8d0f1;
}

bool CUser::lockDeleteCharac()
{
    char c = isLockedDeleteCharac();
    if (c == 0)
        m_field8d0f1 = 1;
    return c == 0;
}

bool CUser::unlockDeleteCharac()
{
    char c = isLockedDeleteCharac();
    if (c == 1)
        m_field8d0f1 = 0;
    return c == 1;
}

int CUser::get_growth_power_exp_reward()
{
    return m_expReward.m_reward;
}

void CUser::setAntibotKey(int key)
{
    m_antibotKey = key;
}

void CUser::setClientVersion(unsigned int version)
{
    m_clientVersion = version;
}

unsigned int CUser::getClientVersion()
{
    return m_clientVersion;
}

void CUser::ResetUsedCoinInDungeon()
{
    m_field8eae0 = 0;
}

void CUser::ClearItemSellCount()
{
    m_itemSellCount = 0;
    memset(&m_sellCountData, 0, 0x1c);
}

// ---- 发包族（PacketGuard 局部 + InterfacePacketBuf） ----
int CUser::Send(PacketGuard& packet)
{
    PacketBuf* pBuf = ((InterfacePacketBuf*)&packet)->get();
    if (pBuf->is_finallized() == 0)
    {
        unsigned short packetId = *((unsigned short*)pBuf->get_packet(1));
        const char* name = getCurCharacName();
        LogManager::logFormat(1, "user.cpp", "bool CUser::Send(PacketGuard&)", 0x8a0,
                              "User %s - Packet.is_finallized packet_id(%d) return false",
                              name, (int)packetId);
        return 0;
    }
    unsigned short packetId = *((unsigned short*)pBuf->get_packet(1));
    unsigned char area = pBuf->at(0);
    GetPacketName(area, packetId);
    unsigned char b = pBuf->at(1);
    if (b != 6)
    {
        m_pad8cf0c[0] = b;
        m_pad8cf0c[1] = pBuf->at(0);
    }
    return AntibotSend(*pBuf);
}

int CUser::SendPacket(eSendTarget target, PacketGuard& packet)
{
    if (target == (eSendTarget)1)
        Send(packet);
    else if (target < (eSendTarget)2)
    {
        if (target == (eSendTarget)0)
            G_GameWorld()->send_all(packet, this);
    }
    else if (target == (eSendTarget)2)
    {
        CParty* pParty = (CParty*)GetParty();
        if (pParty == 0)
            Send(packet);
        else
            pParty->send_to_party(packet);
    }
    else if (target == (eSendTarget)3)
    {
        WarRoom* pRoom = (WarRoom*)GetWarRoom();
        if (pRoom == 0)
            Send(packet);
        else
            pRoom->SendToRoom(packet);
    }
    return 1;
}

int CUser::SendCmdPacket(eSendTarget target, ENUM_CMDPACKET cmd, int param)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    if ((int)cmd == 0x41)
    {
        ((InterfacePacketBuf*)&packet)->put_byte(1);
        ((InterfacePacketBuf*)&packet)->put_byte(param);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        return SendPacket(target, packet);
    }
    LogManager::logFormat(1, "user.cpp",
                          "bool CUser::SendCmdPacket(CUser::eSendTarget, ENUM_CMDPACKET, int)",
                          0x7741, "User %s - CmdID %d invalid", getCurCharacName(),
                          (int)cmd);
    return 0;
}

int CUser::SendCmdOkPacket(ENUM_CMDPACKET cmd)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

int CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error)
{
    PacketGuard packet;
    return SendCmdErrorPacket(cmd, error, packet);
}

int CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PAY_TYPE pay)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->put_int((int)pay);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

int CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PacketGuard& packet)
{
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

int CUser::SendCmdErrorPacket2Byte(ENUM_CMDPACKET cmd, unsigned short error)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

int CUser::SendCmdErrorPacketJpn(ENUM_CMDPACKET cmd, unsigned short error)
{
    PacketGuard packet;
    ARAD::DISPATCHER::make_cmd_packetheader_jpn(packet, (int)cmd, 0);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

void CUser::SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                   unsigned char error)
{
    PacketGuard packet;
    SendCmdPeerErrorPacket(cmd, type, error, packet);
}

void CUser::SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                   unsigned char error, PacketGuard& packet)
{
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error & 0xff);
    ((InterfacePacketBuf*)&packet)->put_byte((int)type);
    if ((int)type == 4)
        ((InterfacePacketBuf*)&packet)->put_int(0);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

void CUser::SendZeroExpPacket()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x107);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

void CUser::SendMaxEquipLevel()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x1b6);
    ((InterfacePacketBuf*)&packet)->put_short(GetCurCharacMaxEquipLevel());
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

void CUser::SendFatigue()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x24);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacTotalFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacTotalMaxFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacUsedFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacFatigueGrownUpBuff() & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// ============================================================================
// G1-4 批次：索引 / 状态访问器（ORIG 逐符号转录）
// ============================================================================

bool CUser::CheckInTrade()
{
    return (~(unsigned short)m_field8d002 & 0x8000) != 0;
}

bool CUser::CheckInParty()
{
    return (~(unsigned short)m_field8d004 & 0x8000) != 0;
}

bool CUser::checkInBlueMarble()
{
    return (~(unsigned short)m_field8d00c & 0x8000) != 0;
}

bool CUser::CheckInWarRoom()
{
    return (~(unsigned short)m_field8d008 & 0x8000) != 0;
}

bool CUser::checkInDeathTower()
{
    return (~(unsigned short)m_field8d00e & 0x8000) != 0;
}

bool CUser::checkInBossTower()
{
    return (~(unsigned short)m_field8d010 & 0x8000) != 0;
}

bool CUser::checkInAdvanceAltar() const
{
    return (~(unsigned short)m_field8d012 & 0x8000) != 0;
}

short CUser::getAdvanceAltarIndex() const
{
    return (short)(unsigned short)m_field8d012;
}

unsigned short CUser::getBlueMarbleIndex()
{
    return (unsigned short)m_field8d00c;
}

void CUser::setBlueMarbleIndex(short index)
{
    m_field8d00c = index;
}

short CUser::getDeathTowerIndex()
{
    return (short)(unsigned short)m_field8d00e;
}

short CUser::getBossTowerIndex()
{
    return (short)(unsigned short)m_field8d010;
}

void CUser::setBossTowerIndex(short index)
{
    m_field8d010 = index;
}

short CUser::GetPartyIndex()
{
    return (short)(unsigned short)m_field8d004;
}

bool CUser::CheckInPvp()
{
    return (~(unsigned short)m_field8d006 & 0x8000) != 0;
}

short CUser::GetPvpIndex()
{
    return (short)(unsigned short)m_field8d006;
}

short CUser::GetWarRoomIndex()
{
    return (short)(unsigned short)m_field8d008;
}

int CUser::GetTradeSpace()
{
    return (short)(unsigned short)m_field8d002;
}

void CUser::SetPartyIndex(short index)
{
    lock();
    m_field8d004 = index;
    unlock();
}

void CUser::SetWarRoomIndex(short index)
{
    m_field8d008 = index;
}

void CUser::SetSlotIDX(short idx)
{
    m_field704a4 = idx;
}

short CUser::GetIncreID() const
{
    return (short)(unsigned short)m_field704a6;
}

void CUser::SetIncreID(short id)
{
    m_field704a6 = id;
}

void CUser::set_unique_id(unsigned short id)
{
    m_field704a8 = id;
}

void CUser::set_position(unsigned short x, unsigned short y, char z)
{
    m_field8cffc = x;
    m_field8cffe = y;
    m_field8d000 = z;
}

int CUser::GetUserState()
{
    int state = m_field8cfc4;
    if (state > 3)
    {
        if (state == 5 || state == 10)
            return 1;
        if (state == 8)
            return 3;
        if (state == 6 || state == 7)
            return 2;
        if (state == 12)
            return 1;
        if (state == 11)
            return 7;
        if (state == 12)
            return 1;
        return 0;
    }
    return CUserCharacInfo::GetUserStateByAssault();
}

int CUser::GetAge()
{
    return m_clientSpec.m_age;
}

unsigned char CUser::IsProgLogout()
{
    return m_clientSpec.m_progLogout;
}

int CUser::GetSeedFromDate()
{
    return m_clientSpec.m_seedFromDate;
}

void CUser::SetGMUpgradeMode(ENUM_GM_ITEM_UPGRADE mode)
{
    m_clientSpec.m_gmUpgradeMode = mode;
}

int CUser::SetETC(short key, int value)
{
    m_map8eaa4[key] = value;
    return value;
}

// ============================================================================
// G1-4 批次：CodeHackCheckStorage / 返回用户 / 预登录数据
// ============================================================================

void CUser::setTournamentAccount(bool flag)
{
    m_codeHackCheck.m_field31 = flag;
}

unsigned char CUser::isTournamentAccount()
{
    return m_codeHackCheck.m_field31;
}

void CUser::setJoinedSchoolPointEvent(bool flag)
{
    m_codeHackCheck.m_field44 = flag;
}

void CUser::setOnlinePreliminaryServerGroup(ENUM_SERVER_GROUP group)
{
    m_codeHackCheck.m_onlineServerGroup = group;
}

void CUser::setOnlinePreliminaryCharacNo(unsigned int characNo)
{
    m_codeHackCheck.m_onlineCharacNo = characNo;
}

void CUser::setOnlinePreliminaryPlayType(int playType)
{
    m_codeHackCheck.m_onlinePlayType = playType;
}

int CUser::getOnlinePreliminaryPlayType()
{
    return m_codeHackCheck.m_onlinePlayType;
}

unsigned int CUser::getOnlinePreliminaryCharacNo()
{
    return m_codeHackCheck.m_onlineCharacNo;
}

void CUser::SetPCRoomNo(unsigned int no)
{
    m_codeHackCheck.m_field54 = no;
}

void CUser::SetFirstLogin()
{
    m_codeHackCheck.m_firstLogin = 1;
}

void CUser::SetReturnUser()
{
    m_codeHackCheck.m_field59 = 1;
}

void CUser::ResetReturnUser()
{
    m_codeHackCheck.m_field59 = 0;
}

unsigned char CUser::IsReturnUser()
{
    return m_codeHackCheck.m_field59;
}

void CUser::SetReturnUserFirstUserLogin(bool flag)
{
    m_codeHackCheck.m_field5a = flag;
}

unsigned char CUser::IsReturnUserFirstLogin()
{
    return m_codeHackCheck.m_field5a;
}

int CUser::GetReturnUserExpireTime()
{
    return m_codeHackCheck.m_field5c;
}

void CUser::SetReturnUserExpireTime(int time)
{
    m_codeHackCheck.m_field5c = time;
}

void CUser::EnableSaveMemberBonusFatigue()
{
    m_codeHackCheck.m_field60 = 1;
}

void CUser::incAccountMemberBonusFatigue(short value)
{
    m_codeHackCheck.m_field62 = (short)(m_codeHackCheck.m_field62 + value);
}

void CUser::resetAccountMemberBonusFatigue()
{
    m_codeHackCheck.m_field62 = 0;
}

short CUser::getAccountMemberBonusFatigue()
{
    return (short)(unsigned short)m_codeHackCheck.m_field62;
}

void CUser::resetAccountUsedFatigue()
{
    m_codeHackCheck.m_field46 = 0;
}

void CUser::incChattingMessageCount(int count)
{
    m_codeHackCheck.m_field64 += count;
}

int CUser::getChattingMessageCount()
{
    return m_codeHackCheck.m_field64;
}

void CUser::SetGameMasterCharacter(CGameMasterCharacter* pCharac)
{
    m_codeHackCheck.m_gameMasterCharac = pCharac;
}

int CUser::getStdDropRate()
{
    return m_codeHackCheck.m_field28;
}

Secu_AccountHacking* CUser::getAcccountHackInfo()
{
    return &m_accHacking;
}

void CUser::setHackUserTimer()
{
    m_379.m_field0 = 1;
}

void CUser::setHackUserFlag()
{
    m_379.m_field1 = 1;
}

unsigned char CUser::isHackUser() const
{
    return m_379.m_field1;
}

unsigned char CUser::isHackUserTimer() const
{
    return m_379.m_field0;
}

// ============================================================================
// G1-4 批次：突破奖励 / 账号货物 / 商城 / 日常
// ============================================================================

STBreakAwayRewards::STBreakAwayRewards()
{
    clear();
}

void STBreakAwayRewards::clear()
{
    m_rewardItemId = 0;
    m_rewardAItemCnt = 0;
    m_luckPoint = 0;
    m_fieldc = 0;
    m_itemOrder = 0;
}

void STBreakAwayRewards::operator=(const STBreakAwayRewards& other)
{
    m_rewardItemId = other.m_rewardItemId;
    m_rewardAItemCnt = other.m_rewardAItemCnt;
    m_luckPoint = other.m_luckPoint;
    m_fieldc = other.m_fieldc;
    m_itemOrder = other.m_itemOrder;
}

int CUser::getBreakAwayRewardItemId()
{
    return m_breakAway.m_rewardItemId;
}

void CUser::setBreakAwayRewardItemId(int id)
{
    m_breakAway.m_rewardItemId = id;
}

int CUser::getBreakAwayRewardAItemCnt()
{
    return m_breakAway.m_rewardAItemCnt;
}

void CUser::setBreakAwayRewardAItemCnt(int cnt)
{
    m_breakAway.m_rewardAItemCnt = cnt;
}

int CUser::getUserBreakAwayRewardLuckPoint()
{
    return m_breakAway.m_luckPoint;
}

void CUser::setUserBreakAwayRewardLuckPoint(int point)
{
    m_breakAway.m_luckPoint = point;
}

int CUser::getBreakAwayRewardItemOrder()
{
    return m_breakAway.m_itemOrder;
}

STBreakAwayRewards* CUser::getBreakAwayRewards()
{
    return &m_breakAway;
}

void CUser::setBreakAwayRewards(const STBreakAwayRewards& rewards)
{
    m_breakAway = rewards;
}

int CUser::getBreakAwayQuestTime()
{
    return m_gameOption.m_breakAwayQuestTime;
}

void CUser::setBreakAwayQuestTime(unsigned int time)
{
    m_gameOption.m_breakAwayQuestTime = (int)time;
}

int CUser::getBreakAwayDungeonClearCnt()
{
    return m_gameOption.m_breakAwayDungeonClearCnt;
}

void CUser::setBreakAwayDungeonClearCnt(int cnt)
{
    m_gameOption.m_breakAwayDungeonClearCnt = cnt;
}

void CUser::subBreakAwayDungeonClearCnt()
{
    m_gameOption.m_breakAwayDungeonClearCnt -= 1;
    setBreakAwayDungeonClear(true);
}

void CUser::setBreakAwayDungeonClear(bool flag)
{
    m_gameOption.m_pad644[0] = (char)flag;
}

unsigned char CUser::isBreakAwayRewardData()
{
    return m_gameOption.m_pad654b[8];
}

void CUser::setGmQuestFlag(bool flag)
{
    m_gameOption.m_gmQuestFlag = (char)flag;
}

void CUser::setWebAddress(char* addr)
{
    strncpy(m_gameOption.m_webAddress, addr, 0x10);
}

int CUser::GetCera()
{
    return (int)m_cera;
}

void CUser::SetCera(unsigned int cera)
{
    m_cera = cera;
}

unsigned int CUser::UseCera(unsigned int cera)
{
    if (m_cera >= cera)
        m_cera -= cera;
    else
        m_cera = 0;
    return m_cera;
}

void CUser::setRegDate(unsigned int date)
{
    m_regDate = date;
}

void CUser::setRegTime(long time)
{
    m_regTime = (int)time;
}

int CUser::getRegTime() const
{
    return m_regTime;
}

void CUser::resetUsedCera()
{
    m_guildAgitDB.m_usedCera = 0;
}

void CUser::addUsedCera(unsigned int cera)
{
    m_guildAgitDB.m_usedCera += cera;
}

unsigned int CUser::getUsedCera()
{
    return m_guildAgitDB.m_usedCera;
}

void CUser::SetVillageAttackedReward(bool flag)
{
    m_quest.m_field761c = flag;
}

void CUser::setPcRoomAuth(bool flag)
{
    m_quest.m_field761d = flag;
}

void CUser::fillUserInfo4Antibot()
{
    m_field8cf58 = -1;
    strncpy(m_pad8cf18, getCurCharacName(), 0x14);
}

char* CUser::getUserInfo4Antibot()
{
    return m_pad8cf18;
}

void CUser::SetAccountLastPlayTime(long time)
{
    m_premium.m_lastPlayTime = (int)time;
}

void CUser::SetLoginTime(long time)
{
    m_premium.m_loginTime = (int)time;
}

long CUser::GetLoginTime()
{
    return m_premium.m_loginTime;
}

void CUser::SetLogInOutState(unsigned char state)
{
    m_premium.m_field98c = state;
}

unsigned char CUser::GetLogInOutState()
{
    return m_premium.m_field98c;
}

int CUser::get_public_ip_address()
{
    return m_expReward.m_publicIp;
}

void CUser::set_public_ip_address(int ip)
{
    m_expReward.m_publicIp = ip;
}

void CUser::set_local_ip_address(int ip)
{
    m_expReward.m_localIp = ip;
}

void CUser::setHangameUser(bool flag)
{
    m_expReward.m_hangameUser = flag;
}

unsigned char CUser::isHangameUser()
{
    return m_expReward.m_hangameUser;
}

unsigned char CUser::IsPowerUp()
{
    return m_expReward.m_field6;
}

void CUser::SetRating(float rating)
{
    m_character.m_f44 = rating;
}

void CUser::SetSchoolNo(unsigned int no)
{
    m_character.m_schoolNo = no;
}

unsigned int CUser::GetSchoolNo()
{
    return m_character.m_schoolNo;
}

void CUser::SetSchoolName(const char* name)
{
    m_str8e080 = name;
}

unsigned char CUser::GetTutorialSkipable()
{
    return m_tutorialSkipable;
}

void CUser::SetTutorialSkipable(unsigned char flag)
{
    m_tutorialSkipable = flag;
}

int CUser::getHumanCertifyTimerKey()
{
    return m_humanCertifyTimerKey;
}

void CUser::setHumanCertifyErrorCnt(int cnt)
{
    m_humanCertifyErrorCnt = cnt;
}

unsigned char CUser::isSendMailCertified()
{
    return m_field8e3ed;
}

void CUser::setRestingUserRestrict(bool flag)
{
    m_mcap.m_field544 = flag;
}

void CUser::setPunishTradeAlert(short alert)
{
    m_mcap.m_field546 = alert;
}

short CUser::getPunishTradeAlert()
{
    return (short)(unsigned short)m_mcap.m_field546;
}

void CUser::resetDungeonClearHackCheck()
{
    m_mcap.m_field54c = 0;
}

unsigned char CUser::IsRecvEvent()
{
    return m_mcap.m_field18;
}

int CUser::GetAutoClientRate()
{
    return m_mcap.m_field1c;
}

unsigned char CUser::isARSUserKick()
{
    return m_mcap.m_field561;
}

void CUser::setSecuReward(bool flag)
{
    m_hades.m_secuReward = flag;
}

unsigned char CUser::NeedSecuReward()
{
    return m_hades.m_secuReward;
}

void CUser::setSecuType(unsigned char type)
{
    m_hades.m_secuType = type;
}

unsigned char CUser::getSecuType()
{
    return m_hades.m_secuType;
}

unsigned char CUser::getRewardType()
{
    return m_hades.m_rewardType;
}

void CUser::setRewardType(unsigned char type)
{
    m_hades.m_rewardType = type;
}

void CUser::setSecuGrade(char grade)
{
    m_hades.m_secuGrade = grade;
}

void CUser::setFlagPresentAvengerTitle(bool flag)
{
    m_ceraUserInfo.m_field62 = flag;
}

void CUser::incCharacSlotLimit(unsigned char count)
{
    m_ceraUserInfo.m_field61 += count;
}

void CUser::set_ontime_last_recv_idx(int idx)
{
    m_ceraUserInfo.m_ontimeLastRecvIdx = idx;
}

int CUser::get_ontime_last_recv_idx() const
{
    return m_ceraUserInfo.m_ontimeLastRecvIdx;
}

void CUser::set_update_ontime_last_recv_idx()
{
    m_ceraUserInfo.m_updateOntime = 1;
}

void CUser::setCheckPickUpRandomOptionItem(bool flag)
{
    m_field8eb44 = flag;
}

unsigned char CUser::getCheckPickUpRandomOptionItem()
{
    return m_field8eb44;
}

void CUser::set_cera_item_order_no(unsigned int no)
{
    m_levelUpGift.m_field18 = no;
}

unsigned int CUser::get_cera_item_order_no()
{
    return m_levelUpGift.m_field18;
}

void CUser::setPcRoomPlayStartTime(unsigned int time)
{
    m_pcRoomPlayStartTime = time;
}

unsigned char CUser::IsDeleteEventDungeonInvitation() const
{
    return m_field8eb20;
}

unsigned char CUser::IsUsedCoinInDungeon() const
{
    return m_field8eae0;
}

void CUser::ResetCoinCount()
{
    m_field8eb1c = 0;
}

int CUser::GetUsedCoinCount()
{
    return m_field8eb1c;
}

void CUser::set_trick_area(int area)
{
    m_field8cfc0 = area;
}

int CUser::get_before_area()
{
    return m_field8cfb8;
}

// ============================================================================
// G1-4 批次：锁 / 账号货物 / 其余访问器
// ============================================================================

CAccountCargo* CUser::GetAccountCargo()
{
    return &m_accountCargo;
}

bool CUser::IsExistAccountCargo()
{
    return m_accountCargo.GetCapacity() != 0;
}

void CUser::DeleteCargo()
{
    m_accountCargo.Clear();
}

void CUser::lock()
{
    m_mutex1.lock();
}

void CUser::unlock()
{
    m_mutex1.unlock();
}

int CUser::getBackupSeed()
{
    return m_backupSeed;
}

unsigned char CUser::getPowerSide()
{
    return m_guildDB.m_field95;
}

char* CUser::getClientHWSpec()
{
    return (char*)&m_clientSpec;
}

void CUser::SetGuildDBInfo(const STGuildDBInfoOnly& info)
{
    memcpy(&m_guildDB, &info, 0xbd);
}

void CUser::ChangeGuildName(char* name)
{
    memset(&m_guildDB, 0, 0x17);
    memcpy(&m_guildDB, name, 0x16);
}

void CUser::SetGuildAgitFlag(bool flag)
{
    if (flag)
        m_guildDB.m_agitExist = 1;
    else
        m_guildDB.m_agitExist = 0;
}

void CUser::SetGuildAgitDBInfo(const STGuildAgitDBInfo& info)
{
    memcpy(&m_guildAgitDB, &info, 1);
}

void CUser::SetGuildCargoCapacity(unsigned int capacity)
{
    m_field8e094 = capacity;
}

void CUser::SetTodayGuildMember(bool flag)
{
    m_guildAgitDB.m_todayGuildMember = flag;
}

unsigned char CUser::IsTodayGuildMember()
{
    return m_guildAgitDB.m_todayGuildMember;
}

void CUser::SetLastLoginCharacNo(int characNo)
{
    m_field8d0f4 = characNo;
}

void CUser::SetLastLoginChannelNo(int channelNo)
{
    m_field8d0f8 = channelNo;
}

void CUser::SetValidLastLoginCharac(bool flag)
{
    m_field8d0fc = flag;
}

void CUser::SetReportMannerlessUserCount(int count)
{
    m_field8d110 = count;
}

int CUser::GetChecksumAnswerCount()
{
    return m_field8cfd8;
}

void CUser::IncChecksumAnswerCount()
{
    m_field8cfd8 += 1;
}

int CUser::GetCheckConnInterval()
{
    return m_field8cfd0;
}

void CUser::SetResumeChecksumTime(int time)
{
    m_field8cfd4 = time;
}

short CUser::incre_check_count()
{
    return m_field8cfc8 = (short)(m_field8cfc8 + 1);
}

short CUser::get_connect_count()
{
    return (short)(unsigned short)m_field8cfc8;
}

void CUser::setTotalHackUserTradeGold(unsigned long long gold)
{
    m_379.m_field4 = (int)gold;
    m_379.m_field8 = (int)(gold >> 32);
}

void CUser::setTotalHackUserTradeCnt(unsigned int cnt)
{
    m_379.m_fieldc = cnt;
}

void CUser::SetRevivalTime(long time)
{
    m_field8d1dc = (int)time;
}

long CUser::GetRevivalTime()
{
    return m_field8d1dc;
}

void CUser::SetUseAPCPotionTime(long time)
{
    m_field8d1e0 = (int)time;
}

long CUser::GetUseAPCPotionTime()
{
    return m_field8d1e0;
}

WongWork::CUserPremium* CUser::GetPremiumInfoW()
{
    return &m_premium;
}

void CUser::SetDailyBadge(short* badge)
{
    for (int i = 0; i <= 2; ++i)
        m_dailyBadge[i] = badge[i];
}

void CUser::SetUserDetailInfo(char* a, char* b, char* c)
{
    strncpy(&m_accName[0x16], a, 0x80);
    strncpy(&m_accName[0x96], b, 0x80);
    strncpy(&m_accName[0x116], c, 0x10);
}

// ============================================================================
// G1-4 批次：角色列表 / 视图
// ============================================================================

const char* CUser::get_charac_name(int idx) const
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0;
    return m_characList[idx].m_name;
}

int CUser::GetCharacJob(int idx) const
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0xb;
    return m_characList[idx].m_field26;
}

int CUser::getCharacLevelByCharacNo(int characNo) const
{
    int idx = 0;
    for (std::vector<_Charac_info>::const_iterator it = m_characList.begin();
         it != m_characList.end(); ++it, ++idx)
    {
        if (it->m_characNo == characNo)
            return (unsigned short)it->m_level;
    }
    return -1;
}

const char* CUser::get_charac_name_by_charac_no(int characNo) const
{
    for (unsigned int i = 0; i < m_characList.size(); ++i)
    {
        if (m_characList[i].m_characNo == characNo)
            return m_characList[i].m_name;
    }
    return 0;
}

void CUser::setCharacName(int characNo, const char* name)
{
    int idx = 0;
    for (std::vector<_Charac_info>::iterator it = m_characList.begin();
         it != m_characList.end(); ++it, ++idx)
    {
        if (it->m_characNo == characNo)
        {
            strncpy(it->m_name, name, 0x1d);
            m_characterView.enableSaveCharacView();
            return;
        }
    }
}

void CUser::SetCharacViewState(int state)
{
    m_characterView.enableSaveCharacView();
    CUserCharacInfo::setCharacViewState(state);
}

void CUser::SetCharacViewState(int characNo, int state)
{
    if (characNo < 0 || characNo >= (int)m_characList.size())
        return;
    m_characterView.enableSaveCharacView();
    m_characList[characNo].m_viewState = state;
}

int CUser::GetCharacViewState(int idx) const
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return -1;
    return m_characList[idx].m_viewState;
}

bool CUser::SetCharacViewStateByCharacNo(int characNo, int state)
{
    for (std::vector<_Charac_info>::iterator it = m_characList.begin();
         it != m_characList.end(); ++it)
    {
        if (it->m_characNo == characNo)
        {
            m_characterView.enableSaveCharacView();
            it->m_viewState = state;
            return true;
        }
    }
    return false;
}

// ============================================================================
// 批次 A 访问器（ORIG 反汇编逐偏移补全：0x8101048 / 0x82301ae / 0x827565a /
// 0x85a932a / 0x868a99a / 0x868fa46 / 0x86912b0 / 0x8691cd0 / 0x8692af6 等）
// ============================================================================

void CUser::sendEventInfo()
{
}

void CUser::questAutoClear(int questIdx)
{
}

void CUser::_onCompoundItem(int itemIdx)
{
}

void CUser::_updateNPCRelationship()
{
}

void CUser::processNPCGiftOnLevelUp()
{
}

char* CUser::GetUserName()
{
    return m_accName + 0x16;
}

char* CUser::GetUserEMail()
{
    return m_accName + 0x116;
}

void* CUser::GetCSHashSet()
{
    return &m_set8eb00;
}

char* CUser::GetGuildName()
{
    return (char*)&m_guildDB;
}

unsigned char CUser::GetGuildLevel()
{
    return *(unsigned char*)((char*)&m_guildDB + 0x1b);
}

void* CUser::GetHackCheck()
{
    return &m_hackLogCheck;
}

void* CUser::getBingoData()
{
    return &m_bingo;
}

void* CUser::GetGuildDBInfo()
{
    return &m_guildDB;
}

char* CUser::getGarenaAuthData()
{
    return (char*)this + 0x6ef92;
}

void* CUser::GetPISenderManager()
{
    return &m_piSender;
}

void* CUser::GetPIReceiverManager()
{
    return &m_piReceiver;
}

void* CUser::getGrowthWeaponEventdata()
{
    return &m_growthWeapon;
}

void* CUser::get_ingame_event_history()
{
    return &m_map8eb68;
}

void* CUser::get_ingame_event_history_update()
{
    return &m_map8eb80;
}

void* CUser::getGrowthCreatureEventdata()
{
    return &m_growthCreature;
}

void* CUser::get_server_fatigue_day_data()
{
    return &m_map8eb50;
}

void* CUser::getCharacLevelUpGift_AccountOnce()
{
    return &m_levelUpGift;
}

void* CUser::GetMailBox()
{
    return *(void**)((char*)this + 0x71b98);
}

int CUser::GetMileage()
{
    return *(int*)((char*)this + 0x796d4);
}

int CUser::GetPCRoomNo()
{
    return *(int*)((char*)this + 0x79650);
}

unsigned int CUser::GetCeraPoint()
{
    return *(unsigned int*)((char*)this + 0x8eadc);
}

int CUser::getAntibotKey()
{
    return m_antibotKey;
}

int CUser::GetManageLevel()
{
    return m_field8ebd0;
}

int CUser::GetManagePoint()
{
    return m_field8ebd4;
}

int CUser::GetUserMaxLevel()
{
    return m_field8ebd8;
}

void CUser::onDungeonClear(bool flag)
{
}

int CUser::GetDebugCommand()
{
    return *(int*)((char*)this + 0x8e090);
}

int CUser::GetTradePunishType()
{
    return *(int*)((char*)this + 0x79628);
}

int CUser::getLastLotteryTime()
{
    return *(int*)((char*)this + 0x8e938);
}

int CUser::GetNonClientRandInt()
{
    return *(int*)((char*)this + 0x8eaf8);
}

int CUser::GetNonClientFlag()
{
    return *(unsigned char*)((char*)this + 0x8eaf4);
}

int CUser::GetLastLoginCharacNo()
{
    return m_field8d0f4;
}

int CUser::GetLastLoginChannelNo()
{
    return m_field8d0f8;
}

bool CUser::IsValidLastLoginCharac()
{
    return m_field8d0fc;
}

int CUser::getRecipeProbability()
{
    return m_field8ec2c;
}

int CUser::get_local_ip_address()
{
    return *(int*)((char*)this + 0x8d254);
}

unsigned int CUser::GetGuildCargoCapacity()
{
    return m_field8e094;
}

int CUser::GetGuildPowerWarPoint()
{
    return m_guildDB.m_powerWarPoint;
}

long CUser::GetResumeChecksumTime()
{
    return m_field8cfd4;
}

CGameMasterCharacter* CUser::GetGameMasterCharacter()
{
    return *(CGameMasterCharacter**)((char*)this + 0x7964c);
}

int CUser::getBreakAwayAccureCera()
{
    return *(int*)((char*)this + 0x703b0);
}

int CUser::getBreakAwayRewardOrder()
{
    return *(int*)((char*)this + 0x703b8);
}

int CUser::get_multiboxLotteryItemFailCnt()
{
    return *(int*)((char*)this + 0x703bc);
}

int CUser::GetCurCharacUsedFatigueQuantity()
{
    return m_field8eba8;
}

bool CUser::isHumanCertified()
{
    return *(unsigned char*)((char*)this + 0x8e100) != 0;
}

bool CUser::isLoadingHackGold()
{
    return *(unsigned char*)((char*)this + 0x8e944) != 0;
}

bool CUser::isSaveInformNoticeFlag()
{
    return m_informNoticeFlag != 0;
}

int CUser::CheckMaxLuckyLevel()
{
    return 1;
}

// ---- 批次 A 第 2 组（ORIG 0x82300e8 / 0x82301d8 / 0x8230246 / 0x8692a08 /
// 0x8692b74 / 0x86971ce / 0x86975b6 / 0x8568e48 等） ----

void CUser::OnRecvEvent()
{
    *(char*)((char*)this + 0x8e408) = 1;
}

bool CUser::GetBuyingGold()
{
    return *(unsigned char*)((char*)this + 0x8ead8) != 0;
}

void CUser::SetBuyingGold(bool flag)
{
    *(unsigned char*)((char*)this + 0x8ead8) = flag ? 1 : 0;
}

bool CUser::GetFirstLogin()
{
    return *(unsigned char*)((char*)this + 0x79654) != 0;
}

void CUser::SetProgLogout()
{
    *(char*)((char*)this + 0x711e4) = 1;
}

bool CUser::isRestingUser()
{
    return *(unsigned char*)((char*)this + 0x8e934) != 0;
}

void CUser::InitReceivedRequestType()
{
    *(char*)((char*)this + 0x8ec28) = (char)0xff;
}

bool CUser::isBreakAwayDungeonClear()
{
    return *(unsigned char*)((char*)this + 0x703a8) != 0;
}

bool CUser::isUsedBreakAwayLuckPoint()
{
    return *(unsigned char*)((char*)this + 0x703c1) != 0;
}

bool CUser::isJoinedSchoolPointEvent()
{
    return *(unsigned char*)((char*)this + 0x79640) != 0;
}

void CUser::DisableSaveMemberBonusFatigue()
{
    *(char*)((char*)this + 0x7965c) = 0;
}

bool CUser::IsEnableSaveMemberBonusFatigue()
{
    return *(unsigned char*)((char*)this + 0x7965c) != 0;
}

void CUser::SetProperLevelDungeonUser()
{
    *(char*)((char*)this + 0x8eb99) = 1;
}

void CUser::ClearProperLevelDungeonUser()
{
    *(char*)((char*)this + 0x8eb99) = 0;
}

bool CUser::IsProperLevelDungeonUser()
{
    return *(unsigned char*)((char*)this + 0x8eb99) != 0;
}

bool CUser::isCharacLinkMessageFlag()
{
    return *(unsigned char*)((char*)this + 0x703d8) != 0;
}

bool CUser::isLinkCharacDisconnectFlag()
{
    return *(unsigned char*)((char*)this + 0x703d9) != 0;
}

unsigned char CUser::getDisconnectLinkCharacSlotIdx()
{
    return *(unsigned char*)((char*)this + 0x703da);
}

bool CUser::is_update_ontime_last_recv_idx()
{
    return *(unsigned char*)((char*)this + 0x8d24c) != 0;
}

void CUser::reset_update_ontime_last_recv_idx()
{
    *(char*)((char*)this + 0x8d24c) = 0;
}

bool CUser::CheckLoadRentalInfoFromExchange()
{
    return *(unsigned char*)((char*)this + 0x6ef91) != 0;
}

unsigned char CUser::GetPuUser()
{
    return *(unsigned char*)((char*)this + 0x796d8);
}

void CUser::SetCeraPoint(unsigned int point)
{
    *(unsigned int*)((char*)this + 0x8eadc) = point;
}

void CUser::SetDebugCommand(int cmd)
{
    *(int*)((char*)this + 0x8e090) = cmd;
}

void CUser::decre_check_count()
{
    *(short*)((char*)this + 0x8cfc8) = 0;
}

void CUser::setLastLotteryTime(unsigned long t)
{
    *(unsigned long*)((char*)this + 0x8e938) = t;
}

void CUser::SetNonClientRandInt(int v)
{
    *(int*)((char*)this + 0x8eaf8) = v;
}

void CUser::recipeForceProbability(int v)
{
    *(int*)((char*)this + 0x8ec2c) = v;
}

void CUser::setBreakAwayAccureCera(int v)
{
    *(int*)((char*)this + 0x703b0) = v;
}

void CUser::setCharacAntibotSerialNum(int v)
{
    *(int*)((char*)this + 0x8ec24) = v;
}

void CUser::set_multiboxLotteryItemFailCnt(int v)
{
    *(int*)((char*)this + 0x703bc) = v;
}

void CUser::resetPlayExpAdd()
{
    *(int*)((char*)this + 0x703a4) = 0;
}

void CUser::resetCleanpadFailCnt()
{
    *(int*)((char*)this + 0x8e0fc) = 0;
}

bool CUser::isCleanPadVerifyLimit()
{
    return *(int*)((char*)this + 0x8e0fc) > 2;
}

void CUser::resetTotalPcRoomPlayTime()
{
    *(int*)((char*)this + 0x8eba0) = 0;
}

void CUser::ResetCurCharacUsedFatigueQuantity()
{
    *(int*)((char*)this + 0x8eba8) = 0;
}

int CUser::get_server_fatigue_day_size()
{
    return (int)m_map8eb50.size();
}

void CUser::IncreaseUsedCoinCount()
{
    *(int*)((char*)this + 0x8eb1c) = *(int*)((char*)this + 0x8eb1c) + 1;
}

void CUser::setLoadHackGold(bool flag)
{
    *(unsigned char*)((char*)this + 0x8e944) = flag ? 1 : 0;
}

void CUser::SetNonClientFlag(bool flag)
{
    *(unsigned char*)((char*)this + 0x8eaf4) = flag ? 1 : 0;
}

void CUser::setHumanCertified(bool flag)
{
    *(unsigned char*)((char*)this + 0x8e100) = flag ? 1 : 0;
}

void CUser::incTradeCount()
{
    *(int*)((char*)this + 0x8d10c) = *(int*)((char*)this + 0x8d10c) + 1;
}

float CUser::GetRating()
{
    return *(float*)((char*)this + 0x8e078);
}

void CUser::backupSeed()
{
    m_backupSeed = *(int*)((char*)this + 0x796e0);
}

void CUser::ResetCharacExpandData()
{
    m_characExpand.reset();
}

bool CUser::IsExistGuildAgit()
{
    return *(unsigned char*)((char*)this + 0x8d0be) != 0;
}

short CUser::GetDailyBadge(ENUM_BADGE_TYPE type)
{
    return m_dailyBadge[type];
}

int CUser::get_charac_count()
{
    return (int)m_characList.size();
}

bool CUser::isTradePunishOverThirtyDays()
{
    return *(unsigned char*)((char*)this + 0x7962c) != 0;
}
