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

#include <map>
#include <string>
#include <vector>

#include "GameTypes.h"       // cMyTrace / LogManager
#include "CSystemTime.h"
#include "CItem.h"          // Inven_Item / CItem
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "PacketBuf.h"      // GameWorld / G_GameWorld / G_CDataManager

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

enum eWPSubReason
{
    eWPSubReason_0 = 0
};

enum eFPAddReason
{
    eFPAddReason_0 = 0
};

enum eFPSubReason
{
    eFPSubReason_0 = 0
};

// ---- 前向声明（真实定义属其它 TU / 后续批次） ----
struct _Charac_info;
class CUser;
class CInventory;
class CExpandEquipslot;
struct SIG_LOAD_ETC;

// ---- 依赖最小声明（签名按 ORIG nm 核对；定义由对应 TU / GameStubs 提供） ----
class PvpResultType
{
public:
    PvpResultType();
    ~PvpResultType();
    void Clear();

    char m_pad0[8];             // +0x00
    int m_expPoint;             // +0x08（get_pvp_result_exp_point）
    int m_oldExpPoint;          // +0x0c（get_pvp_result_oldexp_point）
    char m_pad10[4];            // +0x10
    int m_pvpGrade;             // +0x14（get_pvp_grade）
    char m_pad18[0x40];         // +0x18
    int m_winPoint;             // +0x58（GetWinPoint / useWinPoint）
    char m_pad5c[0x84 - 0x5c];  // +0x5c..0x84
};

class GameResultType
{
};

namespace WongWork
{
class CDungeonClear
{
public:
    CDungeonClear();
    ~CDungeonClear();
    void clear();

    char m_pad[0x18];
};
}

class CommonTime
{
public:
    void SetCreateTime(long t);
    operator long() const;

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

    char m_pad[0xc];    // +0x00
    int m_schoolPoint;  // +0x0c（calcCurCharacSchoolPoint 读写 +0x71）
};

namespace expert_job
{
class CExpertJob
{
public:
    int GetType();
};
}

namespace online_preliminary
{
class COnlinePreliminaryTeam
{
public:
    int GetTeamId() const;
    const char* GetTeamName() const;
};
}

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
};

class CharacterOption
{
public:
    char m_pad[0x200];
};

class ContinuousItemInfo
{
public:
    int m_itemIdx;             // +0x00
    unsigned int m_expireTime; // +0x04
};

struct WpBonusPointResult
{
    int m_base;   // +0x00
    int m_add;    // +0x04
};

struct ExpBonusPointResult
{
    int m_base;   // +0x00
    int m_add;    // +0x04
};

struct stBloodBestRecord
{
    int m_round;         // +0x00
    unsigned int m_time; // +0x04
};

class CPowerManager
{
public:
    bool IsPowerWarEventOn();
    float GetPowerWarResponPenalty(CUser* user);
    int GetPowerWarGhostTime(CUser* user, float f);
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
    int delete_item(int idx, int count, eItemDelReason reason);
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

class CTitleBook
{
public:
    bool findItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const;
};

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
    char m_reliablePerson;         // +0xcc
    char m_padcd[3];               // +0xcd..0xcf
    int m_overTradeGoldCount;      // +0xd0
    char m_saveCharacOption;       // +0xd4
    char m_padd5[3];               // +0xd5..0xd7
    long m_characStartPlayTick;    // +0xd8
    char m_userEventCharacterFlag; // +0xdc
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

    // ---- 背包 / 技能 / 仓库 / 附加信息 ----
    Inven_Item* getCurCharacInvenW();
    const Inven_Item* getCurCharacInvenR() const;
    Inven_Item* getCurCharacInvenRefW();
    const Inven_Item* getCurCharacInvenRefR() const;
    char* getCurCharacSkillW();
    const char* getCurCharacSkillR() const;
    CCargo* getCurCharacCargoW();
    const CCargo* getCurCharacCargoR() const;
    Inven_Item* getTagCharacInvenW();
    const Inven_Item* getTagCharacInvenR() const;
    char* getTagCharacSkillW();
    const char* getTagCharacSkillR() const;
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
    unsigned char getCurCharacVill() const;
    unsigned char getCurCharacVill4Save() const;
    unsigned char getCurCharacVillPrev() const;
    void setCurCharacVill(char vill);
    unsigned char isJoinPowerWar() const;
    void setCurrCharacJoinPower(char power);
    unsigned short GetPowerWarHP();
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
    unsigned char IsCurCharacGhost();
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
    unsigned char IsProperDungeonUser();
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
    unsigned char getCurEquipslotFlag();
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
    unsigned char get_charac_visible_values();
    void set_charac_visible_values(char values);
    int get_charac_visible_grow_avatar();
    void set_charac_visible_grow_avatar(bool flag);
    void set_charac_visible_teleport_(bool flag);
    int IsCurCharacVisible();
    void SetVisible();
    void SetInvisible();
    unsigned char IsFirstTimeLogin();
    void SetFirstTimeLogin(bool flag);
    unsigned char isDisguiseCharac();
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
    int GetCurCharacSkillTreeIndex() const;
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
