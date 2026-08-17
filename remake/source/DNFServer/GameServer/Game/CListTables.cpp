// df_game_r 列表/脚本表类还原（G2 第七批）。
// CSkillList / QuestList / CDungeonList / CMapList / CMonsterManager。
// 布局以 ORIG 构造/析构汇编为准；函数形态对照 class_func_reports/
// （AE 口径：调用目标/全局地址归一；字段/栈偏移/常量严格一致）。
// ST*Script / CSkill / CMonster / CDungeon / CMap / Quest 等实现在各自 TU，
// 本文件只声明必要接口并调用。

#include "CDataManager.h"

#include <ext/hash_map>
#include <stdio.h>
#include <string.h>
#include <utility>

#include "GameTypes.h"
#include "GlobalData.h"
#include "CSkill.h"

// ===================== 外部依赖声明（其它 TU 提供） =====================

class STSkillScript;
class STMonsterScript;
class QuestScript;

namespace sync_script
{
class CSyncScript;
}

namespace sync_script
{
class CSyncScript
{
public:
    bool truncate_monster_info_table();
    bool insert_monster_info_to_db(const STMonsterScript& script);
    bool truncate_skill_index();
    bool trancate_skill_info();
    bool insert_skill_index(STSkillScript* script, int job);
    bool IsEnable();
    bool insert_skill_info(STSkillScript* script, int job, int type);
    bool truncate_quest_category();
    bool insert_quest_category(QuestScript* script);
};

CSyncScript* GetInstanceSyncScript();
extern std::map<int, std::string> g_dungeonInfoTable;  // ORIG 0x94f6ea0
extern std::map<int, std::string> g_monsterInfoTable;  // ORIG 0x94ff260
extern std::map<int, std::string> g_skillInfoTable[11]; // ORIG 0x9500820
extern std::map<int, std::string> g_questInfoTable;     // ORIG 0x9500480
}

class CMonster
{
public:
    CMonster();
    ~CMonster();
    int get_index() const;
    void set_monster(class STMonsterScript& script);
    char m_pad[0x1d28];
};

class CDungeon
{
public:
    CDungeon();
    ~CDungeon();
    int get_index() const;
    void set_dungeon(class STDungeonScript& script);
    char m_pad[0x934];
};

class CDungeon_TowerOfDespair : public CDungeon
{
public:
    CDungeon_TowerOfDespair();
    ~CDungeon_TowerOfDespair();
};

class CMap
{
public:
    CMap();
    ~CMap();
    int get_index() const;
    int get_map_type() const;
    char m_pad[0x2d8];
};

class Quest
{
public:
    Quest();
    ~Quest();
    char exposeQuest() const;
    char check_possible(const stSelectQuestParam& param) const;
    char check_power_side(char side);
    void set_quest(QuestScript script);
    char m_pad0[0x8];
    int m_field8;                             // +0x08
    char m_padc[0x20];                        // +0x0c
    std::vector<std::vector<int> > m_vec2c;   // +0x2c
    std::vector<int> m_antiQuestList;         // +0x38
    char m_pad44[0xc4];                       // +0x44
    char m_mailFlag;                          // +0x108
    std::string m_mailTitle;                  // +0x10c
    std::string m_mailContents;               // +0x110
    std::vector<int> m_mailItems;             // +0x114
    char m_pad120[0x30];                      // +0x120
};

struct stSelectQuestParam
{
    char m_pad0[0x18];
    int m_questIdx;    // +0x18
    char m_pad1c[0x34];
    char m_powerSide;  // +0x50
};

namespace WongWork
{
class CQuestClear
{
public:
    bool isClearedQuest(unsigned int questIdx) const;
    static bool syncScript();
};
}

class STSkillScript
{
public:
    STSkillScript();
    ~STSkillScript();
    int m_index;                            // +0x00
    std::string m_name;                     // +0x04
    char m_pad8[0x28];                      // +0x08
    int m_levelLimit;                       // +0x30
    int m_field34;                          // +0x34
    int m_field38;                          // +0x38
    int m_field3c;                          // +0x3c
    int m_field40;                          // +0x40
    std::vector<int> m_vec44;               // +0x44
    std::vector<int> m_vec50;               // +0x50
    char m_skillType;                       // +0x5c
    char m_pad5d[3];
    int m_group;                            // +0x60
    int m_fitness[6];                       // +0x64
    int m_typeMaxLevel[6];                  // +0x7c
    int m_secondFitness[12];                // +0x94
    char m_padC4[0x30];                     // +0xc4..+0xf4
    std::vector<int> m_secondGrowType;      // +0xf4
    char m_pad100[0x70];                    // +0x100..+0x170
    std::vector<STKeyCommandUnit> m_keyCmd; // +0x170
    char m_pad17c[0x2c];                    // +0x17c..+0x1a8
    std::vector<std::vector<float> >
        m_fitnessTable;                     // +0x1a8
    char m_pad1b4[0x28];                    // +0x1b4..+0x1dc
    std::vector<int> m_fitGrowType;         // +0x1dc
    char m_stealable;                       // +0x1e8
    char m_pad1e9[7];
    int m_stealLevelPenalty;                // +0x1f0
    int m_stealSPPenalty;                   // +0x1f4
    char m_pad1f8[0xc];                     // +0x1f8..+0x204
    std::vector<int> m_vec204;              // +0x204
    std::vector<int> m_vec210;              // +0x210
    char m_pad21c[0x44];                    // +0x21c..+0x260
    char m_field260;                        // +0x260
    char m_pad261[0x1f];                    // +0x261..+0x280
    char m_field280;                        // +0x280
};

class STDungeonScript
{
public:
    STDungeonScript();
    ~STDungeonScript();
    char m_pad[0x2c0];
    char m_field2c0[0x58];
    std::vector<int> m_dailySchedule;   // +0x318
    char m_pad324[0x1e5];
    char m_isTowerOfDespair;            // +0x509
};

struct stMonsterIndependentDrop_t
{
    char m_pad[0x30];
};

struct stIndependentDropInfo
{
    int m_field0;                       // +0x00
    int m_monsterIndex;                 // +0x04
    stMonsterIndependentDrop_t m_drop;  // +0x08
};

class STMonsterScript
{
public:
    STMonsterScript();
    ~STMonsterScript();
    int m_index;                              // +0x00
    char m_pad4[0x3a8];                       // +0x04
    std::vector<stMonsterIndependentDrop_t>
        m_dropList;                           // +0x3ac
    char m_pad3b8[0x30];                      // +0x3b8
    int m_minLevel;                           // +0x3e8
    int m_maxLevel;                           // +0x3ec
};

struct MonsterKillItem
{
    char m_pad[0x10];
    int m_itemIdx;    // +0x10
};

class QuestScript
{
public:
    QuestScript();
    QuestScript(const QuestScript& other);
    ~QuestScript();
    int m_index;                            // +0x00
    char m_pad4[0xd0];                      // +0x04
    int m_questType;                        // +0xd4
    char m_padD8[0xc];                      // +0xd8
    int m_npcIdx;                           // +0xe4
    char m_padE8[0x5c];                     // +0xe8
    std::vector<MonsterKillItem>
        m_monsterKillList;                  // +0x144
    int m_mapIdx;                           // +0x150
    int m_mapIdx2;                          // +0x158
    char m_pad15c[0x1c];                    // +0x15c
    int m_questDataType;                    // +0x178
    int m_questDataSubType;                 // +0x17c
    std::vector<int> m_questData;           // +0x180
    char m_pad18c[0xcc];                    // +0x18c..+0x258
    char m_mailQuestFlag;                   // +0x258
    char m_pad259[0x83];                    // +0x259..+0x2dc
    std::vector<int> m_dailySchedule;       // +0x2dc
    char m_pad2e8[0x7c];                    // +0x2e8..+0x364
};

class CNRDScriptData
{
public:
    CNRDScriptData();
    ~CNRDScriptData();
    void import(const char* path);
    int getInt(const char* key);
    char m_pad[0xa4];
};

class CDailyScheduleManager
{
public:
    void AddDailSchedule(const std::vector<int>& schedule);
};

class CGameManager;
CGameManager* G_CGameManager();

namespace QuickParty
{
class CQuickPartySystemManager;
}

class CGameManager
{
public:
    QuickParty::CQuickPartySystemManager* GetQuickPartySystemManager();
};

namespace QuickParty
{
class CQuickPartySystemManager
{
public:
    void init_quick_party_pool(const CDungeon* dungeon);
};
}

int get_rand_int(int range);

extern int ImportSkillScript(STSkillScript* script, const char* path,
                             int job, int type, int flag);
extern int ImportDungeonScript(STDungeonScript* script, const char* path);
extern int ImportMonsterScript(STMonsterScript* script, int type,
                               const char* path, bool flag);
extern int importQuestScript(QuestScript* script, const char* path);

// ====================================================================

std::map<int, PvPMapInfo> CMapList::mapPvpMapInfo_;

PvPMapInfo::PvPMapInfo()
{
    m_mapIdx = 0;
    m_playable = 0;
}

// ===================== CDungeonList =====================

CDungeonList::CDungeonList()
{
}

CDungeonList::~CDungeonList()
{
    destroy();
}

void CDungeonList::destroy()
{
    CDungeon* dungeon = 0;
    __gnu_cxx::hash_map<int, CDungeon*>::iterator it;
    it = m_dungeonMap.begin();
    while (it != m_dungeonMap.end())
    {
        dungeon = it->second;
        delete dungeon;
        it++;
    }
    m_dungeonMap.clear();
}

CDungeon* CDungeonList::find_dungeon(int idx) const
{
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it;
    it = m_dungeonMap.find(idx);
    if (it == m_dungeonMap.end())
    {
        return 0;
    }
    return it->second;
}

CDungeon* CDungeonList::get_dungeon_at_index(int idx) const
{
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it;
    int count = 0;
    it = m_dungeonMap.begin();
    while (true)
    {
        if (it == m_dungeonMap.end())
        {
            return 0;
        }
        if (idx == count)
        {
            break;
        }
        count++;
        it++;
    }
    return it->second;
}

CDungeonList* CDungeonList::getDungeonIndexList()
{
    return this;
}

bool CDungeonList::insert_dungeon(CDungeon* dungeon)
{
    int idx = dungeon->get_index();
    __gnu_cxx::hash_map<int, CDungeon*>::iterator it = m_dungeonMap.find(idx);
    __gnu_cxx::hash_map<int, CDungeon*>::iterator end = m_dungeonMap.end();
    if (it == end)
    {
        m_dungeonMap.insert(std::make_pair(idx, dungeon));
        return true;
    }
    LogManager::logFormat(
        1, "data_manager.cpp", "bool CDungeonList::insert_dungeon(CDungeon*)",
        0x921, "Already registed dungeon index detected : %d",
        dungeon->get_index());
    return false;
}

int CDungeonList::load_list()
{
    char* path = 0;
    STDungeonScript script;
    CDungeon* dungeon = 0;
    for (std::map<int, std::string>::iterator it =
             sync_script::g_dungeonInfoTable.begin();
         it != sync_script::g_dungeonInfoTable.end(); ++it)
    {
        path = (char*)it->second.c_str();
        if (path == 0)
        {
            break;
        }
        if (!ImportDungeonScript(&script, path))
        {
            printf("Dungeon Script Error: %s\n", path);
            return 0;
        }
        if (script.m_dailySchedule.size() != 0)
        {
            GlobalData::s_DailyScheduleManager->AddDailSchedule(
                script.m_dailySchedule);
        }
        if (script.m_isTowerOfDespair)
        {
            dungeon = new CDungeon_TowerOfDespair();
        }
        else
        {
            dungeon = new CDungeon();
        }
        dungeon->set_dungeon(script);
        insert_dungeon(dungeon);
        G_CGameManager()->GetQuickPartySystemManager()
            ->init_quick_party_pool(dungeon);
        if (size() > 1000)
        {
            cMyTrace trace("bool CDungeonList::load_list()", 0x8ec, 5);
            trace("Best Clear Time Save Dungeon Index Over Error: %d\n",
                  size());
            return 0;
        }
    }
    return 1;
}

// ===================== CMapList =====================

CMapList::CMapList()
{
    m_pvpMapCount = 0;
}

CMapList::~CMapList()
{
    destroy();
}

void CMapList::destroy()
{
    CMap* map = 0;
    __gnu_cxx::hash_map<int, CMap*>::iterator it;
    it = m_mainMap.begin();
    while (it != m_mainMap.end())
    {
        map = it->second;
        delete map;
        it++;
    }
    m_mainMap.clear();
    for (int i = 0; i <= 14; ++i)
    {
        m_mapList[i + 1].clear();
        m_typeMap1[i].clear();
        m_typeMap2[i].clear();
    }
}

CMap* CMapList::find_map(int idx)
{
    __gnu_cxx::hash_map<int, CMap*>::iterator it;
    it = m_mainMap.find(idx);
    if (it != m_mainMap.end())
    {
        return it->second;
    }
    return 0;
}

int CMapList::insert_map(int channel, int key, CMap* map,
                         std::vector<PVPMAP_DATA>& pvpData)
{
    if (find_map(map->get_index()) != 0)
    {
        return 0;
    }
    int mapIdx = map->get_index();
    if (mapIdx > 0xc350 && mapIdx <= 0xc738)
    {
        m_pvpMapCount++;
        PvPMapInfo info;
        info.m_mapIdx = map->get_index();
        int count = 0;
        for (std::vector<PVPMAP_DATA>::iterator it = pvpData.begin();
             it != pvpData.end(); ++it)
        {
            if (it->m_mapIdx == info.m_mapIdx)
            {
                break;
            }
            count++;
        }
        PvPMapInfo& dst = mapPvpMapInfo_[count];
        if (&dst != &info)
        {
            memcpy(&dst, &info, 5);
        }
    }
    m_mainMap.insert(std::make_pair(mapIdx, map));
    int mapType = map->get_map_type();
    if (mapType == 0)
    {
        m_mapList[channel + 1].insert(std::make_pair(key, map));
    }
    else if (mapType == 2)
    {
        m_typeMap2[channel].insert(std::make_pair(key, map));
    }
    else
    {
        m_typeMap1[channel].insert(std::make_pair(key, map));
    }
    return 1;
}

void CMapList::LoadPvPSinglePlayableMap(char* path)
{
    char buf[512];
    CNRDScriptData data;
    data.import(path);
    for (std::map<int, PvPMapInfo>::iterator it = mapPvpMapInfo_.begin();
         it != mapPvpMapInfo_.end(); ++it)
    {
        sprintf(buf, "pvp deathmatch single play %d", it->second.m_mapIdx);
        it->second.m_playable = data.getInt(buf) == 1;
    }
}

int CMapList::GetPvpMapInfo(int idx, PvPMapInfo& info)
{
    std::map<int, PvPMapInfo>::iterator it;
    it = mapPvpMapInfo_.find(idx);
    if (it != mapPvpMapInfo_.end())
    {
        if (&info != &it->second)
        {
            memcpy(&info, &it->second, 5);
        }
        return 1;
    }
    return 0;
}

CMapList* CMapList::GetMapList()
{
    return this;
}

int CMapList::GetPVPMapCount() const
{
    return m_pvpMapCount;
}

// ===================== CMonsterManager =====================

CMonsterManager::CMonsterManager()
{
}

CMonsterManager::~CMonsterManager()
{
    destroy();
}

void CMonsterManager::destroy()
{
    CMonster* monster = 0;
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.begin();
    while (it != m_monsterMap.end())
    {
        monster = it->second;
        it++;
        delete monster;
    }
    m_monsterMap.clear();
}

CMonster* CMonsterManager::find_monster(int idx)
{
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.find(idx);
    if (it == m_monsterMap.end())
    {
        return 0;
    }
    return it->second;
}

int CMonsterManager::get_levelrandom_mob(int level)
{
    std::pair<std::multimap<int, int>::iterator,
              std::multimap<int, int>::iterator> range =
        m_levelMonsterList.equal_range(level);
    int count = m_levelMonsterList.count(level);
    int rand = get_rand_int(count);
    std::multimap<int, int>::iterator it = range.first;
    for (int i = 0; i < rand; ++i)
    {
        ++it;
    }
    return it->second;
}

int CMonsterManager::insert_monster(int level, int monsterIdx)
{
    m_levelMonsterList.insert(std::make_pair(level, monsterIdx));
    return 1;
}

bool CMonsterManager::insert_monster(int level, CMonster* monster)
{
    __gnu_cxx::hash_map<int, CMonster*>::iterator it = m_monsterMap.find(level);
    __gnu_cxx::hash_map<int, CMonster*>::iterator end = m_monsterMap.end();
    if (it == end)
    {
        m_monsterMap.insert(std::make_pair(level, monster));
        return true;
    }
    return false;
}

int CMonsterManager::load_monsters()
{
    if (sync_script::GetInstanceSyncScript()->truncate_monster_info_table())
    {
        char* path = 0;
        CMonster* monster = 0;
        for (std::map<int, std::string>::iterator it =
                 sync_script::g_monsterInfoTable.begin();
             it != sync_script::g_monsterInfoTable.end(); ++it)
        {
            path = (char*)it->second.c_str();
            if (path == 0)
            {
                return 1;
            }
            STMonsterScript script;
            if (!ImportMonsterScript(&script, 3, path, false))
            {
                printf("Monster Script Error: %s\n", path);
                return 0;
            }
            if (!sync_script::GetInstanceSyncScript()
                     ->insert_monster_info_to_db(script))
            {
                printf("insert_monster_category Error: %s\n", path);
                return 0;
            }
            std::vector<stIndependentDropInfo>& drops =
                *(std::vector<stIndependentDropInfo>*)
                    ((char*)G_CDataManager() + 0xa890);
            for (unsigned int i = 0; i < drops.size(); ++i)
            {
                if (drops[i].m_field0 == 0 &&
                    drops[i].m_monsterIndex == script.m_index)
                {
                    script.m_dropList.push_back(drops[i].m_drop);
                }
            }
            monster = new CMonster();
            monster->set_monster(script);
            for (int i = script.m_minLevel; i <= script.m_maxLevel; ++i)
            {
                insert_monster(i, monster->get_index());
            }
            insert_monster(monster->get_index(), monster);
        }
        return 1;
    }
    puts("Fail truncate_monster_info_table");
    return 0;
}

// ===================== CSkillList =====================

CSkillList::CSkillList()
{
}

CSkillList::~CSkillList()
{
    destroy();
}

void CSkillList::destroy()
{
    __gnu_cxx::hash_map<int, CSkill*>::iterator it;
    __gnu_cxx::hash_map<int, CSkill*>::iterator end;
    CSkill* skill = 0;
    for (int i = 0; i <= 10; ++i)
    {
        it = m_skillList[i].begin();
        end = m_skillList[i].end();
        while (it != end)
        {
            skill = it->second;
            delete skill;
            it++;
        }
        m_skillList[i].clear();
        m_groupSkillMap[i].clear();
    }
}

CSkill* CSkillList::find_skill(int jobType, int skillIdx)
{
    if (jobType > 10)
    {
        return 0;
    }
    __gnu_cxx::hash_map<int, CSkill*>::iterator it;
    it = m_skillList[jobType].find(skillIdx);
    if (it == m_skillList[jobType].end())
    {
        return 0;
    }
    return it->second;
}

void CSkillList::insert_skill(int jobType, CSkill* skill)
{
    if (skill == 0)
    {
        cMyTrace trace("void CSkillList::insert_skill(int, CSkill*)", 0xf91, 5);
        trace("[%s][%d]_(0 == pSkill)",
              "void CSkillList::insert_skill(int, CSkill*)", 0xf91);
        return;
    }
    int idx = skill->get_index();
    __gnu_cxx::hash_map<int, CSkill*>::iterator it =
        m_skillList[jobType].find(idx);
    __gnu_cxx::hash_map<int, CSkill*>::iterator end =
        m_skillList[jobType].end();
    if (it != end)
    {
        cMyTrace trace("void CSkillList::insert_skill(int, CSkill*)", 0xfa4, 5);
        trace("[%s][%d]_( itr != m_SkillList[job_type].end() )",
              "void CSkillList::insert_skill(int, CSkill*)", 0xfa4);
        return;
    }
    m_skillList[jobType].insert(std::make_pair(idx, skill));
    int group = skill->get_group();
    m_groupSkillMap[jobType].insert(std::make_pair(group, skill));
}

int CSkillList::get_ran_groupskill_index(int jobType, int group, int skillIdx)
{
    int count = m_groupSkillMap[jobType].count(group);
    if (count == 0)
    {
        return -1;
    }
    int rand = get_rand_int(count);
    std::pair<std::multimap<int, CSkill*>::iterator,
              std::multimap<int, CSkill*>::iterator> range =
        m_groupSkillMap[jobType].equal_range(group);
    std::multimap<int, CSkill*>::iterator it = range.first;
    for (int i = 0; i < rand; ++i)
    {
        ++it;
    }
    CSkill* skill = it->second;
    if (skill->get_index() == skillIdx)
    {
        ++it;
        if (it != range.second)
        {
            skill = it->second;
        }
        else
        {
            --it;
            if (it != range.first)
            {
                --it;
                skill = it->second;
            }
            else
            {
                cMyTrace trace(
                    "int CSkillList::get_ran_groupskill_index(int, int, int)",
                    0xfde, 5);
                trace("[%s][%d]_( i == res.first )",
                      "int CSkillList::get_ran_groupskill_index(int, int, int)",
                      0xfde);
            }
        }
    }
    return skill->get_index();
}

void CSkillList::set_post_learning_skill(int jobType, CSkill* skill)
{
    if (skill != 0 && skill->m_preSkillList.size() != 0)
    {
        for (std::list<_pre_skill>::iterator it =
                 skill->m_preSkillList.begin();
             it != skill->m_preSkillList.end(); ++it)
        {
            std::map<int, std::vector<std::pair<int, int> > >::iterator found =
                m_postLearningMap[jobType].find(it->m_jobType);
            std::pair<int, int> value(skill->get_index(), it->m_jobType);
            if (found == m_postLearningMap[jobType].end())
            {
                std::vector<std::pair<int, int> > vec;
                vec.push_back(value);
                m_postLearningMap[jobType].insert(
                    std::make_pair(it->m_jobType, vec));
            }
            else
            {
                found->second.push_back(value);
            }
        }
    }
}

bool CSkillList::get_post_learning_skill(
    int jobType, int skillIdx, std::vector<std::pair<int, int> >& out)
{
    std::map<int, std::vector<std::pair<int, int> > >::iterator it =
        m_postLearningMap[jobType].find(skillIdx);
    if (it != m_postLearningMap[jobType].end())
    {
        out = it->second;
        return true;
    }
    return false;
}

__gnu_cxx::hash_map<int, CSkill*>* CSkillList::getSkillList(int jobType)
{
    if (jobType > 10)
    {
        return 0;
    }
    return &m_skillList[jobType];
}

int CSkillList::init()
{
    STSkillScript script;
    CSkill* skill = 0;
    sync_script::GetInstanceSyncScript()->truncate_skill_index();
    sync_script::GetInstanceSyncScript()->trancate_skill_info();
    int job = 0;
    int path = 0;
    for (job = 0; job <= 10; ++job)
    {
        for (std::map<int, std::string>::iterator it =
                 sync_script::g_skillInfoTable[job].begin();
             it != sync_script::g_skillInfoTable[job].end(); ++it)
        {
            path = (int)it->second.c_str();
            if (path == 0)
            {
                break;
            }
            if (!ImportSkillScript(&script, (const char*)path, job, 0, -1))
            {
                printf("Skill Script Error: %s\n", (const char*)path);
                return 0;
            }
            if (!sync_script::GetInstanceSyncScript()
                     ->insert_skill_index(&script, job))
            {
                printf("insert_skill_index Error: %s\n", (const char*)path);
                return 0;
            }
            if (sync_script::GetInstanceSyncScript()->IsEnable())
            {
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&script, job, 0))
                {
                    printf("insert_skill_info Error: %s\n",
                           (const char*)path);
                    return 0;
                }
                STSkillScript s2;
                if (!ImportSkillScript(&s2, (const char*)path, job, 4, -1) ||
                    !sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s2, job, 4))
                {
                    printf("Skill Script Error: %s\n", (const char*)path);
                    return 0;
                }
                STSkillScript s3;
                if (!ImportSkillScript(&s3, (const char*)path, job, 6, -1) ||
                    !sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s3, job, 6))
                {
                    printf("Skill Script Error: %s\n", (const char*)path);
                    return 0;
                }
                STSkillScript s4;
                if (!ImportSkillScript(&s4, (const char*)path, job, 11, -1) ||
                    !sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s4, job, 11))
                {
                    printf("Skill Script Error: %s\n", (const char*)path);
                    return 0;
                }
            }
            skill = new CSkill();
            skill->set_index(script.m_index);
            skill->reform_ui_group_no(script.m_group, script.m_field260,
                                      script.m_fitGrowType.size());
            skill->set_group(script.m_group);
            skill->set_job_type(job);
            skill->set_skill_type(script.m_skillType);
            skill->set_level_limit(script.m_levelLimit);
            skill->m_fieldC8 = script.m_field34;
            skill->SetStealable(script.m_stealable != 0);
            skill->SetStealLevelPenalty(script.m_stealLevelPenalty);
            skill->SetStealSPPenalty(script.m_stealSPPenalty);
            for (unsigned int i = 0; i < script.m_fitGrowType.size(); ++i)
            {
                skill->set_fitGrowType(i, script.m_fitGrowType[i]);
            }
            for (int i = 0; i <= 5; ++i)
            {
                skill->set_fitness(i, script.m_fitness[i]);
            }
            for (int i = 0; i <= 5; ++i)
            {
                skill->set_type_max_level(i, script.m_typeMaxLevel[i]);
            }
            for (int i = 0; i <= 1; ++i)
            {
                if (!script.m_secondGrowType.empty() &&
                    (unsigned int)i < script.m_secondGrowType.size())
                {
                    skill->set_fitness_second_growtype(
                        i + 1, script.m_secondGrowType[i]);
                }
            }
            for (int i = 0; i <= 5; ++i)
            {
                for (int j = 0; j <= 1; ++j)
                {
                    skill->set_second_growtype_fitness(
                        i, j + 1, *(int*)((char*)&script + 0x94 +
                                          (i * 4 + j) * 4));
                }
            }
            for (int i = 0; i <= 5; ++i)
            {
                for (int j = 0; j <= 1; ++j)
                {
                    skill->set_second_growtype_max_level(
                        i, j + 1, *(int*)((char*)&script + 0x9c +
                                          (i * 4 + j) * 4));
                }
            }
            for (unsigned int i = 0; i < script.m_vec44.size(); ++i)
            {
                skill->add_need_sp(script.m_vec44.at(i));
            }
            for (unsigned int i = 0; i < script.m_vec50.size(); ++i)
            {
                skill->add_need_sfp(script.m_vec50.at(i));
            }
            for (unsigned int i = 0; i < script.m_vec44.size(); ++i)
            {
                _pre_skill pre;
                pre.m_index = script.m_vec44.at(i);
                pre.m_jobType = script.m_vec50.at(i);
                pre.m_group = *(int*)((char*)&script + 0x2cc);
                skill->add_learned_skill(pre);
            }
            for (unsigned int i = 0; i < script.m_vec204.size(); ++i)
            {
                skill->m_guildSkillNeedGSP.push_back(script.m_vec204.at(i));
            }
            for (unsigned int i = 0; i < script.m_vec210.size(); ++i)
            {
                skill->m_guildSkillNeedGold.push_back(script.m_vec210.at(i));
            }
            skill->m_needGuildLevel = script.m_field3c;
            skill->m_needGuildLevelAdd = script.m_field40;
            skill->m_field104 = *(int*)((char*)&script + 0x14);
            for (unsigned int i = 0; i < script.m_fitnessTable.size(); ++i)
            {
                std::vector<int> vec;
                for (unsigned int j = 0;
                     j < script.m_fitnessTable[i].size(); ++j)
                {
                    int v = (int)script.m_fitnessTable[i].at(j);
                    vec.push_back(v);
                }
                skill->m_field108.push_back(vec);
            }
            skill->m_isSpecialSkill = script.m_field260;
            skill->m_keyCommandList = script.m_keyCmd;
            *(char*)((char*)skill + 0x130) = script.m_field280;
            skill->setSkillName(script.m_name.c_str());
            skill->m_pvpSkillGrade = script.m_field38;
            insert_skill(job, skill);
        }
    }
    for (int i = 0; i <= 10; ++i)
    {
        __gnu_cxx::hash_map<int, CSkill*>::iterator it = m_skillList[i].begin();
        __gnu_cxx::hash_map<int, CSkill*>::iterator end = m_skillList[i].end();
        while (it != end)
        {
            skill = it->second;
            skill->set_prelearned_skill();
            set_post_learning_skill(i, skill);
            ++it;
        }
    }
    return 1;
}

// ===================== QuestList =====================

QuestList::QuestList()
{
}

QuestList::~QuestList()
{
    destroy();
}

void QuestList::destroy()
{
    Quest* quest = 0;
    __gnu_cxx::hash_map<int, Quest*>::iterator it;
    it = m_questMap.begin();
    while (it != m_questMap.end())
    {
        quest = it->second;
        delete quest;
        it++;
    }
    m_questMap.clear();
    m_npcQuestList[0].clear();
    m_npcQuestList[1].clear();
    m_npcQuestList[2].clear();
    m_npcQuestList[7].clear();
    m_npcQuestList[3].clear();
    m_npcQuestList[4].clear();
    m_npcQuestList[5].clear();
    m_listE0.clear();
    m_npcQuestList[6].clear();
}

Quest* QuestList::find_quest(int idx)
{
    __gnu_cxx::hash_map<int, Quest*>::iterator it;
    it = m_questMap.find(idx);
    if (it != m_questMap.end())
    {
        return it->second;
    }
    return 0;
}

std::multimap<int, int>* QuestList::getQuestNPCList(ENUM_QUEST_GRADE grade)
{
    switch (grade)
    {
    case ENUM_QUEST_GRADE_NONE:
        return &m_npcQuestList[0];
    case ENUM_QUEST_GRADE_1:
        return &m_npcQuestList[3];
    case ENUM_QUEST_GRADE_2:
        return &m_npcQuestList[4];
    case ENUM_QUEST_GRADE_4:
        return &m_npcQuestList[2];
    case ENUM_QUEST_GRADE_5:
        return &m_npcQuestList[1];
    case ENUM_QUEST_GRADE_6:
        return &m_npcQuestList[5];
    case ENUM_QUEST_GRADE_8:
        return &m_npcQuestList[6];
    default:
        return 0;
    }
}

void QuestList::insert_NPC(int npcIdx)
{
    if (npcIdx != 0)
    {
        for (std::list<int>::iterator it = m_listE0.begin();
             it != m_listE0.end(); ++it)
        {
            if (*it == npcIdx)
            {
                return;
            }
        }
        m_listE0.push_back(npcIdx);
    }
}

int QuestList::Check_Quest(Quest* quest, stSelectQuestParam& param,
                           const WongWork::CQuestClear& clear)
{
    if (quest->exposeQuest())
    {
        if (quest->check_possible(param))
        {
            if (check_ahead_quest(param.m_questIdx, clear))
            {
                if (check_anti_quest(param.m_questIdx, clear))
                {
                    if (quest->check_power_side(param.m_powerSide))
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int QuestList::check_ahead_quest(int questIdx,
                                 const WongWork::CQuestClear& clear)
{
    Quest* quest = find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            "bool QuestList::check_ahead_quest(int, const WongWork::CQuestClear&)",
            0x14b7, "find_quest(%d)", questIdx);
        return 0;
    }
    if (!quest->m_vec2c.empty())
    {
        for (unsigned int i = 0; i < quest->m_vec2c.size(); ++i)
        {
            char found = 0;
            for (unsigned int j = 0; j < quest->m_vec2c[i].size(); ++j)
            {
                unsigned int aheadIdx = quest->m_vec2c[i][j];
                if (!clear.isClearedQuest(aheadIdx))
                {
                    found = 1;
                    break;
                }
            }
            if (found != 1)
            {
                return 1;
            }
        }
        return 0;
    }
    return 1;
}

int QuestList::check_anti_quest(int questIdx,
                                const WongWork::CQuestClear& clear)
{
    Quest* quest = find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            "bool QuestList::check_anti_quest(int, const WongWork::CQuestClear&)",
            0x150a, "find_quest(%d)", questIdx);
        return 0;
    }
    for (std::vector<int>::iterator it = quest->m_antiQuestList.begin();
         it != quest->m_antiQuestList.end(); ++it)
    {
        unsigned int antiIdx = *it;
        if (antiIdx > 29999)
        {
            cMyTrace trace(
                "bool QuestList::check_anti_quest(int, const WongWork::CQuestClear&)",
                0x151a, 5);
            trace("[%s][%d]",
                  "bool QuestList::check_anti_quest(int, const WongWork::CQuestClear&)",
                  0x151a);
            return 0;
        }
        if (clear.isClearedQuest(antiIdx))
        {
            return 0;
        }
    }
    return 1;
}

int QuestList::check_clear(int questIdx, const WongWork::CQuestClear& clear)
{
    if (questIdx < 30000)
    {
        if (clear.isClearedQuest(questIdx))
        {
            return 0;
        }
        return 1;
    }
    cMyTrace trace(
        "bool QuestList::check_clear(int, const WongWork::CQuestClear&)",
        0x152f, 5);
    trace("[%s][%d]",
          "bool QuestList::check_clear(int, const WongWork::CQuestClear&)",
          0x152f);
    return 0;
}

bool QuestList::select_mail_quest(std::list<int>& out,
                                  const WongWork::CQuestClear& clear,
                                  const stSelectQuestParam& param)
{
    out.clear();
    for (std::vector<int>::iterator it = m_vecD4.begin();
         it != m_vecD4.end(); ++it)
    {
        int questIdx = *it;
        Quest* quest = find_quest(questIdx);
        if (quest != 0 &&
            quest->check_possible(param) &&
            check_clear(questIdx, clear) &&
            check_ahead_quest(questIdx, clear) &&
            check_anti_quest(questIdx, clear))
        {
            out.push_back(questIdx);
        }
    }
    return !out.empty();
}

void QuestList::select_quest(std::list<int>& out, stSelectQuestParam& param,
                             const WongWork::CQuestClear& clear)
{
    out.clear();
    allowable_questlist_as_npc(out, m_npcQuestList[0], param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList[1], param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList[2], param, clear, true);
    allowable_questlist_as_npc(out, m_npcQuestList[7], param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList[3], param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList[4], param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList[5], param, clear, false);
}

void QuestList::allowable_questlist_as_npc(
    std::list<int>& out, const std::multimap<int, int>& npcList,
    stSelectQuestParam& param, const WongWork::CQuestClear& clear, bool flag)
{
    Quest* quest = 0;
    for (std::list<int>::iterator it = m_listE0.begin();
         it != m_listE0.end(); ++it)
    {
        int npcIdx = *it;
        std::pair<std::multimap<int, int>::const_iterator,
                  std::multimap<int, int>::const_iterator> range =
            npcList.equal_range(npcIdx);
        for (std::multimap<int, int>::const_iterator cit = range.first;
             cit != range.second; ++cit)
        {
            param.m_questIdx = cit->second;
            quest = find_quest(param.m_questIdx);
            if (quest != 0 && Check_Quest(quest, param, clear) &&
                (flag || check_clear(param.m_questIdx, clear)))
            {
                out.push_back(param.m_questIdx);
            }
        }
    }
}

void QuestList::GetDailyQuestList(std::list<int>& out)
{
    for (std::list<int>::iterator it = m_listE0.begin();
         it != m_listE0.end(); ++it)
    {
        int npcIdx = *it;
        std::pair<std::multimap<int, int>::const_iterator,
                  std::multimap<int, int>::const_iterator> range =
            m_npcQuestList[7].equal_range(npcIdx);
        for (std::multimap<int, int>::const_iterator cit = range.first;
             cit != range.second; ++cit)
        {
            int questIdx = cit->second;
            Quest* quest = find_quest(questIdx);
            if (quest != 0 && quest->m_field8 == 3)
            {
                out.push_back(questIdx);
            }
        }
    }
}

void QuestList::GetTrainingQuestList(std::vector<int>& out, short level)
{
    if (level < 0x47 && level > 0)
    {
        out = *G_CDataManager()->getDailyTrainingQuest(level);
    }
}

static int getQuestItemType(CItem* item)
{
    int (**vtbl)(void*) = *(int (***)(void*))item;
    return vtbl[3](item);
}

int QuestList::load_list()
{
    QuestScript script;
    Quest* quest = 0;
    sync_script::GetInstanceSyncScript()->truncate_quest_category();
    for (std::map<int, std::string>::iterator it =
             sync_script::g_questInfoTable.begin();
         it != sync_script::g_questInfoTable.end(); ++it)
    {
        char* path = (char*)it->second.c_str();
        if (path == 0)
        {
            break;
        }
        if (!importQuestScript(&script, path))
        {
            LogManager::logFormat(1, "data_manager.cpp",
                                  "bool QuestList::load_list()", 0x1357,
                                  "Quest Script Error: %s\n", path);
            return 0;
        }
        if (!script.m_dailySchedule.empty())
        {
            GlobalData::s_DailyScheduleManager->AddDailSchedule(
                script.m_dailySchedule);
        }
        if (!script.m_monsterKillList.empty())
        {
            for (std::vector<MonsterKillItem>::iterator mit =
                     script.m_monsterKillList.begin();
                 mit != script.m_monsterKillList.end(); ++mit)
            {
                CItem* item = G_CDataManager()->find_item(mit->m_itemIdx);
                if (item == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x136f,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Exist\n",
                        path, mit->m_itemIdx);
                    return 0;
                }
                if (!(item->is_stackable() && getQuestItemType(item) == 8))
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x1375,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Quest Item\n",
                        path, mit->m_itemIdx);
                    return 0;
                }
            }
        }
        if (script.m_questDataType == 2 || script.m_questDataType == 6)
        {
            int div = 4;
            if (script.m_questDataType == 6)
            {
                div = 5;
            }
            if (script.m_questData.size() % div != 0 ||
                script.m_questData.size() / div >= 4)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", "bool QuestList::load_list()",
                    0x1383,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType,
                    (int)script.m_questData.size());
                return 0;
            }
            std::vector<int>::iterator vit = script.m_questData.begin();
            std::vector<int>::iterator vend = script.m_questData.end();
            while (vit != vend)
            {
                int v1 = *vit++;
                int v2 = *vit++;
                int v3 = *vit++;
                if (script.m_questDataType == 6)
                {
                    unsigned int enemyType = *vit++;
                    if (enemyType > 10 || ((1 << (enemyType & 0x1f)) & 0x40e)
                                              == 0)
                    {
                        LogManager::logFormat(
                            1, "data_manager.cpp", "bool QuestList::load_list()",
                            0x139d,
                            "Quest Script Error: %s - Unknown enemy type(%d)\n",
                            path, enemyType);
                        return 0;
                    }
                }
                int triggerSize = *vit++;
                if (triggerSize > 0x1ff)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x13a4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Trigger Size is Wrong(%d)\n",
                        path, script.m_questDataType, triggerSize);
                    return 0;
                }
            }
        }
        else if (script.m_questDataType == 0)
        {
            if ((script.m_questData.size() & 1) != 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", "bool QuestList::load_list()",
                    0x13ba,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType,
                    (int)script.m_questData.size());
                return 0;
            }
        }
        else if (script.m_questDataType == 1)
        {
            int size = (int)script.m_questData.size();
            if (script.m_questDataSubType == 4)
            {
                if (size != 2)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x13c4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType, size);
                    return 0;
                }
            }
            else if (script.m_questDataSubType == 6)
            {
                if (size != 2 && size != 3)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x13cc,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType, size);
                    return 0;
                }
            }
            else if (script.m_questDataSubType < 4 ||
                     script.m_questDataSubType == 5 ||
                     (script.m_questDataSubType > 6 &&
                      script.m_questDataSubType < 0xc))
            {
                if (size != 3)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x13d4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType, size);
                    return 0;
                }
            }
            else if (size != 4)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", "bool QuestList::load_list()",
                    0x13dc,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType, size);
                return 0;
            }
        }
        if (script.m_mapIdx != -1)
        {
            CMap* map = (CMap*)G_CDataManager()->find_map(script.m_mapIdx);
            if (map == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", "bool QuestList::load_list()",
                    0x13e9,
                    "Quest Script Error: %s - NPC wrong npc appear map index "
                    "%d.",
                    path, script.m_mapIdx2);
                return 0;
            }
            if (!G_CDataManager()->VerifyMap(*map, script.m_mapIdx2))
            {
                return 0;
            }
        }
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_quest_category(&script))
        {
            printf("insert_quest_category Error: %s\n", path);
            return 0;
        }
        quest = new Quest();
        if (quest == 0)
        {
            cMyTrace trace("bool QuestList::load_list()", 0x13ff, 5);
            trace("[%s][%d]", "bool QuestList::load_list()", 0x13ff);
            return 0;
        }
        QuestScript script2(script);
        quest->set_quest(script2);
        insert_NPC(script.m_npcIdx);
        m_questMap.insert(std::make_pair(script.m_index, quest));
        if (script.m_questType == 4)
        {
            m_npcQuestList[2].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 5)
        {
            m_npcQuestList[1].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 0)
        {
            m_npcQuestList[0].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 2)
        {
            m_npcQuestList[4].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 6)
        {
            m_npcQuestList[5].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 3)
        {
            m_npcQuestList[7].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 1)
        {
            m_npcQuestList[3].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 8)
        {
            m_npcQuestList[6].insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        if (script.m_mailQuestFlag != -1)
        {
            m_vecD4.push_back(script.m_index);
        }
    }
    if (!m_vecD4.empty())
    {
        for (std::vector<int>::iterator it = m_vecD4.begin();
             it != m_vecD4.end(); ++it)
        {
            int questIdx = *it;
            Quest* q = find_quest(questIdx);
            if (q != 0)
            {
                if (q->m_mailFlag == -1)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x145c, "Mail Quest Error (quest index : %d)",
                        questIdx);
                    return 0;
                }
                if (q->m_mailTitle.size() == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x1462,
                        "Mail Quest Error! mail title err(quest index : %d)",
                        questIdx);
                    return 0;
                }
                if (q->m_mailContents.size() == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x1467,
                        "Mail Quest Error! mail contents err(quest index : %d)",
                        questIdx);
                    return 0;
                }
                if ((q->m_mailItems.size() & 1) != 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", "bool QuestList::load_list()",
                        0x146c,
                        "Mail Quest Error! mail enclose item err(quest index : "
                        "%d)",
                        questIdx);
                    return 0;
                }
            }
        }
    }
    if (WongWork::CQuestClear::syncScript())
    {
        return 1;
    }
    LogManager::logFormat(1, "data_manager.cpp", "bool QuestList::load_list()",
                          0x1496,
                          "WongWork::CQuestClear::syncScript() fail");
    return 0;
}
