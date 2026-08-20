#include "GameRand.h"
// ============================================================================
// df_game_r 还原 —— CUser（G1-3 批次，在线用户会话聚合根）
// 布局见 CUser.h；本文件按 ORIG 反汇编（docs/class_func_reports/CUser.md）
// 逐函数转录，identical 口径验证：source/toolchain/check_tu_game_orig.sh。
// ============================================================================

#include <string.h>
#include <cstdarg>
#include <cstdio>

#include "CUser.h"
#include "CMailBox.h"
#include "CGameManager.h"
#include "CInventory.h"
#include "SkillSlot.h"
#include "WarRoom.h"
#include "PacketBuf.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "CItemAmplifier.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "Packet_Register_GM_MID.h"
#include <algorithm>

// ---- 外部符号声明（对应 TU 翻译后移除） ----
extern unsigned char _NS_PI_2ND_GetDefaultRandomHashKey();

extern void GetPacketName(unsigned char area, unsigned short packetId);


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
void WongWork::CMCAPManager::resetExposedCount() {}
int CUser::AntibotSend(PacketBuf& packet) { return 1; }
void Secu_AccountHacking::resetInfo() {}
int item_lock::CItemLock::CheckItemLock(CExpandEquipslot* data) { return 0; }

void cUserHistoryLog::SetUser(CUser* user) {}
void cUserHistoryLog::InitSkill(int, int, int, int, eSkillInitReason) {}
_Quest_Authen_Data::_Quest_Authen_Data() { memset(m_pad, 0, sizeof(m_pad)); }
void _Quest_Authen_Data::reset() { memset(m_pad, 0, sizeof(m_pad)); }
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

extern "C" void _ZN13charac_expand8CDataMgr18ResetDailyMidnightEv(void*);
extern "C" void _ZN13charac_expand8CDataMgr10ResetDailyEv(void*);

void charac_expand::CDataMgr::ResetDailyMidnight()
{
    _ZN13charac_expand8CDataMgr18ResetDailyMidnightEv(this);
}

void charac_expand::CDataMgr::ResetDaily()
{
    _ZN13charac_expand8CDataMgr10ResetDailyEv(this);
}
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
    m_expReward.m_powerUp = 0;
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

int CUser::get_guildwar_point_per_pvpplay()
{
    return m_guildWarPoint;
}

void* CUser::getHades()
{
    return &m_hades;
}

void* CUser::GetPVPRoom()
{
    if (m_field8d006 < 0)
        return 0;
    return G_CGameManager()->GetPvp(m_field8d006, this, 0);
}

void* CUser::GetSecretShopData()
{
    CParty* party = (CParty*)GetParty();
    if (!party)
        return 0;
    return party->GetSecretShopData();
}

void* CUser::GetPICSMap()
{
    return &m_map8eabc;
}

int CUser::CheckMoney(int money)
{
    const char* accName = get_acc_name();
    int level = get_charac_level();
    int limit = G_CDataManager()->GetMoneyLimitPerLevel(level, accName);
    int curMoney = getCurCharacMoney();
    return curMoney + money <= limit;
}

extern "C" int _ZN13CBattle_Field17get_dungeon_indexEv(void*);

int CUser::getDungeonIdxAfterClear()
{
    CParty* party = (CParty*)GetParty();
    if (!party)
        return -1;
    return _ZN13CBattle_Field17get_dungeon_indexEv((char*)party + 0xb24);
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
    return G_CGameManager()->GetParty(m_field8d004);
}

void* CUser::GetWarRoom()
{
    if (m_field8d008 < 0)
        return 0;
    return G_CGameManager()->GetWarRoom(m_field8d008);
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
    return *(ENUM_SERVER_GROUP*)((char*)&m_codeHackCheck.m_serverGroup);
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

UserQuest* CUser::getCurCharacQuestW()
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

void CUser::set_area(int area)
{
    set_before_area(m_field8cfbc);
    m_field8cfbc = area;
}

int CUser::get_area(bool param)
{
    if (param) {
        if (G_GameWorld()->IsSchoolPvPChannel() &&
            getCurCharacVill() == 2 &&
            m_field8cfc0 == 4) {
            return m_field8cfc0;
        }
    }
    if (param && getCurCharacVill() == 8) {
        if (m_field8cfc0 == 0 || m_field8cfc0 == 1 ||
            m_field8cfc0 == 2 || m_field8cfc0 == 3) {
            return m_field8cfc0;
        }
    }
    return m_field8cfbc;
}

extern "C" int _ZNK19CMissionList_Charac14getWinningRateEv(void*);

int CUser::get_pvp_WinningRate_relateMission() const
{
    void* mission = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return _ZNK19CMissionList_Charac14getWinningRateEv(mission);
}

int CUser::get_aura_avatar_option_value(int idx)
{
    const _Charac_info* cur = getCurCharacR();
    if (!cur)
        return -1;
    if (idx < 0 || idx > 2)
        return -1;
    return *(const int*)((const char*)cur + (idx + 0x4a0) * 4 + 1);
}

void CUser::SetTradeSpace(int idx)
{
    lock();
    m_field8d002 = (short)idx;
    unlock();
}

bool CUser::CheckFatigue()
{
    if (m_field704ac == 0)
        return true;
    return getCurCharacTotalFatigue() < getCurCharacTotalMaxFatigue();
}

extern "C" int _ZNK19CMissionList_Charac15getIndex_byKindEi(void*, int);

bool CUser::has_within_Mission() const
{
    void* mission = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return _ZNK19CMissionList_Charac15getIndex_byKindEi(mission, 0x1b) != 0;
}

void* CUser::getBlueMarble()
{
    short idx = m_field8d00c;
    if (idx < 0)
        return 0;
    return G_CGameManager()->getBlueMarble(idx);
}

bool CUser::checkLogOutCorrectly()
{
    ((char*)&m_guildAgitDB)[2] = 1;
    return true;
}

unsigned int CUser::find_pvp_masterid_walkingout_me(unsigned int id)
{
    if (m_set8cfe4.empty())
        return 0;
    std::set<unsigned int>::iterator it = m_set8cfe4.find(id);
    if (it != m_set8cfe4.end())
        return *it;
    return 0;
}

void CUser::insert_pvp_masterid_walkingout_me(unsigned int id)
{
    m_set8cfe4.insert(id);
}

void CUser::update_old_pvp_point()
{
    PvpResultType* writable = getPVPResultRefW();
    const PvpResultType* readable = getPVPResultRefR();
    writable->m_oldExpPoint = readable->m_expPoint;
}

void CUser::SetSaveRentalInfoToExchange(bool flag)
{
    m_rentalFlag1 = flag;
}

void CUser::DeleteRentalItemInfo(int idx)
{
    m_rentalInfo[idx].clear();
}



bool WongWork::CUserPremium::CheckPremium(int type) const
{
    return *(const int*)&m_pad[type * 20] != 0;
}

int WongWork::CUserPremium::GetAdvPremiumCount() const
{
    return *(const int*)&m_pad[0x848];
}

short WongWork::CUserPremium::GetAdvantageFatigueRate() const
{
    return *(const short*)&m_pad[0x850];
}

bool CUser::isAffectedPremium(ENUM_PREMIUM_TYPE type) const
{
    return m_premium.CheckPremium((int)type);
}

char CUser::IsPermissionPrivateStore()
{
    return isAffectedPremium((ENUM_PREMIUM_TYPE)8);
}

void CUser::RecoverFatigue(int value)
{
    if (!getCurCharacR())
        return;
    setCurCharacPremiumFatigue((unsigned short)value);
    setCurCharacFatigue(value);
}

void CUser::LogHistory(const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
}

bool CUser::isBlackUser(unsigned int accId) const
{
    if (m_vec8cef4.empty())
        return false;
    for (std::vector<unsigned int>::const_iterator it = m_vec8cef4.begin();
         it != m_vec8cef4.end(); ++it) {
        if (*it == accId)
            return true;
    }
    return false;
}

bool CUser::IsHavePremiumAdvantage() const
{
    return m_premium.GetAdvPremiumCount() != 0;
}

bool CUser::IsEquipAvatar()
{
    const _Charac_info* cur = getCurCharacR();
    if (!cur)
        return false;
    const CInventory* inven = (const CInventory*)getCurCharacInvenR();
    return inven->IsEquipAvatar();
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

int CUser::CheckMoveTown(int vill)
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    if (vill <= 10)
    {
        if ((1 << vill) & 0x580)
            return 0;
    }
    Village* village = G_GameWorld()->GetVillage(vill);
    if (!village)
        return 8;
    if (get_charac_level() < village->m_requiredLevel)
        return 8;
    if (village->m_questIdx != 0)
    {
        if (!getCurCharacQuestR()->isClearQuest(village->m_questIdx))
            return 7;
    }
    return 0;
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

void CUser::setDeathTowerIndex(short idx)
{
    m_field8d00e = (unsigned short)idx;
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
    m_posX = x;
    m_posY = y;
    m_direction = z;
}

unsigned int CUser::get_posX()
{
    return m_posX;
}

unsigned int CUser::get_posY()
{
    return m_posY;
}

char CUser::get_direction()
{
    return m_direction;
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

bool CUser::isGMUser()
{
    return m_clientSpec.m_gmUserFlag;
}

bool CUser::IsGameMasterMode() const
{
    return m_clientSpec.m_gmUserFlag;
}

void CUser::SetGameMasterMode(bool flag)
{
    m_clientSpec.m_gmUserFlag = flag;
    Packet_Register_GM_MID pkt;
    if (flag)
    {
        unsigned int characNo = getCurCharacNo();
        ENUM_SERVER_GROUP group = GetServerGroup();
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
        proxy->SendTcpPacket((char*)&pkt, 0xe);
    }
}
char CUser::GetEventCreateDnfReward()
{
    return m_eventCreateDnfReward;
}

void CUser::SetPowerUp(bool flag)
{
    m_expReward.m_powerUp = flag;
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

void CUser::setChattingMessageCount(int count)
{
    m_codeHackCheck.m_field64 = count;
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
    return m_expReward.m_powerUp;
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

const char* CUser::GetSsnString()
{
    return m_accName + 0x96;
}

char CUser::getSex()
{
    const char* ssn = GetSsnString();
    if (strlen(ssn) < 0xd)
        return -1;
    if (ssn[6] == '2' || ssn[6] == '4')
        return 0;
    return 1;
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
    return (char*)m_pad6ef92;
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
    return *(void**)((char*)&m_premium.m_mailBox);
}

int CUser::GetMileage()
{
    return *(int*)((char*)m_pad796d4);
}

int CUser::GetPCRoomNo()
{
    return *(int*)((char*)&m_codeHackCheck.m_field54);
}

unsigned int CUser::GetCeraPoint()
{
    return *(unsigned int*)((char*)&m_pad8ead9[3]);
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
    return *(int*)((char*)&m_pad8e08b[5]);
}

int CUser::GetTradePunishType()
{
    return *(int*)((char*)m_codeHackCheck.m_pad2c);
}

int CUser::getLastLotteryTime()
{
    return *(int*)((char*)m_mcap.m_pad548);
}

int CUser::GetNonClientRandInt()
{
    return *(int*)((char*)&m_piSender + 0xc);
}

int CUser::GetNonClientFlag()
{
    return *(unsigned char*)((char*)&m_piSender + 8);
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
    return *(int*)((char*)&m_expReward.m_localIp);
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
    return *(CGameMasterCharacter**)((char*)&m_codeHackCheck.m_gameMasterCharac);
}

int CUser::getBreakAwayAccureCera()
{
    return *(int*)((char*)m_gameOption.m_pad64c);
}

int CUser::getBreakAwayRewardOrder()
{
    return *(int*)((char*)m_gameOption.m_pad654b);
}

int CUser::get_multiboxLotteryItemFailCnt()
{
    return *(int*)((char*)&m_gameOption.m_pad654b[4]);
}

int CUser::GetCurCharacUsedFatigueQuantity()
{
    return m_field8eba8;
}

bool CUser::isHumanCertified()
{
    return *(unsigned char*)((char*)m_pad8e100) != 0;
}

bool CUser::isLoadingHackGold()
{
    return *(unsigned char*)((char*)&m_mcap.m_pad54d[7]) != 0;
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
    *(char*)((char*)&m_mcap.m_field18) = 1;
}

bool CUser::GetBuyingGold()
{
    return *(unsigned char*)((char*)&m_field8ead8) != 0;
}

void CUser::SetBuyingGold(bool flag)
{
    *(unsigned char*)((char*)&m_field8ead8) = flag ? 1 : 0;
}

bool CUser::GetFirstLogin()
{
    return *(unsigned char*)((char*)&m_codeHackCheck.m_firstLogin) != 0;
}

void CUser::SetProgLogout()
{
    *(char*)((char*)&m_clientSpec.m_progLogout) = 1;
}

bool CUser::isRestingUser()
{
    return *(unsigned char*)((char*)&m_mcap.m_field544) != 0;
}

void CUser::InitReceivedRequestType()
{
    *(char*)((char*)&m_field8ec28) = (char)0xff;
}

bool CUser::isBreakAwayDungeonClear()
{
    return *(unsigned char*)((char*)m_gameOption.m_pad644) != 0;
}

bool CUser::isUsedBreakAwayLuckPoint()
{
    return *(unsigned char*)((char*)&m_gameOption.m_pad654b[9]) != 0;
}

bool CUser::isJoinedSchoolPointEvent()
{
    return *(unsigned char*)((char*)&m_codeHackCheck.m_field44) != 0;
}

void CUser::DisableSaveMemberBonusFatigue()
{
    *(char*)((char*)&m_codeHackCheck.m_field60) = 0;
}

bool CUser::IsEnableSaveMemberBonusFatigue()
{
    return *(unsigned char*)((char*)&m_codeHackCheck.m_field60) != 0;
}

void CUser::SetProperLevelDungeonUser()
{
    *(char*)((char*)&m_field8eb99) = 1;
}

void CUser::ClearProperLevelDungeonUser()
{
    *(char*)((char*)&m_field8eb99) = 0;
}

bool CUser::IsProperLevelDungeonUser()
{
    return *(unsigned char*)((char*)&m_field8eb99) != 0;
}

bool CUser::isCharacLinkMessageFlag()
{
    return *(unsigned char*)((char*)m_breakAway.m_pad14) != 0;
}

bool CUser::isLinkCharacDisconnectFlag()
{
    return *(unsigned char*)((char*)&m_breakAway.m_pad14[1]) != 0;
}

unsigned char CUser::getDisconnectLinkCharacSlotIdx()
{
    return *(unsigned char*)((char*)&m_breakAway.m_pad14[2]);
}

bool CUser::is_update_ontime_last_recv_idx()
{
    return *(unsigned char*)((char*)&m_ceraUserInfo + 0x68) != 0;
}

void CUser::reset_update_ontime_last_recv_idx()
{
    *(char*)((char*)&m_ceraUserInfo + 0x68) = 0;
}

bool CUser::CheckLoadRentalInfoFromExchange()
{
    return *(unsigned char*)((char*)&m_rentalFlag2) != 0;
}

unsigned char CUser::GetPuUser()
{
    return *(unsigned char*)((char*)&m_field796d8);
}

void CUser::SetCeraPoint(unsigned int point)
{
    *(unsigned int*)((char*)&m_pad8ead9[3]) = point;
}

void CUser::SetDebugCommand(int cmd)
{
    *(int*)((char*)&m_pad8e08b[5]) = cmd;
}

void CUser::decre_check_count()
{
    *(short*)((char*)&m_field8cfc8) = 0;
}

void CUser::setLastLotteryTime(unsigned long t)
{
    *(unsigned long*)((char*)m_mcap.m_pad548) = t;
}

void CUser::SetNonClientRandInt(int v)
{
    *(int*)((char*)&m_piSender + 0xc) = v;
}

void CUser::recipeForceProbability(int v)
{
    *(int*)((char*)&m_field8ec2c) = v;
}

void CUser::setBreakAwayAccureCera(int v)
{
    *(int*)((char*)m_gameOption.m_pad64c) = v;
}

void CUser::setCharacAntibotSerialNum(int v)
{
    *(int*)((char*)&m_field8ec24) = v;
}

void CUser::set_multiboxLotteryItemFailCnt(int v)
{
    *(int*)((char*)&m_gameOption.m_pad654b[4]) = v;
}

void CUser::resetPlayExpAdd()
{
    *(int*)((char*)&m_gameOption.m_playExpAdd) = 0;
}

void CUser::resetCleanpadFailCnt()
{
    *(int*)((char*)&m_humanCertifyErrorCnt) = 0;
}

bool CUser::isCleanPadVerifyLimit()
{
    return *(int*)((char*)&m_humanCertifyErrorCnt) > 2;
}

void CUser::resetTotalPcRoomPlayTime()
{
    *(int*)((char*)m_pad8eba0) = 0;
}

void CUser::ResetCurCharacUsedFatigueQuantity()
{
    *(int*)((char*)&m_field8eba8) = 0;
}

int CUser::get_server_fatigue_day_size()
{
    return (int)m_map8eb50.size();
}

void CUser::IncreaseUsedCoinCount()
{
    *(int*)((char*)&m_field8eb1c) = *(int*)((char*)&m_field8eb1c) + 1;
}

void CUser::setLoadHackGold(bool flag)
{
    *(unsigned char*)((char*)&m_mcap.m_pad54d[7]) = flag ? 1 : 0;
}

void CUser::SetNonClientFlag(bool flag)
{
    *(unsigned char*)((char*)&m_piSender + 8) = flag ? 1 : 0;
}

void CUser::setHumanCertified(bool flag)
{
    *(unsigned char*)((char*)m_pad8e100) = flag ? 1 : 0;
}

void CUser::incTradeCount()
{
    *(int*)((char*)&m_pad8d0fd[0xf]) = *(int*)((char*)&m_pad8d0fd[0xf]) + 1;
}

float CUser::GetRating()
{
    return *(float*)((char*)&m_character + 0x44);
}

void CUser::backupSeed()
{
    m_backupSeed = *(int*)((char*)&m_pad796da[6]);
}

void CUser::ResetCharacExpandData()
{
    m_characExpand.reset();
}

bool CUser::IsExistGuildAgit()
{
    return *(unsigned char*)((char*)&m_guildDB + 0x9e) != 0;
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
    return *(unsigned char*)((char*)&m_codeHackCheck.m_field30) != 0;
}


void CUser::sendBlueMarbleEnterCount()
{
    PacketGuard packet;
    packet.put_header(0, 0x1b4);
    packet.put_byte(getBlueMarbleEnterCount());
    packet.finalize(true);
    Send(packet);
}

void CUser::ResetDailyCharacExpandDataMidnight()
{
    m_characExpand.ResetDailyMidnight();
}

void CUser::ResetDailyCharacExpandData()
{
    m_characExpand.ResetDaily();
}

void CUser::resetNPCRelationShipDailyData() {}

void CUser::sendNPCRelationShipFavor() {}

void CUser::SendProperDungeonClearCount()
{
    if (get_state() > 2)
        return;
    if (!getCurCharacR())
        return;
    PacketGuard packet;
    packet.put_header(0, 0x10b);
    packet.put_short(GetProperDungeonClearCount());
    packet.finalize(true);
    Send(packet);
}

extern "C" bool _ZN19CMissionList_Charac24MakeMissionList_JustKindERK5CUserj(void*, void*, unsigned int);

bool CUser::acceptable_within_mission() const
{
    void* mission = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    int grade = get_pvp_grade();
    int idx = G_CDataManager()->get_WithinMissionIndex(grade);
    if (idx == 0)
        return false;
    return _ZN19CMissionList_Charac24MakeMissionList_JustKindERK5CUserj(mission, (void*)this, 0x1b);
}

bool CUser::isDuplicationMessage(const std::string& msg)
{
    bool dup = (msg == m_str79664[0]) || (msg == m_str79664[1]);
    unsigned char idx = *(unsigned char*)&m_pad7966c[0];
    std::string* slot = (std::string*)((char*)this + 0x79670 + idx * 4);
    *slot = msg;
    *(unsigned char*)&m_pad7966c[0] = idx ^ 1;
    return dup;
}

void CUser::DimensionInoutUpdate(bool flag1, bool flag2)
{
    for (int i = 0; i <= 5; i++)
    {
        if (flag1)
        {
            char value = (char)G_CDataManager()->get_dimensionInout(i);
            setDemensionInoutValue((char)i, value);
        }
    }
    if (flag2)
    {
        char value = (char)G_CDataManager()->get_limit_inout_count(2);
        setUltimateInoutValue(2, value);
    }
}

extern "C" int _ZNK11RefPvpGrade19GetPvpCurrRankPointEi(void*, int);
extern "C" int _ZNK11RefPvpGrade19GetPvpNextRankPointEi(void*, int);

void CUser::send_pvp_record()
{
    PacketGuard packet;
    packet.put_header(0, 0x30);
    const PvpResultType* result = getPVPResultRefR();
    packet.put_int(*(int*)&result->m_pad0[0]);
    packet.put_int(*(int*)&result->m_pad0[4]);
    packet.put_int(result->m_expPoint);
    packet.put_int(_ZNK11RefPvpGrade19GetPvpCurrRankPointEi(GlobalData::s_ref_pvp_grade, result->m_pvpGrade));
    packet.put_int(_ZNK11RefPvpGrade19GetPvpNextRankPointEi(GlobalData::s_ref_pvp_grade, result->m_pvpGrade));
    packet.put_byte(result->m_pvpGrade);
    packet.put_byte(*(unsigned char*)&result->m_pad5c[0]);
    packet.finalize(true);
    Send(packet);
}

extern "C" void const* sub_CQuestClear_getClearedQuest(void const* self)
    asm("_ZNK8WongWork11CQuestClear15getClearedQuestEv");

void CUser::send_clear_quest_list()
{
    PacketGuard packet;
    packet.put_header(0, 0x163);
    packet.put_int(0x7530);
    UserQuest* quest = getCurCharacQuestR();
    void const* cleared = sub_CQuestClear_getClearedQuest((char*)quest + 4);
    packet.put_binary((const char*)cleared, 0x7530);
    packet.finalize(true);
    Send(packet);
}

void CUser::processReturnUserQuestAutoClear()
{
    if (IsReturnUser() && IsReturnUserFirstLogin())
    {
        questAutoClear(get_charac_level() - 1);
    }
}

bool CUser::update_pvp_rank(const PvpResultType& result)
{
    getPVPResultRefW()->AddNewResult(result);
    return true;
}

void CUser::saveTaxMoneyForUpperMember(int money)
{
    if (get_charac_memberkey() != 0 && is_connect_upper_member() && money > 0)
    {
        add_member_pay_tex_money_to_upper(money);
    }
}

int CUser::gainExpAsUpperMember(int exp)
{
    if (get_charac_memberkey() == 0)
        return 0;
    if (!is_connect_upper_member())
        return 0;
    unsigned char level = get_uppermember_exp_level();
    int result = (int)(exp * (level * 0.1f));
    if (result > 0)
        set_charac_member_bonus_exp(result);
    return result;
}

int CUser::gainPowerWarRewardExp(int)
{
    return 0;
}

void CUser::add_guild_point_item()
{
    int threshold = *(int*)((char*)G_CDataManager() + 0xa510);
    int rand = get_rand_int(0x2710);
    if (rand < threshold)
        add_inventory_item(0xc84);
}

void CUser::SendMoneyFullReason(ENUM_MONEY_FULL_REASON reason, unsigned long a, unsigned long b)
{
    PacketGuard packet;
    packet.put_header(0, 0x33);
    packet.put_byte(reason);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    Send(packet);
}

bool CUser::AddDungeonClear(int dungeonIdx, int clearCount)
{
    const WongWork::CDungeonClear* pClear = getDungeonClearRefR();
    char curDiff = pClear->getClearedDungeonDiff(dungeonIdx);
    if (curDiff < clearCount)
    {
        getDungeonClearRefW()->addClearedDungeon(dungeonIdx, clearCount);
        return true;
    }
    return false;
}

extern "C" int _ZN22CConditionEventManager16GetCurEventIndexEv(void*);

void CUser::SendConditionEventInfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xea);
    void* pMgr = (void*)G_CGameManager()->GetConditionEventManager();
    if (pMgr)
    {
        int eventIdx = _ZN22CConditionEventManager16GetCurEventIndexEv(pMgr);
        unsigned short step = GetCurConditionEventStep();
        unsigned short rewardStep = GetCurConditionEventRewardStep();
        packet.put_short(eventIdx);
        packet.put_byte(step);
        packet.put_byte(rewardStep);
        packet.finalize(true);
        Send(packet);
    }
}

void CUser::ResetCurCharacUsedGiftFatigueQuantity()
{
    int* pVal = (int*)((char*)this + 0x8ebac);
    if (*pVal != 0)
    {
        *pVal = 0;
        SetChangedGiftFatigueQuantity(true);
    }
}

void CUser::SetChangedGiftFatigueQuantity(bool flag)
{
    *(unsigned char*)((char*)this + 0x8ebb0) = flag ? 1 : 0;
}

extern "C" void _ZN9QuestList17GetDailyQuestListERSt4listIiSaIiEE(void*, void*);
extern "C" void _ZN9UserQuest15resetClearQuestEi(void*, int);

void CUser::ResetDailyQuest()
{
    std::list<int> questList;
    void* pQuestList = *(void**)((char*)G_CDataManager() + 0x18);
    _ZN9QuestList17GetDailyQuestListERSt4listIiSaIiEE(pQuestList, &questList);
    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        UserQuest* pQuest = getCurCharacQuestR();
        if (pQuest && pQuest->isClearQuest(questId))
        {
            _ZN9UserQuest15resetClearQuestEi(getCurCharacQuestW(), questId);
        }
    }
}

extern "C" void _ZN9QuestList20GetTrainingQuestListERSt6vectorIiSaIiEEs(void*, void*, short);

void CUser::ResetTrainingQuest()
{
    std::vector<int> questList;
    short level = get_charac_level();
    void* pQuestList = *(void**)((char*)G_CDataManager() + 0x18);
    _ZN9QuestList20GetTrainingQuestListERSt6vectorIiSaIiEEs(pQuestList, &questList, level);
    for (std::vector<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        UserQuest* pQuest = getCurCharacQuestR();
        if (pQuest && pQuest->isClearQuest(questId))
        {
            _ZN9UserQuest15resetClearQuestEi(getCurCharacQuestW(), questId);
        }
    }
}

void CUser::SetPvpIndex(short idx)
{
    lock();
    *(short*)((char*)this + 0x8d006) = idx;
    unlock();
}

void CUser::add_inventory_item(unsigned int itemId)
{
    CItem* pItem = G_CDataManager()->find_item(itemId);
    if (!pItem)
        return;
    Inven_Item invenItem;
    pItem->make_item(invenItem);
    invenItem.m_addInfo2 = 1;  // count
    invenItem.m_addInfo = itemId;  // item id
    int slot = getCurCharacInvenW()->insertItemIntoInventory(invenItem, (eItemAddReason)0x18, true, true);
    if (slot == -1)
        return;
    // send update item packet
}

extern "C" void _ZN19CMissionList_Charac16Send_MissionListER5CUser(void*, void*);

void CUser::send_MissionList()
{
    void* mission = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    _ZN19CMissionList_Charac16Send_MissionListER5CUser(mission, this);
}

void CUser::send_aura_avatar_option()
{
    if (getCurCharacR() == 0) return;
    PacketGuard packet;
    packet.put_header(0, 0x17a);
    packet.put_byte(3);
    for (int idx = 0; idx < 3; ++idx) {
        packet.put_byte((char)idx);
        int now = GlobalData::s_systemTime_.getCurSec();
        const _Charac_info* cur = getCurCharacR();
        int expire = *(const int*)((const char*)cur + (idx + 0x4a0) * 4 + 1);
        packet.put_int(now < expire ? expire - now : 0);
    }
    packet.finalize(true);
    Send(packet);
}
