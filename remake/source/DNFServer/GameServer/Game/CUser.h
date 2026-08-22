#ifndef GAME_CUSER_H_
#define GAME_CUSER_H_

// ============================================================================
// df_game_r 还原 —— CUser（G1-3 批次，在线用户会话聚合根）
// 布局依据 ORIG 构造（0x0864e410）/ 析构（0x0864ef9c）逐子对象推导：
//   +0x000 CUserCharacInfo（基类，0xe0）      +0x0e0 CNetwork<4096,450000>
//   +0x6ef6c UserMercenaryInfoMgr             +0x6ef78 租赁 map
//   +0x6effc CAccountCargo                    +0x6fd64 CGameOption
//   +0x703c4 STBreakAwayRewards               +0x703db STCommand[50]
//   +0x705dc Client_Spec                      +0x711ec list<int>
//   +0x711f4 CPacketVerifier                  +0x7121c WongWork::CUserPremium
//   +0x71bac WongWork::CHackAnalyzer          +0x795e8 Secu_AccountHacking
//   +0x795fc CodeHackCheckStorage             +0x79664 string[2]
//   +0x79674 CharacMoneyLog[7]                +0x796e8 vector<_Charac_info>
//   +0x796f4 CCharacterView                   +0x79700 cUserHistoryLog
//   +0x79744 charac_expand::CDataMgr          +0x79794 InventoryMemory
//   +0x858d4 UserQuest                        +0x8cef4 vector<uint>
//   +0x8cf64 CDungeonGainedGold               +0x8cf68 vector<LimitItemUsage>
//   +0x8cf74 vector<_DBLogItemInfo>           +0x8cf80 list<int>
//   +0x8cf88 Mutex x2                         +0x8cfe4 set<uint>
//   +0x8d020 STGuildDBInfoOnly                +0x8d0dd STGuildAgitDBInfo
//   +0x8d0e4 vector<EventInfo>                +0x8d114 WongWork::CSecurityCard
//   +0x8d160 Sanicova::CPad                   +0x8d1b8 vector<stLookOuts> x2
//   +0x8d1d0 vector<tyIPCounters>             +0x8d1e4 CerashopAddRestrict::UserInfo
//   +0x8d24d STExpReward                      +0x8d264 APSystem::CActionPointManager
//   +0x8df60 advancealtar::CharacAdvanceAltarManager
//   +0x8e034 Character                        +0x8e080 string
//   +0x8e3f0 WongWork::CMCAPManager           +0x8e954 XNuclear::CHades
//   +0x8e980 Secu_GoldControl                 +0x8ea68 Secu_HackLogCheck
//   +0x8eaa4 map<short,int>                   +0x8eabc map<pair<char,char>,int>
//   +0x8eae4 PIReceiverManager                +0x8eaec PISenderManager
//   +0x8eb00 set<pair<uchar,ushort>>          +0x8eb50 map<uchar,short>
//   +0x8eb68 map<int,pair<int,int>>           +0x8eb80 map<int,bool>
//   +0x8ebc4 vector<uchar>                    +0x8ebdc BingoData
//   +0x8ebe4 GrowthWeaponEvent                +0x8ebf8 GrowthCreatureEvent
//   +0x8ec04 CharacLevelUpGiftAccountOnce     （总尺寸 0x8ec30）
// 子对象构造/析构大多为 ORIG weak（原版头文件 inline），本头以 inline 占位
// 定义（后续对应批次 TU 给出强定义后自动覆盖）。CAccountCargo / CDataMgr /
// CharacAdvanceAltarManager / PI(Sender|Receiver)Manager 在 ORIG 为强符号，
// 本头仍以 weak 占位，保证当前链接可用。
// ============================================================================

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <time.h>

#include "CUserCharacInfo.h"
#include "CNetwork.h"
#include "CGameOption.h"
#include "CEnvironment.h"
#include "GameTypes.h"
#include "STEquipmentScript.h"
#include "GameEnums.h"      // ENUM_SKILL_TREE_KIND（master_new_skills 声明所需）
struct _Mastered_skill;     // 完整定义见 SkillSlot.h（避免循环包含）

struct stBuySkillInfo;  // forward decl（ORIG stub 传递引用；完整定义见 CEventCharacterHandler.cpp）


// ---- 局部守卫枚举（值对齐 GameEnums.h；避免整头 include 引发的同名冲突） ----
#ifndef DNF_ENUM_DISCONN_SIG_DEFINED
#define DNF_ENUM_DISCONN_SIG_DEFINED
enum DISCONN_SIG
{
    DISCONN_SIG_0 = 0,
    DISCONN_SIG_5 = 5,
    DISCONN_SIG_0x30 = 0x30,
    DISCONN_SIG_0x31 = 0x31,
    DISCONN_SIG_0x32 = 0x32
};
#endif
#ifndef DNF_ENUM_QUEST_ENEMY_TYPE_DEFINED
#define DNF_ENUM_QUEST_ENEMY_TYPE_DEFINED
enum ENUM_QUEST_ENEMY_TYPE { ENUM_QUEST_ENEMY_TYPE_0 = 0 };
#endif



#ifndef DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
#define DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
enum ENUM_PREMIUM_TYPE { ENUM_PREMIUM_TYPE_0 = 0 };

// ENUM_DUNGEON_MODE：与 CParty.h 共享守卫，避免重定义；
// cUserHistoryLog::DungeonClearInfo 声明需要（GameEnums.h 并无此类型）。
#ifndef DNF_ENUM_ENUM_DUNGEON_MODE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_MODE_DEFINED
enum ENUM_DUNGEON_MODE { ENUM_DUNGEON_MODE_0 = 0 };
#endif
#endif

struct STPremiumItemData
{
    int m_value0;   // +0x00
    int m_value4;   // +0x04
    int m_value8;   // +0x08
    int m_valuec;   // +0x0c
};
// ---- 共享枚举（守卫；CInventory.h 同值，权威归属 CUser.h） ----
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

enum ENUM_BADGE_TYPE
{
    ENUM_BADGE_TYPE_0 = 0
};

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

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_ECOINSUBREASON_DEFINED
#define DNF_ENUM_ECOINSUBREASON_DEFINED
enum eCoinSubReason
{
    eCoinSubReason_0 = 0
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

#ifndef DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
#define DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
enum ENUM_DBLOG_ITEM_TYPE
{
    ENUM_DBLOG_ITEM_TYPE_0 = 0
};
#endif

enum ENUM_CMDPACKET
{
    ENUM_CMDPACKET_0 = 0
};

#ifndef DNF_ENUM_NOTIPACKET_DEFINED
#define DNF_ENUM_NOTIPACKET_DEFINED
enum ENUM_NOTIPACKET { ENUM_NOTIPACKET_0 = 0 };
#endif
#ifndef DNF_ENUM_MONEY_FULL_REASON_DEFINED
#define DNF_ENUM_MONEY_FULL_REASON_DEFINED
enum ENUM_MONEY_FULL_REASON { ENUM_MONEY_FULL_REASON_0 = 0 };
#endif
#ifndef DNF_ENUM_USERINFO_DEFINED
#define DNF_ENUM_USERINFO_DEFINED
enum ENUM_USERINFO { ENUM_USERINFO_0 = 0 };
#endif
#ifndef DNF_ENUM_EQUIPSLOT_DEFINED
#define DNF_ENUM_EQUIPSLOT_DEFINED
enum ENUM_EQUIPSLOT { ENUM_EQUIPSLOT_0 = 0 };
#endif

enum ENUM_PEER_REQUEST_TYPE
{
    ENUM_PEER_REQUEST_TYPE_0 = 0
};

enum ENUM_MESSAGE_TYPE
{
    ENUM_MESSAGE_TYPE_0 = 0
};

enum PAY_TYPE
{
    PAY_TYPE_0 = 0
};

#ifndef DNF_ENUM_EEXPADDREASON_DEFINED
#define DNF_ENUM_EEXPADDREASON_DEFINED
enum eExpAddReason { eExpAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_EWPADDREASON_DEFINED
#define DNF_ENUM_EWPADDREASON_DEFINED
enum eWPAddReason { eWPAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLINITREASON_DEFINED
#define DNF_ENUM_ESKILLINITREASON_DEFINED
enum eSkillInitReason { eSkillInitReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLADDREASON_DEFINED
#define DNF_ENUM_ESKILLADDREASON_DEFINED
enum eSkillAddReason { eSkillAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLDELREASON_DEFINED
#define DNF_ENUM_ESKILLDELREASON_DEFINED
enum eSkillDelReason { eSkillDelReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESPADDREASON_DEFINED
#define DNF_ENUM_ESPADDREASON_DEFINED
enum eSPAddReason { eSPAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESPSUBREASON_DEFINED
#define DNF_ENUM_ESPSUBREASON_DEFINED
enum eSPSubReason { eSPSubReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ECHANGEGROWTYPEREASON_DEFINED
#define DNF_ENUM_ECHANGEGROWTYPEREASON_DEFINED
enum eChangeGrowTypeReason { eChangeGrowTypeReason_0 = 0 };
#endif

namespace WongWork
{
enum ENUM_HACK_TYPE { ENUM_HACK_TYPE_0 = 0 };
enum ENUM_HACKTYPE { ENUM_HACKTYPE_0 = 0 };
class CDeathTower;   // 完整定义见 CDeathTower.h（getDeathTower 返回指针）
}

enum ch_state
{
    ch_state0 = 0,
    ch_state_0 = 0,
    ch_state1 = 1,
    ch_state2 = 2,
    ch_state3 = 3,
    ch_state_3 = 3,
    ch_state4 = 4,
    ch_state_4 = 4,
    ch_state5 = 5,
    ch_state6 = 6,
    ch_state7 = 7,
    ch_state8 = 8,
    ch_state9 = 9,
    ch_state10 = 10,
    ch_state11 = 11
};

namespace UserSpace
{
enum T
{
    UserSpace_T_0 = 0
};
}

enum ENUM_SECURITY_REWARD_TYPE
{
    ENUM_SECURITY_REWARD_TYPE_0 = 0
};

struct SIG_CREATE_CHARAC;

// ---- 前向声明（真实定义属其它 TU） ----
class CInventory;
class CExpandEquipslot;
class PacketGuard;
class Store;
class CSecu_ProtectionField;
class CValueStatistic;
class CSlotBoundChecker;
class GameWorld;
class CGameManager;
class CGameMasterCharacter;
struct user_gold_info;   // 完整定义见 SigTypes.h
enum ENUM_GM_ITEM_UPGRADE { E_GM_ITEM_UPGRADE_NONE = 0 };

#pragma pack(push, 1)
struct _Charac_info
{
    int m_characNo;         // +0x00
    char m_name[0x1d];      // +0x04
    char m_pad21[0x26 - 0x21];  // +0x21..0x25
    char m_field26;         // +0x26（job）
    short m_level;          // +0x27
    char m_pad29[0x14c6 - 0x29];
    int m_viewState;        // +0x14c6
    char m_pad14ca[0x14d1 - 0x14ca];
};
#pragma pack(pop)

// ---- 雇佣兵加载结构（vector 元素；ORIG 0x2d 字节起） ----
struct CHARAC_LOAD_MERCENARY
{
    CHARAC_LOAD_MERCENARY() {}
    ~CHARAC_LOAD_MERCENARY() {}
    void Reset();

    int m_characNo;       // +0x00
    short m_level;        // +0x04
    char m_name[0x1d];    // +0x06
    char m_pad[0x40 - 0x06 - 0x1d];  // 占位补足 0x40
};

// ---- 子对象最小类（尺寸按构造偏移推导；方法签名按 ORIG nm 核对） ----
namespace InstanceRentalSystem
{
struct RentalInfo
{
    RentalInfo() {}
    ~RentalInfo() {}
    char m_pad[0xc];
};
}

struct LimitItemUsage
{
    char m_pad[0x14];
};

struct stLookOuts
{
    char m_pad[0x10];
};

struct tyIPCounters
{
    char m_pad[0x10];
};

namespace LevelupSupportEventManger
{
struct EventInfo
{
    char m_pad[0x10];
};
}

class UserMercenaryInfoMgr
{
public:
    UserMercenaryInfoMgr() {}
    ~UserMercenaryInfoMgr() {}

    void AddCharac(const CHARAC_LOAD_MERCENARY& info);
    void RemoveCharac(unsigned int characNo);
    void SetCharac(unsigned int characNo, const CHARAC_LOAD_MERCENARY& info);
    bool IsCompetition(unsigned int characNo) const;
    bool IsReturnCharac(unsigned int characNo) const;

    std::vector<CHARAC_LOAD_MERCENARY> m_characs;  // +0x00（ORIG C1/D1 = vector C1/D1）
};

class CAccountCargo
{
public:
    // ========================================================================
    // CAccountCargo（账号金库，G4 商城/交易域批次，ORIG 布局 0xd68）
    // 布局依据 C1 (0x8289794) / Clear (0x828986c) / SetDBData (0x8289816)
    // / AddMoney (0x828a742) / GetCapacity (0x822f012) 推导：
    //   +0x000 CUser* m_pUser        +0x004 Inven_Item m_slots[0x37]（0xd58）
    //   +0xd5c unsigned int m_money  +0xd60 unsigned int m_capacity
    //   +0xd64 char m_alter
    // ========================================================================
    CAccountCargo();

    // ---- 生命周期 / DB ----
    void Clear();
    void SetDBData(CUser* user, Inven_Item* items, unsigned int money,
                   unsigned int capacity);
    void ResetSlot(int slot);
    void CreateAccountCargo(CUser* user);
    void UpgradeAccountCargo(CUser* user);
    void DeleteAccountCargo(CUser* user);
    void SendNotifyRecipe(CUser* user, int slot, bool flag);

    // ---- 槽位 / 物品 ----
    Inven_Item GetSlot(int slot);
    Inven_Item* GetSlotRef(int slot);
    int CheckInsertCondition(Inven_Item& item);
    int InsertItem(Inven_Item& item, int slot);
    int DeleteItem(int slot, int count);
    bool MoveItem(int from, int to);
    int GetEmptySlot();
    int CheckSlotEmpty(int slot);
    int GetSpecificItemSlot(int itemIdx);
    int CheckStackLimit(int slot, int itemIdx, int count);
    int CheckValidSlot(int slot) const;
    int GetItemCount();

    // ---- 金钱 ----
    int DepositMoney(unsigned int money);
    int WithdrawMoney(unsigned int money);
    int CheckMoneyLimit(unsigned int money);
    void AddMoney(unsigned int money);
    void SubMoney(unsigned int money);
    unsigned int GetMoney();

    // ---- 容量 / 发包 ----
    int GetCapacity();
    void SetCapacity(unsigned int capacity);
    void SetStable();
    void SendNotifyMoney(ENUM_CMDPACKET cmd);
    int SendItemList();
    void MakeItemPacket(PacketGuard* packet, int slot) const;
    char IsAlter();

    CUser* m_pUser;                    // +0x00
    Inven_Item m_slots[0x38];          // +0x04（0xd58 = 0x38 * 0x3d）
    unsigned int m_money;              // +0xd5c
    unsigned int m_capacity;           // +0xd60
    char m_alter;                      // +0xd64
    char m_pad[3];                     // +0xd65
};

#pragma pack(push, 1)
class STBreakAwayRewards
{
public:
    STBreakAwayRewards();
    void clear();
    void operator=(const STBreakAwayRewards& other);

    int m_rewardItemId;      // +0x00（0x703c4）
    int m_rewardAItemCnt;    // +0x04
    int m_luckPoint;         // +0x08
    int m_fieldc;            // +0x0c
    int m_itemOrder;         // +0x10
    char m_pad14[3];         // +0x14..0x17
};
#pragma pack(pop)

namespace STSkillCommandData
{
class STCommand
{
public:
    STCommand() {}

    char m_pad[4];  // +0x00
};
}

class Client_Spec
{
public:
    Client_Spec() {}

    char m_pad[0x5fc];      // +0x00
    char m_field5fc;        // +0x5fc
    char m_pad5fd[0xb];     // +0x5fd..0x607
    char m_field608;        // +0x608
    char m_pad609[0xbfc - 0x609];  // +0x609..0xbfb
    char m_gmUserFlag;      // +0xbfc（isGMUser）
    char m_padbfd[0xc00 - 0xbfd];  // +0xbfd..0xbff
    int m_gmUpgradeMode;    // +0xc00（0x711dc）
    int m_age;              // +0xc04（0x711e0）
    char m_progLogout;      // +0xc08（0x711e4）
    char m_padc09[3];       // +0xc09..0xc0b
    int m_seedFromDate;     // +0xc0c（0x711e8）
};

class CPacketVerifier
{
public:
    CPacketVerifier() {}
    ~CPacketVerifier() {}

    char m_pad[0x28];  // +0x00
};

namespace WongWork
{
struct SUserPremiumInfo
{
    int m_type;        // +0x00
    int m_start;       // +0x04
    int m_end;         // +0x08
    int m_flag;        // +0x0c
    int m_state;       // +0x10
};

enum ENUM_PREMIUM_STATE
{
    ENUM_PREMIUM_STATE_0 = 0,
    ENUM_PREMIUM_STATE_1 = 1,   // 生效中
    ENUM_PREMIUM_STATE_2 = 2,   // 等待开始
    ENUM_PREMIUM_STATE_3 = 3    // 已过期
};

class CUserPremium
{
public:
    CUserPremium() {}
    ~CUserPremium() {}

    // ---- 基础访问器（ORIG W 弱符号，const）----
    unsigned short getOverSkillLevel() const;
    int GetAdvantageExpRate() const;
    short GetAdvantageFatigueRate() const;
    int GetAdvPremiumCount() const;
    int GetOverEquipableLevel(ENUM_EQUIPMENTTYPE type) const;
    int GetAdvantageCoin() const;           // 0x0864f8e0（推断实现，见 CUser.cpp）
    int GetGoldBonus(int level) const;      // ORIG 存在（推断实现，见 CUser.cpp）
    bool isAffectedUnlimitFatigue() const;  // ORIG 存在（推断实现，见 CUser.cpp）

    // ---- 生命周期 / 状态操作（ORIG T）----
    void InitPremium();
    void AddPremium(int type, int start, int end, int flag);
    void RemovePremium(int type);
    int startPremium(int type, bool force);
    void ReCalcAdvantage();
    void setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state);
    bool _CheckApply(int type, tm t);
    void RecalcAdditionalInfo(CUser const* user);
    int SetPremiumItemData(unsigned long key, const struct STPremiumItemData& data);

    // ---- 查询（ORIG T const）----
    SUserPremiumInfo* GetPremiumInfo(int type) const;
    SUserPremiumInfo CheckPremiumTimeout() const;
    void GetPremiumInfoList(std::vector<SUserPremiumInfo>& list, int type) const;
    int GetReturnItemRate(int type) const;
    bool CheckPremium(int type) const;
    bool IsRestrictedPremium(int type);
    std::vector<std::pair<int, int> > GetAdvantageItem(int type);

    char m_pad[0x97c];      // +0x00
    void* m_mailBox;        // +0x97c
    char m_pad980[4];       // +0x980..0x983
    int m_lastPlayTime;     // +0x984
    int m_loginTime;        // +0x988（0x71ba4）
    unsigned char m_field98c;   // +0x98c
    char m_pad98d[3];       // +0x98d..0x990
};

class CHackAnalyzer
{
public:
    CHackAnalyzer() {}
    ~CHackAnalyzer() {}

    void addServerHackCnt(CUser* user, ENUM_HACK_TYPE type, unsigned int count,
                          unsigned int a, unsigned int b);
    void addServerHackCnt(CUser* user, ENUM_HACKTYPE type, unsigned int count,
                          unsigned int a, unsigned int b);
    void resetHackInfo();
    void resetServerHackAccumulatedCnt(ENUM_HACKTYPE type);
    void reportHackInfo();
    void beginCollectHackInfo(CUser* user);
    void setLastMonsterDeadTime(long t);

    char m_pad[0x7a3c];  // +0x00
};

class CMailBox;
class CMailBoxHelper;  // 完整声明见 CGameManager.h（含 CMailBox::CMail 依赖）

class CSecurityCard
{
public:
    CSecurityCard() {}
    ~CSecurityCard() {}

    int getCancelCnt();
    void resetCancelCnt();

    char m_pad[0x4c];  // +0x00
};

class CMCAPManager
{
public:
    CMCAPManager() {}
    ~CMCAPManager() {}

    void resetExposedCount();
    void reset();   // ORIG W 0x8694fa0（定义见 CMCAPManager.cpp）

    char m_pad[0x18];   // +0x00..0x17
    bool m_field18;     // +0x18（0x8e408 IsRecvEvent）
    char m_pad19[3];    // +0x19..0x1b
    int m_field1c;      // +0x1c（0x8e40c GetAutoClientRate）
    char m_pad20[0x84 - 0x20];
    int m_field84;     // +0x84
    char m_pad88[8];   // +0x88..0x8f
    int m_field90;     // +0x90
    char m_pad94[0x544 - 0x94];
    bool m_field544;   // +0x544（0x8e934 setRestingUserRestrict）
    char m_pad545[1];  // +0x545
    short m_field546;  // +0x546（0x8e936 setPunishTradeAlert）
    int m_lastLotteryTime;  // +0x548（0x8e938 getLastLotteryTime）
    char m_field54c;   // +0x54c
    char m_pad54d[0x561 - 0x54d];
    bool m_field561;   // +0x561（0x8e951 isARSUserKick）
    char m_pad562[0x564 - 0x562];
};
}

class Secu_AccountHacking
{
public:
    Secu_AccountHacking() {}
    ~Secu_AccountHacking() {}

    void resetInfo();

    char m_pad[0x14];  // +0x00
};

class CodeHackCheckStorage
{
public:
    CodeHackCheckStorage();
    ~CodeHackCheckStorage();

    void reset();

    char m_pad[0x28];    // +0x00..0x27
    int m_field28;       // +0x28（0x79624 getStdDropRate）
    int m_tradePunishType;  // +0x2c（0x79628 GetTradePunishType）
    char m_field30;      // +0x30
    bool m_field31;      // +0x31（0x7962d tournament account）
    char m_pad32[2];     // +0x32..0x33
    char* m_serverGroup; // +0x34
    ENUM_SERVER_GROUP m_onlineServerGroup;  // +0x38（0x79634）
    unsigned int m_onlineCharacNo;  // +0x3c（0x79638）
    int m_onlinePlayType;       // +0x40（0x7963c）
    bool m_field44;      // +0x44
    char m_pad45;        // +0x45
    short m_field46;     // +0x46
    int m_field48;       // +0x48
    int m_field4c;       // +0x4c
    CGameMasterCharacter* m_gameMasterCharac;  // +0x50（0x7964c）
    unsigned int m_field54;  // +0x54
    bool m_firstLogin;   // +0x58（0x79654）
    bool m_field59;      // +0x59
    bool m_field5a;      // +0x5a
    char m_pad5b;        // +0x5b
    int m_field5c;       // +0x5c
    bool m_field60;      // +0x60
    char m_pad61;        // +0x61
    short m_field62;     // +0x62
    int m_field64;       // +0x64
};

class CCharacterView
{
public:
    CCharacterView() {}
    ~CCharacterView() {}

    void reset();
    void enableSaveCharacView();
    void disableSaveCharacView();
    bool isSaveCharacView();

    bool m_bSaveCharacView;  // +0x00
    char m_pad1[3];
};

namespace charac_expand
{
class CDataMgr
{
public:
    CDataMgr();
    ~CDataMgr();

    void reset();
    void ResetDailyMidnight();
    void ResetDaily();
    CExpandEquipslot* GetData(ENUM_CHARAC_EXPAND_TYPE type) const;
    CExpandEquipslot* GetDataR(ENUM_CHARAC_EXPAND_TYPE& type) const;

    char m_pad[0x50];  // +0x00
};
}

class InventoryMemory
{
public:
    InventoryMemory() {}

    char m_pad[0xc140];  // +0x00
};

enum QUEST_CONDITION
{
    QUEST_CONDITION_0 = 0,
    QUEST_CONDITION_1 = 1,
    QUEST_CONDITION_2 = 2,
    QUEST_CONDITION_3 = 3,
    QUEST_CONDITION_4 = 4,
    QUEST_CONDITION_5 = 5
};

class _Quest_Authen_Data
{
public:
    _Quest_Authen_Data();
    void reset();
    char m_pad[0x48];
};

class UserQuest
{
public:
    UserQuest() {}
    ~UserQuest() {}

    bool isClearQuest(int questIdx) const;
    void reset();
    void resetClearQuest(int questIdx);  // ORIG _ZN9UserQuest15resetClearQuestEi（UserQuest.cpp 实现）
    void set_authen_data(QUEST_CONDITION cond, int v1, int v2);
    void get_quest_info(char* buf) const;  // ORIG _ZNK9UserQuest14get_quest_infoEPc（UserQuest.cpp 实现）

    char m_pad[0x761c];  // +0x00
    bool m_field761c;    // +0x761c
    bool m_field761d;    // +0x761d
    char m_pad761e[2];   // +0x761e..0x7620
};

class CDungeonGainedGold
{
public:
    CDungeonGainedGold() {}
    ~CDungeonGainedGold() {}

    unsigned int incGainedGold(unsigned int amount);
    void reset();

    char m_pad[4];  // +0x00
};

#pragma pack(push, 1)
struct STGuildDBInfoOnly
{
    STGuildDBInfoOnly() {}

    char m_pad[0x95];          // +0x00..0x94
    char m_field95;            // +0x95（0x8d0b5 getPowerSide）
    char m_pad96[0x9a - 0x96]; // +0x96..0x99
    int m_powerWarPoint;       // +0x9a
    char m_agitExist;          // +0x9e
    char m_pad9f[0xbd - 0x9f]; // +0x9f..0xbd
};

struct STGuildAgitDBInfo
{
    STGuildAgitDBInfo() {}

    char m_pad[1];   // +0x00
    bool m_todayGuildMember;  // +0x01（0x8d0de）
    char m_pad2[1];  // +0x02
    unsigned int m_usedCera;  // +0x03（0x8d0e0）
};
#pragma pack(pop)

namespace Sanicova
{
class CPad
{
public:
    CPad() {}
    ~CPad() {}

    int getCancelCnt();
    void setCancelCnt(int count);
    int getRequestState();
    bool isActivate() const;
    int getFailCnt();
    void setFailCnt(int cnt);

    char m_pad[0x48];  // +0x00（其后为 CUser::._379 @ +0x8d1a8）
};
}

namespace CerashopAddRestrict
{
// ---- 商城限购数据结构（G4 商城/交易批次） ----
enum RestrictType
{
    RestrictType_Account = 0,
    RestrictType_Charac = 1
};

struct InfoDaily
{
    int m_type;           // +0x00（RestrictType）
    unsigned int m_ipgNo; // +0x04
    int m_field8;         // +0x08
    int m_start;          // +0x0c
    int m_end;            // +0x10
    int m_limit;          // +0x14
    int m_field18;        // +0x18
};

struct paramDaily
{
    unsigned int m_ipgNo;  // +0x00
    int m_count;           // +0x04
    int m_nextDate;        // +0x08
};

class DBInputData
{
public:
    DBInputData() {}
    ~DBInputData() {}

    int m_result;         // +0x00（InputBuyInfo 写入结果：1=已登记）
    InfoDaily* m_pInfo;   // +0x04
    char m_pad[0xc];
};

class ScriptLoader
{
public:
    ScriptLoader() {}
    ~ScriptLoader() {}

    int LoadScript();
    int LoadScriptDaily(RestrictType type);
    void ClearScript();
    InfoDaily* GetRestrictInfo(unsigned int ipgNo);
    bool FindIpgNo(unsigned int ipgNo);

    std::map<unsigned int, InfoDaily*> m_infoMap;   // +0x00
};

class Manager
{
public:
    Manager();
    ~Manager();

    bool Load();
    int Destroy();
    bool FindIpgNo(unsigned int ipgNo);
    int CheckBuyableProduct(CUser* user, unsigned int ipgNo,
                            unsigned int itemIdx, int type);
    void UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                   unsigned int itemIdx);
    static void ClearBuyRestrictItem(CUser* user);
    static void LoadRequestBuyRestrictItem(CUser* user);
    static void SendSyncPacket(CUser* user);
    void makeSyncPacket(CUser* user);
    int InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                     DBInputData& out);

    ScriptLoader m_loader;   // +0x04
};

// 用户侧限购数据（CUser +0x8d1e4，尺寸 0x69）
// 注意：map 类型须在 pack 外先行实例化（GCC 4.4 pack 会压缩模板内部布局），
// 使 pack(1) 仅压缩 UserInfo 自身成员对齐（无尾部填充，0x69）。
struct CerashopUserInfoMapPrelude {
    std::map<unsigned int, paramDaily*> preludeDaily;
    std::map<unsigned int, unsigned int> preludeUInt;
};
#pragma pack(push, 1)
class UserInfo
{
public:
    UserInfo() {}
    ~UserInfo() {}

    std::map<unsigned int, paramDaily*> m_accountDaily;  // +0x00
    std::map<unsigned int, paramDaily*> m_characDaily;   // +0x18
    std::map<unsigned int, unsigned int> m_field30;      // +0x30
    std::map<unsigned int, unsigned int> m_field48;      // +0x48
    char m_field60;                                      // +0x60
    unsigned char m_field61;                             // +0x61
    bool m_field62;                                      // +0x62
    char m_pad63;                                        // +0x63
    int m_ontimeLastRecvIdx;                             // +0x64
    bool m_updateOntime;                                 // +0x68
};
#pragma pack(pop)
}

// STExpReward 已迁移至 CDataManager.h（本头经 CUserCharacInfo.h include）。

namespace APSystem
{
class CActionPointManager
{
public:
    CActionPointManager() {}
    ~CActionPointManager() {}

    void Reset();
    int GetActionPoint() const;
    void GM_Set(CUser& user, unsigned int point);

    char m_pad[0xcfc];  // +0x00
};
}

namespace advancealtar
{
class CharacAdvanceAltarManager
{
public:
    CharacAdvanceAltarManager();
    ~CharacAdvanceAltarManager();

    char m_pad[0xd4];  // +0x00
};
}

namespace XNuclear
{
class CHades
{
public:
    CHades() {}
    ~CHades() {}

    void Send_ReturnToVillage();  // ORIG 0x084b9386（纯空函数）

    char m_pad[0x28];   // +0x00..0x27
    bool m_secuReward;  // +0x28（0x8e97c）
    unsigned char m_secuType;    // +0x29（0x8e97d）
    unsigned char m_rewardType;  // +0x2a（0x8e97e）
    char m_secuGrade;   // +0x2b（0x8e97f）
};
}

class Secu_GoldControl
{
public:
    Secu_GoldControl() {}
    ~Secu_GoldControl() {}

    void Init(CUser* user);
    void SetInfo(user_gold_info& info);
    void SetMailCharName(const char* name);
    void resetRangeMoney(user_gold_info& dst, const user_gold_info& src);
    void AddGold(unsigned int value, eMoneyAddReason reason);
    void SubGold(unsigned int value, eMoneySubReason reason);
    void AddMoney(unsigned int value);
    void SubMoney(unsigned int value);
    void AddImportMoney(unsigned int value);
    void SubImportMoney(unsigned int value);
    void AddAuctionMoney(unsigned int value);
    void SubAuctionMoney(unsigned int value);
    void MailComplete(bool flag);
    void TradeComplete(int subValue, int addValue);
    void MailGold(unsigned int value, bool flag);
    void MailSameMid();
    void SavetoDB(bool a, bool b, bool c);
    void CheckMoneyRange();
    void* GetGoldInfo();
    void CheckDate();
    void UseFatigue(int count);

    CUser* m_pUser;                                // +0x00
    // 总金币统计（ORIG user_gold_info @ +0x04）
    unsigned int m_totalGoldLow;                   // +0x04
    unsigned int m_totalGoldHigh;                  // +0x08
    unsigned int m_totalImportGoldLow;             // +0x0c
    unsigned int m_totalImportGoldHigh;            // +0x10
    unsigned int m_totalSubGoldLow;                // +0x14
    unsigned int m_totalSubGoldHigh;               // +0x18
    unsigned int m_totalImportSubLow;              // +0x1c
    unsigned int m_totalImportSubHigh;             // +0x20
    unsigned int m_totalAuctionAddLow;             // +0x24
    unsigned int m_totalAuctionAddHigh;            // +0x28
    unsigned int m_totalAuctionSubLow;             // +0x2c
    unsigned int m_totalAuctionSubHigh;            // +0x30
    unsigned short m_counter34;                    // +0x34
    unsigned short m_counter36;                    // +0x36
    unsigned short m_counter38;                    // +0x38
    unsigned short m_counter3a;                    // +0x3a
    unsigned short m_counter3c;                    // +0x3c
    unsigned short m_counter3e;                    // +0x3e
    // 当前金币统计（ORIG user_gold_info @ +0x40）
    unsigned int m_goldLow;                        // +0x40
    unsigned int m_goldHigh;                       // +0x44
    unsigned int m_importGoldLow;                  // +0x48
    unsigned int m_importGoldHigh;                 // +0x4c
    unsigned int m_subGoldLow;                     // +0x50
    unsigned int m_subGoldHigh;                    // +0x54
    unsigned int m_importSubLow;                   // +0x58
    unsigned int m_importSubHigh;                  // +0x5c
    unsigned int m_auctionAddLow;                  // +0x60
    unsigned int m_auctionAddHigh;                 // +0x64
    unsigned int m_auctionSubLow;                  // +0x68
    unsigned int m_auctionSubHigh;                 // +0x6c
    unsigned short m_counter70;                    // +0x70
    unsigned short m_counter72;                    // +0x72
    unsigned short m_counter74;                    // +0x74
    unsigned short m_counter76;                    // +0x76
    unsigned short m_counter78;                    // +0x78
    unsigned short m_counter7a;                    // +0x7a
    // 备份金币统计（ORIG user_gold_info @ +0x7c..0xb8）
    unsigned int m_backupGoldLow;                  // +0x7c
    unsigned int m_backupGoldHigh;                 // +0x80
    unsigned int m_backupImportLow;                // +0x84
    unsigned int m_backupImportHigh;               // +0x88
    unsigned int m_backupSubLow;                   // +0x8c
    unsigned int m_backupSubHigh;                  // +0x90
    unsigned int m_backupImportSubLow;             // +0x94
    unsigned int m_backupImportSubHigh;            // +0x98
    unsigned int m_backupAuctionAddLow;            // +0x9c
    unsigned int m_backupAuctionAddHigh;           // +0xa0
    unsigned int m_backupAuctionSubLow;            // +0xa4
    unsigned int m_backupAuctionSubHigh;           // +0xa8
    unsigned short m_counterac;                    // +0xac
    unsigned short m_counterae;                    // +0xae
    unsigned short m_counterb0;                    // +0xb0
    unsigned short m_counterb2;                    // +0xb2
    unsigned short m_counterb4;                    // +0xb4
    unsigned short m_counterb6;                    // +0xb6
    int m_fieldb8;                                 // +0xb8
    int m_fieldbc;                                 // +0xbc
    unsigned int m_dateFlag;                       // +0xc0
    int m_mailGold;                                // +0xc4
    char m_mailCharName[0x1e];                     // +0xc8
    unsigned char m_fielde6;                       // +0xe6
};

class Secu_HackLogCheck
{
public:
    Secu_HackLogCheck() {}
    ~Secu_HackLogCheck() {}

    char m_pad[0x3c];  // +0x00
};

class PIReceiverManager
{
public:
    PIReceiverManager();
    ~PIReceiverManager();

    char m_pad[8];  // +0x00
};

class PISenderManager
{
public:
    PISenderManager();
    ~PISenderManager();

    char m_pad[8];       // +0x00..0x07
    unsigned char m_nonClientFlag;   // +0x08（0x8eaf4 GetNonClientFlag）
    char m_pad9[3];      // +0x09..0x0b
    int m_nonClientRandInt;          // +0x0c（0x8eaf8 GetNonClientRandInt）
    unsigned char m_field10;  // +0x10
    char m_pad11[3];   // +0x11..0x14
};

struct BingoData
{
    BingoData() {}

    void clear();

    char m_pad[8];  // +0x00
};

struct GrowthWeaponEvent
{
    GrowthWeaponEvent() {}

    char m_pad[0x14];  // +0x00
};

struct GrowthCreatureEvent
{
    struct eventCharacInfo
    {
        char m_pad[0x10];
    };

    GrowthCreatureEvent();
    ~GrowthCreatureEvent() {}

    std::vector<eventCharacInfo> m_vec;  // +0x00（0xc）
};

struct CharacLevelUpGiftAccountOnce
{
    CharacLevelUpGiftAccountOnce();
    ~CharacLevelUpGiftAccountOnce() {}

    std::multimap<int, int> m_map;  // +0x00（0x18）
    unsigned int m_field18;         // +0x18（0x8ec1c）
    char m_pad1c[4];                // +0x1c..0x20
};

class PlayInfo
{
public:
    PlayInfo() {}
    ~PlayInfo() {}

    void Reset();

    int m_f00;          // +0x00
    int m_f04;          // +0x04
    int m_f08;          // +0x08
    int m_f0c;          // +0x0c
    int m_f10;          // +0x10
    int m_f14;          // +0x14
    int m_f18;          // +0x18
    int m_f1c;          // +0x1c
    int m_f20;          // +0x20
    int m_f24;          // +0x24
    int m_f28;          // +0x28
    char m_f2c;         // +0x2c
    char m_pad2d[3];
    int m_f30;          // +0x30
    int m_f34;          // +0x34
    char m_f38;         // +0x38
    char m_pad39[7];    // +0x39..0x3f
    char m_f40;         // +0x40
    char m_pad41[3];
    float m_f44;        // +0x44（SetRating）
    unsigned int m_schoolNo;     // +0x48（0x8e07c）
};

class Character : public PlayInfo
{
public:
    Character() {}
    ~Character() {}
};

// ---- 用户行为日志门面（CUser +0x79700，0x44 字节） ----
#ifndef DNF_ENUM_ETRADEENDREASON_DEFINED
#define DNF_ENUM_ETRADEENDREASON_DEFINED
enum eTradeEndReason { eTradeEndReason_0 = 0 };
#endif
#ifndef DNF_ENUM_EAVATARITEMDELREASON_DEFINED
#define DNF_ENUM_EAVATARITEMDELREASON_DEFINED
enum eAvatarItemDelReason { eAvatarItemDelReason_0 = 0 };
#endif
struct MSG_MAILBOX_SEND;
class cUserHistoryLog
{
public:
    cUserHistoryLog();               // ORIG 0x8695fe4：清零 m_user（C1/C2 同址）
    ~cUserHistoryLog() {}

    void SetUser(CUser* user);
    void AvatarItemAdd(int itemIdx, int avatarUid, const char* agency,
                       eAvatarItemAddReason reason);
    void ItemAdd(INVEN_TYPE invenType, int reason1, int reason2,
                 const Inven_Item& item, eItemAddReason reason);
    void ItemDel(INVEN_TYPE invenType, int count, int param2,
                 const Inven_Item& item, eItemDelReason reason);
    void MoneyAdd(int money, int add, eMoneyAddReason reason);
    void MoneySub(int money, int sub, eMoneySubReason reason);
    void MoneyAddTrade(int money, int add, eMoneyAddReason reason);  // ORIG 0x8683ae0
    void MoneySubTrade(int money, int sub, eMoneySubReason reason);  // ORIG 0x8683b38
    void CoinSub(int coin, int sub, eCoinSubReason reason);
    void EventCoinSub(int coin, int sub, eCoinSubReason reason);
    void PayCoinSub(int coin, int sub, eCoinSubReason reason);
    void pvpMissionClearReward(int expPoint, int exp, int missionKind,
                               int missionIndex);  // ORIG 0x8686a4e
    void EnterDungeon(const char* dungeonName, int level);  // ORIG 0x8684a16
    void EnterDungeon(const char* dungeonName, int unk, const char* memberNames, int state);  // ORIG 0x86849a2
    void LeaveDungeon(const char* dungeonName, int level);
    void LeaveDungeon(const char* dungeonName, int unk, const char* memberNames, int state);
    void LeaveDungeon(int dungeonIdx, int unk, const char* memberNames, int state);
    void DungeonClearInfo(int isLast, long playTimeSec);  // ORIG 0x8684ac4
    void DungeonClearInfo(int isLast, long playTimeSec, int idx, int mode2,
                          int val2, ENUM_DUNGEON_MODE dungeonMode,
                          const char* name, int val3);     // ORIG 0x8684a6e
    void CreatureItemAdd(INVEN_TYPE invenType, int itemIdx, int count, int addInfo,
                         int type, eItemAddReason reason);
    void InitSkill(int treeKind, int level, int sp, int sfp,
                   eSkillInitReason reason);
    void SkillAdd(int job, int treeKind, int skillIdx, int count,
                  eSkillAddReason reason);
    void SkillDel(int job, int treeKind, int skillIdx, int count,
                  eSkillDelReason reason);
    void SPAdd(int treeKind, int remainSP, int sp, eSPAddReason reason);
    void SPSub(int treeKind, int remainSP, int sp, eSPSubReason reason);
    void SFPAdd(int treeKind, int remainSFP, int sfp, eSPAddReason reason);
    void SFPSub(int treeKind, int remainSFP, int sfp, eSPSubReason reason);
    void FPAdd(int a, int b, int c, eFPAddReason reason);
    void FPSub(int a, int b, int c, eFPSubReason reason);

    void AvatarItemDel(int itemIdx, int avatarUid, const char* agency,
                       eAvatarItemDelReason reason);
    void CreatureItemDel(INVEN_TYPE invenType, int itemIdx, int count,
                         int addInfo, int type, eItemDelReason reason);
    void ChangeGrowType(int a, int b, int c, int d, int e,
                        eChangeGrowTypeReason reason);
    void ClearUsedQP(int before, int after, int initCount, int unused);
    void DeleteInvaildItem(const std::string& str, int a, int b);
    void ItemDelCargo(int a, int b, int c, eItemDelReason reason);
    void LevelDown(int a, int b);
    void LevelUp(int a, int b);
    void Logout(const char* a, const char* b, unsigned short c, int d, int e, int f);
    void MoveArea(int a, int b, int c, int d, int e);
    void RequestCleanPad(int a, int b);
    void SendMail(MSG_MAILBOX_SEND* mail, unsigned int a);
    void SetTrader(const char* accName, const char* characName);
    void TradeBegin();
    void TradeEnd(eTradeEndReason reason, int a, int b, int c, int d);
    void TradeItemAddFail(int a, int b);
    void AchievementComplete(int idx);
    void RedeemItemAdd(int a, int b);   // ORIG 0x8686990
    void pvpMissionAdd(int a, int b, int c, int d);
    void pvpMissionDel(int a, int b);

    // ---- 布局（ORIG 尺寸 0x44：+0x22/+0x04 字符串 + 尾部对齐） ----
    CUser* m_user;                   // +0x00（SetUser 写入 / ctor 清零）
    char m_traderAccount[0x1e];      // +0x04（SetTrader param2 / strncpy 0x1e）
    char m_traderCharac[0x1e];       // +0x22（SetTrader param1 / strncpy 0x1e，首字节当标志）
    char m_pad[0x04];                // +0x40
};

class CUser : public CUserCharacInfo
{
public:
    CUser();
    ~CUser();

    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };

    // ---- 嵌套结构（vector<_DBLogItemInfo> 元素 / 金钱日志） ----
    struct _DBLogItemInfo
    {
        unsigned int m_itemIdx;    // +0x00
        unsigned int m_type;       // +0x04
        ENUM_DBLOG_ITEM_TYPE m_value;  // +0x08
        char m_pad0c[4];           // +0x0c
    };

    class CharacMoneyLog
    {
    public:
        CharacMoneyLog() { reset(); }
        ~CharacMoneyLog() {}
        void reset()
        {
            m_money[0] = 0;
            m_money[1] = 0;
            m_money[2] = 0;
        }
        int m_money[3];  // +0x00（0xc 字节）
    };

    // ---- 成员布局（ORIG 构造 0x864e410 逐子对象推导，总尺寸 0x8ec30） ----
    CNetwork<4096, 450000> m_network;                       // +0x0e0
    UserMercenaryInfoMgr m_mercenaryMgr;                    // +0x6ef6c
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >
        m_rentalInfo;                                       // +0x6ef78
    char m_rentalFlag1;                                     // +0x6ef90
    char m_rentalFlag2;                                     // +0x6ef91
    char m_pad6ef92[0x68];                                  // +0x6ef92..0x6eff9
    bool m_rentalFlag3;                                     // +0x6effa
    char m_pad6effb;                                        // +0x6effb
    CAccountCargo m_accountCargo;                           // +0x6effc
    CGameOption m_gameOption;                               // +0x6fd64
    STBreakAwayRewards m_breakAway;                         // +0x703c4
    STSkillCommandData::STCommand m_skillCommands[50];      // +0x703db
    short m_field704a4;                                     // +0x704a4
    short m_field704a6;                                     // +0x704a6
    unsigned short m_field704a8;                            // +0x704a8
    char m_pad704aa[2];                                     // +0x704aa..0x704ab
    int m_field704ac;                                       // +0x704ac
    char m_accName[0x128];                                  // +0x704b0..0x705d7
    int m_field705d8;                                       // +0x705d8
    Client_Spec m_clientSpec;                               // +0x705dc
    std::list<int> m_list711ec;                             // +0x711ec
    CPacketVerifier m_packetVerifier;                       // +0x711f4
    WongWork::CUserPremium m_premium;                       // +0x7121c
    WongWork::CHackAnalyzer m_hackAnalyzer;                 // +0x71bac
    Secu_AccountHacking m_accHacking;                       // +0x795e8
    CodeHackCheckStorage m_codeHackCheck;                   // +0x795fc
    std::string m_str79664[2];                              // +0x79664
    char m_pad7966c[4];                                     // +0x7966c..0x7966f
    int m_field79670;                                       // +0x79670
    CharacMoneyLog m_moneyLogs[7];                          // +0x79674
    int m_field796c8;                                       // +0x796c8
    char m_field796cc;                                      // +0x796cc
    char m_pad796cd[3];                                     // +0x796cd..0x796cf
    int m_field796d0;                                       // +0x796d0
    int m_mileage;                                          // +0x796d4 (GetMileage/SetMileage)
    char m_field796d8;                                      // +0x796d8
    char m_field796d9;                                      // +0x796d9
    char m_pad796da[0xa];                                   // +0x796da..0x796e3
    int m_backupSeed;                                       // +0x796e4
    std::vector<_Charac_info> m_characList;                 // +0x796e8
    CCharacterView m_characterView;                         // +0x796f4
    int m_field796f8;                                       // +0x796f8
    char m_pad796fc[4];                                     // +0x796fc..0x796ff
    cUserHistoryLog m_historyLog;                           // +0x79700
    charac_expand::CDataMgr m_characExpand;                 // +0x79744
    InventoryMemory m_inventoryMemory;                      // +0x79794
    UserQuest m_quest;                                      // +0x858d4
    std::vector<unsigned int> m_vec8cef4;                   // +0x8cef4
    int m_field8cf00;                                       // +0x8cf00
    int m_field8cf04;                                       // +0x8cf04
    int m_lastPacketID;                                     // +0x8cf08
    char m_pad8cf0c[4];                                     // +0x8cf0c..0x8cf0f
    int m_antibotKey;                                       // +0x8cf10
    unsigned int m_clientVersion;                           // +0x8cf14
    char m_pad8cf18[0x40];                                  // +0x8cf18..0x8cf57
    int m_field8cf58;                                       // +0x8cf58
    char m_pad8cf5c[8];                                     // +0x8cf5c..0x8cf63
    CDungeonGainedGold m_dungeonGainedGold;                 // +0x8cf64
    std::vector<LimitItemUsage> m_limitItemUsage;           // +0x8cf68
    std::vector<_DBLogItemInfo> m_dbLogItems;               // +0x8cf74
    std::list<int> m_list8cf80;                             // +0x8cf80
    Mutex m_mutex1;                                         // +0x8cf88
    Mutex m_mutex2;                                         // +0x8cfa0
    int m_field8cfb8;                                       // +0x8cfb8
    int m_field8cfbc;                                       // +0x8cfbc
    int m_field8cfc0;                                       // +0x8cfc0
    int m_field8cfc4;                                       // +0x8cfc4
    short m_field8cfc8;                                     // +0x8cfc8
    char m_pad8cfca[2];                                     // +0x8cfca..0x8cfcb
    UserSpace::T m_field8cfcc;                              // +0x8cfcc
    int m_field8cfd0;                                       // +0x8cfd0
    int m_field8cfd4;                                       // +0x8cfd4
    int m_field8cfd8;                                       // +0x8cfd8
    char m_pad8cfdc[4];                                     // +0x8cfdc..0x8cfdf
    int m_guildWarPoint;                                    // +0x8cfe0
    std::set<unsigned int> m_set8cfe4;                      // +0x8cfe4
    unsigned short m_posX;                                 // +0x8cffc
    unsigned short m_posY;                                 // +0x8cffe
    char m_direction;                                      // +0x8d000
    char m_pad8d001;                                        // +0x8d001
    short m_field8d002;                                     // +0x8d002
    short m_field8d004;                                     // +0x8d004
    short m_field8d006;                                     // +0x8d006
    short m_field8d008;                                     // +0x8d008
    char m_pad8d00a[2];                                     // +0x8d00a..0x8d00b
    short m_field8d00c;                                     // +0x8d00c
    short m_field8d00e;                                     // +0x8d00e
    short m_field8d010;                                     // +0x8d010
    short m_field8d012;                                     // +0x8d012
    unsigned int m_regDate;                                 // +0x8d014
    int m_regTime;                                          // +0x8d018
    unsigned int m_cera;                                    // +0x8d01c
    STGuildDBInfoOnly m_guildDB;                            // +0x8d020
    STGuildAgitDBInfo m_guildAgitDB;                        // +0x8d0dd
    std::vector<LevelupSupportEventManger::EventInfo>
        m_levelupEvents;                                    // +0x8d0e4
    char m_field8d0f0;                                      // +0x8d0f0
    char m_field8d0f1;                                      // +0x8d0f1
    char m_field8d0f2;                                      // +0x8d0f2
    char m_pad8d0f3;                                        // +0x8d0f3
    int m_field8d0f4;                                       // +0x8d0f4
    int m_field8d0f8;                                       // +0x8d0f8
    bool m_field8d0fc;                                      // +0x8d0fc
    char m_pad8d0fd[0x13];                                  // +0x8d0fd..0x8d10f
    int m_field8d110;                                       // +0x8d110
    WongWork::CSecurityCard m_securityCard;                 // +0x8d114
    Sanicova::CPad m_cpad;                                  // +0x8d160
    // CUser::._379（0x10 字节；ORIG 名 _ZN5CUser5._3795resetEv）
    class _379
    {
    public:
        void reset() asm("_ZN5CUser5._3795resetEv");
        bool m_field0;     // +0x00
        bool m_field1;     // +0x01
        char m_pad2[2];
        int m_field4;      // +0x04
        int m_field8;      // +0x08
        unsigned int m_fieldc;  // +0x0c
    } m_379;                                            // +0x8d1a8
    std::vector<stLookOuts> m_lookOuts1;                    // +0x8d1b8
    std::vector<stLookOuts> m_lookOuts2;                    // +0x8d1c4
    std::vector<tyIPCounters> m_ipCounters;                 // +0x8d1d0
    int m_field8d1dc;                                       // +0x8d1dc
    int m_field8d1e0;                                       // +0x8d1e0
    CerashopAddRestrict::UserInfo m_ceraUserInfo;           // +0x8d1e4
    STExpReward m_expReward;                                // +0x8d24d
    APSystem::CActionPointManager m_actionPoint;            // +0x8d264
    advancealtar::CharacAdvanceAltarManager m_advanceAltar; // +0x8df60
    Character m_character;                                  // +0x8e034
    std::string m_str8e080;                                 // +0x8e080
    short m_dailyBadge[3];                                  // +0x8e084..0x8e089
    unsigned char m_tutorialSkipable;                       // +0x8e08a
    char m_pad8e08b;                                        // +0x8e08b
    int m_field8e08c;                                       // +0x8e08c（FatigueUp 写入）
    int m_debugCommand;                                     // +0x8e090（GetDebugCommand/SetDebugCommand）
    unsigned int m_field8e094;                              // +0x8e094
    char m_pad8e098[0x60];                                  // +0x8e098..0x8e0f7
    int m_humanCertifyTimerKey;                             // +0x8e0f8
    int m_humanCertifyErrorCnt;                             // +0x8e0fc
    char m_pad8e100[0x2ed];                                 // +0x8e100..0x8e3ec
    bool m_field8e3ed;                                      // +0x8e3ed
    WongWork::CMCAPManager m_mcap;                          // +0x8e3f0
    XNuclear::CHades m_hades;                               // +0x8e954
    Secu_GoldControl m_goldControl;                         // +0x8e980
    Secu_HackLogCheck m_hackLogCheck;                       // +0x8ea68
    std::map<short, int> m_map8eaa4;                        // +0x8eaa4
    std::map<std::pair<char, char>, int> m_map8eabc;        // +0x8eabc
    char m_pad8ead4[4];                                     // +0x8ead4..0x8ead7
    char m_field8ead8;                                      // +0x8ead8
    char m_pad8ead9[3];                                     // +0x8ead9..0x8eadb
    unsigned int m_ceraPoint;                               // +0x8eadc（GetCeraPoint/SetCeraPoint）
    bool m_field8eae0;                                      // +0x8eae0
    char m_pad8eae1[3];                                     // +0x8eae1..0x8eae3
    PIReceiverManager m_piReceiver;                         // +0x8eae4
    PISenderManager m_piSender;                             // +0x8eaec
    std::set<std::pair<unsigned char, unsigned short> >
        m_set8eb00;                                         // +0x8eb00
    unsigned char m_field8eb18;                             // +0x8eb18
    char m_pad8eb19[3];
    int m_field8eb1c;                                       // +0x8eb1c
    bool m_field8eb20;                                      // +0x8eb20
    char m_pad8eb21[3];                                     // 0x8eb21..0x8eb23
    int m_itemSellCount;                                    // +0x8eb24
    char m_sellCountData[0x1c];                             // +0x8eb28..0x8eb43
    bool m_field8eb44;                                      // +0x8eb44
    char m_pad8eb45[3];
    int m_field8eb48;                                       // +0x8eb48
    char m_field8eb4c;                                      // +0x8eb4c
    char m_field8eb4d;                                      // +0x8eb4d
    char m_pad8eb4e[2];                                     // 0x8eb4e..0x8eb4f
    std::map<unsigned char, short> m_map8eb50;              // +0x8eb50
    std::map<int, std::pair<int, int> > m_map8eb68;         // +0x8eb68
    std::map<int, bool> m_map8eb80;                         // +0x8eb80
    bool m_field8eb98;                                      // +0x8eb98
    char m_field8eb99;                                      // +0x8eb99
    char m_pad8eb9a[2];                                     // 0x8eb9a..0x8eb9b
    unsigned int m_pcRoomPlayStartTime;                     // +0x8eb9c
    char m_pad8eba0[8];                                     // 0x8eba0..0x8eba7
    int m_field8eba8;                                       // +0x8eba8
    int m_field8ebac;                                       // +0x8ebac
    char m_field8ebb0;                                      // +0x8ebb0
    char m_field8ebb1;                                      // +0x8ebb1
    char m_pad8ebb2[0xf];                                   // 0x8ebb2..0x8ebc0
    char m_informNoticeFlag;                                // +0x8ebc1
    char m_pad8ebc2[2];                                     // 0x8ebc2..0x8ebc3
    std::vector<unsigned char> m_vec8ebc4;                  // +0x8ebc4
    int m_field8ebd0;                                       // +0x8ebd0
    int m_field8ebd4;                                       // +0x8ebd4
    int m_field8ebd8;                                       // +0x8ebd8
    BingoData m_bingo;                                      // +0x8ebdc
    GrowthWeaponEvent m_growthWeapon;                       // +0x8ebe4
    GrowthCreatureEvent m_growthCreature;                   // +0x8ebf8
    CharacLevelUpGiftAccountOnce m_levelUpGift;             // +0x8ec04
    int m_field8ec24;                                       // +0x8ec24
    char m_field8ec28;                                      // +0x8ec28
    bool m_field8ec29;                                      // +0x8ec29
    char m_pad8ec2a[2];                                     // 0x8ec2a..0x8ec2b
    int m_field8ec2c;                                       // +0x8ec2c
    char m_eventCreateDnfReward;                           // +0x8ec30
    char m_field8ec31;                                      // +0x8ec31
    char m_field8ec32;                                      // +0x8ec32
    char m_pad8ec33[1];                                     // +0x8ec33（ORIG CUser = 0x8ec34）

    // ---- 构造 / 重置 ----
    void reset();
    void ResetCurCharac();
    void ResetDailyData();
    void resetDailyData();
    void ResetDailyCharacExpandDataMidnight();
    void ResetDailyCharacExpandData();
    void resetGuildDBInfo();
    void resetBlackList();
    void resetInformNoticeFlag();
    void enableSaveInformNoticeFlag();
    void disableSaveInformNoticeFlag();
    void resetHumanCertifyData();
    void reset_ingame_event_history();
    void reset_ingame_event_history_update();
    void reset_pvp_masterid_walkingout_me();
    void reset_server_fatigue_data();
    void resetNPCRelationShipDailyData();
    void ResetUsedCoinInDungeon();
    void ClearItemSellCount();
    void ResetDBLogItem();
    void DBUpdateDBLogItem() const;
    void resetMoneyLog();
    void set_guildwar_point_per_pvpplay(int point);
    int get_guildwar_point_per_pvpplay();
    void* getHades();
    void* GetPVPRoom();
    void* GetSecretShopData();
    void* GetPICSMap();
    char IsPermissionPrivateStore();
    bool IsEquipAvatar();
    bool IsHavePremiumAdvantage() const;
    void RecoverFatigue(int value);
    void LogHistory(const char* fmt, ...);
    int get_aura_avatar_option_value(int idx);
    bool isDuplicationMessage(const std::string& msg);
    void DimensionInoutUpdate(bool flag1, bool flag2);
    void SetTradeSpace(int idx);
    bool CheckFatigue();
    bool has_within_Mission() const;
    bool acceptable_within_mission() const;
    bool isCompetitionMercenary() const;
    void* getBlueMarble();
    bool checkLogOutCorrectly();
    unsigned int find_pvp_masterid_walkingout_me(unsigned int id);
    void insert_pvp_masterid_walkingout_me(unsigned int id);
    void update_old_pvp_point();
    bool update_pvp_rank(const PvpResultType& result);
    void saveTaxMoneyForUpperMember(int money);
    int gainExpAsUpperMember(int exp);
    int gainPowerWarRewardExp(int exp);
    void add_guild_point_item();
    void add_inventory_item(unsigned int itemId);
    bool AddDungeonClear(int dungeonIdx, int clearCount);
    void SendConditionEventInfo();
    void ResetCurCharacUsedGiftFatigueQuantity();
    void SetChangedGiftFatigueQuantity(bool flag);
    void ResetDailyQuest();
    void ResetTrainingQuest();
    void SetPvpIndex(short idx);
    void SetSaveRentalInfoToExchange(bool flag);
    void DeleteRentalItemInfo(int idx);
    unsigned char is_fighting();
    int CheckMoney(int money);
    int getDungeonIdxAfterClear();
    bool isAffectedPremium(ENUM_PREMIUM_TYPE type) const;
    void set_before_area(int area);

    const char* getWebAddress();
    WongWork::CSecurityCard* getSecurityCard();
    CGameOption* GetGameOptionRef();
    Sanicova::CPad* getPad();

    // ---- 登录 / 初始化 / 发包 ----
    int InitOnAccept(int param);
    void set_acc_info(unsigned int accId, const char* accName);

    // ---- 批次 A 访问器（ORIG 反汇编逐偏移补全；0x85a932a 族） ----
    void questAutoClear(int questIdx);
    void _onCompoundItem(int itemIdx);
    void _updateNPCRelationship();
    void processNPCGiftOnLevelUp();
    char* GetUserName();
    char* GetUserEMail();
    const char* GetSsnString();
    char getSex();
    void* GetCSHashSet();
    char* GetGuildName();
    unsigned char GetGuildLevel();
    void* GetHackCheck();
    void* getBingoData();
    void* GetGuildDBInfo();
    char* getGarenaAuthData();
    void* GetPISenderManager();
    void* GetPIReceiverManager();
    void* getGrowthWeaponEventdata();
    void* get_ingame_event_history();
    void* get_ingame_event_history_update();
    void* getGrowthCreatureEventdata();
    void* get_server_fatigue_day_data();
    void* getCharacLevelUpGift_AccountOnce();
    void* GetMailBox();
    int GetMileage();
    int GetPCRoomNo();
    unsigned int GetCeraPoint();
    int getAntibotKey();
    int GetManageLevel();
    int GetManagePoint();
    int GetUserMaxLevel();
    void onDungeonClear(bool flag);
    int GetDebugCommand();
    int GetTradePunishType();
    int getLastLotteryTime();
    int GetNonClientRandInt();
    int GetNonClientFlag();
    int GetLastLoginCharacNo();
    int GetLastLoginChannelNo();
    bool IsValidLastLoginCharac();
    int getRecipeProbability();
    int get_local_ip_address();
    unsigned int GetGuildCargoCapacity();
    int GetGuildPowerWarPoint();
    long GetResumeChecksumTime();
    CGameMasterCharacter* GetGameMasterCharacter();
    int getBreakAwayAccureCera();
    int getBreakAwayRewardOrder();
    int get_multiboxLotteryItemFailCnt();
    int GetCurCharacUsedFatigueQuantity();
    bool isHumanCertified();
    bool isLoadingHackGold();
    bool isSaveInformNoticeFlag();
    int CheckMaxLuckyLevel();
    void OnRecvEvent();
    bool GetBuyingGold();
    void SetBuyingGold(bool flag);
    bool GetFirstLogin();
    void SetProgLogout();
    bool isRestingUser();
    void InitReceivedRequestType();
    bool isBreakAwayDungeonClear();
    bool isUsedBreakAwayLuckPoint();
    bool isJoinedSchoolPointEvent();
    void DisableSaveMemberBonusFatigue();
    bool IsEnableSaveMemberBonusFatigue();
    void SetProperLevelDungeonUser();
    void ClearProperLevelDungeonUser();
    bool IsProperLevelDungeonUser();
    bool isCharacLinkMessageFlag();
    bool isLinkCharacDisconnectFlag();
    void setLinkCharacDisconnectFlag(bool flag);   // ORIG 0x086973dc
    unsigned char getDisconnectLinkCharacSlotIdx();
    bool is_update_ontime_last_recv_idx();
    void reset_update_ontime_last_recv_idx();
    bool CheckLoadRentalInfoFromExchange();
    unsigned char GetPuUser();
    void SetCeraPoint(unsigned int point);
    void SetDebugCommand(int cmd);
    void decre_check_count();
    void setLastLotteryTime(unsigned long t);
    void SetNonClientRandInt(int v);
    void recipeForceProbability(int v);
    void setBreakAwayAccureCera(int v);
    void setCharacAntibotSerialNum(int v);
    void set_multiboxLotteryItemFailCnt(int v);
    void resetPlayExpAdd();
    void resetCleanpadFailCnt();
    bool isCleanPadVerifyLimit();
    void resetTotalPcRoomPlayTime();
    void ResetCurCharacUsedFatigueQuantity();
    int get_server_fatigue_day_size();
    void IncreaseUsedCoinCount();
    void setLoadHackGold(bool flag);
    void SetNonClientFlag(bool flag);
    void setHumanCertified(bool flag);
    void incTradeCount();
    float GetRating();
    void backupSeed();
    void ResetCharacExpandData();
    bool IsExistGuildAgit();
    short GetDailyBadge(ENUM_BADGE_TYPE type);
    int get_charac_count();
    bool isTradePunishOverThirtyDays();
    void set_area(int area);
    int get_area(bool param);
    void set_state(ch_state state);
    int get_state();
    void setLastPacketID(int id);
    int getLastPacketID();
    void setPlayingDungeonIndex(int index);
    int getPlayingDungeonIndex();
    void setMoveSpace(UserSpace::T space);
    UserSpace::T getMoveSpace() const;
    int CheckMoveTown(int vill);
    void setBeforeAreaTime(long t);
    long getBeforeAreaTime() const;
    void setUseCraneStart(bool flag);
    unsigned char isUseCraneStart() const;
    void setForceDropFlag(bool flag);
    unsigned char getForceDropFlag();
    void setDungeonClearHackCheck(bool flag);
    unsigned char isDungeonClearHackCheck();
    void setAllowLocationGeoState(bool flag);
    unsigned char isAllowLocationGeoState();
    void setComboSkillSet(bool flag);
    unsigned char isComboSkillSet();
    bool lock4DataLoad();
    bool unlock4DataLoad();
    unsigned char isLocked4DataLoad();
    bool lockCreateCharac();
    bool unlockCreateCharac();
    unsigned char isLockedCreateCharac();
    bool lockDeleteCharac();
    bool unlockDeleteCharac();
    unsigned char isLockedDeleteCharac();
    void setAntibotKey(int key);
    void setClientVersion(unsigned int version);
    unsigned int getClientVersion();

    int Send(PacketGuard& packet);
    int SendPacket(eSendTarget target, PacketGuard& packet);
    int SendCmdPacket(eSendTarget target, ENUM_CMDPACKET cmd, int param);
    void SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET cmd, int param);
    void SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);
    int SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE space, int slot);
    int SendCmdOkPacket(ENUM_CMDPACKET cmd);
    int SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error);
    int SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PAY_TYPE pay);
    int SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error,
                            PacketGuard& packet);
    int SendCmdErrorPacket2Byte(ENUM_CMDPACKET cmd, unsigned short error);
    int SendCmdErrorPacketJpn(ENUM_CMDPACKET cmd, unsigned short error);
    void SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                unsigned char error);
    void SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                unsigned char error, PacketGuard& packet);
    int AntibotSend(PacketBuf& packet);
    void SendZeroExpPacket();
    void SendFatigue();
    void SendMaxEquipLevel();
    void SendOpenflag();
    void SendMoenyLog();
    void SendRevengeDungeon();
    void SendEffectItemInfo(unsigned int itemIdx);
    void SendMoneyFullReason(ENUM_MONEY_FULL_REASON reason, unsigned long a,
                             unsigned long b);
    void SendProperDungeonClearCount();
    void SendDungeonClearList();
    void SendTagCharacInfo();
    void SendCashData(bool param);
    void SendRestrictedGoods(bool param, int itemIdx);
    void send_skill_info();
    void send_equip(int slot);
    int send_itemspace(int space);
    void sendCharacOption();
    void send_pvp_record();
    void sendEventInfo();
    void sendNPCRelationShipFavor();
    void send_aura_avatar_option();
    void sendCharacQp();
    void sendCharacQuestPiece();
    void send_MissionList();
    void send_clear_quest_list();
    void processReturnUserQuestAutoClear();
    void send_RedeemInfo();
    void sendBingoData();
    void sendBingoAddData();
    void sendComboSkillList();
    void sendCharacLinkMessage();
    void sendBlueMarbleEnterCount();
    void sendNotiPcRoomPlayTime();
    void sendReturnUserFirstLogin();
    void send_ontime_reward_start_notify();
    void sendSecuRewardItem(Inven_Item* item, ENUM_SECURITY_REWARD_TYPE type,
                            int* param);
    void sendInformNoticeFlag();
    void sendDecorationObjectCnt(short count);

    // ---- 角色选择 / 登录数据 ----
    int SelectCharac(int slot, int param);
    int CreateCharac(SIG_CREATE_CHARAC* data);
    int DeleteCharac(unsigned int characNo);
    int ChangeCharacSlot(unsigned int a, unsigned int b);
    int checkSelectCharac(int slot);
    void ReturnToSelectCharacList(bool param);
    void LogoutCachedCharacter(unsigned char param);
    void SetCharacInfo(int slot, char* data);
    void SetCharacLevel(int level);
    void SetSkillLevel(int skillIdx, int level);
    void onSelectCharacter();
    void select_charac_set_visible_values(char value);
    void UpdateCharacView();
    void UpdateData();
    void UpdateLogout(bool param);
    void OnDisconnect();
    void Recv();
    void Parsing(int param);

    // ---- 基础访问器（GameStubs 桩替换） ----
    void DecreaseGuildPowerWarPoint(unsigned int point);
    int get_charac_no(int type);
    int get_charac_idx(unsigned int characNo) const;
    int get_character_job(unsigned int characNo) const;
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    const char* get_acc_name() const;
    bool CheckItemLock(int a, int b) const;
    int GetAccountLastPlayTime();
    ENUM_SERVER_GROUP GetServerGroup() const;
    bool isBlackUser(unsigned int accId) const;
    Secu_GoldControl* GetGoldControl();
    CDungeonGainedGold* getDungeonGainedGold();
    WongWork::CHackAnalyzer* getHackAnalyzer();
    CExpandEquipslot* GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const;
    CExpandEquipslot* GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE& type) const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    void AddDBLogItem(unsigned int itemIdx, unsigned int type,
                      ENUM_DBLOG_ITEM_TYPE value);
    int addAvatarItem(unsigned long itemIdx, char param2, char param3,
                      const char* ipgAgency, eAvatarItemAddReason reason);
    unsigned char getGmQuestFlag();
    int GetCurExpertJobLevel(int exp);
    WongWork::CUserPremium* GetPremiumInfo() const;
    UserQuest* getCurCharacQuestR() const;
    UserQuest* getCurCharacQuestW();
    bool gain_exp_sp(int exp, int& sp, int& sfp, eExpAddReason reason, int param, bool flag);
    void makeGuildSkillMessage(const char* skillName, int count);
    void set_grow_type(unsigned char firstGrow, unsigned char secondGrow,
                       char* name, eChangeGrowTypeReason reason);
    bool is_clear_stealingSkillMission() const;
    void givePvPSkillTree(int a, bool b, int c);
    int make_basic_info(char* buf, char type);             // ORIG 0x0865a44e（Ghidra undefined4，返回 1）
    int get_unique_id() const;
    int get_pvp_WinningRate_relateMission() const;  // ORIG 0x86626e0
    void* GetParty();
    int GetUserPosInParty();      // 0x08688638
    void* GetWarRoom();
    void set_growth_power_exp_reward(int reward);
    int get_growth_power_exp_reward();
    unsigned short getCurCharacTotalFatigue() const;
    unsigned short getCurCharacTotalMaxFatigue() const;

    // ---- G1-4 访问器/索引族（ORIG 逐符号转录） ----
    bool CheckInTrade();
    bool CheckInParty();
    bool checkInBlueMarble();
    bool CheckInWarRoom();
    bool checkInDeathTower();
    void setDeathTowerIndex(short idx);
    short getDeathTowerIndex();
    WongWork::CDeathTower* getDeathTower();          // ORIG 0x086552a4
    int Add_RedeemInfo(const Inven_Item& item, int count, bool isRedeem);  // ORIG 0x086472c0
    bool checkInBossTower();
    short getBossTowerIndex();
    void setBossTowerIndex(short index);
    bool checkInAdvanceAltar() const;
    short getAdvanceAltarIndex() const;
    unsigned short getBlueMarbleIndex();
    void setBlueMarbleIndex(short index);
    short GetPartyIndex();
    bool CheckInPvp();
    short GetPvpIndex();
    short GetWarRoomIndex();
    int GetTradeSpace();
    void SetPartyIndex(short index);
    void SetWarRoomIndex(short index);
    void SetSlotIDX(short idx);
    short GetIncreID() const;
    void SetIncreID(short id);
    void set_unique_id(unsigned short id);
    void set_position(unsigned short x, unsigned short y, char z);
    unsigned int get_posX();
    unsigned int get_posY();
    char get_direction();
    int GetUserState();
    int GetAge();
    bool IsOverBirthDay();
    unsigned char IsProgLogout();
    int GetSeedFromDate();
    void SetGMUpgradeMode(ENUM_GM_ITEM_UPGRADE mode);
    bool isGMUser();
    bool IsGameMasterMode() const;
    void SetGameMasterMode(bool flag);
    char GetEventCreateDnfReward();
    void SetPowerUp(bool flag);
    int SetETC(short key, int value);

    // ---- G1-4 CodeHackCheckStorage / 返回用户 / 预登录数据 ----
    void setTournamentAccount(bool flag);
    unsigned char isTournamentAccount();
    void setJoinedSchoolPointEvent(bool flag);
    void setOnlinePreliminaryServerGroup(ENUM_SERVER_GROUP group);
    void setOnlinePreliminaryCharacNo(unsigned int characNo);
    void setOnlinePreliminaryPlayType(int playType);
    int getOnlinePreliminaryPlayType();
    unsigned int getOnlinePreliminaryCharacNo();
    void SetPCRoomNo(unsigned int no);
    void SetFirstLogin();
    void SetReturnUser();
    void ResetReturnUser();
    unsigned char IsReturnUser();
    void SetReturnUserFirstUserLogin(bool flag);
    unsigned char IsReturnUserFirstLogin();
    int GetReturnUserExpireTime();
    void SetReturnUserExpireTime(int time);
    void EnableSaveMemberBonusFatigue();
    void incAccountMemberBonusFatigue(short value);
    void resetAccountMemberBonusFatigue();
    short getAccountMemberBonusFatigue();
    void resetAccountUsedFatigue();
    void incChattingMessageCount(int count);
    void setChattingMessageCount(int count);
    int getChattingMessageCount();
    void SetGameMasterCharacter(CGameMasterCharacter* pCharac);
    int getStdDropRate();
    Secu_AccountHacking* getAcccountHackInfo();
    void setHackUserTimer();
    void setHackUserFlag();
    unsigned char isHackUser() const;
    unsigned char isHackUserTimer() const;

    // ---- G1-4 签到 / 突破奖励 / 商城 / 日常 ----
    int getBreakAwayRewardItemId();
    void setBreakAwayRewardItemId(int id);
    int getBreakAwayRewardAItemCnt();
    void setBreakAwayRewardAItemCnt(int cnt);
    int getUserBreakAwayRewardLuckPoint();
    void setUserBreakAwayRewardLuckPoint(int point);
    int getBreakAwayRewardItemOrder();
    STBreakAwayRewards* getBreakAwayRewards();
    void setBreakAwayRewards(const STBreakAwayRewards& rewards);
    int getBreakAwayQuestTime();
    void setBreakAwayQuestTime(unsigned int time);
    int getBreakAwayDungeonClearCnt();
    void setBreakAwayDungeonClearCnt(int cnt);
    void subBreakAwayDungeonClearCnt();
    void setBreakAwayDungeonClear(bool flag);
    unsigned char isBreakAwayRewardData();
    void setGmQuestFlag(bool flag);
    void setWebAddress(char* addr);
    int GetCera();
    void SetCera(unsigned int cera);
    unsigned int UseCera(unsigned int cera);
    void setRegDate(unsigned int date);
    void setRegTime(long time);
    int getRegTime() const;
    void resetUsedCera();
    void addUsedCera(unsigned int cera);
    unsigned int getUsedCera();
    void SetVillageAttackedReward(bool flag);
    void setPcRoomAuth(bool flag);
    void fillUserInfo4Antibot();
    char* getUserInfo4Antibot();
    void SetAccountLastPlayTime(long time);
    void SetLoginTime(long time);
    long GetLoginTime();
    void SetLogInOutState(unsigned char state);
    unsigned char GetLogInOutState();
    int get_public_ip_address();
    void set_public_ip_address(int ip);
    void set_local_ip_address(int ip);
    void setHangameUser(bool flag);
    unsigned char isHangameUser();
    unsigned char IsPowerUp();
    void SetRating(float rating);
    void SetSchoolNo(unsigned int no);
    unsigned int GetSchoolNo();
    void SetSchoolName(const char* name);
    void SetDailyBadge(short* badge);
    void SetUserDetailInfo(char* a, char* b, char* c);
    unsigned char GetTutorialSkipable();
    void SetTutorialSkipable(unsigned char flag);
    int getHumanCertifyTimerKey();
    void setHumanCertifyErrorCnt(int cnt);
    unsigned char isSendMailCertified();
    void setRestingUserRestrict(bool flag);
    void setPunishTradeAlert(short alert);
    short getPunishTradeAlert();
    void resetDungeonClearHackCheck();
    unsigned char IsRecvEvent();
    int GetAutoClientRate();
    unsigned char isARSUserKick();
    void setSecuReward(bool flag);
    unsigned char NeedSecuReward();
    void setSecuType(unsigned char type);
    unsigned char getSecuType();
    unsigned char getRewardType();
    void setRewardType(unsigned char type);
    void setSecuGrade(char grade);
    void setFlagPresentAvengerTitle(bool flag);
    void incCharacSlotLimit(unsigned char count);
    void set_ontime_last_recv_idx(int idx);
    int get_ontime_last_recv_idx() const;
    void set_update_ontime_last_recv_idx();
    void setCheckPickUpRandomOptionItem(bool flag);
    unsigned char getCheckPickUpRandomOptionItem();
    void set_cera_item_order_no(unsigned int no);
    unsigned int get_cera_item_order_no();
    void setPcRoomPlayStartTime(unsigned int time);
    unsigned char IsDeleteEventDungeonInvitation() const;
    unsigned char IsUsedCoinInDungeon() const;
    void ResetCoinCount();
    int GetUsedCoinCount();
    void set_trick_area(int area);
    int get_before_area();
    // ---- G1-4 账号货物 / 锁 / 其余访问器 ----
    CAccountCargo* GetAccountCargo();
    bool IsExistAccountCargo();
    void DeleteCargo();
    void lock();
    void unlock();
    int getBackupSeed();
    unsigned char getPowerSide();
    char* getClientHWSpec();
    void SetGuildDBInfo(const STGuildDBInfoOnly& info);
    void ChangeGuildName(char* name);
    void SetGuildAgitFlag(bool flag);
    void SetGuildAgitDBInfo(const STGuildAgitDBInfo& info);
    void SetGuildCargoCapacity(unsigned int capacity);
    void SetTodayGuildMember(bool flag);
    unsigned char IsTodayGuildMember();
    void SetLastLoginCharacNo(int characNo);
    void SetLastLoginChannelNo(int channelNo);
    void SetValidLastLoginCharac(bool flag);
    void SetReportMannerlessUserCount(int count);
    int GetChecksumAnswerCount();
    void IncChecksumAnswerCount();
    int GetCheckConnInterval();
    void SetResumeChecksumTime(int time);
    short incre_check_count();
    short get_connect_count();
    void setTotalHackUserTradeGold(unsigned long long gold);
    void setTotalHackUserTradeCnt(unsigned int cnt);
    void SetRevivalTime(long time);
    long GetRevivalTime();
    void SetUseAPCPotionTime(long time);
    long GetUseAPCPotionTime();
    WongWork::CUserPremium* GetPremiumInfoW();

    // ---- G1-4 角色列表 / 视图 ----
    const char* get_charac_name(int idx) const;
    int GetCharacJob(int idx) const;
    int getCharacLevelByCharacNo(int characNo) const;
    const char* get_charac_name_by_charac_no(int characNo) const;
    void setCharacName(int characNo, const char* name);
    void SetCharacViewState(int state);
    void SetCharacViewState(int characNo, int state);
    int GetCharacViewState(int idx) const;
    bool SetCharacViewStateByCharacNo(int characNo, int state);

    // ---- GameStubs 迁移（ORIG 逐符号，强符号 T） ----
    void DisConnSig(DISCONN_SIG sig, bool flag, int param);                    // 0x086489f4
    void master_new_skill(stBuySkillInfo& info, bool flag);                    // 0x0866afa2
    bool master_new_skills(_Mastered_skill* skills,                          // 0x0866b53e
                           ENUM_SKILL_TREE_KIND kind);
    void ChangeGrowType_GM(int a, int b);                                      // 0x0867b048
    int AddItem(int itemIdx, int count, eItemAddReason reason,                 // 0x0867b6d4
                ENUM_ITEMSPACE& space, int slot);
    void ReCalcChattingEmoticon();                                             // 0x08689a22
    void ResetItemByScript(std::vector<std::pair<int, int> >& list);           // 0x08689010
    void SendChattingEmoticon();                                               // 0x08689b90
    void deleteDailyStackableItem(std::vector<std::pair<int, int> >& list,     // 0x0867e734
                                  int a, int b);
    void deleteSpecificItem(const std::vector<std::pair<int, int> >& list,     // 0x0867dba0
                            std::vector<std::pair<int, int> >& out);
    void doLinkCharacDisconnect();                                             // 0x08652c8e
    void giveup_panalty();                                                     // 0x086786be
    void log_out();                                                            // 0x08658910
    void prepareDisconnect();                                                  // 0x086487ea
    bool CheckQuestMonster(int a, int b, ENUM_QUEST_ENEMY_TYPE type);          // 0x0866cb04
    void FatigueUp(int v);                                                     // 0x08655c60
    void add_guild_pvp_result(int v);                                          // 0x0865c936
    void add_pvp_play_info(unsigned int a, unsigned int b);                    // 0x0865d986
    void add_pvp_result(bool flag, unsigned int* out);                         // 0x0865c678
    int gainGuildSkillExp(int exp);                                           // 0x0864fb3a
    bool IsGuildSkillLearn(int skillIdx);                                      // 0x0864fac4
    void update_pvp_point(int v);                                              // 0x0865cfd8
    void RecoverCoin(unsigned int v);                                          // 0x08657f10
    void RewardItem2DeleteInvalidItem(const std::string& name,                 // 0x086931c4
                                      const std::vector<std::pair<int, int> >& list);
    void AddDailyItem();                                                       // 0x08656caa
    void SendOneADayItemShopIndex(                                             // 0x0868aaea
        const std::vector<std::pair<int, int> >& list);
    void UpdateAuraAvatarOption(int a, int b);                                 // 0x0868de0a
    char is_equip_aura_avatar(char slot, int& out);                            // 0x0868dff8
    void processDelDailyItem();                                                // 0x0867e092
    void reqHumanCertify4ClearMap(bool flag);                                  // 0x086802b8
    void deleteSpecificItems(const std::vector<std::pair<int, int> >* list);   // (末尾)
    void ProcPremiumFatigue();                                                 // 0x0867cbe4
    bool IsPremiumUser() const;                                                // 0x0867cd20
    int SaveInventory();                                                       // 0x0864fe52

};

#endif  // GAME_CUSER_H_
