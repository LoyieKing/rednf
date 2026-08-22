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

// ===================== 脚本子结构 POD（ORIG 全局作用域） =====================
// 各元素仅承载 vector/map 模板参数名以匹配 ORIG mangled 符号；字段布局未逐字段
// 确认（仅容器语义），元素体为最小占位（[推断]）。

// sItemGenRef：ServerParameterScript.h 的 m_genRefs 元素类型，须先于其定义。
struct sItemGenRef
{
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldc;    // +0x0c
    int m_dropRate;  // +0x10 掉落率倍率（float 位模式，SetItemDropRate 按比例缩放）
};

// EventRewardSection/EventRewardItemInfo 定义于 ServerParameterScript.h。
#include "ServerParameterScript.h"
#include "advancealtar.h"
// stTitleBookInfo 的 stTitleSection/stTitleElement/stAdjust/stTitleRewardBasis 定义于本头。
#include "CTitleBook.h"

// STExpReward 原定义于 CUser.h；因 STGrowthPowerData（本头）需要完整类型且
// CUserCharacInfo.h→CDataManager.h include 顺序靠前，迁移至本头（pack(1) 保持 ORIG）。
#pragma pack(push, 1)
struct STExpReward
{
    STExpReward() {}

    int m_reward;      // +0x00
    char m_field4;     // +0x04
    char m_field5;     // +0x05
    char m_powerUp;    // +0x06（SetPowerUp）
    int m_localIp;     // +0x07（0x8d254）
    int m_publicIp;    // +0x0b（0x8d258）
    char m_padf[4];    // +0x0f..0x12
    bool m_hangameUser;  // +0x13（0x8d260）
    char m_pad14[0x17 - 0x14];  // +0x14..0x17
};
#pragma pack(pop)

struct STAgitUpgradeInfo          { int m_field0; };                       // [推断]
struct STGuildStorageUpgradeInfo  { int m_field0; };                       // [推断]
struct stCustomConditionMsg       { int m_field0; };                       // [推断]
struct stMercenaryCompetitionArea { int m_field0; };                       // [推断]
struct STBreakAwaySection         { int m_field0; };                       // [推断]
struct stCharacLinkSystemData     { int m_field0; };                       // [推断]
struct pieceQuestRewardStatus     { int m_field0; };                       // [推断]
struct stClearTimeAverage         { int m_field0; };                       // [推断]
struct stBonusPointValues         { int m_field0; };                       // [推断]
struct stDungeonClearTimeData     { int m_field0; };                       // [推断]
struct stComboIntervalBoundary    { int m_field0; };                       // [推断]
struct stUnlimitChallengeRewardInfo    { int m_field0; };                  // [推断]
struct stUnlimitChallengeMessageInfo   { int m_field0; };                  // [推断]
struct stUnlimitChallengeItem     { int m_field0; };                       // [推断]
struct PvPSkillTreeValue          { int m_field0; };                       // [推断]
struct FAIR_PVP_SKILL_TREE_SKILL_PER_POINT { int m_field0; };              // [推断]
struct stMonsterIndependentDrop_t { char m_pad[0x30]; };                    // [推断]
struct IndependentDropListInfo    { int m_field0; };                       // [推断]
struct stIndependentDropInfo
{
    int m_field0;                    // +0x00
    int m_monsterIndex;              // +0x04
    stMonsterIndependentDrop_t m_drop;  // +0x08
};
struct stIndependentDropRateControl { int m_field0; };                     // [推断]
struct itemData                   { int m_field0; };                       // [推断]
struct SlotBound                  { int m_field0; };                       // [推断]
struct stConditionEventRewardInfo { int m_field0; };                       // [推断]
struct stUpgradeWithCompound      { int m_field0; };                       // [推断]
struct STUltimateRewardProb       { int m_field0; };                       // [推断]
struct StackableItemData          { int m_field0; };                       // [推断]
struct LevelUpRewardItemData      { int m_field0; };                       // [推断]
struct stOneBuff_t                { int m_field0; };                       // [推断]
struct str_index_to_itemgroup     { int m_field0; };                       // [推断]
struct _str_cost                  { int m_field0; };                       // [推断]
struct _str_option_value_ratio    { int m_field0; };                       // [推断]
struct stVictoryPenalty           { int m_field0; };                       // [推断]

// map key 类型（需要 operator<；字段布局 [推断]）
struct stClearTimeAverageMapKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const stClearTimeAverageMapKey& o) const
    {
        return m_a < o.m_a || (m_a == o.m_a && m_b < o.m_b);
    }
};

struct stUnlimitChallengeKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const stUnlimitChallengeKey& o) const
    {
        return m_a < o.m_a || (m_a == o.m_a && m_b < o.m_b);
    }
};

struct PvPSkillTreeKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const PvPSkillTreeKey& o) const
    {
        return m_a < o.m_a || (m_a == o.m_a && m_b < o.m_b);
    }
};

struct rewardKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const rewardKey& o) const
    {
        return m_a < o.m_a || (m_a == o.m_a && m_b < o.m_b);
    }
};

struct SlotBoundKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const SlotBoundKey& o) const
    {
        return m_a < o.m_a || (m_a == o.m_a && m_b < o.m_b);
    }
};

struct LevelUpRewardItemTableKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const LevelUpRewardItemTableKey& o) const
    {
        return m_a < o.m_a || (m_a == o.m_a && m_b < o.m_b);
    }
};

// 嵌套 vector 容器子类型（ORIG D1 仅析构各自 vector/map 成员）：
struct stLimitItemUsageInfo
{
    struct LimitInfo { int m_field0; };   // [推断]
    std::vector<LimitInfo> m_list;        // +0x00
};

struct stResetItemInfo
{
    struct ResetItem { int m_field0; };   // [推断]
    std::vector<ResetItem> m_list;        // +0x00
};

struct stRefillItemInfo
{
    struct RefillItem { int m_field0; };  // [推断]
    std::vector<RefillItem> m_list;       // +0x00
};

// stNPCCommonData_t（ORIG C1 0x837612e / D1 0x83761c6 / clear 0x89fae9e）：
// +0x00 ushort[10] +0x18 vector<int> +0x24 vector<int>
// +0x30 ushort +0x32 ushort +0x34 ushort +0x38 vector<int>
// +0x44 vector<pair<int,stOneBuff_t>>，sizeof 0x50
struct stNPCCommonData_t
{
    unsigned short m_field0[10];                       // +0x00
    char m_pad14[4];                                   // +0x14
    std::vector<int> m_vec18;                          // +0x18
    std::vector<int> m_vec24;                          // +0x24
    char m_pad2c[4];                                   // +0x2c
    unsigned short m_field30;                          // +0x30
    unsigned short m_field32;                          // +0x32
    unsigned short m_field34;                          // +0x34
    char m_pad36[2];                                   // +0x36
    std::vector<int> m_vec38;                          // +0x38
    std::vector<std::pair<int, stOneBuff_t> > m_vec44; // +0x44
};

// RandomOptionItem_Regenerator（ORIG D1 0x85f97e2）：
// +0x00 map<int,str_index_to_itemgroup> +0x18 map<int,map<int,_str_cost>>
// +0x30 vector<int> +0x3c vector<_str_option_value_ratio>，sizeof 0x48
struct RandomOptionItem_Regenerator
{
    std::map<int, str_index_to_itemgroup> m_map0;      // +0x00
    std::map<int, std::map<int, _str_cost> > m_map18;  // +0x18
    std::vector<int> m_vec30;                          // +0x30
    std::vector<_str_option_value_ratio> m_vec3c;      // +0x3c
    char m_pad48[4];                                   // +0x48
};

// DimensionActivationRewardData（ORIG C1 0x8371348 / D1 0x83713b4）：
// +0x00 vector<EventRewardSection> +0x0c map<int,vector<vector<EventRewardItemInfo>>>
// +0x24 map<pair<int,int>,vector<vector<EventRewardItemInfo>>>，sizeof 0x3c
struct DimensionActivationRewardData
{
    std::vector<EventRewardSection> m_vec0;                                // +0x00
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > > m_mapC; // +0x0c
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > > m_map24;     // +0x24
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
#endif

// ServerParameterScript 定义于 ServerParameterScript.h（尺寸 0x49f4，已在上方 include）。
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

    // ORIG D1 0x837c2c0：+0x00 vector<EventRewardSection>
    // +0x0c map<int,vector<vector<EventRewardItemInfo>>>，sizeof 0x24
    std::vector<EventRewardSection> m_vec0;                     // +0x00
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > > m_mapC;  // +0x0c
};

struct DimensionCoinInfo
{
    int m_field0;   // [推断]
};

class DimensionActivationData
{
public:
    DimensionActivationData();
    ~DimensionActivationData();

    // ORIG C1 0x8371422 / D1 0x837c306：+0x00 int（=0）+0x04
    // map<int,DimensionCoinInfo> +0x1c DimensionActivationRewardData（0x3c），
    // sizeof 0x58
    int m_field0;                                       // +0x00
    std::map<int, DimensionCoinInfo> m_coinMap;         // +0x04
    DimensionActivationRewardData m_rewardData;         // +0x1c
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

    // ORIG C1 0x837c53c：+0x00 int +0x04 vector<int> +0x10 vector<int>
    // +0x1c vector<stUnlimitChallengeRewardInfo> +0x28 vector<stUnlimitChallengeMessageInfo>
    // +0x34 multimap<stUnlimitChallengeKey,stUnlimitChallengeItem>，sizeof 0x4c
    int m_field0;                                       // +0x00
    std::vector<int> m_vec4;                            // +0x04
    std::vector<int> m_vec10;                           // +0x10
    std::vector<stUnlimitChallengeRewardInfo> m_vec1c;  // +0x1c
    std::vector<stUnlimitChallengeMessageInfo> m_vec28; // +0x28
    std::multimap<stUnlimitChallengeKey,
                  stUnlimitChallengeItem> m_multiMap34; // +0x34
};

class stLimitItemUsageInfoEx
{
public:
    stLimitItemUsageInfoEx();
    ~stLimitItemUsageInfoEx();

    // ORIG D1 0x837c7a2：+0x00 stLimitItemUsageInfo +0x0c stResetItemInfo
    // +0x18 stRefillItemInfo，sizeof 0x24
    stLimitItemUsageInfo m_info0;   // +0x00
    stResetItemInfo m_resetC;       // +0x0c
    stRefillItemInfo m_refill18;    // +0x18
};

// stNPCRelationEventInfo 元素布局未确认（[推断]）；ORIG D1 0x890ce34：
// 遍历 map<unsigned short, stNPCRelationEventInfo*> 删除各节点，再析构 map。
struct stNPCRelationEventInfo
{
    int m_field0;   // [推断]
};

class CNPCRelationEventManager
{
public:
    CNPCRelationEventManager();
    ~CNPCRelationEventManager();

    // ORIG D1 0x890ce34：+0x00 map<unsigned short, stNPCRelationEventInfo*>
    std::map<unsigned short, stNPCRelationEventInfo*> m_npcEventMap;  // +0x00
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

    // ORIG D1 0x837c938：+0x08 map<int,pair<int,int>> +0x20
    // multimap<int,stConditionEventRewardInfo> +0x38
    // multimap<int,stConditionEventRewardInfo>，sizeof 0x50
    int m_field0;                                      // +0x00
    int m_field4;                                      // +0x04
    std::map<int, std::pair<int, int> > m_map8;        // +0x08
    std::multimap<int, stConditionEventRewardInfo> m_multiMap20;  // +0x20
    std::multimap<int, stConditionEventRewardInfo> m_multiMap38;  // +0x38
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

    // ORIG D1 0x85f8506：+0x00 void*（非空 delete）+0x04
    // RandomOptionItem_Regenerator（0x48）+0x50 int，sizeof 0x54
    void* m_regeneratorPtr;                    // +0x00
    RandomOptionItem_Regenerator m_regenerator;  // +0x04
    int m_expertJobList;                       // +0x50
};

class stSeriaBlessingScript
{
public:
    stSeriaBlessingScript();
    ~stSeriaBlessingScript();
private:
    char m_pad[0x18];
};

// stLevelUpRewardItemScript / stStepRewardStackableItemScript（ORIG D1
// 0x8371530 / 0x837151c：各含一个 map 成员，sizeof 0x18）。
class stLevelUpRewardItemScript
{
public:
    stLevelUpRewardItemScript();
    ~stLevelUpRewardItemScript();

    std::map<LevelUpRewardItemTableKey, LevelUpRewardItemData> m_map0;  // +0x00
    char m_pad18[0xc];                                                 // +0x18
};

class stStepRewardStackableItemScript
{
public:
    stStepRewardStackableItemScript();
    ~stStepRewardStackableItemScript();

    std::map<int, std::vector<StackableItemData> > m_map0;  // +0x00
    char m_pad18[0xc];                                      // +0x18
};

class stGrowthCapsuleScript
{
public:
    stGrowthCapsuleScript();
    char m_pad[0x14];
    int m_field14;   // +0x14
    char m_pad18[0x2c];
};

struct stDungeonRankSystem_t
{
    stDungeonRankSystem_t();
    ~stDungeonRankSystem_t();
    void clear();

    // ORIG C1 0x8370dcc / clear 0x8370e34：+0x00 map<stClearTimeAverageMapKey,
    // stClearTimeAverage> +0x18 vector<stBonusPointValues>
    // +0x24 ushort[8] m_rankLevel（getRankIndex 读）+ memset 0x12 字节，sizeof 0x38
    std::map<stClearTimeAverageMapKey, stClearTimeAverage> m_map0;  // +0x00
    std::vector<stBonusPointValues> m_vec18;                        // +0x18
    unsigned short m_rankLevel[8];                                  // +0x24
    char m_pad34[2];                                                // +0x34
    char m_pad36[2];                                                // +0x36
};

class stRankSystemInfo
{
public:
    stRankSystemInfo();
    ~stRankSystemInfo();
    void clear();

    // ORIG C1 0x8370eba / D1 0x837ce4c：+0x00 stDungeonRankSystem_t（0x38）
    // +0x38 vector<stDungeonClearTimeData> +0x44 map<int,int>
    // +0x5c vector<int> +0x68 vector<stComboIntervalBoundary>，sizeof 0x78
    stDungeonRankSystem_t m_dungeonRank;                           // +0x00
    std::vector<stDungeonClearTimeData> m_vec38;                   // +0x38
    std::map<int, int> m_map44;                                    // +0x44
    std::vector<int> m_vec5c;                                      // +0x5c
    std::vector<stComboIntervalBoundary> m_vec68;                  // +0x68
    int m_field74;                                                 // +0x74
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
    void clear();

    // ORIG C1 0x8370a3a / clear 0x8370b02：+0x00 float[200]（clear 置 1.0f）
    // +0x320 int（=1）+0x324 int[3] +0x330 float（=1.0f）
    // +0x334 vector<float> +0x340 char[0x18] +0x358 map<int,STUltimateRewardProb>
    // sizeof 0x370
    float m_rate[200];                                    // +0x00
    int m_field320;                                       // +0x320
    int m_field324[3];                                    // +0x324
    float m_field330;                                     // +0x330
    std::vector<float> m_vec334;                          // +0x334
    char m_pad340[0x18];                                  // +0x340
    std::map<int, STUltimateRewardProb> m_map358;         // +0x358
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

    // ORIG C1 0x837cf46：+0x00 int +0x04/+0x10/+0x1c/+0x28
    // vector<stCharacLinkSystemData>，sizeof 0x34
    int m_field0;                                          // +0x00
    std::vector<stCharacLinkSystemData> m_vec4;            // +0x04
    std::vector<stCharacLinkSystemData> m_vec10;           // +0x10
    std::vector<stCharacLinkSystemData> m_vec1c;           // +0x1c
    std::vector<stCharacLinkSystemData> m_vec28;           // +0x28
};

namespace advancealtar
{

// _SummonObject/_StarRestFeeByGold/BuyShopData 元素布局确认（推断自 ORIG）。
struct _SummonObject { int m_field0; };        // [推断]
struct ActionDefine { int m_field0; };         // [推断]
struct _StarRestFeeByGold { int m_field0; };   // [推断]
// ORIG BuyUpgradeData（getItemValueStirng @ 0x8134132）：+0x00 itemId int + 8B + string。
struct BuyUpgradeData
{
    int m_nItemId;          // +0x00
    char m_pad04[8];        // +0x04..+0x0b（getFieldData map 区，未建模）
    std::string m_strDesc;  // +0x0c
};
// ORIG BuyShopData（reset @ 0x88a1090）：+0x00 int；三 vector<BuyUpgradeData> 于 +0x04/+0x10/+0x1c。
struct BuyShopData
{
    int m_nField0;                       // +0x00
    std::vector<BuyUpgradeData> m_vecT0; // +0x04（T_0）
    std::vector<BuyUpgradeData> m_vecT1; // +0x10（T_1）
    std::vector<BuyUpgradeData> m_vecT2; // +0x1c（T_2）
};

class StageTimeLineParameter
{
public:
    StageTimeLineParameter();
    ~StageTimeLineParameter();
    void reset();
    int importStageTimeLineParameter(const char* path);
    void debugLog();

    // ORIG D1 0x8899e88：+0x00 map<int,_SummonObject> +0x18
    // map<int,ActionDefine> +0x30 TimeLine（0x1c），sizeof 0x4c
    std::map<int, _SummonObject> m_summonMap;    // +0x00
    std::map<int, ActionDefine> m_actionMap;     // +0x18
    TimeLine m_timeLine;                         // +0x30
    char m_pad4c[0x4c - 0x30 - 0x1c];            // +0x4c..0x4b（补齐 0x4c）
};

class AdvanceAltarShopParameter
{
public:
    AdvanceAltarShopParameter();
    ~AdvanceAltarShopParameter();

    // ORIG D1 0x88a0fa2：+0x00 map<int,BuyShopData> +0x18
    // map<int,_StarRestFeeByGold>，sizeof 0x30
    std::map<int, BuyShopData> m_buyShopMap;     // +0x00
    std::map<int, _StarRestFeeByGold> m_starRestMap;  // +0x18
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

    // ORIG C1 0x837d168 / D1 0x837d1fa：+0x00 vector<unsigned char>
    // +0x0c vector<stRangeElement<unsigned char>> +0x18 vector<unsigned short>
    // +0x24 vector<stRangeElement<unsigned short>> +0x30 int，sizeof 0x34
    template <typename T>
    struct stRangeElement
    {
        T m_min;   // [推断]
        T m_max;   // [推断]
    };
    std::vector<unsigned char> m_charList;                // +0x00
    std::vector<stRangeElement<unsigned char> > m_charRange;  // +0x0c
    std::vector<unsigned short> m_shortList;              // +0x18
    std::vector<stRangeElement<unsigned short> > m_shortRange;  // +0x24
    int m_slangFilter;                                    // +0x30
};

// STStackablePackageData 定义于 STStackableScript.h（本头已 include CItemList.h）。

class STGrowthPowerData
{
public:
    STGrowthPowerData();
    ~STGrowthPowerData();

    // ORIG C1 0x83711fe / D1 0x837d290：+0x00 vector<STBreakAwaySection>
    // +0x0c vector<STExpReward>
    // +0x18 map<int, vector<vector<STStackablePackageData>>>，sizeof 0x30
    std::vector<STBreakAwaySection> m_vec0;              // +0x00
    std::vector<STExpReward> m_vecC;                     // +0x0c
    std::map<int, std::vector<std::vector<STStackablePackageData> > > m_map18;  // +0x18
};

namespace AvatarVariation
{

struct colorRGB
{
    int m_field0;   // [推断]
};

class AvatarColorInfo
{
public:
    AvatarColorInfo();
    ~AvatarColorInfo();

    // ORIG D1 0x837d358：+0x00 map<int,colorRGB> +0x18 map<int,string>，sizeof 0x30
    std::map<int, colorRGB> m_colorMap;      // +0x00
    std::map<int, std::string> m_nameMap;    // +0x18
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
    void clear();

    // ORIG C1 0x836f1c2 / clear 0x836f130 / D1 0x837d3ca：+0x00 map<char,int>
    // +0x18 int[3] +0x24 vector<int> +0x30 vector<int> +0x3c map<int,int>
    // +0x54 map<int,vector<pieceQuestRewardStatus>>，sizeof 0x6c
    std::map<char, int> m_map0;                              // +0x00
    int m_field18;                                           // +0x18
    int m_field1c;                                           // +0x1c
    int m_field20;                                           // +0x20
    std::vector<int> m_vec24;                                // +0x24
    std::vector<int> m_vec30;                                // +0x30
    std::map<int, int> m_map3c;                              // +0x3c
    std::map<int, std::vector<pieceQuestRewardStatus> > m_map54;  // +0x54
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
    void clear();

    // ORIG C1 0x837058a / clear 0x83704ae / D1 0x837d542，sizeof 0x11c：
    // +0x00 int +0x04 int +0x08 char +0x0c int +0x10 map<int,int>
    // +0x28 int +0x2c vector<STAgitUpgradeInfo> +0x38
    // vector<STGuildStorageUpgradeInfo> +0x44 map<int,int>
    // +0x5c int[0x11][2]（getGuildLevelUpParam 以 +0x5c+(n-1)*8 访问）
    // +0xe4 vector<pair<int,double>> +0xf0 vector<pair<int,vector<int>>>
    // +0xfc double +0x104 int +0x108 int +0x10c int +0x110 double
    int m_field0;                                   // +0x00
    int m_field4;                                   // +0x04
    char m_field8;                                  // +0x08
    int m_fieldC;                                   // +0x0c
    std::map<int, int> m_map10;                     // +0x10
    int m_field28;                                  // +0x28
    std::vector<STAgitUpgradeInfo> m_vec2c;         // +0x2c
    std::vector<STGuildStorageUpgradeInfo> m_vec38; // +0x38
    std::map<int, int> m_map44;                     // +0x44
    int m_levelUpCost[0x11][2];                     // +0x5c（16 项有效，[17] 含）
    std::vector<std::pair<int, double> > m_vecE4;   // +0xe4
    std::vector<std::pair<int, std::vector<int> > > m_vecF0;  // +0xf0
    double m_fieldFC;                               // +0xfc
    int m_field104;                                 // +0x104
    int m_field108;                                 // +0x108
    int m_field10c;                                 // +0x10c
    double m_field110;                              // +0x110
    char m_pad118[4];                               // +0x118
};

class PowerParameterScript
{
public:
    PowerParameterScript();
    ~PowerParameterScript();

public:
    // 布局依据 ORIG ctor (0x08371a26) / clear (0x0837187a) / D1 (0x0837d634) /
    // importPowerParameterScript (0x08a592bc) / checkPowerWarServer (0x08a59db4,
    // 读 +0x9c) 推导，sizeof 保持 0xd8。D1 析构 7 个容器成员：
    // +0xc8 vector<int> +0xbc vector<stVictoryPenalty> +0xac vector<int>
    // +0x9c vector<pair<int,int>> +0x70 vector<pair<int,int>>
    // +0x38 vector<int> +0x20 vector<int>
    char m_pad00[0x14];                                     // +0x00..+0x13
    int m_ghostTime;                                        // +0x14（getPowerWarGhostTime 读 CDataManager+0xa63c）
    int m_penaltyThreshold;                                 // +0x18（GetPowerWarResponPenalty 读 +0xa640）
    int m_pad1c;                                            // +0x1c
    std::vector<int> m_vec20;                               // +0x20
    char m_pad2c[8];                                        // +0x2c..+0x33
    int m_pad34;                                            // +0x34
    std::vector<int> m_vec38;                               // +0x38（ORIG vector<int> 区）
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
    std::vector< std::pair<int, int> > m_vec70;             // +0x70
    char m_pad7c[0x20];                                     // +0x7c..+0x9b（8 int）
    std::vector< std::pair<int, int> > m_powerWarServerList; // +0x9c（checkPowerWarServer 读 +0xa6c4）
    int m_padA8;                                            // +0xa8
    std::vector<int> m_vecAC;                               // +0xac
    int m_powerWarCooldownMinutes;                          // +0xb8（ProcessJoinPowerWar 读 +0xa6e0）
    std::vector<stVictoryPenalty> m_vecBC;                  // +0xbc
    std::vector<int> m_vecC8;                               // +0xc8
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

    // ORIG C1 0x837d828 / D1 0x837d906：+0x00 vector<int>
    // +0x0c vector<stTitleSection> +0x18 map<int,vector<stTitleElement>>
    // +0x30 vector<stCustomConditionMsg> +0x3c vector<stAdjust>
    // +0x48 vector<stTitleRewardBasis*>，sizeof 0x58
    std::vector<int> m_vec0;                                  // +0x00
    std::vector<stTitleSection> m_vecC;                       // +0x0c
    std::map<int, std::vector<stTitleElement> > m_map18;      // +0x18
    std::vector<stCustomConditionMsg> m_vec30;                // +0x30
    std::vector<stAdjust> m_vec3c;                            // +0x3c
    std::vector<stTitleRewardBasis*> m_vec48;                 // +0x48
    char m_pad54[4];                                          // +0x54
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
    void clear();

    // ORIG C1 0x8371eee / D1 0x837da02：+0x00 map<PvPSkillTreeKey,
    // map<int,PvPSkillTreeValue>> +0x18 map<int,map<int,int>> +0x30
    // map<PvPSkillTreeKey,FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>
    // +0x54 map<int,int>，sizeof 0x6c
    std::map<PvPSkillTreeKey, std::map<int, PvPSkillTreeValue> > m_map0;  // +0x00
    std::map<int, std::map<int, int> > m_map18;                           // +0x18
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT> m_map30;                // +0x30
    int m_field48;                                                        // +0x48
    int m_field4c;                                                        // +0x4c
    char m_field50;                                                       // +0x50
    char m_pad51[3];                                                      // +0x51
    std::map<int, int> m_map54;                                           // +0x54
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
    void clear();

    // ORIG C1 0x8372d46 / D1 0x837daac：+0x00 vector<IndependentDropListInfo>
    // +0x0c vector<stIndependentDropInfo> +0x18 vector<stIndependentDropRateControl>
    // +0x24 map<int,pair<int,int>>，sizeof 0x3c
    std::vector<IndependentDropListInfo> m_vec0;           // +0x00
    std::vector<stIndependentDropInfo> m_vecC;             // +0x0c
    std::vector<stIndependentDropRateControl> m_vec18;     // +0x18
    std::map<int, std::pair<int, int> > m_map24;           // +0x24
};

class stMercenarySystemInfo
{
public:
    stMercenarySystemInfo();
    ~stMercenarySystemInfo();

    // ORIG C1 0x8373328：+0x00 int（=0xe10）+0x04 ushort（=0xc8）
    // +0x08 vector<pair<int,pair<int,int>>> +0x14 vector<pair<int,float>>
    // +0x20 vector<pair<int,float>> +0x2c string +0x30 vector<pair<int,float>>
    // +0x3c int（=0）+0x40 vector<stMercenaryCompetitionArea>，sizeof 0x4c
    int m_field0;                                          // +0x00（0xe10）
    unsigned short m_field4;                               // +0x04（0xc8）
    char m_pad6[2];                                        // +0x06
    std::vector<std::pair<int, std::pair<int, int> > > m_vec8;   // +0x08
    std::vector<std::pair<int, float> > m_vec14;           // +0x14
    std::vector<std::pair<int, float> > m_vec20;           // +0x20
    std::string m_str2c;                                   // +0x2c
    std::vector<std::pair<int, float> > m_vec30;           // +0x30
    int m_field3c;                                         // +0x3c（0）
    std::vector<stMercenaryCompetitionArea> m_vec40;       // +0x40
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

    // ORIG D1 0x837dce2：+0x00 map<int,stUpgradeWithCompound>
    // +0x18 vector<pair<int,int>>，sizeof 0x30
    std::map<int, stUpgradeWithCompound> m_map0;    // +0x00
    std::vector<std::pair<int, int> > m_vec18;      // +0x18
    char m_pad24[0xc];                              // +0x24
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
    void clear();

    // ORIG C1 0x8372e66 / clear 0x8372ece / D1 0x837dda0：+0x00 vector<int>
    // +0x0c map<uint,uint>，sizeof 0x24
    std::vector<int> m_vec0;            // +0x00
    std::map<unsigned int, unsigned int> m_mapC;  // +0x0c
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

    // ORIG C1 0x83715b6 / D1 0x837ddfc：+0x00 map<int,int> +0x18
    // map<int,vector<int>> +0x30 map<int,pair<int,int>> +0x48 vector<int>
    // sizeof 0x54
    std::map<int, int> m_map0;                     // +0x00
    std::map<int, std::vector<int> > m_map18;      // +0x18
    std::map<int, std::pair<int, int> > m_map30;   // +0x30
    std::vector<int> m_vec48;                      // +0x48
};

class stBroadCastItemScript
{
public:
    stBroadCastItemScript();
    ~stBroadCastItemScript();

    // ORIG D1 0x837de92：+0x00 vector<int> +0x0c map<int,vector<int>>，sizeof 0x24
    std::vector<int> m_vec0;                     // +0x00
    std::map<int, std::vector<int> > m_mapC;     // +0x0c
};

class stNewAccountLevelUpToJobScript
{
public:
    stNewAccountLevelUpToJobScript();
    ~stNewAccountLevelUpToJobScript();

    // ORIG D1 0x837df44：+0x00 stStepRewardStackableItemScript（0x18）
    // +0x18/+0x30 stLevelUpRewardItemScript（0x18），sizeof 0x48
    stStepRewardStackableItemScript m_stepReward;  // +0x00
    stLevelUpRewardItemScript m_levelUp18;         // +0x18
    stLevelUpRewardItemScript m_levelUp30;         // +0x30
};

class eventReward
{
public:
    eventReward();
    ~eventReward();

    // ORIG D1 0x837dfb2：+0x00 map<rewardKey,map<int,vector<itemData>>>
    // +0x18 CSlotBoundChecker（ORIG 为 map<SlotBoundKey,SlotBound>；重建
    // CSlotBoundChecker 采用静态数组实现（CInventory.h/CSlotBoundChecker.cpp），
    // 无 per-instance 成员，+0x18 以 pad 占位保持 sizeof 0x30）[推断]
    std::map<rewardKey, std::map<int, std::vector<itemData> > > m_map0;  // +0x00
    char m_pad18[0x18];                                                  // +0x18
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
    void _destroy();
    int find(unsigned int idx) const;

    // ORIG D1 0x83762a8 / _destroy 0x8581784：+0x00
    // hash_map<uint,CNPCScript*> +0x14 stNPCCommonData_t（0x50），sizeof 0x64
    __gnu_cxx::hash_map<unsigned int, CNPCScript*> m_npcMap;  // +0x00（ORIG 0x8581680）
    stNPCCommonData_t m_npcCommon;                            // +0x14
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
    void* GetExpertJobEtcScript();   // ORIG CDataManager::GetExpertJobEtcScript（CUser 使用）
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
