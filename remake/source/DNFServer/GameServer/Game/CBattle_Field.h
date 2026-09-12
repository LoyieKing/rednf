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
#include "CMap.h"            // CMap + _mapMonster/_mapItem/TournamentMonster/
                             // STAssignBloodSystem/STAssignBloodPhaseTime 唯一权威定义
#include "CEquipItem.h"
#include "CItem.h"
#include "CItemList.h"
#include "CSystemTime.h"
class CUser;                 // 前向声明（本头仅 CUser* 形参：kill_blood_monster/onUseItem）。
                             // [SweepBattleMonsterDeps 2026-09-10，Main 批准] 原
                             // #include "CUser.h" 会带出 CUser.h→TaiwanQueries.h→
                             // TaiwanCash.h→ThreadInterface.h:29→<iostream> 链，使每个
                             // 消费 TU 发射 _ZStL8__ioinit + _GLOBAL__I_<首强符号>；ORIG 中
                             // BattleMonsterDepsC2 对应 TU 无 GI（509 个 GI 键符号地址无一
                             // 落在 0x08300c76..0x0830e41b）⇒ 断链消多余 GI/ioinit（P327/P338）。
#include "CUserCharacInfo.h"
#include "GameTypes.h"
#include "Inven_Item.h"
#include "PacketBuf.h"

#include "ActiveStaticInfo.h"
#include "CBattle_Field_shared.h"
#include "CClearCondition.h"
#include "CDungeon.h"          // CDungeon + MazeScript/GridScript + ENUM_MAZE_QUEST_TYPE
#include "CDungeonClearTracer.h"
#include "CMonster.h"         // CMonster（CBattle_Field_unresolved.cpp 定义 getMonsterScript 等需要完整类型）
// —— 依赖降级（batch-15 步骤①，CParty.h↔CBattle_Field.h 解环，为 m_battleField
// 内嵌铺路）：CParty 仅指针用 → 前向声明；TYPE/MODE/eRidableObjectState/
// QuickParty::RandomBuffDungeon 改走权威定义头。
class CParty;
#include "CQuickPartyRewardManager.h"   // QuickParty::RandomBuffDungeon（自足头）
#include "GameEnums.h"                  // ENUM_DUNGEON_MODE / eRidableObjectState
#ifndef DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
enum ENUM_DUNGEON_TYPE                   // 与 CParty.h:45-51 占位段逐字同体，
{                                        // 共享 guard，行为与原传递闭包一致
    ENUM_DUNGEON_TYPE_0 = 0
};
#endif
#include "CRidable.h"
#include "CTournamentDungeonReward.h"
#include "HellPartyGroupOrder.h"  // STHellPartyGroupOrder/STHellPartyGroupRate
#include "WarField.h"          // map_item/map_monster/MapInfo/stMapMonsterKillChecker_t/stMapPlayInfo_t（§7 单声明点）

class CWorldMap;               // 指针成员，前向声明即可（CParty.h/CDataManager.h 亦有）

// ================= 共享数据结构（ORIG 构造体实证） =================
// map_monster / map_item / MapInfo 权威定义见 WarField.h（§7 单声明点）；
// 本地重复定义已删除，经 #include "WarField.h" 获取。

// ---- _mapMonster / _mapItem / TournamentMonster / STAssignBloodSystem /
//      STAssignBloodPhaseTime 权威定义见 CMap.h（唯一声明点，经 #include "CMap.h"）。
//      历史本地重复定义已删除（曾与 CMap.h 同名类型构成 ODR 冲突）。

// ---- map_item（0x54 字节；ctor 081512cc；权威定义见 WarField.h） ----

// ---- MapInfo（0xec 字节；ctor 08151394；权威定义见 WarField.h） ----

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
        void Clear();                           // ORIG 0830ea1e
        bool operator<(const TournamentInfo& other) const;  // ORIG 0830eaa6
    };

    // [prett-A 2026-09-11] ORIG prett 串 @0x08c23960 size 0x6a 第 3 参写作
    //   `CBattle_Field::TournamentRoundInfoMap&`：类内 typedef ⇒ 宏打印全限定名
    //   （GCC 4.4 探针 /tmp/r2/probe2.cpp 实证）。纯增量：不改既有声明、mangled 不变、零 codegen。
    typedef std::map<short, std::vector<TournamentInfo> > TournamentRoundInfoMap;

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
        bool m_isBloodClear;              // +0x14
        unsigned int m_rewardExp[4];      // +0x18（P211：与 setRewardExp 形参 uint
                                          // 同型化翻 store 序 LHS-first，ORIG
                                          // 0x085bf14a this 先载实证；宽度/偏移不变）
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
        unsigned char isBloodClear();              // ORIG 085bf21c
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
        int m_monsterIdx;            // +0x0c（ORIG 元素 0x18：push_back lea 0x18；
                                     //   SetGridPath 082ffb30 写 script m_monsterIdx(+4)）
        int m_field10;               // +0x10（script m_field0c）
        int m_field14;               // +0x14（script m_field10）
        // [b28d-Main 代落·S324 P299/P317] 删用户 ctor/dtor 声明：
        // ORIG 0830ee4c / 08316adc 均为 **W**（隐式发射）——ctor 仅 `call pair<int,int>::C1Ev`
        // （隐式默认构造 m_pos），dtor 由 new_allocator::destroy 的 p->~T() 发射。
        // 我方原空 ctor 既漏构 m_pos，又令局部/元素多发 D1Ev（SetGridPath EXTRA ×4）。
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
                       ENUM_DUNGEON_TYPE type, int memberCount,
                       CWorldMap* worldMap, bool isHellParty,
                       int questStageMap);   // 082ff486（ORIG asm 7 栈/寄存参实证）
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
                           TournamentRoundInfoMap& roundMap);  // 083035c8
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
    unsigned char IsAssignNPCByPotion();                               // 0822cee2
    static bool IsEnableHellDungeon();                               // 0830ed07（b28d-Main: P292 更正版，5 调用点 `call; test %al,%al; je` 无 setne ⇒ bool）
    // [E05 R2] P292 勘误/P321 T3：ORIG 定义 0x08307656 体 `movzbl 0x5c(%eax),%eax; ret`
    // （无 setne），消费端 CParty::finish_loading 0x085b16dc `call; test %al,%al; je`
    // （bool 直判无 setne）⇒ 返回 bool + 字段 bool（`return m_boolField` 不产 setne）。
    bool IsFirstEnterDungeon();                               // 08307656
    // [R19a05 ④ 2026-09-13] uchar→bool：ORIG 该符号全部 2 个调用点均证 bool ——
    //   ① 0x085a842e（CParty::checkMoveMapHackCnt）后 `xor $0x1,%eax; test %al,%al;
    //      je`（P292 bool 取反形，uchar 会多 setne）；② 0x085a3e28（CParty::get_item）
    //   后 `test %al,%al; je` 直判（无 setne）。定义体 `return m_visited;`（字段
    //   WarField.h m_visited 已为 bool）⇒ ORIG 0x085bf2e8 单条 `movzbl` 直返。
    bool IsVisitedCurrentMap();                               // 085bf2e8
    bool SetDamagedPacketCnt(int uid);                        // 08308f4e
    int check_movemap_range(int x, int y);                    // 08307b2a
    bool isWorldMapTowerArea();                                      // 085bf31c（P9/P22：bool 字段+bool 返回=ORIG movzbl 直返单条；调用点 xor 物化需 bool 返回）
    void set_visited_map_goto(int x, int y);                  // 08308a24
    void setEplpWaitingTimer(QuickParty::RandomBuffDungeon type);  // 0830ac7a
    void set_monster_ability();                               // 083026da
    void AddTournamentMonster(const TournamentInfo& info, std::list<_mapMonster>& mobList,
                              bool isUltimate);              // 08304718
    bool DestroyPassiveObject(int idx);                       // 082ff3ec
    static bool* GetEnableHellDungeon();                             // 0822cf26（bool 链：被指全局字节为 bool，使 IsEnableHellDungeon 体保持 movzbl 直返）
    void InsertTournamentInfo(std::vector<int>& indexes,
                              std::vector<TournamentInfo>& infos,
                              std::map<short, TournamentInfo>& roundMap);  // 08303e02
    void SetAssignNPCByPotion(bool flag);                     // 085d19d6
    static void SetEnableHellDungeon(bool flag);                     // 0822cf30
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
    bool SetRidableObjectState(int idx, eRidableObjectState state);  // 085bf228（转发 m_ridable 返回值）
    void SetTournamentCurRound(int round);                    // 082fe69c
    int getLayeredMapKeyValue(int idx);                       // 0830df56
    char getUltimateDifficylty();                             // 0822d02e
    void setUltimateDifficulty(char diff);                    // 0822d012
    void BloodSpawnTimerManager(bool a, bool b);              // 08304dd2
    void BloodMonsterSpawnPacket(int cnt, int time, short smallRound,
                                 short nextRound);   // 08305992（ORIG Eiiss：int,int,short,short）
    void BloodRoundMaxTimePacket(short round, int a, int b);  // 08305dc4
    void BloodRoundIntervalPacket(short round, int interval); // 08305e9c
    void ConsistTournamentRound(std::vector<TournamentInfo>& first,
                                std::vector<TournamentInfo>& second,
                                std::map<short, std::vector<TournamentInfo> >& roundMap,
                                std::list<_mapMonster>& mobList);  // 08303ace
    int GetHellPartyDifficulty(ENUM_DUNGEON_TYPE type);       // 082ffa2e
    bool IsKilledAllHellGruoups();                            // 085bf250（b28d-Main: P292 判别，ORIG 调用点 0x085a8377 `call; xor $0x1,%eax; test %al,%al; je`）
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
    // [W13E 2026-09-11] ORIG 返回类型为 const（prett 串直证 ORIG 侧
    // `const CMap* CBattle_Field::getMap(int)` vs 我方无 const）⇒ 补 const。
    // mangled 不含返回类型 ⇒ ABI/mangled 零变化；机器码零变化（仅 .rodata
    // prett 串）；唯一调用点 BattleDepsB2.cpp:88 加 const_cast（零指令）。
    const CMap* getMap(int unused);                           // 08300ed4
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
    // +0x0c..+0x1b = 4 个标量、+0x1c/+0x3c = 两组 pair[4]：
    // ①ORIG C1 0x082a44fe 只对 +0x1c/+0x3c 两组各 4 个元素发
    //   _ZNSt4pairIiiEC1Ev（+0xc..+0x1b 无构造调用）⇒ 原码前 4 个不是 pair；
    // ②ORIG getCurPosXY 08300da6 / getLastPosXY 08300e44 / swap_curpos_xy
    //   0830878e 的索引段 `add $0x2/$0x6,%edx` + `disp 0xc` 与
    //   base+0x1c/0x3c + 索引 seat 地址等价（this+0xc+(seat+2)*8 =
    //   this+0x1c+seat*8）。sizeof/偏移不变（4*4+2*4*8 = 0x50）。
    int m_endX;                            // +0x0c（原 m_pos[0].first；check_end_point 终点 X）
    int m_endY;                            // +0x10（原 m_pos[0].second）
    int m_field14;                         // +0x14（原 m_pos[1].first，仅 ctor 置 -1/reset 置 0）
    int m_field18;                         // +0x18（原 m_pos[1].second）
    std::pair<int, int> m_curPos[4];       // +0x1c（原 m_pos[2..5]，seat 当前位置）
    std::pair<int, int> m_lastPos[4];      // +0x3c（原 m_pos[6..9]，seat 上次位置）
    bool m_firstEnterDungeon;              // +0x5c（[E05 R2] P321 T3：bool，同上）
    char m_pad5d[3];                       // +0x5d
    int m_curX;                            // +0x60
    int m_curY;                            // +0x64
    int m_lastX;                           // +0x68
    int m_lastY;                           // +0x6c
    int m_mapMobCount;                     // +0x70（SelectDungeon 082ff48e 自
                                           //   CDungeon::m_6e0[diff] 复制的每图怪物基数；
                                           //   getAddChampionMobCnt 按难度 ×150/250/500%
                                           //   缩放——ORIG 算术实证为 int，非指针）
    MazeScript* m_gridMaze;                // +0x74
    std::map<int, MapInfo> m_mapInfo;      // +0x78
    Mutex m_mutexField;                    // +0x90
    Mutex m_mutexEvent;                    // +0xa8
    std::vector<std::pair<int, int> > m_dummyMapPos;  // +0xc0
    // [b28d-Lane27] 类型旋钮 unsigned int→int（Main 批准）：ORIG choose_random_bossmap_
    // dummymaps 0x08307f24 `mov 0xcc(%eax),%eax; mov %eax,%ebx`（两段 mov 中转）+ 随后
    // `cmp %eax,%ebx; setb`（无符号比较）= 字段 int 与 `size()`(size_t) 异型触发转换中转
    // （P211/P248 档③）；unsigned 同型 ⇒ 4.4 单段直落丢中转。同宽 4B ⇒ sizeof/偏移零变化。
    int m_dummyMapPosIdx;                  // +0xcc
    CRidable m_ridable;                    // +0xd0
    CClearCondition m_clearCondition;      // +0xf0
    CMap* m_pMap;                          // +0x108
    int m_layeredMapIndex;                 // +0x10c
    bool m_questMaze;                      // +0x110（ORIG setQuestMaze/isQuestMaze 按布尔字节读写；char→bool 不改布局）
    char m_pad111[3];                      // +0x111
    int m_curSeatNo;                       // +0x114（SetCurSeatNo 082a43cc / getCurPosXY 实证）
    std::vector<int> m_apcList;            // +0x118
    int m_apcAllKilledSec;                 // +0x124（APC 全灭时刻兼超时 timer key：
                                           //   Dispatchers APC 分支写 getCurSec()；
                                           //   CParty/OnTimeoutBossDie.md CParty+0xc48==key；
                                           //   reset_field 0x082f? 清 0）
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
    // [b28d-solo-BattleDungeonMapA2] ORIG SelectDungeon 0x082ff51d 为 LHS-first
    // `mov this,%eax; mov type,%edx; mov %edx,0x1b0(%eax)`（P211 档③ 同型）⇒
    // enum 视角赋值（探针 f1/f2/f3：字段 int + enum RHS 恒 RHS-first，cast 无效）；
    // int 视角保留给全仓 reset/比较消费点（CBattle_Field_missing.cpp:155、
    // CParty.cpp:1326、CPartyOnlyOrig.cpp:996）。sizeof/偏移/mangled 零变化。
    union {
        int m_hellPartyMode;                  // +0x1b0（int 视角）
        ENUM_DUNGEON_TYPE m_hellPartyModeE;   // +0x1b0（enum 视角）
    };
    // b28d-solo-CBossTower P235/P254：ORIG CBossTower::handleStartGame 的
    // EnterDungeon 实参装载为两段 `mov 0xcd8(%eax),%eax; mov %eax,%esi`
    // （enum→int 转换），比较/字节截断等消费点同形 ⇒ 匿名 union 双视角。
    // sizeof/偏移/mangled 零变化。
    union {
        int m_bloodState;                  // +0x1b4（int 视角）
        ENUM_BLOOD_STATE m_bloodStateE;    // +0x1b4（enum 视角）
    };
    char m_hellPartyActive;                // +0x1b8
    char m_pad1b9[3];                      // +0x1b9
    int m_hellMapIdx;                      // +0x1bc
    int m_hellPosX;                        // +0x1c0
    int m_hellPosY;                        // +0x1c4
    bool m_visitedGotoFlag;                // +0x1c8（b28d-solo-W8_CU_A: char→bool，
                                           //   P248 档②：ORIG GetItemSerialChecker
                                           //   0x082072c `movzbl 0xcec(%eax); xor $1;
                                           //   test %al,%al; jne` = bool 字段 + 裸
                                           //   `!field`；char 恒产 test/je 无 xor。
                                           //   写点仅 0/1 常量赋值，语义不变）
    char m_sealDoorItemsOk;                // +0x1c9（唯一写点 UseSealDoorItems @0x085a8038：全员通过=1/失败=0）
    bool m_killedAllHellGroups;            // +0x1ca（b28d-Main: bool 直载 movzbl 复现 ORIG 体 0x085bf256）
    char m_field1cb;                       // +0x1cb
    char m_hellPartyDifficulty;            // +0x1cc
    char m_hellPartyEventType;             // +0x1cd
    // [b28d-solo-BattleDungeonMapA2] ORIG SelectDungeon 0x082ff61b 为 LHS-first
    // `mov this; movzbl bool形参槽; mov %dl,0x1ce`（P211 同型 + P248 档②）⇒
    // 字段 bool（char 与 bool 形参异型时 4.4 恒 RHS-first）。1 字节/偏移不变。
    bool m_hellPartyRandom;                // +0x1ce
    char m_hellPartyDropFlag;              // +0x1cf
    int m_hellItemStatistic[6];            // +0x1d0
    // +0x1e8/+0x1ec/+0x1f4 实为 int dword：reset_hell_party_value.md 0x0830a595/
    // 0x0830a5ae/0x0830a5c7 以整数 mov 拷贝 script[0]/[+4]/[+0xc]；消费方均
    // fildl/int 运算（send_hellParty_value +0xd0c 分母、sub_hellparty_value
    // +0xd10 下限、cal_hellparty_value +0xd18 系数与 +0xd0c 上限、clear_reward
    // +0x2ab3）。+0x1f0 无 ORIG 读写证据，保留 float（2026-09-03 修正）。
    int m_hellPartyRatio0;                 // +0x1e8（int：script[0] 分母/上限）
    int m_hellPartyRatio1;                 // +0x1ec（int：script[1] 最小下限）
    float m_hellPartyRatio2;               // +0x1f0（无证据，保留 float）
    int m_hellPartyRatio3;                 // +0x1f4（int：script[3] 乘法系数）
    float m_hellPartyRatio4;               // +0x1f8
    float m_hellPartyRatio5;               // +0x1fc
    float m_hellPartyDungeonRate[4];       // +0x200
    float m_hellPartyMapRate[5];           // +0x210
    float m_hellPartyItemRate[4];          // +0x224
    float m_hellPartyScriptRate;           // +0x234
    int m_standardDimensionLevel;        // +0x238（B5 二名归一：ORIG CParty::setStandardDimensionLevel @0x0859f6c6 写 0xd5c=CParty 内嵌 BF+0x238；BF ctor 0x082a477c movl $0,0x238 归属；hell 消费点系 (char) 低字节读同槽）
    bool m_worldMapTowerArea;              // +0x23c
    char m_pad23d[3];                      // +0x23d
    std::vector<STAssignBloodSystem> m_bloodSystems;        // +0x240
    std::vector<STAssignBloodPhaseTime> m_bloodPhaseTimes;  // +0x24c
    std::vector<short> m_bloodPhaseOrder;  // +0x258
    std::vector<bool> m_bloodPhaseClear;   // +0x264（vector<bool> 0x14 字节）
    short m_bloodSpawnCnt[11];             // +0x278（ORIG BloodPhaseSpawnTimerResgister
                                           //   movw 写入实证为 short[11]，尺寸 0x16 不变）
    short m_bloodSpawnTime[11];            // +0x28e（同上；TimerDispatcher 血轮 Register
                                           //   族按 short 读取）
    int m_bloodSpawnCntCur;                // +0x2a4
    short m_bloodCrazyRound;               // +0x2a8
    char m_bloodCrazyType;                 // +0x2aa
    char m_ultimateDifficulty;             // +0x2ab
    CBloodRound m_bloodRound;              // +0x2ac
    ENUM_BLOOD_STATE m_bloodState2;        // +0x2b4（类型修正：ORIG setBloodState 载入顺序实证）
    CBloodClearRewardData m_bloodReward;   // +0x2b8
    CTowerOfDespairRewardData m_todReward; // +0xc84
    CTournamentDungeonReward m_tournamentReward;  // +0xc98
};

static_assert(sizeof(CBattle_Field) == 0xcd0, "CBattle_Field size");
// [b28d-Lane27] 三处同宽类型旋钮的零布局佐证（uchar→bool / uint→int / char→bool）：
static_assert(sizeof(CDungeon) == 0x934, "CDungeon size after bool knobs");
static_assert(__builtin_offsetof(CDungeon, m_dummyAppearCount) == 0x628,
              "CDungeon +0x628 drift after m_hasDummyMap bool");
static_assert(__builtin_offsetof(CBattle_Field, m_ridable) == 0xd0,
              "CBattle_Field +0xd0 drift after m_dummyMapPosIdx int");
static_assert(__builtin_offsetof(MapInfo, m_playInfo) == 0x7c,
              "MapInfo +0x7c drift after m_visited bool");
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
static_assert(sizeof(CBattle_Field::stEventMonsterStatus_t) == 0x18,
              "stEventMonsterStatus_t size");
static_assert(sizeof(CBattle_Field::TournamentInfo) == 0x18, "TournamentInfo size");
static_assert(sizeof(CBattle_Field::CBloodRound) == 8, "CBloodRound size");

#endif  // DNF_GAME_CBATTLE_FIELD_H_
