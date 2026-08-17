// df_game_r CDungeon / CDungeon_TowerOfDespair / STDungeonScript 还原（G2 副本链）。
// 布局依据（ORIG df_game_r 反汇编）：
//   CDungeon ctor 0x08373f3c / dtor 0x08376fd8（sizeof 0x934，vtable + 16*4 容器族）
//   STDungeonScript ctor 0x0836b5e2 / dtor 0x0836bd5c / Clear 0x0836abc6（sizeof 0x50a）
//   MazeScript ctor 0x088f2b4a / copy 0x08376d8a / dtor 0x08376f00（sizeof 0x74）
//   stDungeonAssignItem_t ctor 0x088f2ca8（int + list<pair<int,int>>，0x1c 字节）
//   STWarRoomData ctor 0x0836a1f6 / clear 0x0836a398 / copy 0x0836a578（0x100 字节）
#pragma once

#include <ext/hash_map>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"

class CUser;
class CMap;

#ifndef DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
#define DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
enum ENUM_MAZE_QUEST_TYPE
{
    ENUM_MAZE_QUEST_TYPE_0 = 0
};
#endif

// ---------------------------------------------------------------------------
// 迷宫/网格支撑类型（GridScript/RidableObject/RidableScript/DungeonClearCondition）
// ---------------------------------------------------------------------------

struct GridScript
{
    int m_gridValue;                        // +0x00
    std::vector<int> m_mapCandidates;       // +0x04
    std::vector<int> m_secondCandidates;    // +0x10
    std::vector<int> m_layeredMapIndexes;   // +0x1c

    GridScript();
    GridScript(const GridScript& other);
    ~GridScript();
    GridScript& operator=(const GridScript& other);
    void clear();
};

struct RidableObject
{
    char m_pad[4];

    RidableObject();
    ~RidableObject();
    void clear();
};

class RidableScript
{
public:
    RidableScript();
    RidableScript(const RidableScript& other);
    ~RidableScript();
    RidableScript& operator=(const RidableScript& other);
    void clear();

    int m_0;                                // +0x00
    char m_4;                               // +0x04
    std::vector<RidableObject> m_objects;   // +0x08
};

struct DungeonClearCondition
{
    int m_0;   // +0x00
    int m_4;   // +0x04
    int m_8;   // +0x08

    DungeonClearCondition();
    ~DungeonClearCondition();
    void clear();
};

// MazeScript（0x74 字节，由 ORIG ctor/copy/dtor 逐字段实证）
class MazeScript
{
public:
    MazeScript();
    MazeScript(const MazeScript& other);
    ~MazeScript();
    MazeScript& operator=(const MazeScript& other);
    int getGrid(int x, int y);
    void clear();

    int m_width;                              // +0x00
    int m_height;                             // +0x04
    std::vector<GridScript> m_grids;          // +0x08
    std::vector<std::pair<int, int> > m_vec14;  // +0x14
    std::vector<std::pair<int, int> > m_vec20;  // +0x20
    int m_2c;                                 // +0x2c
    int m_30;                                 // +0x30
    int m_34;                                 // +0x34
    RidableScript m_ridable;                  // +0x38
    int m_4c;                                 // +0x4c（maze quest type）
    int m_50;                                 // +0x50
    int m_54;                                 // +0x54
    std::vector<DungeonClearCondition> m_clearCondition; // +0x58
    int m_64;                                 // +0x64
    int m_68;                                 // +0x68（party count min）
    int m_6c;                                 // +0x6c（party count max）
    int m_70;                                 // +0x70
};

// ---- CDungeon 支撑类型（尺寸来自 ORIG 容器函数 / 构造体）----

struct stDungeonAssignItem_t
{
    int m_assignItemIdx;                        // +0x00（-1 = 按标准等级生成）
    std::list<std::pair<int, int> > m_list;     // +0x04

    stDungeonAssignItem_t();
    stDungeonAssignItem_t(const stDungeonAssignItem_t& other);
    ~stDungeonAssignItem_t();
    stDungeonAssignItem_t& operator=(const stDungeonAssignItem_t& other);
    void clear();
};

struct RandomList
{
    char m_pad[8];

    ~RandomList();
};

struct stEventMonster_t
{
    int m_eventIdx;      // +0x00
    int m_monsterIdx;    // +0x04
    int m_prob;          // +0x08
    int m_field0c;       // +0x0c
    int m_field10;       // +0x10

    ~stEventMonster_t();
};

struct stSecondEventMonster_t
{
    int m_eventIdx;      // +0x00
    int m_monsterIdx;    // +0x04
    int m_prob;          // +0x08
    int m_field0c;       // +0x0c
    int m_field10;       // +0x10

    ~stSecondEventMonster_t();
};

struct DimensionPartyCount
{
    char m_0;   // +0x00
    char m_1;   // +0x01
    char m_2;   // +0x02

    bool operator==(const DimensionPartyCount& other) const
    {
        return m_0 == other.m_0 && m_1 == other.m_1;
    }
};

class STMonsterTypeSpawnProb
{
public:
    STMonsterTypeSpawnProb();
    void clear();
    char m_pad[0x10];
};

class STDeadTowerData
{
public:
    STDeadTowerData();
    ~STDeadTowerData();
    void clear();
    std::vector<int> m_vec;   // +0x00
};

class STWarRoomData
{
public:
    STWarRoomData();
    ~STWarRoomData();
    void clear();
    void copy(STWarRoomData* other);

    int m_0;                                  // +0x00
    int m_4;                                  // +0x04
    int m_8;                                  // +0x08
    int m_c;                                  // +0x0c
    int m_10;                                 // +0x10
    std::vector<int> m_vec14;                 // +0x14
    std::vector<int> m_vec20;                 // +0x20
    std::vector<int> m_vec2c;                 // +0x2c
    std::vector<int> m_vec38;                 // +0x38
    std::vector<int> m_vec44;                 // +0x44
    int m_50;                                 // +0x50
    int m_54;                                 // +0x54
    int m_58;                                 // +0x58
    int m_5c[4];                              // +0x5c
    std::vector<int> m_vec6c;                 // +0x6c
    std::vector<int> m_vec78;                 // +0x78
    std::vector<int> m_vec84;                 // +0x84
    std::vector<int> m_vec90;                 // +0x90
    int m_9c[4];                              // +0x9c
    int m_ac;                                 // +0xac
    std::vector<STMonsterTypeSpawnProb> m_spawnProb; // +0xb0
    int m_bc;                                 // +0xbc
    int m_c0;                                 // +0xc0
    int m_c4[4];                              // +0xc4
    int m_d4;                                 // +0xd4
    int m_d8;                                 // +0xd8
    int m_dc[4];                              // +0xdc
    int m_ec;                                 // +0xec
    int m_f0;                                 // +0xf0
    int m_f4;                                 // +0xf4
    int m_f8;                                 // +0xf8
    int m_fc;                                 // +0xfc
};

class RequiredItem
{
public:
    RequiredItem();
    void clear();

    int m_0;   // +0x00（-1）
    int m_4;   // +0x04
    char m_8;  // +0x08
};

namespace advancealtar
{
struct _RewardItem
{
    char m_pad[0xc];
};

class ClearRewardData
{
public:
    ClearRewardData();
    ~ClearRewardData();
    ClearRewardData& operator=(const ClearRewardData& other);
    void clear();

    std::vector<_RewardItem> m_vec0;    // +0x00
    std::vector<_RewardItem> m_vecc;    // +0x0c
};

struct RoundRewardData
{
    int m_0;                          // +0x00
    int m_4;                          // +0x04
    std::vector<_RewardItem> m_vec8;  // +0x08
};

class SurvivalRewardData
{
public:
    SurvivalRewardData();
    ~SurvivalRewardData();
    SurvivalRewardData& operator=(const SurvivalRewardData& other);

    std::vector<RoundRewardData> m_vec;  // +0x00
};
}

struct TournamentRewardRate
{
    int m_0;   // +0x00
    int m_4;   // +0x04
    int m_8;   // +0x08
    int m_c;   // +0x0c
};

struct TournamentRewardItem
{
    char m_pad[4];
};

class TournamentRewardItemRate
{
public:
    TournamentRewardItemRate();
    ~TournamentRewardItemRate();
    TournamentRewardItemRate& operator=(const TournamentRewardItemRate& other);
    void Clear();

    std::vector<TournamentRewardItem> m_items;  // +0x00
    int m_c;                                    // +0x0c
};

struct STNamedMonsterMapPos
{
    int m_x;   // +0x00
    int m_y;   // +0x04
};

class stConditionEnterBossRoom_t
{
public:
    stConditionEnterBossRoom_t();
    ~stConditionEnterBossRoom_t();
    void clear();

    int m_0;                    // +0x00
    std::vector<int> m_vec4;    // +0x04
};

struct RecoveryInfo
{
    char m_pad[4];
};

struct RecoveryStairInfo
{
    RecoveryStairInfo();
    RecoveryStairInfo(const RecoveryStairInfo& other);
    ~RecoveryStairInfo();
    RecoveryStairInfo& operator=(const RecoveryStairInfo& other);

    char m_pad[0xc];
    std::vector<RecoveryInfo> m_vec;  // +0x0c
};

struct UseCoinDungeonDiff
{
    int m_0;    // +0x00
    char m_4;   // +0x04
    char m_5;   // +0x05
    char m_6;   // +0x06
    char m_7;   // +0x07
};

struct _DailySchedule
{
    char m_pad[4];
};

namespace AbsoluteChangeStat
{
enum T
{
    T_0 = 0
};
}

class STTowerDialog
{
public:
    STTowerDialog();
    STTowerDialog(const STTowerDialog& other);
    ~STTowerDialog();
    STTowerDialog& operator=(const STTowerDialog& other);
    void clear();

    int m_0;            // +0x00
    std::string m_str4; // +0x04
    char m_8;           // +0x08
    char m_9;           // +0x09
    char m_a;           // +0x0a
};

class STTowerFullDialog
{
public:
    STTowerFullDialog();
    ~STTowerFullDialog();
    void clear();

    std::vector<STTowerDialog> m_vec0;    // +0x00
    std::vector<STTowerDialog> m_vecc;    // +0x0c
    std::vector<STTowerDialog> m_vec18;   // +0x18
};

class CDailyScheduleManager
{
public:
    void AddDailSchedule(const std::vector<_DailySchedule>& schedule);
};

// ---- STDungeonScript（sizeof 0x50a，ORIG ctor 0x0836b5e2 / dtor 0x0836bd5c）----
struct STDungeonScript
{
    int m_0;                                // +0x00
    int m_4;                                // +0x04
    int m_8;                                // +0x08
    int m_c;                                // +0x0c
    std::string m_str10;                    // +0x10
    int m_14;                               // +0x14
    int m_18;                               // +0x18
    std::string m_str1c;                    // +0x1c
    std::string m_str20;                    // +0x20
    std::string m_str24;                    // +0x24
    std::string m_str28;                    // +0x28
    int m_2c;                               // +0x2c
    std::string m_str30;                    // +0x30
    int m_34;                               // +0x34
    std::vector<std::string> m_vec38;       // +0x38
    int m_44;                               // +0x44
    std::vector<std::string> m_vec48;       // +0x48
    std::string m_str54;                    // +0x54
    std::string m_str58;                    // +0x58
    std::string m_str74;                    // +0x74
    int m_78;                               // +0x78
    RequiredItem m_requiredItem7c;          // +0x7c
    char m_88;                              // +0x88
    RequiredItem m_requiredItem8c;          // +0x8c
    std::vector<std::pair<int, int> > m_vec98;   // +0x98
    int m_a4;                               // +0xa4
    int m_a8;                               // +0xa8
    int m_ac;                               // +0xac
    int m_b0;                               // +0xb0
    std::vector<std::pair<int, int> > m_vecc8;   // +0xc8
    std::vector<std::pair<int, int> > m_vecd4;   // +0xd4
    std::vector<std::pair<int, int> > m_vece0;   // +0xe0
    std::vector<stDungeonAssignItem_t> m_vecec;  // +0xec
    char m_140;                             // +0x140
    char m_141;                             // +0x141
    char m_142;                             // +0x142
    std::vector<std::pair<int, int> > m_vec144;  // +0x144
    char m_150;                             // +0x150
    int m_154;                              // +0x154
    char m_158;                             // +0x158
    char m_159;                             // +0x159
    std::vector<int> m_vec15c;              // +0x15c
    std::vector<MazeScript> m_mazeList170;  // +0x170
    std::string m_str17c;                   // +0x17c
    STDeadTowerData m_deadTower180;         // +0x180
    STWarRoomData m_warRoom18c;             // +0x18c
    std::map<AbsoluteChangeStat::T, float> m_statMap290;  // +0x290
    char m_2a8;                             // +0x2a8
    char m_2a9;                             // +0x2a9
    char m_2aa;                             // +0x2aa
    char m_2ab;                             // +0x2ab
    int m_2ac;                              // +0x2ac
    int m_2b0[3];                           // +0x2b0
    int m_2bc[3];                           // +0x2bc
    std::vector<int> m_vec2c8;              // +0x2c8
    advancealtar::ClearRewardData m_clearReward2d4;  // +0x2d4
    advancealtar::SurvivalRewardData m_survival2f8;  // +0x2f8
    std::vector<DimensionPartyCount> m_dimension308; // +0x308
    char m_314;                             // +0x314
    char m_315;                             // +0x315
    char m_316;                             // +0x316
    char m_317;                             // +0x317
    std::vector<_DailySchedule> m_dailySchedule;     // +0x318
    char m_324;                             // +0x324
    char m_325;                             // +0x325
    stConditionEnterBossRoom_t m_bossRoom328;        // +0x328
    int m_338;                              // +0x338
    int m_33c;                              // +0x33c
    std::map<int, std::vector<RandomList> > m_map340; // +0x340
    int m_358;                              // +0x358
    std::vector<std::pair<int, int> > m_vec35c;      // +0x35c
    int m_368;                              // +0x368
    int m_36c;                              // +0x36c
    int m_370;                              // +0x370
    int m_374;                              // +0x374
    int m_378;                              // +0x378
    int m_37c;                              // +0x37c
    int m_380;                              // +0x380
    std::vector<RecoveryStairInfo> m_vec384;         // +0x384
    STTowerFullDialog m_towerDialog390;     // +0x390
    int m_3b4;                              // +0x3b4
    int m_3b8;                              // +0x3b8
    int m_3bc;                              // +0x3bc
    int m_3c0;                              // +0x3c0
    std::vector<STNamedMonsterMapPos> m_vec3c8;      // +0x3c8
    int m_3d4;                              // +0x3d4
    int m_3d8;                              // +0x3d8
    int m_3dc;                              // +0x3dc
    std::vector<stEventMonster_t> m_vec3e0;          // +0x3e0
    int m_3ec;                              // +0x3ec
    int m_3f0;                              // +0x3f0
    int m_3f4;                              // +0x3f4
    std::vector<stSecondEventMonster_t> m_vec3f8;    // +0x3f8
    std::vector<std::vector<float> > m_vec404;       // +0x404
    int m_438;                              // +0x438
    int m_43c;                              // +0x43c
    int m_440;                              // +0x440
    int m_444;                              // +0x444
    std::string m_str448;                   // +0x448
    char m_44c;                             // +0x44c
    int m_450;                              // +0x450
    int m_454;                              // +0x454
    std::vector<int> m_vec458;              // +0x458
    std::vector<int> m_vec464;              // +0x464
    std::vector<int> m_vec470;              // +0x470
    char m_47c;                             // +0x47c
    int m_480;                              // +0x480
    int m_484;                              // +0x484
    int m_488;                              // +0x488
    char m_48c;                             // +0x48c
    int m_490;                              // +0x490
    char m_494;                             // +0x494
    int m_498;                              // +0x498
    std::map<int, TournamentRewardRate> m_map49c;    // +0x49c
    TournamentRewardItemRate m_itemRate4b4;          // +0x4b4
    int m_4c4;                              // +0x4c4
    std::map<unsigned char, unsigned int> m_map4c8;  // +0x4c8
    char m_4e0;                             // +0x4e0
    std::map<int, UseCoinDungeonDiff> m_map4e4;      // +0x4e4
    int m_4fc;                              // +0x4fc
    int m_500;                              // +0x500
    char m_504;                             // +0x504
    char m_505;                             // +0x505
    char m_508;                             // +0x508
    char m_509;                             // +0x509

    STDungeonScript();
    ~STDungeonScript();
    void Clear();
};

// ---- CDungeon（sizeof 0x934，vtable @0，CDungeon_TowerOfDespair 只换 vtable）----
class CDungeon
{
public:
    CDungeon();
    virtual ~CDungeon();

    virtual int _do_after_dungeon_start(CUser* user) const;

    // ---- 弱访问器（ORIG W 符号，定义于 CDungeon.cpp）----
    int get_index() const;
    int get_min_level() const;
    int get_standard_level() const;
    const char* GetDungeonName() const;
    int getDungeonKind() const;
    float get_exp_weight() const;
    int get_limit_party_count() const;
    int get_limit_Inout_count() const;
    int get_blood_dungeon_type() const;
    int get_dimension_possible() const;
    int get_dimension_min_partymem() const;
    int limitOfStackableItemInTower() const;
    int isNoFatigueDungeon() const;
    int GetType_DungeonInHeritance() const;
    int isTowerOfDespairDungeon() const;
    int getQuestNpcDungeon() const;
    int getAICharacterAppearRate() const;
    int getEventMonsterTotalProb() const;
    int getTryGenEventMonsterCnt() const;
    int getTryGenEventMonsterProb() const;
    int getSecondEventMonsterTotalProb() const;
    int getTryGenSecondEventMonsterCnt() const;
    int getTryGenSecondEventMonsterProb() const;
    int getRandomTowerMaxStage() const;
    int getDeathTowerMaxStage() const;
    int getDeathTowerMapIndex(unsigned int idx) const;
    int getTowerMaxClearRewardItemNum() const;
    int getHeroesRequiredQuest() const;
    int GetDummyAppearCount() const;
    int IsHaveDummyMap() const;
    int hasTowerItemDrop() const;
    std::vector<std::pair<int, int> >* getDropItems() const;
    std::vector<std::pair<int, int> >* getObjectDropItems() const;
    std::vector<stDungeonAssignItem_t>* getSpecialPassiveObjectItems() const;
    std::vector<stEventMonster_t>* getEventMonsters() const;
    std::vector<stSecondEventMonster_t>* getSecondEventMonsters() const;
    std::map<int, std::vector<RandomList> >* getRandomTowerMapIndexes() const;
    std::vector<MazeScript>* GetQuestMazeScriptVector(
        ENUM_MAZE_QUEST_TYPE mazeQuestType) const;

    void set_dungeon(STDungeonScript& script);
    void InsertMap(int mapType, int direction, CMap* map);
    void insertStart(int direction, CMap* map);
    int ChooseRandomMap(int mapType, int direction, int entrance) const;
    int GetMaze(int idx) const;
    void GetRandMaze(int& out) const;
    int GetRandMazebyPartyCount(int partyCount) const;
    int GetStartMap(int direction) const;
    int get_clear_item() const;
    int get_clear_item(int level) const;
    bool VerifyMaze() const;
    bool VerifyMaze(const MazeScript& maze) const;
    bool get_dimension_member_count(DimensionPartyCount& out) const;
    int isRiskDungeon() const;
    int isTournamentDungeon() const;
    int getTournamentRoundFatigue() const;
    int getLimitCoinDiff(int diff, int& out) const;
    bool checkFreeRevivalCondition(int diff) const;
    bool IsEnterEachMap() const;
    bool IsOpenEvenEnemy() const;
    bool IsOpenScheduleDungeon() const;
    int GetPartyMemberCoinLimit() const;

    int m_flag4;                    // +0x04
    int m_index;                    // +0x08
    int m_c;                        // +0x0c
    int m_10;                       // +0x10
    int m_14;                       // +0x14
    std::string m_name;             // +0x18
    std::map<int, int> m_clearItemMap; // +0x1c
    int m_34;                       // +0x34
    char m_38;                      // +0x38
    char m_39;                      // +0x39
    char m_3a;                      // +0x3a
    char m_3b;                      // +0x3b
    char m_3c;                      // +0x3c
    std::vector<std::pair<int, int> > m_vec40; // +0x40
    std::vector<std::pair<int, int> > m_vec4c; // +0x4c
    std::vector<stDungeonAssignItem_t> m_vec58; // +0x58
    __gnu_cxx::hash_map<int, CMap*> m_maplist[16]; // +0x64
    std::multimap<int, CMap*> m_map1a4[16];       // +0x1a4
    std::multimap<int, CMap*> m_dummyMap324[16];  // +0x324
    std::multimap<int, CMap*> m_dummyMap4a4[16];  // +0x4a4
    char m_624;                     // +0x624
    int m_628;                      // +0x628
    std::vector<MazeScript> m_mazeList;  // +0x62c
    std::vector<MazeScript> m_mazeList0; // +0x638
    std::vector<MazeScript> m_mazeList1; // +0x644
    int m_650;                      // +0x650
    int m_654;                      // +0x654
    std::map<int, std::vector<RandomList> > m_map658; // +0x658
    int m_670;                      // +0x670
    std::vector<int> m_vec674;      // +0x674
    int m_680;                      // +0x680
    int m_684;                      // +0x684
    int m_688;                      // +0x688
    int m_68c;                      // +0x68c
    std::vector<stEventMonster_t> m_eventMonsters; // +0x690
    int m_69c;                      // +0x69c
    int m_6a0;                      // +0x6a0
    int m_6a4;                      // +0x6a4
    std::vector<stSecondEventMonster_t> m_secondEventMonsters; // +0x6a8
    char m_6b4;                     // +0x6b4
    char m_6b5;                     // +0x6b5
    int m_6b8;                      // +0x6b8
    std::vector<DimensionPartyCount> m_dimensionPartyCount; // +0x6bc
    char m_6c8;                     // +0x6c8
    char m_6c9;                     // +0x6c9
    int m_6cc;                      // +0x6cc
    char m_6d0;                     // +0x6d0
    char m_6d1;                     // +0x6d1
    std::vector<int> m_vec6d4;      // +0x6d4
    char m_pad6e0[0x6f4 - 0x6e0];   // +0x6e0
    STWarRoomData m_warRoomData;    // +0x6f4
    RequiredItem m_requiredItem1;   // +0x7f4
    char m_800;                     // +0x800
    int m_804;                      // +0x804
    int m_808[3];                   // +0x808
    std::vector<int> m_vec814;      // +0x814
    int m_820[3];                   // +0x820
    advancealtar::ClearRewardData m_clearReward; // +0x82c
    advancealtar::SurvivalRewardData m_survivalReward; // +0x850
    RequiredItem m_requiredItem2;   // +0x860
    std::vector<std::pair<int, int> > m_vec86c; // +0x86c
    char m_878;                     // +0x878
    char m_879;                     // +0x879
    char m_87a;                     // +0x87a
    char m_87b;                     // +0x87b
    int m_87c;                      // +0x87c
    int m_880[5];                   // +0x880
    int m_894;                      // +0x894
    int m_898;                      // +0x898
    char m_89c;                     // +0x89c
    char m_89d;                     // +0x89d
    char m_89e;                     // +0x89e
    char m_89f;                     // +0x89f
    int m_8a0;                      // +0x8a0
    int m_8a4;                      // +0x8a4
    char m_8a8;                     // +0x8a8
    std::map<int, TournamentRewardRate> m_map8ac; // +0x8ac
    TournamentRewardItemRate m_itemRate8c4;       // +0x8c4
    std::map<unsigned char, unsigned int> m_map8d8; // +0x8d8
    std::vector<STNamedMonsterMapPos> m_vec8f0;   // +0x8f0
    char m_8fc;                     // +0x8fc
    std::vector<std::vector<float> > m_vec900;    // +0x900
    stConditionEnterBossRoom_t m_bossRoom90c;     // +0x90c
    std::map<int, UseCoinDungeonDiff> m_map91c;   // +0x91c
};

class CDungeon_TowerOfDespair : public CDungeon
{
public:
    CDungeon_TowerOfDespair();
    virtual ~CDungeon_TowerOfDespair();
    virtual int _do_after_dungeon_start(CUser* user) const;
};

namespace QuickParty
{
class CQuickPartySystemManager;
}

class CGameManager
{
public:
    QuickParty::CQuickPartySystemManager* GetQuickPartySystemManager();
};

CGameManager* G_CGameManager();

namespace QuickParty
{
class CQuickPartySystemManager
{
public:
    void init_quick_party_pool(const CDungeon* dungeon);
};
}
