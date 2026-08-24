// df_game_r CMap / STMapScript 还原（G2 副本链）。
// 布局依据：ORIG CMap ctor(0834dcda)/dtor(0834def6)/set_map(0834e18a)、
// STMapScript ctor(0836cc94)/dtor(0836d4bc)、各容器函数元素尺寸。
// CMap sizeof = 0x130；STMapScript sizeof = 0x37c。
#pragma once

#include <cstring>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "advancealtar.h"   // advancealtar::TimeLine（STMapScript 成员）

class CDataManager;
class CMap;

int get_rand_int(int range);
int isInPathArea(int x, int y, const int (*area1)[2], const int (*area2)[2]);
CDataManager* G_CDataManager();



// ---- CMap 支撑类型（尺寸来自 ORIG 容器函数 / 构造体）----

struct _mapItem
{
    int m_objectIdx;   // +0x00（ctor 不初始化）
    int m_field04;     // +0x04（ctor=0）
    int m_field08;     // +0x08（ctor=0）
    int m_field0c;     // +0x0c（ctor=0）

    _mapItem();   // ORIG 0815088e（唯一定义见 CMap.cpp；无 dtor 符号——平凡析构）
};

struct _mapMonster
{
    int m_mobId;              // +0x00  (monster idx, ctor=-1)
    char m_flag04;            // +0x04  (ctor=1)
    int m_field08;            // +0x08
    int m_spawnType;          // +0x0c  (0=单人 1=队伍 2=队伍x2)
    int m_levelAdd;           // +0x10
    int m_field14;            // +0x14
    int m_roleType;           // +0x18
    char m_champion;          // +0x1c
    int m_posX;               // +0x20
    int m_level;              // +0x24  (level = STAssignMonster.m_14/10；ORIG insert_monster 实证)
    int m_hpPct;              // +0x28  (ctor=100)
    int m_orderIdx;           // +0x2c
    char m_flag30;            // +0x30
    char m_pad31[3];          // +0x31
    std::vector<int> m_mobList; // +0x34

    _mapMonster();                        // ORIG 0830e4fc
    _mapMonster(const _mapMonster& other); // ORIG 0813d708
    ~_mapMonster();                       // ORIG 081398c6（仅析构 +0x34 vector）
};                                     // 唯一定义均见 CMap.cpp

class STAssignMonster
{
public:
    STAssignMonster();
    STAssignMonster(const STAssignMonster& other);
    ~STAssignMonster();
    STAssignMonster& operator=(const STAssignMonster& other);
    void Clear();

    int m_0;                        // +0x00  (ctor=-1)
    char m_4;                       // +0x04  (ctor=1)
    int m_8;                        // +0x08
    int m_c;                        // +0x0c
    int m_10;                       // +0x10
    int m_14;                       // +0x14  (level)
    int m_18;                       // +0x18  (ctor=1)
    int m_1c;                       // +0x1c  (ctor=1)
    int m_20;                       // +0x20
    int m_24;                       // +0x24
    std::vector<int> m_vec28;       // +0x28
    int m_34;                       // +0x34
    int m_38;                       // +0x38
    int m_3c;                       // +0x3c
    std::vector<std::pair<int, int> > m_vec40; // +0x40
    int m_4c;                       // +0x4c  (ctor=100)
    int m_50;                       // +0x50
    char m_54;                      // +0x54
    char m_55;                      // +0x55
    int m_58;                       // +0x58  (ctor=-1)
    std::vector<unsigned char> m_vec5c; // +0x5c
};

struct STAICharacterMapArrangeData
{
    int m_type;        // +0x00
    char m_pad4[0xc];  // +0x04
    int m_level;       // +0x10
    int m_roleType;    // +0x14
    int m_field18;     // +0x18
    char m_unk19;      // +0x19
    char m_pad1a[0x1e];
};  // 0x38 字节

struct STRivalMapData
{
    char m_pad[0xc];
};

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

struct AssignNpc
{
    char m_pad[0x1c];
};

struct SPOBJ_ACTION_DATAS
{
    int m_type;    // +0x00
    int m_4;       // +0x04
    int m_8;       // +0x08
    int m_c;       // +0x0c
    int m_10;      // +0x10
    int m_14;      // +0x14
};

struct STAssignSpecialPassiveObj
{
    char m_pad0[0x10];
    std::vector<SPOBJ_ACTION_DATAS> m_vec10; // +0x10
    int m_1c;                                // +0x1c
};

struct STAssignPassiveObject
{
    int m_0;
};

struct TournamentMonster
{
    int m_mobId;   // +0x00
    int m_rate;    // +0x04

    TournamentMonster();  // ORIG 0830e4d4（定义见 TournamentMonster.cpp）
    ~TournamentMonster();
    void Clear();         // ORIG 0830e4bc
};

// ---- STMapScript（sizeof 0x37c）----
struct STMapScript
{
    STMapScript();
    STMapScript(const STMapScript& other);
    ~STMapScript();
    void Clear();

    int m_0;                  // +0x00
    std::list<int> m_list4;   // +0x04
    int m_c;                  // +0x0c
    std::vector<std::pair<int, int> > m_vec10; // +0x10
    char m_pad1c[0x2c - 0x1c]; // +0x1c
    std::list<STAssignPassiveObject> m_list2c; // +0x2c
    std::vector<STAssignMonster> m_vec34;      // +0x34
    std::vector<STAssignMonster> m_vec40;      // +0x40
    int m_4c;                 // +0x4c
    char m_pad50[0x58 - 0x50]; // +0x50
    std::list<STAssignSpecialPassiveObj> m_list58; // +0x58
    std::vector<AssignNpc> m_vec60;            // +0x60
    char m_pad6c[0x100 - 0x6c];                // +0x6c
    int m_pathArea1[8];       // +0x100
    int m_pathArea2[8];       // +0x120
    char m_pad140[0x148 - 0x140];              // +0x140
    int m_148;                 // +0x148（load_map_files 0x835fe02：>0 时对每张注册地图调 CDungeon::insertStart）
    char m_pad14c[0x178 - 0x14c];              // +0x14c
    int m_178;                // +0x178  (set_map 读取 → CMap::m_110)
    char m_pad17c[0x250 - 0x17c];              // +0x17c
    std::vector<STRivalMapData> m_vec250;      // +0x250
    std::vector<STAICharacterMapArrangeData> m_vec25c; // +0x25c
    int m_268;                // +0x268
    char m_pad26c[0x278 - 0x26c];              // +0x26c
    std::vector<STRivalMapData> m_vec278;      // +0x278
    int m_284;                // +0x284
    std::vector<STAssignBloodSystem> m_vec288; // +0x288
    std::vector<STAssignBloodPhaseTime> m_vec294; // +0x294
    char m_pad2a0[0x2b8 - 0x2a0];              // +0x2a0
    std::map<char, std::vector<TournamentMonster> > m_map2b8; // +0x2b8
    char m_pad2d0[0x324 - 0x2d0];              // +0x2d0
    int m_324;                // +0x324
    int m_328;                // +0x328
    char m_pad32c[0x330 - 0x32c];              // +0x32c
    advancealtar::TimeLine m_timeLine330;      // +0x330
    std::string m_str34c;     // +0x34c
    char m_pad350[0x36e - 0x350];              // +0x350
    char m_36e;               // +0x36e
    std::string m_str370;     // +0x370
    char m_pad374[0x37c - 0x374];              // +0x374
};

// ---- CMap（sizeof 0x130，无虚表）----
class CMap
{
public:
    CMap();
    ~CMap();
    void destroy();
    void destory();
    int set_map(STMapScript script);
    int insert_monster(STAssignMonster monster, int area);
    void insert_condition_monster(STAssignMonster monster);
    int insert_item(int idx, int level, int prob, int cond);
    int checkFitInPathObject(int x, int y) const;
    int box_item_index(std::list<std::pair<int, int> > itemList) const;
    int CheckEntrance(int flag) const;

    int get_index() const { return m_4; }
    int get_map_type() const { return m_8; }
    std::list<_mapMonster> get_monster_list() const { return m_monsterList; }
    std::list<_mapMonster> get_condition_monster_list() const
    {
        return m_conditionMonsterList;
    }
    std::list<_mapItem> get_item_list() const { return m_itemList; }
    int GetBossCount() const { return m_bossCount; }
    int getAssignedNPCCount() const { return m_assignedNpcCount; }
    const std::vector<STAICharacterMapArrangeData>* getAICharacters() const
    {
        return &m_aiCharacters;
    }
    int getAPCRandomPoint() const { return m_78; }
    std::list<int>* getDungeonLists() const
    {
        return const_cast<std::list<int>*>(&m_pathObjectList);
    }

    int m_bossCount;                // +0x00  (monster type==3 时累加)
    int m_4;                        // +0x04  (map index)
    int m_8;                        // +0x08  (map type)
    std::list<_mapMonster> m_monsterList;       // +0x0c
    std::list<_mapMonster> m_conditionMonsterList; // +0x14
    std::list<_mapItem> m_itemList; // +0x1c
    std::list<int> m_pathObjectList; // +0x24
    int m_pathArea1[8];             // +0x2c
    int m_pathArea2[8];             // +0x4c
    std::vector<STAICharacterMapArrangeData> m_aiCharacters; // +0x6c
    int m_78;                       // +0x78  (APC random point)
    int m_assignedNpcCount;         // +0x7c
    int m_80;                       // +0x80
    std::vector<std::pair<int, int> > m_entranceVec; // +0x84
    std::vector<STRivalMapData> m_rivalData1;  // +0x90
    std::vector<STRivalMapData> m_rivalData2;  // +0x9c
    std::vector<int> m_passiveObjectList;      // +0xa8
    int m_b4;                       // +0xb4
    std::vector<STAssignBloodSystem> m_bloodSystem; // +0xb8
    std::vector<STAssignBloodPhaseTime> m_bloodPhaseTime; // +0xc4
    char m_padd0[0xec - 0xd0];      // +0xd0
    int m_ec;                       // +0xec
    int m_f0;                       // +0xf0
    advancealtar::TimeLine m_timeLine; // +0xf4
    int m_110;                      // +0x110
    std::map<char, std::vector<TournamentMonster> > m_tournamentMap; // +0x114
    char m_12c;                     // +0x12c
};
