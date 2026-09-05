#ifndef GAME_CUSERCHARACINFO_H_
#define GAME_CUSERCHARACINFO_H_

// ============================================================================
// df_game_r 还原 —— CUserCharacInfo（G1-2 批次，CUser 基类 @ +0x00）。
// 布局依据 ORIG 构造（0x0864e15e）/ 析构（0x08697ac0）/ 各访问器逐成员推导：
//   +0x00..+0x0c 保存标志（isSave* 族）  +0x10 _Charac_info* m_selected
//   +0x14 _Charac_info* m_tag            +0x18 PvpResultType（0x84）
//   +0x9c std::map<int,GameResultType>   +0xb4 WongWork::CDungeonClear（0x18）
//   +0xcc char m_reliablePerson          +0xd0 int m_overTradeGoldCount
//   +0xd4 char m_saveCharacOption        +0xd8 long m_characStartPlayTick
//   +0xdc char m_userEventCharacterFlag  +0x79700 cUserHistoryLog（外部）
// _Charac_info（0x14d1）经本类访问器读写；其完整布局在 CUserCharacInfo.cpp
// 内以本地 struct 呈现（避免与 SkillSlot.h 的 _Charac_info 冲突）。
// ============================================================================
class SkillSlot;   // wave1: getTagCharacSkillR(653) 返回类型前置声明

#include <map>
#include <string>
#include <vector>

#include "GameTypes.h"       // cMyTrace / LogManager
#include "CSystemTime.h"
#include "CItem.h"          // Inven_Item / CItem
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeonClear.h"
#include "PacketBuf.h"      // GameWorld / G_GameWorld / G_CDataManager
#include "CPowerManager.h"   // 权威 CPowerManager 声明（GlobalData::s_power_manager 完整类型）
#include "GameResultType.h"   // 权威 GameResultType（唯一声明点；取代本文件内空占位）

// ---- 枚举（ORIG mangling 需要类型名；与 CInventory.h 共享守卫） ----
#ifndef DNF_ENUM_ITEMSPACE_DEFINED
#define DNF_ENUM_ITEMSPACE_DEFINED
enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_0 = 0
};
#endif

#ifndef DNF_ENUM_CHARAC_EXPAND_TYPE_DEFINED
#define DNF_ENUM_CHARAC_EXPAND_TYPE_DEFINED
enum ENUM_CHARAC_EXPAND_TYPE
{
    ENUM_CHARAC_EXPAND_TYPE_0 = 0,
    ENUM_CHARAC_EXPAND_TYPE_1 = 1,
    ENUM_CHARAC_EXPAND_TYPE_2 = 2,
    ENUM_CHARAC_EXPAND_TYPE_3 = 3,
    ENUM_CHARAC_EXPAND_TYPE_4 = 4,
    ENUM_CHARAC_EXPAND_TYPE_5 = 5,
    ENUM_CHARAC_EXPAND_TYPE_6 = 6,
    ENUM_CHARAC_EXPAND_TYPE_7 = 7,
    ENUM_CHARAC_EXPAND_TYPE_8 = 8,
    ENUM_CHARAC_EXPAND_TYPE_9 = 9,
    ENUM_CHARAC_EXPAND_TYPE_10 = 10,
    ENUM_CHARAC_EXPAND_TYPE_11 = 11,
    ENUM_CHARAC_EXPAND_TYPE_12 = 12,
    ENUM_CHARAC_EXPAND_TYPE_13 = 13,
    ENUM_CHARAC_EXPAND_TYPE_14 = 14,
    ENUM_CHARAC_EXPAND_TYPE_15 = 15,
    ENUM_CHARAC_EXPAND_TYPE_16 = 16,
    ENUM_CHARAC_EXPAND_TYPE_17 = 17,
    ENUM_CHARAC_EXPAND_TYPE_18 = 18,
    ENUM_CHARAC_EXPAND_TYPE_19 = 19
};
#endif

#ifndef DNF_ENUM_EWPSUBREASON_DEFINED
#define DNF_ENUM_EWPSUBREASON_DEFINED
enum eWPSubReason { eWPSubReason_0 = 0 };
#endif
#ifndef DNF_ENUM_EFPADDREASON_DEFINED
#define DNF_ENUM_EFPADDREASON_DEFINED
enum eFPAddReason { eFPAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_EFPSUBREASON_DEFINED
#define DNF_ENUM_EFPSUBREASON_DEFINED
enum eFPSubReason { eFPSubReason_0 = 0 };
#endif

// ---- 前向声明（真实定义属其它 TU / 后续批次） ----
struct _Charac_info;
class CUser;
class CInventory;
class CExpandEquipslot;
struct SIG_LOAD_ETC;

#include "PvpResultType.h"   // PvpResultType 权威定义（HdrFieldG ODR 收敛 2026-08-28：
                             //   本头原局部 pad 版类与 PvpResultType.h/PvpResultType.cpp
                             //   三重定义；实证布局完全同构：m_pvpGrade@0x14、m_winPoint@0x58，
                             //   +0x18..0x58 pad 区对应权威 m_avg[15]@0x18+m_count@0x54；
                             //   +0x10 Clear 置 -1 对应 m_field10；+0x5c 今日胜场【推断】）。

// GameResultType 权威定义见 GameResultType.h（上方已 include）；不再用空占位。

class CommonTime
{
public:
    void SetCreateTime(long t);
    void SetCreateTime(char* src);   // ORIG T 0x082a40dc（定义 CommonTime.cpp；setCharacInfoDetail 消费）
    void SetCurTime();   // ORIG _ZN10CommonTime10SetCurTimeEv（0x082a4224，ShowLogo 使用）
    operator long() const;
    void clear();   // ORIG W 0x086947d8 _ZN10CommonTime5clearEv（_Charac_info::Reset 消费；定义 owner 待分派）
    void GetCreateTime(char* buf) const;   // ORIG T 0x082a41e0（定义 CommonTime.cpp；getCharacInfoDetail 消费）

    char m_pad[0x14];   // 与 _Charac_info +0x39 对齐（0x14 字节）
};

class CReliablePerson
{
public:
    unsigned int GetTradeGoldDaily() const;
    unsigned int GetTradeGoldTotal() const;
    unsigned int GetTradeGoldTotalBillion() const;
    void SetTradeGoldDaily(unsigned int gold);
    void IncrementTradeGold(unsigned int gold);
    void InitTradeGold(unsigned int a, unsigned int b, unsigned int c);   // ORIG T 0x086453b8（LinkClosureA-2 setCharacInfoDetail 消费；定义 owner 待分派）

    char m_pad[0xc];    // +0x00
    int m_schoolPoint;  // +0x0c（calcCurCharacSchoolPoint 读写 +0x71）
};

namespace expert_job { class CExpertJob; }  // 完整定义见 CExpertJob.h

// online_preliminary 域（PlayCountInfoEx/COnlinePreliminaryTeam/
// COnlinePreliminary）已迁至唯一权威头 OnlinePreliminary.h
// （OnlyOrig_OnlinePvp 批，2026-08-27，经 Main 批准）。
#include "OnlinePreliminary.h"

class seriaRoom_AniDeco
{
public:
    seriaRoom_AniDeco();

    short m_field0;   // +0x00
    short m_field2;   // +0x02
};

class stAuraAvatarOption
{
public:
    int m_value0;   // +0x00
    int m_value1;   // +0x04
    int m_value2;   // +0x08

    // ORIG W _ZN18stAuraAvatarOption5resetEv @0x844d616：memset(this,0,0xc)。
    // （DB_LoadEtc::LoadAuraAvatarOption 0x844d616 调用点忠实转写；内联定义
    //   与 ORIG 弱符号同语义，无布局变化。）
    void reset() { memset(this, 0, 0xc); }
};

class CharacterOption
{
public:
    CharacterOption();   // ORIG W _ZN14CharacterOptionC1Ev @0x0822abec（memset 0xff,0x200；LinkE2 定义）
    char m_pad[0x200];
};

class ContinuousItemInfo
{
public:
    ContinuousItemInfo() : m_itemIdx(0), m_expireTime(0) {}  // ORIG W 0x08695c48（两字段清零）
    int m_itemIdx;             // +0x00
    unsigned int m_expireTime; // +0x04
};

struct WpBonusPointResult
{
    WpBonusPointResult();   // ORIG W _ZN18WpBonusPointResultC1Ev 08695c78（定义在 WpBonusPointResult.cpp，调用 clear）

    int m_base;   // +0x00
    int m_add;    // +0x04
};

struct ExpBonusPointResult
{
    ExpBonusPointResult();  // ORIG W _ZN19ExpBonusPointResultC1Ev/C2Ev @0x08695ca4（call clear）
    void clear();           // ORIG W _ZN19ExpBonusPointResult5clearEv @0x08695cb8（+0x00/+0x04 清零）
    int m_base;   // +0x00
    int m_add;    // +0x04
};

struct stBloodBestRecord
{
    int m_round;         // +0x00
    unsigned int m_time; // +0x04
};


class InventoryMemory;
class PacketGuard;

// 与 CInventory.h 共享守卫（避免双定义）
#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

// ============================================================================
// CCargo（角色仓库，G4 商城/交易域批次，ORIG 布局 0x14）
// 布局依据 CCargo::C1 (0x86949da) / reset (0x850b0c2) / set_parent (0x86949fe)
// / SetInventoryMemory (0x86949c6) / get_cargo_start (0x84b4262) 推导：
//   +0x00 CUser* m_pParent       +0x04 Inven_Item* m_pItems（0x98 槽）
//   +0x08 int m_itemCount        +0x0c int m_capacity
//   +0x10 char m_sorted          （总尺寸 0x14，_Charac_info +0xdaa 内嵌）
// 物品缓冲 0x2438 = 0x98 * 0x3d，由 SetInventoryMemory 指向
// InventoryMemory +0x9d07（角色扩展仓库区）。
// ============================================================================
class CCargo
{
public:
    CCargo();
    ~CCargo();

    // ---- 生命周期 / 绑定 ----
    void set_parent(CUser* user);
    bool set_cargo(void* data);
    bool get_cargo(void* out) const;
    void SetCapacity(int capacity);
    void SetInventoryMemory(InventoryMemory* memory);
    void reset();

    // ---- 槽位访问 ----
    Inven_Item get_cargo_slot(int idx) const;
    Inven_Item* GetCargoRef(int idx) const;
    Inven_Item* GetCargoRef(int idx);
    int GetCargoData(int itemIdx, Inven_Item& out) const;
    const Inven_Item* get_cargo_start() const;
    int GetCapacity() const;
    int GetItemCount() const;
    bool IsOperatorable(int idx) const;

    // ---- 插入 / 删除 / 移动 ----
    int insert_item(const Inven_Item& item);
    int insert_item_special_slot(Inven_Item& item, int slot);
    int tryInsertItemSpecialSlot(const Inven_Item& item, int slot) const;
    int _tryAddStackItem(const Inven_Item& item, int slot) const;
    int _AddStackItem(const Inven_Item& item, int slot);
    void _AddItem(const Inven_Item& item, int slot);
    bool delete_item(int idx, int count, eItemDelReason reason);  // ORIG bool 返回
    int move_item(int from, int to);
    void update_item(int idx, const Inven_Item& item);
    void _ResetSlot(int idx, eItemDelReason reason);

    // ---- 查询 ----
    int check_slot_empty(int idx) const;
    int check_item_exist(int itemIdx) const;
    int get_empty_slot() const;
    int _GetItemCount();
    void calcCargoItemCount();
    int _IsExistEquipItem(int itemIdx, int slot);
    int FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const;

    // ---- 扩容 / 排序 ----
    int UpgradeCargo(int capacity);
    int isUpgradable(int capacity) const;
    void sort();
    static int _CompareSlot(const void* a, const void* b);

    // ---- 发包 ----
    void SendItemLockList() const;
    int MakeItemList(PacketGuard* packet) const;
    void MakeItemPacket(PacketGuard* packet, int slot) const;

    CUser* m_pParent;       // +0x00
    Inven_Item* m_pItems;   // +0x04（0x98 槽；SetInventoryMemory 绑定）
    int m_itemCount;        // +0x08
    int m_capacity;         // +0x0c
    char m_sorted;          // +0x10
    char m_pad[3];          // +0x11
};

class CTitleBook;   // 权威定义见 CTitleBook.h（本头经 CDataManager.h include）

int GetTenThousandPercentage(int value, int percent);

// ---- 全局对象（真实声明在 GlobalData.h；CInventory.h 用 namespace 形态，
//      此处合并声明避免同时包含 class/namespace 冲突） ----
extern int DEFAULT_MAX_FATIGUE;

namespace GlobalData
{
extern CSystemTime s_systemTime_;
extern CPowerManager* s_power_manager;
}

// ============================================================================
// _Charac_info 布局视图（ORIG 尺寸 0x14d1，pack(1)）。
// 完整 _Charac_info 定义在 CUser.h（vector<_Charac_info> 角色列表宿主）；
// 本视图供 CUserCharacInfo 访问器以具名字段读写 m_selected/m_tag 指向的
// 同一块 ORIG 内存。偏移证据：docs/class_func_reports/CUserCharacInfo/ 各
// 函数汇编 + cusercharac_worklist.tsv（orig_addr 逐函数核对）。
// STL 成员用 pack 外包装类型固定布局（pack(1) 会使首次实例化缩为 0x15）。
// ============================================================================
struct CharacInfoCoolList
{
    std::vector<ContinuousItemInfo> m_list;
};
struct CharacInfoEffectList
{
    std::vector<ContinuousItemInfo> m_list;
};
struct CharacInfoBloodMap
{
    std::map<int, stBloodBestRecord> m_map;
};

// m_growType 位域（ORIG getCurCharFirst/SecondGrowType 为真实成员位域提取；
// setCurCharGrowthType/setCurCharChangingGrowthType 汇编 shl/sar 有符号位域
// 形态，0x864e2d8 / 0x864e34e）
union GrowTypeBits
{
    unsigned char byte;
    struct
    {
        signed char first : 4;   // bit0..3
        signed char second : 3;  // bit4..6
        signed char third : 1;   // bit7
    };
};

#pragma pack(push, 1)
struct CharacInfoFields
{
    int m_characNo;                    // +0x00（getCurCharacNo）
    char m_name[0x1e];                 // +0x04..0x22（getCurCharacName）
    unsigned char m_vill;              // +0x22
    unsigned char m_villPvP;           // +0x23
    char m_pad24;                      // +0x24
    unsigned char m_villPrev;          // +0x25
    unsigned char m_job;               // +0x26
    short m_level;                      // +0x27
    GrowTypeBits m_growType;           // +0x29（位域）
    GrowTypeBits m_growTypeChanging;  // +0x2a
    int m_exp;                         // +0x2b
    int m_partyBonusExp;               // +0x2f
    unsigned short m_fatigue;          // +0x33
    unsigned short m_maxFatigue;       // +0x35
    unsigned short m_fatigueGrownUpBuff;   // +0x37
    CommonTime m_createTime;           // +0x39（0x14；set_charac_createtime）
    unsigned short m_usedFatigue;      // +0x4d
    unsigned short m_fatigueBattery;   // +0x4f
    unsigned int m_tutorialFlags;      // +0x51
    char m_tutorialFlags2[0x10];       // +0x55
    CReliablePerson m_reliablePerson;  // +0x65（0x10）
    unsigned char m_stamina;           // +0x75
    unsigned char m_backupStamina;     // +0x76
    unsigned short m_premiumFatigue;   // +0x77
    unsigned short m_maxPremiumFatigue;// +0x79
    long m_lastPlayTick;               // +0x7b
    long m_lastPlayTickPowerWar;       // +0x7f
    char m_pad83[5];                   // +0x83..0x88
    char m_addInfo[0x52];              // +0x88..0xda
    char m_inven[0x892 - 0xda];        // +0xda..0x892（CInventory 内嵌）
    char m_skill[0xdaa - 0x892];       // +0x892..0xdaa
    char m_cargo[0xdbe - 0xdaa];       // +0xdaa..0xdbe（CCargo 内嵌，0x14）
    int m_guildkey;                    // +0xdbe
    unsigned char m_guildCreateRight;  // +0xdc2
    unsigned short m_guildCreateFlag;  // +0xdc3
    char m_guildName[0x17];            // +0xdc5
    char m_guildAddr[0xd];             // +0xddc
    int m_guildExp;                    // +0xde9
    unsigned short m_guildTempExp;      // +0xded
    int m_guildTodayExp;               // +0xdef
    unsigned char m_guildMemberGrade;  // +0xdf3
    unsigned int m_dungeonHelpAbuseRatio;  // +0xdf4
    unsigned int m_dungeonHelpAbuseExp;    // +0xdf8
    short m_levelBeforeDungeon;        // +0xdfc
    unsigned int m_helpAbuseComputedRatio; // +0xdfe
    long m_loginTick;                  // +0xe02
    int m_finishPoint;                 // +0xe06
    int m_finishPointTotal;            // +0xe0a
    float m_warAreaKill[5];            // +0xe0e（0x14，InitFinishPoint 清零区）
    int m_memberkey;                   // +0xe22
    int m_connectUpperMember;          // +0xe26
    int m_memberBonusExp;              // +0xe2a
    int m_fatigueBufBonusExp;          // +0xe2e
    unsigned char m_uppermemberExpLevel;  // +0xe32
    int m_memberPayTexMoney;           // +0xe33
    int m_memberPayTexFatigue;         // +0xe37
    char m_overEquipFlag;              // +0xe3b
    int m_blackCount;                  // +0xe3c
    union
    {
        unsigned char m_visibleValues;              // +0xe40（整字节）
        struct
        {
            signed int visible : 1;                 // bit0
            signed int growAvatar : 1;              // bit1
            signed int teleport : 1;                // bit2
            signed int m_padBits : 5;
        } m_visibleBits;
    };
    int m_assaultPlace;                // +0xe41
    int m_assaultCount;                // +0xe45
    int m_chaosPoint;                  // +0xe49
    int m_chaosExp;                    // +0xe4d
    int m_chaosModeCount;              // +0xe51
    int m_chaosKillCount;              // +0xe55
    int m_chaosDieCount;               // +0xe59
    int m_chaosDieTime;                // +0xe5d
    int m_chaosStateTime;              // +0xe61
    unsigned char m_superState;        // +0xe65
    unsigned char m_ghost;             // +0xe66
    unsigned char m_leaveTimeout;      // +0xe67
    int m_chaosKillTime;               // +0xe68
    int m_assaultFatigueCount;         // +0xe6c
    unsigned char m_opencoin;          // +0xe70
    int m_luckPoint;                   // +0xe71
    int m_luckPointUpdateTimeCount;    // +0xe75
    int m_dungeonPlayCount;            // +0xe79
    int m_expertJobType;               // +0xe7d
    int m_expertJobExp;                // +0xe81
    expert_job::CExpertJob* m_expertJob;   // +0xe85
    online_preliminary::COnlinePreliminaryTeam* m_onlinePreliminaryTeam;  // +0xe89
    unsigned short m_powerWarHP;       // +0xe8d
    unsigned char m_joinPower;         // +0xe8f
    char m_padE90;                     // +0xe90
    long m_powerWarProcessLastUpdateTime;  // +0xe91
    long m_powerWarLastPlayTime;       // +0xe95
    char m_padE99;                     // +0xe99
    CharacInfoCoolList m_coolTimeItemList;   // +0xe9a
    CharacInfoEffectList m_effectItemList;   // +0xea6
    unsigned char m_isAffectedExpDouble;   // +0xeb2
    unsigned char m_oneDayLetheFlag;       // +0xeb3
    unsigned char m_isInitSkillFlag;       // +0xeb4
    unsigned char m_oneDayLetheFlag2ND;    // +0xeb5
    unsigned char m_isInitSkillFlag2ND;    // +0xeb6
    unsigned char m_firstTimeLogin;        // +0xeb7
    unsigned char m_saveDemensionInout;    // +0xeb8
    unsigned char m_demensionInout[6];     // +0xeb9（ORIG 宽度实证：CParty
                                           //   CheckEnterDimensionDungeon 0859f510
                                           //   `movzbl 0xeb9(%eax,%edx,1)`；访问器
                                           //   getDemensionInoutValue 返回 unsigned char）
    unsigned char m_saveBloodInout;        // +0xebf
    unsigned char m_ultimateInout[3];      // +0xec0（ORIG 宽度实证：CParty
                                           //   check_dungeon_start 0859fa8f `movzbl
                                           //   0xec2(%eax)`=本数组[2]；访问器
                                           //   getUltimateInoutValue 返回 unsigned char）
    unsigned char m_bloodBestRecordUpdate; // +0xec3
    int m_bloodBestRound;                  // +0xec4
    unsigned int m_bloodBestTime;          // +0xec8
    CharacInfoBloodMap m_bloodBestMap;       // +0xecc（0x18）
    unsigned char m_isExpItemAffected;     // +0xee4
    float m_expAffectRate;                 // +0xee5
    bool m_isDisguiseCharac;               // +0xee9
    unsigned char m_disguiseKind;          // +0xeea
    unsigned short m_disguiseIndex;        // +0xeeb
    char m_backupSkill[0x199];             // +0xeed
    char m_backupSkill2ND[0x199];          // +0x1086
    unsigned short m_powerWarPoint;        // +0x121f
    int m_powerWarAssaultCount;            // +0x1221
    int m_powerWarAssaultVictoryCount;     // +0x1225
    unsigned short m_straightVictories;    // +0x1229
    unsigned char m_isEndLetheAtLogin;     // +0x122b
    unsigned short m_curCharacEmotion;     // +0x122c
    long m_lastItemLimitEditionBuyTime;    // +0x122e
    unsigned short m_curConditionEventStep;    // +0x1232
    unsigned short m_curConditionEventRewardStep;  // +0x1234
    unsigned short m_properDungeonClearCount;     // +0x1236
    bool m_properDungeonUser;              // +0x1238
    char m_skillTreeIndex;                 // +0x1239
    unsigned short m_curCharacEventItemCount;    // +0x123a
    unsigned short m_curCharacEventAddItemCount; // +0x123c
    char m_curExEquipSlotStat;             // +0x123e
    unsigned char m_eventCharacterGrowtype;     // +0x123f
    char m_pad1240;                    // +0x1240
    unsigned char m_memberDungeonFlag;     // +0x1241
    unsigned char m_revengeCount;          // +0x1242
    unsigned char m_revengeDungeonCount;   // +0x1243
    unsigned char m_villageAttackDBUpdate; // +0x1244
    char m_linkCharacSlotIndex;            // +0x1245
    unsigned char m_characLinkType;        // +0x1246
    int m_characLinkBonusExp;              // +0x1247
    int m_characLinkBonusGold;             // +0x124b
    char m_pad124f[5];                     // +0x124f
    char m_skillCommand[0x18];             // +0x1254
    unsigned short m_memberBonusFatigue;   // +0x126c
    unsigned char m_guildSecede;           // +0x126e
    int m_lastPlayDungeonIndex;            // +0x126f
    unsigned char m_upgradeSeparateTrue;   // +0x1273
    unsigned char m_upgradeSeparateFalse;  // +0x1274
    unsigned char m_curEquipslotFlag;      // +0x1275
    unsigned char m_curChannelEquipslotSwitch;  // +0x1276
    unsigned char m_curExpandEquipslotSwitch;   // +0x1277
    unsigned char m_bringUpRewardFlag;     // +0x1278
    unsigned char m_curUseBringUpMaterialCount; // +0x1279
    unsigned char m_curUseBringUpMaterialTotal; // +0x127a
    unsigned char m_growthPowerExpRewardRight;  // +0x127b
    int m_seriaBufBonusExp;                // +0x127c
    unsigned char m_growthPowerReward;     // +0x1280
    union
    {
        stAuraAvatarOption m_auraAvatarOption;  // +0x1281（ORIG 结构复制形态）
        int m_auraAvatarOptionArr[3];           // +0x1281（ORIG 下标形态）
    };
    int m_premiumBonusExp;                 // +0x128d
    int m_weekendBonusExp;                 // +0x1291
    char m_pad1295[8];                     // +0x1295
    WpBonusPointResult m_wpBonus;    // +0x129d（8 字节）
    ExpBonusPointResult m_expBonus;   // +0x12a5（8 字节）
    int m_chaosResponTime;                 // +0x12ad
    int m_assaultMoney;                    // +0x12b1
    unsigned short m_assaultedUserId;      // +0x12b5
    CharacterOption m_characterOption;     // +0x12b7
    unsigned char m_strikerSkillIndex;     // +0x14b7
    int m_tagCharacNo;                     // +0x14b8
    char m_pad14bc;                        // +0x14bc
    unsigned char m_blueMarbleEnterCount;  // +0x14bd
    int m_blueMarbleUserDeadCount;         // +0x14be
    int m_maxEquipLevel;                   // +0x14c2
    int m_characViewState;                 // +0x14c6
    seriaRoom_AniDeco m_seriaRoomAniDeco;  // +0x14ca
    unsigned char m_cubePremiumContract;   // +0x14ce
    unsigned char m_cubePremiumCount;      // +0x14cf
    unsigned char m_cubePremiumSaved;      // +0x14d0
};
#pragma pack(pop)
static_assert(sizeof(CharacInfoFields) == 0x14d1,
              "_Charac_info layout drift (ORIG size 0x14d1 @CUserCharacInfo worklist)");

// ============================================================================
// CUserCharacInfo
// ============================================================================
class CUserCharacInfo
{
public:
    CUserCharacInfo();
    ~CUserCharacInfo();

    // ---- 成员布局（ORIG 构造/访问器推导） ----
    char m_saveCharacInfo;         // +0x00
    char m_saveCharacStat;         // +0x01
    char m_saveCharacEtc;          // +0x02
    char m_saveCharacManageInfo;   // +0x03
    char m_saveInven;              // +0x04
    char m_saveSkill;              // +0x05
    char m_savePvP;                // +0x06
    char m_saveQuest;              // +0x07
    char m_field08;                // +0x08
    char m_saveDungeonClear;       // +0x09
    char m_saveBattleRecord;       // +0x0a
    char m_saveCargo;              // +0x0b
    char m_field0c;                // +0x0c
    char m_pad0d[3];               // +0x0d..0x0f
    _Charac_info* m_selected;      // +0x10
    _Charac_info* m_tag;           // +0x14
    PvpResultType m_pvpResult;     // +0x18（0x84）
    std::map<int, GameResultType> m_battleRecord;  // +0x9c（0x18）
    WongWork::CDungeonClear m_dungeonClear;        // +0xb4（0x18）
    bool m_reliablePerson;         // +0xcc
    char m_padcd[3];               // +0xcd..0xcf
    int m_overTradeGoldCount;      // +0xd0
    bool m_saveCharacOption;       // +0xd4
    char m_padd5[3];               // +0xd5..0xd7
    long m_characStartPlayTick;    // +0xd8
    unsigned char m_userEventCharacterFlag;     // +0xdc
    // 基类尺寸 = 0xe0（ORIG CUser 布局：CNetwork<4096,450000> @ +0xe0）。
    // cUserHistoryLog 位于派生 CUser +0x79700，CUserCharacInfo 方法经
    // `(char*)this + 0x79700` 显式访问（原版源码手法，无需把基类垫大）。
    char m_padDD[0xe0 - 0xdd];     // +0xdd..0xe0

    // ---- 保存标志 ----
    char isSaveCharacInfo();
    char IsSaveCharacStat();
    char isSaveCharacEtc();
    char IsSaveCharacManageInfo();
    char isSaveInven();
    char isSaveSkill();
    char isSavePvP();
    char isSaveQuest();
    char isSaveDungeonClear();
    char isSaveBattleRecord();
    char isSaveCargo();
    char isSaveCharacOption();
    void enableSaveCharacInfo();
    void enableSaveCharacStat();
    void enableSaveCharacEtc();
    void enableSaveCharacManageInfo();
    void enableSaveInven();
    void enableSaveSkill();
    void enableSavePvP();
    void enableSaveQuest();
    void enableSaveDungeonClear();
    void enableSaveBattleRecord();
    void enableSaveCargo();
    void setSaveCharacOption(bool flag);
    void disableSaveCharacInfo();
    void disableSaveCharacStat();
    void disableSaveCharacEtc();
    void disableSaveCharacManageInfo();
    void disableSaveInven();
    void disableSaveSkill();
    void disableSavePvP();
    void disableSaveQuest();
    void disableSaveDungeonClear();
    void disableSaveBattleRecord();
    void disableSaveCargo();
    void resetSaveFlags();

    // ---- 当前角色指针 ----
    _Charac_info* getCurCharacW();
    const _Charac_info* getCurCharacR() const;
    _Charac_info* getTagCharacW();
    const _Charac_info* getTagCharacR() const;
    void setCurCharac(_Charac_info* p);
    void setTagCharac(_Charac_info* p);

    // ---- 内嵌对象 ----
    PvpResultType* getPVPResultW();
    PvpResultType* getPVPResultR() const;
    PvpResultType* getPVPResultRefW();
    const PvpResultType* getPVPResultRefR() const;
    std::map<int, GameResultType>* getBattleRecordRefW();
    const std::map<int, GameResultType>* getBattleRecordRefR() const;
    WongWork::CDungeonClear* getDungeonClearRefW();
    const WongWork::CDungeonClear* getDungeonClearRefR() const;
    int get_pvp_result_exp_point() const;
    int get_pvp_result_oldexp_point() const;
    int get_pvp_grade() const;
    int GetWinPoint();
    char GetReliablePerson();
    void SetReliablePerson(bool flag);
    int GetOverTradeGoldCount();
    void SetOverTradeGoldCount(unsigned int count);
    long GetCharacStartPlayTick();
    void SetCharacStartPlayTick(long tick);
    unsigned char getUserEventCharacterFlag();
    void setUserEventCharacterFlag(unsigned char flag);
    void useWinPoint(int point, eWPSubReason reason);
    unsigned short getPowerWarHP();

    // ---- 背包 / 技能 / 仓库 / 附加信息 ----
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    CInventory* getCurCharacInvenRefW();   // ORIG 返回 m_inven（CInventory* 口径）
    const CInventory* getCurCharacInvenRefR() const;   // ORIG 返回 m_inven（CInventory* 口径）
    char* getCurCharacSkillW();
    const char* getCurCharacSkillR() const;
    CCargo* getCurCharacCargoW();
    const CCargo* getCurCharacCargoR() const;
    Inven_Item* getTagCharacInvenW();
    CInventory* getTagCharacInvenR() const;   // ORIG W 0x0822f110
    char* getTagCharacSkillW();
    SkillSlot* getTagCharacSkillR() const;    // ORIG W 0x0822f120
    CCargo* getTagCharacCargoW();
    const CCargo* getTagCharacCargoR() const;
    void* getCurCharacAddInfoW();
    const void* getCurCharacAddInfoR() const;
    void* getCurCharacAddInfoRefW();
    const void* getCurCharacAddInfoRefR() const;
    const void* getTagCharacAddInfoR() const;
    unsigned int getCurCharacCoin() const;
    int getCurCharacMoney() const;
    unsigned int getCurCharacPayCoin() const;
    unsigned int getCurCharacEventCoin() const;
    bool FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot,
                      CUser* user) const;
    bool isExistItem(int itemIdx);
    void checkBonusPoint();
    void resetBonusPoint();
    void getWpBonusPoint(WpBonusPointResult& out);
    void getExpBonusPoint(ExpBonusPointResult& out);
    void GetContinuousItemInfo(SIG_LOAD_ETC* p) const;
    std::vector<ContinuousItemInfo>* GetCurCharacCoolTimeItemList();
    std::vector<ContinuousItemInfo>* GetCurCharacEffectItemList();
    const std::vector<ContinuousItemInfo>* GetCurCharacCoolTimeItemListR() const;
    const std::vector<ContinuousItemInfo>* GetCurCharacEffectItemListR() const;
    void ClearUselessCooltimeItem(unsigned int expireTime);
    void ClearUselessEffectItem(unsigned int expireTime);
    bool EnablePeriodExpItem();
    bool EnablePeriodExpDouble();
    unsigned char IsExpItemAffected();
    float GetExpAffectRate();
    void SetExpAffectItemEffect(bool flag, float rate);
    void SetIsAffectedExpDouble(bool flag);

    // ---- 基础属性 ----
    int getCurCharacNo() const;
    const char* getCurCharacName() const;
    int get_charac_level() const;
    int get_charac_job() const;
    int get_charac_exp() const;
    int getCurCharacExp() const;
    void addCurCharacExp(int exp);
    void subtractCurCharacExp(int exp);
    void setCurCharacExp(int exp);
    int get_level_up_exp(int level);
    void incCurCharacLevel();
    void decCurCharacLevel();
    void setCurCharacLevel(int level);
    int get_charac_level_before_dungeon() const;
    void set_level_before_dungeon(short level);
    void reset_level_before_dungeon();
    unsigned short GetFatigue() const;
    unsigned short getCurCharacFatigue() const;
    void setCurCharacFatigue(int fatigue);
    unsigned short getCurCharacMaxFatigue() const;
    unsigned short GetFatigueBatteryCharging();
    void IncFatigueBatteryCharging(short value);
    void DecFatigueBatteryCharging(short value);
    unsigned short getCurCharacUsedFatigue();
    void incCurCharacUsedFatigue(unsigned short value);
    void resetCurCharacUsedFatigue();
    unsigned short getCurCharacPremiumFatigue() const;
    void setCurCharacPremiumFatigue(unsigned short value);
    unsigned short getCurCharacMaxPremiumFatigue() const;
    void setCurCharacMaxPremiumFatigue(unsigned short value);
    unsigned short getCurCharacFatigueGrownUpBuff() const;
    void setCurCharacFatigueGrownUpBuff(unsigned short value);
    void decCurCharacFatigueGrownUpBuff(unsigned int value);
    void resetCharacFatigueGrownUpBuff();
    void setWeekendBonusFatigue(unsigned short value);
    unsigned short getCurCharacMemberBonusFatigue();
    void incCurCharacMemberBonusFatigue(short value);
    void resetCurCharacMemberBonusFatigue();
    unsigned char getCurCharacStamina() const;
    void setCurCharacStamina(unsigned char value);
    void setCurCharacBackupStamina(unsigned char value);
    void addCurcharacStamina(unsigned char value);
    unsigned char getCurCharacGrowType() const;
    char getCurCharFirstGrowType() const;
    char getCurCharSecondGrowType() const;
    void setCurCharGrowthType(char first, char second);
    void setCurCharChangingGrowthType(char first, char second);
    int getCurCharacSchoolPoint();
    void calcCurCharacSchoolPoint(int fatigue);
    void resetCurCharacSchoolPoint();
    int get_charac_party_bonus_exp() const;
    void set_charac_party_bonus_exp(int exp);
    int get_charac_member_bonus_exp() const;
    void set_charac_member_bonus_exp(int exp);
    int get_charac_fatigue_buf_bonus_exp() const;
    void set_charac_fatigue_buf_bonus_exp(int exp);
    int get_charac_seria_buf_bonus_exp() const;
    void set_charac_seria_buf_bonus_exp(int exp);
    long getCurCharacLastPlayTick() const;
    void setCurCharacLastPlayTick(long tick);
    long getCurCharacLastPlayTickPowerWar() const;
    void setCurCharacLastPlayTickPowerWar(long tick);
    int GetCharacPlayTick();
    long getCurCharacCreateTime() const;
    void set_charac_createtime(long t);
    int get_charac_10_level_section() const;
    int getRequestRenameState() const;
    void setCharacViewState(int state);
    int GetCurCharacMaxEquipLevel() const;
    void SetCurCharacMaxEquipLevel(int level);
    int GetCurCharacLuckPoint();
    void SetCurCharacLuckPoint(int point);
    void IncCurCharacLuckPoint(int point);
    void DecCurCharacLuckPoint(int percent);
    int GetCurCharacLuckPointUpdateTimeCount();
    void IncCurCharacLuckPointUpdateTimeCount();
    void SetCurCharacLuckPointUpdateTimeCount(int count);
    int getPremiumBonusExp() const;
    int setPremiumBonusExp(unsigned int exp, int percent);
    void resetPremiumBonusExp();
    int getWeekendBonusExp();
    void setWeekendBonusExp(unsigned int exp, int percent);
    void resetWeekendBonusExp();

    // ---- 工会 ----
    int get_charac_guildkey();
    void set_charac_guildkey(unsigned int key);
    int get_guild_exp();
    void add_guild_exp(int exp);
    void set_guild_exp(int exp);
    void reset_guild_exp();
    int get_guild_temp_exp();
    void add_guild_temp_exp(int exp);
    void reset_guild_temp_exp();
    int get_guild_today_exp();
    void add_guild_today_exp(int exp);
    void reset_guild_today_exp();
    unsigned char get_guild_create_right();
    void set_guild_create_right(unsigned char right);
    unsigned char getGuildMemberGrade();
    void setGuildMemberGrade(unsigned char grade);
    bool isSetGuildCreateFlag(unsigned short flag);
    void setGuildCreateFlag(unsigned short flag);
    void resetGuildCreateFlag();
    char* getCreateGuildName();
    char* getCreateGuildAddr();
    void resetTempGuildNameAddr();
    void resetCharGuildInfo();
    int get_charac_memberkey();
    void set_charac_memberkey(unsigned int key);
    int get_connect_upper_member();
    void set_connect_upper_member(unsigned int member);
    bool is_connect_upper_member();
    unsigned char get_uppermember_exp_level();
    void set_uppermember_exp_level(unsigned char level);
    int get_member_pay_tex_money_to_upper() const;
    void set_member_pay_tex_money_to_upper(int money);
    void add_member_pay_tex_money_to_upper(int money);
    int get_member_pay_tex_fatigue_to_upper() const;
    void set_member_pay_tex_fatigue_to_upper(int fatigue);
    void add_member_pay_tex_fatigue_to_upper(int fatigue);
    unsigned char getCurCharacGuildSecede();
    void setCurCharacGuildSecede(char flag);

    // ---- 村庄 / 战斗区 / PvP / 混沌 ----
    signed char getCurCharacVill() const;
    unsigned char getCurCharacVill4Save() const;
    unsigned char getCurCharacVillPrev() const;
    void setCurCharacVill(char vill);
    unsigned char isJoinPowerWar() const;
    void setCurrCharacJoinPower(char power);
    void SetIsInitSkillFlag(bool flag);
    void SetIsInitSkillFlag2ND(bool flag);
    void IncVillageAttackCount();
    void setPowerWarHP(unsigned short hp);
    unsigned short GetUserPowerWarPoint();
    void IncreasePowerWarPoint(short point);
    void DecreasePowerWarPoint(short point);
    int GetPowerWarAssaultCount();
    int GetPowerWarAssaultVictoryCount();
    void IncPowerWarAssaultCount();
    void IncPowerWarAssaultVictoryCount();
    void ResetPowerWarAssaultRecord();
    long GetPowerWarLastPlayTime();
    void SetPowerWarLastPlayTime(long t);
    long GetPowerWarProcessInfoLastUpdateTime();
    void SetPowerWarProcessInfoLastUpdateTime(long t);
    int GetCurCharacChaosPoint();
    void SetCurCharacChaosPoint(int point);
    void AddCurCharacChaosPoint(int point);
    void SubCurCharacChaosPoint(int point);
    void GmSetCurCharacChaosPoint(int point);
    int GetCurCharacChaosExp();
    void GainCurCharacChaosExp(int exp);
    int GetCurCharacChaosModeCount();
    void IncCurCharacChaosModeCount();
    void SetCurCharacChaosModeCount(int count);
    int GetCurCharacChaosKillCount();
    void IncCurCharacChaosKillCount();
    void SetCurCharacChaosKillCount(int count);
    int GetCurCharacChaosDieCount();
    void IncCurCharacChaosDieCount();
    void SetCurCharacChaosDieCount(int count);
    int GetCurCharacChaosDieTime();
    void SetCurCharacChaosDieTime(int t);
    int GetCurCharacChaosStateTime();
    int GetCurCharacChaosResponTime();
    void SetCurCharacChaosResponTime(int t);
    int GetCurCharacChaosKillTime();
    void SetCurCharacChaosKillTime(int t);
    unsigned char GetCurCharacSuperState();
    void SetCurCharacSuperState(bool flag);
    bool IsCurCharacGhost();   // ORIG 调用点 0x086810f2 `test %al;je` 直判（P143 bool 判据）；
                               //   callee 0x080da2da movzbl 直返兼容
    void SetCurCharacGhost(bool flag);
    unsigned char IsCurCharacLeaveTimeout();
    void SetCurCharacLeaveTimeout(bool flag);
    void DieCurCharac(int responTime, CUser* user);
    void LiveCurCharac();
    int GetUserStateByAssault();
    int GetAssaultPlace();
    void SetAssaultPlace(int place);
    int GetAssaultCount();
    void SetAssaultCount(int count);
    void IncCurCharacAssaultCount();
    int GetCurCharacAssaultMoney();
    void SetCurCharacAssaultMoney(int money);
    unsigned short GetCurCharacAssaultedUserId();
    void SetCurCharacAssaultedUserId(unsigned short userId);
    int GetCurCharacAssaultFatigueCount();
    void IncCurCharacAssaultFatigueCount();
    void SetCurCharacAssaultFatigueCount(int count);
    float GetWarAreaKillCount(int index);
    float GetWarAreaKillCountTotal();
    int GetFinishPoint();
    int GetFinishPointTotal();
    int UseFinishPoint(int point);
    void ChangeFinishPoint(int point);
    void InitFinishPoint();
    void DecreaseWarPoint(int point);
    void WarAreaKill(int index, int point, float kill);

    // ---- 教程 / 事件 / 副本 ----
    int getCurCharacEscaladeTutorialFlag(unsigned int flag);
    unsigned char* getCurCharacEscaladeTutorialFlag();
    int getCurCharacTutorialFlag();
    bool setCurCharacTutorialFlag(unsigned int flag);
    void resetCharacTutorialFlag();
    unsigned char getCurCharacEventCharacterGrowtype();
    void setCurCharacEventCharacterGrowtype(char growtype);
    bool IsProperDungeonUser();   // ORIG W 0x08335ca4：读 +0x1238 字节标志（bool 语义）
    void SetProperDungeonUser();
    void ClearProperDungeonUser();
    unsigned short GetProperDungeonClearCount() const;
    void IncProperDungeonClearCount();
    void SetProperDungeonClearCount(short count);
    void ClearProperDungeonClearCount();
    unsigned char getMemberDungeonFlag() const;
    void setMemberDungeonFlag(bool flag);
    int GetCurCharacDungeonPlayCount() const;
    void IncCurCharacDungeonPlayCount();
    void SetCurCharacDungeonPlayCount(int count);
    void ResetCurCharacDungeonPlayCount();
    unsigned short GetCurCharacEventItemCount();
    void ResetCurCharacEventItemCount();
    unsigned short GetCurCharacEventAddItemCount();
    void IncCurCharacEventAddItemCount(unsigned short count);
    void ResetCurCharacEventAddItemCount();
    unsigned short GetCurConditionEventStep() const;
    void IncCurConditionEventStep();
    void SetCurConditionEventStep(short step);
    void ClearCurConditionEventStep();
    unsigned short GetCurConditionEventRewardStep() const;
    void IncCurConditionEventRewardStep();
    void SetCurConditionEventRewardStep(short step);
    void ClearCurConditionEventRewardStep();
    unsigned char IsVillageAttackDBUpdate() const;
    void VillageAttack_DBUpdate();
    void VillageAttack_NotDBUpdate();
    unsigned char GetCurVillageAttackCount() const;
    unsigned char GetCurRevengeDungeonCount() const;
    void SetCurRevengeDungeonCount(unsigned char count);
    void SetCurCharacRevengeData(unsigned char count, unsigned char ticket);
    void ResetRevengeDungeonClear();
    void DecreaseRevengeDungeonTicket();
    int getCurEquipslotFlag();   // ORIG 0x08696c10 出口 movzbl 全字/mov $-1 ⇒ int（广播3，CUserC6 接受）
    void setCurEquipslotFlag(char flag);
    unsigned char getCurChannelEquipslotSwitch();
    void setCurChannelEquipslotSwitch(char flag);
    unsigned char getCurExpandEquipslotSwitch();
    char getUpgradeSeparateTrueFlag();
    void setUpgradeSeparateTrueFlag(char flag);
    char getUpgradeSeparateFlaseFlag();
    void setUpgradeSeparateFalseFlag(char flag);
    unsigned char GetCurExEquipSlotStat();
    void SetCurExpEquipSlotStat(char stat);
    unsigned char needCurCharacCheckOverEquip();
    void resetCurCharacOverEquipFlag();
    int getCurrCharacBlackCount() const;
    void setCurrCharacBlackCount(unsigned int count);
    int getCurCharacTagCharacNo() const;
    void setCurCharacTagCharacNo(unsigned int no);
    unsigned char getCurCharacStrikerSkillIndex() const;
    void setCurCharacStrikerSkillIndex(unsigned char index);
    unsigned char getCurUseBringUpMaterialCount() const;
    void incCurUseBringUpMaterialCount();
    void resetCurUseBringUpMaterialCount();
    unsigned char getCurUseBringUpMaterialTotal() const;
    void setCurUseBringUpMaterialTotal(unsigned char total);
    unsigned char getBringUpRewardFlag() const;
    void setBringUpRewardFlag(bool flag);
    int getCurCharacLastPlayDungeonIndex();
    void setCurCharacLastPlayDungeonIndex(unsigned int index);
    unsigned char is_growth_power_exp_reward_right() const;
    void set_growth_power_exp_reward_right(bool flag);
    unsigned char get_growth_power_reward() const;
    void set_growth_power_reward(int reward);
    long getLastItemLimitEditionBuyTime() const;
    void updateLastItemLimitEditionBuyTime(long t);
    unsigned int getCurCharacHelpAbuseComputedRatio() const;
    void setCurCharacHelpAbuseComputedRatio(unsigned int ratio);
    unsigned int getCurCharacDungeonHelpAbuseRatio() const;
    unsigned int getCurCharacDungeonHelpAbuseExp() const;
    int calc_disp_help_abuse_ratio(int param);
    void calcHelpAbuseRatio(unsigned int param, bool flag);
    void calcHelpAbuseRatioBeforeLevelUp(int param);

    // ---- 可见性 / 外观 / 其它状态 ----
    signed char get_charac_visible_values();
    void set_charac_visible_values(char values);
    bool get_charac_visible_grow_avatar();                                  // ORIG 0x0868bf14 caller call 后 `test %al,%al` ⇒ bool ABI（广播 2026-09-05，DispC6/GDataC6 无异议）
    void set_charac_visible_grow_avatar(bool flag);
    void set_charac_visible_teleport_(bool flag);
    bool IsCurCharacVisible();  // ORIG 0x0868bee4 mov $1/$0,%eax 直返（bool ABI）
    void SetVisible();
    void SetInvisible();
    unsigned char IsFirstTimeLogin();
    void SetFirstTimeLogin(bool flag);
    bool isDisguiseCharac();    // ORIG 0x0822f8f6；调用点 0x0821b4d8 xor $1 判据（bool-only）
    unsigned char getDisguiseKind();
    void setDisguiseKind(unsigned char kind);
    unsigned short getDisguiseIndex();
    void setDisguiseIndex(unsigned short index);
    void setDisguiseCharac(bool flag, unsigned char kind, unsigned short index);
    void set_aura_avatar_option(stAuraAvatarOption& option);
    void set_aura_avatar_option_value(int index, int value);
    long GetLoginTick();
    void SetLoginTick(long tick);
    unsigned char GetOpencoin();
    void SetOpencoin(char value);
    unsigned char IsEndLetheAtLogin();
    void SetIsEndLetheAtLogin();
    void reset_is_end_lethe_at_login();
    unsigned char GetOneDayLetheFlag() const;
    void SetOneDayLetheFlag(unsigned char flag);
    unsigned char GetOneDayLetheFlag2ND() const;
    void SetOneDayLetheFlag2ND(unsigned char flag);
    unsigned char isSavedPremiumCubeInfo() const;
    void disableSavedPremiumCubeInfo();
    bool getContractOfCubePremiumInfo(unsigned char& active, unsigned char& count);
    bool initContractOfCubePremiumInfo(bool active, unsigned char count);
    bool setConctractOfCubePremiumInfo(bool active, unsigned char count);
    unsigned char IsSaveBloodInoutValue();
    void SetSaveBloodInoutValue(bool flag);
    unsigned char getUltimateInoutValue(char index);
    void setUltimateInoutValue(char index, char value);
    void subUltimateInoutValue(char index);
    unsigned char IsSaveDemensionInoutValue();
    void SetSaveDemensionInoutValue(bool flag);
    unsigned char getDemensionInoutValue(char index);
    void setDemensionInoutValue(char index, char value);
    void subDemensionInoutValue(char index);
    unsigned char isBloodBestRecordUpdate();
    void setBloodBestRecordUpdate(bool flag);
    int getBloodBestRoundRecord();
    unsigned int getBloodBestTimeRecord();
    void setBloodBestRecord(unsigned int time, int round, int record, bool flag);
    unsigned short GetCurCharacEmotion();
    void SetCurCharacEmotion(short emotion);
    const char* getSkillCommand() const;
    int GetCurCharacSkillTreeIndex() const;  // ORIG caller 双证：0x81dadb4 无扩展直存(int)，0x81c00fe movsbl 系调用点 (char)cast
    void SetCurCharacSkillTreeIndex(char index);
    const char* GetCurCharacBackupSkill() const;
    const char* GetCurCharacBackupSkill2ND() const;
    char getCharacLinkType();
    int getCharacLinkBonusExp();
    void setCharacLinkBonusExp(unsigned int exp);
    int getCharacLinkBonusGold();
    void setCharacLinkBonusGold(unsigned int gold);
    char getLinkCharacSlotIndex();
    void setLinkCharacSlotIndex(char index);
    unsigned short getStraightVictories() const;
    void incStraightVictories();
    void setStraightVictories(unsigned short count);
    void resetStraightVictories();
    bool IsAvailableCurCharacTradeGoldDaily(unsigned int gold);
    unsigned int getCurCharacTradeGoldDaily();
    unsigned int getCurCharacTradeGoldTotal();
    unsigned int getCurCharacTradeGoldTotalBillion();
    void setCurCharacTradeGoldDaily(unsigned int gold);
    void IncrementCurCharacTradeGoldDaily(unsigned int gold);
    unsigned char getBlueMarbleEnterCount() const;
    void setBlueMarbleEnterCount(unsigned char count);
    void increaseBlueMarbleEnterCount();
    void resetBlueMarbleEnterCount();
    int getBlueMarbleUserDeadCount() const;
    void increaseBlueMarbleUserDeadCount();
    void resetBlueMarbleUserDeadCount();

    // ---- 专家职业 / 在线预备队 ----
    int GetCurCharacExpertJobType() const;
    void SetCurCharacExpertJobType(int type);
    int GetCurCharacExpertJobExp() const;
    void SetCurCharacExpertJobExp(int exp);
    expert_job::CExpertJob* GetCurCharacExpertJob() const;
    void SetCurCharacExpertJob(expert_job::CExpertJob* job);
    bool IsBusyDisjointStore();
    online_preliminary::COnlinePreliminaryTeam* GetCurCharacOnlinePreliminaryTeam();
    void SetCurCharacOnlinePreliminaryTeam(online_preliminary::COnlinePreliminaryTeam* team);
    int GetCurCharacOnlinePreliminaryTeamId() const;
    const char* GetCurCharacOnlinePreliminaryTeamName() const;

    // ---- 选项 / 文本 / 其它 ----
    void SetCurCharacOption(CharacterOption& option);
    void setCharacOption(CharacterOption& option);
    void getSeparatedTextUnits(std::vector<std::string>& out, const char* text,
                               const char* sep, int maxUnits);
    void setSeriaRoomAniDecoInfo(seriaRoom_AniDeco info);
    seriaRoom_AniDeco getSeriaRoomAniDecoInfo();
};

#endif  // GAME_CUSERCHARACINFO_H_
