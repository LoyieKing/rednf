// df_game_r CDungeonNode 还原（G2 副本链）。
// 布局依据：ORIG CDungeonNode ctor(08486ae6)/dtor(08486d3c)/copy ctor(08487a86)、
// InsertDB(08485ba4)/InsertMonsterKey/InsertSkillKey/Update*。
// sizeof(CDungeonNode) = 0x90；Monster = 0xc；CDungeonJobNode = 0x18；
// CDungeonSuccessNode = 0x30；DungeonKey = 8 字节。
#pragma once

#include <map>

struct DungeonKey
{
    int m_dungeonIndex;   // +0x00
    int m_dungeonDiff;    // +0x04
};

struct SIG_DUNGEON_DATA_STATISTIC;

class Monster
{
public:
    Monster();
    ~Monster();
    void Reset();
    Monster& operator+=(const Monster& other);

    int m_damageTotal;   // +0x00
    int m_damageCount;   // +0x04
    int m_count;         // +0x08
};

class CDungeonJobNode
{
public:
    CDungeonJobNode();
    ~CDungeonJobNode();
    void InsertDB(const DungeonKey& key, int job);
    void InsertSkillKey(int skillIndex);
    void UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat);
    char m_pad[0x18];
};

class CDungeonSuccessNode
{
public:
    CDungeonSuccessNode();
    ~CDungeonSuccessNode();
    void InsertDB(const DungeonKey& key, bool flag);
    void UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat);
    char m_pad[0x30];
};

class CDungeonNode
{
public:
    CDungeonNode();
    CDungeonNode(const CDungeonNode& other);
    ~CDungeonNode();

    bool InsertDB(const DungeonKey& key);
    bool InsertMonsterKey(int monsterIndex);
    bool InsertSkillKey(int job, int skillIndex, int type, int flag);
    bool UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat);
    bool UpdateMonster(SIG_DUNGEON_DATA_STATISTIC* stat);
    bool UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat);

    std::map<int, Monster> m_monsterMap;             // +0x00
    std::map<int, CDungeonJobNode> m_jobMap;         // +0x18
    CDungeonSuccessNode m_success[2];                // +0x30
};

class MySQL;
MySQL* GetMySqLInstance();

