#include "LogManager.h"
#include "GameRand.h"
// df_game_r 列表/脚本表类还原（G2 第七批）。
// CSkillList / QuestList / CDungeonList / CMapList / CMonsterManager。
// 布局以 ORIG 构造/析构汇编为准；函数形态对照 class_func_reports/
// （AE 口径：调用目标/全局地址归一；字段/栈偏移/常量严格一致）。
// ST*Script / CSkill / CMonster / CDungeon / CMap / Quest 等实现在各自 TU，
// 本文件只声明必要接口并调用。

#include "CDataManager.h"
// ENUM_CHARACTERJOB 由 CDataManager.h 链中的 STEquipmentScript.h 提供（与
// Quest.h/STSkillScript.h 同名同构）；仅作 ImportSkillScript 形参 cast 使用。
#include "CMap.h"

#include <ext/hash_map>
#include <stdio.h>
#include <string.h>
#include <utility>

#include "GameTypes.h"
#include "GlobalData.h"
#include "CDailyScheduleManager.h"  // _DailySchedule 权威布局（ORIG 0x8125c3c AddDailSchedule 签名）
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
}

// ---- 脚本文件列表全局表（ORIG C 名，定义在 sync_script.cpp） ----
extern "C" std::map<int, std::string> g_stDungeonScriptFileList;    // ORIG 0x94f6ea0
extern "C" std::map<int, std::string> g_stMonsterScriptFileList;    // ORIG 0x94ff260
extern "C" std::map<int, std::string> g_staSkillScriptFileList[11]; // ORIG 0x9500820
extern "C" std::map<int, std::string> g_questScriptFileList;        // ORIG 0x9500480

class CMonster
{
public:
    CMonster();
    ~CMonster();
    int get_index();   // ORIG 仅非 const（_ZN7CMonster9get_indexEv；调用经 mangled 绑定真实实现）
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

class Quest
{
public:
    Quest();
    ~Quest();
    bool exposeQuest() const;
    bool check_possible(const stSelectQuestParam& param) const;
    bool check_power_side(char side);   // SIGFIX：ORIG 串定 bool（P174）
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
    // 本 TU 镜像（P144）：ORIG CListTables TU 视图仅 7 字段（size 0x1c）——证据
    // Check_Quest 0x08355474 的隐式拷贝只覆盖 +0x00..+0x18（+0x14 char 单字节，
    // +0x15..0x17 padding 不拷）。Quest.h 权威定义含 +0x1c..+0x24 三字段（供
    // Quest.cpp/UserQuest.cpp），本 TU 不使用，故镜像裁剪以复现拷贝形态。
    int m_level;          // +0x00
    int m_job;            // +0x04
    int m_firstGrow;      // +0x08
    int m_secondGrow;     // +0x0c
    int m_pvpGrade;       // +0x10
    char m_powerSide;     // +0x14
    int m_questIdx;       // +0x18
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
// 本地镜像布局（权威完整定义见 CDungeon.h，sizeof 0x50a）；
// 偏移以 ORIG load_list 0x0834d376 为准：
//   TowerOfDespair flag = movzbl script+0x2a9（-0x2a3(%ebp)，script 栈基 -0x54c）
//   m_dailySchedule     = vector<_DailySchedule> @ +0x318（AddDailSchedule 直传）
class STDungeonScript
{
public:
    STDungeonScript();
    ~STDungeonScript();
    char m_pad[0x2a9];                            // +0x000..+0x2a9
    char m_isTowerOfDespair;                      // +0x2a9
    char m_pad2aa[0x6e];                          // +0x2aa..+0x318
    std::vector<_DailySchedule> m_dailySchedule;  // +0x318
    char m_pad324[0x1e6];                         // +0x324..+0x50a
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
    ~QuestScript();
    // [Main b28d · ODR] 本文件是 QuestScript 的**局部镜像类**（布局 0x364 ≠ Quest.h 权威类
    // 0x30c）。若让编译器隐式生成拷贝构造，会发射与权威类同名的弱符号
    // `_ZN11QuestScriptC1ERKS_`（小/大版本竞争，链接器选择不定，可能把 0x364 版套到
    // 0x30c 权威对象上越界）。此处仅**声明**抑制本 TU 发射，拷贝使用点
    // （CListTables.cpp:1558 `QuestScript script2(script);`）解析到权威类的隐式 W
    // （ORIG 0x0837adc8 W 0xd65，N84 处置②），两类的成员偏移在 0x30c 内逐一对齐。
    QuestScript(const QuestScript&);
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
    std::vector<_DailySchedule> m_dailySchedule;  // +0x2dc（ORIG size/AddDailSchedule 实证）
    char m_pad2e8[0x7c];                    // +0x2e8..+0x364
};

class CNRDScriptData
{
public:
    CNRDScriptData();
    ~CNRDScriptData();
    void import(const char* path);
    int getInt(const char* key) const;
    char m_pad[0xa4];
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
                             ENUM_CHARACTERJOB job, int type, int flag);
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

// ---------------------------------------------------------------------------
// CDataManager::getDungeonIndexList()
// ORIG T 0x08365434 sz 0x15（_ZN12CDataManager19getDungeonIndexListEv 非 const）。
// b28 BM4/Main 归位：原定义在 UserPartyLink4.cpp，ORIG 与 CDungeonList::getDungeonIndexList
// 地址相邻（0x0836542c/0x08365434 同 TU）且 callee 定义先见使 4.4 outgoing 精确继承
// （消 0x18 保守帧 → ORIG sub $0x4）。this+0x00 = m_dungeonList，透传返回值。
// （CDungeonList.cpp 为镜像 TU 不入构建，定义落本 TU 才是真实发射点。）
// ---------------------------------------------------------------------------
CDungeonList* CDataManager::getDungeonIndexList()
{
    return m_dungeonList->getDungeonIndexList();
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
        1, "data_manager.cpp", __PRETTY_FUNCTION__,
        0x921, "Already registed dungeon index detected : %d",
        dungeon->get_index());
    return false;
}

bool CDungeonList::load_list()
{
    char* path = 0;
    STDungeonScript script;
    CDungeon* dungeon = 0;
    for (std::map<int, std::string>::iterator it =
             g_stDungeonScriptFileList.begin();
         it != g_stDungeonScriptFileList.end(); ++it)
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
            // ORIG 0x0834d448：lea script+0x318 直传 AddDailSchedule
            // （vector<_DailySchedule> const&，ORIG 0x8125c3c），无逐项转换。
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
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8ec, 5);
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

// [b28d-32 N(P223/N89): 迭代器/STL 形态与岛差（82/95） | ORIG 0x0834f08c]
void CMapList::destroy()
{
    // [b28d-solo-CListTables] ORIG 0x0834f08c：同 QuestList::destroy 形——for-init
    // 逗号式直构 it/end、`(*it)`（deEv）、前置 `++it`（ppEv）、map 无 0 初始化。
    CMap* map;
    for (__gnu_cxx::hash_map<int, CMap*>::iterator it = m_mainMap.begin(),
             end = m_mainMap.end();
         it != end;
         ++it)
    {
        map = (*it).second;
        delete map;
    }
    m_mainMap.clear();
    // [b28d-solo-CListTables N(N1/N94): 82/83，ORIG 把下标 i+1 折进常数
    // （i*0x14+0x10+0x4），4.4 对 m_mapList[i+1] 恒多一条 `lea 0x1(%eax),%edx`；
    // (m_mapList+1)[i] / (m_mapList+i)[1] 两形实测反多 lea+add（85 条）⇒ 保持
    // 下标形，落 N。 | ORIG 0x0834f11c]
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

// [b28d-32 N(P75/N89): vector/STL 内联与装载序差（202/187） | ORIG 0x0834f18e]
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
    // ORIG 0x0834f2ed..0x0834f42e：4 处插入各只有两次 call —— 先就地构造
    // hash_map 的 value_type（pair<const int,CMap*>），再 insert(value_type const&)：
    //   +0x17b _ZNSt4pairIKiP4CMapEC1IiRS2_EEOT_OT0_   （第一实参为 get_index() 右值）
    //   +0x1c4/+0x226/+0x272 …C1IRiRS2_…            （第一实参为左值 key）
    // 用 make_pair 会多出 pair<int,CMap*> → value_type 的转换临时（第三次 call）。
    m_mainMap.insert(
        __gnu_cxx::hash_map<int, CMap*>::value_type(map->get_index(), map));
    int mapType = map->get_map_type();
    if (mapType == 0)
    {
        m_mapList[channel + 1].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    else if (mapType == 2)
    {
        m_typeMap2[channel].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    else
    {
        m_typeMap1[channel].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    return 1;
}

// [b28d-32 N89/N82: STL 内联/槽位差（74=74 但装载序多块不同） | ORIG 0x0834f4ba]
void CMapList::LoadPvPSinglePlayableMap(char* path)
{
    char buf[512];
    CNRDScriptData data;
    data.import(path);
    for (std::map<int, PvPMapInfo>::iterator it = mapPvpMapInfo_.begin();
         it != mapPvpMapInfo_.end(); ++it)
    {
        // [b28d-solo-CListTables] ORIG 0x0834f4ba：ptEv 一次后把 &it->second 存具名
        // 指针槽 -0xc（mov %eax,-0xc(%ebp) / 复用回读），sprintf 与 m_playable 写
        // 均经该槽（P0 有槽往返 ⇒ 原码有具名局部）。
        PvPMapInfo* p = &it->second;
        sprintf(buf, "pvp deathmatch single play %d", p->m_mapIdx);
        p->m_playable = data.getInt(buf) == 1;
    }
}

// [b28d-32 N(setcc-fold): ORIG 寄存器内比较直跳 vs 4.4 物化 setne+test（P197④ 镜像负例） | ORIG 0x0834f5c8]
bool CMapList::GetPvpMapInfo(int idx, PvPMapInfo& info)
{
    // [b28c-21 代落 C28L35 spec] 初始化式直构：ORIG find 直接落 -0x10，无 C1Ev 默认构造。
    std::map<int, PvPMapInfo>::iterator it = mapPvpMapInfo_.find(idx);
    if (it != mapPvpMapInfo_.end())
    {
        // [b28c-21 代落 C28L35 spec] (b) 三形实测：原形（本形，两次 ptEv）/引用局部
        // （-0xc 槽回读 41 条）/裸 register 指针（40 条但 %ebx 保存+恢复，ORIG 地址
        // 在 caller-saved %eax ⇒ 违反 C28L28 判据）均不闭合；落 N：P183 地址重物化
        // 跨版本差（4.1 复用 ptEv 地址，4.4 二次物化）。
        // [b28d-solo-CListTables N(P183): 38/44，ORIG 无槽 + 单次 ptEv（4.1 前端
        // SAVE_EXPR 合并同一 &it->second，地址留 %eax 跨 cmp→memcpy）；4.4 二次物化
        // ptEv。具名指针形/引用形实测 41 条（多 -0xc 槽，ORIG 无该槽）、结构赋值形
        // 46 条（多一次自比较）均不闭合 ⇒ 保持无具名局部原形，落 N。 | ORIG 0x0834f5c8]
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

// [b28d-32 AE: const_iterator 对 + 后置 it++ + 具名 result 复现 ORIG，51=51 逐条全等 | ORIG 0x0834ff20]
int CMonsterManager::get_levelrandom_mob(int level)
{
    // [b28d-32] ORIG 0x0834ff20：range 类型为 const_iterator 对 ⇒ 从 non-const
    // equal_range 结果隐式转换构造（_C1ISt17_Rb_tree_iterator...EOS_ 实例）；
    // 迭代器遍历用后置 it++（ORIG 增量段 ppEi，P28）。
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range =
        m_levelMonsterList.equal_range(level);
    int count = m_levelMonsterList.count(level);
    int rand = get_rand_int(count);
    std::multimap<int, int>::const_iterator it = range.first;
    for (int i = 0; i < rand; ++i)
    {
        it++;
    }
    // ORIG 0x0834ffc0 尾：`mov 0x4(%eax),%eax; mov %eax,-0x10(%ebp);
    // mov -0x10(%ebp),%eax` ⇒ 具名结果局部 + return（P171 照抄）。
    int result = it->second;
    return result;
}

int CMonsterManager::insert_monster(int level, int monsterIdx)
{
    // ORIG 直接以 value_type 就地构造 pair<const int,int>(int&,int&)（单 ctor、
    // 无 make_pair 中转、无临时对象拷贝）→ 用显式 std::pair<const int,int> 形。
    m_levelMonsterList.insert(std::pair<const int, int>(level, monsterIdx));
    return 1;
}

// [b28d-32 N93/N98] ORIG 0x0834fdee：迭代器 it/end 槽序 ORIG it 深 end 浅，
// 4.4 按首次使用序分配不可翻（声明序交换反劣 61），54=54 助记符全等。
bool CMonsterManager::insert_monster(int level, CMonster* monster)
{
    // [b28d-32] ORIG 0x0834fdee：迭代器先默认构造（C1Ev）再 find 赋值
    // （P50 复活 + find 结果 8 字节拷贝到 it 槽）；insert 用显式
    // pair<const int,CMonster*>(int&,CMonster*&) 直构，无 make_pair 中转。
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.find(level);
    __gnu_cxx::hash_map<int, CMonster*>::iterator end = m_monsterMap.end();
    if (it == end)
    {
        m_monsterMap.insert(std::pair<const int, CMonster*>(level, monster));
        return true;
    }
    return false;
}

// [b28d-32 N(P75/N89): 扫描器/STL 内联与槽位差（215/203） | ORIG 0x0834fa84]
int CMonsterManager::load_monsters()
{
    if (sync_script::GetInstanceSyncScript()->truncate_monster_info_table())
    {
        char* path = 0;
        CMonster* monster = 0;
        for (std::map<int, std::string>::iterator it =
                 g_stMonsterScriptFileList.begin();
             it != g_stMonsterScriptFileList.end(); ++it)
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
                G_CDataManager()->m_independentDrop.m_vecC;
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

// [b28d-32 N(P223/N89): ORIG 尾岛与 STL 调用形差（71/98） | ORIG 0x08350b42]
void CSkillList::destroy()
{
    // [b28d-solo-CListTables] ORIG 0x08350b42：每轮取具名指针 p=&m_skillList[i]
    // （-0x14，地址计算 i*0x14+this 存槽）+ q=&m_groupSkillMap[i]（-0xc，
    // i*0x18+0xdc+this）+ 内层 for-init 逗号式直构 begin/end（无 C1Ev）+
    // `(*it)`（deEv）/前置 `++it`（ppEv）；skill 无 0 初始化。
    for (int i = 0; i <= 10; ++i)
    {
        __gnu_cxx::hash_map<int, CSkill*>* p = &m_skillList[i];
        CSkill* skill;
        for (__gnu_cxx::hash_map<int, CSkill*>::iterator it = p->begin(),
                 end = p->end();
             it != end;
             ++it)
        {
            skill = (*it).second;
            delete skill;
        }
        p->clear();
        std::multimap<int, CSkill*>* q = &m_groupSkillMap[i];
        q->clear();
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

// [b28d-32 N(P75/N93): 装载序/槽位差（125/118） | ORIG 0x083519a6]
void CSkillList::insert_skill(int jobType, CSkill* skill)
{
    if (skill == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xf91, 5);
        trace("[%s][%d]_(0 == pSkill)",
              __PRETTY_FUNCTION__, 0xf91);
        return;
    }
    int idx = skill->get_index();
    __gnu_cxx::hash_map<int, CSkill*>::iterator it =
        m_skillList[jobType].find(idx);
    __gnu_cxx::hash_map<int, CSkill*>::iterator end =
        m_skillList[jobType].end();
    if (it != end)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xfa4, 5);
        trace("[%s][%d]_( itr != m_SkillList[job_type].end() )",
              __PRETTY_FUNCTION__, 0xfa4);
        return;
    }
    // ORIG 0x08351aa3/0x08351af5：两处插入各只有两次 call —— 就地构造 value_type
    //   +0x0fd _ZNSt4pairIKiP6CSkillEC1IRiRS2_EEOT_OT0_（idx 左值）→ hash_map::insert
    //   +0x14f _ZNSt4pairIKiP6CSkillEC1IiRS2_EEOT_OT0_ （get_group() 右值，无中间变量）
    //          → multimap::insert；make_pair 会多出转换临时（第三次 call）。
    m_skillList[jobType].insert(
        __gnu_cxx::hash_map<int, CSkill*>::value_type(idx, skill));
    m_groupSkillMap[jobType].insert(
        std::multimap<int, CSkill*>::value_type(skill->get_group(), skill));
}

// [b28d-32 N(P183/N82): 容器重复访问/槽位差（138/117） | ORIG 0x08351c1e]
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
                    __PRETTY_FUNCTION__,
                    0xfde, 5);
                trace("[%s][%d]_( i == res.first )",
                      __PRETTY_FUNCTION__,
                      0xfde);
            }
        }
    }
    return skill->get_index();
}

// [b28d-32 N(P75/N93): 实参装载序与块序差（163/185） | ORIG 0x08351e18]
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

// [b28d-32 N(P75/N89): 大解析函数 STL 内联/槽位差（866/845） | ORIG 0x08350c26]
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
                 g_staSkillScriptFileList[job].begin();
             it != g_staSkillScriptFileList[job].end(); ++it)
        {
            path = (int)it->second.c_str();
            if (path == 0)
            {
                break;
            }
            if (!ImportSkillScript(&script, (const char*)path, (ENUM_CHARACTERJOB)job, 0, -1))
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
                if (!ImportSkillScript(&s2, (const char*)path, (ENUM_CHARACTERJOB)job, 4, -1) ||
                    !sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s2, job, 4))
                {
                    printf("Skill Script Error: %s\n", (const char*)path);
                    return 0;
                }
                STSkillScript s3;
                if (!ImportSkillScript(&s3, (const char*)path, (ENUM_CHARACTERJOB)job, 6, -1) ||
                    !sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s3, job, 6))
                {
                    printf("Skill Script Error: %s\n", (const char*)path);
                    return 0;
                }
                STSkillScript s4;
                if (!ImportSkillScript(&s4, (const char*)path, (ENUM_CHARACTERJOB)job, 11, -1) ||
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

// [b28d-32 N(P223/N89): ORIG 空块岛与迭代器/STL 形态差（81/93） | ORIG 0x08353c86]
void QuestList::destroy()
{
    // [b28d-solo-CListTables] ORIG 0x08353c86：for-init 逗号式同时直构 it=begin()
    // 与 end=m.end()（两个 sret 槽 -0x14/-0x1c，ORIG 条件区不重调 end）+
    // operator*（deEv，P167 ⇒ `(*it)`）+ 前置 `++it`（ppEv）+ quest 无 0 初始化。
    Quest* quest;
    for (__gnu_cxx::hash_map<int, Quest*>::iterator it = m_questMap.begin(),
             end = m_questMap.end();
         it != end;
         ++it)
    {
        quest = (*it).second;
        delete quest;
    }
    m_questMap.clear();
    m_npcQuestList0.clear();
    m_npcQuestList1.clear();
    m_npcQuestList2.clear();
    m_npcQuestList7.clear();
    m_npcQuestList3.clear();
    m_npcQuestList4.clear();
    m_npcQuestList5.clear();
    m_listE0.clear();
    m_npcQuestList6.clear();
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
        return &m_npcQuestList0;
    case ENUM_QUEST_GRADE_1:
        return &m_npcQuestList3;
    case ENUM_QUEST_GRADE_2:
        return &m_npcQuestList4;
    case ENUM_QUEST_GRADE_4:
        return &m_npcQuestList2;
    case ENUM_QUEST_GRADE_5:
        return &m_npcQuestList1;
    case ENUM_QUEST_GRADE_6:
        return &m_npcQuestList5;
    case ENUM_QUEST_GRADE_8:
        return &m_npcQuestList6;
    default:
        return 0;
    }
}

// [b28d-32 AE: P50 先声明后赋值（C1Ev）+ 后置 it++ + 双 nop 岛，61=61 逐条全等 | ORIG 0x08354f02]
void QuestList::insert_NPC(int npcIdx)
{
    // [b28d-32] ORIG 0x08354f02：`C1Ev` 默认构造迭代器 + 后置 `it++`（ppEi）+
    // 双 nop 岛（循环内 return 与正常出口汇合）⇒ 先声明后赋值（P50 复活）、
    // while 形、后置增量。
    if (npcIdx == 0)
        return;
    std::list<int>::iterator it;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        if (*it == npcIdx)
        {
            return;
        }
        it++;
    }
    m_listE0.push_back(npcIdx);
}

// [b28d-32 N(P74/P75): ORIG char 成员单字节拷贝 vs 4.4 折 dword（+0x15..17 padding 合并）+ 实参装载序（79/74）；本 TU 镜像已裁为 ORIG 7 字段 | ORIG 0x08355474]
int QuestList::Check_Quest(Quest* quest, stSelectQuestParam& param,
                           const WongWork::CQuestClear& clear)
{
    // [b28d-32] ORIG 0x08355474：函数头把 param 7 字段逐条拷入栈局部
    // （-0x24..-0xd，+0x14 为 char），随后各检查用负形早退
    // `call; xor $1,%eax; test %al,%al; je fail; mov $0,%eax; jmp end`
    // （exposeQuest/check_possible 仍用原 param，其余用副本字段），尾 return 1。
    // [b28d-solo-CListTables] ORIG 0x08355474：逐字段重载 param 基址拷贝
    // （+0x14 单字节 movzbl/mov %al），4.4 隐式拷贝折成基址复用 + dword 块
    // （+0x14 按 4 字节搬）⇒ 写显式逐字段赋值。
    int level = param.m_level;
    int job = param.m_job;
    int firstGrow = param.m_firstGrow;
    int secondGrow = param.m_secondGrow;
    int pvpGrade = param.m_pvpGrade;
    char powerSide = param.m_powerSide;
    int questIdx = param.m_questIdx;
    if (!quest->exposeQuest())
        return 0;
    if (!quest->check_possible(param))
        return 0;
    if (!check_ahead_quest(questIdx, clear))
        return 0;
    if (!check_anti_quest(questIdx, clear))
        return 0;
    if (!quest->check_power_side(powerSide))
        return 0;
    return 1;
}

// [b28d-32 N(P75/N93): 实参装载序与槽位差（94/86） | ORIG 0x08354fc8]
bool QuestList::check_ahead_quest(int questIdx,
                                 const WongWork::CQuestClear& clear)
{
    Quest* quest = find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__,
            0x14b7, "find_quest(%d)", questIdx);
        return 0;
    }
    // [b28d-solo-CListTables] ORIG 0x08354fc8：empty() 正形 guard（call empty;
    // test; je 循环；非空 fall-through 到 return 1）+ 外层 size 具名局部 -0x1c +
    // 内层 size 具名局部 -0x14（P157 每次重读 operator[]）。
    if (quest->m_vec2c.empty())
    {
        return 1;
    }
    // [b28d-solo-CListTables] ORIG 0x08355042：empty 检查后无条件发射一次
    // __normal_iterator<int*,vector<int>>::C1Ev（-0x28 槽），全函数未再使用
    // ⇒ 原码存在未使用的迭代器局部（P171 照抄死写入/P232 死局部占帧）。
    std::vector<int>::iterator unusedIt;
    unsigned int aheadIdx;
    unsigned int size1 = quest->m_vec2c.size();
    for (unsigned int i = 0; i < size1; ++i)
    {
        unsigned int size2 = quest->m_vec2c[i].size();
        // [b28d-solo-CListTables] P158：ORIG `movzbl found; xor $1; test; je`
        // ⇒ found 为 bool 且条件写 !found（char+!=1 在 4.4 折 cmpb $1）。
        bool found = false;
        for (unsigned int j = 0; j < size2; ++j)
        {
            aheadIdx = quest->m_vec2c[i][j];
            if (!clear.isClearedQuest(aheadIdx))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return 1;
        }
    }
    return 0;
}

// [b28d-32 N(P75/N93): 栈拷贝/装载序与分支块序差（88/79） | ORIG 0x0835511e]
bool QuestList::check_anti_quest(int questIdx,
                                const WongWork::CQuestClear& clear)
{
    Quest* quest = find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__,
            0x150a, "find_quest(%d)", questIdx);
        return 0;
    }
    // [b28d-solo-CListTables] ORIG 0x0835511e：具名迭代器先 C1Ev 默认构造（-0x2c）
    // 再 `it = begin()`（sret 临时 -0x3c，P50 复活）+ while 每轮调 end()（-0x28）
    // + 尾后置 `it++`（ppEi 临时 -0x14，P28）；antiIdx 为 int（-0xc，ORIG
    // `cmpl $0x752f; jle` 有符号，P196）。
    std::vector<int>::iterator it;
    it = quest->m_antiQuestList.begin();
    while (it != quest->m_antiQuestList.end())
    {
        int antiIdx = *it;
        if (antiIdx > 29999)
        {
            // [b28d-solo-CListTables] P221：全表达式临时链翻 sibling/具名对象槽方向
            // （ORIG trace 槽 -0x24 浅于 it -0x2c；具名形 4.4 恒相反）。
            cMyTrace(__PRETTY_FUNCTION__, 0x151a, 5)(
                "[%s][%d]", __PRETTY_FUNCTION__, 0x151a);
            return 0;
        }
        if (clear.isClearedQuest(antiIdx))
        {
            return 0;
        }
        it++;
    }
    return 1;
}

// [b28d-32 AE: P223 正向 if-early-return 消 goto 尾对齐 nop，31=31 逐条全等 | ORIG 0x08355284]
bool QuestList::check_clear(int questIdx, const WongWork::CQuestClear& clear)
{
    // [b28d-32] ORIG 0x08355284：`jle +0x5c`（>0x752f 走 trace 块 fall-through，
    // ≤ 直跳 body）。正向 if-early-return 形复现，goto 尾标签会多一条对齐 nop。
    if (questIdx > 0x752f)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x152f, 5);
        trace("[%s][%d]",
              __PRETTY_FUNCTION__,
              0x152f);
        return 0;
    }
    if (clear.isClearedQuest(questIdx))
    {
        return 0;
    }
    return 1;
}

// [b28d-32 N(P223/N89): 迭代器循环与岛差（113/94） | ORIG 0x08355304]
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
    allowable_questlist_as_npc(out, m_npcQuestList0, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList1, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList2, param, clear, true);
    allowable_questlist_as_npc(out, m_npcQuestList7, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList3, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList4, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList5, param, clear, false);
}

// [b28d-32 N(P183/N82): multimap 遍历重复访问与槽位差（131/109） | ORIG 0x0835556c]
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

// [b28d-32 N(P89/N134): 具名局部/STL 调用链形态差（106/85） | ORIG 0x0835589e]
void QuestList::GetDailyQuestList(std::list<int>& out)
{
    // [b28d-solo-CListTables] ORIG 0x0835589e：it 先 C1Ev 默认构造（-0x24）再
    // `it = begin()`（sret -0x3c 拷贝）、quest 函数头清零（-0xc，P171 死初始化）、
    // range 先 C1Ev（-0x34）再 `range = equal_range(...)`（临时 -0x1c + op=）、
    // cit 初始化式（-0x38）、it++/cit++ 均后置（ppEi，临时 -0x10/-0x14）。
    std::list<int>::iterator it;
    Quest* quest = 0;
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        int npcIdx = *it;
        range = m_npcQuestList7.equal_range(npcIdx);
        std::multimap<int, int>::const_iterator cit = range.first;
        while (cit != range.second)
        {
            int questIdx = cit->second;
            quest = find_quest(questIdx);
            if (quest != 0 && quest->m_field8 == 3)
            {
                out.push_back(questIdx);
            }
            cit++;
        }
        it++;
    }
}

void QuestList::GetTrainingQuestList(std::vector<int>& out, short level)
{
    // [b28c-21] ORIG 两个 jmp+nop 中继岛（P223 多岛形：goto 尾标签串）。
    if (level >= 0x47)
        goto END1;
    if (level <= 0)
        goto END2;
    {
        out = *G_CDataManager()->getDailyTrainingQuest(level);
    }
END2:;
END1:;
}

// [b28d-32 P282 修复] ORIG 0x08353fb5 `vptr+0xc` 虚槽 = CItem vtable slot 3 =
// GetItemType()（CItem.h:45，槽序 ~CItem×2/make_item/GetItemType）；旧实现用
// static getQuestItemType 手动 vtbl[3] 调用（发明符号+手动虚调，违 §4.5）已删，
// 改为真实 `item->GetItemType() == 8`。残差 N89 内联差 991/983。
bool QuestList::load_list()
{
    QuestScript script;
    Quest* quest = 0;
    sync_script::GetInstanceSyncScript()->truncate_quest_category();
    for (std::map<int, std::string>::iterator it =
             g_questScriptFileList.begin();
         it != g_questScriptFileList.end(); ++it)
    {
        char* path = (char*)it->second.c_str();
        if (path == 0)
        {
            break;
        }
        if (!importQuestScript(&script, path))
        {
            LogManager::logFormat(1, "data_manager.cpp",
                                  __PRETTY_FUNCTION__, 0x1357,
                                  "Quest Script Error: %s\n", path);
            return 0;
        }
        if (!script.m_dailySchedule.empty())
        {
            // ORIG 0x8353eb1：lea script+0x2dc 直传 AddDailSchedule
            // （vector<_DailySchedule> const&，ORIG 0x8125c3c），无逐项转换。
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
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x136f,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Exist\n",
                        path, mit->m_itemIdx);
                    return 0;
                }
                if (!(item->is_stackable() && item->GetItemType() == 8))
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                            1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
            cMyTrace trace(__PRETTY_FUNCTION__, 0x13ff, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x13ff);
            return 0;
        }
        QuestScript script2(script);
        quest->set_quest(script2);
        insert_NPC(script.m_npcIdx);
        m_questMap.insert(std::make_pair(script.m_index, quest));
        if (script.m_questType == 4)
        {
            m_npcQuestList2.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 5)
        {
            m_npcQuestList1.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 0)
        {
            m_npcQuestList0.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 2)
        {
            m_npcQuestList4.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 6)
        {
            m_npcQuestList5.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 3)
        {
            m_npcQuestList7.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 1)
        {
            m_npcQuestList3.insert(
                std::make_pair(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 8)
        {
            m_npcQuestList6.insert(
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
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x145c, "Mail Quest Error (quest index : %d)",
                        questIdx);
                    return 0;
                }
                if (q->m_mailTitle.size() == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x1462,
                        "Mail Quest Error! mail title err(quest index : %d)",
                        questIdx);
                    return 0;
                }
                if (q->m_mailContents.size() == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x1467,
                        "Mail Quest Error! mail contents err(quest index : %d)",
                        questIdx);
                    return 0;
                }
                if ((q->m_mailItems.size() & 1) != 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
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
    LogManager::logFormat(1, "data_manager.cpp", __PRETTY_FUNCTION__,
                          0x1496,
                          "WongWork::CQuestClear::syncScript() fail");
    return 0;
}
