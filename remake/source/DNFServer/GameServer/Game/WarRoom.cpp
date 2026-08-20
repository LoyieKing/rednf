#include "LogManager.h"
// df_game_r WarRoom（G2-3 PvP 链，战场房间，池化 40）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/WarRoom.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// WarField/map_monster/map_item 及跨类方法按 asm-label extern 调用
// （链接桩由 PvP_deps.cpp / 主 agent 提供，本 TU 只保证自身符号一致）。
#include <string.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "WarRoom.h"
#include "CItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"

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

// ---- 本地枚举（与 CInventory.h 同 guard，避免未来同时包含时重定义） ----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8
};
#endif

#ifndef DNF_ENUM_EMONEYADDREASON_DEFINED
#define DNF_ENUM_EMONEYADDREASON_DEFINED
enum eMoneyAddReason
{
    eMoneyAddReason_0 = 0,
    eMoneyAddReason_4 = 4,
    eMoneyAddReason_7 = 7,
    eMoneyAddReason_0x1a = 0x1a
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14
};
#endif

#ifndef DNF_ENUM_VALUE_STATISTIC_FIELD_DEFINED
#define DNF_ENUM_VALUE_STATISTIC_FIELD_DEFINED
enum VALUE_STATISTIC_FIELD
{
    VALUE_STATISTIC_FIELD_0 = 0
};
#endif

namespace user_creature
{
class CCreatureMgr;
}

namespace WongWork
{
class CGenUniqueNo
{
public:
    enum ENUM_IPGNO_TYPE
    {
        ENUM_IPGNO_TYPE_6 = 6
    };
    static void genIPGNo(ENUM_IPGNO_TYPE type, unsigned int characNo, char* out);
};
}

class CInventory
{
public:
    bool use_money(int amount, eMoneySubReason reason, bool bLog);
    int gain_money(int amount, eMoneyAddReason reason, bool bLog, int param);
    int insertItemIntoInventory(Inven_Item item, eItemAddReason reason, bool bLog,
                                bool b);
    int RemoveKCItem();
    user_creature::CCreatureMgr* GetCreatureMgrW();
    const user_creature::CCreatureMgr* GetCreatureMgrR() const;
};

class CValueStatistic
{
public:
    void AddValueStatistic(VALUE_STATISTIC_FIELD field, CUser* user,
                           unsigned int value);
};

class Store
{
public:
    void GetSellItemPrice(Inven_Item& item, const CItem* pItem, short count, bool flag,
                          int& outPrice);
};

extern Store* G_Store();
extern CValueStatistic* GetInstanceValueStatistic();
char* NumberToString(unsigned int value, int radix);

// ---- GlobalData（CUserCharacInfo.h 已声明 namespace GlobalData；补缺失成员） ----
namespace GlobalData
{
extern CSystemTime s_systemTime_;
extern void* s_event_manager;
}

// ============================================================================
// 局部子对象（与 ORIG 布局一致；构造/析构经 asm-label 走真实/桩符号）
// ============================================================================
struct WR_map_monster
{
    int m_mobId;         // +0x00
    int m_instId;        // +0x04
    char m_roleType;     // +0x08
    char m_pad9[3];      // +0x09
    int m_mobIndex;      // +0x0c
    char m_pad10[4];     // +0x10
    float m_field14;     // +0x14
    float m_field18;     // +0x18
    char m_pad1c[8];     // +0x1c
    int m_field24;       // +0x24
    float m_field28;     // +0x28
    char m_pad2c[0x31c]; // +0x2c .. 0x348（ORIG ctor 访问至 +0x345）
};

struct WR_map_item
{
    char m_count;        // +0x00
    char m_pad1[3];      // +0x01
    short m_itemIndex;   // +0x04
    unsigned short m_dropIndex;  // +0x08
    int m_createTick;    // +0x0c
    Inven_Item m_item;   // +0x10（0x3d）
    char m_pad4d[3];     // +0x4d
    int m_field50;       // +0x50
};

// ============================================================================
// 依赖子对象 / 外部函数（asm-label extern；PvP_deps.cpp / 主 agent 提供桩）
// ============================================================================
extern "C" void sub_map_monster_C1(void* self) asm("_ZN11map_monsterC1Ev");
extern "C" void sub_map_monster_D1(void* self) asm("_ZN11map_monsterD1Ev");
extern "C" void sub_map_item_C1(void* self) asm("_ZN8map_itemC1Ev");
extern "C" void sub_map_item_D1(void* self) asm("_ZN8map_itemD1Ev");

extern "C" void sub_WarField_Reset(void* self) asm("_ZN8WarField5ResetEv");
extern "C" void sub_WarField_ConsistMap(void* self, void* guard, const void* dungeon,
                                        const void* map, int a, int b)
    asm("_ZN8WarField10ConsistMapEPvPK8CDungeonPK4CMaphi");
extern "C" int sub_WarField_DropItem(void* self, WR_map_item item)
    asm("_ZN8WarField8DropItemE8map_item");
extern "C" bool sub_WarField_GetFieldItem(void* self, int idx, void* item)
    asm("_ZN8WarField12GetFieldItemEiR8map_item");
extern "C" bool sub_WarField_HandleMonsterKill(void* self, int a, void* guard,
                                                        void* monster, void* user,
                                                        const void* dungeon)
    asm("_ZN8WarField17HandleMonsterKillEiR11PacketGuardR11map_monsterP5CUserPK8CDungeon");
extern "C" int sub_WarField_HandleWpPerMonster(void* self, void* user, int a, int b,
                                               void* monster)
    asm("_ZN8WarField18HandleWpPerMonsterEP5CUseriiR11map_monster");
extern "C" bool sub_WarField_IsGridClear(void* self)
    asm("_ZN8WarField11IsGridClearEv");
extern "C" int sub_WarField_CheckPickupItem(void* self, int a, int b, void* iter)
    asm("_ZN8WarField15CheckPickupItemEiiRSt17_Rb_tree_iteratorISt4pairIKi8map_itemEE");
extern "C" void sub_WarField_PickupItem(void* self, void* iter)
    asm("_ZN8WarField10PickupItemESt17_Rb_tree_iteratorISt4pairIKi8map_itemEE");

extern "C" void sub_map_monster_map_clear(void* self)
    asm("_ZNSt3mapIi11map_monsterSt4lessIiESaISt4pairIKiS0_EEE5clearEv");

// ---- CUser（CUser.h 未声明的方法 -> asm extern；返回类型按 ORIG 调用点） ----
extern "C" bool sub_CUser_CheckFatigue(void* self)
    asm("_ZN5CUser12CheckFatigueEv");
extern "C" bool sub_CUser_isGMUser(void* self) asm("_ZN5CUser8isGMUserEv");
extern "C" int sub_CUser_gainExpAsUpperMember(void* self, int v)
    asm("_ZN5CUser20gainExpAsUpperMemberEi");
extern "C" int sub_CUser_gainGuildSkillExp(void* self, int v)
    asm("_ZN5CUser17gainGuildSkillExpEi");
extern "C" bool sub_CUser_gain_exp_sp(void* self, int exp, int& sp, int& sfp,
                                      int reason, int a, bool b)
    asm("_ZN5CUser11gain_exp_spEiRiS0_13eExpAddReasonib");
extern "C" int sub_CUser_gainPowerWarRewardExp(void* self, int v)
    asm("_ZN5CUser21gainPowerWarRewardExpEi");
extern "C" bool sub_CUser_IsHavePremiumAdvantage(void* self)
    asm("_ZNK5CUser22IsHavePremiumAdvantageEv");
extern "C" void sub_CUser_saveTaxMoneyForUpperMember(void* self, int v)
    asm("_ZN5CUser26saveTaxMoneyForUpperMemberEi");
extern "C" void sub_CUser_CheckQuestMonster(void* self, int a, int b, int type)
    asm("_ZN5CUser17CheckQuestMonsterEii21ENUM_QUEST_ENEMY_TYPE");
extern "C" bool sub_CUser_isCompetitionMercenary(void* self)
    asm("_ZNK5CUser22isCompetitionMercenaryEv");
extern "C" void sub_CUser_FatigueUp(void* self, int v) asm("_ZN5CUser9FatigueUpEi");
extern "C" bool sub_CUser_IsEquipAvatar(void* self)
    asm("_ZN5CUser13IsEquipAvatarEv");

// ---- WongWork::CHackAnalyzer（CUser.h 仅声明类，无方法） ----
extern "C" void sub_CHackAnalyzer_reportHackInfo(void* self)
    asm("_ZN8WongWork13CHackAnalyzer14reportHackInfoEv");
extern "C" void sub_CHackAnalyzer_resetHackInfo(void* self)
    asm("_ZN8WongWork13CHackAnalyzer13resetHackInfoEv");
extern "C" void sub_CHackAnalyzer_beginCollectHackInfo(void* self, void* user)
    asm("_ZN8WongWork13CHackAnalyzer20beginCollectHackInfoEP5CUser");
extern "C" void sub_CHackAnalyzer_setLastMonsterDeadTime(void* self, int v)
    asm("_ZN8WongWork13CHackAnalyzer22setLastMonsterDeadTimeEl");

// ---- user_creature::CCreatureMgr（CInventory.h 最小声明无这些方法） ----
extern "C" bool sub_CCreatureMgr_IsEquippedCreature(void* self)
    asm("_ZNK13user_creature12CCreatureMgr18IsEquippedCreatureEv");
extern "C" bool sub_CCreatureMgr_IsGrowCreature_Equipped_Creature(void* self)
    asm("_ZNK13user_creature12CCreatureMgr32IsGrowCreature_Equipped_CreatureEv");
extern "C" void sub_CCreatureMgr_GainExp(void* self, int v)
    asm("_ZN13user_creature12CCreatureMgr7GainExpEi");

// ---- GameWorld / cUserHistoryLog ----
extern "C" void sub_GameWorld_out_from_warroom(void* world, void* user)
    asm("_ZN9GameWorld16out_from_warroomEP5CUser");
extern "C" TimerQueue* sub_G_TimerQueue() asm("_Z12G_TimerQueuev");
extern "C" void sub_cUserHistoryLog_EnterDungeon(void* self, const char* name, int level)
    asm("_ZN15cUserHistoryLog12EnterDungeonEPKci");
extern "C" void sub_cUserHistoryLog_LeaveDungeon(void* self, const char* name, int level)
    asm("_ZN15cUserHistoryLog12LeaveDungeonEPKci");

// ---- 事件 / 脚本 / 统计 / 商店 / 日志等 ----
extern "C" float sub_channel_script_t_getBonusRate(void* self, unsigned char a,
                                                   unsigned int b, unsigned int c)
    asm("_ZNK16channel_script_t12getBonusRateEhjj");
extern "C" void* sub_CEventManager_GetRepeatEvent(void* self, int v)
    asm("_ZN13CEventManager14GetRepeatEventEi");
extern "C" int sub_CExpDoubleEvent_GetExpFactor(void* self)
    asm("_ZN15CExpDoubleEvent12GetExpFactorEv");
extern "C" int sub_CBurningFatigueEvent_getBonusExpRate(void* self, unsigned short v)
    asm("_ZNK20CBurningFatigueEvent15getBonusExpRateEt");
extern "C" int sub_CServerEvent_GetExpRate() asm("_ZN12CServerEvent10GetExpRateEv");
extern "C" int sub_CUserPremium_GetAdvantageExpRate(void* self)
    asm("_ZNK8WongWork12CUserPremium19GetAdvantageExpRateEv");
extern "C" void sub_CValueStatistic_AddValueStatistic(void* self, int field, void* user,
                                                      unsigned v)
    asm("_ZN15CValueStatistic17AddValueStatisticE21VALUE_STATISTIC_FIELDP5CUserj");
extern "C" void* sub_GetInstanceValueStatistic() asm("_Z25GetInstanceValueStatisticv");
extern "C" void* sub_G_Store() asm("_Z7G_Storev");
extern "C" int sub_Store_GetSellItemPrice(void* self, void* item, const void* items,
                                          int count, int flag, int& out)
    asm("_ZN5Store16GetSellItemPriceER10Inven_ItemPK5CItemsbRi");
extern "C" void sub_DB_InsertRandomOptionItemInform_makeRequest(int uid, int acc,
                                                                unsigned char flag)
    asm("_ZN31DB_InsertRandomOptionItemInform11makeRequestEijh");
extern "C" void sub_HistoryLog_WriteUseCoin(void* self, void* file, const char* name)
    asm("_ZN10HistoryLog12WriteUseCoinEP8_IO_FILEPKc");
extern "C" void sub_CDungeonEntranceLog_IncrementDungeonEntrance(void* self, int idx,
                                                                 bool flag)
    asm("_ZN19CDungeonEntranceLog24IncrementDungeonEntranceEib");
extern "C" void* sub_GetInstanceDungeonEntranceLog() asm("_Z29GetInstanceDungeonEntranceLogv");
extern "C" Inven_Item sub_CCargo_get_cargo_slot(void* self, int slot)
    asm("_ZNK6CCargo14get_cargo_slotEi");
extern "C" bool sub_CCargo_delete_item(void* self, int slot, int count, int reason)
    asm("_ZN6CCargo11delete_itemEii14eItemDelReason");
extern "C" bool sub_CInventory_delete_item(void* self, int type, int slot,
                                                    int count, int reason, bool b)
    asm("_ZN10CInventory11delete_itemE10INVEN_TYPEii14eItemDelReasonb");
extern "C" Inven_Item sub_CInventory_GetInvenSlot(void* self, int type, int slot)
    asm("_ZNK10CInventory12GetInvenSlotEii");
extern "C" void sub_UpgradeSeparateInfo_SetUpgradeSeparate(void* self, unsigned char v)
    asm("_ZN19UpgradeSeparateInfo18SetUpgradeSeparateEh");

// ============================================================================
// 构造 / 析构
// ============================================================================
WarRoom::WarRoom()
{
    SetState((WARROOM_STATE)-1);
    for (int i = 0; i <= 5; ++i)
        m_slots[i] = 0;
}

WarRoom::~WarRoom()
{
}

// ============================================================================
// 基础访问器（ORIG weak：GetIndex 0x822ee8a / GetState 0x822ee96 /
//   SetState 0x82a5a56 / GenTimerKey 0x86c0b24 / CheckTimerKey 0x863bd92）
// ============================================================================
int WarRoom::GetIndex()
{
    return m_index;
}

int WarRoom::GetState()
{
    return m_state;
}

void WarRoom::SetState(WARROOM_STATE state)
{
    m_state = state;
}

unsigned int WarRoom::GenTimerKey(TIMER_MESSAGE msg)
{
    return ++m_timerKey[msg - 0x3f];
}

bool WarRoom::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return m_timerKey[msg - 0x3f] == key;
}

void WarRoom::CheckState()
{
    if (GetWaiterCount() == 0)
    {
        ChangeState(WARROOM_STATE_0);
    }
    else if ((GetWaiterCount() < 3) && (GetState() == 1))
    {
        ChangeState(WARROOM_STATE_0);
    }
}

void WarRoom::ChangeState(WARROOM_STATE state)
{
    if (m_state != state)
    {
        OnLeaveState(state);
        m_state = state;
        OnEnterState();
    }
}

int WarRoom::GetLiveCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_pad184[i] != 0))
            ++count;
    }
    return count;
}

int WarRoom::GetRemainSeat() const
{
    return 6 - GetWaiterCount();
}

int WarRoom::GetUserSlot(CUser* user)
{
    for (int i = 0; i <= 5; ++i)
    {
        if ((CUser*)m_slots[i] == user)
            return i;
    }
    return -1;
}

int WarRoom::GetWaiterCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            ++count;
    }
    return count;
}

void WarRoom::SetIndex(int idx)
{
    m_index = idx;
    Reset();
}

void WarRoom::CurSpawnStepReset()
{
    m_curSpawnStep = 0;
}

int WarRoom::Create(CUser* user, char* name, short idx)
{
    m_state = 0;
    m_slots[0] = (int)name;
    m_slotState[0] = 1;
    user->SetWarRoomIndex((short)m_index);
    return 0;
}

void WarRoom::AddBanList(CUser* user)
{
    if (sub_CUser_isGMUser(user) != 0)
        return;
    unsigned int id = user->get_acc_id();
    m_map[id] = GlobalData::s_systemTime_.getCurSec();
}

int WarRoom::IsBanUser(CUser* user)
{
    unsigned int id = user->get_acc_id();
    return m_map.find(id) != m_map.end();
}

void WarRoom::UpdateBanList()
{
    int cur = GlobalData::s_systemTime_.getCurSec();
    for (std::map<unsigned int, int>::iterator it = m_map.begin(); it != m_map.end(); )
    {
        int t = it->second;
        int diff = cur - t;
        if (diff < *(int*)((char*)G_CDataManager() + 0x88a0) / 1000)
        {
            ++it;
        }
        else
        {
            m_map.erase(it++);
        }
    }
}

void WarRoom::BattleReset()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            sub_CHackAnalyzer_reportHackInfo(((CUser*)m_slots[i])->getHackAnalyzer());
            sub_CHackAnalyzer_resetHackInfo(((CUser*)m_slots[i])->getHackAnalyzer());
            ((CUser*)m_slots[i])->ResetCoinCount();
        }
        m_pad184[i] = 0;
        m_slotData[i] = 0;
        ((int*)m_pad1a4)[i] = 0;
        ((int*)m_pad14)[i] = -1;
    }
    sub_WarField_Reset((char*)&m_field);
    memset(&m_pad1a4[0x18], 0, 0x10);
    m_userCount = 0;
    m_flag10 = 0;
    m_flag11 = 0;
    m_flag12 = 0;
    CurSpawnStepReset();
}

void WarRoom::Reset()
{
    BattleReset();
    m_field8 = 0;
    m_levelBand = 2;
    m_field1 = 0;
    m_field30 = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_slots[i] = 0;
        m_slotState[i + 1] = 0xff;
        m_slotData[i] = 0;
    }
}

void WarRoom::InvalidUserCheck()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            if (((CUser*)m_slots[i])->getCurCharacR() == 0)
            {
                short idx = ((CUser*)m_slots[i])->GetWarRoomIndex();
                cMyTrace trace("void WarRoom::InvalidUserCheck()", 0x236, 5);
                trace("WarRoom::InvalidUserCheck slot(%d) warRoomIndex(%d)", i, (int)idx);
                m_slotState[i + 1] = 0xff;
                m_slots[i] = 0;
                m_pad184[i] = 0;
            }
        }
    }
}

int WarRoom::IsJoinable(CUser* user)
{
    int st = GetState();
    if ((st == 4) || (st == 3) || (st == -1) || (st == 2))
        return 0x13;
    if (sub_CUser_isCompetitionMercenary(user) != 0)
        return 0x15;
    if (user->getCurCharacMoney() <
        *(int*)((char*)G_CDataManager() + 4 + (m_levelBand + 0x221c) * 4))
        return 10;
    if (sub_CUser_CheckFatigue(user) == 1)
    {
        int level = user->get_charac_level();
        switch (m_levelBand)
        {
        case 2:
            if ((level < 0x14) || (0x1d < level))
                return 0xe;
            break;
        case 3:
            if ((level < 0x1e) || (0x27 < level))
                return 0xe;
            break;
        case 4:
            if ((level < 0x28) || (0x31 < level))
                return 0xe;
            break;
        case 5:
            if ((level < 0x32) || (0x3b < level))
                return 0xe;
            break;
        case 6:
            if ((level < 0x3c) || (0x46 < level))
                return 0xe;
            break;
        default:
            return 0xe;
        }
        UpdateBanList();
        if (IsBanUser(user) != 0)
            return 0x1e;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] == 0) && (m_slotState[i + 1] == 0xff))
                return 0;
        }
        return 4;
    }
    return 0x43;
}

int WarRoom::WalkOutUserBySlot(int slot)
{
    if (m_slots[slot] == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp",
                              "int WarRoom::WalkOutUserBySlot(int)", 0x5cc,
                              "WarRoom::WalkOutUserBySlot no slot(%d)", slot);
        return 4;
    }
    CUser* user = (CUser*)m_slots[slot];
    sub_CHackAnalyzer_reportHackInfo(user->getHackAnalyzer());
    sub_CHackAnalyzer_resetHackInfo(user->getHackAnalyzer());
    m_slotState[slot + 1] = 0xff;
    sub_GameWorld_out_from_warroom(G_GameWorld(), user);
    if (m_pDungeon != 0)
    {
        sub_cUserHistoryLog_LeaveDungeon((char*)user + 0x79700,
                                         m_pDungeon->GetDungeonName(),
                                         (unsigned char)m_levelBand);
    }
    m_slots[slot] = 0;
    m_pad184[slot] = 0;
    PacketGuard packet;
    MakeSlotInfo((char*)&packet, slot);
    G_GameWorld()->send_all(packet);
    return 0;
}

int WarRoom::WalkOutUser(CUser* user)
{
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    return WalkOutUserBySlot(slot);
}

int WarRoom::Destroy(int idx)
{
    int user = m_slots[idx];
    if (user == 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                user = m_slots[i];
                break;
            }
        }
    }
    Reset();
    return user;
}

void WarRoom::Open()
{
    SetState(WARROOM_STATE_0);
}

void WarRoom::Close()
{
    SetState((WARROOM_STATE)-1);
}

int WarRoom::ShutDown()
{
    if (m_state < 1)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                CUser* u = (CUser*)m_slots[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)7);
            }
        }
        Close();
        return 1;
    }
    m_field30 = 1;
    return 0;
}

void WarRoom::Prepare()
{
    int dungeonIdx =
        *(int*)((char*)G_CDataManager() + 4 + (m_levelBand + 0x2228) * 4);
    void* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp", "void WarRoom::Prepare()", 0x66e,
                              "WARROOM G_CDataManager()->find_dungeon( %d) fail", 0);
    }
    else
    {
        m_pDungeon = (CDungeon*)dungeon;
        int mapIdx = *(int*)((char*)m_pDungeon + 0x704);
        void* map = G_CDataManager()->find_map(mapIdx);
        if (map == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", "void WarRoom::Prepare()", 0x678,
                                  "WARROOM Find Map[%d] Error",
                                  *(int*)((char*)m_pDungeon + 0x704));
        }
        else
        {
            m_fp = (int)map;
        }
    }
}

void WarRoom::Start()
{
    PacketGuard packet;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            packet.clear();
            packet.put_header(0, 0x5a);
            packet.finalize(true);
            ((CUser*)m_slots[i])->Send(packet);
            if (m_pDungeon != 0)
            {
                sub_CDungeonEntranceLog_IncrementDungeonEntrance(
                    sub_GetInstanceDungeonEntranceLog(), m_pDungeon->get_index(), false);
            }
        }
    }
    ChangeState(WARROOM_STATE_1);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            CUser* u = (CUser*)m_slots[i];
            sub_CHackAnalyzer_beginCollectHackInfo(u->getHackAnalyzer(), u);
        }
    }
}

void WarRoom::ReviveAll()
{
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_pad184[i] == 0))
        {
            SetCharacterLive(i, (CUser*)m_slots[i], true, false, 0, 0xff);
        }
    }
}

void WarRoom::RemoveKC()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            ((CUser*)m_slots[i])->InitFinishPoint();
            ((CInventory*)((CUser*)m_slots[i])->getCurCharacInvenW())->RemoveKCItem();
        }
    }
}

void WarRoom::CancelAllTimer()
{
    int dummy;
    for (int i = 0x40; i <= 0x47; ++i)
        GenTimerKey((TIMER_MESSAGE)i);
}

void WarRoom::SkipLastRankResultTimer()
{
}

int WarRoom::SendWalkOut(CUser* user, ENUM_WALKOUT_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 10);
    packet.put_byte(0);
    packet.put_byte((int)type);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

void WarRoom::SendWarRoomFailMsg()
{
    unsigned int key = GenTimerKey((TIMER_MESSAGE)0x46);
    sub_G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                    (TIMER_MESSAGE)0x46, 10, key, 0);
}

void WarRoom::WarRoomFail()
{
    if (GetLiveCount() == 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                CUser* u = (CUser*)m_slots[i];
                AddBanList(u);
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)4);
            }
        }
        ChangeState(WARROOM_STATE_0);
    }
}

int WarRoom::HostChangeWarRoom(CUser* user, char a, unsigned long b)
{
    if ((m_slots[(int)a] != 0) && ((CUser*)m_slots[(int)a] == user))
    {
        ((unsigned long*)m_pad14)[a] = b;
        if (m_userCount == 0)
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x47);
            sub_G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x47, 5, key, 0);
        }
        m_userCount += 1;
    }
    return 0;
}

void WarRoom::CheckHostChange()
{
    if (m_userCount > 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x160);
        unsigned int bestIdx = 0xffffffff;   // -0x18
        int bestSlot = -1;                   // -0x14
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (((int*)m_pad14)[i] != -1) &&
                ((unsigned int)((int*)m_pad14)[i] <= bestIdx))
            {
                bestIdx = ((unsigned int*)m_pad14)[i];
                bestSlot = i;
            }
        }
        if (bestSlot != -1)
        {
            packet.put_byte(bestSlot);
            packet.finalize(true);
            SendToRoom(packet);
        }
        else
        {
            goto end;
        }
    }
    for (int i = 0; i <= 5; ++i)
        ((int*)m_pad14)[i] = -1;
    m_userCount = 0;
end:
    ;
}

void WarRoom::HandleTimerTimeBomb()
{
    sub_map_monster_map_clear((char*)&m_field + 0x14);
    m_flag10 = 1;
    if (*(int*)((char*)&m_field + 0xf4) == *(int*)((char*)m_pDungeon + 0x744))
        ChangeState(WARROOM_STATE_5);
    else
        ChangeState(WARROOM_STATE_4);
}

void WarRoom::HandleTimerResultCount()
{
    ReviveAll();
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_slotData[i] != 0))
        {
            CUser* u = (CUser*)m_slots[i];
            AddBanList(u);
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)m_slotData[i]);
        }
    }
    BattleReset();
    CheckFatuigue();
    if (GetWaiterCount() < 3)
        ChangeState(WARROOM_STATE_0);
    else
        ChangeState(WARROOM_STATE_1);
}

void WarRoom::SendStatePacket()
{
    PacketGuard packet;
    packet.put_header(0, 0x58);
    packet.put_short(1);
    MakeStateInfo((char*)&packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

void WarRoom::MakeStateInfo(char* buf)
{
    InterfacePacketBuf* p = (InterfacePacketBuf*)buf;
    p->put_short(m_index);
    p->put_byte(m_state);
    p->put_short(m_curSpawnStep);
}

void WarRoom::MakeRoomInfo(char* buf)
{
    ((InterfacePacketBuf*)buf)->put_short(m_index);
    ((InterfacePacketBuf*)buf)->put_byte((unsigned char)m_field8);
    ((InterfacePacketBuf*)buf)->put_byte((unsigned char)m_levelBand);
    ((InterfacePacketBuf*)buf)->put_byte((unsigned char)m_field1);
    ((InterfacePacketBuf*)buf)->put_byte(m_state);
    if (m_pDungeon == 0)
        ((InterfacePacketBuf*)buf)->put_short(0);
    else
        ((InterfacePacketBuf*)buf)->put_short(*(int*)((char*)m_pDungeon + 0x704));
    for (int i = 0; i <= 5; ++i)
    {
        ((InterfacePacketBuf*)buf)->put_byte(m_slotState[i + 1]);
        if (m_slots[i] == 0)
            ((InterfacePacketBuf*)buf)->put_short(0xffff);
        else
            ((InterfacePacketBuf*)buf)->put_short(((CUser*)m_slots[i])->get_unique_id());
    }
}

void WarRoom::MakeSlotInfo(char* buf, int seat)
{
    ((InterfacePacketBuf*)buf)->put_header(0, 0x59);
    ((InterfacePacketBuf*)buf)->put_short(m_index);
    if (seat == 0xffff)
    {
        int count = GetWaiterCount();
        ((InterfacePacketBuf*)buf)->put_byte(count);
        if (count == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", "void WarRoom::MakeSlotInfo(char*, int)",
                                  0x793, "Send Member 0 PvpRoom %d State(%d)", m_index,
                                  m_state);
        }
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                ((InterfacePacketBuf*)buf)->put_byte(i);
                ((InterfacePacketBuf*)buf)->put_byte(m_slotState[i + 1]);
                ((InterfacePacketBuf*)buf)->put_short(((CUser*)m_slots[i])->get_unique_id());
            }
        }
    }
    else
    {
        ((InterfacePacketBuf*)buf)->put_byte(1);
        ((InterfacePacketBuf*)buf)->put_byte(seat);
        ((InterfacePacketBuf*)buf)->put_byte(m_slotState[seat + 1]);
        if (m_slots[seat] == 0)
            ((InterfacePacketBuf*)buf)->put_short(0xffff);
        else
            ((InterfacePacketBuf*)buf)->put_short(((CUser*)m_slots[seat])->get_unique_id());
    }
    ((InterfacePacketBuf*)buf)->finalize(true);
}

void WarRoom::SendToRoom(PacketGuard& guard)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            ((CUser*)m_slots[i])->Send(guard);
    }
}

// ============================================================================
// 状态机
// ============================================================================
void WarRoom::OnLeaveState(WARROOM_STATE state)
{
    if (state == WARROOM_STATE_0)
        CurSpawnStepReset();
    if ((m_state != 0) && (m_state == 2) && (state == 3))
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                int money = ((CUser*)m_slots[i])->getCurCharacMoney();
                int min = std::min(
                    *(int*)((char*)G_CDataManager() + 4 + (m_levelBand + 0x221c) * 4),
                    money);
                ((CInventory*)((CUser*)m_slots[i])->getCurCharacInvenW())
                    ->use_money(min, (eMoneySubReason)0xf, 1);
                if (((CUser*)m_slots[i])->GetFinishPointTotal() > 0)
                {
                    cMyTrace trace("void WarRoom::OnLeaveState(WARROOM_STATE)", 0x55b, 5);
                    trace("%d", ((CUser*)m_slots[i])->GetFinishPointTotal());
                    ((CUser*)m_slots[i])->InitFinishPoint();
                }
            }
        }
    }
}

void WarRoom::OnEnterState()
{
    CancelAllTimer();
    InvalidUserCheck();
    SendStatePacket();
    switch (m_state)
    {
    case 0:
        UpdateBanList();
        if (m_field30 != 0)
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                    WalkOutUserBySlot(i);
            }
            Close();
            m_field30 = 0;
            PacketGuard packet;
            packet.put_header(0, 0x58);
            packet.put_short(1);
            MakeStateInfo((char*)&packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        break;
    case 1:
        m_fpBase = GlobalData::s_systemTime_.getCurSec();
        BattleReset();
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] == 0)
                m_pad184[i] = 0;
            else
                m_pad184[i] = 1;
        }
        if (GetWaiterCount() < 3)
        {
            bool hasGM = false;
            for (int i = 0; i <= 5; ++i)
            {
                if ((m_slots[i] != 0) && (sub_CUser_isGMUser((void*)m_slots[i]) != 0))
                {
                    hasGM = true;
                    break;
                }
            }
            if (!hasGM)
            {
                ChangeState(WARROOM_STATE_0);
                return;
            }
        }
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x41);
            int ms = *(int*)((char*)G_CDataManager() + 0x88a8);
            sub_G_TimerQueue()->InsertTimerInMilisecond(
                (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x41, ms, key, 0);
        }
        break;
    case 2:
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x42);
            sub_G_TimerQueue()->InsertTimerInMilisecond(
                (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x42, 11000, key, 0);
            m_curSpawnStep = 1;
        }
        break;
    case 3:
        m_flag10 = 0;
        m_flag11 = 0;
        {
            PacketGuard packet;
            packet.clear();
            int count = GetWaiterCount();
            sub_WarField_ConsistMap((char*)&m_field, &packet, m_pDungeon,
                                    (void*)m_fp, (m_levelBand + 1) * 10, count);
            SendToRoom(packet);
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    sub_CUser_FatigueUp((void*)m_slots[i],
                                        *(int*)((char*)m_pDungeon + 0x7e8));
                }
            }
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x45);
            int ms = *(int*)((char*)m_pDungeon + 0x7f0);
            sub_G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x45, ms / 1000, key, 0);
        }
        break;
    case 4:
        ReviveAll();
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x43);
            sub_G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x43, 10, key, 0);
        }
        if (m_curSpawnStep != *(int*)((char*)m_pDungeon + 0x744))
            m_curSpawnStep += 1;
        break;
    case 5:
        if ((m_flag10 != 0) && (*(int*)((char*)&m_field + 0xf4) ==
                                *(int*)((char*)m_pDungeon + 0x744)))
        {
            PacketGuard packet;
            packet.put_header(0, 0x156);
            packet.put_byte(GetWaiterCount());
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                    int dm = *(int*)((char*)G_CDataManager() + 0x88ec);
                    if (dm < fp)
                    {
                        ((CUser*)m_slots[i])->DecreaseWarPoint(dm);
                    }
                    else
                    {
                        ((CUser*)m_slots[i])->DecreaseWarPoint(0);
                    }
                    packet.put_byte(i);
                    packet.put_int(((CUser*)m_slots[i])->GetFinishPointTotal());
                }
            }
            packet.finalize(true);
            SendToRoom(packet);
        }
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (((CUser*)m_slots[i])->GetFinishPointTotal() == 0))
                m_slotData[i] = 6;
        }
        ClearReward();
        SendGuildFP();
        RemoveKC();
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x44);
            sub_G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x44, 0x18, key, 0);
        }
        break;
    default:
        break;
    }
}

void WarRoom::CheckFatuigue()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            if (sub_CUser_CheckFatigue((void*)m_slots[i]) != 1)
            {
                CUser* u = (CUser*)m_slots[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)5);
            }
            else
            {
                int money = ((CUser*)m_slots[i])->getCurCharacMoney();
                CDataManager* mgr = G_CDataManager();
                int need =
                    *(int*)((char*)mgr + 4 + (m_levelBand + 0x221c) * 4);
                if (money < need)
                {
                    CUser* u = (CUser*)m_slots[i];
                    WalkOutUserBySlot(i);
                    SendWalkOut(u, (ENUM_WALKOUT_TYPE)8);
                }
            }
        }
    }
}

void WarRoom::CalcGuildFP()
{
    char local_4c[12];
    int local_40[6] = {0};
    int local_28;
    int local_24;
    int local_20;
    int local_1c;
    int local_18;
    int local_14;
    int local_10;
    local_4c[6] = 0;
    local_4c[7] = 0;
    local_4c[8] = 0;
    local_4c[9] = 0;
    local_4c[10] = 0;
    local_4c[11] = 0;
    for (local_28 = 0; local_28 <= 5; ++local_28)
    {
        if (m_slots[local_28] != 0)
        {
            local_40[local_28] =
                ((CUser*)m_slots[local_28])->get_charac_guildkey();
            ((int*)m_pad1a4)[local_28] =
                ((CUser*)m_slots[local_28])->GetFinishPointTotal();
            if (local_40[local_28] == 0)
                local_4c[local_28 + 6] = 1;
        }
    }
    for (local_24 = 0; local_24 <= 5; ++local_24)
    {
        if ((m_slots[local_24] != 0) && (local_4c[local_24 + 6] == 0))
        {
            local_20 = ((int*)m_pad1a4)[local_24];
            local_1c = 1;
            local_4c[local_24 + 6] = 1;
            local_4c[0] = 0;
            local_4c[1] = 0;
            local_4c[2] = 0;
            local_4c[3] = 0;
            local_4c[4] = 0;
            local_4c[5] = 0;
            local_4c[local_24] = 1;
            for (local_14 = local_24 + 1; local_14 <= 5; ++local_14)
            {
                if ((m_slots[local_14] != 0) && (local_4c[local_14 + 6] == 0) &&
                    (local_40[local_24] == local_40[local_14]))
                {
                    local_20 += ((int*)m_pad1a4)[local_14];
                    local_1c += 1;
                    local_4c[local_14 + 6] = 1;
                    local_4c[local_14] = 1;
                }
            }
            if (local_1c > 1)
            {
                local_18 = local_20 / local_1c;
                for (local_10 = 0; local_10 <= 5; ++local_10)
                {
                    if ((m_slots[local_10] != 0) && (local_4c[local_10] != 0))
                        ((int*)m_pad1a4)[local_10] = local_20 / local_1c;
                }
            }
        }
    }
}

void WarRoom::SendGuildFP()
{
    PacketGuard packet;
    packet.put_header(0, 0x60);
    packet.put_byte(GetWaiterCount());
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            if (((CUser*)m_slots[i])->GetFinishPointTotal() <
                *(int*)((char*)G_CDataManager() + 0x88a4))
            {
                m_slotData[i] = 9;
            }
        }
    }
    if (GetWaiterCount() > 4)
    {
        int minFp = 0x7fffffff;
        std::vector<int> v;
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                if (fp < minFp)
                {
                    minFp = fp;
                    v.clear();
                    v.push_back(i);
                }
                else if (fp == minFp)
                {
                    v.push_back(i);
                }
            }
        }
        for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
            m_slotData[*it] = 10;
        }
    }
    char local_3a[6] = {0};
    int count = 0;
    while (count < GetWaiterCount())
    {
        int bestSlot = -1;
        int bestFp = -1;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (local_3a[i] == 0) &&
                (bestFp <= ((CUser*)m_slots[i])->GetFinishPointTotal()))
            {
                bestFp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                bestSlot = i;
            }
        }
        if (bestSlot == -1)
        {
            LogManager::logFormat(1, "WarRoom.cpp", "void WarRoom::SendGuildFP()", 0x430,
                                  "maxPlayer (%d)", 0xffffffff);
        }
        local_3a[bestSlot] = 1;
        packet.put_byte(bestSlot);
        packet.put_byte(m_slotData[bestSlot]);
        ++count;
    }
    packet.finalize(true);
    SendToRoom(packet);
}

void WarRoom::ClearReward()
{
    int totalExp = 0;
    for (int i = 0; i <= 3; ++i)
    {
        totalExp += (int)((float)*(int*)((char*)m_pDungeon + 0x10 + (i + 0x1e0) * 4) *
                          ((float*)&m_pad1a4[0x18])[i]);
    }
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            PacketGuard packet1;
            PacketGuard packet2;
            int sp = 0;
            int sfp = 0;
            float rate = *(float*)((char*)m_pDungeon + 0x7a0);
            int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
            int baseExp = (int)((float)totalExp * ((float)fp * rate + 1.0f));
            float avatarRate = (sub_CUser_IsEquipAvatar((void*)m_slots[i]) != 0) ? 0.02f : 0.0f;
            float creatureRate =
                (sub_CCreatureMgr_IsEquippedCreature(
                     (void*)((const CInventory*)((CUser*)m_slots[i])
                                 ->getCurCharacInvenR())
                         ->GetCreatureMgrR()) != 0)
                    ? 0.05f
                    : 0.0f;
            int expDouble = 0;
            int channelExp = 0;
            int serverExp = 0;
            int burningExp = 0;
            int avatarExp = 0;
            int creatureExp = 0;
            float expFactor = 0.0f;
            void* evt = sub_CEventManager_GetRepeatEvent(GlobalData::s_event_manager, 3);
            if ((*(unsigned char (**)(void*, int))(*(char**)evt + 0x34))(evt, 0) != 0)
            {
                int f = sub_CExpDoubleEvent_GetExpFactor(
                    sub_CEventManager_GetRepeatEvent(GlobalData::s_event_manager, 3));
                expFactor = (float)f / 100.0f;
            }
            void* channel = G_CDataManager()->GetChannelScript();
            float bonus = sub_channel_script_t_getBonusRate(
                channel, *(unsigned char*)((char*)G_CEnvironment() + 0x378),
                (unsigned int)G_CEnvironment()->get_channel_no(),
                (unsigned int)m_pDungeon->get_index());
            channelExp = (int)((float)baseExp * bonus);
            serverExp = (int)((float)baseExp *
                              ((0.0f + (float)sub_CServerEvent_GetExpRate()) / 100.0f));
            if (avatarRate != 0.0f)
            {
                avatarExp = std::max((unsigned int)((float)baseExp * avatarRate), 1u);
            }
            burningExp = (int)((float)baseExp * expFactor);
            if (creatureRate != 0.0f)
            {
                creatureExp = std::max((unsigned int)((float)baseExp * creatureRate), 1u);
            }
            if (sub_CUser_IsHavePremiumAdvantage((void*)m_slots[i]) != 0)
            {
                float premiumRate =
                    (float)sub_CUserPremium_GetAdvantageExpRate(
                        ((CUser*)m_slots[i])->GetPremiumInfo()) /
                    100.0f;
                burningExp += (int)((float)baseExp * premiumRate);
            }
            int burningBonus = 0;
            void* burning =
                sub_CEventManager_GetRepeatEvent(GlobalData::s_event_manager, 0xf);
            if ((*(unsigned char (**)(void*, int))(*(char**)burning + 0x34))(burning, 0) !=
                0)
            {
                int rate2 = sub_CBurningFatigueEvent_getBonusExpRate(
                    burning, ((CUser*)m_slots[i])->getCurCharacUsedFatigue());
                if (rate2 != 0)
                    burningBonus = (rate2 * baseExp) / 100;
            }
            int total = avatarExp + baseExp + burningExp + serverExp + channelExp + burningBonus +
                        creatureExp;
            int upperExp = sub_CUser_gainExpAsUpperMember((void*)m_slots[i], baseExp);
            total += upperExp;
            int guildExp = sub_CUser_gainGuildSkillExp((void*)m_slots[i], baseExp);
            total += guildExp;
            if (sub_CUser_gain_exp_sp((void*)m_slots[i], total, sp, sfp, 6, 0, 0) != 0)
            {
                packet2.clear();
                packet2.put_header(0, 2);
                packet2.put_byte(1);
                packet2.put_short(1);
                ((CUser*)m_slots[i])->make_basic_info((char*)&packet2, 1);
                packet2.finalize(true);
                ((CUser*)m_slots[i])->Send(packet2);
            }
            packet1.clear();
            packet1.put_header(0, 0x5c);
            packet1.put_byte(sp);
            packet1.put_byte(sfp);
            packet1.put_int(((CUser*)m_slots[i])->GetFinishPointTotal());
            packet1.put_int(baseExp + serverExp + burningBonus);
            packet1.put_int(avatarExp);
            packet1.put_int(burningExp);
            packet1.put_int(channelExp);
            packet1.put_int(upperExp);
            packet1.put_int(creatureExp);
            packet1.finalize(true);
            ((CUser*)m_slots[i])->Send(packet1);
            int cur = GlobalData::s_systemTime_.getCurSec();
            sub_CHackAnalyzer_setLastMonsterDeadTime(
                ((CUser*)m_slots[i])->getHackAnalyzer(), cur);
        }
    }
}

int WarRoom::Join(CUser* user, int& seat)
{
    int count = GetWaiterCount();
    PacketGuard packet1;
    PacketGuard packet2;
    packet1.put_header(0, 2);
    packet2.put_header(0, 0xb);
    packet1.put_byte(1);
    packet1.put_short(count);
    packet2.put_byte(count);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            ((CUser*)m_slots[i])->make_basic_info((char*)&packet1, 1);
            packet2.put_short(((CUser*)m_slots[i])->get_unique_id());
            packet2.put_int(((CUser*)m_slots[i])->m_network.get_inner_ip());
            packet2.put_int(((CUser*)m_slots[i])->m_network.get_ip());
            packet2.put_short(((CUser*)m_slots[i])->m_network.get_port());
            packet2.put_int(((CUser*)m_slots[i])->get_acc_id());
            packet2.put_byte(((CUser*)m_slots[i])->m_network.get_nat_type());
            packet2.put_int(((CUser*)m_slots[i])->m_network.get_mtu());
            if (((CUser*)m_slots[i])->getCurCharacR() == 0)
            {
                packet2.put_byte(0);
            }
            else
            {
                packet2.put_byte(
                    *(unsigned char*)((char*)((CUser*)m_slots[i])->getCurCharacR() + 0x75));
            }
        }
    }
    packet1.finalize(true);
    packet2.finalize(true);
    int slot = -1;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] == 0) && (m_slotState[i + 1] == 0xff))
        {
            slot = i;
            m_slots[i] = (int)user;
            user->SetWarRoomIndex((short)m_index);
            seat = i;
            break;
        }
    }
    if (slot == -1)
    {
        return 4;
    }
    user->Send(packet1);
    user->Send(packet2);
    if (m_pDungeon != 0)
    {
        sub_cUserHistoryLog_EnterDungeon((char*)user + 0x79700,
                                         m_pDungeon->GetDungeonName(),
                                         (unsigned char)m_levelBand);
    }
    m_slotState[slot + 1] = 0;
    m_pad184[slot] = 1;
    packet1.clear();
    packet1.put_header(0, 2);
    packet1.put_byte(1);
    packet1.put_short(1);
    user->make_basic_info((char*)&packet1, 1);
    packet1.finalize(true);
    SendToRoom(packet1);
    packet2.clear();
    packet2.put_header(0, 0xb);
    packet2.put_byte(1);
    packet2.put_short(user->get_unique_id());
    packet2.put_int(user->m_network.get_inner_ip());
    packet2.put_int(user->m_network.get_ip());
    packet2.put_short(user->m_network.get_port());
    packet2.put_int(user->get_acc_id());
    packet2.put_byte(user->m_network.get_nat_type());
    packet2.put_int(user->m_network.get_mtu());
    if (user->getCurCharacR() == 0)
    {
        packet2.put_byte(0);
    }
    else
    {
        packet2.put_byte(*(unsigned char*)((char*)user->getCurCharacR() + 0x75));
    }
    packet2.finalize(true);
    SendToRoom(packet2);
    packet1.clear();
    packet1.put_header(1, 0x5d);
    packet1.put_byte(1);
    packet1.put_int(user->getCurCharacMoney());
    packet1.put_short(GlobalData::s_systemTime_.getCurSec() - m_fpBase);
    packet1.finalize(true);
    user->Send(packet1);
    return 0;
}

void WarRoom::SetCharacterLive(int seat, CUser* user, bool live, bool a, short b, int c)
{
    m_pad184[seat] = live ? 1 : 0;
    PacketGuard packet;
    packet.put_header(0, 0x5f);
    packet.put_byte(GetUserSlot(user));
    packet.put_byte(live ? 1 : 0);
    packet.put_byte(a ? 1 : 0);
    if (((c == 0xff) || (c < 0)) || (c > 5) || (m_slots[c] == 0))
    {
        packet.put_short(0);
    }
    else if (m_flag12 == 0)
    {
        packet.put_short(*(int*)((char*)m_pDungeon + 0x7c8));
    }
    else
    {
        packet.put_short(0);
    }
    packet.put_short(b);
    packet.finalize(true);
    SendToRoom(packet);
}

int WarRoom::SetSlotState(CUser* user, int seat, ENUM_SEAT_STATE state, CUser** out,
                          bool& a, bool& b, bool& c)
{
    if ((user != 0) && (state == (ENUM_SEAT_STATE)0xff))
    {
        if (m_slots[seat] == 0)
        {
            cMyTrace trace(
                "int WarRoom::SetSlotState(CUser*, int, ENUM_SEAT_STATE, CUser**, bool&, bool&, bool&)",
                0x590, 5);
            trace("WARAREA  WarRoom::SetSlotState players_[pos] is NULL pos(%d)", seat);
        }
        else
        {
            *out = (CUser*)m_slots[seat];
            m_slotState[seat + 1] = 0xff;
            m_slots[seat] = 0;
            m_pad184[seat] = 0;
            a = true;
            if (GetState() > 2)
            {
                AddBanList(user);
            }
            sub_CHackAnalyzer_reportHackInfo(user->getHackAnalyzer());
            sub_CHackAnalyzer_resetHackInfo(user->getHackAnalyzer());
            if (m_pDungeon != 0)
            {
                sub_cUserHistoryLog_LeaveDungeon((char*)user + 0x79700,
                                                 m_pDungeon->GetDungeonName(),
                                                 (unsigned char)m_levelBand);
            }
        }
    }
    return 0;
}

void WarRoom::HandleSetSeatState(CUser* user, int seat, ENUM_SEAT_STATE state)
{
    PacketGuard packet;
    int ret = 0;
    CUser* outUser = 0;
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    ret = SetSlotState(user, seat, state, &outUser, b1, b2, b3);
    if (ret == 0)
    {
        if (outUser != 0)
        {
            sub_GameWorld_out_from_warroom(G_GameWorld(), outUser);
        }
        if (b1 != 0)
        {
            packet.clear();
            MakeSlotInfo((char*)&packet, seat);
            G_GameWorld()->send_all(packet);
        }
        if (b3 != 0)
            return;
    }
    if (ret > 0)
    {
        packet.put_header(1, 0x5e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
    }
}

int WarRoom::HandleDieCharacer(CUser* user, int killerSlot)
{
    int st = GetState();
    if ((st != 3) && (st != 4) && (m_state != 5))
        return 0x13;
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    if (m_pad184[slot] == 0)
        return 0x12;
    if ((killerSlot != 0xff) && ((killerSlot < 0) || (killerSlot > 5)))
    {
        cMyTrace trace("int WarRoom::HandleDieCharacer(CUser*, int)", 0x6d0, 5);
        trace("WarRoom::HandleDieCharacer killerSlotIndex Error(%d)", killerSlot);
        return 1;
    }
    short killerUid = -1;
    if (killerSlot != 0xff)
    {
        if (m_slots[killerSlot] == 0)
        {
            cMyTrace trace("int WarRoom::HandleDieCharacer(CUser*, int)", 0x6de, 5);
            trace("WARAREA WarRoom::HandleDieCharacer player_[] is NULL killerSlotIndex(%d)",
                  killerSlot);
            killerUid = -1;
        }
        else
        {
            killerUid = ((CUser*)m_slots[killerSlot])->get_unique_id();
        }
    }
    SetCharacterLive(slot, user, false, false, killerUid, killerSlot);
    if ((killerSlot != 0xff) && (killerSlot >= 0) && (killerSlot <= 5) &&
        (m_slots[killerSlot] != 0))
    {
        if (m_flag12 == 0)
        {
            ((CUser*)m_slots[killerSlot])->WarAreaKill(
                4, *(int*)((char*)m_pDungeon + 0x7c8), 1.0f);
        }
        else
        {
            ((CUser*)m_slots[killerSlot])->WarAreaKill(4, 0, 1.0f);
        }
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x5f);
        packet.put_byte(1);
        packet.put_byte(killerSlot);
        packet.finalize(true);
        SendToRoom(packet);
    }
    if (m_flag10 == 0)
    {
        if (GetLiveCount() == 0)
            SendWarRoomFailMsg();
    }
    else
    {
        ReviveAll();
        if ((*(int*)((char*)&m_field + 0xf4) == *(int*)((char*)m_pDungeon + 0x744)) ||
            (m_flag11 != 0))
        {
            return 0;
        }
        PacketGuard packet;
        packet.put_header(0, 0x156);
        packet.put_byte(GetWaiterCount());
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                if (*(int*)((char*)G_CDataManager() + 0x88ec) < fp)
                {
                    ((CUser*)m_slots[i])->DecreaseWarPoint(
                        *(int*)((char*)G_CDataManager() + 0x88ec));
                }
                else
                {
                    ((CUser*)m_slots[i])->DecreaseWarPoint(0);
                }
                packet.put_byte(i);
                packet.put_int(((CUser*)m_slots[i])->GetFinishPointTotal());
            }
        }
        packet.finalize(true);
        SendToRoom(packet);
        m_flag11 = 1;
    }
    return 0;
}

int WarRoom::HandleMonsterKill(CUser* user, int monsterIdx, CUser* killer)
{
    WR_map_monster monster;
    sub_map_monster_C1(&monster);
    PacketGuard packet;
    int result = 0;
    int count = GetWaiterCount();
    if (count != 0)
    {
        int wp = 0;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (killer != 0) && ((CUser*)m_slots[i] == killer))
            {
                wp = sub_WarField_HandleWpPerMonster((char*)&m_field, killer,
                                                     monsterIdx, 0, &monster);
            }
        }
        if (sub_WarField_HandleMonsterKill((char*)&m_field, monsterIdx, &packet,
                                           &monster, killer, m_pDungeon) != 1)
        {
            result = 0;
        }
        else
        {
            ((float*)&m_pad1a4[0x18])[monster.m_roleType] += monster.m_field28;
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    bool isKiller =
                        (killer != 0) && ((CUser*)m_slots[i] == killer);
                    if (isKiller)
                    {
                        killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                        sub_CUser_CheckQuestMonster(
                            killer, m_pDungeon->get_index(), monster.m_mobIndex, 1);
                    }
                    int exp = (int)((float)*(int*)((char*)m_pDungeon + 0x10 +
                                                   (monster.m_roleType + 0x1e0) * 4) *
                                    monster.m_field18);
                    if (isKiller)
                    {
                        exp = (int)(*(float*)((char*)G_CDataManager() + 0x88f0) *
                                    (float)exp);
                    }
                    ExpBonusPointResult bonus;
                    if (isKiller)
                    {
                        killer->checkBonusPoint();
                        killer->getExpBonusPoint(bonus);
                    }
                    int total = bonus.m_base + exp;
                    exp = (int)((float)bonus.m_add / 100.0f * (float)total) + total;
                    int upperExp =
                        sub_CUser_gainExpAsUpperMember((void*)m_slots[i], exp);
                    int guildExp =
                        sub_CUser_gainGuildSkillExp((void*)m_slots[i], exp);
                    int fatigueExp = 0;
                    if (((CUser*)m_slots[i])->getCurCharacFatigueGrownUpBuff() != 0)
                    {
                        fatigueExp = exp;
                        ((CUser*)m_slots[i])->set_charac_fatigue_buf_bonus_exp(exp);
                    }
                    int rewardExp =
                        sub_CUser_gainPowerWarRewardExp((void*)m_slots[i], exp);
                    int sp = 0;
                    int sfp = 0;
                    if (sub_CUser_gain_exp_sp((void*)m_slots[i],
                                              upperExp + exp + guildExp + rewardExp +
                                                  fatigueExp,
                                              sp, sfp, 4, monster.m_mobIndex, 0) != 0)
                    {
                        ((CUser*)m_slots[i])->SendNotiPacket(
                            (CUser::eSendTarget)3, (ENUM_NOTIPACKET)2, 1);
                    }
                }
            }
            short killerUid = -1;
            if (killer != 0)
                killerUid = killer->get_unique_id();
            packet.put_byte(1);
            packet.put_short(killerUid);
            packet.put_short(wp);
            packet.finalize(true);
            SendToRoom(packet);
            if (sub_WarField_IsGridClear((char*)&m_field) != 0)
            {
                for (int i = 0; i <= 5; ++i)
                {
                    if (m_slots[i] != 0)
                    {
                        CUser* u = (CUser*)m_slots[i];
                        if (*(int*)((char*)u + 0x8e08c) > 0)
                        {
                            if (sub_CCreatureMgr_IsGrowCreature_Equipped_Creature(
                                    (void*)((const CInventory*)u->getCurCharacInvenR())
                                        ->GetCreatureMgrR()) != 1)
                            {
                                sub_CCreatureMgr_GainExp(
                                    ((CInventory*)u->getCurCharacInvenW())
                                        ->GetCreatureMgrW(),
                                    *(int*)((char*)u + 0x8e08c));
                            }
                        }
                        *(int*)((char*)u + 0x8e08c) = 0;
                    }
                }
                GenTimerKey((TIMER_MESSAGE)0x45);
                if (*(int*)((char*)&m_field + 0xf4) == *(int*)((char*)m_pDungeon + 0x744))
                {
                    m_flag12 = 1;
                    ChangeState(WARROOM_STATE_5);
                }
                else
                {
                    ChangeState(WARROOM_STATE_4);
                }
            }
            result = 1;
        }
    }
    return result;
}

int WarRoom::HandleWpPerMonster(CUser* user, int a, int b, CUser* killer)
{
    WR_map_monster monster;
    sub_map_monster_C1(&monster);
    int result = 3;
    int count = GetWaiterCount();
    if (count != 0)
    {
        int wp = 0;
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                if ((killer == 0) || ((CUser*)m_slots[i] != killer))
                {
                    // 非击杀者：跳过
                }
                else
                {
                    wp = sub_WarField_HandleWpPerMonster((char*)&m_field, killer, a,
                                                         b, &monster);
                    killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                }
            }
        }
        short killerUid = -1;
        if (killer != 0)
            killerUid = killer->get_unique_id();
        PacketGuard packet;
        packet.put_header(0, 0x151);
        packet.put_int(a);
        packet.put_short(killerUid);
        packet.put_int(b);
        packet.put_short(wp);
        packet.finalize(true);
        SendToRoom(packet);
        result = 0;
    }
    sub_map_monster_D1(&monster);
    return result;
}

int WarRoom::HandleGetItem(CUser* user, int idx)
{
    char flag = 1;
    CUser* target = 0;
    WR_map_item fieldItem;
    sub_map_item_C1(&fieldItem);
    PacketGuard packet;
    int result = 0;
    bool ok = sub_WarField_GetFieldItem((char*)&m_field, idx, &fieldItem);
    if (ok != 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        result = 0;
    }
    else if (GetWaiterCount() < 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        result = 0;
    }
    else if (fieldItem.m_itemIndex == 0)
    {
        int money = fieldItem.m_item.get_add_info();
        packet.put_header(0, 0x5d);
        packet.put_short(idx);
        packet.put_short(user->get_unique_id());
        char iter[4] = {0};
        int check = sub_WarField_CheckPickupItem((char*)&m_field, idx,
                                                 user->get_unique_id(), iter);
        if (check < 1)
        {
            int gained = ((CInventory*)user->getCurCharacInvenW())
                             ->gain_money(money, (eMoneyAddReason)0xf, 1, 0);
            sub_CUser_saveTaxMoneyForUpperMember(user, money);
            if (gained < money)
            {
                if (gained < 0)
                    gained = 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, money, gained);
            }
            sub_WarField_PickupItem((char*)&m_field, iter);
            packet.put_int(gained);
            packet.finalize(true);
            SendToRoom(packet);
            result = 1;
        }
        else
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(check);
            packet.finalize(true);
            user->Send(packet);
            result = 0;
        }
    }
    else
    {
        int itemIdx = fieldItem.m_itemIndex;
        CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", "bool WarRoom::HandleGetItem(CUser*, int)",
                                  0x957, "G_CDataManager()->find_item(%d) fail", itemIdx);
            result = 0;
        }
        else
        {
            target = user;
            if (user == 0)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(0x16);
                packet.finalize(true);
                SendToRoom(packet);
                result = 0;
            }
            else
            {
                bool randomOption = false;
                if ((*(unsigned char (**)(void*))(*(char**)item + 0x4c))(item) != 0)
                {
                    if (user->getCheckPickUpRandomOptionItem() == 1)
                        randomOption = false;
                    else
                        randomOption = true;
                }
                if (randomOption)
                {
                    user->setCheckPickUpRandomOptionItem(true);
                    sub_DB_InsertRandomOptionItemInform_makeRequest(user->GetUID(),
                                                                    user->get_acc_id(), 1);
                    PacketGuard notice;
                    notice.clear();
                    notice.put_header(0, 0x171);
                    notice.put_short(user->get_unique_id());
                    notice.put_byte(1);
                    notice.finalize(true);
                    user->Send(notice);
                }
                char iter[4] = {0};
                int check = sub_WarField_CheckPickupItem((char*)&m_field, idx,
                                                         user->get_unique_id(), iter);
                if (check < 1)
                {
                    int added = -1;
                    if (fieldItem.m_item.isAvatarItemType() == 0)
                    {
                        added = ((CInventory*)user->getCurCharacInvenW())
                                    ->insertItemIntoInventory(fieldItem.m_item,
                                                              (eItemAddReason)0x16, 1,
                                                              1);
                    }
                    else
                    {
                        char ipg[24];
                        WongWork::CGenUniqueNo::genIPGNo(
                            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                            user->getCurCharacNo(), ipg);
                        added = user->addAvatarItem(
                            (unsigned long)fieldItem.m_itemIndex, 0,
                            *(char*)((char*)&fieldItem.m_item + 0x0b), ipg,
                            (eAvatarItemAddReason)0x11);
                    }
                    if (added == -1)
                    {
                        packet.put_header(1, 0x2e);
                        packet.put_byte(0);
                        packet.put_byte(4);
                        packet.finalize(true);
                        user->Send(packet);
                        result = 0;
                    }
                    else
                    {
                        sub_WarField_PickupItem((char*)&m_field, iter);
                        packet.put_header(0, 0x5d);
                        packet.put_short(idx);
                        packet.put_short(user->get_unique_id());
                        packet.put_short(added);
                        packet.finalize(true);
                        SendToRoom(packet);
                        result = 1;
                    }
                }
                else
                {
                    cMyTrace trace("bool WarRoom::HandleGetItem(CUser*, int)", 0x982, 5);
                    trace("fieldData_.PickupItem( %d/%d ) ", idx, itemIdx);
                    packet.put_header(1, 0x2e);
                    packet.put_byte(0);
                    packet.put_byte(check);
                    packet.finalize(true);
                    user->Send(packet);
                    result = 0;
                }
            }
        }
    }
    return result;
}

int WarRoom::HandleItemDrop(CUser* user, int a, int b, char c, int d, int e)
{
    char dropType = c;
    int st = GetState();
    if ((st != 3) && (st != 4))
        return 0x13;
    if (e < 0)
    {
        user->getCurCharacName();
        unsigned int accId = user->get_acc_id();
        NumberToString(accId, 0);
        LogManager::logFormat(1, "WarRoom.cpp",
                              "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)",
                              0xa1a, "user - accid[%s] %s : count<0");
        return 0x11;
    }
    if (user->CheckInTrade() != 0)
        return 0x13;
    WR_map_item field;
    sub_map_item_C1(&field);
    Inven_Item item;
    if (dropType == 2)
    {
        Inven_Item slotItem =
            sub_CCargo_get_cargo_slot((void*)user->getCurCharacCargoR(), d);
        item = slotItem;
    }
    else if (dropType == 3)
    {
        Inven_Item slotItem;
        slotItem = sub_CInventory_GetInvenSlot((void*)user->getCurCharacInvenR(), 0, d);
        item = slotItem;
    }
    else if (dropType == 0)
    {
        if (d == 0)
        {
            field.m_count = 0;
            field.m_pad1[1] = 2;
            field.m_itemIndex = 0;
            field.m_item.set_add_info(e);
            field.m_dropIndex = 0;
            field.m_item.ResetItemAttr();
        }
        else
        {
            Inven_Item slotItem;
            slotItem =
                sub_CInventory_GetInvenSlot((void*)user->getCurCharacInvenR(), 1, d);
            item = slotItem;
        }
    }
    int price = 0;
    int itemIdx = item.m_addInfo;
    if (itemIdx != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            LogManager::logFormat(
                1, "WarRoom.cpp",
                "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)", 0xa4c,
                "[WarRoom::HandleDropItem] Null Item %d");
            return 0xffffffff;
        }
        int attach = pItem->GetAttachType();
        switch (attach)
        {
        case 1:
        case 2:
        case 5:
            return 0x17;
        case 3:
            if (item.m_field0 != 1)
                return 0x17;
            break;
        default:
            break;
        }
        if ((item.isEquipableItemType() != 0) &&
            ((*(int*)((char*)pItem + 0x234) == 0xb) ||
             (*(int*)((char*)pItem + 0x234) < 10)))
        {
            return 0x17;
        }
        field.m_count = 0;
        field.m_pad1[1] = item.m_field1;
        field.m_itemIndex = itemIdx;
        if (item.isEquipableItemType() != 0)
        {
            field.m_item.set_add_info(item.m_addInfo2);
        }
        else
        {
            field.m_item.set_add_info(e);
        }
        field.m_item.SetItemAttr(item.GetItemAttr());
        field.m_dropIndex = *(unsigned short*)((char*)&item + 0x0c);
        field.m_item = item;
        *(int*)((char*)&field.m_item + 0x0d) = *(int*)((char*)&item + 0x0d);
        field.m_item.m_amp = item.m_amp;
        sub_UpgradeSeparateInfo_SetUpgradeSeparate(
            &field.m_item.m_upgradeSep, item.m_upgradeSep.GetUpgradeSeparate());
        int price2;
        sub_Store_GetSellItemPrice(sub_G_Store(), &item, pItem, e, 0, price2);
        price = price2;
    }
    if (dropType == 2)
    {
        if (sub_CCargo_delete_item(user->getCurCharacCargoW(), d, e, 0x10) != 1)
        {
            cMyTrace trace(
                "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)", 0xabf,
                5);
            trace("CParty::drop_item , delete_item failed 2, User ch=%d , %d %d",
                  user->getCurCharacNo(), d, b);
            return 0x11;
        }
    }
    else if (dropType < 3)
    {
        if (dropType == 0)
        {
            if (d == 0)
            {
                if (((CInventory*)user->getCurCharacInvenW())
                        ->use_money(e, (eMoneySubReason)6, 1) != 1)
                {
                    cMyTrace trace(
                        "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)",
                        0xa9f, 5);
                    trace("CParty::drop_item , use_money failed, User ch=%d , %d %d",
                          user->getCurCharacNo(), d, e);
                    return 0x11;
                }
                sub_CValueStatistic_AddValueStatistic(
                    sub_GetInstanceValueStatistic(), 0x17, user, (unsigned)e);
            }
            else
            {
                if (sub_CInventory_delete_item(
                        (CInventory*)user->getCurCharacInvenW(), 1, d, e, 6, 1) != 1)
                {
                    cMyTrace trace(
                        "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)",
                        0xaae, 5);
                    trace("CParty::drop_item , delete_item failed, User ch=%d , %d %d",
                          user->getCurCharacNo(), d, b);
                    return 0x11;
                }
                sub_CValueStatistic_AddValueStatistic(sub_GetInstanceValueStatistic(),
                                                      0x16, user, (unsigned)price);
            }
        }
    }
    else if (dropType == 3)
    {
        if (sub_CInventory_delete_item(
                (CInventory*)user->getCurCharacInvenW(), 0, d, 1, 6, 1) != 1)
        {
            cMyTrace trace(
                "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)", 0xaca,
                5);
            trace("CParty::drop_item , delete_item failed 3, User ch=%d , %d",
                  user->getCurCharacNo(), d);
            return 0x11;
        }
        sub_CValueStatistic_AddValueStatistic(sub_GetInstanceValueStatistic(), 0x16,
                                              user, (unsigned)price);
    }
    else if (dropType == 7)
    {
        if (sub_CInventory_delete_item(
                (CInventory*)user->getCurCharacInvenW(), 3, d, 1, 6, 1) != 1)
        {
            cMyTrace trace(
                "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)", 0xaed,
                5);
            trace("CParty::drop_item , delete_item failed 4, User ch=%d , %d",
                  user->getCurCharacNo(), d);
            return 0x11;
        }
        sub_CValueStatistic_AddValueStatistic(sub_GetInstanceValueStatistic(), 0x16,
                                              user, (unsigned)price);
    }
    if (item.m_field1 != 1)
    {
        if ((field.m_itemIndex != 0) && (field.m_item.get_add_info() > 0x7d0))
        {
            cMyTrace trace(
                "int WarRoom::HandleItemDrop(CUser*, int, int, char, int, int)", 0xb00,
                4);
            trace("war room drop item(%d) stack size over %d add_info(%d)",
                  field.m_itemIndex, 0x7d0, field.m_item.get_add_info());
        }
    }
    int dropIdx = sub_WarField_DropItem((char*)&m_field, field);
    if (dropIdx == -1)
        return 0x16;
    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id());
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(dropIdx);
    packet.put_int(field.m_itemIndex);
    packet.put_byte(field.m_item.GetItemAttr());
    packet.put_int(field.m_item.get_add_info());
    packet.put_short(*(unsigned short*)((char*)&field.m_item + 0x0b));
    packet.put_int(*(int*)((char*)&field.m_item + 0x0d));
    packet.put_byte(field.m_item.m_amp.getAbilityType());
    packet.put_short(field.m_item.m_amp.getAbilityValue());
    packet.put_packet(field.m_item);
    packet.finalize(true);
    SendToRoom(packet);
    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)dropType);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

int WarRoom::ReviveUserByCoin(CUser* user, unsigned short uid)
{
    int st = GetState();
    if ((st != 3) && (st != 4))
        return 0x13;
    CUser* target = 0;
    int slot = 0;
    for (slot = 0; slot <= 5; ++slot)
    {
        if ((m_slots[slot] != 0) &&
            (((CUser*)m_slots[slot])->get_unique_id() == uid))
        {
            if (m_pad184[slot] != 0)
                return 0x12;
            target = (CUser*)m_slots[slot];
            break;
        }
    }
    if (target == 0)
        return 0x15;
    if (user->GetUsedCoinCount() < *(int*)((char*)m_pDungeon + 0x87c))
    {
        const char* name = target->getCurCharacName();
        sub_HistoryLog_WriteUseCoin(user, (void*)user->m_field796f8, name);
        SetCharacterLive(slot, target, true, true, 0, 0xff);
        return 0;
    }
    return 0x16;
}
