#ifndef GAME_CDATAMANAGER_H_
#define GAME_CDATAMANAGER_H_

// df_game_r CDataManager（全服数据仓库）还原（2026-08-16）。
// 布局依据 ORIG 构造/析构汇编逐成员推导；ST*Script 属后续脚本批次，
// 此处仅给最小声明（尺寸 + 本类函数需要访问的字段）。

#include <bitset>
#include <ext/hash_map>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>

#include "GameTypes.h"
#include "CEnvironment.h"
#include "CItemList.h"
#include "CItemGeneratorMgr.h"

class CUser;
class CMap;
class CDungeon;
class CItem;
class CMonster;
class CSkill;
class CWorldMap;
class Quest;
class CMTRand;   // 全局 MT19937（PvP_deps.h/CMTRand.cpp 定义）

namespace secretshop
{
struct SALE_INFO;   // CSecretShop.cpp 定义（0x1c）
}

namespace WongWork
{
class CQuestClear;
}

struct stSelectQuestParam;

struct PVPMAP_DATA
{
    int m_field0;    // +0x00
    int m_mapIdx;    // +0x04
};

struct PvPMapInfo
{
    PvPMapInfo();
    int m_mapIdx;    // +0x00
    char m_playable; // +0x04
};

enum ENUM_QUEST_GRADE
{
    ENUM_QUEST_GRADE_NONE = 0,
    ENUM_QUEST_GRADE_1 = 1,
    ENUM_QUEST_GRADE_2 = 2,
    ENUM_QUEST_GRADE_4 = 4,
    ENUM_QUEST_GRADE_5 = 5,
    ENUM_QUEST_GRADE_6 = 6,
    ENUM_QUEST_GRADE_8 = 8
};

class CMapList
{
public:
    CMapList();
    ~CMapList();
    void destroy();
    int insert_map(int channel, int key, CMap* map,
                   std::vector<PVPMAP_DATA>& pvpData);
    CMap* find_map(int idx);
    void LoadPvPSinglePlayableMap(char* path);
    static int GetPvpMapInfo(int idx, PvPMapInfo& info);
    CMapList* GetMapList();
    int GetPVPMapCount() const;

    __gnu_cxx::hash_map<int, CMap*> m_mainMap;         // +0x00
    __gnu_cxx::hash_map<int, CMap*> m_mapList[16];     // +0x14
    __gnu_cxx::hash_map<int, CMap*> m_typeMap1[16];    // +0x154
    __gnu_cxx::hash_map<int, CMap*> m_typeMap2[16];    // +0x294
    int m_pvpMapCount;                                 // +0x3d4

    static std::map<int, PvPMapInfo> mapPvpMapInfo_;   // ORIG 0x943c14c
};

class CMonsterManager
{
public:
    CMonsterManager();
    ~CMonsterManager();
    void destroy();
    int load_monsters();
    int insert_monster(int level, int monsterIdx);
    bool insert_monster(int level, CMonster* monster);
    CMonster* find_monster(int idx);
    int get_levelrandom_mob(int level);

    std::multimap<int, int> m_levelMonsterList;        // +0x00
    __gnu_cxx::hash_map<int, CMonster*> m_monsterMap;  // +0x18
};

class CDungeonList
{
public:
    CDungeonList();
    ~CDungeonList();
    int load_list();
    void destroy();
    bool insert_dungeon(CDungeon* dungeon);
    CDungeon* find_dungeon(int idx) const;
    CDungeon* get_dungeon_at_index(int idx) const;
    CDungeonList* getDungeonIndexList();
    int size() { return m_dungeonMap.size(); }

    __gnu_cxx::hash_map<int, CDungeon*> m_dungeonMap;  // +0x00
};

struct stDBLogItem
{
    unsigned int m_itemIdx;  // +0x00
    int m_time;              // +0x04
    int m_value;             // +0x08
};

// ===================== ST* 脚本占位类型（后续批次实现） =====================

class reseal_table_t
{
public:
    reseal_table_t();
    ~reseal_table_t();
private:
    // ORIG 0x8370742：vector<uint>@+0 + int[6]@+0xc + int[6]@+0x24
    std::vector<unsigned int> m_vec;   // +0x00（0xc 字节）
    int m_arr1[6];                     // +0x0c
    int m_arr2[6];                     // +0x24
};

#ifndef GAME_SERVERPARAMETERSCRIPT_H_
struct sItemGenRef
{
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldc;    // +0x0c
    int m_dropRate;  // +0x10 掉落率倍率（float 位模式，SetItemDropRate 按比例缩放）
};
#endif

// ServerParameterScript 定义于 ServerParameterScript.h（尺寸 0x49f4）。
#include "ServerParameterScript.h"
#include "APSystemTypes.h"

class InGameAdvertisementScript
{
public:
    InGameAdvertisementScript();
private:
    char m_pad[0x98];
};

class PcBangItemRentarData
{
public:
    PcBangItemRentarData();
    ~PcBangItemRentarData();
private:
    char m_pad[0x24];
};

class DimensionActivationData
{
public:
    DimensionActivationData();
    ~DimensionActivationData();
private:
    char m_pad[0x58];
};

class seriaRoomDecoEventScript
{
public:
    seriaRoomDecoEventScript();
    ~seriaRoomDecoEventScript();

    std::vector<int> m_eventItemIdx;    // +0x00
    int m_maxEventItemCount;            // +0x0c
    std::map<int, int> m_matchEventMap; // +0x10
    char m_pad28[0xc];                  // +0x28
    int m_fatigueCondition;             // +0x34
    int m_rewardMailExpireDate;         // +0x38
};

class growthEquipmentScript
{
public:
    growthEquipmentScript();
    ~growthEquipmentScript();

    std::map<int, int> m_rewardMap;     // +0x00
    int m_giftBox;                      // +0x18
    int m_materialItem;                 // +0x1c
    unsigned short m_materialMax;       // +0x20
    unsigned short m_eventBubbleMax;    // +0x22
    unsigned short m_eventSandMax;      // +0x24
    char m_pad26[2];                    // +0x26
};

class createChracScript
{
public:
    createChracScript();
    ~createChracScript();

    char m_pad[0x18];
};

class useCharacterJobFatigue
{
public:
    useCharacterJobFatigue();
    ~useCharacterJobFatigue();

    struct fatigueMatchGift
    {
        char m_pad0[0x1c];
        std::string m_msg1;   // +0x1c
        std::string m_msg2;   // +0x20
    };
    std::map<int, fatigueMatchGift> m_giftMap;  // +0x00
};

class CharacterCreateCreatureGiftScript
{
public:
    CharacterCreateCreatureGiftScript();
    ~CharacterCreateCreatureGiftScript();

    struct eventJob
    {
        int m_minLevel;   // +0x00
        int m_maxLevel;   // +0x04
    };
    std::map<int, eventJob> m_eventJobMap;  // +0x00
    int m_changeAbleCreatureLev;            // +0x18
    std::map<int, int> m_clientChangeMap;   // +0x1c
    std::map<int, int> m_infinityChangeMap; // +0x34
};

struct seriaRoomDecoRewardInfo
{
    int m_itemIdx;   // +0x00
    int m_reward;    // +0x04
};

class seriaRoomDecoAniEventScript
{
public:
    seriaRoomDecoAniEventScript();
    ~seriaRoomDecoAniEventScript();

    std::vector<seriaRoomDecoRewardInfo> m_rewardList;  // +0x00
    std::map<int, int> m_itemMap;                       // +0x0c
    char m_pad24[0x34];                                 // +0x24
};

class stUnlimitChallengeInfo
{
public:
    stUnlimitChallengeInfo();
    ~stUnlimitChallengeInfo();

    char m_pad[0x4c];
    int m_field4c;   // +0x4c
};

class stLimitItemUsageInfoEx
{
public:
    stLimitItemUsageInfoEx();
    ~stLimitItemUsageInfoEx();
private:
    char m_pad[0x24];
};

class CNPCRelationEventManager
{
public:
    CNPCRelationEventManager();
    ~CNPCRelationEventManager();

    char m_pad[0x18];
    int m_itemLimitEditionMgr;   // +0x18
};

// ORIG 布局（0x891b560 构造）：+0x0 int +0x4 int +0x8 string +0xc string
// +0x10 char x3 +0x14 string +0x18 vector<string>（std::string 4B 旧 ABI）
struct stOneEmotionData
{
    stOneEmotionData();
    ~stOneEmotionData();
    void clear();
    int m_0;              // +0x00
    int m_4;              // +0x04
    std::string m_8;      // +0x08
    std::string m_c;      // +0x0c
    char m_10;            // +0x10
    char m_11;            // +0x11
    char m_12;            // +0x12
    char m_pad13;         // +0x13
    std::string m_14;     // +0x14
    std::vector<std::string> m_18;  // +0x18
};

class stEmotionTypeTagsInfo
{
public:
    stEmotionTypeTagsInfo();
    ~stEmotionTypeTagsInfo();
    void clear();
    std::vector<int> m_tags;              // +0x00
    std::vector<stOneEmotionData> m_data; // +0x0c
};

class stEquipmentGradeSample
{
public:
    stEquipmentGradeSample();
    ~stEquipmentGradeSample();
private:
    std::vector<unsigned int> m_grade[4];  // +0x00,+0xc,+0x18,+0x24（ORIG 0x837c810）
    char m_pad30[0xc];                     // +0x30..0x3c
};

class stConditionEventInfo
{
public:
    stConditionEventInfo();
    ~stConditionEventInfo();
private:
    char m_pad[0x50];
};

class ConditionLevelChkDungeon
{
public:
    ConditionLevelChkDungeon();
    ~ConditionLevelChkDungeon();
private:
    char m_pad[0x10];
};

class FairPvPStatScript
{
public:
    FairPvPStatScript();
    ~FairPvPStatScript();
private:
    char m_pad[0x18];
};

class RandomOptionScript
{
public:
    RandomOptionScript();
    ~RandomOptionScript();

    bool init();
    char m_pad[0x230];
};

class RegenerationROI
{
public:
    RegenerationROI();
    ~RegenerationROI();

    char m_pad[0x50];
    int m_expertJobList;   // +0x50
};

class stSeriaBlessingScript
{
public:
    stSeriaBlessingScript();
    ~stSeriaBlessingScript();
private:
    char m_pad[0x18];
};

class stLevelUpRewardItemScript
{
public:
    stLevelUpRewardItemScript();
    ~stLevelUpRewardItemScript();
private:
    char m_pad[0x18];
};

class stGrowthCapsuleScript
{
public:
    stGrowthCapsuleScript();
    char m_pad[0x14];
    int m_field14;   // +0x14
    char m_pad18[0x2c];
};

class stRankSystemInfo
{
public:
    stRankSystemInfo();
    ~stRankSystemInfo();

    char m_pad[0x24];
    unsigned short m_rankLevel[8];   // +0x24
    char m_pad34[0x44];              // +0x34
};

struct stDeathTower_t
{
    stDeathTower_t();
    char m_pad[0xc98];
};

struct stBloodDungeon_t
{
    stBloodDungeon_t();
    ~stBloodDungeon_t();
    char m_pad[0x370];
};

struct stVillageAttacked_t
{
    stVillageAttacked_t();
    ~stVillageAttacked_t();
    char m_pad[0xc];
};

namespace APSystem
{
// _SIG_LOAD_ACTION_POINT（含嵌套 _TodayRewardItem）/CActionPointEx/_MedalRewardItem
// 权威定义见 APSystemTypes.h（MoreTypes.h 同样包含）。
class CActionPointEtcParameter
{
public:
    CActionPointEtcParameter();
    ~CActionPointEtcParameter();
    void ClearData();
    bool IsValidTodayRewardItem(_SIG_LOAD_ACTION_POINT::_TodayRewardItem item);
private:
    // ORIG ClearData 0x088948fc 布局（32 位：4×map(0x18) + 2×vector(0xc) = 0x78）
    std::map<unsigned short, std::vector<CActionPointEx> > m_actionPoints;       // +0x00
    std::map<unsigned short, const CActionPointEx*> m_actionPointRefs;           // +0x18
    std::map<unsigned int, std::string> m_rewardMessages;                        // +0x30
    std::map<unsigned int, std::string> m_rewardMessages2;                       // +0x48
    std::vector<_MedalRewardItem> m_medalRewards;                                // +0x60
    std::vector<_SIG_LOAD_ACTION_POINT::_TodayRewardItem> m_todayRewards;        // +0x6c
};
}

struct stCharacLinkSystem_t
{
    stCharacLinkSystem_t();
    ~stCharacLinkSystem_t();
    char m_pad[0x34];
};

namespace advancealtar
{
class StageTimeLineParameter
{
public:
    StageTimeLineParameter();
    ~StageTimeLineParameter();
    void reset();
    int importStageTimeLineParameter(const char* path);
    void debugLog();
private:
    char m_pad[0x4c];
};

class AdvanceAltarShopParameter
{
public:
    AdvanceAltarShopParameter();
    ~AdvanceAltarShopParameter();
private:
    char m_pad[0x30];
};

class RewardParameter
{
public:
    RewardParameter();
    ~RewardParameter();
private:
    char m_pad[0x18];
};

class ConfigParameter
{
public:
    ConfigParameter();
    ~ConfigParameter();

    // ORIG 0x889b662：int,int, char[0x10], char*@+0x18=&(+0x8), char[0x10]@+0x1c
    int m_0;              // +0x00
    int m_4;              // +0x04
    char m_str[0x10];     // +0x08
    char* m_strPtr;       // +0x18
    char m_str2[0x10];    // +0x1c
    int m_strikerScript;      // +0x2c
    int m_characManageScript; // +0x30
};
}

class stQuestShop
{
public:
    stQuestShop();
    ~stQuestShop();
private:
    char m_pad[0x78];
};

class WorldDropInfo
{
public:
    WorldDropInfo();
    ~WorldDropInfo();
    char m_pad[0x14];
};

class SlangNameData
{
public:
    SlangNameData();
    ~SlangNameData();

    std::vector<unsigned char> m_charList;    // +0x00
    std::vector<unsigned short> m_shortList;  // +0x0c
    char m_pad18[0xc];                        // +0x18
};

class InvalidCharData
{
public:
    InvalidCharData();
    ~InvalidCharData();

    char m_pad[0x30];
    int m_slangFilter;   // +0x30
};

// STStackablePackageData 定义于 STStackableScript.h（本头已 include CItemList.h）。

class STGrowthPowerData
{
public:
    STGrowthPowerData();
    ~STGrowthPowerData();
private:
    char m_pad[0x30];
};

namespace AvatarVariation
{
class AvatarColorInfo
{
public:
    AvatarColorInfo();
    ~AvatarColorInfo();
private:
    char m_pad[0x30];
};
}

#ifndef DNF_BOOSTERGAGEDATA_DEFINED
class BoosterGageData
{
public:
    BoosterGageData();
    ~BoosterGageData();
private:
    char m_pad[0x10];
};
#endif

class CWorldMapList
{
public:
    CWorldMapList();
    ~CWorldMapList();
    CWorldMap* find_world_map(int idx) const;  // ORIG T 0x83647a2（const）
private:
    std::map<int, CWorldMap*> m_map;   // +0x00（ORIG CWorldMapList 即一个 map<int,CWorldMap*>）
};

class StageMapList
{
public:
    StageMapList();
    ~StageMapList();
private:
    char m_pad[0x1c];
};

class WarAreaCounter
{
public:
    WarAreaCounter();
    ~WarAreaCounter();
    void* GetCurrenTimeTable();
    int GetWarRoomCountAtPeekTime(int idx);
    int GetWarRoomCountFirstIndex(int idx);
    int GetWarRoomCountLastIndex(int idx);
private:
    // 布局依据 ORIG 构造/GetCurrenTimeTable/GetWarRoomCountAtPeekTime
    // 直访偏移推导（WarField.cpp 原以 (char*)this+0x.. 裸偏移访问，现改为具名）。
    int m_timeHourList[34];        // +0x60..+0xe7（当日各时段起始小时，34 int）
    int m_curHourIdx;              // +0xe8（当前时段索引）
    char m_pad_ec[0x140 - 0xe8 - 4]; // +0xec..+0x13f（补齐）
    int m_num;                     // +0x140（时段数）
    int m_warRoomCount[10][10];    // +0x144..+0x2d3（[curHourIdx][idx] 目标房间数）
    char m_pad_2d4[0x504 - 0x144 - 100 * 4]; // +0x2d4..+0x503（补齐 sizeof 0x504）
};

class QuestParameterScript
{
public:
    QuestParameterScript();
    ~QuestParameterScript();
private:
    char m_pad[0x6c];
};

class TrainingQuestScript
{
public:
    TrainingQuestScript();
    ~TrainingQuestScript();
    void suffleTrainingQuests();
private:
    char m_pad[0x1490];
};

class GuildParameterScript
{
public:
    GuildParameterScript();
    ~GuildParameterScript();
    // 2026-08-16 补充（CGuildServerProxy::SendGuildExpLimit ORIG 直访，
    // 8B [guild levelup cost] 记录：循环 i=0..16 读 [i][0]；m_guildExpLimit8/
    // m_guildExpLimit16 同址重复读取 record[8][0] / record[16][0]）：
    char m_pad[0x60];                  // +0x00
    int m_levelUpCost[0x11][2];        // +0x60（+0xa0 = record[8][0]，+0xe0 = record[16][0]）
    char m_padE8[0x11c - 0xe8];        // +0xe8..+0x11b（补齐 0x11c）
};

class PowerParameterScript
{
public:
    PowerParameterScript();
    ~PowerParameterScript();

public:
    // 布局依据 ORIG ctor (0x08371a26) / clear (0x0837187a) / importPowerParameterScript
    // (0x08a592bc) / checkPowerWarServer (0x08a59db4, 读 +0x9c) 推导，sizeof 保持 0xd8。
    char m_pad00[0x14];                                     // +0x00..+0x13
    int m_ghostTime;                                        // +0x14（getPowerWarGhostTime 读 CDataManager+0xa63c）
    int m_penaltyThreshold;                                 // +0x18（GetPowerWarResponPenalty 读 +0xa640）
    int m_pad1c;                                            // +0x1c
    char m_pad20[0x14];                                     // +0x20..+0x33（ORIG vector<int> 区）
    int m_pad34;                                            // +0x34
    char m_pad38[0xc];                                      // +0x38..+0x43（ORIG vector<int> 区）
    int m_pad44;                                            // +0x44
    int m_pad48;                                            // +0x48
    int m_pad4c;                                            // +0x4c
    int m_pad50;                                            // +0x50
    int m_pad54;                                            // +0x54
    int m_pad58;                                            // +0x58
    int m_pad5c;                                            // +0x5c（ORIG clear 默认 5）
    int m_pad60;                                            // +0x60（ORIG clear 默认 5）
    int m_minEnterLevel;                                    // +0x64（CheckPowerWarEnterLimit 读 +0xa68c）
    int m_minEnterAge;                                      // +0x68（CheckPowerWarEnterLimit 读 +0xa690）
    int m_pad6c;                                            // +0x6c
    char m_pad70[0x2c];                                     // +0x70..+0x9b（ORIG vector<pair> + ints + pair 区）
    std::vector< std::pair<int, int> > m_powerWarServerList; // +0x9c（checkPowerWarServer 读 +0xa6c4）
    int m_padA8;                                            // +0xa8
    char m_padAC[0xc];                                      // +0xac..+0xb7（ORIG vector<int> 区）
    int m_powerWarCooldownMinutes;                          // +0xb8（ProcessJoinPowerWar 读 +0xa6e0）
    char m_padBC[0xc];                                      // +0xbc..+0xc7（ORIG vector<stVictoryPenalty> 区）
    char m_padC8[0xc];                                      // +0xc8..+0xd3（ORIG vector<int> 区）
    int m_padD4;                                            // +0xd4
};

// ---- SecretShopScript 脚本域结构（全局作用域，匹配 ORIG mangled：stSaleInfo/
//      stDungeonSales/stNpcSales/stDungeonNpc/stLevelSection 均无命名空间） ----
struct stSaleInfo
{
    int m_itemIdx;    // +0x00
    char m_rule;      // +0x04（eBuyRule：0=金币随机价，1=配方固定价）
    char m_pad05[3];  // +0x05
    int m_price;      // +0x08
    int m_materialCount;  // +0x0c
    int m_limit;      // +0x10
    int m_rate;       // +0x14（域权重，GetDomainRate/GetItems 累加）
};

struct stDungeonSales
{
    int m_field0;                 // +0x00
    int m_maxCount;               // +0x04（GetItems 读取，本域单次最大售出数）
    std::map<int, stSaleInfo> m_sales;  // +0x08
};

struct stNpcSales
{
    int m_field0;                       // +0x00
    int m_field4;                       // +0x04
    std::map<int, stDungeonSales> m_dungeonSales;  // +0x08
};

struct stDungeonNpc
{
    int m_field0;   // +0x00
    int m_npcIdx;   // +0x04（GetNpcIdx 返回）
    int m_rate;     // +0x08（GetNpcIdx 累加权重）
    int m_fieldC;   // +0x0c
};

struct stLevelSection
{
    int m_levelIdx;   // +0x00（GetLevelIdx 返回）
    int m_minLev;     // +0x04
    int m_maxLev;     // +0x08
    bool operator<(const stLevelSection& o) const
    {
        return m_minLev < o.m_minLev;
    }
};

class SecretShopScript
{
public:
    SecretShopScript();
    ~SecretShopScript();
    char GetNpcByDungeonIdx(int& out, int a, int b);
    void GetNpcByDungeonLev(int& out, int a, int b);
    char GetItemByDungeonIdx(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out,
                             int dungeonIdx, int level, bool event);
    char GetItemByDungeonLev(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out,
                             int dungeonIdx, int level, bool event);

private:
    // ---- 布局（依据 ORIG SecretShopScript::SecretShopScript() 构造汇编推导） ----
    std::multimap<int, stDungeonNpc> m_npcMap;      // +0x00（[npc] 段）
    char m_pad0c[0x0c];                             // +0x0c
    std::multimap<int, stDungeonNpc> m_levelNpcMap; // +0x18（[level npc] 段，verifyNpc）
    char m_pad24[0x0c];                             // +0x24
    std::set<stLevelSection> m_levelSections;       // +0x30（[level section] 段，GetLevelIdx）
    char m_pad3c[0x0c];                             // +0x3c
    std::map<int, stNpcSales> m_dungeonNpcMap;      // +0x48（[dungeon npc] 段，GetItemByDungeonIdx）
    char m_pad54[0x0c];                             // +0x54
    stDungeonSales m_eventDungeonSales;             // +0x60（GetItemByEvent）
    char m_pad74[0x0c];                             // +0x74
    int m_priceVarPercent;                          // +0x80（GetRandItemPrice 读取 /100）
    int m_int84;                                    // +0x84

    // 内部辅助（CSecretShop.cpp 实现，供 GetItemByDungeon* 调用）
    char GetItems(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out,
                  stDungeonSales& sales);
    char GetItemByEvent(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out);
    void CopyItem(secretshop::SALE_INFO& sale, stSaleInfo& info);
    bool IsExistItem(std::vector<secretshop::SALE_INFO>& out, int itemIdx);
    int GetDomainRate(stDungeonSales& sales);
    int GetLevelIdx(int level);
    int GetRandItemPrice(int price);
    // ORIG 0x8a82998：遍历 equal_range 结果累加 stDungeonNpc::m_rate（+0x8），
    // 首次超过 target 时 *out = m_npcIdx（+0x4）并返回 1；否则 0。
    // 供 GetNpcByDungeonIdx/Lev（0x8a828f8/0x8a8293c）调用。
    char GetNpcIdx(int& out, int target,
                   std::pair<std::multimap<int, stDungeonNpc>::iterator,
                             std::multimap<int, stDungeonNpc>::iterator>& range);
};

class stTitleBookInfo
{
public:
    stTitleBookInfo();
    ~stTitleBookInfo();
private:
    char m_pad[0x58];
};

class AccountCargoScript
{
public:
    AccountCargoScript();
    ~AccountCargoScript();
    void* GetCurrUpgradeInfo(int idx);
    void* GetCreateInfo();
    void* GetNextUpgradeInfo(int idx);
private:
    // ORIG +0x8 为 std::map<int, UpgradeInfo>（C1 0x8372480 构造）；
    // GetCurrUpgradeInfo/GetNextUpgradeInfo/GetCreateInfo（0x88c80ba/0x88c8124/
    // 0x88c8056）在该 map 上 find/begin/++ 返回 value 指针。
    struct UpgradeInfo
    {
        int m_field0;   // +0x00（CAccountCargo.cpp 读作新容量）
        int m_field4;   // +0x04
        int m_field8;   // +0x08（所需道具 idx）
        int m_fieldC;   // +0x0c（所需金币）
    };
    int m_reqLevel;        // +0x00（CAccountCargo.cpp:482 读作 reqLevel）
    int m_itemIdx;         // +0x04
    std::map<int, UpgradeInfo> m_upgradeMap;  // +0x08
    char m_pad14[0x20 - 0x14];                // +0x14..0x20（保持 0x20 总大小）
};

class PvPSkillTreeParameterScript
{
public:
    PvPSkillTreeParameterScript();
    ~PvPSkillTreeParameterScript();
    int getPvPSkillPoint(int job, int firstGrow, int secondGrow, int grade,
                         bool flag) const;
    void getGiveSkill(int job, int firstGrow, int secondGrow,
                      std::vector<std::pair<int, int> >& out) const;
private:
    char m_pad[0x6c];
};

class EventCharacterParameterScript
{
public:
    EventCharacterParameterScript();
    ~EventCharacterParameterScript();
private:
    char m_pad[0x18];
};

class IndependentDropParameterScript
{
public:
    IndependentDropParameterScript();
    ~IndependentDropParameterScript();
private:
    char m_pad[0x3c];
};

class stMercenarySystemInfo
{
public:
    stMercenarySystemInfo();
    ~stMercenarySystemInfo();
private:
    char m_pad[0x4c];
};

struct channel_info_t
{
    // ORIG 布局（getChannelInfo 返回 channel_info_t*；getBonusRate/GetPvPChannelNumber 访问）：
    // +0x00 channelNo；+0x04 channelType；+0x08 名称/类型区（调用方按 +8 访问）；
    // +0x10 std::vector<int>* m_pBonusRates（getBonusRate 解引用，堆上 vector）；
    // +0x14 unsigned char m_bonusRate（getBonusRate 以 /1000.0f 转 float）。
    char m_pad0[0x10];                        // +0x00..+0x10
    std::vector<int>* m_pBonusRates;          // +0x10
    unsigned char m_bonusRate;                // +0x14
    char m_pad1[3];                           // +0x15..+0x18
};

struct channel_script_t
{
    // ORIG 布局（ctor 0x837dc56 / CDataManager ctor 0x8356081 反编译，32 位）：
    // +0x00 std::map<std::string, channel_dungeon_info_t>（0x18，导入脚本用，未建模 →
    //        以 void* 值占位，布局大小一致）；+0x18 std::map<int,std::map<int,channel_info_t>>
    //        （0x18，getChannelInfo/getBonusRate 查表，外层 key=channel，内层 key=channelNo）。
    channel_script_t();
    ~channel_script_t();
    channel_info_t* getChannelInfo(unsigned char channel, unsigned int idx) const;
    float getBonusRate(unsigned char channel, unsigned int a, unsigned int b) const;
private:
    std::map<std::string, void*> m_channelDungeonMap;    // +0x00（opaque 占位）
    std::map<int, std::map<int, channel_info_t> > m_channelMap;  // +0x18
};

struct stVendingMachine_t
{
    char m_pad[0x10];
};


class CMission;  // 前向：m_missionMap/m_kindMissionList 值类型（完整定义见 CMission.h）
class PvP_MissionSystem
{
public:
    // ORIG 布局（ctor 0x85e1abc / dtor 0x85e1c48 反编译，32 位）：
    // +0x00 map<int,CMission*>（0x18）；+0x18 vector<list<CMission*>>（ctor resize 36）；
    // +0x24 vector<uint> m_dailyMissionIndices；+0x30 int；+0x34 list<pair<ui,ui>>；
    // +0x3c int；+0x40 bitset<32>；+0x44 CMTRand*；+0x48 vector<uint>。
    std::bitset<32> get_daily_mission_kind() const;
    CMission* find_mission(int idx);
    void get_New_DailyMission(std::vector<std::pair<unsigned int, unsigned int> >& out);
    int get_WithinMissionIndex(int idx) const;
    std::list<CMission*>& get_kind_mission_list(unsigned int idx);
    int get_BaseMissionExp_byRank(unsigned short rank) const;
    void sort_list();
private:
    std::map<int, CMission*> m_missionMap;                        // +0x00
    std::vector<std::list<CMission*> > m_kindMissionList;         // +0x18
    std::vector<unsigned int> m_dailyMissionIndices;              // +0x24
    int m_field30;                                                // +0x30
    std::list<std::pair<unsigned int, unsigned int> > m_dailyMissionList;  // +0x34
    int m_field3c;                                                // +0x3c
    std::bitset<32> m_dailyMissionKind;                           // +0x40
    void* m_trand;                                                // +0x44（CMTRand*，布局未落地）
    std::vector<unsigned int> m_baseMissionExp;                   // +0x48
};

struct pvp_channel_info_t
{
    pvp_channel_info_t();
    char m_pad8[0x8];       // +0x00 (头部；+0x8 为 max grade 字段)
    int m_maxGrade;         // +0x08（GetMaxGradePvPChannel ORIG 0x822b65a 返回）
    int m_upgradeRevision;  // +0x0c
    char m_pad10[0x14];
    PvP_MissionSystem* m_pvpMissionSystem;  // +0x24

};

class stItemMakingSkill
{
public:
    stItemMakingSkill();
    ~stItemMakingSkill();
private:
    char m_pad[0x30];
};

class ChoiceItemInfo
{
public:
    ChoiceItemInfo();
    bool m_checked;             // +0x00
    char m_pad1[3];             // +0x01
    unsigned int m_choice;      // +0x04
    int m_blueMarbleScriptMgr;  // +0x08
};

class ImageCommunicationData
{
public:
    ImageCommunicationData();
    char m_pad[0x4];
};

namespace ARAD
{
namespace SCRIPT
{
class AradJumping_Script
{
public:
    AradJumping_Script();
    ~AradJumping_Script();
private:
    char m_pad[0xc];
};
}
}

class CLottery_NeedMoney
{
public:
    CLottery_NeedMoney();
    ~CLottery_NeedMoney();
    bool isNeedMoney(unsigned int money) const;
    void add_lottery_needMoney(unsigned int money, unsigned int cost);
private:
    std::map<unsigned int, unsigned int> m_needMoneyMap;  // +0x00（ORIG 0x8365162；sizeof 0xc）
    char m_pad[0xc];   // +0x0c..0x18（保持 0x18 总大小，避免 CDataManager 内后续成员偏移漂移）
};

struct live_server_info_t
{
    live_server_info_t();
    ~live_server_info_t();
    char m_pad[0x18];
    int m_eventScriptMng;   // +0x18
};

class EventAvatarListScript
{
public:
    EventAvatarListScript();
    ~EventAvatarListScript();
private:
    char m_pad[0x18];
};

class EventEtcScript
{
public:
    EventEtcScript();
    ~EventEtcScript();
private:
    char m_pad[0x24];
};

class stReturnUserRewardScript
{
public:
    stReturnUserRewardScript();
    ~stReturnUserRewardScript();
private:
    char m_pad[0x20];
};

class stBingoScript
{
public:
    stBingoScript();
    ~stBingoScript();
private:
    char m_pad[0x54];
};

class stBroadCastItemScript
{
public:
    stBroadCastItemScript();
    ~stBroadCastItemScript();
private:
    char m_pad[0x24];
};

class stNewAccountLevelUpToJobScript
{
public:
    stNewAccountLevelUpToJobScript();
    ~stNewAccountLevelUpToJobScript();
private:
    char m_pad[0x48];
};

class eventReward
{
public:
    eventReward();
    ~eventReward();
private:
    char m_pad[0x30];
};

class DeleteInvalidItemScript
{
public:
    DeleteInvalidItemScript();
    ~DeleteInvalidItemScript();
    bool IsInvalidItem2Delete(int idx);
private:
    // 布局依据 ORIG C1/D1（0x8ac1d40/0x8ac1dce）：string +0x0 / string +0x4 /
    // map<int, DeleteInvalidItemData> +0x8；IsInvalidItem2Delete（0x8ac2096）
    // 在 +0x8 map 上 find(idx)。
    struct DeleteInvalidItemData
    {
        int m_field0;   // +0x00
        int m_field4;   // +0x04
        int m_field8;   // +0x08
    };
    std::string m_field0;                                  // +0x00
    std::string m_field4;                                  // +0x04
    std::map<int, DeleteInvalidItemData> m_deleteMap;      // +0x08
    char m_pad14[0x20 - 0x14];                             // +0x14..0x20
};

// ===================== 外部依赖类（其它 TU 提供） =====================

class CSkillList
{
public:
    CSkillList();
    ~CSkillList();
    void destroy();
    int init();
    void insert_skill(int jobType, CSkill* skill);
    CSkill* find_skill(int skillId, int level);
    int get_ran_groupskill_index(int jobType, int group, int skillIdx);
    void set_post_learning_skill(int jobType, CSkill* skill);
    bool get_post_learning_skill(int jobType, int skillIdx,
                                 std::vector<std::pair<int, int> >& out);
    __gnu_cxx::hash_map<int, CSkill*>* getSkillList(int jobType);

    __gnu_cxx::hash_map<int, CSkill*> m_skillList[11];  // +0x00
    std::multimap<int, CSkill*> m_groupSkillMap[11];    // +0xdc
    std::map<int, std::vector<std::pair<int, int> > >
        m_postLearningMap[11];                          // +0x1e4
};

class CCharacList;
class QuestList
{
public:
    QuestList();
    ~QuestList();
    void destroy();
    int load_list();
    void insert_NPC(int npcIdx);
    int check_ahead_quest(int questIdx, const WongWork::CQuestClear& clear);
    int check_anti_quest(int questIdx, const WongWork::CQuestClear& clear);
    int check_clear(int questIdx, const WongWork::CQuestClear& clear);
    bool select_mail_quest(std::list<int>& out,
                           const WongWork::CQuestClear& clear,
                           const stSelectQuestParam& param);
    int Check_Quest(Quest* quest, stSelectQuestParam& param,
                    const WongWork::CQuestClear& clear);
    void allowable_questlist_as_npc(
        std::list<int>& out, const std::multimap<int, int>& npcList,
        stSelectQuestParam& param, const WongWork::CQuestClear& clear,
        bool flag);
    void select_quest(std::list<int>& out, stSelectQuestParam& param,
                      const WongWork::CQuestClear& clear);
    void GetDailyQuestList(std::list<int>& out);
    void GetTrainingQuestList(std::vector<int>& out, short level);
    Quest* find_quest(int idx);
    std::multimap<int, int>* getQuestNPCList(ENUM_QUEST_GRADE grade);
    QuestList* getQuestList();

    __gnu_cxx::hash_map<int, Quest*> m_questMap;   // +0x00
    std::multimap<int, int> m_npcQuestList[8];     // +0x14
    std::vector<int> m_vecD4;                      // +0xd4
    std::list<int> m_listE0;                       // +0xe0
};
class DisJoint;
class PacketGuard;  // 前向：CItemUpgrade::make3rdChroniclePacket 参数
namespace WongWork
{
class CItemUpgrade
{
public:
    ~CItemUpgrade();
    float getUpgradeItemRepairCostRate(int idx, bool flag) const;
    void make3rdChroniclePacket(CUser* user, const Inven_Item& item,
                                PacketGuard* packet);
};
}
class CItemUpgrade_Separate;

// ---- ChattingEmoticonBase（ORIG vtable 0x8b30748；虚函数槽由 ReCalc 调用点
//      +0x0c/+0x10/+0x14 与派生类 vtable 0x8b30680 验证）----
class ChattingEmoticonBase
{
public:
    virtual ~ChattingEmoticonBase() {}
    virtual bool isUsable(const CUser& user) const = 0;        // vtable+0x08
    virtual int GetStartArrayIndex() const = 0;                // vtable+0x0c
    virtual int GetEndArrayIndex() const = 0;                  // vtable+0x10
    virtual bool isCommonEmoticon(const CUser& user) const;    // vtable+0x14
    virtual void AddPremiumIndex(int index);                   // vtable+0x18

    int GetIndex() const { return m_index; }
    void SetIndex(int index) { m_index = index; }

private:
    int m_index;             // +0x04
    std::set<int> m_set;     // +0x08
};

class CChattingEmoticonConfig;  // CGameOption.h 完整定义
class CChattingEmoticonList
{
public:
    ~CChattingEmoticonList();
    ChattingEmoticonBase* find_emoticon(int idx);
    void ReCalc(CChattingEmoticonConfig& config, const CUser& user);

private:
    std::map<int, ChattingEmoticonBase*> m_map;            // +0x00
    std::map<int, std::vector<ChattingEmoticonBase*> > m_mapVec;  // +0x18
    std::map<int, int> m_mapInt;                           // +0x30
};
class CNPCScript;  // 前向：m_npcMap 值类型
class CNPCScriptList
{
public:
    ~CNPCScriptList();
    int find(unsigned int idx) const;
private:
    __gnu_cxx::hash_map<unsigned int, CNPCScript*> m_npcMap;  // +0x00（ORIG 0x8581680）
};

// ============================================================================
// ItemShopScript（NPC 物品商店脚本，G4 商城/交易域批次，ORIG 布局 0x4c）
// 布局依据 ItemShopScript::C1 (0x85140c8) / clear (0x8514046) /
// importItemShopScript (0x89dc5b4) / CItemShop 各方法位移推导：
//   +0x00 int shopId             +0x04 int limitType
//   +0x08 int limitCount         +0x0c char field_c
//   +0x10 std::string shopName   +0x14 std::vector<int> curItemList
//   +0x20 std::vector<std::string>  +0x2c std::vector<int> limitNpcBuyList
//   +0x38 std::vector<std::vector<int> > dailyList
//   +0x44 char isOneADay         +0x48 int lastUpdateTime
// 注：本工具链 COW std::string 尺寸 4，vector 12，map 12。
// ============================================================================
struct ItemShopScript
{
    void clear();                                   // ORIG 0x8514046
    int m_shopId;                                   // +0x00
    int m_limitType;                                // +0x04
    int m_limitCount;                               // +0x08
    char m_field0c;                                 // +0x0c
    char m_pad0d[3];                                // +0x0d
    std::string m_shopName;                         // +0x10
    std::vector<int> m_curItemList;                 // +0x14
    std::vector<std::string> m_strList;             // +0x20
    std::vector<int> m_limitNpcBuyList;             // +0x2c
    std::vector<std::vector<int> > m_dailyList;     // +0x38
    char m_isOneADay;                               // +0x44
    char m_pad45[3];                                // +0x45
    int m_lastUpdateTime;                           // +0x48
};

// ============================================================================
// CItemShop（NPC 物品商店管理，G4 商城/交易域批次）
// 布局依据 CItemShop::C1 (0x8374e76) / GetOneADayItemList (0x8513076)：
//   +0x00 std::map<int, ItemShopScript> m_shopMap
//   +0x18 std::vector<std::pair<int,int> > m_oneADayList（总尺寸 0x24）
// ============================================================================
class CItemShop
{
public:
    CItemShop();

    // ---- 装载 ----
    bool loadItemShopFiles();

    // ---- 查询 ----
    bool checkValidShopItem(unsigned int itemIdx, unsigned long count);
    bool checkLimitNpcBuyItemList(unsigned int itemIdx, unsigned long count);
    char IsOneADayShop(int shopId) const;
    std::vector<std::pair<int, int> >& GetOneADayItemList() const;

    // ---- 每日商店 ----
    unsigned int setOneADayItemListNo(ItemShopScript& script);
    void updateOneADayItemList();             // ORIG 0x8512e98
    void testupdateOneADayItemList();         // ORIG 0x8512f88

    std::map<int, ItemShopScript> m_shopMap;                        // +0x00
    std::vector<std::pair<int, int> > m_oneADayList;                // +0x18
};
class CAICharacter;
class CAICharacterList
{
public:
    ~CAICharacterList();
    static CAICharacter* get(void* list, unsigned int idx);
    int getAPCount() const { return 0; }
};
class SlangFilter;
namespace WongWork
{
class CItemGeneratorMgr;  // 权威声明见 CItemGeneratorMgr.h（ORIG 位于 namespace WongWork）
}
class CAutoMarketConditionsControl;
class CNPCDynamicInfoManager;
class CItemLimitEditionMgr;
class StrikerScript;
class CharacManageScript;
class BlueMarbleScriptManager;
class CExpertJobList;
class CEventScriptMng;

// ===================== CDataManager =====================

class CDataManager
{
public:
    CDataManager();
    ~CDataManager();

    // ---- 查询（find_*）----
    CItem* find_item(int idx) const;
    CItem* find_item(const char* name) const;
    CItem* FindGoods(int goodsIdx) const;
    void set_AuctionRegFreeCeraItem(unsigned int idx);
    void set_lottery_use_cost(unsigned int cost);
    CSkill* find_skill(int skillId, int level) const;
    Quest* find_quest(int idx) const;
    CDungeon* find_dungeon(int idx) const;
    void* get_dungeon(int idx);       // ORIG W 0x8111036（Village::set_village 引用）
    void* find_map(int idx) const;
    void* find_monster(int idx) const;
    void* find_mission(int idx) const;
    int find_npc(unsigned int idx) const;
    bool find_lottery_use_cost(unsigned int money) const;
    int* find_hellparty_group(int idx);

    // ---- 等级/经验 ----
    int set_level_exp();
    int get_level_exp(int level) const;
    int get_level_section_exp(int level) const;
    int get_level_exp_percent(int level, int percent) const;
    int get_exp_level(int exp) const;
    int get_creature_level_exp(int level) const;
    int get_creature_exp_level(int exp) const;
    unsigned int GetSpAtLevelUp(int level) const;
    unsigned int GetMoneyLimitPerLevel(int level, const char* serverGroup) const;
    void get_mob_reward(int level, unsigned int& out) const;
    int set_mob_reward();
    float BaseExpPenalty(int levelA, int levelB);
    void SetExpRate(float rate);
    void SetItemDropRate(float rate);

    // ---- PvP/排行 ----
    int set_pvp_grade();
    int set_reward_sp();
    unsigned short getRankIndex(int idx);
    unsigned int getNewRankIndex(int idx) const;
    int getRankBonusIndex(int idx) const;
    int calRankPoint(int a, int b);
    unsigned int getDungeonClearAverageTime(int a, int b, int difficulty);
    int getClearTimeBonusPoint(int a, int b);
    int getPvPChannelGrade(int channel);

    // ---- 任务 ----
    std::vector<int>* getDailyTrainingQuest(int idx);
    int isThereDailyTrainingQuestList(int idx, int questIdx);
    int reselectDailyTrainingQuest();
    int get_WithinMissionIndex(int idx) const;
    std::list<CMission*>* get_kind_mission_list(int idx) const;
    void get_New_DailyMission(std::vector<std::pair<unsigned int, unsigned int> >& out) const;
    int get_BaseMissionExp_byRank(unsigned short rank) const;
    std::bitset<32> get_daily_mission_kind() const;

    // ---- 事件/道具 ----
    int getDecoEventItemIdx();
    bool getVaildEventItem(int idx);
    int getMatchEventPoint(int idx);
    int getMaxEventItemCount();
    int getFatigue_contidion();
    int getRewardItem_MailExpireDate();
    int get_GiftGrowthWeaponBox();
    int get_GrowthWeaponMaterialItem();
    unsigned short get_GrowthWeaponMaterialMax();
    unsigned short get_GrowthWeaponEventBubbleMax();
    unsigned short get_GrowthWeaponEventSandMax();
    int get_GrowthWeaponEventReward(int idx) const;
    int getChangeAbleCreatureLev();
    int getClientChangeCreatureIndex(int idx);
    int getInfinityChangeCreatureIndex(int idx);
    int getGiveGrowCreatureEventJobCount(int level);
    bool isAvailableGiveGrowCreatureEventJob(int job, int level);
    int getSeriaRoomAniDecoItem(int idx);
    int getSeriaRoomAniDecoRewardItem(int idx);
    unsigned int maxAniDecoItem();
    bool invalidAniDecoItem(int idx, int value);
    int get_useJobFatigueMatchGift(char job, int a, int b);
    void get_useJobFatigueMatchGiftMsg(int job, std::string& msg1, std::string& msg2);
    void* get_createCharacterGift(int idx, void* out) const;
    void* get_hellparty_script_values();
    void* get_hellparty_groupOrder_map(int idx);

    // ---- 情感/过滤 ----
    void ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user);
    void rewriteUsableEmoticon(const CUser* user, char* text);
    std::vector<int> IsUsableEmoticons(const CUser& user,
                                       const std::vector<int>& emoticons) const;
    bool hasPreventString(const char* name);
    bool hasPreventStringName(const char* name);
    int hasPreventPrefixCharacName(const char* name, int unused);
    bool hasSpecialCharacter(const char* name);
    bool hasSpecialCharacterExceptSpace(const char* name);
    void _subPunctuationMark(const char* src, char* dst);
    bool _isOneByteCharacter(unsigned char c);
    bool getCharacNameFilterPass() const;
    void setCharacNameFilterPass(bool flag);

    // ---- 其它 ----
    void InitLoginState(int serverGroup, int channelNo);
    int loadAdvanceAltarTimeLineParameter(advancealtar::StageTimeLineParameter& param);
    int init();
    void destroy();
    int initRandomOptionItem();
    int initFairPVPSkillPoint();
    int load_map_files(std::vector<void*>& maps);
    int Verify() const;
    int VerifyMap(const CMap& map, int idx) const;
    int VerifyDungeon(const CDungeon& dungeon) const;
    const char* GetScriptPacks() const;
    void SetScriptPacks(const char* path);
    const char* get_server_name(int idx) const;
    void* GetItemMakingSkill() const;
    void* GetImageCommnicationData() const;
    WongWork::CItemGeneratorMgr* getItemGenerator();  // ORIG 0x0814a6ea：返回 m_itemGeneratorMgr (+0x8cc0)
    std::set<int>* getUltimateDungeonList() const;
    void getDungeonIndexList() const;
    int* getBlueMarbleScript() const;
    void setChoiceItem(bool flag, unsigned int idx);
    bool checkChoiceItem() const;
    unsigned int getChoiceItem() const;
    void set_lottery_use_cost(unsigned int money, unsigned int cost);
    void set_original_dimensionInout(int idx, char value);
    void reset_dimensionInout();
    void testActionType();
    void testBingo();
    void* GetExpertJobScript(int job);
    void* GetChannelScript() const;
    void* getBlueMarbleScript();
    void* get_event_script_mng();
    int get_dimensionInout(int idx);
    int get_limit_inout_count(int type);
    void GetPvPChannelGrade();               // ORIG _ZN12CDataManager18GetPvPChannelGradeEv
    int GetMaxGradePvPChannel();
    int GetUpgradeRevisionPvPChannel();
    int IsDBLogItem(unsigned int itemIdx);
    float getUpgradeItemRepairCostRate(int idx, bool flag) const;
    bool checkValidShopItem(unsigned int itemIdx, unsigned long count);
    bool checkLimitNpcBuyItemList(unsigned int itemIdx, unsigned long count);
    const char* GetCeraShopGoodsName();
    const char* SuddenShopFileName1();
    const char* SuddenShopFileName2();
    const char* SuddenShopFileName3();

    // ---- 成员布局（ORIG 0x8355b30 构造顺序）----
    CDungeonList* m_dungeonList;        // +0x00
    CMapList* m_mapList;                // +0x04
    CMonsterManager* m_monsterManager;  // +0x08
    CItemList* m_itemList;              // +0x0c
    CSkillList* m_skillList;            // +0x10
    CCharacList* m_characList;          // +0x14
    QuestList* m_questList;             // +0x18
    DisJoint* m_disJoint;               // +0x1c
    WongWork::CItemUpgrade* m_itemUpgrade;  // +0x20
    CItemUpgrade_Separate* m_itemUpgradeSeparate;  // +0x24
    CChattingEmoticonList* m_emoticonList;          // +0x28

    reseal_table_t m_resealTable;                 // +0x2c
    ServerParameterScript m_serverParameter;      // +0x68
    InGameAdvertisementScript m_inGameAd;         // +0x4a5c
    PcBangItemRentarData m_pcBangRentar;          // +0x4af4
    DimensionActivationData m_dimensionAct;       // +0x4b18
    seriaRoomDecoEventScript m_seriaRoomDeco;     // +0x4b70
    growthEquipmentScript m_growthWeapon;         // +0x4bac
    createChracScript m_createChrac;              // +0x4bd4
    useCharacterJobFatigue m_useJobFatigue;       // +0x4bec
    CharacterCreateCreatureGiftScript m_createCreatureGift;  // +0x4c04
    seriaRoomDecoAniEventScript m_seriaAniDeco;   // +0x4c50
    stUnlimitChallengeInfo m_unlimitChallenge;    // +0x4ca8
    stLimitItemUsageInfoEx m_limitItemUsage;      // +0x4cf8
    std::vector<stDBLogItem> m_dbLogItems; // +0x4d1c
    CNPCRelationEventManager m_npcRelation;       // +0x4d28
    stEmotionTypeTagsInfo m_emotionTags;          // +0x4d44
    stEquipmentGradeSample m_equipGrade;          // +0x4d5c
    stConditionEventInfo m_conditionEvent;        // +0x4d98
    ConditionLevelChkDungeon m_conditionDungeon;  // +0x4de8
    FairPvPStatScript m_fairPvPStat;              // +0x4df8
    RandomOptionScript m_randomOption;            // +0x4e10
    RegenerationROI m_regeneration;               // +0x5040
    stSeriaBlessingScript m_seriaBlessing;        // +0x5094
    stLevelUpRewardItemScript m_levelUpReward;    // +0x50ac
    stGrowthCapsuleScript m_growthCapsule;        // +0x50c4
    stRankSystemInfo m_rankSystem;                // +0x5108
    std::map<unsigned int, stDeathTower_t> m_deathTowerMap;  // +0x5180
    stDeathTower_t m_deathTower;                  // +0x5198
    stBloodDungeon_t m_bloodDungeon;              // +0x5e30
    stVillageAttacked_t m_villageAttacked;        // +0x61a0
    APSystem::CActionPointEtcParameter m_actionPoint1;   // +0x61ac
    APSystem::CActionPointEtcParameter m_actionPoint2;   // +0x6224
    stCharacLinkSystem_t m_characLink;            // +0x629c
    advancealtar::StageTimeLineParameter m_stageTimeLine; // +0x62d0
    advancealtar::AdvanceAltarShopParameter m_altarShop;  // +0x631c
    advancealtar::RewardParameter m_altarReward;          // +0x634c
    advancealtar::ConfigParameter m_altarConfig;          // +0x6364
    stQuestShop m_questShop;                      // +0x6398
    int m_moneyLimit[201];                        // +0x6410
    WorldDropInfo m_worldDrop1[201];              // +0x6734
    WorldDropInfo m_worldDrop2[201];              // +0x76e8
    CAICharacterList* m_aiCharacterList;          // +0x869c
    SlangNameData m_slangName;                    // +0x86a0
    InvalidCharData m_invalidChar;                // +0x86c4
    std::map<int, std::vector<std::vector<STStackablePackageData> > > m_stackableMap;  // +0x86f8
    STGrowthPowerData m_growthPower;              // +0x8710
    AvatarVariation::AvatarColorInfo m_avatarColor;  // +0x8740
    BoosterGageData m_boosterGage;                // +0x8770
    CWorldMapList m_worldMapList;                 // +0x8780
    StageMapList m_stageMapList;                  // +0x8798
    WarAreaCounter m_warAreaCounter;              // +0x87b4
    std::string m_scriptPackPath;                 // +0x8cb8
    CItemShop* m_itemShop;                        // +0x8cbc
    WongWork::CItemGeneratorMgr* m_itemGeneratorMgr;  // +0x8cc0
    CAutoMarketConditionsControl* m_autoMarket;   // +0x8cc4
    QuestParameterScript m_questParameter;        // +0x8cc8
    TrainingQuestScript m_trainingQuest;          // +0x8d34
    std::vector<int> m_dailyTrainingQuest[70];    // +0xa1c4
    GuildParameterScript m_guildParameter;        // +0xa50c
    PowerParameterScript m_powerParameter;        // +0xa628
    SecretShopScript m_secretShop;                // +0xa700
    stTitleBookInfo m_titleBook;                  // +0xa788
    AccountCargoScript m_accountCargo;            // +0xa7e0
    PvPSkillTreeParameterScript m_pvpSkillTree;   // +0xa800
    EventCharacterParameterScript m_eventCharacter;  // +0xa86c
    IndependentDropParameterScript m_independentDrop;  // +0xa884
    std::string m_packName1;                      // +0xa8c0
    std::string m_packName2;                      // +0xa8c4
    std::string m_packName3;                      // +0xa8c8
    std::string m_packName4;                      // +0xa8cc
    CNPCScriptList* m_npcScriptList;              // +0xa8d0
    CNPCDynamicInfoManager* m_npcDynamicInfo;     // +0xa8d4
    bool m_characNameFilterPass;                  // +0xa8d8
    char m_fieldA8d9[3];                          // +0xa8d9
    stMercenarySystemInfo m_mercenarySystem;      // +0xa8dc
    std::map<int, int> m_mapA928;                 // +0xa928
    std::map<int, int> m_mapA940;                 // +0xa940
    char m_fieldA958[0x11c];                      // +0xa958
    union
    {
        struct
        {
            char m_dimensionInout[6];             // +0xaa74
            char m_originalDimensionInout[6];     // +0xaa7a
            char m_padAA80[4];                    // +0xaa80
            int m_expTable[200];                  // +0xaa84
        };
        char m_rawAA74[0x328];
    };                                            // +0xaa74
    int m_mobReward[201];                         // +0xada4
    int m_mobRewardBackup[201];                   // +0xb0c8
    float m_expRate;                              // +0xb3ec
    float m_dropRate;                             // +0xb3f0
    channel_script_t m_channelScript;             // +0xb3f4
    std::vector<stVendingMachine_t> m_vendingMachine;  // +0xb424
    pvp_channel_info_t m_pvpChannel;              // +0xb430
    stItemMakingSkill m_itemMakingSkill;          // +0xb458
    std::set<int> m_ultimateDungeonList;          // +0xb488
    ChoiceItemInfo m_choiceItem;                  // +0xb4a0
    std::set<unsigned int> m_choiceSet;           // +0xb4ac
    ImageCommunicationData m_imageCommData;       // +0xb4c4
    ARAD::SCRIPT::AradJumping_Script m_aradJumping;  // +0xb4c8
    CLottery_NeedMoney m_lotteryNeedMoney;        // +0xb4d4
    live_server_info_t m_liveServerInfo;          // +0xb4ec
    EventAvatarListScript m_eventAvatarList;      // +0xb508
    EventEtcScript m_eventEtc;                    // +0xb520
    std::string m_scriptPacks;                    // +0xb544
    stReturnUserRewardScript m_returnUserReward;  // +0xb548
    stBingoScript m_bingo;                        // +0xb568
    stBroadCastItemScript m_broadCastItem;        // +0xb5bc
    stNewAccountLevelUpToJobScript m_newAccountLevelUp;  // +0xb5e0
    eventReward m_eventReward;                    // +0xb628
    DeleteInvalidItemScript m_deleteInvalidItem;  // +0xb658
};

CDataManager* G_CDataManager();

#endif  // GAME_CDATAMANAGER_H_
