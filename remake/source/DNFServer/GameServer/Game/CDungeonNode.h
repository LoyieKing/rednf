// df_game_r CDungeonNode 族还原（CDungeonNode 批次独占）。
// 布局依据：ORIG ctor(0x08486ae6)/dtor(0x08486d3c)/copy(0x08487a86)、
// InsertDB(0x08485ba4)/InsertMonsterKey(0x084852de)/InsertSkillKey(0x08485404)/
// Update*(0x08485564/0x08485628/0x0848559a) + CDungeonJobNode/CDungeonSuccessNode/
// CSuccessJobNode 报告（docs/class_func_reports/ 三目录）+ objdump 兜底。
// sizeof：Monster=0x0c、Skill=0x10、JobParty=0x04、CDungeonJobNode=0x18（map<int,Skill>）、
// CDungeonSuccessNode=0x30（map<int,Party>+map<JobKey,CSuccessJobNode>）、
// CSuccessJobNode=0x58（map<int,JobParty>+PlayInfo）、
// CDungeonNode=0x90（0x18+0x18+0x30*2）。DungeonKey=12 字节（3 int，
// CDungeonDataManager 迭代器 add $0xc 取 CDungeonNode 实证，OrigDungDataMgr 证据）。
// 返回类型：InsertDB/Update* 类 ORIG Ghidra 为 undefined4（mov $0/$1 直返）→ 用 int；
// InsertMonsterKey/InsertSkillKey/UpdateMonster/UpdateSkill 用 bool（return true/false 常量）。
#pragma once

#include <map>

#include "IData.h"
#include "Skill.h"
#include "JobParty.h"
#include "JobKey.h"
#include "Party.h"           // map<int,Party> 值类型
#include "CSuccessJobNode.h" // map<JobKey,CSuccessJobNode> 值类型

struct DungeonKey
{
    int m_dungeonIndex;   // +0x00
    int m_dungeonDiff;    // +0x04
    int m_field8;         // +0x08（ORIG 第三字段；sizeof 0xc。不参与 InsertDB 传参与 operator<）

    ~DungeonKey() {}   // ORIG D1/D2 W 0x084868b2：空体用户析构（P67 保留，勿删转 trivial）

    bool operator<(const DungeonKey& other) const   // ORIG W 0x0848686c：
    {                                               // 先比 +4 diff、相等再比 +0 index（P56）
        if (m_dungeonDiff < other.m_dungeonDiff)
            return true;
        if (m_dungeonDiff != other.m_dungeonDiff)
            return false;
        if (m_dungeonIndex < other.m_dungeonIndex)
            return true;
        return false;
    }
};

struct SIG_DUNGEON_DATA_STATISTIC;

// ---- Monster：CDungeonNode::m_monsterMap 值类型（0x0c，IData EBO）----
// ORIG W：ctor 0x08486978 = IData::C1 + Reset；Reset 0x08486956 清 3 int；
// operator+= 0x08486996（+0/+4 相加、+8 自增）；dtor 0x084869d0（trivial）。
class Monster : public IData
{
public:
    Monster() : IData()     // W 0x08486978
    {
        Reset();
    }
    ~Monster() {}           // W 0x084869d0

    void Reset()            // W 0x08486956
    {
        m_damageTotal = 0;
        m_damageCount = 0;
        m_count = 0;
    }
    void operator+=(const Monster& other)   // W 0x08486996：尾无 this-reload⇒void（Skill/PlayInfo 同族定案）
    {
        m_damageTotal += other.m_damageTotal;
        m_damageCount += other.m_damageCount;
        ++m_count;
    }

    int m_damageTotal;   // +0x00
    int m_damageCount;   // +0x04
    int m_count;         // +0x08
};

// ---- CDungeonJobNode：CDungeonNode::m_jobMap 值类型（0x18 = map<int,Skill>）----
// ORIG W：ctor 0x08486ce8（map C1）、copy 0x084878d0、dtor 0x08486cfc（map D1）；
// T：InsertSkillKey 0x08484a86 / UpdateSkill 0x08484bac / InsertDB 0x08485e00。
class CDungeonJobNode
{
public:
    CDungeonJobNode() : m_skillMap() {}              // W 0x08486ce8
    CDungeonJobNode(const CDungeonJobNode& other)
        : m_skillMap(other.m_skillMap) {}            // W 0x084878d0
    ~CDungeonJobNode() {}                            // W 0x08486cfc

    bool InsertSkillKey(int skillIndex);             // T 0x08484a86
    bool UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat);  // T 0x08484bac
    int InsertDB(const DungeonKey& key, int job);    // T 0x08485e00

    std::map<int, Skill> m_skillMap;                 // +0x00（0x18）
};

// ---- CDungeonSuccessNode：CDungeonNode::m_success 元素（0x30）----
// ORIG W：copy 0x08487a30、dtor 0x08486aa0；T：ctor 0x08484dda /
// InsertDB 0x08485f8a / UpdateCharacter 0x084851de。
class CDungeonSuccessNode
{
public:
    CDungeonSuccessNode();                                        // T 0x08484dda
    CDungeonSuccessNode(const CDungeonSuccessNode& other)         // W 0x08487a30
        : m_partyMap(other.m_partyMap), m_jobMap(other.m_jobMap)
    {
    }
    ~CDungeonSuccessNode() {}                                     // W 0x08486aa0

    int InsertDB(const DungeonKey& key, bool flag);               // T 0x08485f8a
    int UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat);        // T 0x084851de

    std::map<int, Party> m_partyMap;                              // +0x00（0x18）
    std::map<JobKey, CSuccessJobNode> m_jobMap;                   // +0x18（0x18）
};

// ---- CDungeonNode（0x90）----
// ORIG W：ctor 0x08486ae6、copy 0x08487a86、dtor 0x08486d3c；
// T：InsertDB 0x08485ba4 / InsertMonsterKey 0x084852de /
// InsertSkillKey 0x08485404 / UpdateCharacter 0x08485564 /
// UpdateMonster 0x08485628 / UpdateSkill 0x0848559a。
class CDungeonNode
{
public:
    CDungeonNode()                                // W 0x08486ae6
        : m_monsterMap(), m_jobMap(), m_success()
    {
    }
    // W 0x08487a86：ORIG 为隐式拷贝构造（m_success 数组逐元素 C1ERKS_），
    // 显式赋值会生成 C1Ev+aSERKS_，偏离 ORIG —— 保持隐式。
    ~CDungeonNode() {}                            // W 0x08486d3c

    int InsertDB(const DungeonKey& key);          // T 0x08485ba4
    bool InsertMonsterKey(int monsterIndex);      // T 0x084852de
    bool InsertSkillKey(int skillIndex, int job, int type, int flag);  // T 0x08485404
    bool UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat);   // T 0x08485564
    bool UpdateMonster(SIG_DUNGEON_DATA_STATISTIC* stat);     // T 0x08485628
    bool UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat);       // T 0x0848559a

    std::map<int, Monster> m_monsterMap;         // +0x00（0x18）
    std::map<int, CDungeonJobNode> m_jobMap;     // +0x18（0x18）
    CDungeonSuccessNode m_success[2];            // +0x30（0x30*2）
};

class MySQL;
MySQL* GetMySqLInstance();
