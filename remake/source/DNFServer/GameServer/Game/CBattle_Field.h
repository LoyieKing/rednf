#ifndef DNF_GAME_CBATTLE_FIELD_H_
#define DNF_GAME_CBATTLE_FIELD_H_

// df_game_r CBattle_Field 还原（G2-4）。布局 0xcd0 字节，依据：
//   - ctor 082a44fe / dtor 082a499c（子对象构造/析构顺序 + 标量初值）
//   - reset_field 0830a1f6（全字段清零链）
//   - 各方法 Ghidra C + ORIG 反汇编（docs/class_func_reports/CBattle_Field.md）
// 共享结构（map_monster/MapInfo/map_item/_mapMonster 等）以 ORIG 构造体为准，
// 与并行批次 CMap.h 存在同名定义，集成时去重。

#include <algorithm>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CDataManager.h"
#include "CEquipItem.h"
#include "CItem.h"
#include "CItemList.h"
#include "CSystemTime.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameTypes.h"
#include "Inven_Item.h"
#include "PacketBuf.h"

#include "ActiveStaticInfo.h"
#include "CBattle_Field_deps.h"
#include "CClearCondition.h"
#include "CDungeonClearTracer.h"
#include "CRidable.h"
#include "CTournamentDungeonReward.h"

// ================= 共享数据结构（ORIG 构造体实证） =================

// ---- map_monster（0x34c 字节；ctor 081511c8） ----
struct map_monster
{
    int m_mobId;              // +0x00
    int m_instId;             // +0x04（实例序号/等级，int/byte 混用）
    char m_roleType;          // +0x08
    char m_pad9[3];           // +0x09
    int m_mobIndex;           // +0x0c（击杀逻辑的怪物索引）
    int m_level;              // +0x10（常按 byte 使用）
    char m_flag14;            // +0x14
    char m_flag13;            // +0x15
    char m_flag12;            // +0x16
    char m_flag11;            // +0x17
    int m_field18;            // +0x18
    char m_pad1c[4];          // +0x1c
    int m_field20;            // +0x20
    int m_field24;            // +0x24
    int m_damage;             // +0x28
    int m_dropCnt;            // +0x2c（默认 100）
    int m_dropRate;           // +0x30（默认 100）
    char m_flag34;            // +0x34
    char m_pad35[3];          // +0x35
    ActiveStaticInfo m_active;      // +0x38（0x2f0 字节）
    std::vector<int> m_mobList;     // +0x328
    short m_hellPartyGroupIdx;      // +0x334
    short m_hellPartyGroupOrder;    // +0x336
    char m_hellPartyScriptIdx;      // +0x338
    char m_pad339;                  // +0x339
    short m_bloodSmallRoundOrder;   // +0x33a
    short m_bloodPhaseTime;         // +0x33c
    short m_bloodRoundOrder;        // +0x33e
    short m_bloodSpawnIdx;          // +0x340
    short m_bloodPhaseIdx;          // +0x342
    char m_bloodKillFlag;           // +0x344
    char m_bloodDifficulty;         // +0x345
    short m_bloodRoundOrder2;       // +0x346
    char m_bloodSpawned;            // +0x348
    char m_isBoss;                  // +0x349
    char m_bloodRound;              // +0x34a
    char m_bloodRoundFlag;          // +0x34b

    map_monster();                          // ORIG 081511c8
    map_monster(const map_monster& other);  // ORIG 08151aea
    ~map_monster();                         // ORIG 081515ae
    map_monster& operator=(const map_monster& other);  // ORIG 08151d3a
    bool isNamedMonster() const;            // ORIG 08151182
};

// ---- _mapMonster（0x40 字节；ctor 0830e4fc） ----
struct _mapMonster
{
    int m_mobId;              // +0x00（-1 = 随机）
    char m_flag04;            // +0x04（ctor=1）
    int m_level;              // +0x08
    int m_spawnType;          // +0x0c（0=单人 1=队伍 2=队伍x2）
    int m_levelAdd;           // +0x10
    int m_field14;            // +0x14
    int m_roleType;           // +0x18
    char m_champion;          // +0x1c
    int m_posX;               // +0x20
    char m_pad24;             // +0x24
    int m_hpPct;              // +0x28（ctor=100）
    int m_orderIdx;           // +0x2c
    char m_flag30;            // +0x30
    char m_pad31[3];          // +0x31
    std::vector<int> m_mobList;  // +0x34

    _mapMonster();   // ORIG 0830e4fc
    ~_mapMonster();  // ORIG 0830e5b0
};

// ---- map_item（0x54 字节；ctor 081512cc） ----
struct map_item
{
    int m_count;              // +0x00
    int m_itemIndex;          // +0x04
    unsigned short m_dropIndex;  // +0x08
    char m_pad0a[2];          // +0x0a
    int m_createTick;         // +0x0c
    Inven_Item m_item;        // +0x10（0x3d 字节）
    char m_pad4d[3];          // +0x4d
    int m_ownerId;            // +0x50（ctor=0）

    map_item();   // ORIG 081512cc
    ~map_item();  // ORIG 081512dc
};

// ---- _mapItem（0x10 字节；ctor 0815088e） ----
struct _mapItem
{
    int m_objectIdx;   // +0x00
    int m_field04;     // +0x04（ctor=0）
    int m_field08;     // +0x08（ctor=0）
    int m_field0c;     // +0x0c（ctor=0）

    _mapItem();   // ORIG 0815088e
    ~_mapItem();
};

// ---- TournamentMonster（8 字节；ctor 0830e4d4 / Clear 0830e4bc） ----
struct TournamentMonster
{
    int m_mobId;   // +0x00
    int m_rate;    // +0x04

    TournamentMonster();  // ORIG 0830e4d4
    void Clear();         // ORIG 0830e4bc
};

// ---- STAssignBloodSystem（0x28 字节；vector 元素尺寸 08311824 实证） ----
struct STAssignBloodSystem
{
    int m_id;          // +0x00
    int m_field04;     // +0x04
    int m_phase;       // +0x08
    int m_field0c;     // +0x0c
    int m_field10;     // +0x10
    int m_timeA;       // +0x14
    int m_timeB;       // +0x18
    int m_startCnt;    // +0x1c
    int m_cntInc;      // +0x20
    int m_totalCnt;    // +0x24
};

// ---- STAssignBloodPhaseTime（0x1c 字节；vector 元素尺寸 0830ffee 实证） ----
struct STAssignBloodPhaseTime
{
    int m_smallRoundOrder;   // +0x00
    int m_phaseIdx;          // +0x04（-1 = 随机）
    int m_interval;          // +0x08
    float m_time;            // +0x0c
    int m_field10;           // +0x10
    int m_spawnCnt;          // +0x14
    char m_difficulty;       // +0x18
    char m_pad19[3];         // +0x19
};

// ---- MapInfo（0xec 字节；ctor 08151394） ----
struct MapInfo
{
    char m_key;                       // +0x00（GetMapInfoKeyValue 结果低字节）
    char m_visited;                   // +0x01
    char m_visitedGoto;               // +0x02
    char m_pad3[1];                   // +0x03
    int m_mapIndex;                   // +0x04
    char m_gridValue;                 // +0x08
    char m_pad9[3];                   // +0x09
    std::map<int, map_monster> m_monsterMap;   // +0x0c
    std::map<int, map_item> m_itemMap;         // +0x24
    int m_assignItemIdx;              // +0x3c
    int m_dropItemCnt;                // +0x40
    char m_mapType;                   // +0x44
    char m_dummyBossMap;              // +0x45
    char m_pad46[2];                  // +0x46
    int m_mapBasisLevel;              // +0x48
    stMapMonsterKillChecker_t m_killChecker1;  // +0x4c（0x18）
    stMapMonsterKillChecker_t m_killChecker2;  // +0x64（0x18）
    stMapPlayInfo_t m_playInfo;               // +0x7c（0x10）
    std::map<int, int> m_hellPartyGroup;      // +0x8c
    char m_assignNPCByPotion;         // +0xa4
    char m_padA5[3];                  // +0xa5
    std::map<int, int> m_mapA8;       // +0xa8（ORIG MapInfo ctor 实证）
    std::vector<int> m_vecC0;         // +0xc0（ORIG MapInfo ctor 实证）
    short m_cc;                       // +0xcc（Clear 写 0）
    char m_padCE[2];                  // +0xce
    std::multimap<int, map_monster> m_tournamentMob;  // +0xd0
    char m_layeredMap;                // +0xe8
    char m_padE9[3];                  // +0xe9

    MapInfo();                                // ORIG 08151394
    MapInfo(const MapInfo& other);            // ORIG 08235d40
    ~MapInfo();                               // ORIG 081518bc
    MapInfo& operator=(const MapInfo& other); // ORIG 08151946
    void Clear();                             // ORIG 081514ae
    void Add_Mob(map_monster mob);            // ORIG 08151612（按值）
    void Add_Item(map_item item);             // ORIG 081517e0（按值）
    void SelectDonsterItemDropLimit(unsigned int limit);  // ORIG 0830e800
};

// ================= CBattle_Field =================

class CBattle_Field
{
public:
    // ---- 嵌套类型 ----
    struct TournamentInfo
    {
        short m_roundOrder;            // +0x00
        char m_pad2[2];                // +0x02
        TournamentMonster m_monster[2];  // +0x04（2 × 8 字节）
        int m_group;                   // +0x14

        TournamentInfo();                       // ORIG 0830ea64
        ~TournamentInfo();                      // ORIG 08314bb6
        void Clear();                           // ORIG 0830ea1e
        bool operator<(const TournamentInfo& other) const;  // ORIG 0830eaa6
    };

    class CBloodRound
    {
    public:
        short m_preMapRoundOrder;       // +0x00
        short m_roundOrder;             // +0x02
        short m_smallRoundOrder;        // +0x04
        short m_lastSmallRoundOrder;    // +0x06

        void setBloodRoundOrder();                 // ORIG 0830eb72
        void setPreMapBloodRoundOrder();           // ORIG 0830eb84
        void incBloodRoundOrder();                 // ORIG 0830ebe6
        unsigned short getBloodRoundOrder(); // ORIG 0830ebc2
        void setBloodSmallRoundOrder(short v);     // ORIG 0830eba8
        void incBloodSmallRoundOrder();            // ORIG 0830ebfc
        unsigned short getBloodSmallRoundOrder();  // ORIG 0830ebce
        void setBloodLastSmallRoundOrder(int v);   // ORIG 0830eb96
        unsigned short getBloodLastSmallRoundOrder();  // ORIG 0830ebda
        void reset_blood_values();                 // ORIG 08306d66
    };

    class CBloodClearRewardData
    {
    public:
        struct stRewardItem_t
        {
            int m_count;                  // +0x00
            Inven_Item m_item[10];        // +0x04（10 × 0x3d）
            char m_pad[2];                // +0x266

            stRewardItem_t();             // ORIG 082a4458
            void reset();                 // ORIG 08306f5c
        };

        unsigned int m_playTime;          // +0x00
        unsigned int m_prevPlayTime;      // +0x04
        int m_bloodRound;                 // +0x08
        unsigned int m_startTick;         // +0x0c
        unsigned int m_endTick;           // +0x10
        char m_isBloodClear;              // +0x14
        int m_rewardExp[4];               // +0x18
        stRewardItem_t m_reward[4];       // +0x28
        int m_ultimateClearPoint;         // +0x9c8

        CBloodClearRewardData();               // ORIG 082a448e
        void reset();                          // ORIG 08306ee0
        void onStartBloodRound();              // ORIG 08306faa
        void onFinishBloodRound(bool isClear, short round, CParty* party,
                                const CDungeon* dungeon);  // ORIG 08306fc4
        unsigned int getBloodRoundClearTime();  // ORIG 0830ec64
        void addPlayTime(unsigned int t);       // ORIG 0830ec3a
        unsigned int getPlayTime();       // ORIG 0830ec5a
        int getCurrentBloodRound();       // ORIG 085bf0e6
        void setCurrentBloodRound(int v);       // ORIG 085bf0f2
        int getRewardItemCount(int idx);  // ORIG 085bf100
        Inven_Item* getRewardItem(int idx);     // ORIG 085bf11a
        int getRewardExp(int idx);        // ORIG 085bf132
        void setRewardExp(int idx, unsigned int exp);  // ORIG 085bf144
        void setRewardItem(int idx, int itemIdx);      // ORIG 085bf15a
        void setRewarItemCopy(Inven_Item item, int rewardIdx, int itemIdx);  // ORIG 085bf176
        void setRewardItemAddInfo(float rate, int rewardIdx, int itemIdx);   // ORIG 085bf1a8
        bool isBloodClear();              // ORIG 085bf21c
        void setBloodClear(bool v);             // ORIG 0830ec7c
        int getUltimateRewardItem();            // ORIG 083070ca
        int getUltimateRoundClearPoint(); // ORIG 0830ec94
        void incUltimateRoundClearPoint(int v); // ORIG 08307096
    };

    struct CTowerOfDespairRewardData
    {
        std::vector<std::pair<int, int> > m_todMap;  // +0x00
        std::pair<int, int> m_todPos;                // +0x0c

        CTowerOfDespairRewardData();  // ORIG 082a44c8
        ~CTowerOfDespairRewardData(); // ORIG 082a44ea
        void reset_tod();             // ORIG 083071a6
    };

    struct stConditionEnterBossRoomStatus_t
    {
        int m_monsterIdx;                       // +0x00
        std::vector<std::pair<int, int> > m_pos;  // +0x04
        int m_curKillCount;                     // +0x10
        int m_maxKillCount;                     // +0x14

        stConditionEnterBossRoomStatus_t();                    // ORIG 0830ee22
        stConditionEnterBossRoomStatus_t(const stConditionEnterBossRoomStatus_t&);  // ORIG 08312ee4
        ~stConditionEnterBossRoomStatus_t();                   // ORIG 082d2c00
        stConditionEnterBossRoomStatus_t& operator=(
            const stConditionEnterBossRoomStatus_t&);          // ORIG 08312f84
    };

    struct stEventMonsterStatus_t
    {
        int m_eventIdx;              // +0x00
        std::pair<int, int> m_pos;   // +0x04
        int m_field0c;               // +0x0c
        int m_field10;               // +0x10

        stEventMonsterStatus_t();    // ORIG 0830ee4c
        ~stEventMonsterStatus_t();   // ORIG 08316adc
    };

    // ================= 构造/析构 =================
    CBattle_Field();   // ORIG 082a44fe
    ~CBattle_Field();  // ORIG 082a499c

    // ================= 成员函数（262 符号全量） =================
    void ConsistMap();                                        // 083031d2
    int GetCurPosX();                                         // 085bf32c
    int GetCurPosY();                                         // 085bf382
    CDungeon* GetDungeon();                                   // 080fdcfc
    MapInfo* GetMapInfo();                                    // 0822d03e
    void MakePacket(int packetId, void* packet);              // 083090a4
    void onClearMap(bool isClear);                            // 0830dd2c
    void onStartMap();                                        // 0830dc74
    int set_map_id(const GridScript* gs, int& mapId,
                   std::vector<int>& candidator);             // 08308882
    bool GetFieldMob(int uid, map_monster& out);              // 0830a708
    void SetGridPath(MazeScript* maze, int dummyAppearCnt);   // 082ffb30
    void getCurPosXY(int& x, int& y);                         // 08300da6
    int getMapWidth();                                        // 0822cefc
    int get_cur_map();                                        // 0822cffc
    bool isQuestMaze();                                       // 0830e31e
    int pickup_item(int uid);                                 // 0830daee
    void reset_field();                                       // 0830a1f6
    void SetCurSeatNo(int seatNo);                            // 082a43cc
    void getLastPosXY(int& x, int& y);                        // 08300e44
    int getMapHeight();                                       // 0822cf0a
    bool kill_monster(PacketGuard& packet, map_monster& mob, unsigned int& reward,
                      const STKillMonsterData& data);         // 0830bc78
    void setQuestMaze(bool flag);                             // 0830e302
    CMap* GetCurrentMap();                                    // 085bf28c
    std::map<int, map_monster>* GetMonsterMap();              // 085bf0a2
    void MakeRoundInfo(std::map<short, TournamentInfo>& roundMap,
                       std::map<short, std::vector<TournamentInfo> >& roundInfoMap,
                       std::list<_mapMonster>& mobList);      // 08303f66
    bool SelectDungeon(int dungeonIdx, const CDungeon* dungeon, int diff,
                       ENUM_DUNGEON_TYPE type, int mode, int a, CWorldMap* worldMap,
                       bool isHellParty, int questStageMap);  // 082ff486
    int getBloodState();                                      // 0822cf18
    int get_map_index(int x, int y);                          // 085bf260
    void moveMap_1_Log(int x, int y);                         // 0830766e
    void moveMap_2_Log(int x, int y);                         // 08307740
    void moveMap_3_Log(int x, int y);                         // 08307812
    void moveMap_4_Log(int x, int y, const GridScript* gs);   // 083078e4
    bool setBloodState(ENUM_BLOOD_STATE state);               // 085bf0b8（返回是否更新）
    bool ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx);  // 085bf29a
    MapInfo* GetLastMapInfo();                                // 0830eac8
    bool IsClearDungeon();                                    // 0822d08c
    bool SetDamageValue(int uid, int damage);                 // 08308ff8
    void _MakeDropItems(std::list<map_item>& itemList, MapInfo& mapInfo,
                        const map_monster& mob, unsigned short uid, int dieReason,
                        int a, int b, bool hellPartyKilled, std::vector<bool>& flags,
                        float rate, int premiumRate);         // 0830adf6
    int check_boss_map(int x, int y);                         // 083079ba
    bool get_field_item(int uid, map_item& out);              // 0830d9c0
    bool is_visited_map(int x, int y);                        // 08307c7a
    bool is_visited_map_goto(int x, int y);                   // 08307cbe
    void swap_curpos_xy(int x, int y);                        // 0830878e
    void BloodPhaseSpawn(short smallRound, short& nextRound, int timerKey,
                         int phaseIdx);                       // 0830553a
    void checkLayeredMap(int x, int y);                       // 0830e32e
    bool check_end_point();                                   // 0822cfa6
    void set_visited_map(int x, int y, bool visited);         // 083086d2
    void ChoiceFirstGroup(std::vector<TournamentInfo>& src,
                          std::vector<TournamentInfo>& dst);  // 08303a1c
    bool check_grid_clear();                                  // 0830a0e8
    int getBloodMaxRound();                                   // 0830ec12
    int get_dungeon_diff();                                   // 080f981c
    bool get_monster_list(std::list<_mapMonster>& mobList);   // 083012a4
    void setQuestMazeInfo();                                  // 0830e170
    void set_dungeon_diff(char diff);                         // 0830ed1e
    bool ConsistTournament(const CMap* map, std::list<_mapMonster>& mobList,
                           std::map<short, std::vector<TournamentInfo> >& roundMap);  // 083035c8
    MapInfo* GetCurrentMapInfo();                             // 0822cbcc
    MapInfo* GetMapInfoFromPos(int x, int y, bool create);    // 0822cc48
    void MakeNextRoundInfo(int memberCount,
                           std::map<short, TournamentInfo>& roundMap);  // 0830410c
    bool add_blood_monster(map_monster& mob, int& uid, int& seq,
                           std::vector<STAssignBloodSystem> systems,
                           std::vector<STAssignBloodPhaseTime> phaseTimes);  // 08304804
    void add_item_curr_map(int& itemUid, int& itemIdx);       // 08302bc6
    bool add_monster_blood(map_monster& mob, int& uid, int& seq);  // 08301bf6
    void add_monster_event(int& seq, int x, int y);           // 083023ec
    bool check_start_point();                                 // 0822cf50
    bool check_visited_map(int x, int y, bool& firstVisit);   // 08307d7c
    int get_dungeon_index();                                  // 0822d0d8
    int GetCurrentMapIndex();                                 // 0822d05e
    int GetMapInfoKeyValue(int x, int y);                     // 0822cbbe
    bool MakeBasicRoundInfo(int memberCount,
                            std::vector<TournamentMonster>& monsters,
                            std::vector<TournamentInfo>& roundInfo);  // 08303896
    void add_monster_APC_AI(int& seq);                        // 08301d76
    int getLayeredMapIndex();                                 // 0830df2e
    int get_candidator_map(const CDungeon* dungeon, const GridScript* gs, int& mapId,
                           std::vector<int>& candidator, int x, int y, int dir);  // 08308214
    bool is_last_field_item(int uid);                         // 0830da6c
    bool kill_blood_monster(CUser* user, int count, unsigned short* uids);  // 0830d58c
    void on_not_visited_map(int x, int y, bool& flag);        // 08307d0c
    void reset_blood_values();                                // 08306d86
    void setLayeredMapIndex();                                // 0830df3c
    void setPremiumDropRate(WongWork::stGenerateRefData_t& ref, int premiumRate);  // 0830b848
    void startUltimateRound();                                // 083064de
    bool IsAssignNPCByPotion();                               // 0822cee2
    bool IsEnableHellDungeon();                               // 0830ed07
    char IsFirstEnterDungeon();                               // 08307656
    bool IsVisitedCurrentMap();                               // 085bf2e8
    bool SetDamagedPacketCnt(int uid);                        // 08308f4e
    int check_movemap_range(int x, int y);                    // 08307b2a
    bool isWorldMapTowerArea();                               // 085bf31c
    void set_visited_map_goto(int x, int y);                  // 08308a24
    void setEplpWaitingTimer(QuickParty::RandomBuffDungeon type);  // 0830ac7a
    void set_monster_ability();                               // 083026da
    void AddTournamentMonster(const TournamentInfo& info, std::list<_mapMonster>& mobList,
                              bool isUltimate);              // 08304718
    bool DestroyPassiveObject(int idx);                       // 082ff3ec
    char* GetEnableHellDungeon();                             // 0822cf26
    void InsertTournamentInfo(std::vector<int>& indexes,
                              std::vector<TournamentInfo>& infos,
                              std::map<short, TournamentInfo>& roundMap);  // 08303e02
    void SetAssignNPCByPotion(bool flag);                     // 085d19d6
    void SetEnableHellDungeon(bool flag);                     // 0822cf30
    bool checkBloodSpawnOrder(short smallRound);              // 08306e32
    void check_elevator_timer();                              // 0830d902
    int getAddChampionMobCnt();                               // 083010ee
    int get_invade_direction(int x, int y);                   // 08307e44
    void setDungeonClearTrace(int mapId, int maxBoss, int killedBoss,
                              int killedMonster);             // 0830107e
    void setWorldMapTowerArea(bool flag);                     // 085bf300
    int GetHellPartyEventType(char diff);                     // 082ffb06
    int GetTotalSkillMaterial();                              // 0830de1c
    int GetTournamentCurRound();                              // 082fe6ae
    void SetRidableObjectState(int idx, eRidableObjectState state);  // 085bf228
    void SetTournamentCurRound(int round);                    // 082fe69c
    int getLayeredMapKeyValue(int idx);                       // 0830df56
    char getUltimateDifficylty();                             // 0822d02e
    void setUltimateDifficulty(char diff);                    // 0822d012
    void BloodSpawnTimerManager(bool a, bool b);              // 08304dd2
    void BloodMonsterSpawnPacket(int a, short b, short c);    // 08305992
    void BloodRoundMaxTimePacket(short round, int a, int b);  // 08305dc4
    void BloodRoundIntervalPacket(short round, int interval); // 08305e9c
    void ConsistTournamentRound(std::vector<TournamentInfo>& first,
                                std::vector<TournamentInfo>& second,
                                std::map<short, std::vector<TournamentInfo> >& roundMap,
                                std::list<_mapMonster>& mobList);  // 08303ace
    int GetHellPartyDifficulty(ENUM_DUNGEON_TYPE type);       // 082ffa2e
    bool IsKilledAllHellGruoups();                            // 085bf250
    void ResetFirstEnterDungeon();                            // 08307662
    void add_hell_party_monster(map_monster& mob, int& uid, int& seq,
                                std::map<unsigned short, STHellPartyGroupOrder>* groups);
                                                              // 083071ba
    void add_monster_hell_party(map_monster& mob, bool& killedAll, int& uid, int& seq);
                                                              // 08301b8c
    int getItemDropDungeonType();                             // 0830aa46
    MapInfo* getLayeredMapInfoIndex(int idx);                 // 0830df88
    int getNextRoundDifficulty(short round);                  // 083061b6
    short getNextSamllRoundOrder(short round, int diff);      // 08306212
    void reset_hell_party_value();                            // 0830a57c
    void CheckAppearRidableObject(PacketGuard* packet);       // 0830eca2
    void add_monsters_current_map(std::list<_mapMonster>& mobList,
                                  std::vector<int>& championList, map_monster& mob,
                                  int& count, int& uid, int& mapCount);  // 0830162e
    bool forceValuesToChangeRound(short round, short smallRound, int diff);  // 08305f50
    MapInfo* getCurrentLayeredMapInfo();                      // 0830df66
    int getRandomBuffDungeonType(int dungeonIdx);             // 0830aaa4
    void MakeTournamentMonsterList(
        std::map<short, std::vector<TournamentInfo> >& roundMap,
        std::list<_mapMonster>& mobList);                     // 08304594
    void _MakeNotiPacketDieMonster(PacketGuard& packet, std::list<map_item>& itemList,
                                   int uid, unsigned short mapIndex,
                                   const map_monster& mob, map_monster& out);  // 0830b8cc
    void send_elevator_time_packet(char state);               // 0830d848
    void add_monster_APC_AI_by_rate(map_monster& mob, int& uid, int x, int y);  // 083020b6
    bool checkClearedNamedMonsterMap(unsigned char idx, int uid);  // 08300c76
    void BloodCrazyMonsterSpawnPacket();                      // 08305d0c
    void KillHellPartyGroupMonsterCnt(MapInfo& mapInfo, map_monster& mob, bool& killedAll);
                                                              // 0830d704
    int getNextRoundRandomDifficulty();                       // 08306156
    int get_blood_spawn_cnt_of_phase(short smallRound, short phase);  // 08306a62
    void sendUltimateRandomDifficulty(int a, int b);          // 08306436
    void sendUltimateRandomDifficulty(int a, int b, int c);   // 0830637c
    int BloodPhaseSpawnTimerResgister(short smallRound);      // 083053d2
    void checkKilledSpawnBloodMonsters(int monsterCount);     // 08304f8e
    int get_blood_spawn_time_of_phase(short smallRound);      // 0830690a
    void insert_tournamentRoundMonster();                     // 083029b8
    int get_blood_round_of_small_round(short smallRound);     // 083065f8
    void add_monster_additional_champion(std::vector<int>& championList, int count);  // 083022a8
    void check_random_appear_hell_dungeon(const CDungeon* dungeon, ENUM_DUNGEON_MODE mode,
                                          ENUM_DUNGEON_TYPE type, bool& flag);  // 0830a862
    int get_blood_spawn_cnt_of_small_round(short smallRound); // 08306670
    void add_monster_list_bossroom_condition(std::list<_mapMonster>& mobList,
                                             int x, int y);   // 083013e2
    void choose_random_bossmap_from_dummymaps(const CDungeon* dungeon,
                                              const GridScript* gs, int& mapId,
                                              bool& flag, int x, int y, int dir);  // 08307eca
    int get_blood_spawn_small_round_interval(short smallRound);  // 083066ea
    int get_blood_spawn_time_of_next_small_round(short smallRound);  // 0830676a
    int get_blood_round_difficylty_of_small_round(short smallRound);  // 083062fe
    int get_blood_spawn_phase_total_cnt_of_small_round(short smallRound);  // 08306c0e
    CMap* getMap(int unused);                                 // 08300ed4
    int MoveMap(int x, int y, bool& firstVisit);              // 08308ae6
    MazeScript* getMaze();                                    // 0822d052
    void SetParty(CParty* party);                             // 082a43be
    int drop_item(map_item item);                             // 0830db84
    void onUseItem(CUser* user, const Inven_Item& item);      // 0830dbde
    MazeScript* GetAppropriateMaze(ENUM_DUNGEON_DIFFICULTY diff) const;  // 082ff128
    char convertAPCRoleType2MonsterRoleType(char role) const;  // 0830a812
    bool CheckHellPartyCondition();                           // 082ff988
    bool CheckAppearRidableObject();

    // ================= 成员布局（0xcd0 字节） =================
    CParty* m_party;                       // +0x00
    int m_startX;                          // +0x04
    int m_startY;                          // +0x08
    // +0x0c 起为 10 元素 pair 数组（ORIG getCurPosXY 08300da6 / getLastPosXY
    // 08300e44 / swap_curpos_xy 0830878e 实证）：槽 0=终点（check_end_point
    // 比对 +0xc/+0x10）、槽 2+seat=当前位置、槽 6+seat=上次位置。
    std::pair<int, int> m_pos[10];         // +0x0c（0x50 字节，+0x0c..+0x5b）
    char m_firstEnterDungeon;              // +0x5c
    char m_pad5d[3];                       // +0x5d
    int m_curX;                            // +0x60
    int m_curY;                            // +0x64
    int m_lastX;                           // +0x68
    int m_lastY;                           // +0x6c
    MazeScript* m_maze;                    // +0x70
    MazeScript* m_gridMaze;                // +0x74
    std::map<int, MapInfo> m_mapInfo;      // +0x78
    Mutex m_mutexField;                    // +0x90
    Mutex m_mutexEvent;                    // +0xa8
    std::vector<std::pair<int, int> > m_dummyMapPos;  // +0xc0
    unsigned int m_dummyMapPosIdx;         // +0xcc
    CRidable m_ridable;                    // +0xd0
    CClearCondition m_clearCondition;      // +0xf0
    CMap* m_pMap;                          // +0x108
    int m_layeredMapIndex;                 // +0x10c
    char m_questMaze;                      // +0x110
    char m_pad111[3];                      // +0x111
    int m_curSeatNo;                       // +0x114（SetCurSeatNo 082a43cc / getCurPosXY 实证）
    std::vector<int> m_apcList;            // +0x118
    int m_field124;                        // +0x124
    short m_villageMonsterId;              // +0x128
    char m_pad12a[2];                      // +0x12a
    int m_villageMonsterCount;             // +0x12c
    int m_field130;                        // +0x130
    int m_field134;                        // +0x134
    char m_elevatorState;                  // +0x138
    char m_elevatorTimer;                  // +0x139
    char m_elevatorFlag;                   // +0x13a
    char m_pad13b;                         // +0x13b
    std::map<short, std::vector<TournamentInfo> > m_tournamentRoundMap;  // +0x13c
    int m_tournamentCurRound;         // +0x154（Set/GetTournamentCurRound 实证）
    CDungeonClearTracer m_dungeonClearTracer;  // +0x158（0x18 字节）
    std::vector<stConditionEnterBossRoomStatus_t> m_bossRoomConditions;   // +0x170
    std::vector<stEventMonsterStatus_t> m_eventMonsterStatus;             // +0x17c
    CDungeon* m_pDungeon;                  // +0x188
    int m_mapWidth;                        // +0x18c
    int m_mapHeight;                       // +0x190
    int m_dungeonIndex;                    // +0x194
    int m_field198;                        // +0x198
    int m_questStageMap;                   // +0x19c
    int m_dungeonDiff;                     // +0x1a0
    int m_field1a4;                        // +0x1a4
    int m_apcKillCount;                    // +0x1a8
    int m_mazeType;                        // +0x1ac
    int m_hellPartyMode;                   // +0x1b0
    int m_bloodState;                      // +0x1b4
    char m_hellPartyActive;                // +0x1b8
    char m_pad1b9[3];                      // +0x1b9
    int m_hellMapIdx;                      // +0x1bc
    int m_hellPosX;                        // +0x1c0
    int m_hellPosY;                        // +0x1c4
    char m_visitedGotoFlag;                // +0x1c8
    char m_field1c9;                       // +0x1c9
    char m_killedAllHellGroups;            // +0x1ca
    char m_field1cb;                       // +0x1cb
    char m_hellPartyDifficulty;            // +0x1cc
    char m_hellPartyEventType;             // +0x1cd
    char m_hellPartyRandom;                // +0x1ce
    char m_hellPartyDropFlag;              // +0x1cf
    int m_hellItemStatistic[6];            // +0x1d0
    float m_hellPartyRatio0;               // +0x1e8
    float m_hellPartyRatio1;               // +0x1ec
    float m_hellPartyRatio2;               // +0x1f0
    float m_hellPartyRatio3;               // +0x1f4
    float m_hellPartyRatio4;               // +0x1f8
    float m_hellPartyRatio5;               // +0x1fc
    float m_hellPartyDungeonRate[4];       // +0x200
    float m_hellPartyMapRate[5];           // +0x210
    float m_hellPartyItemRate[4];          // +0x224
    float m_hellPartyScriptRate;           // +0x234
    int m_hellPartyLevel;                  // +0x238
    char m_worldMapTowerArea;              // +0x23c
    char m_pad23d[3];                      // +0x23d
    std::vector<STAssignBloodSystem> m_bloodSystems;        // +0x240
    std::vector<STAssignBloodPhaseTime> m_bloodPhaseTimes;  // +0x24c
    std::vector<short> m_bloodPhaseOrder;  // +0x258
    std::vector<bool> m_bloodPhaseClear;   // +0x264（vector<bool> 0x14 字节）
    char m_bloodSpawnCnt[0x16];            // +0x278
    char m_bloodSpawnTime[0x16];           // +0x28e
    int m_bloodSpawnCntCur;                // +0x2a4
    short m_bloodCrazyRound;               // +0x2a8
    char m_bloodCrazyType;                 // +0x2aa
    char m_ultimateDifficulty;             // +0x2ab
    CBloodRound m_bloodRound;              // +0x2ac
    int m_bloodState2;                     // +0x2b4
    CBloodClearRewardData m_bloodReward;   // +0x2b8
    CTowerOfDespairRewardData m_todReward; // +0xc84
    CTournamentDungeonReward m_tournamentReward;  // +0xc98
};

static_assert(sizeof(CBattle_Field) == 0xcd0, "CBattle_Field size");
static_assert(sizeof(map_monster) == 0x34c, "map_monster size");
static_assert(sizeof(_mapMonster) == 0x40, "_mapMonster size");
static_assert(sizeof(map_item) == 0x54, "map_item size");
static_assert(sizeof(MapInfo) == 0xec, "MapInfo size");
static_assert(sizeof(TournamentMonster) == 8, "TournamentMonster size");
static_assert(sizeof(STAssignBloodSystem) == 0x28, "STAssignBloodSystem size");
static_assert(sizeof(STAssignBloodPhaseTime) == 0x1c, "STAssignBloodPhaseTime size");
static_assert(sizeof(CBattle_Field::CBloodClearRewardData::stRewardItem_t) == 0x268,
              "stRewardItem_t size");
static_assert(sizeof(CBattle_Field::CBloodClearRewardData) == 0x9cc,
              "CBloodClearRewardData size");
static_assert(sizeof(CBattle_Field::CTowerOfDespairRewardData) == 0x14,
              "CTowerOfDespairRewardData size");
static_assert(sizeof(CBattle_Field::stConditionEnterBossRoomStatus_t) == 0x18,
              "stConditionEnterBossRoomStatus_t size");
static_assert(sizeof(CBattle_Field::stEventMonsterStatus_t) == 0x14,
              "stEventMonsterStatus_t size");
static_assert(sizeof(CBattle_Field::TournamentInfo) == 0x18, "TournamentInfo size");
static_assert(sizeof(CBattle_Field::CBloodRound) == 8, "CBloodRound size");

#endif  // DNF_GAME_CBATTLE_FIELD_H_
