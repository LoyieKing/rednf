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
#include "SigTypes.h"        // SIG_LOAD_ETC/SIG_LOAD_DUNGEON_CLEAR/SIG_REQ_BLACK_COUNT/SIG_ATTENDANCE_EVENT 完整布局
#include "CDungeonClear.h"   // WongWork::CDungeonClear::toString
#include "GameResultType.h"  // GameResultType（GetRecord 序列化 0x50/元素）
#include "CTitleBook.h"   // item_lock::CItemLock / stItemLockRef 权威定义
#include "CGameManager.h"
#include "CInventory.h"
#include "APSystemTypes.h"
#include "GameBasicsScripts.h"
#include "CParty.h"
#include "SkillSlot.h"
#include "WarRoom.h"
#include "PvP_Room.h"   // add_pvp_result/update_pvp_point 房间查询（get_user_seat 等）
#include "SyncScriptDeps.h"   // toTString（RewardItem2DeleteInvalidItem 字符串表翻译）
#include "PacketBuf.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "RDARScriptStringManager.h"   // g_scriptStringManager_.findString
#include "CItemAmplifier.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "CPowerManager.h"    // s_power_manager 完整类型（make_basic_info 排名查询）
#include "AuctionProxy.h"    // CAuctionServerProxy/CCeraAuctionServerProxy 完整类型（IsRunning）
#include "CDeathTowerRanking.h"
#include "Packet_Register_GM_MID.h"
#include "Packet_Randombox_statistic.h"  // CUser::SendRandomBoxStatistic（ORIG 0x08676826）
#include "Packet_Event_UpgradeJar_Output_Statistic_GTS.h"  // SendEventJarOutputItemStatistics
#include "CConditionEventManager.h"
#include "CAssaultMgr.h"
#include "CMissionList_Charac.h" // CMissionList_Charac 任务列表查询（charac_expand 槽位 8）
#include "RefPvpGrade.h"        // RefPvpGrade::GetPvpCurrRankPoint/GetPvpNextRankPoint
#include "QuestClear.h"         // WongWork::CQuestClear::getClearedQuest（UserQuest +0x04）
#include "CUserGlobalInfoHandle.h"  // CUserGlobalInfoHandleInstance/reset_uniqueid_flag
#include "CBoosterGage.h"       // charac_expand::CData（CDataMgr 20 槽 ResetDaily 循环）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（isBoundaryExpValue/GetLevel）
#include "Redeem_Item__CRedeemItem.h"  // Redeem_Item::CRedeemItem（isAddableFilter/AddRedeemList）
#include <algorithm>
#include <iostream>
#include "CQuestShop.h"          // set_grow_type 清 QP
#include "Quest.h"               // Quest/MonsterKillItem（CheckQuestMonster）
#include "CUserProc.h"           // APSystem::_ActionGroupIndex / CUserProc
#include "HeroMissionEvent.h"     // ARAD::Singleton<HeroMissionEvent>
#include "EpollHandler.h"        // G_EpollHandler
#include "MsgQueueMgr.h"         // s_msgq_mgr->put
#include "CStreamGuard.h"                  // CStreamGuard（SendAradEventItemLog）
#include "StreamPool.h"                    // GlobalData::s_stream_pool->Acquire
#include "Arad_StatisticsEventItemList.h"  // Arad_DataManager Singleton/needStatistics
// SIG_* 布局经 CDoubleConnCheckServerProxy.h → SigTypes.h 可见。
// ORIG T 0x08655327（自由函数）
void SetUserManagerToolIP(CUser* user, SIG_UPDATE_LOGOUT* sig);
#include "CDoubleConnCheckServerProxy.h"

// stBuySkillInfo 完整定义（与 CEventCharacterHandler.cpp 一致；CUser.h 仅前向声明）
struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};

// np_server_xml::g_ServerString_（ORIG 0x93fa660 B；定义在 CBoosterGage.cpp）
namespace np_server_xml
{
class CServerXml
{
public:
    std::string GetServerString(int nIndex, bool* pFlag) const;
};
extern CServerXml g_ServerString_;
}

// ORIG W 0x082a70ab（自由函数）
class EpollHandler;
EpollHandler* G_EpollHandler();


// private_store 域最小前向声明（完整定义见 CPrivateStoreMgr.h；避免其头链
// 引入与 SkillSlot.h 冲突的 SigTypes.h 包含序问题）
namespace private_store
{
class CPrivateStore;
class CPrivateStoreMgr
{
public:
    CPrivateStore* FindStoreSeller(CUser* user);
    bool IsBusyPrivateStore(CUser* user);   // ORIG 0x85cad5e（isEnableAvatarSocketAction 调用）
};
CPrivateStoreMgr* GetInstancePrivateStoreMgr();
}

// CBattle_Field（本地最小视图；get_dungeon_index 定义于 CBattle_Field.cpp，
// ORIG 0x0822d0d8，符号 _ZN13CBattle_Field17get_dungeon_indexEv。完整类头
// CBattle_Field.h 与 WarField.h 的 map_item/map_monster 重复类型冲突，属
// FixMapMonsterReal/FixWarFieldTypes 域，此处仅声明所需方法）
class CBattle_Field
{
public:
    int get_dungeon_index();
};

// ---- 外部符号声明（对应 TU 翻译后移除） ----
extern unsigned char _NS_PI_2ND_GetDefaultRandomHashKey();

extern void GetPacketName(unsigned char area, unsigned short packetId);

extern char IsLightServer();   // ORIG 0x0822ad44 W
extern void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow);  // 0x08668823 T


namespace ARAD
{
namespace DISPATCHER
{
void make_cmd_packetheader_jpn(PacketGuard& packet, int cmd, int param);
}
}

namespace item_lock
{
}
// CItemLock 权威声明在 CTitleBook.h（item_lock::CItemLock）

// ---- G1-3 临时外部桩（对应 TU 翻译后移除） ----
unsigned char _NS_PI_2ND_GetDefaultRandomHashKey() { return 0; }
void GetPacketName(unsigned char, unsigned short) {}
void ARAD::DISPATCHER::make_cmd_packetheader_jpn(PacketGuard&, int, int) {}
void WongWork::CMCAPManager::resetExposedCount() {}
// 0x0864809c：antibot 校验/加密发送主链路（依据 AntibotSend.md 反编译；
// s_antibotChecker_ vtable +0x34 校验 / +0x28 加密，ORIG 栈缓冲此处以 vector 等价）。
extern void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user); // ORIG T 0x082751ee
int CUser::AntibotSend(PacketBuf& packet)
{
    char* base = packet.get_packet(0);
    unsigned short packetId = *(unsigned short*)(base + 1);
    bool verified = false;
    if (packet.isEncryptRequired())
    {
        typedef int (*VerifyFn)(void*, int, unsigned short);        // vtable +0x34
        void* checker = GlobalData::s_antibotChecker_;
        VerifyFn fn = *(VerifyFn*)((*(void***)checker)[0x34 / 4]);
        verified = fn(checker, getAntibotKey(), packetId) != 0;
    }
    if (verified)
    {
        const int bufLen = packet.get_len() + 500;
        std::vector<char> out(bufLen, 0);
        typedef int (*EncryptFn)(void*, int, char*, int, char*, int*);  // +0x28
        void* checker = GlobalData::s_antibotChecker_;
        EncryptFn fn = *(EncryptFn*)((*(void***)checker)[0x28 / 4]);
        int outLen = bufLen;
        if (fn(checker, getAntibotKey(), packet.get_packet(0),
               packet.get_len(), &out[0], &outLen) == 0)
        {
            PacketGuard guard;
            ((InterfacePacketBuf*)&guard)->put_header(0, 0x231);
            ((InterfacePacketBuf*)&guard)->put_binary(&out[0], bufLen);
            ((InterfacePacketBuf*)&guard)->finalize(false);
            Send(guard);
            return 1;
        }
    }
    PacketBuf sendBuf;
    char* data = packet.get_packet(0);
    int len = packet.get_len();
    if (verified || packet.isEncryptRequired())
    {
        sendBuf.copy(packet);
        sendBuf.encrypt_packet();
        data = sendBuf.get_packet(0);
        len = sendBuf.get_len();
        sendBuf.encFinalize();
    }
    _NS_PI_2ND_SendHash_SC(data, (unsigned short)len, this);
    if (m_network.send_packet(data, len) < 0)
    {
        if (packetId != 199)
            DisConnSig((DISCONN_SIG)6, false, 0);
        return 0;
    }
    return 1;
}
void Secu_AccountHacking::resetInfo() {}

_Quest_Authen_Data::_Quest_Authen_Data() { memset(&m_characNo, 0, 0x48); }   // ORIG memset 0x48（覆盖尾部 m_field761c/d）
void _Quest_Authen_Data::reset() { memset(&m_characNo, 0, 0x48); }
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
// WongWork::CDeathTowerRanking 权威声明见 CDeathTowerRanking.h（唯一声明点，
// 定义在 CDeathTower.cpp；本地同名类已删除，§7）。
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
// _SIG_LOAD_ACTION_POINT 权威定义见 CDataManager.h（经其传递包含），
// 本 TU 不再本地镜像；DB_UpdateActionPoint::makeRequest 仅指针使用。
class DB_UpdateActionPoint
{
public:
    // 与 ORIG/DB_AccountCargoSync.h 一致：int + const _SIG_LOAD_ACTION_POINT& + bool，
    // 符号 _ZN8APSystem20DB_UpdateActionPoint11makeRequestEiRKNS_22_SIG_LOAD_ACTION_POINTEb。
    static void makeRequest(int uid, const _SIG_LOAD_ACTION_POINT& data,
                            bool save);
};
}   // APSystem（完整声明见 CUserProc.h）

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

// CCoinEventPerDay 最小镜像（docs/class_func_reports/CCoinEventPerDay/）：
// 每日币事件。GetCoinNoPerDay 按等级档位取 +0x24/+0x28/+0x2c 处 int
// （level<0x12 / <0x1b / 其余），字段语义推断（档位币数）。
class CCoinEventPerDay : public CEventBase
{
public:
    int GetCoinNoPerDay(int level);   // 0x0810ad16
};

int CCoinEventPerDay::GetCoinNoPerDay(int level)
{
    if (level < 0x12)
        return *(int*)((char*)this + 0x24);
    if (level < 0x1b)
        return *(int*)((char*)this + 0x28);
    return *(int*)((char*)this + 0x2c);
}

namespace CerashopAddRestrict
{
}

namespace WongWork
{
void CSecurityCard::resetCancelCnt() {}
int CSecurityCard::getCancelCnt() { return 0; }
void CAvatarItemMgr_Reset(CUser*) {}
void CDeathTowerRanking_unreg(unsigned int) {}
}


void UserMercenaryInfoMgr::AddCharac(const CHARAC_LOAD_MERCENARY& info) {}
// ORIG T 0x081b441a：按 characNo 线性查找并 erase（ORIG 经 FindIndexByCharacNo，
// 内联等价）。
void UserMercenaryInfoMgr::RemoveCharac(unsigned int characNo)
{
    for (size_t i = 0; i < m_characs.size(); ++i)
    {
        if ((unsigned int)m_characs[i].m_characNo == characNo)
        {
            m_characs.erase(m_characs.begin() + i);
            return;
        }
    }
}
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
advancealtar::CharacAdvanceAltarManager::CharacAdvanceAltarManager() : m_field0(0), m_data() {}
advancealtar::CharacAdvanceAltarManager::~CharacAdvanceAltarManager() {}
PIReceiverManager::PIReceiverManager() {}
PIReceiverManager::~PIReceiverManager() {}
PISenderManager::PISenderManager() {}
PISenderManager::~PISenderManager() {}
// ORIG T 0x0833c28a：休息(3)↔战斗(1) 两态切换；无宠物早退。
void user_creature::CCreatureMgr::TurnStomach(bool fighting)
{
    if (m_equippedCreature == 0)
        return;
    if (fighting)
    {
        if (m_equippedCreature->GetState() == 3)
            m_equippedCreature->SetState(m_user, (CREATURE_STATE)1);
    }
    else
    {
        if (m_equippedCreature->GetState() == 1)
            m_equippedCreature->SetState(m_user, (CREATURE_STATE)3);
    }
}
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

// ORIG 0x0832ade8：对 20 个成员（+0x00..+0x4c，各为 charac_expand::CData*）
// 依次调用虚表槽 0（ResetDailyMidnight）。
// ORIG 0x0832adaa：同上，调用虚表槽 3（ResetDaily）。
void charac_expand::CDataMgr::ResetDailyMidnight()
{
    charac_expand::CData** entries =
        reinterpret_cast<charac_expand::CData**>(m_pad);
    for (int i = 0; i < 0x14; ++i)
    {
        entries[i]->ResetDailyMidnight();
    }
}

void charac_expand::CDataMgr::ResetDaily()
{
    charac_expand::CData** entries =
        reinterpret_cast<charac_expand::CData**>(m_pad);
    for (int i = 0; i < 0x14; ++i)
    {
        entries[i]->ResetDaily();
    }
}

// ORIG T 0x0832ad30（SaveInventory 直接依赖）：`this[type*4]`（CData* 槽）isSave→saveData。
// 与同文件 ResetDaily/ResetDailyMidnight 一致的 CData*[0x14] 槽解释；type 越界不访问。
char charac_expand::CDataMgr::update(CUser* pUser, ENUM_CHARAC_EXPAND_TYPE type)
{
    char result = 1;
    charac_expand::CData** entries =
        reinterpret_cast<charac_expand::CData**>(m_pad);
    int idx = (int)type;
    if (idx >= 0 && idx < 0x14)
    {
        charac_expand::CData* pData = entries[idx];
        if (pData->isSave())
        {
            if (pData->saveData(pUser) != 1)
                result = 0;
        }
    }
    return result;
}
void BingoData::clear() {}
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
        // ORIG（CUser::ResetCurCharac 反汇编实证）：characNo = getCurCharacNo()，
        // 然后 getRankTable(i)->unregistBestRecord(characNo)（成员签名，见 CDeathTowerRanking.h）。
        unsigned int characNo = getCurCharacNo();
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistBestRecord(characNo);
        characNo = getCurCharacNo();
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistRanking(characNo);
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

// ORIG 0x0818d3fe：在 map<uchar,short>(+0x8eb50) 中按 type 查疲劳值，未命中返回 0。
short CUser::get_server_fatigue_count(unsigned char type)
{
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(type);
    if (it == m_map8eb50.end())
        return 0;
    return it->second;
}

// ORIG 0x0818d4c4：若当前疲劳 DB 类型(+0x8eb4d) == type 返回 (+0x8eb4c) 否则 0。
int CUser::get_fatigue_count_db(int type)
{
    if ((unsigned char)m_field8eb4d == (unsigned char)type)
        return (unsigned char)m_field8eb4c;
    return 0;
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

int CUser::getDungeonIdxAfterClear()
{
    CParty* party = (CParty*)GetParty();
    if (!party)
        return -1;
    return reinterpret_cast<CBattle_Field*>((char*)party + 0xb24)
        ->get_dungeon_index();
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

// CUser::GetUserPosInParty @ 0x08688638 —— 返回 own 在队伍中的槽位，不在返回 -1。
int CUser::GetUserPosInParty()
{
    CParty* party = (CParty*)GetParty();
    if (party == 0)
        return -1;
    return party->GetUserPosition(this);
}

void* CUser::GetWarRoom()
{
    if (m_field8d008 < 0)
        return 0;
    return G_CGameManager()->GetWarRoom(m_field8d008);
}

// ORIG T 0x0864df12：遍历 m_characList（this+0x796e8），首字段 m_characNo
// 与 characNo 相等则返回下标，否则返回 -1。
int CUser::get_charac_idx(unsigned int characNo) const
{
    int idx = 0;
    for (std::vector<_Charac_info>::const_iterator it = m_characList.begin();
         it != m_characList.end(); ++it)
    {
        if ((unsigned int)it->m_characNo == characNo)
            return idx;
        ++idx;
    }
    return -1;
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
    unsigned char lock =
        ((stAmplifyOption_t*)((char*)pInven->GetInvenRef(a, b) + 0x11))->GetLock();
    if (lock == 0)
        return 0;
    // ORIG 0x86469d4：((CItemLock*)GetCharacExpandDataR(2))->CheckItemLock(lock)。
    if (!((item_lock::CItemLock*)GetCharacExpandDataR(
              (ENUM_CHARAC_EXPAND_TYPE)2))
             ->CheckItemLock(lock))
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

// ============================================================================
// CUser::make_basic_info —— ORIG _ZN5CUser15make_basic_infoEPcc @0x0865a44e
// 报告：docs/class_func_reports/CUser/make_basic_info.md（## 汇编 + ## 反编译 C
// 全文核对）。按 type 分派：0=ENUM_USERINFO_MINIMUM、1=ENUM_USERINFO_ADDITION、
// 2=账号角色列表（m_characList 循环）、3=ENUM_USERINFO_OTHER_CHARACTER；
// 其余值直接返回 1。三个需当前角色的分支在 getCurCharacR()==0 时
// LogManager::logFormat 报错并返回 0；各分支 lock → 填包 → unlock。
// ============================================================================

// ---- checkSpecialChar / checkSpecialCharName（ORIG 自由函数，分支 2 使用）----
// ORIG _Z16checkSpecialCharPc T 0x0829efae / _Z20checkSpecialCharNamePc T 0x0829f01f。
// 依赖的 CDataManager 名字过滤四方法均已建模；后续若其它 TU 需要应迁往公共 TU，
// 届时删除此处定义避免重复符号。
bool checkSpecialChar(char* name)
{
    if (G_CDataManager()->getCharacNameFilterPass())
    {
        G_CDataManager()->setCharacNameFilterPass(false);
        return false;
    }
    return G_CDataManager()->hasSpecialCharacter(name)
           || G_CDataManager()->hasPreventString(name);
}

bool checkSpecialCharName(char* name)
{
    if (G_CDataManager()->getCharacNameFilterPass())
    {
        G_CDataManager()->setCharacNameFilterPass(false);
        return false;
    }
    return G_CDataManager()->hasPreventStringName(name);
}

namespace
{

#pragma pack(push, 1)

// PvpResultType 只读视图：权威最小定义（CUserCharacInfo.h）中 +0x5c 尚在 m_pad5c
// 内；ORIG 分支 0/3 以 int 读该槽（全量布局见 PvpResultType.h：今日胜场【推断】）。
struct PvpResultBasicView
{
    int m_totalWinCount;            // +0x00
    int m_field04;                  // +0x04
    int m_field08;                  // +0x08
    int m_field0c;                  // +0x0c
    int m_field10;                  // +0x10
    int m_pvpGrade;                 // +0x14（与 get_pvp_grade 同源）
    int m_pad18[(0x5c - 0x18) / 4];
    int m_field5c;                  // +0x5c（分支 0/3 写出槽）
};
typedef char pvp_view_size_ok[(sizeof(PvpResultBasicView) == 0x60) ? 1 : -1];

// getCurCharacR() 当前角色数据扩展字节视图（权威声明中这些偏移均在 pad 区；
// +0x39 起 0x14 字节与 CommonTime 创建时间区对齐【推断】）。
struct CurCharacByteView
{
    char m_pad00[0x39];
    char m_field39;                 // +0x39（分支 3）
    char m_field3a;                 // +0x3a（分支 3）
    char m_field3b;                 // +0x3b（分支 3）
    char m_pad3c[0x75 - 0x3c];
    unsigned char m_field75;        // +0x75（分支 0，无符号写出）
};

// m_characList 元素扩展视图：权威 struct _Charac_info（CUser.h，pack(1)，总长
// 0x14d1）中段为 pad；此处仅命名 ORIG 分支 2 访问的偏移（视图止于 +0x14ca）。
struct CharacListElemView
{
    int m_characNo;                 // +0x000
    char m_name[0x1d];              // +0x004
    char m_pad21[0x26 - 0x21];
    char m_job;                     // +0x026
    short m_level;                  // +0x027
    char m_field29;                 // +0x029
    char m_pad2a[0x83 - 0x2a];
    char m_forbidPlayFlag;          // +0x083（禁玩标记；到期清零并投递 SIG_FORBID_USER_TO_PLAY）
    int m_forbidPlayLeftSec;        // +0x084（剩余秒）
    char m_pad88[0xda - 0x88];
    char m_inventory[0xe40 - 0xda]; // +0x0da 每角色 CInventory 子对象（MakeEquipList 接收者）
    char m_fielde40;                // +0xe40
    char m_pade41[0xe66 - 0xe41];
    unsigned char m_reenterable;    // +0xe66（now < expireSec → 1，随 viewState 写出）
    char m_pade67[0x1240 - 0xe67];
    unsigned char m_field1240;      // +0x1240
    char m_pad1241[0x1246 - 0x1241];
    char m_field1246;               // +0x1246
    char m_pad1247[0x12ad - 0x1247];
    int m_expireSec;                // +0x12ad（与 CSystemTime::getCurSec() 比较）
    char m_pad12b1[0x14c6 - 0x12b1];
    int m_viewState;                // +0x14c6（==0xff 时写 8）
};
typedef char elem_view_size_ok[(sizeof(CharacListElemView) == 0x14ca) ? 1 : -1];

// GetGuildDBInfo()（STGuildDBInfoOnly，CUser.h 中 +0x00..0x94 为 pad）访问视图：
// +0x00 公会名字符串、+0x1b 随名写出的字节、+0x44 成员数、成员表自 +0x45 起
// 5 字节/项（int 等级 + char 职位）。
struct GuildMemberEntryView
{
    int m_grade;                    // 条目 +0x00（>0xc7 才写出）
    char m_position;                // 条目 +0x04（有符号写出）
};

struct GuildDbInfoView
{
    char m_guildName[0x1b];         // +0x00
    unsigned char m_field1b;        // +0x1b
    char m_pad1c[0x44 - 0x1c];
    unsigned char m_memberCount;    // +0x44
    GuildMemberEntryView m_members[1]; // 实际数量由 m_memberCount 给出
};
typedef char guild_view_size_ok[(sizeof(GuildDbInfoView) == 0x4a) ? 1 : -1];

// TOD_UserState::getEnterCount（ORIG _ZNK13TOD_UserState13getEnterCountEv T
// 0x08643872：*(int*)(this+0x10)；类未建模，本地只读视图，同 CDungeon.cpp 惯例）。
struct TodEnterCountView
{
    char m_pad0[0x10];
    int m_enterCount;               // +0x10
};

// CCharacBloodDungeon::GetEnterCount（ORIG _ZNK18CCharacBloodDungeon13GetEnterCountEv
// W 0x085bfe88：*(int*)(this+0x0c)；类未建模，本地只读视图）。
struct BloodEnterCountView
{
    char m_pad0[0xc];
    int m_enterCount;               // +0x0c
};

// SIG_FORBID_USER_TO_PLAY 前 9 字节写入视图（SigTypes.h 现为 0x100 pad 占位；
// ORIG：memset 9 字节后 +0x00=characNo、+0x04=0、+0x05..0x08=0）。
struct SigForbidUserToPlayBody
{
    unsigned int m_characNo;        // +0x00
    unsigned char m_field04;        // +0x04
    unsigned int m_field05;         // +0x05..0x08
};

#pragma pack(pop)

}  // namespace

int CUser::make_basic_info(char* buf, char type)
{
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)buf;
    PacketGuard& pg = *(PacketGuard*)buf;

    if (type == 1)
    {
        // ---- 分支 1：ENUM_USERINFO_ADDITION（ORIG +0xbb9）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                                  "bool CUser::make_basic_info(char*, char)",
                                  0x31c3,
                                  "Error ENUM_USERINFO_ADDITION, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        // 公会技能/成员双组块 ×2（保留索引-回填计数模式）
        const GuildDbInfoView* guild =
            (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        for (int group = 0; group < 2; ++group)
        {
            const int skillBase = (group == 0) ? 6 : 0xe;
            const int slotBase = (group == 0) ? 0x20 : 0xe8;
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                const char* skillR = getCurCharacSkillR();
                if (skillR[skillBase + (i + slotBase) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)skillR[skillBase + (i + slotBase) * 2]);
                    pkt.put_byte(
                        (int)(char)skillR[skillBase + 1 + (i + slotBase) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        pkt.put_byte(6);
        for (int i = 0; i < 6; ++i)
            pkt.put_byte((int)(char)getDemensionInoutValue((char)i));
        if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            pkt.put_byte(0);
        else
            pkt.put_byte((int)GetCurRevengeDungeonCount());
        pkt.put_byte((int)(char)getUltimateInoutValue(2));
        const TodEnterCountView* tod =
            (const TodEnterCountView*)GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0xd);
        pkt.put_byte(tod->m_enterCount);
        int limit = (int)(char)G_CDataManager()->get_limit_inout_count(
            ENUM_BLOOD_TYPE_1);
        const BloodEnterCountView* blood =
            (const BloodEnterCountView*)GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)1);
        if (limit - blood->m_enterCount < 0)
            pkt.put_byte(0);
        else
            pkt.put_byte(limit - blood->m_enterCount);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    else if (type < 2)
    {
        if (type == 0)
        {
            // ---- 分支 0：ENUM_USERINFO_MINIMUM（ORIG +0x52）----
            if (getCurCharacR() == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp", "bool CUser::make_basic_info(char*, char)",
                    0x308c,
                    "Error ENUM_USERINFO_MINIMUM, getCurCharacR() == 0");
                return 0;
            }
            lock();
            pkt.put_short((int)(get_unique_id() & 0xffff));
            const char* name = getCurCharacName();
            pkt.put_int((int)strlen(name));
            pkt.put_str(name, (int)strlen(name));
            pkt.put_byte(get_charac_job());
            pkt.put_byte((int)getCurCharacGrowType());
            if (G_GameWorld()->IsCharacterLevelRevisionChannel())
                pkt.put_byte(0x55);
            else
                pkt.put_byte(get_charac_level());
            pkt.put_byte(get_pvp_grade());
            const PvpResultBasicView* pvp =
                (const PvpResultBasicView*)getPVPResultR();
            pkt.put_byte(pvp->m_field5c);
            pkt.put_byte(GetUserState());
            ((CInventory*)getCurCharacInvenR())
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)0,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
                pkt.put_int((int)GetSchoolNo());
            if (G_GameWorld()->GetChannelType() == 0xe)
                pkt.put_int(GetCurCharacOnlinePreliminaryTeamId());
            else
                pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());

            const user_creature::CCreatureMgr* creatureMgr =
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
            pkt.put_int(creatureMgr->GetCreatureItemId());
            const char* creatureName = creatureMgr->GetCreatureName();
            pkt.put_int((int)strlen(creatureName));
            pkt.put_str(creatureName, (int)strlen(creatureName));
            pkt.put_byte(creatureMgr->IsDieCreature() ? 0 : 1);

            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_int(getCurrCharacBlackCount());

            const GuildDbInfoView* guild =
                (const GuildDbInfoView*)GetGuildDBInfo();
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
            {
                // BLOCKED（依赖未建模）：ORIG
                //   _ZN12CLeagueMatch22GetPlayCountPerOneTeamEv @0x084ed37b W
                // 读全局字节 CLeagueMatch::nPlayerCountPerOneTeam_
                // （@0x093abb74 D）；CLeagueMatch 类与该全局变量本工程尚未建模，
                // 不伪造返回值，此 put_byte 暂缺（其余字段照常）。
                pkt.put_int((int)m_str8e080.length());
                pkt.put_str(m_str8e080.c_str(), (int)m_str8e080.length());
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                // BLOCKED（依赖未建模）：ORIG
                //   _ZN18online_preliminary18COnlinePreliminary22GetPlayCountPerOneTeamEv
                //   @0x0855fee8 T（= G_CDataManager()->GetPvPTeamCount()，
                //   _ZN12CDataManager15GetPvPTeamCountEv @0x08568ce0 未建模）。
                // 不伪造返回值，此 put_byte 暂缺（其余字段照常）。
                const char* teamName = GetCurCharacOnlinePreliminaryTeamName();
                pkt.put_int((int)strlen(teamName));
                pkt.put_str(teamName, (int)strlen(teamName));
            }
            else
            {
                pkt.put_byte((int)guild->m_field1b);
                pkt.put_int((int)strlen(guild->m_guildName));
                pkt.put_str(guild->m_guildName, (int)strlen(guild->m_guildName));
            }

            pkt.put_int(GetCurCharacChaosPoint());
            pkt.put_byte(0xe < GetAge() ? 1 : 0);
            pkt.put_byte((int)(char)getPowerSide());
            pkt.put_byte((int)(char)isJoinPowerWar());
            pkt.put_byte(GetCurCharacExpertJobType());
            pkt.put_int(GetCurCharacExpertJobExp());
            pkt.put_byte((int)getDisguiseKind());
            pkt.put_short((int)getDisguiseIndex());
            pkt.put_byte((int)(char)get_charac_visible_values());

            const char* curName = getCurCharacName();
            if (G_GameWorld()->IsForbiddenChat(curName))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenChat(curName);
                if (left == 0)
                    G_GameWorld()->EnableChat(curName);
                {
                    cMyTrace trace("bool CUser::make_basic_info(char*, char)",
                                   0x3154, 0);
                    trace("%s FCT1: %ld\n", curName, left);
                }
                left = left / 60000u;
                {
                    cMyTrace trace("bool CUser::make_basic_info(char*, char)",
                                   0x3156, 0);
                    trace("%s FCT2: %ld\n", curName, left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }
            if (G_GameWorld()->IsForbiddenMove(curName))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenMove(curName);
                if (left == 0)
                    G_GameWorld()->EnableMove(curName);
                {
                    cMyTrace trace("bool CUser::make_basic_info(char*, char)",
                                   0x3163, 0);
                    trace("%s FMT1: %ld\n", curName, left);
                }
                left = left / 60000u;
                {
                    cMyTrace trace("bool CUser::make_basic_info(char*, char)",
                                   0x3165, 0);
                    trace("%s FMT2: %ld\n", curName, left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }

            pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
            const CurCharacByteView* curBytes =
                (const CurCharacByteView*)getCurCharacR();
            pkt.put_byte((int)curBytes->m_field75);
            pkt.put_short((int)getCurCharacUsedFatigue());
            pkt.put_byte((int)GetServerGroup());
            pkt.put_short((int)(short)GetUserPowerWarPoint());
            pkt.put_byte(GlobalData::s_power_manager->GetRankingByCharacNo(
                (unsigned int)getCurCharacNo()));
            pkt.put_short((int)(short)GetCurCharacEmotion());
            pkt.put_byte((int)(char)GetCurCharacSkillTreeIndex());
            pkt.put_byte(IsReturnUser() != 0 ? 1 : 0);
            char linkType = getCharacLinkType();
            pkt.put_byte((int)getLinkCharacSlotIndex());
            pkt.put_byte((int)linkType);
            unlock();
        }
    }
    else if (type == 2)
    {
        // ---- 分支 2：账号角色列表（ORIG +0x19e3；无当前角色守卫）----
        lock();
        pkt.put_byte((int)getCharacSlotLimit());
        pkt.put_byte((int)getSlotEffectCount());
        pkt.put_byte(GetManageLevel());
        pkt.put_int(GetManagePoint());
        int written = 0;
        int countIdx = pkt.get_index();
        pkt.put_short(written);
        const int nowSec = GlobalData::s_systemTime_.getCurSec();
        for (int i = 0; i < (int)m_characList.size(); ++i)
        {
            CharacListElemView& v =
                reinterpret_cast<CharacListElemView&>(m_characList[i]);
            if (G_GameWorld()->GetChannelType() == 0xe
                && (unsigned int)v.m_characNo != getOnlinePreliminaryCharacNo())
                continue;
            pkt.put_short(written);
            ++written;
            pkt.put_int((int)strlen(v.m_name));
            pkt.put_str(v.m_name, (int)strlen(v.m_name));
            bool special = checkSpecialChar(v.m_name) != false
                           || checkSpecialCharName(v.m_name) != false;
            pkt.put_byte(special ? 1 : 0);
            pkt.put_byte((int)(char)v.m_job);
            pkt.put_byte((int)(char)v.m_field29);
            pkt.put_byte((int)v.m_level);
            pkt.put_byte(0);
            if (!(nowSec < v.m_expireSec))
                v.m_reenterable = 0;
            else
                v.m_reenterable = 1;
            if (v.m_viewState == 0xff)
                pkt.put_byte(8);
            else
                pkt.put_byte(v.m_reenterable != 0 ? 4 : 0);
            ((CInventory*)v.m_inventory)
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)2,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());
            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_byte((int)(char)v.m_fielde40);
            pkt.put_byte((int)v.m_field1240);
            pkt.put_byte((int)(char)v.m_field1246);
            if (v.m_forbidPlayFlag == '\0')
            {
                pkt.put_int(0);
            }
            else if (v.m_forbidPlayLeftSec < 1)
            {
                pkt.put_int(0);
                v.m_forbidPlayFlag = 0;
                v.m_forbidPlayLeftSec = 0;
                Stream* stream =
                    GlobalData::s_stream_pool->Acquire("user.cpp", 0x344c);
                CStreamGuard guard(stream, true);
                guard << 0x93;
                guard << (int)GetUID();
                SigForbidUserToPlayBody* body =
                    guard.GetInBuffer<SigForbidUserToPlayBody>();
                memset(body, 0, 9);
                body->m_characNo = (unsigned int)v.m_characNo;
                body->m_field04 = 0;
                body->m_field05 = 0;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            else
            {
                int days = v.m_forbidPlayLeftSec / 0x15180;
                pkt.put_int(days < 1 ? 1 : days);
            }
        }
        pkt.put_short(countIdx, written);
        unlock();
    }
    else if (type == 3)
    {
        // ---- 分支 3：ENUM_USERINFO_OTHER_CHARACTER（ORIG +0x127f）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(
                1, "user.cpp", "bool CUser::make_basic_info(char*, char)",
                0x328c,
                "Error ENUM_USERINFO_OTHER_CHARACTER, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)3, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        const GuildDbInfoView* guild = (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        for (int group = 0; group < 2; ++group)
        {
            const int skillBase = (group == 0) ? 6 : 0xe;
            const int slotBase = (group == 0) ? 0x20 : 0xe8;
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                const char* skillR = getCurCharacSkillR();
                if (skillR[skillBase + (i + slotBase) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)skillR[skillBase + (i + slotBase) * 2]);
                    pkt.put_byte(
                        (int)(char)skillR[skillBase + 1 + (i + slotBase) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        const PvpResultBasicView* pvp = (const PvpResultBasicView*)getPVPResultR();
        pkt.put_int(pvp->m_totalWinCount);
        pkt.put_int(pvp->m_field04);
        pkt.put_int(pvp->m_pvpGrade);
        pkt.put_byte(pvp->m_field5c);
        const CurCharacByteView* curBytes =
            (const CurCharacByteView*)getCurCharacR();
        pkt.put_byte((int)(char)curBytes->m_field39);
        pkt.put_byte((int)(char)curBytes->m_field3a);
        pkt.put_byte((int)(char)curBytes->m_field3b);
        pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
        pkt.put_short((int)(short)GetUserPowerWarPoint());
        pkt.put_int(GetGuildPowerWarPoint());
        const char* guildName = GetGuildName();
        pkt.put_int((int)strlen(guildName));
        pkt.put_str(guildName, (int)strlen(guildName));
        pkt.put_byte((int)GetGuildLevel());
        pkt.put_byte((int)(char)getPowerSide());
        pkt.put_byte(IsExistGuildAgit() ? 1 : 0);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    return 1;
}
void CUser::send_equip(int slot)
{
    // ORIG 0x0865dd14：state(+0x8cfc4) ∈ {5,8,10,0xc} 且 slot==0x16 → 发给自己；
    // 否则（slot<0xc 或 0x16）→ 广播 make_basic_info(type 0) 给全房间。
    if (slot >= 0xc && slot != 0x16)
        return;
    int state = m_field8cfc4;   // +0x8cfc4（推断：角色状态）
    bool inState = (state == 0xc) || (state == 5) || (state == 8) || (state == 10);
    if (inState)
    {
        if (slot != 0x16)
            return;
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
    }
    else
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        G_GameWorld()->send_all(packet, this);
    }
}

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

// 0x0868bc7c
// ORIG：无角色 → -1；无专家职业 → 0；否则按 GetExpertJobScript 的
// isBoundaryExpValue/GetLevel（_ZN17STExpertJobScript*）计算等级，
// 边界值时用 ExpertJobEtcScript 内 map<uchar,short>（等级→所需角色等级）修正：
// 查不到或角色等级不足 → 等级 -1。
int CUser::GetCurExpertJobLevel(int exp)
{
    if (getCurCharacR() == 0)
        return -1;
    if (GetCurCharacExpertJobType() == 0)
        return 0;
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
    if (script == 0)
        return 0;
    char isBoundary = script->isBoundaryExpValue(exp);
    int level = script->GetLevel((unsigned int)exp);
    if (isBoundary != 0)
    {
        void* etc = G_CDataManager()->GetExpertJobEtcScript();
        // 推断：etc 对象首部为 std::map<uchar,short>（等级 → 所需角色等级）
        if (etc != 0)
        {
            std::map<unsigned char, short>* m =
                (std::map<unsigned char, short>*)etc;
            std::map<unsigned char, short>::iterator it =
                m->find((unsigned char)level);
            if (it != m->end() && it->second <= get_charac_level())
                return level;
        }
        level = level - 1;
    }
    return level;
}

bool CUser::is_clear_stealingSkillMission() const
{
    return mission_list_is_clear(GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
}

// ORIG T 0x08679736：公会技能公告。文案 = ServerString(0x3ea) + "%d "(count) +
// skillName + ServerString(0x3eb)；总长 ≥0x78 丢弃；公会公告板写入
// (boardNo=0, charNo, job=-1, growType=-1, guildKey)。
void CUser::makeGuildSkillMessage(const char* skillName, int count)
{
    if (skillName == 0)
        return;
    char countStr[256];
    memset(countStr, 0, sizeof(countStr));
    std::string body = np_server_xml::g_ServerString_.GetServerString(0x3ea, 0);
    sprintf(countStr, "%d ", count);
    body += countStr;
    body += std::string(skillName);
    body += np_server_xml::g_ServerString_.GetServerString(0x3eb, 0);
    if (body.length() >= 0x78)
        return;
    char board[120];
    memset(board, 0, sizeof(board));
    memcpy(board, body.c_str(), body.length());
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (proxy != 0)
        proxy->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                                   -1, -1, (unsigned int)get_charac_guildkey());
}

// ============================================================================
// CUser::makeGuildItemUpgradMessage @ 0x8679cee
//   公会强化公告。isAmplify && to>10 时用 "+to itemName"；否则 from>=0xb 时
//   用 "+{from+1} itemName"。长度<0x78，经 CGuildServerProxy::SendGuildBoardWrite
//   广播。字符串槽位沿用 makeGuildSkillMessage 族（0x3ea/0x3eb 前缀/后缀）。
// ============================================================================
void CUser::makeGuildItemUpgradMessage(const char* itemName, int from,
                                       int to, bool isAmplify)
{
    if (itemName == 0)
        return;
    char numStr[256];
    memset(numStr, 0, sizeof(numStr));
    std::string body = np_server_xml::g_ServerString_.GetServerString(0x3ea, 0);
    if (isAmplify)
    {
        if (to <= 10)
            return;
        sprintf(numStr, "+%d %s", to, itemName);
    }
    else
    {
        if (from < 0xb)
            return;
        sprintf(numStr, "+%d %s", from + 1, itemName);
    }
    body += numStr;
    body += np_server_xml::g_ServerString_.GetServerString(0x3eb, 0);
    if (body.length() >= 0x78)
        return;
    char board[120];
    memset(board, 0, sizeof(board));
    memcpy(board, body.c_str(), body.length());
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (proxy != 0)
        proxy->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                                   -1, -1, (unsigned int)get_charac_guildkey());
}

// ORIG 0x086787fc：改成长类型主分支（set_grow_type.md）。校验 first<6/second<3；
// second==0 重做任务列表(0x1f→0x20)；reason==1||second==0 清 QP；写历史日志；
// setCurCharGrowthType → enableSaveCharacView → adjust_charac_stat（未建模，推断）
// → 按 reason 分支 get_give_skill 取技能表 growtype_skill 双树写入；
// 成功后 _UpdateUserInfo4Guild + Guild/Monitor SendCharLevelGrowType。
void CUser::set_grow_type(unsigned char firstGrow, unsigned char secondGrow,
                          char* name, eChangeGrowTypeReason reason)
{
    (void)name;   // 推断：包内编码分支未完整建模（InterfacePacketBuf 回填 API 待核）
    if (firstGrow >= 6 || secondGrow >= 3)
    {
        LogManager::logFormat(1, "user.cpp",
            "void CUser::set_grow_type(unsigned char, unsigned char, char*, eChangeGrowTypeReason)",
            0x713b, "User %s - grow type < 1 || grow type >= GROW_TYPE_NUMBER",
            getCurCharacName());
        return;
    }

    if (reason == (eChangeGrowTypeReason)1 || secondGrow == 0)
    {
        CQuestShop* questShop =
            (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
        if (questShop != 0)
        {
            questShop->clearQP(this);
            questShop->sendCharacQp(this, 1);
        }
    }

    char oldGrow = getCurCharacGrowType();
    m_historyLog.ChangeGrowType(
        get_charac_job(), (int)(signed char)getCurCharacVill(),
        secondGrow == 0 ? (int)firstGrow : (int)secondGrow,
        (int)(signed char)oldGrow, secondGrow == 0 ? 1 : 2, reason);
    setCurCharGrowthType((char)firstGrow, (char)secondGrow);
    m_characterView.enableSaveCharacView();

    std::vector<std::pair<int, int> > giveVec;
    bool ok = false;
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        ok = true;
    }
    else if (reason == (eChangeGrowTypeReason)2 ||
             reason == (eChangeGrowTypeReason)0)
    {
        CCharacter* chars = *(CCharacter**)((char*)G_CDataManager() + 0x14);
        ok = chars[get_charac_job()].get_give_skill(firstGrow, secondGrow,
                                                    giveVec, 2) == 1;
    }
    else if (reason == (eChangeGrowTypeReason)1)
    {
        CCharacter* chars = *(CCharacter**)((char*)G_CDataManager() + 0x14);
        ok = chars[get_charac_job()].get_give_skill(
            firstGrow, secondGrow == 0 ? 0 : secondGrow, giveVec,
            secondGrow == 0 ? 1 : 0) == 1;
    }
    if (!ok)
    {
        LogManager::logFormat(1, "user.cpp",
            "void CUser::set_grow_type(unsigned char, unsigned char, char*, eChangeGrowTypeReason)",
            0x720c,
            "User %s - CUser::set_grow_type first_grow_type %d, second_grow_type %d",
            getCurCharacName(), (int)firstGrow, (int)secondGrow);
        return;
    }
    for (size_t i = 0; i < giveVec.size(); ++i)
    {
        for (int round = 0; round <= 1; ++round)
        {
            ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                get_charac_job(), giveVec[i].first, giveVec[i].second,
                (ENUM_SKILL_TREE_KIND)round);
        }
    }
    send_skill_info();
    _UpdateUserInfo4Guild();
    short growNow = (short)(signed char)getCurCharacGrowType();
    CGuildServerProxy* guild =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (guild != 0)
        guild->SendCharLevelGrowType(get_acc_id(),
                                     (unsigned int)get_charac_no(-1),
                                     (short)get_charac_level(), growNow);
    CMonitorServerProxy* monitor =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
    if (monitor != 0)
        monitor->SendCharLevelGrowType(get_acc_id(),
                                       (unsigned int)get_charac_no(-1),
                                       (short)get_charac_level(), growNow);
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

// ---- quest point / piece helpers（ORIG：CUser 0x868ad88 / 0x868afd0 / 0x868adc2 / 0x868af66）----

void CUser::clearInitQuestPoint()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
        shop->setInitQp(0);
}

void CUser::clearQuestPiece()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
        shop->setPiece(0);
}

void CUser::incInitQuestPoint(int val)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
        shop->incInitQp(val);
}

void CUser::incQuestPiece(int val, int piece, bool send)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->getPiece();
        shop->addPiece(val);
        if (!send)
            shop->sendCharacQuestPiece(this);
    }
}

void CUser::gainWinPoint(int winPoint, eWPAddReason reason)
{
    // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c。
    if (winPoint != 0)
    {
        // iVar1 = CUserCharacInfo::getPVPResultW(this); *(int*)(iVar1+0x58) += param_1
        PvpResultType* wpW = getPVPResultW();
        wpW->m_winPoint += winPoint;
        // iVar1 = getPVPResultR(this); cUserHistoryLog::WPAdd(this+0x79700, *(int*)(iVar1+0x58), param_1, param_3)
        PvpResultType* wpR = getPVPResultR();
        m_historyLog.WPAdd(wpR->m_winPoint, winPoint, reason);
    }
}

void CUser::setInitQuestPoint()
{
    // ORIG 0x868adfc T：清空 init QP 与 piece，再按 NPC 任务表逐条累加已清任务奖励点。
    clearInitQuestPoint();
    clearQuestPiece();

    std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE);
    if (npcList == 0)
        return;

    for (std::multimap<int, int>::const_iterator it = npcList->begin();
         it != npcList->end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);
        if (quest != 0)
        {
            WongWork::CQuestClear* cleared =
                (WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4);
            if (cleared->isClearedQuest((unsigned int)it->second))
            {
                incInitQuestPoint(quest->m_field138);
                incQuestPiece(quest->m_field13a, quest->m_index, true);
            }
        }
    }
}

WongWork::CUserPremium* CUser::GetPremiumInfo() const
{
    return (WongWork::CUserPremium*)&m_premium;
}

// 推断：ORIG 无独立报告。每日币事件加成，按 premium type 0xc 生效状态返回加成量；
// 无字段证据，无 premium 优势时返回 0（与 ORIG 无加成路径一致）。
int WongWork::CUserPremium::GetAdvantageCoin() const
{
    return 0;   // 推断：premium 币加成数据字段未建模
}

// 推断：ORIG 无独立报告（CharacManageScript::GetGoldBonus 为另一全局表版本）。
// 升级金币奖励，无 premium 时 ORIG 返回 0。
int WongWork::CUserPremium::GetGoldBonus(int level) const
{
    return 0;   // 推断：premium 金币奖励表未建模
}

// 推断：ORIG 无独立报告。无限疲劳 premium 判定。
bool WongWork::CUserPremium::isAffectedUnlimitFatigue() const
{
    return false;   // 推断：无 premium 时 ORIG 为 false
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

int CUser::get_pvp_WinningRate_relateMission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return mission->getWinningRate();
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

// ORIG T 0x0864f7fe sz0xeb：扣除 1 点疲劳（先消耗高级会员疲劳额度）。
//   min(premiumFatigue, maxPremiumFatigue) → 不足时回落到普通疲劳。
void CUser::gainCurCharacFatigue(int value)
{
    unsigned int maxPrefatigue = 0;
    if (m_premium.GetAdvantageFatigueRate() != 0)
        maxPrefatigue = getCurCharacMaxPremiumFatigue() & 0xffff;

    unsigned int prefatigue = getCurCharacPremiumFatigue() & 0xffff;
    unsigned int premiumVal = prefatigue < maxPrefatigue ? prefatigue : maxPrefatigue;
    int remain = (int)premiumVal - value;
    if (remain < 0)
    {
        if ((int)premiumVal > 0)
            setCurCharacPremiumFatigue(0);
        unsigned int normal = getCurCharacFatigue() & 0xffff;
        int sum = (int)normal + remain;
        setCurCharacFatigue(sum < 0 ? 0 : sum);
    }
    else
    {
        setCurCharacPremiumFatigue((unsigned short)(premiumVal - (unsigned int)value));
    }
}

bool CUser::has_within_Mission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return mission->getIndex_byKind(0x1b) != 0;
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

// ORIG T 0x08680a96：拍卖操作前置门（AuctionPreCheck.md）。
// 控制流：
//   1) pay==1（cera 拍卖）且 CCeraAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   2) pay==0（普通拍卖）且 CAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   3) get_state()<3 或 getCurCharacR()==0 → 返回 0x8151；
//   4) CSecu_ProtectionField::Check(this, 0x35) !=0 → 报对应错误，返回 0；
//   5) 全部通过 → 返回 -1。
int CUser::AuctionPreCheck(ENUM_CMDPACKET cmd, PAY_TYPE pay)
{
    bool fail;

    if (pay == (PAY_TYPE)1)
    {
        fail = !GlobalData::s_cera_auction_proxy->IsRunning();
    }
    else
    {
        fail = false;
    }

    if (fail)
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (pay == (PAY_TYPE)0)
    {
        fail = !GlobalData::s_auction_proxy->IsRunning();
    }
    else
    {
        fail = false;
    }

    if (fail)
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (get_state() < 3 || getCurCharacR() == 0)
    {
        return 0x8151;
    }

    unsigned int err =
        GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x35);
    if (err != 0)
    {
        SendCmdErrorPacket(cmd, (unsigned char)err, pay);
        return 0;
    }

    return -1;
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

int CUser::isEnableAvatarSocketAction()
{
    // ORIG _ZN5CUser26isEnableAvatarSocketActionEv @ 0x086810a8
    // 检查链：安全保护(0x11) → 私摊忙 → 幽灵角色 → 交易中 → 袭击区 → PVP 频道。
    // 安全保护命中返回其错误码，其余任一命中返回 0x13（禁止），全部通过返回 0（允许）。
    int ret = GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x11);
    if (ret != 0)
        return ret;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
        return 0x13;
    if (IsCurCharacGhost())
        return 0x13;
    if (CheckInTrade())
        return 0x13;
    if (GetAssaultPlace() != 0)
        return 0x13;
    if (G_GameWorld()->IsPVPChannel())
        return 0x13;
    return 0;
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

WongWork::CDeathTower* CUser::getDeathTower()
{
    // ORIG 0x086552a4：idx<0 => NULL；否则 G_CGameManager()->getDeathTower(idx)
    int idx = (int)getDeathTowerIndex();
    if (idx < 0) return 0;
    return G_CGameManager()->getDeathTower(idx);
}

// ---- Redeem_Item::CRedeemItem（GetCharacExpandData(6) 返回，未建模为
// CExpandEquipslot* 槽位；权威声明见 Redeem_Item__CRedeemItem.h，实现于
// Redeem_Item__CRedeemItem.cpp，ORIG 0x085f76f0 / 0x085f71a8）----

int CUser::Add_RedeemInfo(const Inven_Item& item, int count, bool isRedeem)
{
    // ORIG 0x086472c0：GetCharacExpandData(6)->isAddableFilter(item)，可加则记录历史并 AddRedeemList
    Redeem_Item::CRedeemItem* cdata =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (cdata == 0) return 0;
    if (!cdata->isAddableFilter(item)) return 0;

    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem && pItem->is_stackable())
        m_historyLog.RedeemItemAdd(item.m_addInfo, item.get_add_info());
    else
        m_historyLog.RedeemItemAdd(item.m_addInfo, 1);

    return cdata->AddRedeemList(item, count, isRedeem);
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

bool CUser::IsOverBirthDay()
{
    // ORIG 0x0867ec66：出生月/日是否已过（用于 PowerWar 最低年龄判定）。
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&cur, &local);
    int curMonth = local.tm_mon + 1;
    int curDay = local.tm_mday;
    unsigned char birthMon = *(unsigned char*)((char*)this + 0x711d5);
    unsigned char birthDay = *(unsigned char*)((char*)this + 0x711d4);
    if (birthMon < curMonth)
        return true;
    if (birthMon == curMonth && birthDay < curDay)
        return true;
    return false;
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

// ORIG 0x08230860 (W)：movzbl +0x8d245（m_ceraUserInfo.m_field61）
unsigned char CUser::getCharacSlotLimit() const
{
    return m_ceraUserInfo.m_field61;
}

// ORIG 0x08230870 (W)：movzbl +0x8d244（m_ceraUserInfo.m_field60）
unsigned char CUser::getSlotEffectCount() const
{
    return static_cast<unsigned char>(m_ceraUserInfo.m_field60);
}

// ORIG 0x08230880 (W)：写 +0x8d244（m_ceraUserInfo.m_field60）
void CUser::setSlotEffectCount(unsigned char count)
{
    m_ceraUserInfo.m_field60 = static_cast<char>(count);
}

// ORIG 0x08676826 (T)：组 Packet_Randombox_statistic（header 0x17b6/0xc，boxKind@+0xa、
// actionKind@+0xb）经 CStatisticServerProxy::SendPacket(pkt, 0xc) 发统计服务器（组 0）。
void CUser::SendRandomBoxStatistic(int boxKind, ENUM_RANDOMBOX_ACTION_KIND actionKind)
{
    Packet_Randombox_statistic pkt;
    pkt.m_boxKind = static_cast<char>(boxKind);
    pkt.m_actionKind = static_cast<char>(actionKind);
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    if (proxy != 0)
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xc);
}

// ORIG 0x0868f6bc (T)：find_item(itemIdx)→verifyStackableItemProcess
int CUser::verifyStackableItemActionPlace(StackableAction::Type type, unsigned int itemIdx)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    return verifyStackableItemProcess(type, item);
}

// ORIG 0x0868f60e (T)：is_stackable→verifyStackableAction→verifyUsablePlace→verifyUsableServerType
int CUser::verifyStackableItemProcess(StackableAction::Type type, const CItem* item)
{
    if (item == 0)
        return 0;
    if (!item->is_stackable())
        return 0;
    const CStackableItem* cstackable = static_cast<const CStackableItem*>(item);
    if (!cstackable->verifyStackableAction(type))
        return 0;
    // 非 const 方法（CStackableItem::verifyUsablePlace/verifyUsableServerType）：
    // ORIG Ghidra 中以 CStackableItem* 调用，mangled 参数为 CItem const*，此处还原。
    CStackableItem* stackable = const_cast<CStackableItem*>(cstackable);
    if (!stackable->verifyUsablePlace(this))
        return 0;
    if (!stackable->verifyUsableServerType())
        return 0;
    return 1;
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
    return m_mileage;
}

int CUser::GetPCRoomNo()
{
    return m_codeHackCheck.m_field54;
}

unsigned int CUser::GetCeraPoint()
{
    return m_ceraPoint;
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
    return m_debugCommand;
}

int CUser::GetTradePunishType()
{
    return m_codeHackCheck.m_tradePunishType;
}

int CUser::getLastLotteryTime()
{
    return m_mcap.m_lastLotteryTime;
}

int CUser::GetNonClientRandInt()
{
    return m_piSender.m_nonClientRandInt;
}

int CUser::GetNonClientFlag()
{
    return m_piSender.m_nonClientFlag;
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
    m_ceraPoint = point;
}

void CUser::SetDebugCommand(int cmd)
{
    m_debugCommand = cmd;
}

void CUser::decre_check_count()
{
    *(short*)((char*)&m_field8cfc8) = 0;
}

void CUser::setLastLotteryTime(unsigned long t)
{
    m_mcap.m_lastLotteryTime = (int)t;
}

void CUser::SetNonClientRandInt(int v)
{
    m_piSender.m_nonClientRandInt = v;
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
    m_piSender.m_nonClientFlag = flag ? 1 : 0;
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
    return m_character.m_f44;
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

bool CUser::acceptable_within_mission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    int grade = get_pvp_grade();
    int idx = G_CDataManager()->get_WithinMissionIndex(grade);
    if (idx == 0)
        return false;
    return mission->MakeMissionList_JustKind(*this, 0x1b);
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
        char value = (char)G_CDataManager()->get_limit_inout_count((ENUM_BLOOD_TYPE)2);
        setUltimateInoutValue(2, value);
    }
}

void CUser::send_pvp_record()
{
    PacketGuard packet;
    packet.put_header(0, 0x30);
    const PvpResultType* result = getPVPResultRefR();
    packet.put_int(result->m_totalWinCount);
    packet.put_int(result->m_totalLoseCount);
    packet.put_int(result->m_expPoint);
    packet.put_int(GlobalData::s_ref_pvp_grade->GetPvpCurrRankPoint(result->m_pvpGrade));
    packet.put_int(GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(result->m_pvpGrade));
    packet.put_byte(result->m_pvpGrade);
    packet.put_byte(*(unsigned char*)&result->m_pad5c[0]);
    packet.finalize(true);
    Send(packet);
}

void CUser::send_clear_quest_list()
{
    PacketGuard packet;
    packet.put_header(0, 0x163);
    packet.put_int(0x7530);
    UserQuest* quest = getCurCharacQuestR();
    void const* cleared = reinterpret_cast<WongWork::CQuestClear*>((char*)quest + 4)
                              ->getClearedQuest();
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
void CUser::SendConditionEventInfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xea);
    CConditionEventManager* pMgr = G_CGameManager()->GetConditionEventManager();
    if (pMgr)
    {
        int eventIdx = pMgr->GetCurEventIndex();
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

void CUser::ResetDailyQuest()
{
    std::list<int> questList;
    QuestList* pQuestList = *(QuestList**)((char*)G_CDataManager() + 0x18);
    pQuestList->GetDailyQuestList(questList);
    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        UserQuest* pQuest = getCurCharacQuestR();
        if (pQuest && pQuest->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
}

void CUser::ResetTrainingQuest()
{
    std::vector<int> questList;
    short level = get_charac_level();
    QuestList* pQuestList = *(QuestList**)((char*)G_CDataManager() + 0x18);
    pQuestList->GetTrainingQuestList(questList, level);
    for (std::vector<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        UserQuest* pQuest = getCurCharacQuestR();
        if (pQuest && pQuest->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
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

void CUser::send_MissionList()
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    mission->Send_MissionList(*this);
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

// ===================== GameStubs 迁移（ORIG 强符号 T；最小正确语义） =====================
// 全部签名经 nm -C /home/wangyilei/下载/df_game_r 核实。

// 0x086489f4
void CUser::DisConnSig(DISCONN_SIG sig, bool flag, int param)
{
    // 0x086489f4 主分支（DisConnSig.md）：持锁；删 Arad 周期事件奖励数据；
    // socket<0 或 state==0 仅 trace 返回；非 ProgLogout 时按 sig==5 重映射
    // 0x30/0x31/0x32，sig∉{6,0x30..0x32} 先发包 199(int sig)，ResetEpoll 后
    // 组流(<<1<<uid) 投递队列 1。深水区统计上报见报告（推断，暂略）。
    Mutex& mtx = *(Mutex*)((char*)this + 0x8cfa0);   // 推断：连接互斥量
    mtx.lock();

    // 推断：ORIG 经 ARAD::Singleton<Arad_DataManager>::findGameScript 取
    // Arad_EventPeriodDataManager 并 delUserRewardData(this)；该 Singleton 特化
    // 未实例化，留待 ARAD 域批次。

    int sock = *(int*)((char*)this + 0xe0);   // 推断：CNetwork socket 字段
    int chState = m_field8cfc4;
    if (sock < 0 || chState == 0)
    {
        mtx.unlock();
        return;
    }

    if (!IsProgLogout())
    {
        SetProgLogout();
        char ip[16];
        bool haveIp = ((CNetwork<4096, 450000>*)((char*)this + 0xe0))
                          ->GetPeerIP2(ip, 0x10) != 0;
        (void)haveIp;
        int reason = (int)sig;
        if (reason == 5)
        {
            if (get_acc_id() == 0 && ip[0] != '\0')      { reason = 0x30; flag = true; }
            else if (get_acc_id() == 0 && ip[0] == '\0') { reason = 0x32; flag = true; }
            else                                         { reason = 0x31; flag = true; }
        }
        SendClientCrashDownLogToLogDB(reason);   // ORIG T 0x0864870e（声明见 CUser.h）

        if (sock != -1)
        {
            if (reason != 6 && reason != 0x30 && reason != 0x31 && reason != 0x32)
            {
                PacketGuard pkt;
                ((InterfacePacketBuf*)&pkt)->put_header(0, 199);
                ((InterfacePacketBuf*)&pkt)->put_int(reason);
                ((InterfacePacketBuf*)&pkt)->finalize(true);
                Send(pkt);
            }
            G_EpollHandler()->ResetEpoll(sock);
            Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x991);
            CStreamGuard guard(s, true);
            guard << (int)1;
            guard << (int)GetUID();
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
        }
    }
    mtx.unlock();
}

// 0x08665400：PvP 技能树发放主循环（givePvPSkillTree.md）：
// secondGrow∈[0..CurSecondGrow]、grade∈[a..pvp_grade]；脚本 miss 跳过；
// mode 1/2 只发树 0/树 1，mode 3 双树；b=true 且已有等级≥目标跳过。
void CUser::givePvPSkillTree(int a, bool b, int c)
{
    for (int secondGrow = 0;
         secondGrow <= (int)(signed char)getCurCharSecondGrowType(); ++secondGrow)
    {
        for (int grade = a; grade <= get_pvp_grade(); ++grade)
        {
            std::vector<std::pair<int, int> >* tree = 0;
            PvPSkillTreeParameterScript* script =
                (PvPSkillTreeParameterScript*)((char*)G_CDataManager() + 0xa800);
            if (!script->getPvPSkillTree(get_charac_job(),
                                         (int)(signed char)getCurCharFirstGrowType(),
                                         secondGrow, grade, b, &tree))
                continue;
            for (size_t i = 0; i < tree->size(); ++i)
            {
                const int skillIdx = (*tree)[i].first;
                const int level = (*tree)[i].second;
                SkillSlot* slotR = (SkillSlot*)getCurCharacSkillR();
                SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
                if (c == 1 || c == 3)
                {
                    if (!(b && slotR->FindSkill(skillIdx,
                            (ENUM_SKILL_TREE_KIND)0) >= level))
                        slotW->growtype_skill(get_charac_job(), skillIdx, level,
                                              (ENUM_SKILL_TREE_KIND)0);
                }
                if (c == 2 || c == 3)
                {
                    if (!(b && slotR->FindSkill(skillIdx,
                            (ENUM_SKILL_TREE_KIND)1) >= level))
                        slotW->growtype_skill(get_charac_job(), skillIdx, level,
                                              (ENUM_SKILL_TREE_KIND)1);
                }
            }
        }
    }
}

// 0x0867a95c

// 0x086804ce
// ORIG：断线回调。state==7（PvP 房间）时通知 PvP_Room::Disconnect；
// 在袭击区域（GetAssaultPlace != 0）时通知 CAssaultMgr::OnLeaveAssaultPlace(user, false)。
void CUser::OnDisconnect()
{
    if (get_state() == 7)
    {
        short pvpIdx = GetPvpIndex();
        PvP_Room* pRoom = G_CGameManager()->GetPvp((int)pvpIdx, this, 0);
        if (pRoom != 0)
            pRoom->Disconnect(this);
    }
    if (GetAssaultPlace() != 0)
    {
        pvp_assault::CAssaultMgr* pMgr = pvp_assault::GetInstanceAssaultMgr();
        if (pMgr != 0)
            pMgr->OnLeaveAssaultPlace(this, false);
    }
}

// 0x0867a95c
void CUser::SetCharacLevel(int level)
{
    // 0x0867a95c 主分支（SetCharacLevel.md）：等级钳制[1,200]；按方向取
    // GetSpAtLevelUp 计算 SP 增量加到树 0/1 并 clamp；exp=get_level_exp；
    // SFP：<50 清 idx2/3，旧级<0x33 则=level-0x31 否则各+差值；逐级加/减
    // 成长属性、写日志、premium 金币奖励；专家/公会/监控/副本通知收尾。
    if (getCurCharacR() == 0)
        return;
    if (level < 1) level = 1;
    if (level > 200) level = 200;

    int oldLevel = get_charac_level();
    int spGain = (oldLevel < level)
                     ? G_CDataManager()->GetSpAtLevelUp(level)
                     : G_CDataManager()->GetSpAtLevelUp(level + 1);
    int spDiff = (level - oldLevel) * spGain;

    setCurCharacExp(G_CDataManager()->get_level_exp(level));

    for (int kind = 0; kind <= 1; ++kind)
    {
        ENUM_SKILL_TREE_KIND k = (ENUM_SKILL_TREE_KIND)kind;
        int v = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(k) + spDiff;
        if (v > 0x3fffffff) v = 0x3fffffff;
        if (v < 0) v = 0;
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(v, k);
    }

    if (level < 0x32)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)3);
    }
    else
    {
        int sfp2, sfp3;
        if (oldLevel < 0x33)
        {
            sfp2 = sfp3 = level - 0x31;
        }
        else
        {
            sfp2 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)2) + (level - oldLevel);
            sfp3 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)3) + (level - oldLevel);
        }
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp2, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp3, (ENUM_SKILL_TREE_KIND)3);
    }

    // 成长类型属性表：G_CDataManager()+0x14 为 CCharacter 表基址（job 步长 0x7dc，推断）
    CCharacter* growTable = *(CCharacter**)((char*)G_CDataManager() + 0x14);
    const void* growStat = growTable[get_charac_job()].get_growtype_stat(
        getCurCharFirstGrowType(), getCurCharSecondGrowType());
    (void)growStat;   // 属性 add/subtract 走 _Additioal_info（未建模，推断保留控制流）

    while (get_charac_level() != level)
    {
        if (get_charac_level() < level)
        {
            incCurCharacLevel();
            m_historyLog.LevelUp(get_charac_level(), (eLevelUpReason)1);
            int bonus = m_premium.GetGoldBonus(get_charac_level());
            if (bonus != 0)
            {
                getCurCharacInvenW()->gain_money(bonus, (eMoneyAddReason)0xd, true, 0);
                SendUpdateItemList((eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
            }
        }
        else
        {
            decCurCharacLevel();
            m_historyLog.LevelDown(get_charac_level(), (eLevelDownReason)0);
        }
        m_premium.RecalcAdditionalInfo(this);
    }
    setCurCharacLevel(level);

    makeGuildLevelUpMessage(level);   // ORIG T 0x8679754

    if (level == 0x46 &&
        GlobalData::s_pGMAccounts_->isGM(get_acc_id()) != 1)
    {
        char pkt[0x28];   // Packet_Monitor_Max_Level_BroadCast（名字位于 +0x0a，推断）
        memset(pkt, 0, sizeof(pkt));
        strncpy(pkt + 0x0a, getCurCharacName(), 0x1e);
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket(pkt, 0x28);
    }

    if (get_charac_level() > 0x3b)
        DimensionInoutUpdate(true, true);

    m_characterView.enableSaveCharacView();
}

// 0x0866afa2：习得单技能主分支（master_new_skill.md）：
// skillIdx∈[0xc8,0xd0] 走公会技能（需会长/副会长）并经 guild proxy 上报；
// 否则普通 SkillSlot::master_new_skill。回包 header(1,0x1f)：失败字节0+错误码
// 映射{-1:0x15,-2:0x16,-3:0x12,-4:0x01,-5:0x5b,-6:0x5c}；成功 byte1+treeIdx+
// 剩余SP/SFP(short)+新槽位+skillIdx+等级。
void CUser::master_new_skill(stBuySkillInfo& info, bool flag)
{
    (void)flag;
    int treeIdx = GetCurCharacSkillTreeIndex();
    unsigned char skillIdx = (unsigned char)info.m_skillIdx;
    int ret;
    int learnCnt = 0;
    int needGold = 0;
    short needGSP = 0;
    bool isGuildSkill = (skillIdx >= 0xc8 && skillIdx <= 0xd0);

    if (isGuildSkill)
    {
        if (!IsGuildMaster() && !IsSubGuildMaster())
            return;
        unsigned short guildLevel = *(unsigned short*)((char*)&m_guildDB + 0x42);
        ret = ((SkillSlot*)getCurCharacSkillR())->master_new_guild_skill(
            (int)skillIdx, get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            m_guildDB, (int)getCurCharacMoney(),
            needGold, learnCnt, needGSP, (int)guildLevel);
        if (ret >= 0)
        {
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            if (proxy != 0)
                proxy->SendBuyGuildSkill((unsigned int)get_charac_no(-1),
                                         (unsigned int)get_charac_guildkey(),
                                         (int)skillIdx, learnCnt, needGSP,
                                         (unsigned int)needGold);
        }
    }
    else
    {
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (info.m_field0 == 1)
                return;
        }
        else if (info.m_field0 != 1)
        {
            return;
        }
        ret = ((SkillSlot*)getCurCharacSkillW())->master_new_skill(
            (int)skillIdx, get_charac_level(), get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            (int)info.m_count);
    }

    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(1, 0x1f);
    if (ret < 0)
    {
        pkt->put_byte(0);
        static const unsigned char errMap[6] = {0x15, 0x16, 0x12, 0x01, 0x5b, 0x5c};
        unsigned char e = (ret >= -6 && ret <= -1) ? errMap[-ret - 1] : 0;
        pkt->put_byte(e);
    }
    else
    {
        pkt->put_byte(1);
        pkt->put_byte(treeIdx);
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)treeIdx));
        int sfpKind = (treeIdx == -1 || treeIdx == 0) ? 2 : 3;
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)sfpKind));
        if (!isGuildSkill)
        {
            pkt->put_byte(ret);
            pkt->put_byte(skillIdx);
            pkt->put_byte(((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret));
        }
        else
        {
            pkt->put_byte(0xff);
            pkt->put_byte(skillIdx);
            pkt->put_byte(learnCnt);
        }
    }
    pkt->finalize(true);
    Send(packet);
}

// 0x0867b048

// ORIG：GM 改成长类型。无角色/参数越界(0..5, 0..1) → 0 并 cerr 报错；
// 否则 debugCheckGrowTypeSkill 校验 → set_grow_type(reason=2) →
// _postCheckForceChangeGrowType 后处理，返回 1。
void CUser::ChangeGrowType_GM(int a, int b)
{
    if (getCurCharacR() == 0)
        return;
    if (a < 0 || a > 5 || b < 0 || b > 1)
    {
        std::cerr << "Invalid grow type , first :" << a << "second :" << b << std::endl;
        return;
    }
    int level = get_charac_level();
    unsigned int overLevel = m_premium.getOverSkillLevel();
    SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
    slotW->debugCheckGrowTypeSkill(level + (int)(overLevel & 0xffff),
                                   get_charac_job(), a, b);
    set_grow_type((unsigned char)a, (unsigned char)b, 0, (eChangeGrowTypeReason)2);
    _postCheckForceChangeGrowType(this, a, b);
}

// 0x0867b6d4
// ORIG：添加物品。角色空/交易中/物品不存在/count<0 → -1；
// 构造 Inven_Item（make_item），stackable 则 set_add_info(count)；
// *space 默认 0（生物物品 → 7）；slot∈(0,0x20) 且为装备 → SetUpgrade(slot)；
// insertItemIntoInventory(reason, true, true)，失败 -1；
// 生物物品再 InsertCreatureItem。
int CUser::AddItem(int itemIdx, int count, eItemAddReason reason,
                   ENUM_ITEMSPACE& space, int slot)
{
    if (getCurCharacR() == 0)
        return -1;
    if (CheckInTrade() != 0)
        return -1;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;
    if (count < 0)
        return -1;
    Inven_Item inven;
    item->make_item(inven);
    if (item->is_stackable() != 0)
        inven.set_add_info(count);
    space = (ENUM_ITEMSPACE)0;
    if (inven.IsCreatureItemType() != 0)
        space = (ENUM_ITEMSPACE)7;
    if (slot > 0 && slot < 0x20 && inven.m_field1 == (unsigned char)1)
        inven.SetUpgrade((unsigned char)slot);
    CInventory* invenW = getCurCharacInvenW();
    int ret = invenW->insertItemIntoInventory(inven, reason, true, true);
    if (ret < 0)
        return -1;
    if (inven.IsCreatureItemType() != 0)
    {
        int expiration = item->getExpirationDate();
        int usable = item->getUsablePeriod();
        getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
            &inven, ret, (int)reason, usable, expiration);
    }
    return ret;
}

// 0x0864cb66
void CUser::LogoutCachedCharacter(unsigned char param)
{
    // 0x0864cb66 主分支（LogoutCachedCharacter.md）：货币日志 → 缓存角色内存 →
    // Guild/Monitor 代理 Logout → param==0 双开检测 UDP Logout → PC 房通知 →
    // 清 ontime 奖励 → HeroMission 存档。
    SendMoenyLog();
    int cached = CacheCharacterMemory();   // 0/1；透传给 CMonitorServerProxy::SendLogout（ORIG uVar1）

    unsigned int memberKey = (unsigned int)get_charac_memberkey();
    unsigned int guildKey = (unsigned int)get_charac_guildkey();
    unsigned int characNo = (unsigned int)get_charac_no(-1);
    const char* name = getCurCharacName();
    int channel = G_CEnvironment()->get_channel_no();
    unsigned int accId = get_acc_id();

    CGuildServerProxy* guildProxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (guildProxy != 0)
        guildProxy->SendLogout(accId, channel, (char*)name, characNo,
                               guildKey, memberKey, param);

    CMonitorServerProxy* monProxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
    if (monProxy != 0)
        monProxy->SendLogout(accId, channel, (char*)name, characNo,
                             guildKey, memberKey, param,
                             (unsigned char)(cached ? 1 : 0));

    if (param == 0)
    {
        int dcGroup = *(int*)((char*)G_CEnvironment() + 0x378);   // 推断：双开检测 group
        GlobalData::s_double_check_proxy->SendLogout(accId, channel, dcGroup);
    }

    if (getCurCharacR() != 0)
        SendLogoutToPCRoomServer();       // ORIG T 0x0868170c

    delete_ontime_reward_list();          // ORIG T 0x0868c3bc
    // 推断：ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true)
    //（ORIG T 0x08166d38）待 HeroMission 域补声明后接线。
}

// 0x08689a22：表情配置重算（ReCalcChattingEmoticon.md）。读 GameOption 表情配置 →
// CDataManager 重算 → 写回；变化则组 SIG_SAVE_CHATTING_EMOTICON（0x214：+0 accId、
// +4.. 配置 0x210）投递队列 2。SIG 当前 pad 布局按偏移写入（推断）。
void CUser::ReCalcChattingEmoticon()
{
    CGameOption* opt = GetGameOptionRef();
    CChattingEmoticonConfig cfg = opt->getEmoticonInfo();
    G_CDataManager()->ReCalcEmoticonConf(cfg, *this);
    if (!opt->SetEmoticonInfo(cfg))
        return;

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xa076);
    CStreamGuard guard(s, true);
    guard << (int)0x2d7;
    guard << (int)GetUID();
    char* sig = (char*)guard.GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
    *(unsigned int*)(sig + 0) = get_acc_id();
    memcpy(sig + 4, &cfg, 0x210);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// 0x08689010：表驱动重置列表（ResetItemByScript.md）：元素 +0 itemspace、
// +4 到期秒；到期则压 (space,0)。
void CUser::ResetItemByScript(std::vector<std::pair<int, int> >& list)
{
    std::vector<stResetItemInfo::ResetItem>* items =
        G_CDataManager()->GetResetItemInfo();
    int now = GlobalData::s_systemTime_.getCurSec();
    for (size_t i = 0; i < items->size(); ++i)
    {
        if ((*items)[i].m_resetSec <= now)
            list.push_back(std::make_pair((*items)[i].m_space, 0));
    }
}

// 0x08689b90：聊天表情配置包(0x19b)。24 行：index、m_nCount(行内 +0x14)、
// 名字（行首 0x14 字节）。行大小 0x16（CChattingEmoticonConfigRow, pack(1)）。
void CUser::SendChattingEmoticon()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x19b);
    ((InterfacePacketBuf*)&packet)->put_short(0x18);
    CChattingEmoticonConfig cfg = GetGameOptionRef()->getEmoticonInfo();
    const char* p = (const char*)&cfg;
    for (int i = 0; i < 0x18; ++i)
    {
        const char* row = p + i * 0x16;
        ((InterfacePacketBuf*)&packet)->put_short(i);
        ((InterfacePacketBuf*)&packet)->put_short(*(const short*)(row + 0x14));
        int len = (int)strlen(row);
        ((InterfacePacketBuf*)&packet)->put_int(len);
        ((InterfacePacketBuf*)&packet)->put_str(row, len);
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x08653270：登出前数据保存主链路（UpdateData.md）。守卫 state>2 且频道!=7；
// CheckNFix；会员税上报并清零；_SaveCharacData/UpdateCharacView/HeroMission；
// 各 isSaveXxx 脏块经 StreamPool+CStreamGuard 组包投递队列 2 后 disable。
void CUser::UpdateData()
{
    if (m_field8cfc4 <= 2)
        return;
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    if (getCurCharacR() == 0 || isLocked4DataLoad())
        return;

    if (getCurCharacInvenR()->checkToFix())
        getCurCharacInvenW()->CheckNFix();

    if (getCurCharacR() != 0 && get_charac_memberkey() != 0)
    {
        int payTaxMoney = get_member_pay_tex_money_to_upper();
        int payTaxFatigue = get_member_pay_tex_fatigue_to_upper();
        CMonitorServerProxy* monProxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (monProxy != 0)
            monProxy->SendMemberPayTax(is_connect_upper_member() != 0,
                                       (unsigned int)getCurCharacNo(),
                                       payTaxMoney, payTaxFatigue);
        if (payTaxMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payTaxFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    _SaveCharacData();                    // ORIG T 0x08650aa2
    UpdateCharacView();
    // 推断：saveEventData(this,false) 同上待接线。

    if (isSaveInven() || isSaveCargo())
    {
        SaveInventory();
        disableSaveInven();
        disableSaveCargo();
    }

    WongWork::CMailBoxHelper::ReqDBSendStoredMail(this);

    if (isSaveSkill())
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1ded);
        CStreamGuard guard(s, true);
        guard << (int)0x24;              // SIG_LOAD_SKILL
        guard << (int)GetUID();
        char* sig = (char*)guard.GetInBuffer<SIG_LOAD_SKILL>();
        memset(sig, 0, 0x414);
        *(unsigned int*)(sig + 0) = (unsigned int)getCurCharacNo();
        *(sig + 0x411) = *((char*)G_CDataManager() + 0xa850);   // 推断：全局开关
        int sp1, sp2, sfp1, sfp2;
        ((SkillSlot*)getCurCharacSkillR())->get_remain_sp(&sp1, &sp2, &sfp1, &sfp2);
        *(int*)(sig + 0x8) = sp1;   *(int*)(sig + 0x1a4) = sp2;
        *(int*)(sig + 0x340) = sfp1; *(int*)(sig + 0x344) = sfp2;
        _Mastered_skill* buf0 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)0);
        _Mastered_skill* buf1 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)1);
        if (buf0 != 0 && buf1 != 0)
        {
            memcpy(sig + 0xc, buf0, 0x198);
            memcpy(sig + 0x1a8, buf1, 0x198);
        }
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveSkill();
    }

    if (isSavePvP())
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e2c);
        CStreamGuard guard(s, true);
        guard << (int)0x25;              // SIG_SAVE_PVP
        guard << (int)GetUID();
        char* sig = (char*)guard.GetInBuffer<SIG_SAVE_PVP>();
        memset(sig, 0, 0x88);
        *(unsigned int*)(sig + 0x84) = (unsigned int)getCurCharacNo();
        memcpy(sig, getPVPResultRefR(), sizeof(PvpResultType) < 0x84
                   ? sizeof(PvpResultType) : 0x84);   // 推断：PvpResultType 整体落盘
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSavePvP();
    }

    if (isSaveDungeonClear())
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e4e);
        CStreamGuard guard(s, true);
        guard << (int)0x27;              // SIG_SAVE_DUNGEON_CLEAR
        guard << (int)GetUID();
        char* sig = (char*)guard.GetInBuffer<SIG_SAVE_DUNGEON_CLEAR>();
        memset(sig, 0, 0x180c);
        *(unsigned int*)(sig + 0) = (unsigned int)getCurCharacNo();
        *(unsigned int*)(sig + 4) = get_acc_id();
        // 推断：WongWork::CDungeonClear::toString(sig+0xc, 0x1800) 序列化，
        // 失败走 "Quest buffer overflow" 报错分支，待 QuestClear 域接线。
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveDungeonClear();
    }

    if (isSaveQuest())
    {
        Stream* s = GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1e8c);
        CStreamGuard guard(s, true);
        guard << (int)0x29;              // SIG_UPDATE_QUEST
        guard << (int)GetUID();
        char* sig = (char*)guard.GetInBuffer<SIG_UPDATE_QUEST>();
        memset(sig, 0, 0x7601);
        UserQuest* questR = getCurCharacQuestR();
        WongWork::CQuestClear* qc = (WongWork::CQuestClear*)((char*)questR + 4);
        memcpy(sig + 0xa4, qc->getClearedQuest(), 30000);   // checkSaveFlag 待声明
        *(sig + 0x75d4) = 1;
        *(unsigned int*)(sig + 0) = (unsigned int)getCurCharacNo();
        for (int i = 0; i < 0x14; ++i)
        {
            *(unsigned int*)(sig + 4 + i * 4) =
                *(unsigned int*)((char*)questR + 8 + (0x1d4c + i) * 4);
            *(unsigned int*)(sig + 4 + (i + 0x14) * 4) =
                *(unsigned int*)((char*)questR + 8 + (0x1d60 + i) * 4);
        }
        *(unsigned int*)(sig + 0x75d5) = *(unsigned int*)((char*)questR + 0x75f0);
        memcpy(sig + 0x75d9, (char*)questR + 0x75f4, 0x28);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveQuest();
    }

    // 推断：CCreatureMgr::IsSaveCreatureItem/SaveCreatureItem 待 CCreatureMgr 域声明。

    short eventItemCount = GetCurCharacEventItemCount();
    if (eventItemCount != 0)
    {
        // 推断：DB_UpdateItemCountValue::makeRequest(accId, cnt) 待 DB 域接线。
        ResetCurCharacEventItemCount();
    }

    if (isSaveInformNoticeFlag())
    {
        Stream* s2 = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f2b);
        CStreamGuard guard(s2, true);
        guard << (int)0x2f5;             // InformNoticeFlagData
        guard << (int)-1;
        char* sig = (char*)guard.GetInBuffer<InformNoticeFlagData>();
        memset(sig, 0, 0x14);
        *(unsigned int*)(sig + 0) = get_acc_id();
        // 推断：getInformNoticeFlag(i) 逐位填充（getter 待 CUserCharacInfo 域声明）
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveInformNoticeFlag();
    }

    if (GetCurCharacUsedFatigueQuantity() != 0)
    {
        // 推断：DB_UpdateUserFatigueQuantity::makeRequest(accId, qty) 待 DB 域接线。
        ResetCurCharacUsedFatigueQuantity();
    }
    // 推断：GetChangedGiftFatigueQuantity → DB_UpdateUserGiftFatigueQuantity::
    // makeRequest(accId, characNo, used) → SetChangedGiftFatigueQuantity(false)，
    // 访问器待 CUserCharacInfo 域声明后接线。
    // 尾段事件脚本 0x19b OBJECT_BRING_UP 等深水区见 UpdateData.md 后段（推断，暂略）。
}

// 0x086554aa：登出主链路（UpdateLogout.md）。SIG_UPDATE_LOGOUT(0x2c, 0x8b 字节)：
// accId/characNo/gcNo/fatigue；state>1 记 playTime/IP/机器码/会员奖励疲劳并复位；
// 写登录时间戳；历史日志 Logout；DB 杂项；!param 发包(1,3)。
void CUser::UpdateLogout(bool param)
{
    UpdateCharacView();

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x2260);
    CStreamGuard guard(s, true);
    guard << (int)0x2c;
    guard << (int)GetUID();
    char* sig = (char*)guard.GetInBuffer<SIG_UPDATE_LOGOUT>();
    memset(sig, 0, 0x8b);
    *(unsigned int*)(sig + 0x00) = get_acc_id();
    *(unsigned int*)(sig + 0x08) =
        (unsigned int)G_CEnvironment()->get_gc_no();
    if (getCurCharacR() != 0)
    {
        *(unsigned int*)(sig + 0x04) = (unsigned int)getCurCharacNo();
        *(unsigned short*)(sig + 0x0c) = GetFatigue();
    }

    long now = (long)GlobalData::s_systemTime_.getCurSec();
    *(sig + 0x12) = 0;
    if (get_state() > 1)
    {
        *(sig + 0x12) = 1;
        *(int*)(sig + 0x13) =
            (int)(now - *(long*)((char*)this + 0x71ba4));   // 推断：登录时刻
        *(unsigned int*)(sig + 0x17) = *(unsigned int*)((char*)this + 0x8d10c);
        *(unsigned int*)(sig + 0x1b) = *(unsigned int*)((char*)this + 0x703a4);
        *(unsigned short*)(sig + 0x1f) = *(unsigned short*)((char*)this + 0x79642);
        *(unsigned char*)(sig + 0x21) = GetPuUser();
        SetUserManagerToolIP(this, reinterpret_cast<SIG_UPDATE_LOGOUT*>(sig));  // ORIG 0x865569e 实证传结构基址

        char mac[0x41];
        memset(mac, 0, sizeof(mac));
        memcpy(mac, m_network.get_mac_addr(), 0x41);
        memcpy(sig + 0x47, mac, 0x41);
        *(sig + 0x88) = (char)(IsEnableSaveMemberBonusFatigue() ? 1 : 0);
        DisableSaveMemberBonusFatigue();
        *(unsigned short*)(sig + 0x89) =
            (unsigned short)getAccountMemberBonusFatigue();
        resetAccountMemberBonusFatigue();
    }
    *(long*)((char*)this + 0x71ba4) = now;

    bool sendOut = get_acc_id() != 0 ||
                   G_GameWorld()->GetChannelType() == 7;
    if (sendOut)
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);

    if (m_network.get_str_ip()[0] != '\0' && getCurCharacName() != 0)
    {
        int startTick = GetCharacStartPlayTick();
        if (startTick > 0)
        {
            long cur = (long)GlobalData::s_systemTime_.getCurSec();
            m_historyLog.Logout(getWebAddress(), m_network.get_str_ip(),
                                m_network.get_port(),
                                (unsigned short)*(int*)((char*)G_CEnvironment() + 0x1b0),
                                (int)(cur - startTick), GetLogInOutState());
        }
    }

    DBUpdateDBLogItem();
    ResetDBLogItem();

    if (is_update_ontime_last_recv_idx())
    {
        // 推断：DB_UpdateOntimeLastRecvIdx::makeRequest(uid, accId, idx) 待 DB 域接线。
        reset_update_ontime_last_recv_idx();
    }
    reset_ingame_event_history();

    if (!param)
    {
        PacketGuard pkt;
        ((InterfacePacketBuf*)&pkt)->put_header(1, 3);
        ((InterfacePacketBuf*)&pkt)->put_byte(1);
        ((InterfacePacketBuf*)&pkt)->finalize(true);
        Send(pkt);
    }
}

// 0x0867e734：日删可堆叠物品（deleteDailyStackableItem.md）。区间 [a,b] 三重过滤
// （空槽/find_item/is_stackable/+0x3c5>0 日删标记）后删除并压 (space=0, slot)。
void CUser::deleteDailyStackableItem(std::vector<std::pair<int, int> >& list,
                                     int a, int b)
{
    if (getCurCharacR() == 0)
        return;
    CInventory* inven = (CInventory*)getCurCharacInvenR();
    for (int slot = a; slot <= b; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0 || !ci->is_stackable())
            continue;
        if (*(const char*)((const char*)ci + 0x3c5) <= 0)   // 推断：日删标记
            continue;
        enableSaveInven();
        inven->delete_item((INVEN_TYPE)1, slot, item.m_addInfo2,
                           (eItemDelReason)0x13, true);
        list.push_back(std::make_pair((int)ENUM_ITEMSPACE_0, slot));
    }
}

// 0x0867dba0：指定物品清理编排（deleteSpecificItem.md）：关私摊 → 五域清理 →
// 邮箱超时/匹配/无效物品移除（header 0x62 包）→ 无效物品邮件补偿 → redeem 清理。
void CUser::deleteSpecificItem(const std::vector<std::pair<int, int> >& list,
                               std::vector<std::pair<int, int> >& out)
{
    if (getCurCharacR() == 0)
        return;

    private_store::CPrivateStoreMgr* mgr =
        private_store::GetInstancePrivateStoreMgr();
    private_store::CPrivateStore* store =
        (private_store::CPrivateStore*)mgr->FindStoreSeller(this);
    if (store != 0)
    {
        typedef void (*Vfn)(void*, int);          // 推断：虚表+4 结束个人商店
        ((Vfn*)*(void**)store)[1](store, 0);
    }

    _deleteSpecificItemInven((INVEN_TYPE)0, list, out, 10, 0x15);
    _deleteSpecificItemInven((INVEN_TYPE)1, list, out, 3, 0x137);
    _deleteSpecificItemCargo(list, out);
    _deleteSpecificItemAccountCargo(list, out);
    _deleteSpecificItemTitieBook(list);

    WongWork::CMailBox* mailBox =
        *(WongWork::CMailBox**)((char*)this + 0x71b98);   // 推断：邮箱成员
    if (mailBox != 0)
    {
        std::vector<std::pair<int, int> > mailDel;
        unsigned int rmList[21];
        int rmCount = 0;
        long nowSec = (long)GlobalData::s_systemTime_.getCurSec();
        mailBox->setMailIterator();
        for (WongWork::CMailBox::CMail* mail = mailBox->getNextMail();
             mail != 0; mail = mailBox->getNextMail())
        {
            // ORIG：超时(IsTimeOver)/已匹配/无效物品 三者均移除；无效物品
            // 记入 mailDel 供 RewardItem2DeleteInvalidItem 邮件补偿。
            bool remove = false;
            if (mail->IsTimeOver(nowSec))
            {
                remove = true;
            }
            else
            {
                const Inven_Item& pkg = mail->GetPackageItem();
                if (_isMatchedItem(pkg.m_addInfo, list) == -1)
                {
                    if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                            pkg.m_addInfo))
                    {
                        mailDel.push_back(
                            std::make_pair(pkg.m_addInfo, pkg.m_addInfo2));
                        remove = true;
                    }
                }
                else
                {
                    remove = true;
                }
            }
            if (remove && rmCount < 20)
            {
                rmList[++rmCount] = mail->GetLetterId();
                mailBox->RemoveMail(mail->GetLetterId());
            }
        }
        if (rmCount != 0)
        {
            rmList[0] = (unsigned int)rmCount;
            WongWork::CMailBoxHelper::ReqDBRemoveMail(this, rmList, false);
            PacketGuard packet;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
            pkt->put_header(0, 0x62);
            pkt->put_int(rmCount);
            for (int i = 1; i <= rmCount; ++i)
                pkt->put_int((int)rmList[i]);
            pkt->finalize(true);
            Send(packet);
        }
        RewardItem2DeleteInvalidItem("Mail", mailDel);
    }
    _deleteSpecificRedeemItem(list);
}

// 0x08652c8e
// ORIG：联动角色断线处理。标志开启时：若数据加载/建角/删角被锁定则记错误日志；
// 否则把联动角色槽位移到列表尾（ChangeCharacSlot 两次）、清标志并刷新角色视图。
void CUser::doLinkCharacDisconnect()
{
    if (isLinkCharacDisconnectFlag() == 0)
        return;
    bool locked = isLocked4DataLoad() != 0 || isLockedCreateCharac() != 0 ||
                  isLockedDeleteCharac() != 0;
    if (locked)
    {
        LogManager::logFormat(1, "user.cpp",
            "void CUser::doLinkCharacDisconnect()", 0x1cc1,
            "LINK_LOG : checkLinkCharacDisconnect ERROR!! m_id(%s)",
            NumberToString(get_acc_id(), 0));
        return;
    }
    int slotIdx = (int)getDisconnectLinkCharacSlotIdx() - 1;
    int last = (int)m_characList.size() - 1;
    ChangeCharacSlot((unsigned int)slotIdx, (unsigned int)last);
    ChangeCharacSlot((unsigned int)slotIdx, (unsigned int)last);
    setLinkCharacDisconnectFlag(false);
    UpdateCharacView();
}

// 0x08651b7a
// ORIG：把 m_characList（vector<_Charac_info> @ +0x796e8）中下标 a 的元素移动到
// 下标 b 处（a<b 时先 insert(begin()+b+1, v[a]) 再 erase(begin()+a)；
// a>b 时 insert(begin()+b, v[a]) 再 erase(begin()+a+1)），随后
// enableSaveCharacView(&m_characterView)。越界（a/b >= size）直接返回。
int CUser::ChangeCharacSlot(unsigned int a, unsigned int b)
{
    if (a >= m_characList.size() || b >= m_characList.size())
        return 0;
    _Charac_info tmp = m_characList[a];
    if (a < b)
    {
        m_characList.insert(m_characList.begin() + (b + 1), tmp);
        m_characList.erase(m_characList.begin() + a);
    }
    else
    {
        m_characList.insert(m_characList.begin() + b, tmp);
        m_characList.erase(m_characList.begin() + (a + 1));
    }
    m_characterView.enableSaveCharacView();
    return 0;
}

// ORIG 0x086973dc：设置联动角色断线标志。与 isLinkCharacDisconnectFlag() 一致，
// 标志位于 m_breakAway.m_pad14[1]。
void CUser::setLinkCharacDisconnectFlag(bool flag)
{
    *(unsigned char*)((char*)&m_breakAway.m_pad14[1]) = flag ? 1 : 0;
}

// 0x08651740（TSV：地址空缺，以类报告为准）
// ORIG：GameWorld::GetChannelType()!=7 且 CCharacterView::isSaveCharacView() 时，
// 经 StreamPool 分配 Stream，写 SIG_UPDATE_CHARAC_VIEW（0x14d8 字节，首字段 acc_id、
// +0x14d4=serverGroup），逐 _Charac_info 填充 {acc_id, name, …} 各可见字段，
// 调 updateCharacViewVisibleData 后经 MsgQueueMgr::put 投递并 disableSaveCharacView()。
// 依赖 Stream/StreamPool/CStreamGuard/SIG_UPDATE_CHARAC_VIEW/updateCharacViewVisibleData
// 等未建模链路，提供可链接的签名匹配骨架（推断）。
void CUser::UpdateCharacView()
{
    m_characterView.disableSaveCharacView();
}

// 0x086786be
// ORIG：放弃惩罚通知。非轻量服、+0x711d8 标志为 0（推断：惩罚冷却标志）、
// 未受影响 premium(0xc)、等级 > 0x11、moveSpace != 1 时，
// 发包(0x21)并 setCurCharacStamina(10)。
void CUser::giveup_panalty()
{
    if (IsLightServer())
        return;
    // 推断：+0x711d8 放弃惩罚冷却标志（位于 m_clientSpec 区内，未具名）
    if (*(char*)((char*)this + 0x711d8) != 0)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    if (get_charac_level() <= 0x11)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    if (getMoveSpace() == 1)
        return;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x21);
    setCurCharacStamina((unsigned char)0xa);
    ((InterfacePacketBuf*)&packet)->put_byte(10);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x08658910
// ORIG：登出清理——清登录时间/播放经验/多开彩票失败计数、移除 GM 标记、
// 复位 uid/槽位/自增 ID/账号名、断网并整体 reset。
void CUser::log_out()
{
    *(int*)((char*)this + 0x8d10c) = 0;   // 推断：登录时间戳（pad 区内，未具名）
    resetPlayExpAdd();
    set_multiboxLotteryItemFailCnt(0);
    GlobalData::s_pGMAccounts_->removeGM(get_acc_id(), 0);
    m_field796f8 = 0;
    m_field8cfc4 = 0;
    m_field704ac = 0;
    CUserGlobalInfoHandleInstance()->reset_uniqueid_flag(m_field704a8);
    m_field704a8 = 0;
    SetSlotIDX(0);
    SetIncreID(0);
    memset(m_accName, 0, 0x15);
    m_field8cfc8 = 0;
    *((char*)this + 0x8cfdc) = 0;   // 推断：状态标志字节（m_pad8cfdc 内）
    m_network.disconnect();
    reset();
}

// 0x086487ea
void CUser::prepareDisconnect()
{
    // 0x086487ea 主分支（prepareDisconnect.md）：state<2 早退；非人证用户记
    // hack 0x3b9 并 DB 强存 SSO；安全卡/密保请求态取消认证；antibotChecker
    // vtable +0x08/+0x24 注销客户端；DBUpdateExitTime。
    if (get_state() < 2)
        return;
    if (!isHumanCertified())
    {
        setHumanCertified(true);
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x3b9, 1, 0, 0);
        *(int*)((char*)this + 0x8e0fc) += 1;   // 推断：SSO 强制保存计数
        // 推断：DB_ForceSaveSimpleSSO::makeRequest(accId, cnt) 待 DB 域接线。
    }
    // 推断：trace "[Taiwan, Antibot] Delete client. (sequence:%d, mid:%u)"
    // （CEnvironment::getProcessSequence 待声明）。
    if (GlobalData::s_antibotChecker_)
    {
        void* checker = GlobalData::s_antibotChecker_;
        void** vtbl = *(void***)checker;
        typedef void (*Fn3)(void*, int, int, unsigned int);   // vptr+0x08
        typedef void (*Fn1)(void*, unsigned int);             // vptr+0x24
        ((Fn3)vtbl[2])(checker, 0, 0, get_acc_id());   // 推断：sequence 待 CEnvironment 声明
        ((Fn1)vtbl[9])(checker, get_acc_id());
    }
    // 推断：GarenaAuthData::getTimeStamp 读对象首 4 字节（类型未建模）
    unsigned int ts = *(unsigned int*)getGarenaAuthData();   // 推断
    (void)ts;
    // 推断：DBUpdateExitTime::makeRequest(accId, uid&0xffff, ts) 待 DB 域接线。
}

// 0x0865db6c
// ORIG：按物品空间号生成并发送对应列表包：
//   0/1 → CInventory::MakeItemList(1/2)、2 → CCargo::MakeItemList、
//   7 → 生物列表 + MakeItemList(3)、0xc → CAccountCargo::SendItemList（不存在时返回 1）。
int CUser::send_itemspace(int space)
{
    PacketGuard packet;
    char ok = 0;
    switch (space)
    {
    case 0:
        ok = (char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)1, &packet);
        break;
    case 1:
        ok = (char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)2, &packet);
        break;
    case 2:
        ok = (char)getCurCharacCargoR()->MakeItemList(&packet);
        break;
    case 7:
        if (getCurCharacInvenR()->GetCreatureMgrR()->SendCreatureItemList() != (char)1)
            return 0;
        ok = (char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)3, &packet);
        break;
    case 0xc:
        if (IsExistAccountCargo() == 0)
            return 1;
        return GetAccountCargo()->SendItemList();
    default:
        return 0;
    }
    if (ok != (char)1)
        return 0;
    Send(packet);
    return 1;
}

// 0x0868c170
void CUser::send_ontime_reward_start_notify()
{
    // 0x0868c170 主分支（send_ontime_reward_start_notify.md）：事件 0x33 活跃且
    // 奖励未开始时，本地 last_recv_idx < ontime_id 则发包 header(0,0x105) 单 int
    // = 奖励结束时间 - now（秒），负数不发。事件字段经 vtable/字段读取（推断）。
    CEventBase* eventRaw = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    if (eventRaw == 0 || !eventRaw->IsEventing((CUser*)0))
        return;
    int remain = *(int*)((char*)eventRaw + 0x1c) - (int)time(0);   // 推断：+0x1c 结束时刻
    if (get_ontime_last_recv_idx() >= *(int*)((char*)eventRaw + 0x18))
        return;                                                     // 推断：+0x18 ontime_id
    if (remain < 0)
        return;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x105);
    ((InterfacePacketBuf*)&packet)->put_int(remain);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x0866c46a：技能信息包（send_skill_info.md）。header(0,0x13)；两棵树各为
// remain_sp(short)+count(byte)+N×{idx,lvl,master}（计数事后回填）；公会技能段
// （+0x44 计数、+0x45+i*5 等级、+0x49 master，5 字节步进）在 PvP 树频道清零；
// 末尾两个 sfp 余量 short；发完追加 sendComboSkillList。
static void s2_putSkillTree(InterfacePacketBuf* pkt, const char* skillBase,
                            int baseOff, int idxBase)
{
    int cntIdx = pkt->get_index();
    int written = 0;
    pkt->put_byte(0);
    for (int i = 0; i < 0xcc; ++i)
    {
        unsigned char level =
            (unsigned char)skillBase[baseOff + (idxBase + i) * 2];
        if (level == 0)
            continue;
        signed char master =
            (signed char)skillBase[baseOff + (idxBase + i) * 2 + 1];
        pkt->put_byte(i);
        pkt->put_byte((int)level);
        pkt->put_byte((int)master);
        ++written;
        pkt->put_byte(0);   // 技能命令段：findSkillCommandData 未建模，按 0 占位（推断）
    }
    pkt->put_byte(cntIdx, written);   // 回填数量
}

void CUser::send_skill_info()
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    SkillSlot* skill = (SkillSlot*)getCurCharacSkillR();
    const char* sb = (const char*)getCurCharacSkillR();

    pkt->put_header(0, 0x13);
    pkt->put_short(skill->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0));
    s2_putSkillTree(pkt, sb, 6, 0x20);          // 树0：base+6, idx+0x20

    void* guildDB = GetGuildDBInfo();
    int guildCnt = *(unsigned char*)((char*)guildDB + 0x44);
    if (G_GameWorld()->IsPvPSkilTreeChannel())
        guildCnt = 0;
    unsigned char grade = getGuildMemberGrade();
    for (int i = 0; i < guildCnt; ++i)
    {
        int glv = *(int*)((char*)guildDB + i * 5 + 0x45);
        if (grade == 0 || glv < 200)
            continue;
        pkt->put_byte(i + 0x8a);
        pkt->put_byte(glv);
        pkt->put_byte((int)*(signed char*)((char*)guildDB + i * 5 + 0x49));
    }

    pkt->put_short(skill->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1));
    s2_putSkillTree(pkt, sb, 0xe, 0xe8);        // 树1：base+0xe, idx+0xe8

    pkt->put_short(skill->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2));
    pkt->put_short(skill->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3));
    pkt->finalize(true);
    Send(packet);
    sendComboSkillList();
}

// 0x0866cb04

bool CUser::CheckQuestMonster(int a, int b, ENUM_QUEST_ENEMY_TYPE type)
{
    // 0x0866cb04 主分支（CheckQuestMonster.md）：遍历进行中任务表 this+0x7538+i*4，
    // 逐任务 CheckKillMonster 掷掉落：rate = 基础(+0x18) + 背包 add_info 加成 +
    // premium；掷 tryCount(+0x14) 次 rand(100)；入包 0xe（新槽插入 / 已有槽累加封顶）。
    // 任务表/地图 idx 等偏移按 ORIG（推断标注）。
    for (int i = 0;; ++i)
    {
        int questIdx = *(int*)((char*)this + 0x7538 + i * 4);
        if (questIdx <= 0)
            return true;
        Quest* quest = G_CDataManager()->find_quest(questIdx);
        if (quest == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                "bool CUser::CheckQuestMonster(int, int, ENUM_QUEST_ENEMY_TYPE)",
                0x54f7, "G_CDataManager()->find_quest(%d) fail", questIdx);
            continue;
        }

        std::vector<MonsterKillItem> kills;
        if (!quest->CheckKillMonster(*(short*)((char*)this + 0x75da), a, b,
                                     type, kills))
            return false;
        if (kills.empty())
            return true;

        int r = get_rand_int((int)kills.size());
        if (r < 0 || r >= (int)kills.size())
            return true;
        const MonsterKillItem& drop = kills[r];
        const int itemIdx = drop.m_field10;    // 推断：掉落道具 idx
        const int needCount = drop.m_field1c;  // 推断：需要数量
        const int dropRate = drop.m_field18;   // 推断：基础掉率
        const int tryCount = drop.m_field14;   // 推断：掷骰次数

        int rate = dropRate;
        for (int s = 0; s <= 0x19; ++s)
        {
            Inven_Item* it =
                ((CInventory*)getCurCharacInvenR())->GetInvenRef((INVEN_TYPE)0, s);
            if (it != 0 && it->m_addInfo != 0)
                rate += dropRate * it->m_addInfo / 100;
        }

        int gained = 0;
        for (int t = 0; t < tryCount; ++t)
            if (get_rand_int(100) < rate)
                ++gained;
        if (gained == 0)
            continue;

        Inven_Item item;
        CItem* ci = G_CDataManager()->find_item(itemIdx);
        if (ci == 0)
            continue;
        ci->make_item(item);
        item.set_add_info(gained > 999 ? 1000 : gained);

        PacketGuard packet;
        InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
        pkt->put_header(0, 0xe);
        pkt->put_byte(0);
        pkt->put_short(1);
        int slot = -1;
        for (int t2 = 0; t2 <= 0x19; ++t2)
        {
            Inven_Item cur = getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, t2);
            if (cur.m_addInfo == itemIdx) { slot = t2; item = cur; break; }
        }
        if (slot < 0)
        {
            int newSlot = getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)8, true, true);
            if (newSlot > 0)
            {
                pkt->put_short(newSlot);
                pkt->put_int(itemIdx);
                pkt->put_int(item.get_add_info());
                pkt->finalize(true);
                Send(packet);
            }
        }
        else
        {
            int cur = item.get_add_info();
            int nv = (needCount > 0 && cur + gained >= needCount)
                         ? needCount : cur + gained;
            item.set_add_info(nv);
            getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, item);
            pkt->put_short(slot);
            pkt->put_int(itemIdx);
            pkt->put_int(item.get_add_info());
            pkt->put_packet(item);
            pkt->finalize(true);
            Send(packet);
        }
    }
}

// 0x08655c60
void CUser::FatigueUp(int v)
{
    // 0x08655c60 主分支（FatigueUp.md）：v==0→1；Hades+GoldControl 双扣减；
    // state==5 时混沌点扣减与累计(+0x8e064)；+0x8e08c=v；prev 疲劳逢 20 上报
    // hack 0x263；0x9c/0x32/0x64 清动作组 0x16/0x28/0x2a；事件脚本 inc/gift/
    // bring_up；学园点；英雄任务 MissionNo=2；repeat 事件 0x29/0x2b；
    // AvatarRechargeServer 计耐久；+0x79642 累计；公会 temp exp。
    if (v == 0)
        v = 1;
    unsigned short prevFatigue =
        (unsigned short)getCurCharacUsedFatigue();

    // XNuclear::CHades::UseFatigue（ORIG T 0x084b9552；权威定义见 CHades.cpp）
    ((XNuclear::CHades*)getHades())->UseFatigue((unsigned int)v);
    GetGoldControl()->UseFatigue(v);

    if (get_state() == 5 && getCurCharacR() != 0)
    {
        IncCurCharacAssaultFatigueCount();
        if (GetCurCharacAssaultFatigueCount() > 1)
        {
            SubCurCharacChaosPoint(1);
            SetCurCharacAssaultFatigueCount(0);
            *(int*)((char*)this + 0x8e064) += v;   // 推断：混沌疲劳累计
        }
    }
    m_field8e08c = v;
    incCurCharacUsedFatigue((unsigned short)v);
    if ((short)(prevFatigue + (unsigned short)v) % 20 == 0)
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x263, 1, 0, 0);

    unsigned short nowFatigue = (unsigned short)getCurCharacUsedFatigue();
    int actionGroup = 0;
    if (nowFatigue == 0x9c)
        actionGroup = 0x16;
    else if (nowFatigue == 0x32)
        actionGroup = 0x28;
    else if (nowFatigue == 100)
        actionGroup = 0x2a;
    if (actionGroup != 0)
    {
        APSystem::_ActionGroupIndex ag;
        ag.m_value = actionGroup;
        APSystem::CUserProc::ClearActionAndSendtoUser(this, ag, 0,
                                                      (ENUM_PACKETCLASS)0);
    }

    // 推断：EventClassify::CEventScriptMng inc_user_fatigue/send_gift_fatigue_item/
    // send_bring_up_item_fatigue(0x19b) 待该域声明后接线。
    if (isJoinedSchoolPointEvent())
        calcCurCharacSchoolPoint(prevFatigue);

    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, (HeroMissionCondition::MissionNo::T)2, (unsigned int)v);

    // 推断：repeat 事件 0x29（角色日，按 job 匹配 GetEventJob）→
    // _processCharacterDayEvent；0x2b → _processFatigueBuffEvent；
    // 依赖未建模，待事件域批次接线。

    // ARAD::Singleton<AvatarRechargeServer>::CalcDurability(this,v) 待特化实例化（推断）
    *(short*)((char*)this + 0x79642) = *(short*)((char*)this + 0x79642) + (short)v;
    if (get_charac_level() != 0)
    {
        // 推断：GuildParameterScript::getGuildExpWeight(G_CDataManager()+0xa50c)
        // 权重 * v / 100 计入 add_guild_temp_exp。
        add_guild_temp_exp(v / 100);
    }
}

// 0x0867ba5c（SendNotiPacket.md）：header(0,cmd)；cmd==2 → byte+short(1)+
// make_basic_info；cmd==0x25 → level/exp/组队/会员 exp/SP/SFP/胜点(+0x58)/
// 疲劳/seria bonus/周末(事件 0x57 活跃才发并 reset)/premium bonus(reset)。
void CUser::SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET cmd, int param)
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(0, cmd);
    if (cmd == (ENUM_NOTIPACKET)2)
    {
        pkt->put_byte(param);
        pkt->put_short(1);
        make_basic_info((char*)&packet, (char)param);
    }
    else if (cmd == (ENUM_NOTIPACKET)0x25)
    {
        pkt->put_byte(get_charac_level());
        pkt->put_int(get_charac_exp());
        pkt->put_int(get_charac_party_bonus_exp());
        pkt->put_int(get_charac_member_bonus_exp());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)0));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)1));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)2));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)3));
        pkt->put_int(getPVPResultRefR()->m_winPoint);
        pkt->put_int(get_charac_fatigue_buf_bonus_exp());
        pkt->put_int(get_charac_seria_buf_bonus_exp());
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (ev != 0 && ev->IsEventing((CUser*)0))
        {
            pkt->put_int(getWeekendBonusExp());
            resetWeekendBonusExp();
        }
        else
        {
            pkt->put_int(0);
        }
        pkt->put_int(getPremiumBonusExp());
        resetPremiumBonusExp();
    }
    else
    {
        LogManager::logFormat(1, "user.cpp",
            "bool CUser::SendNotiPacket(CUser::eSendTarget, ENUM_NOTIPACKET, int)",
            0x76d1, "User %s - NotifyID = %d", getCurCharacName(), cmd);
        return;
    }
    pkt->finalize(true);
    SendPacket(target, packet);
}

// Packet_Notice_Guild_War_Point_Change 最小镜像（ORIG ctor 0x086944e0 W）
class Packet_Notice_Guild_War_Point_Change
{
public:
    Packet_Notice_Guild_War_Point_Change() {}
    char m_buf[0xa];   // 推断：guildkey(int) + point(byte) + 头
};

// 0x0865c936
// ORIG：公会战积分累加。PvP 房间内且非观察者、公会战事件(0x21)进行中且
// 有公会时：通知公会服 + 发包(0x6e) + 累加 +0x8cfe0（m_guildWarPoint）。
void CUser::add_guild_pvp_result(int v)
{
    PvP_Room* pRoom = (PvP_Room*)GetPVPRoom();
    if (pRoom == 0)
        return;
    int seat = pRoom->get_user_seat(this);
    if (pRoom->IsPvpObserver(seat) != 0)
        return;
    CEventBase* pEvent = GlobalData::s_event_manager->GetRepeatEvent(0x21);
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0) != 0 &&
        get_charac_guildkey() != 0)
    {
        Packet_Notice_Guild_War_Point_Change notice;
        // 推断：+0 处 guildkey、+4 处点数（ORIG 手动填充后 SendPacket(0xf)）
        *(unsigned int*)((char*)&notice + 0) = (unsigned int)get_charac_guildkey();
        *(char*)((char*)&notice + 4) = (char)v;
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket((char*)&notice, 0xf);
        send_pvp_record();
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x6e);
        ((InterfacePacketBuf*)&packet)->put_byte(v);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
        m_guildWarPoint = m_guildWarPoint + v;
    }
}

// 0x0865d986
// ORIG：PVP 每日/每周游玩次数累计。PvpResultType 布局（CUserCharacInfo.h）：
//   +0x60 当日游玩次数（隔天重置为 0）、+0x64 累计游玩次数、
//   +0x68 当日基准时间戳（0 点 + 0x15180）。字段未具名，按 ORIG 偏移访问（推断）。
void CUser::add_pvp_play_info(unsigned int a, unsigned int b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    if (*(int*)((char*)getPVPResultRefR() + 0x68) + 0x15180 < (int)now)
    {
        *(int*)((char*)getPVPResultRefW() + 0x60) = 0;
        tm tmv;
        localtime_r(&now, &tmv);
        tmv.tm_hour = 0;
        tmv.tm_min = 0;
        tmv.tm_sec = 0;
        *(time_t*)((char*)getPVPResultRefW() + 0x68) = mktime(&tmv) + 0x15180;
    }
    *(unsigned int*)((char*)getPVPResultRefW() + 0x60) += a;
    *(unsigned int*)((char*)getPVPResultRefW() + 0x64) += b;
}

// 0x0865c678
// ORIG：PvP 结果结算。观战者直接返回；winPointBuffer(+0x796fc) 清零并入账；
// 胜/败计数（整合 PvP 基础频道且渠道类型==0 时不计）；额外胜点补发并记 WPAdd 日志；
// 战斗模式 4/5 且 out 非空时按 mob_reward×胜点增量 + gainPowerWarRewardExp 计算奖励。
void CUser::add_pvp_result(bool flag, unsigned int* out)
{
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return;
    lock();
    const PvpResultType* before = getPVPResultR();
    int beforeWinPoint = before->m_winPoint;
    int delta = m_winPointBuffer;
    m_winPointBuffer = 0;
    if (!flag)
    {
        // ORIG：整合 PvP 基础频道且渠道类型 0 → 不计败场。
        if (!(G_GameWorld()->IsIntegratedPvPBaseChannel() &&
              G_CDataManager()->GetPvPChannelType() == 0))
        {
            getPVPResultW()->m_totalLoseCount += 1;
        }
    }
    else
    {
        delta = delta + 1;
        if (!(G_GameWorld()->IsIntegratedPvPBaseChannel() &&
              G_CDataManager()->GetPvPChannelType() == 0))
        {
            getPVPResultW()->m_totalWinCount += 1;
        }
    }
    getPVPResultW()->m_winPoint += delta;
    m_historyLog.WPAdd(getPVPResultR()->m_winPoint, delta, eWPAddReason_0);
    int extra =
        room->get_extra_win_point(getPVPResultW()->m_winPoint - beforeWinPoint);
    if (extra != 0)
    {
        getPVPResultW()->m_winPoint += extra;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, extra, eWPAddReason_0);
    }
    unlock();
    int battleMode = room->get_pvp_battle_mode();
    bool rewardMode = (battleMode == 4) || (room->get_pvp_battle_mode() == 5);
    if (rewardMode && out != 0)
    {
        unsigned int mobReward = 0;
        int level = get_charac_level();
        G_CDataManager()->get_mob_reward(level, mobReward);
        *out = (unsigned int)((getPVPResultR()->m_winPoint - beforeWinPoint) *
                              (int)mobReward);
        *out = *out + (unsigned int)gainPowerWarRewardExp((int)*out);
    }
    send_pvp_record();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
}

// 0x0864fac4
// ORIG：公会技能已学查询。GetGuildDBInfo 内 +0x44 成员数、+0x45 起
// 每 5 字节 {skillIdx(int), level(byte)}，返回匹配项的 level 字节。
bool CUser::IsGuildSkillLearn(int skillIdx)
{
    void* gdb = GetGuildDBInfo();
    if (gdb == 0)
        return false;
    unsigned char count = *(unsigned char*)((char*)gdb + 0x44);
    for (int i = 0; i < (int)count; ++i)
    {
        if (*(int*)((char*)gdb + i * 5 + 0x45) == skillIdx)
            return *(unsigned char*)((char*)gdb + i * 5 + 0x49) != 0;
    }
    return false;
}

// 0x0864fb3a
// ORIG：公会技能经验换算。无公会/非公会成员/未学技能 0xc9 → 0；
// 否则按职业 find_skill 返回的 vector<vector<int>>（+0x108）取
// 公会等级-1 的权重，返回 param_1 * 权重 / 100。
int CUser::gainGuildSkillExp(int exp)
{
    if (get_charac_guildkey() == 0)
        return 0;
    if (getGuildMemberGrade() == 0)
        return 0;
    if (IsGuildSkillLearn(0xc9) == 0)
        return 0;
    CSkill* skill = G_CDataManager()->find_skill(get_charac_job(), 0);
    if (skill == 0)
        return 0;
    // 推断：find_skill 返回 CSkill 族对象，+0x108 为 vector<vector<int>> 权重表
    std::vector<std::vector<int> >* gradeWeights =
        (std::vector<std::vector<int> >*)((char*)skill + 0x108);
    int gradeIdx = (int)getGuildMemberGrade() - 1;
    if (gradeIdx < 0 || gradeIdx >= (int)gradeWeights->size())
        return 0;
    std::vector<int>& w = (*gradeWeights)[gradeIdx];
    if (w.empty())
        return 0;
    return (exp * w[0]) / 100;
}

// ---- 0x08659faa make_charac_stat(PacketGuard&)：角色属性块序列化。
// 报告：docs/class_func_reports/CUser/make_charac_stat.md（汇编 0x8659faa..0x865a44d
// + Ghidra 反编译 C）。三通路，各尾部统一 put_int(0x53)/put_binary(0x52)/put_byte：
//   C) GameWorld::IsCharacterStatRevisionChannel()（W 0x86977da）为真（ORIG
//      落入块）→ AddInfo 副本被 FairPvPStatScript::FindCharacStat（T 0x8693c66，
//      G_CDataManager()+0x4df8 即 m_fairPvPStat）按 (job, firstGrowType) 查得的
//      CharacterStatus（0x54）非零字段覆盖：头 2 int + 4 u16、循环1 u16
//      [0x10..0x16]、循环2 u16[0x18..0x38]、尾段簇（记录尾段比 _Additioal_info
//      整体错后 2 字节：0x3c↔0x3a … 0x50↔0x4e）；
//   B) 否则有 VIP 特权 → AddInfo 副本 += WongWork::CUserPremium::GetAdditionalInfo
//      （W 0x8694a88，sret 临时）经 _Additioal_info::operator+=（W 0x8693cea）；
//   A) 否则 → 直接输出 getCurCharacAddInfoR() 原块（无副本）。
// 副本经拷贝初始化生成 ORIG 同款内联逐块复制（无 memset 构造）。
void CUser::make_charac_stat(PacketGuard& packet)
{
    if (G_GameWorld()->IsCharacterStatRevisionChannel())
    {
        // 通路 C：FairPvP 表非零字段覆盖副本。
        _Additioal_info addInfo =
            *static_cast<const _Additioal_info*>(getCurCharacAddInfoR());
        int growType = (int)getCurCharFirstGrowType();
        int job = get_charac_job();
        CharacterStatus* stat =
            G_CDataManager()->m_fairPvPStat.FindCharacStat(job, growType);
        if (stat != 0)
        {
            if (stat->m_field00 != 0)
                addInfo.m_field00 = stat->m_field00;
            if (stat->m_field04 != 0)
                addInfo.m_field04 = stat->m_field04;
            if (stat->m_field08[0] != 0)
                addInfo.m_field08 = stat->m_field08[0];
            if (stat->m_field08[1] != 0)
                addInfo.m_field0a = stat->m_field08[1];
            if (stat->m_field08[2] != 0)
                addInfo.m_field0c = stat->m_field08[2];
            if (stat->m_field08[3] != 0)
                addInfo.m_field0e = stat->m_field08[3];
            for (int i = 0; i <= 3; ++i)   // ORIG 循环1：+0x10..+0x16
            {
                if (stat->m_field08[i + 4] != 0)
                    addInfo.m_field10[i] = stat->m_field08[i + 4];
            }
            for (int j = 0; j <= 0x10; ++j)   // ORIG 循环2：+0x18..+0x38
            {
                if (stat->m_field08[j + 8] != 0)
                    addInfo.m_field18[j] = stat->m_field08[j + 8];
            }
            if (stat->m_field3c != 0)
                addInfo.m_field3a = stat->m_field3c;
            if (stat->m_field40 != 0)
                addInfo.m_field3e = stat->m_field40;
            if (stat->m_field42 != 0)
                addInfo.m_field40 = stat->m_field42;
            if (stat->m_field44 != 0)
                addInfo.m_field42 = stat->m_field44;
            if (stat->m_field48 != 0)
                addInfo.m_field46 = stat->m_field48;
            if (stat->m_field4a != 0)
                addInfo.m_field48 = stat->m_field4a;
            if (stat->m_field4c != 0)
                addInfo.m_field4a = stat->m_field4c;
            if (stat->m_field4e != 0)
                addInfo.m_field4c = stat->m_field4e;
            if (stat->m_field50 != 0)
                addInfo.m_field4e = stat->m_field50;
        }
        packet.put_int(0x53);
        packet.put_binary(reinterpret_cast<const char*>(&addInfo), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
    else if (IsHavePremiumAdvantage())
    {
        // 通路 B：AddInfo 副本叠加 VIP 附加属性。
        _Additioal_info addInfo =
            *static_cast<const _Additioal_info*>(getCurCharacAddInfoR());
        addInfo += m_premium.GetAdditionalInfo();
        packet.put_int(0x53);
        packet.put_binary(reinterpret_cast<const char*>(&addInfo), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
    else
    {
        // 通路 A：put_int(0x53) → put_binary(AddInfo 原指针, 0x52)。
        packet.put_int(0x53);
        packet.put_binary(
            static_cast<const char*>(getCurCharacAddInfoR()), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
}

// ---- 0x0869729a incPlayExpAdd：m_gameOption.m_playExpAdd += v（ORIG W 内联）----
void CUser::incPlayExpAdd(int v)
{
    m_gameOption.m_playExpAdd += v;
}

// 0x0866a9a0 gain_sp：SP 双树（0/1）clamp 0x7fffffff 后累加。
void CUser::gain_sp(int v)
{
    for (int kind = 0; kind < 2; ++kind)
    {
        SkillSlot* slotR = (SkillSlot*)getCurCharacSkillR();
        int remain =
            slotR->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)kind);
        SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
        if (0x7fffffff - v < remain)
            slotW->set_remain_sp_at_index(0x7fffffff,
                                          (ENUM_SKILL_TREE_KIND)kind);
        else
            slotW->set_remain_sp_at_index(remain + v,
                                          (ENUM_SKILL_TREE_KIND)kind);
    }
}

// 0x0866aad2 gain_sfp：SFP 双树（2/3），v==0 直接返回，clamp 0x7fffffff。
void CUser::gain_sfp(int v)
{
    if (v == 0)
        return;
    for (int kind = 2; kind < 4; ++kind)
    {
        SkillSlot* slotR = (SkillSlot*)getCurCharacSkillR();
        int remain =
            slotR->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)kind);
        SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
        if (0x7fffffff - v < remain)
            slotW->set_remain_sfp_at_index(0x7fffffff,
                                           (ENUM_SKILL_TREE_KIND)kind);
        else
            slotW->set_remain_sfp_at_index(remain + v,
                                           (ENUM_SKILL_TREE_KIND)kind);
    }
}

// 0x0866ac0e history_log_sp：before==-1 或当前值变化时记 SPAdd(tree 0)。
void CUser::history_log_sp(int before, int delta, eSPAddReason reason)
{
    SkillSlot* slotR = (SkillSlot*)getCurCharacSkillR();
    int remain = slotR->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
    if (before == -1 || remain != before)
        m_historyLog.SPAdd(0, remain, delta, reason);
}

// 0x0866acd0 history_log_sfp：同上（tree 2 / SFPAdd）。
void CUser::history_log_sfp(int before, int delta, eSPAddReason reason)
{
    SkillSlot* slotR = (SkillSlot*)getCurCharacSkillR();
    int remain = slotR->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2);
    if (before == -1 || remain != before)
        m_historyLog.SFPAdd(2, remain, delta, reason);
}

// 0x866273c Remove_Mission：转发 charac_expand 槽 8 的任务列表。
void CUser::Remove_Mission(short kind)
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    list->Remove_Mission((int)kind);
}

// 0x0866a3fe
bool CUser::gain_exp_sp(int exp, int& sp, int& sfp, eExpAddReason reason,
                        int param, bool flag)
{
    return false;
}

// 0x0868e9c0
bool CUser::isCompetitionMercenary() const
{
    return false;
}

// 0x0865cfd8
// ORIG：PvP 积分增减。负值：事件(0x36)进行且 grade∈[10,19] 半额（0.5f）→ 0.9f →
// GetPvPPenaltyRevision 三级衰减；整合 PvP 基础渠道 ×70%；0xf 频道超档按
// GetPvPChannelGradePenalty 衰减；任务内限制升降档；重算 grade 并在变更时
// 重发任务信息包并重建 0x1e 任务列表。
int CUser::update_pvp_point(int v)
{
    if (v == 0)
        return 1;
    if (v < 0)
    {
        if (v < -100000 || v > 100000)
        {
            cMyTrace trace("bool CUser::update_pvp_point(int)", 0x35d6, 5);
            trace("pvp_exp_error5 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
        if (getPVPResultRefR()->m_pvpGrade < 10)
            return 1;
        CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x36);
        if (event != 0 && event->IsEventing((CUser*)0))
        {
            int grade = getPVPResultRefR()->m_pvpGrade;
            if (grade >= 10 && grade <= 0x13)
                v = (int)(0.5f * (float)v);
        }
        v = (int)(0.9f * (float)v);
        float revision =
            G_CDataManager()->m_serverParameter.GetPvPPenaltyRevision(
                getPVPResultRefR()->m_pvpGrade);
        v = (int)(revision * (float)v);
        if (v < -100000 || v > 100000)
        {
            cMyTrace trace("bool CUser::update_pvp_point(int)", 0x35ed, 5);
            trace("pvp_exp_error6 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
    }
    // 整合 PvP 基础渠道且渠道类型 0 → 打七折。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetPvPChannelType() == 0)
    {
        v = (v * 70) / 100;
    }
    // 0xf 频道正向积分超过当前渠道最高档 → 按档位差衰减。
    if (G_GameWorld()->GetChannelType() == 0xf && v > 0)
    {
        int diff = getPVPResultRefR()->m_pvpGrade -
                   G_CDataManager()->GetMaxGradePvPChannel();
        if (diff > 0)
        {
            int penalty =
                G_CDataManager()->m_serverParameter.GetPvPChannelGradePenalty(
                    diff);
            v = (v * penalty) / 100;
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace trace("bool CUser::update_pvp_point(int)", 0x3607, 5);
        trace("pvp_exp_error7 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return 1;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return 1;
    bool inMission = has_within_Mission();
    if (!inMission)
    {
        if (G_GameWorld()->GetChannelType() == 0xf &&
            acceptable_within_mission() && v > 0)
            v = 0;
    }
    else
    {
        PacketGuard packet;
        if (v < 1)
        {
            if (v < 0)
            {
                Remove_Mission((short)0x1b);
                Remove_Mission((short)7);
                ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
                ((InterfacePacketBuf*)&packet)->put_byte(1);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                Send(packet);
            }
        }
        else if (G_GameWorld()->GetChannelType() == 0xf)
        {
            v = 0;
        }
        else
        {
            Remove_Mission((short)0x1b);
            Remove_Mission((short)7);
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace trace("bool CUser::update_pvp_point(int)", 0x3653, 5);
        trace("pvp_exp_error8 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    getPVPResultRefW()->m_expPoint += v;
    if (getPVPResultRefR()->m_expPoint < 0)
    {
        LogManager::logFormat(1, "user.cpp", "bool CUser::update_pvp_point(int)",
                              0x365c, "PVP correct now<%d> point<%d>",
                              getPVPResultRefR()->m_expPoint, v);
        getPVPResultRefW()->m_expPoint = 0;
    }
    if (getPVPResultRefR()->m_pvpGrade >= 0x1e)
        return 1;
    if (!has_within_Mission() && G_GameWorld()->GetChannelType() == 0xf)
        acceptable_within_mission();
    int newGrade =
        GlobalData::s_ref_pvp_grade->GetPvpGrade(
            getPVPResultRefR()->m_expPoint);
    if (newGrade < 1)
    {
        LogManager::logFormat(1, "user.cpp", "bool CUser::update_pvp_point(int)",
                              0x36bc, "PVP Grade Incorrect %d",
                              getPVPResultRefR()->m_expPoint);
        return 1;
    }
    if (newGrade < getPVPResultRefR()->m_pvpGrade)
    {
        // 降档保护：grade∈[10,15] 时按当前档下限回填积分。
        int grade = getPVPResultRefR()->m_pvpGrade;
        if (grade >= 10 && grade <= 0xf)
        {
            getPVPResultRefW()->m_expPoint =
                GlobalData::s_ref_pvp_grade->GetPvpPoint(grade) + 1;
            newGrade = getPVPResultRefR()->m_pvpGrade;
        }
    }
    int oldGrade = getPVPResultRefR()->m_pvpGrade;
    getPVPResultRefW()->m_pvpGrade = newGrade;
    if (has_within_Mission() && getPVPResultRefR()->m_pvpGrade > oldGrade)
        getPVPResultRefW()->m_pvpGrade = oldGrade;   // 任务中禁止升档
    if (getPVPResultRefR()->m_pvpGrade != oldGrade)
    {
        PacketGuard packet;
        getCurCharacQuestR()->get_quest_info(
            reinterpret_cast<char*>(&packet));
        if (G_GameWorld()->GetChannelType() != 0xf)
        {
            Remove_Mission((short)0x1e);
            CMissionList_Charac* list = (CMissionList_Charac*)GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)8);
            list->MakeMissionList_JustKind(*this, 0x1e);
        }
        Send(packet);
    }
    return 1;
}
// 0x08657450
// ORIG：每日重置——副本金币数据、安全卡/密保取消次数（非 0 时归零并回写 DB）、
// NPC 好感度、ActionPoint（APSystem）落库并结算今日行动/勋章奖励。
void CUser::resetDailyData()
{
    getDungeonGainedGold()->reset();
    if (getSecurityCard()->getCancelCnt() != 0)
    {
        getSecurityCard()->resetCancelCnt();
        DB_SecurityCardUpdateCancelCnt::makeRequest(get_acc_id(), 0);
    }
    if (getPad()->getCancelCnt() > 0)
    {
        getPad()->setCancelCnt(0);
        DB_PassPadUpdateCancelCnt::makeRequest(get_acc_id(), 0, getWebAddress());
    }
    resetNPCRelationShipDailyData();
    // 推断：GetActionPoint() 返回内部 _SIG_LOAD_ACTION_POINT 存储体（对象首地址）；
    // 按 const& 签名解引用传递以匹配 ORIG 符号。
    APSystem::DB_UpdateActionPoint::makeRequest(GetUID(),
        *(const APSystem::_SIG_LOAD_ACTION_POINT*)&m_actionPoint, true);
    APSystem::CUserProc::SetTodayActionAndCheckMedalReward(this);
}

// 0x08657f10
// ORIG：每日币恢复。getCurCharacR() 为空直接返回；若每日币事件(4)进行中，
// 按角色等级取当日币数并读背包币；premium 有优势时加 premium 币加成；
// 最后按背包币是否足够（coin < need）设置 SetOpencoin(0/1)。
void CUser::RecoverCoin(unsigned int v)
{
    if (getCurCharacR() == 0)
        return;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    bool eventOn = (pEvent != 0) && (pEvent->IsEventing((CUser*)0) != 0);
    if (eventOn)
    {
        v = (unsigned int)pEvent->GetCoinNoPerDay(get_charac_level());
        getCurCharacInvenR()->GetCoin();
    }
    if (IsHavePremiumAdvantage())
    {
        v = v + (unsigned int)m_premium.GetAdvantageCoin();
        getCurCharacInvenR()->GetCoin();
    }
    if (getCurCharacInvenR()->GetCoin() < v)
        SetOpencoin((char)0);
    else
        SetOpencoin((char)1);
}

// 0x086568fc
// ORIG：开放标志包(0xb1)。角色存在时：每日币事件(4)进行中取等级 1 的当日币数，
// premium 有优势加币加成；计算缺口（need - coin）并在 opencoin 标志下发包。
void CUser::SendOpenflag()
{
    if (getCurCharacR() == 0)
        return;
    unsigned int coinNeed = 0;
    int lack = 0;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    bool eventOn = (pEvent != 0) && (pEvent->IsEventing((CUser*)0) != 0);
    if (eventOn)
        coinNeed = (unsigned int)pEvent->GetCoinNoPerDay(1);
    if (IsHavePremiumAdvantage())
        coinNeed = coinNeed + (unsigned int)m_premium.GetAdvantageCoin();
    unsigned int coin = getCurCharacInvenR()->GetCoin();
    bool enough = !(coin < coinNeed && GetOpencoin() != (char)1);
    if (enough)
    {
        lack = 0;
    }
    else
    {
        lack = (int)(coinNeed - getCurCharacInvenR()->GetCoin());
    }
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb1);
    ((InterfacePacketBuf*)&packet)->put_byte((int)GetOpencoin());
    ((InterfacePacketBuf*)&packet)->put_int(lack);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x086931c4
// ORIG：无效物品删除补偿。逐条记 DeleteInvaildItem 日志并按脚本换算奖励道具；
// 邮件发件人/正文按脚本字符串表（表 4）翻译，空则用 defalt_string；
// 每 10 件一批 ReqDBSendNewSystemMultiMail，余件单独一封。
void CUser::RewardItem2DeleteInvalidItem(
    const std::string& name, const std::vector<std::pair<int, int> >& list)
{
    static const char* defalt_string = "";   // ORIG 局部静态 defalt_string(0x93ace50)

    if (list.empty() || getCurCharacR() == 0)
        return;

    std::vector<Inven_Item> items;
    for (size_t i = 0; i < list.size(); ++i)
    {
        m_historyLog.DeleteInvaildItem(name, list[i].first, list[i].second);
        CItem* itemDef = G_CDataManager()->find_item(list[i].first);
        if (itemDef == 0)
            continue;
        int rewardIdx = 0;
        int rewardCnt = 0;
        if (G_CDataManager()->m_deleteInvalidItem.GetRewardItemInfo(
                list[i].first, rewardIdx, rewardCnt) &&
            rewardCnt > 0)
        {
            CItem* rewardDef = G_CDataManager()->find_item(rewardIdx);
            if (rewardDef != 0)
            {
                Inven_Item add;
                add.m_addInfo = rewardIdx;   // +0x02 先写奖励物品索引
                rewardDef->make_item(add);   // ORIG vtable 槽 2 填充默认字段
                if (rewardDef->is_stackable())
                    add.m_addInfo2 = rewardCnt;   // +0x07 堆叠数量
                items.push_back(add);
            }
        }
    }
    if (items.empty())
        return;

    std::string str0;
    std::string str1;
    char sender[0x1e];
    memset(sender, 0, sizeof(sender));
    char body[0x100];
    memset(body, 0, sizeof(body));
    G_CDataManager()->m_deleteInvalidItem.GetMailInfo(str0, str1);
    const char* s0 =
        str0.empty() ? defalt_string
                     : g_scriptStringManager_.findString(4, toTString(str0), 0);
    strncpy(sender, s0, 0x1d);
    const char* s1 =
        str1.empty() ? defalt_string
                     : g_scriptStringManager_.findString(4, toTString(str1), 0);
    strncpy(body, s1, 0xff);

    unsigned int count = 0;
    Inven_Item batch[10];
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        batch[count].setCopy(items[i]);
        count++;
        if (count == 10)
        {
            count = 0;
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                sender, batch, 10u, 0u, (unsigned int)getCurCharacNo(), body,
                (int)strlen(body), 0u, GetServerGroup(), false);
        }
    }
    if (count != 0)
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, batch, count, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false);
    }
}

// 0x08656caa
// ORIG：每日登录物品补给。等级>0x13 时道具 0x1057 补至 3 个（无则插入、有则
// 堆叠补满并记 ItemAdd 日志）；随后 ResetItemByScript/RefillItemByScript/
// deleteSpecificItem 脚本链并回发受影响槽位包；祭坛免费票补满；截止时间前
// 发放道具 0x1cab（背包满走 game_server_msg_24/25 系统邮件）。
void CUser::AddDailyItem()
{
    int level = get_charac_level();
    if (level > 0x13)
    {
        CItem* def = G_CDataManager()->find_item(0x1057);
        if (def != 0)
        {
            Inven_Item tmp;
            int slot = getCurCharacInvenR()->GetInvenData(0x1057, tmp);
            if (slot < 0)
            {
                def->make_item(tmp);
                tmp.m_addInfo = 0x1057;
                tmp.m_addInfo2 = 3;   // 数量
                slot = getCurCharacInvenW()->insertItemIntoInventory(
                    tmp, eItemAddReason_0xd, true, false);
            }
            else if (tmp.m_addInfo2 < 3)
            {
                m_historyLog.ItemAdd((INVEN_TYPE)1, 3 - tmp.m_addInfo2,
                                     tmp.get_add_info(), tmp,
                                     (eItemAddReason)0xd);
                tmp.m_addInfo2 = 3;
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, tmp);
            }
            if (slot >= 0)
                SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }
    std::vector<std::pair<int, int> > resetList;
    std::vector<std::pair<int, int> > delList;
    ResetItemByScript(resetList);
    RefillItemByScript(resetList);
    deleteSpecificItem(resetList, delList);
    for (std::vector<std::pair<int, int> >::const_iterator it = delList.begin();
         it != delList.end(); ++it)
    {
        if (it->first >= 0 && it->first <= 0x1b && it->second >= 0)
            SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)it->first,
                               it->second);
    }

    if (GlobalData::s_systemTime_.getCurSec() < (time_t)0x4811f1f0)
    {
        CItem* def = G_CDataManager()->find_item(0x1cab);
        if (def != 0)
        {
            Inven_Item tmp;
            def->make_item(tmp);
            tmp.m_addInfo = 0x1cab;
            tmp.m_addInfo2 = 1;
            int slot = getCurCharacInvenW()->insertItemIntoInventory(
                tmp, eItemAddReason_0xd, true, true);
            if (slot == -1)
            {
                char sender[0x15];
                strncpy(sender,
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_24", 0),
                        0x14);
                char body[0x100];
                strncpy(body,
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_25", 0),
                        0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    sender, tmp, 0u, (unsigned int)getCurCharacNo(), body,
                    (int)strlen(body), 0u, GetServerGroup(), false, false);
            }
            else
            {
                PacketGuard packet;
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(1);
                bool ok = getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)1, slot, packet);
                if (ok)
                {
                    ((InterfacePacketBuf*)&packet)->finalize(true);
                    Send(packet);
                }
            }
        }
    }
}

// 0x0868aaea
void CUser::SendOneADayItemShopIndex(
    const std::vector<std::pair<int, int> >& list)
{
}

// 0x0867c2d8
// ORIG：单槽位物品更新包(0xe)。GetInvenRef 空 → return 0；否则写槽头 +
// 物品字段（addInfo2 为 0 时首 int 写 -1）、强化选项、avatar 宝石槽 0x1e 字节
// 与扩展信息 4 字节、生物装备剩余天数（type==5），追加三转编年史与整件包体。
int CUser::SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot)
{
    int invenType = GetInvenTypeFromItemSpace(space);
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(invenType, slot);
    if (item == 0)
        return 0;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    int uid = item->m_addInfo2;   // +0x07
    if (uid == 0)
        ((InterfacePacketBuf*)&packet)->put_int(-1);
    else
        ((InterfacePacketBuf*)&packet)->put_int(item->m_addInfo);   // +0x02
    ((InterfacePacketBuf*)&packet)->put_int(uid);
    ((InterfacePacketBuf*)&packet)->put_byte((int)item->GetItemAttr());
    ((InterfacePacketBuf*)&packet)->put_short((int)item->m_fieldb);
    ((InterfacePacketBuf*)&packet)->put_byte((int)(unsigned char)item->m_field0);
    ((InterfacePacketBuf*)&packet)->put_int(item->m_fieldd);
    ((InterfacePacketBuf*)&packet)->put_byte(
        (int)item->m_amp.getAbilityType());
    ((InterfacePacketBuf*)&packet)->put_short(
        (int)item->m_amp.getAbilityValue());
    if (item->isAvatarItemType())
    {
        ((InterfacePacketBuf*)&packet)->put_int(0x1e);
        const WongWork::CAvatarItemMgr* mgr = getCurCharacInvenR()
                                                  ->GetAvatarItemMgrR();
        const char* socketData = mgr->getJewelSocketData(item->get_add_info());
        ((InterfacePacketBuf*)&packet)->put_binary(socketData, 0x1e);
        char* expansion =
            (char*)mgr->GetExpansionInfo(item->get_add_info());
        ((InterfacePacketBuf*)&packet)->put_int(4);
        if (expansion == 0)
        {
            stAvatarExpansionInfo_t init4;
            init4.init();
            ((InterfacePacketBuf*)&packet)->put_binary((char*)&init4, 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_binary(expansion, 4);
        }
    }
    if (item->m_field1 == (unsigned char)5)
    {
        int tick = 0;
        tick = OS_API::GetDateTimeTick();
        int remainDate = getCurCharacInvenR()->GetCreatureMgrR()->GetRemainDate(
            item->get_add_info(), tick);
        ((InterfacePacketBuf*)&packet)->put_int(remainDate);
    }
    G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(this, *item,
                                                            &packet);
    ((InterfacePacketBuf*)&packet)->put_packet(*item);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return 1;
}

// 0x0868de0a
// ORIG：光环 avatar 选项变更 -> DB_UpdateAuraAvatarOption::makeRequest(uid, characNo, idx, value)
void CUser::UpdateAuraAvatarOption(int a, int b)
{
    if (a < 0 || a > 2)
        return;
    DB_UpdateAuraAvatarOption::makeRequest(GetUID(), getCurCharacNo(), a, b);
}

// 0x0868dff8
// ORIG：光环 avatar 装备检查。无角色 → 1；槽 9 无物品/不可装备 → 0x11；
// 物品为光环 avatar 选项 → 回写选项值并返回 0，否则 0x17。
char CUser::is_equip_aura_avatar(char slot, int& out)
{
    if (getCurCharacR() == 0)
        return 1;
    Inven_Item* item = getCurCharacInvenW()->GetInvenRef(0, 9);
    if (item == 0)
        return 0x11;
    if (item->isEmpty() != 0)
        return 0x11;
    if (item->isEquipableItemType() != (char)1)
        return 0x11;
    int itemIdx = item->m_addInfo;   // +0x02 物品索引
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(itemIdx);
    if (equip == 0)
        return 0x11;
    if (equip->is_aura_avatar_option(slot) != (char)1)
        return 0x17;
    out = equip->get_aura_avatar_option_value();
    return 0;
}

// 0x0867e092
// ORIG：每日道具过期删除（idx 区间 [3,8] 与 [0x39,0x68]）后聚合发送：29 个 0xe 包
// 按物品空间预置包头并记录计数字段位置；cargo/账号金库直接 MakeItemPacket，
// 背包空间空槽写固定"已删"载荷；最后按非零计数回填数量并逐包下发。
void CUser::processDelDailyItem()
{
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）

    std::vector<std::pair<int, int> > list;
    deleteDailyStackableItem(list, 3, 8);
    deleteDailyStackableItem(list, 0x39, 0x68);
    if (list.empty())
        return;

    PacketGuard pkts[29];
    int counts[29];
    int indices[29];
    memset(counts, 0, sizeof(counts));
    for (int i = 0; i < 0x1d; ++i)
    {
        ((InterfacePacketBuf*)&pkts[i])->put_header(0, 0xe);
        ((InterfacePacketBuf*)&pkts[i])->put_byte(i);
        indices[i] = ((InterfacePacketBuf*)&pkts[i])->get_index();
        ((InterfacePacketBuf*)&pkts[i])->put_short(0);
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = list.begin();
         it != list.end(); ++it)
    {
        const int space = it->first;
        const int slot = it->second;
        if (space == 2)
        {
            getCurCharacCargoR()->MakeItemPacket(&pkts[space], slot);
            counts[space]++;
        }
        else if (space == 0xc)
        {
            if (IsExistAccountCargo())
            {
                GetAccountCargo()->MakeItemPacket(&pkts[space], slot);
                counts[space]++;
            }
        }
        else
        {
            int invenType = 1;
            if (space == 3)
                invenType = 0;
            else if (space == 7)
                invenType = 3;
            else if (space == 1)
                invenType = 2;
            Inven_Item slotItem =
                getCurCharacInvenR()->GetInvenSlot(invenType, slot);
            if (slotItem.m_addInfo == 0)   // ORIG：test dword [ret+2]
            {
                InterfacePacketBuf* pkt = (InterfacePacketBuf*)&pkts[space];
                pkt->put_short(slot);
                pkt->put_int(-1);
                pkt->put_int(0);
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_int(0);
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_packet(g_emptySlot);
            }
            counts[space]++;
        }
    }
    for (int i = 0; i < 0x1d; ++i)
    {
        if (counts[i] != 0)
        {
            ((InterfacePacketBuf*)&pkts[i])->put_short(indices[i], counts[i]);
            ((InterfacePacketBuf*)&pkts[i])->finalize(true);
            Send(pkts[i]);
        }
    }
}

// 0x086802b8
void CUser::reqHumanCertify4ClearMap(bool flag)
{
}

// （ORIG 末尾区；GameStubs 原定义）
void CUser::deleteSpecificItems(const std::vector<std::pair<int, int> >* list)
{
}

// ---- CUser::master_new_skills @ 0x0866b53e ----
// 批量习得技能：按 _Mastered_skill 数组计算 SP/SFP 用量并扣减对应技能树余量，
// 将技能槽写入（set_skill_slot_at_index），同时记录 SP/SFP 消耗日志。
// 依据 docs/class_func_reports/CUser/master_new_skills.md 反编译；
// 成长型 give-skill 展开部分（CCharacter 布局 + G_CDataManager()+0x14）待
// CCharacter 布局批次落地后按报告补全，此处置 PvP 技能树通路返回 true。
bool CUser::master_new_skills(_Mastered_skill* skills, ENUM_SKILL_TREE_KIND kind)
{
    int usedSp = 0;
    int usedSfp = 0;
    SkillSlot* slotR = (SkillSlot*)getCurCharacSkillR();
    slotR->calcUsedSP(skills, usedSp, usedSfp);

    int remainSp = slotR->get_remain_sp_at_index(kind);
    SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
    if (remainSp < usedSp)
    {
        slotW->set_remain_sp_at_index(0, kind);
    }
    else
    {
        slotW->set_remain_sp_at_index(remainSp - usedSp, kind);
    }

    ENUM_SKILL_TREE_KIND sfpKind =
        (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
            ? SKILL_TREE_SFP_1
            : SKILL_TREE_SFP_2;
    int remainSfp = slotR->get_remain_sfp_at_index(sfpKind);
    if (remainSfp < usedSfp)
    {
        slotW->set_remain_sfp_at_index(0, sfpKind);
    }
    else
    {
        slotW->set_remain_sfp_at_index(remainSfp - usedSfp, sfpKind);
    }

    m_historyLog.SFPSub(sfpKind, remainSfp, usedSfp,
                        (eSPSubReason)1);   // ORIG eSPSubReason=1
    m_historyLog.SPSub(kind, slotR->get_remain_sp_at_index(kind), usedSp,
                       (eSPSubReason)1);    // ORIG eSPSubReason=1

    slotW->set_skill_slot_at_index((char*)skills, 0x198, kind);

    // TODO(G1)：PvP 通路（GameWorld::IsPvPSkilTreeChannel）与成长型
    // give-skill 展开（CCharacter::get_give_skill 经 G_CDataManager()+0x14）
    // 按 master_new_skills.md 补全后返回相应结果；当前按固定应用成功返回。
    return true;
}

// 0x0867cbe4（从 GameStubs_remaining.cpp 迁移，语义对齐 CUser/ProcPremiumFatigue.md）
void CUser::ProcPremiumFatigue()
{
    if (getCurCharacR() == 0)
        return;
    if (!IsHavePremiumAdvantage())
        return;
    WongWork::CUserPremium* premium = GetPremiumInfoW();
    const bool pcRoomPremium =
        GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0) &&
        premium->GetAdvantageFatigueRate() != 0 && GetPCRoomNo() != 0;
    const int premiumProduct =
        static_cast<int>(static_cast<unsigned short>(
            premium->GetAdvantageFatigueRate())) *
        DEFAULT_MAX_FATIGUE;
    unsigned int maxPremiumFatigue =
        static_cast<unsigned int>(premiumProduct / 100);
    if (pcRoomPremium)
    {
        maxPremiumFatigue += *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(G_CDataManager()) + 0x51c);
    }
    setCurCharacMaxPremiumFatigue(
        static_cast<unsigned short>(maxPremiumFatigue));
}

// 0x0867cd20（原 GameStubs 桩，保持符号；premium 状态查询后续细化）
bool CUser::IsPremiumUser() const
{
    return false;
}

// ============================================================================
// 租借物品序列化链（CUser::SaveInventory 直接依赖；m_rentalInfo@+0x6ef78 为
// map<uint, map<uint, vector<InstanceRentalSystem::RentalInfo>>>）。
// ============================================================================

// ORIG T 0x08676dba：`out = m_rentalInfo[param]`（外层 map 键 param，值
// map<uint, vector<RentalInfo>>）；仅当键存在时拷贝。
void CUser::GetInstanceRentalInfo(
    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> >& out,
    int param)
{
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::iterator it =
        m_rentalInfo.find((unsigned int)param);
    if (it != m_rentalInfo.end())
    {
        out = it->second;
    }
}

// ORIG T 0x086773ea：返回 m_rentalInfo[param][charac_no] 是否存在；存在则拷贝到 out。
bool CUser::getCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& out, int param)
{
    if (getCurCharacR() == 0)
        return false;
    unsigned int characNo = (unsigned int)getCurCharacNo();
    if (characNo == 0)
        return false;

    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > inner;
    GetInstanceRentalInfo(inner, param);
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::iterator it =
        inner.find(characNo);
    if (it == inner.end())
        return false;
    out = it->second;
    return true;
}

// ORIG T 0x08677516：`m_rentalInfo[param][charac_no] = in`（vector 拷贝）。
void CUser::setCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& in, int param)
{
    if (getCurCharacR() == 0)
        return;
    unsigned int characNo = (unsigned int)getCurCharacNo();
    if (characNo == 0)
        return;
    m_rentalInfo[(unsigned int)param][characNo] = in;
}

// ORIG T 0x08677584：遍历当前角色租借列表，对仍存在的租借物品在序列化缓冲
// （SIG_SAVE_INVENTORY）中清除对应背包槽；并回写租借条目的 inEquip 标志。
int CUser::removeRentalItemInfoFromInvenBuf(SIG_SAVE_INVENTORY* sig, int param)
{
    if (sig == 0)
        return 0;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;

    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        int itemIndex = rental.m_itemIndex;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(itemIndex, slot, bInEquip) == 1)
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip == false)
            {
                sig->m_invenType1[slot].reset();   // 背包（类型1）槽
            }
            else
            {
                slot -= 10;
                sig->m_invenType0[slot].reset();   // 装备（类型0）槽
            }
        }
        else
        {
            LogManager::logFormat(
                1, "user.cpp",
                "bool CUser::removeRentalItemInfoFromInvenBuf(SIG_SAVE_INVENTORY*, int)",
                0x6d15,
                "Rent error. CUser::removeRentalItemInfoFromInvenBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
    }

    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
}

// 0x0864fe52（ORIG CUser::SaveInventory()）
// ORIG 反编译控制流（docs/class_func_reports/CUser/SaveInventory.md）：
//   1) isLocked4DataLoad() 非零 → cMyTrace 报错 "Can not save character until
//      loading done(m_id: %s, charac_no: %d" 并返回 0；
//   2) G_GameWorld()->GetChannelType()==7 → 返回 0；
//   3) 否则 BigStreamPool::Acquire + CStreamGuard 序列化 0xad99 字节的
//      SIG_SAVE_INVENTORY：header(0x23)+UID → memset 整块 0 → _GetSaveInvenBase 填
//      header 区 → 写 m_saveFlag=0 → GetInvenStart(0/1/3) 三块背包 + inven[22..25]
//      宠物尾 → RemoveKCItem（state==10 时再 RemoveFPItem）→ cargo 容量+数据 →
//      m_characExpand.update(this,9) → removeRentalItemInfoFromInvenBuf(1/2) →
//      MsgQueueMgr::put(DB_Q,guard) → 返回 1。
int CUser::SaveInventory()
{
    if (isLocked4DataLoad())
    {
        char* accIdStr = NumberToString(get_acc_id(), 0);
        cMyTrace trace("bool CUser::SaveInventory()", 0x181a, 5);
        trace("Can not save character until loading done(m_id: %s, charac_no: %d",
              accIdStr, getCurCharacNo());
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        return 0;
    }

    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1828);
    CStreamGuard guard(stream, true);
    guard << (int)0x23;          // SIG_SAVE_INVENTORY
    guard << (int)GetUID();

    SIG_SAVE_INVENTORY* sig = guard.GetInBuffer<SIG_SAVE_INVENTORY>();
    memset(sig, 0, 0xad99);
    _GetSaveInvenBase(sig);
    sig->m_saveFlag = 0;

    const CInventory* inven = getCurCharacInvenR();
    memcpy(sig->m_invenType0, inven->GetInvenStart(0), 0x53e);
    memcpy(sig->m_invenType1, inven->GetInvenStart(1), 0x4a58);
    memcpy(sig->m_invenType3, inven->GetInvenStart(3), 0x39aa);
    // CInventory +0x55a = &m_invenItem[22]（ORIG 原始偏移；宠物尾 4 项）
    memcpy(sig->m_creatureTail, &inven->m_invenItem[22], 0xf4);

    CInventory::RemoveKCItem(sig->m_invenType0, sig->m_invenType1);
    if (get_state() == 10)
    {
        CInventory::RemoveFPItem(sig->m_invenType0, sig->m_invenType1);
    }

    const CCargo* cargo = getCurCharacCargoR();
    sig->m_cargoCapacity = cargo->GetCapacity();
    memcpy(sig->m_cargo, cargo->get_cargo_start(), 0x2438);

    m_characExpand.update(this, (ENUM_CHARAC_EXPAND_TYPE)9);
    removeRentalItemInfoFromInvenBuf(sig, 1);
    removeRentalItemInfoFromInvenBuf(sig, 2);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ============================================================================
// _postCheckForceChangeGrowType（免费函数，TSV 修复，ORIG 0x08668823 T）。
// ORIG：遍历 QuestList hash_map<int,Quest*>（G_CDataManager()+0x18→getQuestList），
// 逐任务按 CUserCharacInfo::get_charac_job 做 checkPossibleJob，再按 quest 内
// m_fieldB8==1（比对 firstGrow == quest.m_fieldC0）/==2（checkMatchedGrowType +
// secondGrow == quest.m_fieldC0）判定，命中的任务经 _postCheckForceChangeGrowType4AheadQuest
// → CQuestClear::setClearedQuest 清任务并发包。
// 依赖 Quest/QuestList/CQuestClear 等当前未建模类型，提供可链接的签名匹配骨架（推断）。
// ============================================================================
void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow)
{
    (void)user;
    (void)firstGrow;
    (void)secondGrow;
}

// ===================== U1 批次（FixUndefU1）=====================

// ORIG T 0x08679754 sz0x2e2：仅 level%10==0 写公会公告板；文案 = 角色名 +
// ServerString(0x3e9) + "%d"(level) + ServerString(0x3e8)；len>=0x78 丢弃。
void CUser::makeGuildLevelUpMessage(int level)
{
    if (level % 10 != 0)
        return;

    char numBuf[256];
    memset(numBuf, 0, sizeof(numBuf));

    std::string body(getCurCharacName());
    body += np_server_xml::g_ServerString_.GetServerString(0x3e9, 0);
    sprintf(numBuf, "%d", level);
    body += numBuf;
    body += np_server_xml::g_ServerString_.GetServerString(0x3e8, 0);
    if (body.length() >= 0x78)
        return;

    char board[120];
    memset(board, 0, sizeof(board));
    memcpy(board, body.c_str(), body.length());

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (proxy != 0)
        proxy->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                                   -1, -1, (unsigned int)get_charac_guildkey());
}

// ORIG T 0x0867cd20 sz0x18d：guildkey!=0 时组 header 0x63 包广播队列 2。
void CUser::_UpdateUserInfo4Guild()
{
    if (get_charac_guildkey() == 0)
        return;

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x793f);
    CStreamGuard guard(s, true);
    guard << (int)0x63;                       // header
    guard << (int)-1;
    guard << (int)get_charac_guildkey();
    guard << (unsigned int)get_acc_id();
    guard << (int)getCurCharacNo();
    guard << (int)getCurCharacGrowType();
    guard << (int)get_charac_level();
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG T 0x0864c85a sz0x30b：把当前角色内存快照填入 CACHE_CHARACTER_TYPE 后交
// exchange_server 缓存。控制流/子对象偏移/错误路径逐指令核对 CacheCharacterMemory.md：
//   getCurCharacR()==0 -> false；get_state()<3 -> false；characNo=*curR(首字段)；
//   mem=exchange_server::AcquireCharacterMemory(characNo)（失败 trace + false）；
//   依次 GetRentInfo(+0x703)/GetCharacInfo(+0x0)/GetInventoryInfo(+0x703)/
//   quest(+0x15b4f)/ticket(+0x1d157)/dungeonClear(+0x1d2f3)/blacklist(+0x1eb03)/
//   actionPoint(+0x1eb13)/expand vtable 槽+8(+0x34884)/altar(+0x1f804)/etc(+0x1fd20)；
//   三个子对象内字段拷贝 + memcpy(+0x3f850, this+0x8d0fd, 0xc)；
//   CacheCharacterMemory 失败 -> FreeCharacterMemory + trace + false；成功置 lastLogin + true。
int CUser::CacheCharacterMemory()
{
    if (getCurCharacR() == 0)
        return 0;
    if (get_state() < 3)
        return 0;

    unsigned int characNo = (unsigned int)getCurCharacNo();   // 即 *curR 首字段（ORIG）
    int characIdx = get_charac_idx(characNo);
    if (characIdx < 0)
    {
        cMyTrace tr("bool CUser::CacheCharacterMemory()", 0x113c, 0);
        tr("get_charac_idx() return = %d", characIdx);
        return 0;
    }

    CACHE_CHARACTER_TYPE* mem = exchange_server::AcquireCharacterMemory((int)characNo);
    if (mem == 0)
    {
        cMyTrace tr("bool CUser::CacheCharacterMemory()", 0x1150, 0);
        tr("AcquireCharacterMemory() return = NULL, charac_no(%d)", (int)characNo);
        return 0;
    }
    char* m = (char*)mem;

    GetRentInfo(*reinterpret_cast<SIG_LOAD_INVENTORY*>(m + 0x703));
    GetCharacInfo(reinterpret_cast<SIG_LOAD_CHARAC*>(m));
    GetInventoryInfo(reinterpret_cast<SIG_LOAD_INVENTORY*>(m + 0x703));
    getCurCharacQuestR()->get_quest(reinterpret_cast<SIG_LOAD_QUEST*>(m + 0x15b4f));
    GetTicket(reinterpret_cast<SIG_LOAD_TICKET*>(m + 0x1d157));
    GetDungeonClear(reinterpret_cast<SIG_LOAD_DUNGEON_CLEAR*>(m + 0x1d2f3));
    GetBlackListCount(reinterpret_cast<SIG_REQ_BLACK_COUNT*>(m + 0x1eb03));
    APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory(
        *this, *reinterpret_cast<APSystem::_SIG_LOAD_ACTION_POINT*>(m + 0x1eb13));
    CExpandEquipslot* expand = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xc);
    // ORIG：调用 vtable 槽 +8（index 2 = CExpandEquipslot::getData）
    // (**(code**)(*expand + 8))(expand, local_10 + 0x34884)。CExpandEquipslot 在本
    // TU 仅前向声明，故按 ORIG 以 vtable 槽函数指针直调（非桩）。
    typedef void (*GetDataFn)(void*, char*);
    GetDataFn getData = *(GetDataFn*)(*(void**)expand + 8);
    getData(expand, m + 0x34884);
    advancealtar::DBAndExchangeServerProc::getFromMemory(
        *this, *reinterpret_cast<advancealtar::_CharacAdvanceAltarDbData*>(m + 0x1f804));
    GetEtcInfo(reinterpret_cast<SIG_LOAD_ETC*>(m + 0x1fd20));

    // 三个子对象内字段直拷（ORIG 报告偏移）
    *(int*)(m + 0x1fd2c) = *(int*)(m + 0x686);
    *(int*)(m + 0x70b) = *(int*)(m + 0x1fd2c);
    memcpy(m + 0x3f850, m_pad8d0fd, 0xc);

    if (exchange_server::CacheCharacterMemory((int)characNo, mem))
    {
        SetLastLoginChannelNo(G_CEnvironment()->get_channel_no());
        SetLastLoginCharacNo((int)characNo);
        return 1;
    }
    exchange_server::FreeCharacterMemory(mem);
    cMyTrace tr("bool CUser::CacheCharacterMemory()", 0x1190, 0);
    tr("CacheCharacterMemory() return = false");
    return 0;
}

// ============================================================================
// 角色内存快照填充（CacheCharacterMemory 依赖）——ORIG 报告逐函数实现。
// ============================================================================

// ORIG 0x0864de58 _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
bool CUser::GetDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig) const
{
    sig->m_selected = 0;
    const _Charac_info* cur = getCurCharacR();
    if (cur != 0)
    {
        getDungeonClearRefR()->toString(sig->m_clearData, 0x1800);
    }
    else
    {
        cMyTrace tr("bool CUser::GetDungeonClear(SIG_LOAD_DUNGEON_CLEAR*) const", 0x1400, 5);
        tr("CUser::GetDungeonClear User  m_selected is null");
    }
    return cur != 0;
}

// ORIG 0x0867f1d4 _ZNK5CUser17GetBlackListCountEP19SIG_REQ_BLACK_COUNT
int CUser::GetBlackListCount(SIG_REQ_BLACK_COUNT* sig) const
{
    sig->m_count = 0;
    sig->m_blackCount = getCurrCharacBlackCount();
    return 1;
}

// ============================================================================
// 角色内存快照填充：GetRentInfo/GetCharacInfo/GetInventoryInfo/GetTicket（CacheCharacterMemory 依赖）
// 及 helper 链（GetPcRoomRentInfo/GetDimensionRentInfo/getCharacInfoDetail/getTagCharacInfo/
// getTagCharacInventory/GetInventory/removeRentalItemInfoFromInvenLoadBuf）。
// 具名填充 SIG_LOAD_CHARAC/INVENTORY/TICKET（SigTypes.h 权威布局）。
// ============================================================================

// ORIG T 0x086769b4 sz0x35：SIG_LOAD_INVENTORY 的租借区（pcroom@+0x153e4 /
// dimension@+0x153f0）由 GetPcRoomRentInfo/GetDimensionRentInfo 填充。
void CUser::GetRentInfo(SIG_LOAD_INVENTORY& sig)
{
    GetPcRoomRentInfo(reinterpret_cast<SIG_SAVE_PC_ROOM_RENT_ITEM*>(&sig.m_rentPcRoom));
    GetDimensionRentInfo(reinterpret_cast<SIG_SAVE_DIMENSION_RENT_ITEM*>(&sig.m_rentDimension));
}

// ORIG T 0x086769ea：PC 房间租借物品 → SIG_SAVE_PC_ROOM_RENT_ITEM（最多 2 个 0xc 条目）。
// 逐条目读取 getCurrentCharacRentalInfo(list,1)，对仍存在物品写 [0]itemIndex/[4]byte/[5]byte/
// [6]inEquip/[8]int。参数为租借清单类型 1。
int CUser::GetPcRoomRentInfo(SIG_SAVE_PC_ROOM_RENT_ITEM* sig)
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (m_rentalFlag1 != 1)
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 1) != 1)
        return 1;

    int index = 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_fieldB;
            sig->m_entry[index].m_inEquip = bInEquip ? 1 : 0;
            sig->m_entry[index].m_field8 = rental.m_flag0;
            ++index;
            if (index > 1)
                return 0;
        }
    }
    return 1;
}

// ORIG T 0x08676bec：次元租借物品 → SIG_SAVE_DIMENSION_RENT_ITEM（最多 12 个 8 字节条目）。
int CUser::GetDimensionRentInfo(SIG_SAVE_DIMENSION_RENT_ITEM* sig)
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (m_rentalFlag1 != 1)
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 2) != 1)
        return 1;

    int index = 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_fieldB;
            sig->m_entry[index].m_inEquip = bInEquip ? 1 : 0;
            sig->m_entry[index].m_field7 = 0;
            ++index;
            if (index > 11)
                return 0;
        }
    }
    return 1;
}

// ORIG T 0x0864bc04 sz0x1b5：填充 SIG_LOAD_CHARAC。
//   +0x0 acc_id / +0x4 charac_no(*curR 首字段) / +0x8 result(0 初始, 失败置 1) /
//   +0xc CHARAC_DATA（getCharacInfoDetail）/ +0x686 tag_charac_no /
//   +0x68a striker_skill_index / +0x68b TAG_CHARAC_DATA（getTagCharacInfo）。
void CUser::GetCharacInfo(SIG_LOAD_CHARAC* sig) const
{
    sig->m_accId = get_acc_id();
    const _Charac_info* cur = getCurCharacR();
    if (cur != 0)
        sig->m_characNo = (unsigned int)cur->m_characNo;
    int characIdx = get_charac_idx(sig->m_characNo);
    sig->m_result = 0;

    CHARAC_DATA* cd = reinterpret_cast<CHARAC_DATA*>(&sig->m_characData);
    if (getCharacInfoDetail(characIdx, sig->m_characNo, cd) != 1)
    {
        cMyTrace tr("bool CUser::GetCharacInfo(SIG_LOAD_CHARAC*) const", 0x1052, 0);
        tr("getCharacInfoDetail() return = false");
        sig->m_result = 1;
        return;
    }

    unsigned int tagCharacNo = (unsigned int)getCurCharacTagCharacNo();
    int tagIdx = get_charac_idx(tagCharacNo);
    if (tagIdx >= 0 && tagIdx < (int)m_characList.size())
    {
        sig->m_tagCharacNo = tagCharacNo;
        sig->m_strikerSkillIndex = getCurCharacStrikerSkillIndex();
    }
    getTagCharacInfo(tagIdx, sig->m_tagCharacNo,
                     reinterpret_cast<TAG_CHARAC_DATA*>(&sig->m_tagCharacData));
}

// ORIG T 0x0864d646：填充 SIG_LOAD_INVENTORY 的背包/金币/装备镜像/cargo/avatar 区。
int CUser::GetInventory(SIG_LOAD_INVENTORY* sig)
{
    const CInventory* inven = getCurCharacInvenR();
    sig->m_money = inven->get_money();
    sig->m_coin = inven->GetCoin();
    sig->m_payCoin = inven->GetPayCoin();
    sig->m_eventCoin = inven->GetEventCoin();
    inven->GetInvenData(1, &sig->m_invenType1, 0x4a58);
    inven->GetInvenData(2, &sig->m_invenType2, 0x1905);
    inven->GetInvenData(3, &sig->m_invenType3, 0x39aa);
    memcpy(&sig->m_equipMirror, (char*)inven + 0x1c, 0x53e);
    // creature tail：绝对偏移 +0x12787（位于 invenType3 区内尾段）
    memcpy((char*)sig + 0x12787, (char*)inven + 0x55a, 0xf4);
    inven->GetClearAvatar((unsigned long*)&sig->m_clearAvatar);
    const CCargo* cargo = getCurCharacCargoR();
    sig->m_cargoCapacity = (unsigned int)cargo->GetCapacity();
    cargo->get_cargo(&sig->m_cargo);
    sig->m_avatarCoin = *(unsigned int*)((char*)inven + 0x658);  // AvatarCoin::Get（inven+0x658 处 4 字节）
    sig->m_inventoryCapacity = inven->get_inventory_capacity();
    return 1;
}

// ORIG T 0x0864d4ba：填充 SIG_LOAD_INVENTORY（含 GetInventory + creature + avatar + tag + rent）。
void CUser::GetInventoryInfo(SIG_LOAD_INVENTORY* sig)
{
    sig->m_result = 0;
    CExpandEquipslot* expand2 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2);
    typedef void (*GetDataFn)(void*, char*);
    (*(GetDataFn*)(*(void**)expand2 + 8))(expand2, (char*)&sig->m_expandData2);
    CExpandEquipslot* expand9 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
    (*(GetDataFn*)(*(void**)expand9 + 8))(expand9, (char*)&sig->m_expandData9);

    bool bFailed = false;
    if (GetInventory(sig) != 1)
        bFailed = true;
    if (!bFailed)
    {
        const CInventory* inven = getCurCharacInvenR();
        if (inven->GetCreatureMgrR()->GetCreatureItem(sig) != 1)
            bFailed = true;
    }
    if (!bFailed)
    {
        const CInventory* inven = getCurCharacInvenR();
        if (inven->GetAvatarItemMgrR()->GetItem(
                reinterpret_cast<AvatarDetailInfo*>(&sig->m_avatarDetail),
                &sig->m_avatarCount) != 1)
            bFailed = true;
    }
    if (bFailed)
    {
        sig->m_result = 1;
        return;
    }

    if (getTagCharacR() != 0)
        getTagCharacInventory(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(&sig->m_tagInven));
    removeRentalItemInfoFromInvenLoadBuf(sig, 1);
    removeRentalItemInfoFromInvenLoadBuf(sig, 2);
    sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
}

// ORIG T 0x0869067c：TAG 角色背包 → TAG_CHARAC_INVEN_DATA。
bool CUser::getTagCharacInventory(TAG_CHARAC_INVEN_DATA* out) const
{
    if (getTagCharacR() == 0)
        return false;
    // ORIG：getTagCharacInvenR() 返回 CInventory*（位于 _Charac_info.m_inven，本源码返回 Inven_Item* 同址）
    const CInventory* inven = reinterpret_cast<const CInventory*>(getTagCharacInvenR());
    inven->GetInvenData(2, (char*)out + 0x53e, 0x1905);
    memcpy(out, (char*)inven + 0x1c, 0x53e);
    inven->GetClearAvatar((unsigned long*)((char*)out + 0x3ff8));
    inven->GetAvatarItemMgrR()->GetItem(
        reinterpret_cast<AvatarDetailInfo*>((char*)out + 0x1e47),
        (int*)((char*)out + 0x1e43));
    const CCargo* cargo = getTagCharacCargoR();
    *(unsigned int*)((char*)out + 0x4020) = (unsigned int)cargo->GetCapacity();
    return true;
}

// ORIG T 0x0867776c：从 SIG_LOAD_INVENTORY 背包镜像清除已不存在的租借物品。
int CUser::removeRentalItemInfoFromInvenLoadBuf(SIG_LOAD_INVENTORY* sig, int param)
{
    if (sig == 0)
        return 0;
    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip)
                reinterpret_cast<Inven_Item*>((char*)&sig->m_equipMirror + slot * 0x3d)->reset();
            else
                reinterpret_cast<Inven_Item*>((char*)&sig->m_invenType1 + slot * 0x3d)->reset();
        }
        else
        {
            LogManager::logFormat(
                1, "user.cpp",
                "bool CUser::removeRentalItemInfoFromInvenLoadBuf(SIG_LOAD_INVENTORY*, int)",
                0x6d54,
                "Rent error. CUser::removeRentalItemInfoFromInvenLoadBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), rental.m_itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
    }
    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
}

// ORIG T 0x0867846e sz0xb7：遍历 m_list8cf80（list<int>）票表 → SIG_LOAD_TICKET。
void CUser::GetTicket(SIG_LOAD_TICKET* sig) const
{
    sig->m_field4 = 0;
    int index = 0;
    for (std::list<int>::const_iterator it = m_list8cf80.begin();
         it != m_list8cf80.end() && index <= 99; ++it, ++index)
    {
        sig->m_tickets[index] = *it;
    }
    sig->m_count = index;
}

// ORIG T 0x0864bdba：从 m_characList[param_1] 填充 CHARAC_DATA（SIG_LOAD_CHARAC +0xc 区）。
int CUser::getCharacInfoDetail(int param_1, unsigned int characNo, CHARAC_DATA* out) const
{
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    const _Charac_info& src = m_characList[param_1];
    if ((unsigned int)src.m_characNo != characNo)
        return 0;
    char* d = out->m_data;
    const char* s = (const char*)&src;

    *(int*)(d + 0x00) = src.m_characNo;
    strncpy(d + 0x04, s + 0x04, 0x1e);
    *(unsigned char*)(d + 0x22) = *(const unsigned char*)(s + 0x22);
    *(unsigned char*)(d + 0x23) = *(const unsigned char*)(s + 0x25);
    *(unsigned char*)(d + 0x24) = *(const unsigned char*)(s + 0x26);
    *(short*)(d + 0x25) = *(const short*)(s + 0x27);
    *(unsigned char*)(d + 0x27) = *(const unsigned char*)(s + 0x29);
    *(int*)(d + 0x28) = *(const int*)(s + 0x2b);
    *(unsigned short*)(d + 0x2c) = *(const unsigned short*)(s + 0x33);
    *(unsigned short*)(d + 0x2e) = *(const unsigned short*)(s + 0x35);
    *(unsigned short*)(d + 0x30) = *(const unsigned short*)(s + 0x37);
    *(unsigned short*)(d + 0x32) = *(const unsigned short*)(s + 0x4f);
    memcpy(d + 0x34, s + 0x39, 0x14);  // CommonTime::GetCreateTime
    *(int*)(d + 0x4d) = *(const int*)(s + 0x7b);
    *(int*)(d + 0x56) = *(const int*)(s + 0xdbe);
    *(unsigned char*)(d + 0x5a) = *(const unsigned char*)(s + 0xdc2);
    *(unsigned short*)(d + 0x5b) = *(const unsigned short*)(s + 0x77);
    *(unsigned short*)(d + 0x5d) = *(const unsigned short*)(s + 0x79);
    *(unsigned char*)(d + 0x5f) = (*(const int*)(s + 0xe22) != 0) ? 1 : 0;
    memcpy(d + 0x60, s + 0x88, 0x52);
    *(unsigned char*)(d + 0xb2) = *(const unsigned char*)(s + 0x75);
    *(unsigned short*)(d + 0xb3) = *(const unsigned short*)(s + 0x4d);
    *(unsigned int*)(d + 0xb5) = *(const unsigned int*)(s + 0x51);
    memcpy(d + 0xb9, s + 0x55, 0x10);
    *(int*)(d + 0xc9) = *(const int*)(s + 0x65);
    *(int*)(d + 0xcd) = *(const int*)(s + 0x69);
    *(int*)(d + 0xd1) = *(const int*)(s + 0x6d);
    *(int*)(d + 0xd5) = *(const int*)(s + 0xe49);
    *(int*)(d + 0xd9) = *(const int*)(s + 0xe4d);
    *(int*)(d + 0xdd) = *(const int*)(s + 0xe51);
    *(int*)(d + 0xe1) = *(const int*)(s + 0xe55);
    *(int*)(d + 0xe5) = *(const int*)(s + 0xe59);
    *(int*)(d + 0xe9) = *(const int*)(s + 0xe5d);
    *(int*)(d + 0xed) = *(const int*)(s + 0xe45);
    *(int*)(d + 0xf1) = *(const int*)(s + 0xe68);
    *(unsigned char*)(d + 0xf5) = *(const unsigned char*)(s + 0xe70);
    *(int*)(d + 0xf6) = *(const int*)(s + 0xe71);
    *(int*)(d + 0xfa) = *(const int*)(s + 0xe79);
    *(unsigned int*)(d + 0xfe) = *(const unsigned int*)(s + 0xdf4);
    *(unsigned int*)(d + 0x102) = *(const unsigned int*)(s + 0xdf8);
    *(int*)(d + 0x106) = *(const int*)(s + 0xe7d);
    *(int*)(d + 0x10a) = *(const int*)(s + 0xe81);
    *(unsigned short*)(d + 0x10e) = *(const unsigned short*)(s + 0x121f);
    *(int*)(d + 0x110) = *(const int*)(s + 0x1221);
    *(int*)(d + 0x114) = *(const int*)(s + 0x1225);
    *(int*)(d + 0x118) = *(const int*)(s + 0x7f);
    *(unsigned short*)(d + 0x11c) = *(const unsigned short*)(s + 0x122c);
    *(char*)(d + 0x11e) = *(const char*)(s + 0x1239);
    *(char*)(d + 0x11f) = *(const char*)(s + 0x123e);
    *(unsigned char*)(d + 0x120) = *(const unsigned char*)(s + 0x123f);
    *(unsigned char*)(d + 0x121) = *(const unsigned char*)(s + 0x1241);
    *(unsigned short*)(d + 0x122) = *(const unsigned short*)(s + 0x126c);
    *(unsigned char*)(d + 0x124) = *(const unsigned char*)(s + 0x126e);
    *(int*)(d + 0x125) = *(const int*)(s + 0x126f);
    *(char*)(d + 0x129) = *(const char*)(s + 0x1275);
    *(unsigned char*)(d + 0x12a) = *(const unsigned char*)(s + 0x1276);
    *(unsigned char*)(d + 0x12b) = *(const unsigned char*)(s + 0x1277);
    *(unsigned char*)(d + 0x12c) = *(const unsigned char*)(s + 0x1278);
    *(unsigned char*)(d + 0x12d) = *(const unsigned char*)(s + 0x127a);
    *(unsigned char*)(d + 0x12e) = *(const unsigned char*)(s + 0xe40);
    *(unsigned char*)(d + 0x12f) = *(const unsigned char*)(s + 0x1280);
    *(int*)(d + 0x130) = *(const int*)(s + 0x12ad);
    *(int*)(d + 0x134) = (int)get_acc_id();
    return 1;
}

// ORIG T 0x086904f0：从 m_characList[param_1] 填充 TAG_CHARAC_DATA（SIG_LOAD_CHARAC +0x68b 区）。
int CUser::getTagCharacInfo(int param_1, unsigned int characNo, TAG_CHARAC_DATA* out) const
{
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    const _Charac_info& src = m_characList[param_1];
    if ((unsigned int)src.m_characNo != characNo)
        return 0;
    char* d = out->m_data;
    const char* s = (const char*)&src;
    *(int*)(d + 0x00) = src.m_characNo;
    strncpy(d + 0x04, s + 0x04, 0x1d);
    *(unsigned char*)(d + 0x21) = *(const unsigned char*)(s + 0x26);
    *(short*)(d + 0x22) = *(const short*)(s + 0x27);
    *(unsigned char*)(d + 0x24) = *(const unsigned char*)(s + 0x29);
    *(char*)(d + 0x25) = *(const char*)(s + 0x1239);
    memcpy(d + 0x26, s + 0x88, 0x52);
    return 1;
}

// ORIG T 0x0864d646：CInventory/CCargo 序列化 helper（供 GetInventoryInfo）。

// ORIG 0x08677dd8 _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
// 遍历 getBattleRecordRefR() 的 map<int,GameResultType>，每元素 0x50 字节序列化到
// sig+0x3c8+idx*0x50（最多 100 个），末尾写 count 到 sig+0x3c4。
void CUser::GetRecord(SIG_LOAD_ETC* sig) const
{
    const std::map<int, GameResultType>* rec = getBattleRecordRefR();
    int index = 0;
    for (std::map<int, GameResultType>::const_iterator it = rec->begin();
         it != rec->end() && index <= 99; ++it, ++index)
    {
        memcpy((char*)sig + 0x3c8 + index * 0x50, &it->second, 0x50);
    }
    sig->m_recordCount = (int)rec->size();
}

// ORIG 0x08688dc4 _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
// 遍历 m_limitItemUsage 向量，每元素序列化到 sig+(i+0x263)*0x10 的 0x10 字节区。
void CUser::GetLimitItemUsage(SIG_LOAD_ETC* sig) const
{
    if (sig == 0) return;
    *(int*)((char*)sig + 0x2638) = 0;   // 元素计数（ORIG +0x2638）
    int index = 0;
    for (std::vector<LimitItemUsage>::const_iterator it = m_limitItemUsage.begin();
         it != m_limitItemUsage.end(); ++it, ++index)
    {
        char* dst = (char*)sig + (index + 0x263) * 0x10;
        *(int*)(dst + 0x14) = it->m_field15;
        *(int*)(dst + 0x18) = it->m_field19;
        *(int*)(dst + 0x10) = it->m_fieldc;
        *(unsigned char*)(dst + 0xc) = it->m_field10;
    }
    *(int*)((char*)sig + 0x2638) = index;
}

// ORIG 0x086910a6 _ZNK5CUser27GetCharacCashAttendanceInfoER20SIG_ATTENDANCE_EVENT
void CUser::GetCharacCashAttendanceInfo(SIG_ATTENDANCE_EVENT& info) const
{
    *(char*)&info = 0;
}

// ---- 四个 CUser 字段访问器（CacheCharacterMemory 域；ORIG W 弱符号）----
int CUser::getTotalPcRoomPlayTime() const            // 0x08110e10：*(int*)(this+0x8eba0)
{
    return *(int*)((const char*)m_pad8eba0);
}
unsigned char CUser::getPcRoomPlayTimeRewardCnt() const   // 0x08110dec：*(uchar*)(this+0x8eba4)
{
    return *(unsigned char*)((const char*)m_pad8eba0 + 4);
}
int CUser::GetCurCharacUsedGiftFatigueQuantity() const   // 0x08110e6c：*(int*)(this+0x8ebac)
{
    return m_field8ebac;
}
unsigned char CUser::getFlagPresentAvengerTitle() const  // 0x08697578：*(uchar*)(this+0x8d246)
{
    return m_ceraUserInfo.m_field62;
}

// ORIG 0x0868ddc2 _ZNK5CUser19GetAuraAvatarOptionER18stAuraAvatarOption
// 读当前角色 _Charac_info+0x1281 的 3 个 int（ORIG 汇编；_Charac_info 布局在本 TU 不完整，
// 用文档化偏移访问，见 CUserCharacInfo.h:13 说明）。
void CUser::GetAuraAvatarOption(stAuraAvatarOption& option) const
{
    const _Charac_info* cur = getCurCharacR();
    if (cur != 0)
    {
        const char* c = (const char*)cur;
        option.m_value0 = *(const int*)(c + 0x1281);
        option.m_value1 = *(const int*)(c + 0x1285);
        option.m_value2 = *(const int*)(c + 0x1289);
    }
}

// ORIG 0x0868f03c _ZNK5CUser18GetCurCharacOptionER15CharacterOption
// memcpy 0x200 字节自 _Charac_info+0x12b7（CharacterOption = char[0x200]）。
void CUser::GetCurCharacOption(CharacterOption& option) const
{
    const _Charac_info* cur = getCurCharacR();
    if (cur != 0)
    {
        memcpy(&option, (const char*)cur + 0x12b7, sizeof(CharacterOption));
    }
}

// ORIG 0x0864dcf2 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
bool CUser::GetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo) const
{
    *(int*)((char*)skill + 4) = 0;
    SkillSlot* slot = (SkillSlot*)getCurCharacSkillR();
    slot->get_remain_sp((int*)((char*)skill + 8), (int*)((char*)skill + 0x1a4),
                        (int*)((char*)skill + 0x340), (int*)((char*)skill + 0x344));
    slot->get_skill_slot((char*)skill + 0xc, (char*)skill + 0x1a8, 0x198);
    STSkillCommandData::STCommand cmds[50];
    memset(cmds, 0, sizeof(cmds));
    get_skill_command_buf(cmds);
    memcpy((char*)skill + 0x348, cmds, sizeof(cmds));
    // ORIG：G_CDataManager()+0xa850 的 byte（当前 CDataManager 无具名字段，文档化偏移）
    *(char*)((char*)skill + 0x411) = *(char*)((char*)G_CDataManager() + 0xa850);
    slot->getComboSkillSaveData(combo);
    return 1;
}

// ORIG 0x08654768 _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
// 将 CUserCharacInfo 角色内存 +0x1254 处（getSkillCommand()）的 skill command map
// （std::map<int,STSkillCommandData>）逐项压缩为 STCommand（32 位位域）写入 out[]，最多 50 项。
// map 为空指针时 out 不变；返回 out（ORIG 返回未定义值，调用方 GetSkill 忽略返回值）。
STSkillCommandData::STCommand* CUser::get_skill_command_buf(STSkillCommandData::STCommand* out) const
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return out;
    int idx = 0;
    for (std::map<int, STSkillCommandData>::iterator it = pMap->begin();
         it != pMap->end() && idx <= 0x31; ++it, ++idx)
    {
        const STSkillCommandData& v = it->second;
        STSkillCommandData::STCommand cmd;
        cmd.m_command = v.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = v.m_count;     // bits 29-31
        cmd.m_key0    = v.m_key0;      // bits 14-16
        cmd.m_key1    = v.m_key1;      // bits 17-19
        cmd.m_key2    = v.m_key2;      // bits 20-22
        cmd.m_key3    = v.m_key3;      // bits 23-25
        cmd.m_key4    = v.m_key4;      // bits 26-28
        out[idx] = cmd;
    }
    return out;
}

// ORIG 0x0865459a _ZN5CUser21get_skill_command_bufEv
// 同上，但写入成员 m_skillCommands[50]（+0x703db）并返回其指针；map 为空指针返回 0。
STSkillCommandData::STCommand* CUser::get_skill_command_buf()
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return 0;
    int idx = 0;
    for (std::map<int, STSkillCommandData>::iterator it = pMap->begin();
         it != pMap->end() && idx <= 0x31; ++it, ++idx)
    {
        const STSkillCommandData& v = it->second;
        STSkillCommandData::STCommand cmd;
        cmd.m_command = v.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = v.m_count;     // bits 29-31
        cmd.m_key0    = v.m_key0;      // bits 14-16
        cmd.m_key1    = v.m_key1;      // bits 17-19
        cmd.m_key2    = v.m_key2;      // bits 20-22
        cmd.m_key3    = v.m_key3;      // bits 23-25
        cmd.m_key4    = v.m_key4;      // bits 26-28
        m_skillCommands[idx] = cmd;
    }
    return m_skillCommands;
}

// ORIG 0x0867794c _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// 依 SIG_LOAD_ETC 具名布局填充。expand 子区经 CExpandEquipslot vtable 槽 +8（getData）
// 直调（同 CacheCharacterMemory 的 0xc 槽位写法）；混合区内子对象用文档化偏移定位。
int CUser::GetEtcInfo(SIG_LOAD_ETC* sig) const
{
    sig->m_field8 = 0;
    memcpy(sig->m_pvpResult, getPVPResultR(), 0x84);   // PvpResultType::Get（0x85d5924）为 0x21 dword memcpy
    GetRecord(sig);
    GetContinuousItemInfo(sig);
    GetLimitItemUsage(sig);
    sig->m_oneDayLetheFlag = GetOneDayLetheFlag();
    memcpy(sig->m_backupSkill, GetCurCharacBackupSkill(), 0x199);
    sig->m_oneDayLetheFlag2ND = GetOneDayLetheFlag2ND();
    memcpy(sig->m_backupSkill2ND, GetCurCharacBackupSkill2ND(), 0x199);
    sig->m_curConditionEventStep = GetCurConditionEventStep();
    sig->m_curConditionEventRewardStep = GetCurConditionEventRewardStep();
    sig->m_properDungeonClearCount = GetProperDungeonClearCount();

    typedef void (*GetDataFn)(void*, char*);
    // expand type 6 → +0x40ce
    CExpandEquipslot* e6 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)6);
    (*(GetDataFn)((char*)*(void**)e6 + 8))(e6, (char*)sig->m_expandData6);

    sig->m_curVillageAttackCount = GetCurVillageAttackCount();
    sig->m_curRevengeDungeonCount = GetCurRevengeDungeonCount();
    sig->m_flagPresentAvengerTitle = getFlagPresentAvengerTitle();
    sig->m_totalPcRoomPlayTime = getTotalPcRoomPlayTime();
    sig->m_pcRoomPlayTimeRewardCnt = getPcRoomPlayTimeRewardCnt();
    sig->m_curCharacUsedGiftFatigueQuantity = GetCurCharacUsedGiftFatigueQuantity();
    sig->m_bitset = 1;   // std::bitset<32>::set 位0（ORIG +0xc9bc）

    // skill 区（+0xc9c0）+ combo（+0x14aaa）
    GetSkill((SIG_LOAD_SKILL*)sig->m_mixedRegion, (SIG_COMBO_SKILL*)sig->m_comboSkill);

    // 混合区内 expand 子区（m_mixedRegion 内文档化偏移）
    char* mix = (char*)sig->m_mixedRegion;
    CExpandEquipslot* e8 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    (*(GetDataFn)((char*)*(void**)e8 + 8))(e8, mix + (0xcdd5 - 0xc9c0));
    CExpandEquipslot* e7 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)7);
    (*(GetDataFn)((char*)*(void**)e7 + 8))(e7, mix + (0xcf1f - 0xc9c0));
    CExpandEquipslot* eb = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xb);
    (*(GetDataFn)((char*)*(void**)eb + 8))(eb, mix + (0xd5a7 - 0xc9c0));
    CExpandEquipslot* ed = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xd);
    (*(GetDataFn)((char*)*(void**)ed + 8))(ed, mix + (0xd718 - 0xc9c0));
    CExpandEquipslot* e10 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x10);
    (*(GetDataFn)((char*)*(void**)e10 + 8))(e10, mix + (0xd73c - 0xc9c0));
    CExpandEquipslot* ee = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xe);
    (*(GetDataFn)((char*)*(void**)ee + 8))(ee, (char*)sig->m_expandDataE);
    CExpandEquipslot* ef = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xf);
    (*(GetDataFn)((char*)*(void**)ef + 8))(ef, (char*)sig->m_expandDataF);
    CExpandEquipslot* e11 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x11);
    (*(GetDataFn)((char*)*(void**)e11 + 8))(e11, mix + (0xecc8 - 0xc9c0));
    CExpandEquipslot* e12 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x12);
    (*(GetDataFn)((char*)*(void**)e12 + 8))(e12, mix + (0xecd0 - 0xc9c0));

    // tag 角色技能槽（+0x10/+0x1a8）
    if (getTagCharacR() != 0)
    {
        ((SkillSlot*)getTagCharacSkillR())->get_skill_slot(sig->m_tagSkillSlot, sig->m_tagSkillCombo, 0x198);
    }

    // aura（+0xd730）/ character option（+0xeac8）在混合区内
    GetAuraAvatarOption(*(stAuraAvatarOption*)(mix + (0xd730 - 0xc9c0)));
    GetCurCharacOption(*(CharacterOption*)(mix + (0xeac8 - 0xc9c0)));

    // attendance（+0x14a99）+ blueMarble（+0x14a98）+ expand 0x13（+0x14a9a）
    GetCharacCashAttendanceInfo(*(SIG_ATTENDANCE_EVENT*)&sig->m_attendanceEvent);
    sig->m_blueMarbleEnterCount = getBlueMarbleEnterCount();
    CExpandEquipslot* e13 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x13);
    (*(GetDataFn)((char*)*(void**)e13 + 8))(e13, (char*)&sig->m_attendanceEvent + (0x14a9a - 0x14a99));

    return 1;
}

// ORIG T 0x08650aa2 sz0x87f：SIG_SAVE_CHARAC(0x10d) 组包；四个脏块分支后投递
// 队列 2 并清 save 标志。个别字段写入为报告偏移直写（推断标注见注释）。
void CUser::_SaveCharacData()
{
    SetAccountLastPlayTime(GlobalData::s_systemTime_.getCurSec());

    if (getCurCharacVill() == 7)   // power war 村
    {
        setCurCharacLastPlayTickPowerWar(GlobalData::s_systemTime_.getCurSec());
        SetCurCharacChaosDieTime(0);
        m_characterView.enableSaveCharacView();
    }
    setCurCharacLastPlayTick(GlobalData::s_systemTime_.getCurSec());

    if (!isSaveCharacInfo() && !IsSaveCharacStat() && !isSaveCharacEtc() &&
        !IsSaveCharacManageInfo())
        return;

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x19c3);
    CStreamGuard guard(s, true);
    guard << (int)0x22;                      // SIG_SAVE_CHARAC
    guard << (int)GetUID();
    char* sig = (char*)guard.GetInBuffer<SIG_SAVE_CHARAC>();
    memset(sig, 0, 0x10d);

    *(unsigned int*)(sig + 0xf2) = *(unsigned int*)getDungeonGainedGold(); /*推断: +0x00 值*/
    *(unsigned int*)(sig + 0xe6) = get_acc_id();
    *(unsigned int*)(sig + 0xea) = *(unsigned int*)((char*)this + 0x8d110); /*推断*/
    *(unsigned int*)(sig + 0xee) = GetOverTradeGoldCount();
    *(float*)(sig + 0xf6) = (float)GetRating();
    sig[0xfa] = (char)GetTutorialSkipable();
    *(unsigned int*)(sig + 0x00) = getCurCharacNo();

    sig[4]  = (char)isSaveCharacInfo();
    sig[5]  = (char)IsSaveCharacStat();
    sig[6]  = (char)isSaveCharacEtc();
    sig[7]  = (char)IsSaveCharacManageInfo();
    sig[0x10a] = (char)isSavedPremiumCubeInfo();
    if (sig[0x10a])
    {
        unsigned char cubA = 0, cubB = 0;
        getContractOfCubePremiumInfo(cubA, cubB);
        sig[0x10b] = (char)cubA;
        sig[0x10c] = (char)cubB;
    }
    if (sig[4])   // info 块
    {
        *(unsigned short*)(sig + 0x8) = (unsigned short)get_charac_level();
        sig[0xa] = (char)getCurCharacGrowType();
        memcpy(sig + 0xb, (const char*)getCurCharacR() + 0x88, 0x52); /*推断*/
        sig[0x5d] = (char)GetCurCharacExpertJobType();
    }
    if (sig[5])   // stat 块
    {
        *(unsigned int*)(sig + 0x5e)  = getCurCharacExp();
        sig[0x62] = (char)getCurCharacVill4Save();
        *(unsigned short*)(sig + 0x63) = (unsigned short)GetFatigue();
        *(unsigned short*)(sig + 0x65) = (unsigned short)getCurCharacPremiumFatigue();
        sig[0x67] = (char)getCurCharacStamina();
        *(unsigned short*)(sig + 0x68) = (unsigned short)getCurCharacUsedFatigue();
        *(unsigned int*)(sig + 0x6a)  = getCurCharacTutorialFlag();
        if (getCurCharacR() != 0)
            memcpy(sig + 0x6e, getCurCharacEscaladeTutorialFlag(), 0x10);
        *(unsigned int*)(sig + 0x7e)  = getCurCharacTradeGoldTotal();
        *(unsigned int*)(sig + 0x82)  = getCurCharacTradeGoldTotalBillion();
        *(unsigned int*)(sig + 0x86)  = getCurCharacTradeGoldDaily();
        *(unsigned int*)(sig + 0x8a)  = getCurCharacDungeonHelpAbuseRatio();
        *(unsigned int*)(sig + 0x8e)  = getCurCharacDungeonHelpAbuseExp();
        *(unsigned int*)(sig + 0x92)  = GetCurCharacChaosPoint();
        *(unsigned int*)(sig + 0x96)  = GetCurCharacChaosExp();
        *(unsigned int*)(sig + 0x9a)  = GetCurCharacChaosModeCount();
        *(unsigned int*)(sig + 0x9e)  = GetCurCharacChaosKillCount();
        *(unsigned int*)(sig + 0xa2)  = GetCurCharacChaosDieCount();
        *(unsigned int*)(sig + 0xa6)  = GetCurCharacChaosDieTime();
        sig[0xaa] = (char)GetOpencoin();
        *(unsigned int*)(sig + 0xab)  = GetAssaultCount();
        *(unsigned int*)(sig + 0xaf)  = GetCurCharacChaosKillTime();
        *(unsigned int*)(sig + 0xb3)  = GetCurCharacLuckPoint();
        *(unsigned int*)(sig + 0xbb)  = GetCurCharacExpertJobExp();
        *(unsigned short*)(sig + 0xbf) = (unsigned short)GetFatigueBatteryCharging();
        *(unsigned short*)(sig + 0xc1) = (unsigned short)GetUserPowerWarPoint();
        sig[0xc3] = (char)getCurCharacVillPrev();
        *(unsigned int*)(sig + 0xc4)  = GetPowerWarAssaultCount();
        *(unsigned int*)(sig + 200)   = GetPowerWarAssaultVictoryCount();
        *(unsigned short*)(sig + 0xcc) = (unsigned short)getCurCharacFatigueGrownUpBuff();
        sig[0xd5] = (char)get_growth_power_reward();
        sig[0xce] = (char)getMemberDungeonFlag();
        *(unsigned short*)(sig + 0xcf) = (unsigned short)getCurCharacMemberBonusFatigue();
        *(unsigned int*)(sig + 0xd1)  = getCurCharacLastPlayDungeonIndex();
        *(unsigned int*)(sig + 0xd6)  = GetCurCharacChaosResponTime();
        sig[0xff]  = (char)getCurEquipslotFlag();
        sig[0x100] = (char)getCurChannelEquipslotSwitch();
        sig[0x101] = (char)getCurExpandEquipslotSwitch();
        *(unsigned int*)(sig + 0xe2)  = getCurCharacLastPlayTickPowerWar();
        *(unsigned short*)(sig + 0xfb) = (unsigned short)GetCurCharacEmotion();
        sig[0xfd] = (char)GetCurCharacSkillTreeIndex();
        sig[0xfe] = (char)GetCurExEquipSlotStat();
        getCurCharacInvenW()->GetAvatarItemMgrW()->SaveEndurance();
    }
    if (sig[6])   // etc 块
    {
        sig[0xda] = (char)get_guild_create_right();
        *(unsigned short*)(sig + 0xdb) = (unsigned short)getCurCharacMaxPremiumFatigue();
        sig[0xdd] = (char)getCurCharacEventCharacterGrowtype();
    }
    if (sig[7])   // manage info 块
    {
        sig[0x103] = (char)getCurCharacStrikerSkillIndex();
        *(unsigned int*)(sig + 0x104) = getCurCharacTagCharacNo();
        *(unsigned short*)(sig + 0x108) = (unsigned short)GetCurCharacMaxEquipLevel();
    }

    *(unsigned int*)(sig + 0xde) = getCurCharacLastPlayTick();
    sig[0x102] = (char)get_charac_visible_values(); /*推断*/
    *(unsigned char*)(sig + 0x102) &= (unsigned char)~0x01;  // setBit_Byte(b,false,0)

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    disableSaveCharacInfo();
    disableSaveCharacStat();
    disableSaveCharacEtc();
    disableSaveCharacManageInfo();
    disableSavedPremiumCubeInfo();
}

// ORIG T 0x0864d84e sz0x2a：param!=0 时 push_back 到受限商品列表(+0x711ec)。
void CUser::SetRestrictedGoods(int itemIdx)
{
    if (itemIdx != 0)
        m_list711ec.push_back(itemIdx);
}

// ORIG T 0x0864d878 sz0x91：线性查找列表。
bool CUser::IsRestrictedGoods(int itemIdx)
{
    if (itemIdx == 0)
        return false;
    for (std::list<int>::const_iterator it = m_list711ec.begin();
         it != m_list711ec.end(); ++it)
    {
        if (*it == itemIdx)
            return true;
    }
    return false;
}

// ORIG T 0x0864d9a4 sz0x17b：header(0,0x44)；flag=true 单条，false 全列表。
void CUser::SendRestrictedGoods(bool flag, int itemIdx)
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(0, 0x44);
    if (flag)
    {
        pkt->put_byte(0);
        pkt->put_short(1);
        pkt->put_int(itemIdx);
    }
    else
    {
        pkt->put_byte(1);
        pkt->put_short((int)m_list711ec.size());
        for (std::list<int>::const_iterator it = m_list711ec.begin();
             it != m_list711ec.end(); ++it)
        {
            pkt->put_int(*it);
        }
    }
    pkt->finalize(true);
    Send(packet);
}

// ORIG T 0x08656636 sz0xad：遍历 m_characList 统计 _Charac_info+0x27(short)==type。
int CUser::GetSpecialLevelCharacCnt(int type)
{
    int cnt = 0;
    for (size_t i = 0; i < m_characList.size(); ++i)
    {
        if ((int)*(const short*)((const char*)&m_characList[i] + 0x27) == type)
            ++cnt;
    }
    return cnt;
}

// ORIG _ZNK5CUser22HasCurCharacRentalItemEi T 0x086772d2 sz0x117：
// 外层 map 键=rental 类型，内层键=当前角色 characNo（+0x00），vector 非空即有。
bool CUser::HasCurCharacRentalItem(int type) const
{
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0 || *(const unsigned int*)cur == 0)
        return false;
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::const_iterator outer =
        m_rentalInfo.find((unsigned int)type);
    if (outer == m_rentalInfo.end())
        return false;
    std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator inner =
        outer->second.find(*(const unsigned int*)cur);
    if (inner == outer->second.end())
        return false;
    return !inner->second.empty();
}

// ORIG W 0x08230172 sz0x3b：guildkey!=0 且当前角色==m_guildDB+0x17（master）。
bool CUser::IsGuildMaster()
{
    if (get_charac_guildkey() == 0)
        return false;
    return (int)getCurCharacNo() ==
           *(const int*)((const char*)&m_guildDB + 0x17);
}

// ORIG T 0x0866c414 sz0x55：m_guildDB+0x2e 起 5 个 int 副会长表。
bool CUser::IsSubGuildMaster()
{
    for (int i = 0; i < 5; ++i)
    {
        if (*(const int*)((const char*)&m_guildDB + 0x2e + i * 4) ==
            (int)getCurCharacNo())
            return true;
    }
    return false;
}

// ---------------- U1 批次第二段（deleteSpecific 域 / 杂项发送域）----------------

// 定义于 CDataManagerScripts.cpp（ORIG 0x867cf8d T）
int _checkTimeoutItem(const Inven_Item& item);

// ORIG T 0x0867cf0c sz0x81：找 pair.first==itemIdx，返回 second；未找到 -1。
int CUser::_isMatchedItem(int itemIdx,
                          const std::vector<std::pair<int, int> >& list)
{
    for (std::vector<std::pair<int, int> >::const_iterator it = list.begin();
         it != list.end(); ++it)
    {
        if (it->first == itemIdx)
            return it->second;
    }
    return -1;
}

// ORIG T 0x0867d0be sz0x330：[begin,end] 槽位超时/匹配/无效物品删除；
// 无效物品入 mailDel，最后邮件补偿。
void CUser::_deleteSpecificItemInven(INVEN_TYPE type,
                                     const std::vector<std::pair<int, int> >& list,
                                     std::vector<std::pair<int, int> >& out,
                                     int begin, int end)
{
    CInventory* inven = getCurCharacInvenW();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = begin; slot <= end; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(type, slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        int matched = -1;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                mailDel.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            enableSaveInven();
            if (matched == 0)
                inven->delete_item(type, slot, item.m_addInfo2,
                                   (eItemDelReason)0x13, true);
            else
                inven->delete_item(type, slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13, true);

            if (type == (INVEN_TYPE)0)
                out.push_back(std::make_pair(3, slot));   /*推断: ENUM_ITEMSPACE(3)*/
            else if (type == (INVEN_TYPE)1)
                out.push_back(std::make_pair(0, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Inven", mailDel);
}

// ORIG T 0x0867d3ee sz0x2ae：同型逻辑作用于 CCargo 槽位 0..0x97。
void CUser::_deleteSpecificItemCargo(
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    CCargo* cargo = getCurCharacCargoW();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = 0; slot < 0x98; ++slot)
    {
        Inven_Item item = cargo->get_cargo_slot(slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        int matched = -1;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                mailDel.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->delete_item(slot, item.m_addInfo2, (eItemDelReason)0x13);
            else
                cargo->delete_item(slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13);
            out.push_back(std::make_pair(2, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Cargo", mailDel);
}

// ORIG T 0x0867d69c sz0x1d2：GetCharacExpandData(6) 兑换列表遍历删除，
// 超时或命中即 erase + alter() 标脏。m_redeemList 私有，按 ORIG 布局经 +0x08 访问。
void CUser::_deleteSpecificRedeemItem(
    const std::vector<std::pair<int, int> >& list)
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
        return;

    typedef std::list<Redeem_Item::Inven_Item_Expand_RedeemInfo> RedeemList;
    RedeemList& items = *(RedeemList*)((char*)redeem + 0x08);  // 推断：权威布局 +0x08

    unsigned int count = 0;
    RedeemList::iterator it = items.begin();
    while (it != items.end() && count <= 0x14)
    {
        Inven_Item item;
        memcpy(&item, &*it, sizeof(Inven_Item));   // RedeemInfo 前 0x3d 字节即 Inven_Item
        if (item.m_addInfo != 0)
        {
            bool remove =
                _checkTimeoutItem(item) ||
                (_isMatchedItem(item.m_addInfo, list) != -1);
            if (remove)
            {
                items.erase(it++);
                redeem->alter();
            }
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;   /*推断*/  // ORIG 空项仅跳过不 erase
        }
        ++count;
    }
}

// ORIG T 0x0867d8a4 sz0x2c1：账号金库（须 IsExistAccountCargo），槽位数=GetCapacity()。
void CUser::_deleteSpecificItemAccountCargo(
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    if (!IsExistAccountCargo())
        return;

    CAccountCargo* cargo = GetAccountCargo();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = 0; slot < cargo->GetCapacity(); ++slot)
    {
        Inven_Item item = cargo->GetSlot(slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        int matched = -1;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                mailDel.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->DeleteItem(slot, item.m_addInfo2);
            else
                cargo->DeleteItem(slot, item.m_addInfo2 - matched);
            out.push_back(std::make_pair(0xc, slot));
        }
    }

    RewardItem2DeleteInvalidItem("AccountCargo", mailDel);
}

// ORIG T 0x0867db66 sz0x39：转发给 GetCharacExpandData(0xe)（CTitleBook）。
void CUser::_deleteSpecificItemTitieBook(
    const std::vector<std::pair<int, int> >& list)
{
    CTitleBook* titleBook =
        (CTitleBook*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (titleBook != 0)
        titleBook->deleteTimeoutItem(list);
}

// ORIG T 0x086928c2 sz0x145：金币兑换。itemIdx<=0x28d291 → (idx-0x28d287)*10^6；
// 否则 idx*10^7-0x549a800。bReset 只做上限校验（err=10 返回 false）。
bool CUser::DoGetGoldItem(int itemIdx, std::vector<std::pair<int, int> >& out,
                          bool bReset, ENUM_ERROR& err)
{
    int amount;
    if (itemIdx <= 0x28d291)
        amount = (itemIdx - 0x28d287) * 1000000;
    else
        amount = itemIdx * 10000000 - 0x549a800;

    if (bReset)
    {
        unsigned int limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());
        if ((unsigned int)getCurCharacMoney() + (unsigned int)amount > limit)
        {
            err = (ENUM_ERROR)0xa;
            return false;
        }
    }
    else
    {
        CInventory* inven = getCurCharacInvenW();
        inven->gain_money(amount, (eMoneyAddReason)0xe, true, 0);
        out.push_back(std::make_pair((int)ENUM_ITEMSPACE_0, 0));
    }
    return true;
}

// ORIG T 0x08692a34 sz0xc2：!bReset 时经 s_pIPGHelper 虚调用 vtable+0x0c/+0x2c；
// WongWork::IPG::CIPGHelper 仅最小建模（仅 dtor 虚函数），虚接口待补后恢复真实调用。
bool CUser::DoGetCeraPointItem(unsigned int money, bool bReset,
                               ENUM_ERROR& err, unsigned long itemIdx)
{
    (void)err; (void)money; (void)itemIdx;
    if (!bReset && GlobalData::s_pIPGHelper != 0)
    {
        // TODO(ORIG 0x8692a58/0x8692a96/0x8692ad2)：vtable+0xc(this,this,money,
        // itemIdx==0x28ec3d?3:1,0,0) 与 vtable+0x2c(this,this)。推断：虚接口未建模。
    }
    return true;
}

// ORIG T 0x0868c3bc sz0xa2：OnTimeEvent(0x33) 删除奖励用户。
void CUser::delete_ontime_reward_list()
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    if (ev == 0)
        return;
    // 虚表 +0x34：IsEventing(int)（PvP_Room.cpp 同型裸虚调用）
    typedef int (*IsEventingFn)(CEventBase*, int);
    if (((IsEventingFn)(*(void**)(*(char**)ev + 0x34)))(ev, 0))
    {
        // COnTimeEvent::delete_reward_user(CUser*) ORIG T 0x081bbc64（类未建模，
        // 函数指针桥，Secu_GoldControl.cpp 先例）
        typedef void (*DeleteRewardUserFn)(void*, CUser*);
        ((DeleteRewardUserFn)0x081bbc64)(ev, this);
    }
}

// ORIG T 0x086924aa sz0x119：job==9 才发包；去重 combo 技能 → 包 0x1bd。
void CUser::sendComboSkillList()
{
    if (get_charac_job() != 9)
        return;
    ((SkillSlot*)getCurCharacSkillW())->eraseDuplicationComboSKill();
    PacketGuard packet;
    packet.put_header(0, 0x1bd);
    packet.put_byte(isComboSkillSet() ? 1 : 0);
    packet.put_byte(2);
    ((const SkillSlot*)getCurCharacSkillR())->getComboSkillSendInfo(packet);
    packet.finalize(true);
    Send(packet);
    setComboSkillSet(false);
}

// ORIG T 0x08656808 sz0xbb：发送当前金币（MakeItemPacket(1,1) → 0xe 包）。
void CUser::SendCoin()
{
    if (getCurCharacR() == 0)
        return;
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 1, packet))
    {
        packet.finalize(true);
        Send(packet);
    }
}

// ORIG T 0x086504d8 sz0x11d：频道类型 !=7 时经 BigStreamPool 构造
// header(0x23)+UID 内部流，GetInBuffer<SIG_SAVE_INVENTORY> 后 _GetSaveInvenBase
// 填充、+0x08 字节置 1，投递 DB_Q。反编译见 CUser/SaveMoneyCoin.md。
bool CUser::SaveMoneyCoin()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 7)
        return false;

    Stream* stream = GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x18cc);
    CStreamGuard guard(stream, true);
    guard << 0x23;
    guard << (int)GetUID();
    SIG_SAVE_INVENTORY* sig =
        guard.GetInBuffer<SIG_SAVE_INVENTORY>();
    _GetSaveInvenBase(sig);
    sig->m_saveFlag = 1;                // ORIG movb $0x1,0x8(%eax)（+0x08 标志）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ORIG T 0x08690e36 sz0x82：事件邮件提醒 tooltip。空包 put_header(0,0x1ad) +
// finalize(true) 后直接 Send（docs/class_func_reports/CUser/SendEventMailAlaramTooltip.md）。
void CUser::SendEventMailAlaramTooltip()
{
    PacketGuard packet;
    packet.put_header(0, 0x1ad);
    packet.finalize(true);
    Send(packet);
}

// —— 未建模 ORIG 符号的函数指针桥（Secu_GoldControl.cpp 先例）——
// CUser::verifyStackableItemProcess  ORIG T 0x0868f60e
// （SaveMoneyCoin/SendCoin 已按报告还原为真实成员，桥接移除。）
typedef int (*U1VerifyStackableFn)(CUser*, int, const CItem*);  /*推断: 枚举 Type 未建模*/

// ORIG T 0x0869154a sz0x664：抽奖计数道具发放（事件币/道具/邮件补偿）。
void CUser::giveLotteryCountItem(int itemNo, bool match)
{
    if (getCurCharacR() == 0)
    {
        LogManager::logFormat(
            1, "user.cpp", "void CUser::giveLotteryCountItem(int, bool)", 0xb45f,
            "giveLotteryCountItem curchar = null id(%s) item_no(%u) match(%d)",
            NumberToString(get_acc_id(), 0), itemNo, match ? 1 : 0);
        return;
    }

    CStackableItem* item =
        (CStackableItem*)G_CDataManager()->find_item(itemNo);
    if (item == 0)
        return;
    if (!((U1VerifyStackableFn)0x0868f60e)(
            this, 0x36, item))
        return;

    std::vector<Inven_Item> rewards;
    const std::vector<int>& src = item->m_vec3a0;   // ORIG +0x3a0
    for (size_t i = 0; i + 1 < src.size(); i += 2)
    {
        Inven_Item it;                              // 默认构造后按 ORIG 填 8 字节
        *(int*)((char*)&it + 0) = src[i];
        *(int*)((char*)&it + 4) = src[i + 1];
        rewards.push_back(it);

        if (*(int*)&it == 1 && src[i + 1] >= 0)     // 事件币分支
        {
            int amount = src[i + 1];
            unsigned int now = getCurCharacInvenR()->GetEventCoin()
                             + (unsigned int)amount;
            getCurCharacInvenW()->SetEventCoin(now);
            m_historyLog.EventCoinAdd((int)now, amount, (eCoinAddReason)2);
            SaveMoneyCoin();
            SendCoin();
            continue;
        }

        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, it, src[i + 1]);

        bool give = (*(int*)&it != 0) || (it.get_add_info() != 0);
        if (!give)
            continue;

        int slot = getCurCharacInvenW()->insertItemIntoInventory(
            it, (eItemAddReason)0xd, true, true);
        if (slot < 0)
        {
            char msg[256];
            const char* str = g_scriptStringManager_.findString(
                4, "game_server_msg_104", (bool*)0);
            strncpy(msg, str ? str : "", 0xff);
            msg[0xff] = '\0';
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                getCurCharacName(), it, 0, (unsigned int)getCurCharacNo(),
                msg, (int)strlen(msg), 0, GetServerGroup(), false, false);
        }
        else
        {
            SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1d0);
    packet.put_byte((int)rewards.size());
    for (size_t i = 0; i < rewards.size(); ++i)
    {
        packet.put_int(*(int*)((char*)&rewards[i] + 2));
        packet.put_int(*(int*)((char*)&rewards[i] + 7));
    }
    packet.finalize(true);
    Send(packet);
}

// ORIG T 0x0868f472 sz0x158：i=0..min(6,m_field796c8) 发 0x1e 字节 MoneyLogPacket；
// 最后 resetMoneyLog()。
void CUser::SendMoenyLog()
{
    struct MoneyLogPacket   /*推断*/   // 前 6 字节为 ORIG ctor 包头，未建模置零
    {                                  // 共 0x1e 字节
        char  m_hdr[6];      // +0x00 包头（推断）
        int   m_characNo;    // +0x06
        int   m_money0;      // +0x0a
        int   m_money1;      // +0x0e
        unsigned int m_accId;// +0x12
        int   m_money2;      // +0x16
        int   m_serverGroup; // +0x1a
    };

    if (getCurCharacR() != 0)
    {
        for (int i = 0; i < 7 && i <= m_field796c8; ++i)
        {
            if (m_moneyLogs[i].m_money[0] == 0 &&
                m_moneyLogs[i].m_money[1] == 0)
                continue;
            MoneyLogPacket pkt;
            memset(&pkt, 0, sizeof(pkt));
            pkt.m_characNo    = getCurCharacNo();
            pkt.m_money0      = m_moneyLogs[i].m_money[0];
            pkt.m_money1      = m_moneyLogs[i].m_money[1];
            pkt.m_accId       = get_acc_id();
            pkt.m_money2      = m_moneyLogs[i].m_money[2];
            pkt.m_serverGroup = (int)GetServerGroup();
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                    GetServerGroup());
            if (proxy != 0)
                proxy->SendPacket((char*)&pkt, 0x1e);
        }
    }
    resetMoneyLog();
}

// ORIG T 0x0868170c sz0x113：PC 房 logout 通知。nexon_packet::stLoginPacket
// (0x08694252)/stPacket::GetLength(0x08694224) 未建模 → 函数指针桥。
void CUser::SendLogoutToPCRoomServer()
{
    char login[0x210];
    char packet[0x1210];
    memset(login, 0, sizeof(login));
    login[0] = 1;
    strcpy(login + 2, get_acc_name());
    login[1] = (char)strlen(get_acc_name());
    strcpy(login + 0x102, getCurCharacName());
    login[0x101] = (char)strlen(getCurCharacName());
    *(int*)(login + 0x201) = get_local_ip_address();
    login[0x205] = 1;

    typedef void* (*MakeLoginPacketFn)(void*, const void*);  // 0x08694252
    ((MakeLoginPacketFn)0x08694252)(packet, login);
    typedef int (*GetLengthFn)(void*);                       // 0x08694224
    int len = ((GetLengthFn)0x08694224)(packet);
    ((void(*)(void*, char*, int))0x08471a32)(GlobalData::s_pcroom_proxy,
                                             packet, len);  /*推断: SendTcpPacket 桥*/
}

// ORIG T 0x0864870e sz0xdc：LOGDB_Q(=4)：<<0xcc、<<-1、<<reason。
void CUser::SendClientCrashDownLogToLogDB(int reason)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x8ac);
    CStreamGuard guard(stream, true);
    guard << 0xcc;
    guard << -1;
    guard << reason;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// —— onUpdateHackCount 依赖的未建模符号函数指针桥 ——
// WongWork::CMCAPManager::getPointPerHackType  ORIG W 0x08695025
// WongWork::CMCAPManager::incPoint             ORIG T 0x084eb53e
// WongWork::CMCAPManager::getPoint             ORIG W 0x0822eea4
// XNuclear::CHades::AddHackTypeCnt             ORIG T 0x084b9328
// CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver ORIG T 0x08695360
// CHackAnalyzer::getServerHackAccumulatedCnt   ORIG W 0x0822eef6
// CHackAnalyzer::getServerHackCnt              ORIG W 0x08695086
// CUser::setHackUser                           ORIG T 0x0867f748
// cUserHistoryLog::HackDetected                ORIG T 0x08685320
// WongWork::CHackAnalyzer::setUserP            ORIG T 0x080f79c0

// 局部栈上视图（不参与 ODR）：HackTypeInfo_t 16 字节
struct HackTypeInfoView
{
    unsigned short m_typeA;    // +0x00
    unsigned short m_pad02;    // +0x02
    unsigned short m_typeB;    // +0x04（==3 触发二次检查）
    unsigned short m_typeC;    // +0x06
    unsigned int   m_extra;    // +0x08
    unsigned int   m_pad0c;    // +0x0c
};
typedef int  (*McapGetPointPerHackTypeFn)(WongWork::ENUM_HACKTYPE);
typedef void (*McapIncPointFn)(WongWork::CMCAPManager*, int);
typedef unsigned int (*McapGetPointFn)(WongWork::CMCAPManager*);
typedef void (*HadesAddHackTypeCntFn)(void*, WongWork::ENUM_HACKTYPE,
                                      unsigned int, unsigned int, unsigned int);
typedef int (*CheckMaxHackCntOverFn)(WongWork::CAutoPunishRuleHackTypeMgr*,
                                     WongWork::ENUM_HACKTYPE, unsigned int,
                                     WongWork::CHackAnalyzer*, void*,
                                     void*, void*, void*, char*);
typedef unsigned int (*GetServerHackAccumulatedCntFn)(
    WongWork::CHackAnalyzer*, WongWork::ENUM_HACKTYPE);
typedef unsigned int (*GetServerHackCntFn)(
    WongWork::CHackAnalyzer*, WongWork::ENUM_HACKTYPE);
typedef void (*SetHackUserFn)(CUser*, unsigned int, unsigned int,
                              unsigned int, unsigned int, unsigned short,
                              unsigned int);
typedef void (*HistHackDetectedFn)(cUserHistoryLog*, unsigned int,
                                   unsigned int, unsigned int, unsigned int,
                                   unsigned int, unsigned int);
typedef void (*SetUserPFn)(WongWork::CHackAnalyzer*, CUser*);

static void s_user_setHackUserFromInfo(
    CUser* user, const HackTypeInfoView& info,
    GetServerHackAccumulatedCntFn getAccCnt)
{
    unsigned int cntB = getAccCnt(user->getHackAnalyzer(),
                                  (WongWork::ENUM_HACKTYPE)info.m_typeB);
    unsigned int cntA = getAccCnt(user->getHackAnalyzer(),
                                  (WongWork::ENUM_HACKTYPE)info.m_typeA);
    ((SetHackUserFn)0x0867f748)(user, info.m_typeA, cntA, info.m_typeB,
                                cntB, info.m_typeC, info.m_extra);
}

// ORIG T 0x0867fd0a sz0x3bb：MCAP 扣减率 → hades 计数 → checkMaxHackCntOver 处置
// → HackDetected 历史记录。
void CUser::onUpdateHackCount(WongWork::ENUM_HACKTYPE type, int count,
                              unsigned int a, unsigned int b)
{
    McapGetPointPerHackTypeFn getPer =
        (McapGetPointPerHackTypeFn)0x08695025;
    McapIncPointFn incPoint = (McapIncPointFn)0x084eb53e;
    McapGetPointFn getPoint = (McapGetPointFn)0x0822eea4;
    HadesAddHackTypeCntFn addCnt = (HadesAddHackTypeCntFn)0x084b9328;
    CheckMaxHackCntOverFn checkMax = (CheckMaxHackCntOverFn)0x08695360;
    GetServerHackAccumulatedCntFn getAccCnt =
        (GetServerHackAccumulatedCntFn)0x0822eef6;
    GetServerHackCntFn getCnt = (GetServerHackCntFn)0x08695086;
    HistHackDetectedFn hackDetected = (HistHackDetectedFn)0x08685320;

    incPoint(&m_mcap, getPer(type) * count);
    if (getPoint(&m_mcap) > 1000)
        setStdDropRate(100 - (int)(getPoint(&m_mcap) / 100));

    char ip[16];
    if (!isHackUserTimer())
    {
        addCnt(getHades(), type, (unsigned int)count, a, b);
        ((SetUserPFn)0x080f79c0)(getHackAnalyzer(), this);
        m_network.GetPeerIP2(ip, 0x10);

        HackTypeInfoView info;
        memset(&info, 0, sizeof(info));  /*推断*/  // HackTypeInfo_t ctor 仅清零
        if (checkMax(GlobalData::g_autoPunishRuleHackTypeMgr, type,
                     getAccCnt(getHackAnalyzer(), type), getHackAnalyzer(),
                     &info, (char*)this + 0x8d1b8, (char*)this + 0x8d1d0,
                     (char*)this + 0x8d1c4, ip))
        {
            s_user_setHackUserFromInfo(this, info, getAccCnt);
        }
        if (info.m_typeB == 3)
        {
            HackTypeInfoView info2;
            memset(&info2, 0, sizeof(info2));
            if (checkMax(GlobalData::g_autoPunishRuleHackTypeMgr, type,
                         getAccCnt(getHackAnalyzer(), type), getHackAnalyzer(),
                         &info2, (char*)this + 0x8d1b8,
                         (char*)this + 0x8d1d0, (char*)this + 0x8d1c4, ip))
            {
                s_user_setHackUserFromInfo(this, info2, getAccCnt);
            }
        }
    }

    hackDetected(&m_historyLog, type, (unsigned int)count,
                 getCnt(getHackAnalyzer(), type),
                 getAccCnt(getHackAnalyzer(), type), a, b);
}

// ORIG _ZN5CUser14setStdDropRateEi @ 0x0828715c (W)：
// *(int*)(this+0x79624) = rate，即 m_codeHackCheck(+0x795fc).m_field28。
void CUser::setStdDropRate(int rate)
{
    m_codeHackCheck.m_field28 = rate;
}

// ============================================================================
// CUser 连续/冷却道具链簇（ORIG T，单函数还原见各 .md）
// ============================================================================

// ORIG _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj @ 0x0865eb0c (T)
// cooltime 处理：CStackableItem+0x2d0(ms) 换算为秒累加当前秒，更新/插入
// CheckCoolTimeItem 所读的 cooltime 列表条目（GetCurCharacCoolTimeItemList）。
void CUser::_processCoolTimeItem(CStackableItem* item, unsigned int itemIdx,
                                 unsigned int curSec)
{
    unsigned int expireSec =
        (unsigned int)((unsigned int)item->m_int2d0 / 1000) + curSec;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    char found = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        if ((*cool)[i].m_itemIdx == (int)itemIdx)
        {
            if ((*cool)[i].m_expireTime < expireSec)
                (*cool)[i].m_expireTime = expireSec;
            found = 1;
        }
    }
    if (!found)
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        cool->push_back(info);
    }
    UpdateCoolTimeItemInfo(curSec);
}

// ORIG _ZN5CUser18_processEffectItemEP14CStackableItemjj @ 0x0865ec0e (T)
// effect 处理：CStackableItem+0x2e0 第 0 字节非 1 → 报错；
// 否则读 +0x2dc(ms) → 秒并 insert timer(0x76)，更新/插入 effect 列表。
void CUser::_processEffectItem(CStackableItem* item, unsigned int itemIdx,
                               unsigned int curSec)
{
    if (*(char*)((char*)item + 0x2e0) != 1)
    {
        cMyTrace trace("void CUser::_processEffectItem(CStackableItem*, "
                       "unsigned int, unsigned int)", 0x391e, 0);
        trace("ERROR : Mismatch tag [stat change duration] and "
              "[effect maintenance]. There is no duration value.");
        return;
    }
    unsigned int durationSec = (unsigned int)item->m_int2dc / 1000;
    unsigned int expireSec = curSec + durationSec;
    CStackableItem::StatChangeInfo stat = item->GetStatChangeInfo();
    if (stat.flag != 0)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                    (TIMER_MESSAGE)0x76, (int)durationSec,
                                    (int)itemIdx, (unsigned int)getCurCharacNo());
    }
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    char found = 0;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        if ((*effect)[i].m_itemIdx == (int)itemIdx)
        {
            if ((*effect)[i].m_expireTime < expireSec)
                (*effect)[i].m_expireTime = expireSec;
            found = 1;
        }
    }
    if (!found)
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        effect->push_back(info);
    }
    UpdateEffectItemInfo(curSec);
}

// ORIG _ZN5CUser22_processContinuousItemEjP14CStackableItem @ 0x0865ea62 (T)
void CUser::_processContinuousItem(unsigned int itemIdx, CStackableItem* item)
{
    unsigned int curSec = (unsigned int)GlobalData::s_systemTime_.getCurSec();
    if (item != 0)
    {
        CStackableItem::CoolTimeOption coolOpt = item->GetCoolTimeOption();
        if (coolOpt.flag != 0)
            _processCoolTimeItem(item, itemIdx, curSec);
        CStackableItem::CoolTimeOption effOpt = item->GetEffectItemOption();
        if (effOpt.flag != 0)
            _processEffectItem(item, itemIdx, curSec);
    }
    _processSpeicalContinuousTimeItem(itemIdx);
}

// ORIG _ZN5CUser33_processSpeicalContinuousTimeItemEj @ 0x08659b60 (T)
// 特殊连续时间道具：查 item→is_stackable→IsExpAffect→GetExpAffectRatio>0
// 时 SetExpAffectItemEffect(true, ratio)。
void CUser::_processSpeicalContinuousTimeItem(unsigned int itemIdx)
{
    CItem* pItem = G_CDataManager()->find_item((int)itemIdx);
    if (pItem == 0)
        return;
    if (!pItem->is_stackable())
        return;
    CStackableItem* stack = (CStackableItem*)pItem;
    if (!stack->IsExpAffect())
        return;
    float ratio = stack->GetExpAffectRatio();
    if (ratio > 0.0f)
        SetExpAffectItemEffect(true, ratio);
}

// ORIG _ZN5CUser17CheckCoolTimeItemEm @ 0x0865e994 (T)
// 当前角色 cooltime 列表中存在 itemIdx==param_1 且 expireTime>curSec 的条目。
bool CUser::CheckCoolTimeItem(unsigned long itemIdx) const
{
    if (getCurCharacR() == 0)
        return false;
    const std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemListR();
    unsigned int curSec = (unsigned int)GlobalData::s_systemTime_.getCurSec();
    for (std::vector<ContinuousItemInfo>::const_iterator it = cool->begin();
         it != cool->end(); ++it)
    {
        if ((unsigned long)it->m_itemIdx == itemIdx)
        {
            if (curSec < it->m_expireTime)
                return true;
            return false;
        }
    }
    return false;
}

// ORIG _ZN5CUser22UpdateCoolTimeItemInfoEj @ 0x08658b2a (T)
// 组 SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO（0x100 占位：characNo@+0x0 +
// count@+0x4 + [0x194]条目簇）投递队列 2。
void CUser::UpdateCoolTimeItemInfo(unsigned int expireTime)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x2cb8);
    CStreamGuard guard(s, true);
    guard << (int)0x132;
    guard << (int)GetUID();
    char* sig =
        (char*)guard.GetInBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    memset(sig, 0, 0x198);
    *(unsigned int*)(sig + 0x0) = (unsigned int)getCurCharacNo();
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    unsigned int count = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        unsigned int idx = (unsigned int)(*cool)[i].m_itemIdx;
        unsigned int exp = (*cool)[i].m_expireTime;
        if (exp < expireTime || idx > 0x7ffffffe || exp > 2000000000u)
        {
            cool->erase(cool->begin() + i);
            --i;
            continue;
        }
        if (count < 0x32)
        {
            *(int*)(sig + 0x8 + count * 8) = (*cool)[i].m_itemIdx;
            *(int*)(sig + 0xc + count * 8) = (int)(*cool)[i].m_expireTime;
            ++count;
        }
    }
    *(unsigned char*)(sig + 0x4) = (unsigned char)count;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG _ZN5CUser20UpdateEffectItemInfoEj @ 0x08658d90 (T)
void CUser::UpdateEffectItemInfo(unsigned int expireTime)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x2ceb);
    CStreamGuard guard(s, true);
    guard << (int)0x137;
    guard << (int)GetUID();
    char* sig =
        (char*)guard.GetInBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    memset(sig, 0, 0x198);
    *(unsigned int*)(sig + 0x0) = (unsigned int)getCurCharacNo();
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    unsigned int count = 0;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        unsigned int idx = (unsigned int)(*effect)[i].m_itemIdx;
        unsigned int exp = (*effect)[i].m_expireTime;
        if (exp < expireTime || idx > 0x7ffffffe || exp > 2000000000u)
        {
            effect->erase(effect->begin() + i);
            --i;
            continue;
        }
        if (count < 0x32)
        {
            *(int*)(sig + 0x8 + count * 8) = (*effect)[i].m_itemIdx;
            *(int*)(sig + 0xc + count * 8) = (int)(*effect)[i].m_expireTime;
            ++count;
        }
    }
    *(unsigned char*)(sig + 0x4) = (unsigned char)count;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
//   @ 0x0868a782 (T)
// vec 长==2 时：元素加信息(+2 addInfo)==0x28be7b 即罐子；另一元素为产出道具。
// 组 Packet_Event_UpgradeJar_Output_Statistic_GTS(0x106e/0x12) 经 statistic
// proxy(GET_ENUM_SERVER_GROUP_NONE) 下发。罐子 id 为 0x28be7b，剩余逻辑：
//   local_e = 罐子在 vec 中的下标（0 或 1）；m_itemCode = vec[1-local_e].addInfo；
//   m_itemCount = vec[local_e].+7（m_addInfo2 罐子数量）。
void CUser::SendEventJarOutputItemStatistics(
    const std::vector<Inven_Item>& items)
{
    if ((int)items.size() != 2)
        return;
    unsigned short jarIdx = 0;
    if (items[0].m_addInfo != 0x28be7b)
    {
        if (items[1].m_addInfo != 0x28be7b)
            return;
        jarIdx = 1;
    }
    int itemCode = items[1 - (int)jarIdx].m_addInfo;
    int itemCount = items[(int)jarIdx].m_addInfo2;
    Packet_Event_UpgradeJar_Output_Statistic_GTS pkt;
    pkt.m_itemCode = itemCode;
    pkt.m_itemCount = itemCount;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)0);
    if (proxy != 0)
        proxy->SendPacket((char*)&pkt, 0x12);
}

// ============================================================================
// ORIG 0x08692b84 CUser::SendAradEventItemLog(unsigned long, int)：
// "Arad_StatisticsEventItemList" 脚本 needStatistics(itemIndex) 命中时，
// 构造 16 字节 Sig{itemIndex,count,accId,characNo(-1)} 经内部流 0x20 投递
// LOGDB_Q。反编译见 docs/class_func_reports/CUser/SendAradEventItemLog.md。
// ============================================================================
namespace ARAD { namespace DISPATCHER {
// 定义于 AvatarRechargeServer.cpp（ORIG 0x081977b6）
void make_internal_stream_jpn(CStreamGuard& guard, int type, int param);
}}

void CUser::SendAradEventItemLog(unsigned long itemIndex, int count)
{
    ARAD::Arad_DataManager* mgr = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
    ARAD::Arad_InterfaceGameScript* script =
        mgr->findGameScript("Arad_StatisticsEventItemList");
    if (script == 0 ||
        !static_cast<ARAD::Arad_StatisticsEventItemList*>(script)->needStatistics(itemIndex))
    {
        return;
    }

    // 线格式（ORIG put_binary 0x10）：+0x00 ulong itemIndex / +0x04 int count /
    // +0x08 uint accId / +0x0c int characNo。
    struct SigAradEventItemLogLine
    {
        unsigned long itemIndex;
        int count;
        unsigned int accId;
        int characNo;
    } sig;
    sig.itemIndex = itemIndex;
    sig.count = count;
    sig.accId = get_acc_id();
    sig.characNo = get_charac_no(-1);

    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0xb9c7);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x20, (int)GetUID());
    guard.put_binary(&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ============================================================================
// 任务 QP 发送链（RestoreUserQuestStubs：Dispatcher_Debug_Clear_Quest 依赖）
// ============================================================================

// ORIG T _ZN5CUser12sendCharacQpEv @0x0868ac24：
// GetCharacExpandData(0xb)（CQuestShop）非空 → shop->sendCharacQp(this, 0)。
void CUser::sendCharacQp()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->sendCharacQp(this, 0);
    }
}

// ORIG T _ZN5CUser13incQuestPointEiib @0x0868ac66：
// GetCharacExpandData(0xb) 非空 → old=getQp()、addQp(qp)；!send →
// shop->sendCharacQp(this,0)；m_historyLog.QPAdd(old, questIdx, qp)。
void CUser::incQuestPoint(int qp, int questIdx, bool send)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        int before = shop->getQp();
        shop->addQp(qp);
        if (!send)
        {
            shop->sendCharacQp(this, 0);
        }
        m_historyLog.QPAdd(before, questIdx, qp);
    }
}
