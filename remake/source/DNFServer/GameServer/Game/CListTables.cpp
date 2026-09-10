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
    char m_pad8[0xc];                       // +0x08..+0x14
    int m_field14;                          // +0x14（ORIG 0x083517a9 单条 disp 读）
    // [b28d-solo-CListTables 真语义 bug 修复] ORIG init 0x083513e4/0x0835143b：
    // need_sp/need_sfp 循环访问 script+0x18 / script+0x24（Ghidra avStack_354/
    // avStack_348，相对 STSkillScript 基址 +0x18/+0x24）；旧镜像把这两个 vector
    // 塞进 pad，并把循环错指向 +0x44/+0x50（pre-skill 索引表）⇒ 读错字段。
    std::vector<int> m_needSp;              // +0x18
    std::vector<int> m_needSfp;             // +0x24
    int m_levelLimit;                       // +0x30
    int m_field34;                          // +0x34
    int m_field38;                          // +0x38
    int m_field3c;                          // +0x3c
    int m_field40;                          // +0x40
    std::vector<int> m_vec44;               // +0x44
    std::vector<int> m_vec50;               // +0x50
    int m_skillType;                        // +0x5c（ORIG 0x0835113d `mov; movsbl %al`
                                            //  = int 字段转 char 形参；权威头 m_5c 亦 int）
    int m_group;                            // +0x60
    int m_fitness[6];                       // +0x64
    int m_typeMaxLevel[6];                  // +0x7c
    // [b28d-solo-CListTables] 权威 STSkillScript.h：+0x94 为 m_growType[6]，
    // 每项 {int m_0[2]; int m_8[2];}（16 字节）。ORIG 0x0835132a/0x08351390
    // 的 `shl $2; lea; add $0x24; mov -0x364(%ebp,%eax,4)` 即 m_growType[i].m_0[j]
    // / m_8[j]（0x94+16i+4j / 0x9c+16i+4j）。旧镜像用 int[12]+pad 裸偏移建模。
    struct StGrowType
    {
        int m_0[2];   // +0x00
        int m_8[2];   // +0x08
    } m_growType[6];                        // +0x94..+0xf4
    std::vector<int> m_secondGrowType;      // +0xf4
    char m_pad100[0x70];                    // +0x100..+0x170
    std::vector<STKeyCommandUnit> m_keyCmd; // +0x170
    char m_pad17c[0x2c];                    // +0x17c..+0x1a8
    std::vector<std::vector<float> >
        m_fitnessTable;                     // +0x1a8
    char m_pad1b4[0x28];                    // +0x1b4..+0x1dc
    std::vector<int> m_fitGrowType;         // +0x1dc
    bool m_stealable;                       // +0x1e8（ORIG SetStealable 实参直读，无 setne）
    char m_pad1e9[7];
    // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x08351192 读 script+0x1f4 传
    // CSkill::SetStealLevelPenalty、0x083511a7 读 script+0x1f0 传 SetStealSPPenalty
    // （nm: 0x08374b48=SetStealLevelPenalty / 0x08374b5a=SetStealSPPenalty）
    // ⇒ 旧镜像两字段命名互换，导致 Level/SP 惩罚值互换。
    int m_stealSPPenalty;                   // +0x1f0
    int m_stealLevelPenalty;                // +0x1f4
    char m_pad1f8[0xc];                     // +0x1f8..+0x204
    std::vector<int> m_vec204;              // +0x204
    std::vector<int> m_vec210;              // +0x210
    char m_pad21c[0x44];                    // +0x21c..+0x260
    // [b28d-solo-CListTables] ORIG 0x083510ec `movzbl -0x108; movzbl %al`（传
    // reform_ui_group_no 的 bool 形参，无 setne）⇒ 该字段按 bool 直读。
    bool m_field260;                        // +0x260
    char m_pad261[0x1f];                    // +0x261..+0x280
    char m_field280;                        // +0x280
    // [b28d-solo-CListTables sizeof 修复] 权威 STSkillScript.h 尾部 +0x284..+0x2ac：
    // 旧镜像截断到 +0x281（sizeof 0x284），ORIG 栈对象间隔 0x2ac（script -0x368 /
    // s2 -0x614 / s3 -0x8c0 / s4 -0xb6c，差恒 0x2ac）⇒ 全帧槽位整体漂移 0x28。
    std::vector<int> m_vec284;              // +0x284
    char m_field290;                        // +0x290
    std::vector<int> m_vec294;              // +0x294
    std::vector<int> m_vec2a0;              // +0x2a0
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
    // [b28d-solo-CListTables] 补足权威 sizeof（STMonsterScript.h 尾部注释 0x1cc4；
    // ORIG load_monsters 帧 0x1d1c / script 槽 -0x1d0c 实证），此前截断到 0x3f0
    // 导致栈帧仅 0x44c。
    char m_pad3f0[0x1cc4 - 0x3f0];            // +0x3f0..0x1cc4
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
    // [b28d-solo-CListTables 真字段偏移修复] 无此 pad 时 m_mapIdx2 落 +0x154，
    // 其后所有字段（m_questDataType +0x178 / m_questData +0x180 / m_dailySchedule
    // +0x2dc）整体少 4 ⇒ 读错字段（ORIG 0x0835408a 用 script+0x178、
    // 0x08354193 用 script+0x180、0x08353e9b 用 script+0x2dc 实证）。
    char m_pad154[4];                       // +0x154..+0x158
    int m_mapIdx2;                          // +0x158
    char m_pad15c[0x1c];                    // +0x15c
    int m_questDataType;                    // +0x178
    int m_questDataSubType;                 // +0x17c
    std::vector<int> m_questData;           // +0x180
    char m_pad18c[0xcc];                    // +0x18c..+0x258
    char m_mailQuestFlag;                   // +0x258
    char m_pad259[0x83];                    // +0x259..+0x2dc
    std::vector<_DailySchedule> m_dailySchedule;  // +0x2dc（ORIG size/AddDailSchedule 实证）
    // [b28d-solo-CListTables sizeof 修复] ORIG load_list 帧 0x76c / script@-0x734
    // ⇒ sizeof = 0x314（旧镜像 pad 到 0x364 使全帧 +0x50、浅区整体深移）。
    char m_pad2e8[0x24];                    // +0x2e8..+0x30c
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

extern bool ImportSkillScript(STSkillScript* script, const char* path,
                             ENUM_CHARACTERJOB job, int type, int flag);
// [b28d-solo-CListTables P248档①/P292] ORIG 调用点 0x0834d408 `call; xor $1;
// test %al; je`（无 setne 物化）⇒ 返回 bool（返回类型不入 mangled）。
extern bool ImportDungeonScript(STDungeonScript* script, const char* path);
extern bool ImportMonsterScript(STMonsterScript* script, int type,
                               const char* path, bool flag);
// [b28d-solo-CListTables P248档①] ORIG 0x08353e51 `call; xor $1; test %al`（无
// setne）⇒ bool。SweepSTQuestScript 已把 STQuestScript.h:98 / .cpp:481 定义改
// bool，本 TU 声明同步（返回类型不入 mangled，零 ABI 风险）。
extern bool importQuestScript(QuestScript* script, const char* path);

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
    // [b28d-solo-CListTables] ORIG 0x0834d846：`while (it != end)` 底部条件形
    // （4.1 把条件块置循环尾：+0x74 end()/operator!=/jne 体；相等命中直接
    // `return it->second`，遍历完落 `mov $0,%eax`）。旧写 `while(true){ if(it==end)
    // return 0; ... break; }` 多 1 条对齐 nop 且块序相反。
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it;
    int count = 0;
    it = m_dungeonMap.begin();
    while (it != m_dungeonMap.end())
    {
        if (idx == count)
        {
            return it->second;
        }
        count++;
        it++;
    }
    return 0;
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

// [b28d-solo-CListTables 修复] ORIG 0x0834d6cc：69/69（gap=0）。已修：it 先 C1Ev
// 再 `it = find(idx)`（P50）、insert 改 value_type 直构 `pair<const int,CDungeon*>(
// dungeon->get_index(), dungeon)`（ORIG 0x0838a848 C2IiRS2_EEOT_OT0_；旧 make_pair
// 多出 make_pair + 转换 ctor 两个 EXTRA call，p167 现 CLEAR）。
// [round1-L29 2026-09-10 修复] ①end 改内联 `if (it == m_dungeonMap.end())`（ORIG 的 end 无具名槽）；
//   ②`int idx;` 前置声明（先声明浅槽 -0x2c）+ 赋值语句后置到 it 声明之后（保 C1Ev→get_index 调用序）
//   ⇒ 69=69 IDENTICAL（此前 NEAR）。
bool CDungeonList::insert_dungeon(CDungeon* dungeon)
{
    // [b28d-solo-CListTables] ORIG 0x0834d6d2：迭代器先 C1Ev 默认构造（-0x34）
    // 再 `it = find(idx)`（sret -0x40 拷贝，P50）；insert 用 value_type 直构
    // `pair<const int,CDungeon*>(get_index(), dungeon)`（ORIG 0x0838a848 实例
    // `C2IiRS2_EEOT_OT0_`，且 key 是**重新调用** get_index()，非缓存 idx）。
    int idx;
    __gnu_cxx::hash_map<int, CDungeon*>::iterator it;
    idx = dungeon->get_index();
    it = m_dungeonMap.find(idx);
    if (it == m_dungeonMap.end())
    {
        m_dungeonMap.insert(
            __gnu_cxx::hash_map<int, CDungeon*>::value_type(
                dungeon->get_index(), dungeon));
        return true;
    }
    LogManager::logFormat(
        1, "data_manager.cpp", __PRETTY_FUNCTION__,
        0x921, "Already registed dungeon index detected : %d",
        dungeon->get_index());
    return false;
}

// [b28d-solo-CListTables 修复] ORIG 0x0834d376：178/178（gap=0）。已修：①it 先
// C1Ev 再 `it = begin()` + 尾部后置 it++（P50/P28）；②ImportDungeonScript 声明
// int→bool（ORIG 0x0834d408 `call; xor $1; test %al`，定义侧 STDungeonScript.cpp
// 已 bool）；③cMyTrace 改全表达式临时链（P227，size() 结果入 %ebx 跨 ctor 保活）。
// 残差仅帧 0x55c vs 0x56c + script/it/cMyTrace 槽位差（N82 族）⇒ 封顶。
bool CDungeonList::load_list()
{
    char* path = 0;
    STDungeonScript script;
    CDungeon* dungeon = 0;
    // [b28d-solo-CListTables] ORIG 0x0834d39e：迭代器先 C1Ev 默认构造（-0x3c）
    // 再 `it = begin()`（P50 复活）；尾部后置 it++（ppEi，sret -0x24，P28）。
    std::map<int, std::string>::iterator it;
    it = g_stDungeonScriptFileList.begin();
    while (it != g_stDungeonScriptFileList.end())
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
            // [b28d-solo-CListTables P227] ORIG 0x0834d532：size() 结果先入 %ebx
            // 再跨 cMyTrace C1 保活（`mov %eax,%ebx` 在 ctor 前）⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x8ec, 5)(
                "Best Clear Time Save Dungeon Index Over Error: %d\n",
                size());
            return 0;
        }
        it++;
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

// [b28d-solo-CListTables AE] ORIG 0x0834f08c：真语义 bug 修复——尾循环三组 clear
// 第一组旧写 m_mapList[i+1]（漏清 index 0、多清 index 15），ORIG 目标为
// this+i*0x14+0x14/+0x154/+0x294（Ghidra C 同步）= m_mapList[i] ⇒ 修正后 82=82 AE。
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
    // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x0834f11c 三组 clear 目标为
    // this+i*0x14+0x14/+0x154/+0x294（Ghidra C 同步）⇒ 第一组是 m_mapList[i]；
    // 旧实现写 m_mapList[i+1] 既漏清 m_mapList[0] 又多清 m_mapList[15]（越界 1 元素）。
    for (int i = 0; i <= 14; ++i)
    {
        m_mapList[i].clear();
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

// [b28d-solo-CListTables AE] ORIG 0x0834f18e：五处修复——①m_mapList[channel+1]→
// [channel]（ORIG 0x0834f357 目标 this+0x14*channel+0x14，真语义 bug）；②循环迭代器
// 改 const_iterator（P96，ORIG 有 C1<const PVPMAP_DATA*> 转换构造）；③get_map_type()
// 两次内联调用（p167 MISSING 1）；④mapPvpMapInfo_[count] = info 结构赋值复现单
// operator[] + 自比较 + memcpy(5)；⑤(*it).m_mapIdx 替 it->（deEv vs ptEv）。
// p167 25/25 CLEAR ⇒ 202=202 AE。
int CMapList::insert_map(int channel, int key, CMap* map,
                         std::vector<PVPMAP_DATA>& pvpData)
{
    // [b28d-solo-CListTables] 范围检查为内联 get_index()（无具名槽，P0 右值直传）。
    if (find_map(map->get_index()) != 0)
    {
        return 0;
    }
    // [b28d-solo-CListTables] ORIG 0x0834f18e：范围检查为内联表达式（每次重调
    // get_index()，无具名槽往返；P0 右值直传）。
    if (map->get_index() > 0xc350 && map->get_index() <= 0xc738)
    {
        m_pvpMapCount++;
        PvPMapInfo info;
        info.m_mapIdx = map->get_index();
        int count = 0;
        // [b28d-solo-CListTables P96/P270] ORIG 0x0834f25a 迭代器为 const_iterator
        // （begin() 结果走 normal_iterator<const PVPMAP_DATA*> 转换构造 call）。
        for (std::vector<PVPMAP_DATA>::const_iterator it = pvpData.begin();
             it != pvpData.end(); ++it)
        {
            // [b28d-solo-CListTables P167] ORIG 用 operator*（deEv）非 operator->（ptEv）。
            if ((*it).m_mapIdx == info.m_mapIdx)
            {
                break;
            }
            count++;
        }
        // [b28d-solo-CListTables] ORIG 0x0834f2d0：单次 operator[] + 自比较守卫 +
        // memcpy 5 —— 4.4 对 5 字节结构赋值即产同形（同 GetPvpMapInfo）。
        mapPvpMapInfo_[count] = info;
    }
    // ORIG 0x0834f2ed..0x0834f42e：4 处插入各只有两次 call —— 先就地构造
    // hash_map 的 value_type（pair<const int,CMap*>），再 insert(value_type const&)：
    //   +0x17b _ZNSt4pairIKiP4CMapEC1IiRS2_EEOT_OT0_   （第一实参为 get_index() 右值）
    //   +0x1c4/+0x226/+0x272 …C1IRiRS2_…            （第一实参为左值 key）
    // 用 make_pair 会多出 pair<int,CMap*> → value_type 的转换临时（第三次 call）。
    m_mainMap.insert(
        __gnu_cxx::hash_map<int, CMap*>::value_type(map->get_index(), map));
    // [b28d-solo-CListTables P167/P183] ORIG 调 get_map_type() 两次（无具名 mapType 局部）。
    if (map->get_map_type() == 0)
    {
        // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x0834f357 目标地址
        // this+0x14*channel+0x14 = m_mapList[channel]（旧实现 channel+1 偏移错一格）。
        m_mapList[channel].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    else if (map->get_map_type() == 2)
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

// [b28d-solo-CListTables AE] ORIG 0x0834f5c8：`info = it->second;` 结构赋值即复现
// ORIG 的单次 ptEv + 寄存器内比较直跳 + memcpy(5)（4.4 对 5 字节 POD 赋值同形）；
// 旧 `if (&info != &it->second) memcpy(&info,&it->second,5)` 两次 ptEv + setne
// 物化（P197④ 镜像）⇒ 38=38 AE。
bool CMapList::GetPvpMapInfo(int idx, PvPMapInfo& info)
{
    // [b28c-21 代落 C28L35 spec] 初始化式直构：ORIG find 直接落 -0x10，无 C1Ev 默认构造。
    std::map<int, PvPMapInfo>::iterator it = mapPvpMapInfo_.find(idx);
    if (it != mapPvpMapInfo_.end())
    {
        // [b28d-solo-CListTables] 结构赋值即复现 ORIG 单次 ptEv + 直跳比较 + memcpy(5)。
        info = it->second;
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

// [b28d-solo-CListTables N98/N93 NEAR] ORIG 0x0834fdee：54=54 助记符全等，仅
// it/end 槽互换（ORIG it -0x2c/end -0x24 vs 4.4 it -0x24/end -0x2c）。it 为先
// 声明后赋值（P50 复活，ORIG 单次 C1Ev），end 为初始化式直构；全头部声明多出
// 第二个 C1Ev（+5 条），交换声明序又改执行序 ⇒ 4.4 按首次使用序分配不可翻，封顶。
// [round1-L29 2026-09-10 修复] end 改内联 `if (it == m_monsterMap.end())`（ORIG end 无具名槽，
//   it/end 两具名对象槽互换消失）⇒ 54=54 IDENTICAL（此前 NEAR）。
bool CMonsterManager::insert_monster(int level, CMonster* monster)
{
    // [b28d-32] ORIG 0x0834fdee：迭代器先默认构造（C1Ev）再 find 赋值
    // （P50 复活 + find 结果 8 字节拷贝到 it 槽）；insert 用显式
    // pair<const int,CMonster*>(int&,CMonster*&) 直构，无 make_pair 中转。
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.find(level);
    if (it == m_monsterMap.end())
    {
        m_monsterMap.insert(std::pair<const int, CMonster*>(level, monster));
        return true;
    }
    return false;
}

// [b28d-solo-CListTables N(N82) DIFF] ORIG 0x0834fa84：215/215（gap=0）+ p167
// CLEAR(34/34，差集全为 C1/C2·D1/D2 别名）。已修：truncate 负形早退复现
// `xor $1; test; je`（P168a）、drops 下标 unsigned→int 复现 `setg`（P196）。
// 残差 8 处槽位差（unusedIt -0x38 vs -0x3c、STMonsterScript 基址 ±4、path==0
// 分支 je/jne 布局）：sizeof 0x1cc0/0x1cc4 两探针均不可同时对齐 ⇒ N82 槽位族封顶。
// [round1-L29 2026-09-10 部分修复] 判据⑩：`unusedIt` 移到 it 声明之后（后声明者深）⇒
//   -0x38(5)/-0x3c(1) 与 ORIG 逐值对齐。剩 `script` 基址 -0x1918 vs ORIG -0x1910（±8）+ 1 条 gap
//   ⇒ N126/N82 族；script 声明前移试验反劣（ndiff 15→22，已回退）。
bool CMonsterManager::load_monsters()  // [P330 int→bool] 与 CDataManager.h 声明同步（ORIG caller xor $1 形）
{
    // [b28d-solo-CListTables P168a] ORIG 0x0834fa8e `call; xor $1; test %al,%al;
    // je 主体` = 负形早退（失败分支 fall-through 到 puts/return 0）。
    if (!sync_script::GetInstanceSyncScript()->truncate_monster_info_table())
    {
        puts("Fail truncate_monster_info_table");
        return 0;
    }
    {
        char* path = 0;
        CMonster* monster = 0;
        // [b28d-solo-CListTables] ORIG：主循环 map 迭代器先 C1Ev 后 begin() 赋值
        // （P50）+ 尾后置 it++（ppEi，P28）。
        std::map<int, std::string>::iterator it;
        it = g_stMonsterScriptFileList.begin();
        // [b28d-solo-CListTables] ORIG 有一处未使用的 list<int>::iterator C1Ev
        // （p167 MISSING 1；P171/P232 死局部照抄）。声明序按判据⑩（后声明者深）。
        std::list<int>::iterator unusedIt;
        while (it != g_stMonsterScriptFileList.end())
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
            // [b28d-solo-CListTables P196/P218] ORIG 0x0834fc48 `cmp i; setg` 有符号
            // 比较（unsigned i 生 seta）⇒ 下标局部 int 化。
            for (int i = 0; i < (int)drops.size(); ++i)
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
            it++;
        }
        return 1;
    }
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

// [b28d-solo-CListTables N82/N93 NEAR] ORIG 0x083519a6：125=125 + p167 CLEAR(14/14)。
// 真修复：分支体反写——ORIG `call eq; je trace 尾块`（it==end 走 insert fall-through）
// ⇒ 源码须 `if (it == end) { insert; return; } trace;`（旧 `if (it != end) { trace;
// return; }` 令 4.4 选 ne 实例并反转块序）。残差为 idx/it/end/trace1/trace2 槽分配
// 差（ORIG trace1 -0x54/trace2 -0x18 分散，4.4 集中 -0x60/-0x50）⇒ 封顶。
// [round1-L29 2026-09-10 复核·判据⑪ 槽直方图] 我方槽集合 = ORIG 槽集合 + 0x10 常数平移
//   （ours -0x50/-0x48/-0x44… vs ORIG -0x60/-0x58/-0x54…，逐值间距全同）⇒ **N126 版本首槽基址
//   常数**，声明重排无效（按判据⑪ 未耗编译轮次）。trace1 已改全表达式临时链（P227）
//   使平移由段内错位归一为单一常数。剩 N126 封顶（ORIG 0x083519a6）。
void CSkillList::insert_skill(int jobType, CSkill* skill)
{
    // [b28d-solo-CListTables] it 先 C1Ev 后 find 赋值（P50）已对齐；残差见函数头判词。
    if (skill == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xf91, 5)(
            "[%s][%d]_(0 == pSkill)",
              __PRETTY_FUNCTION__, 0xf91);
        return;
    }
    int idx = skill->get_index();
    // [b28d-solo-CListTables] ORIG 0x083519a6：it 先 C1Ev 默认构造（-0x60）再
    // `it = find()`（sret -0x70 拷贝，P50 复活）；end 初始化式（-0x44）。
    __gnu_cxx::hash_map<int, CSkill*>::iterator it;
    it = m_skillList[jobType].find(idx);
    // [b28d-solo-CListTables] ORIG 0x08351a82 调 operator==（eq）后 `je trace`：
    // 相等走 insert fall-through、不等落 trace 尾块 ⇒ 源码必须是正向
    // `if (it == end) { insert; return; } trace;`（写 `if (it != end) { trace;
    // return; }` 会让 4.4 选 ne 实例并反转块序）。
    // ORIG 0x08351aa3/0x08351af5：两处插入各只有两次 call —— 就地构造 value_type
    //   +0x0fd _ZNSt4pairIKiP6CSkillEC1IRiRS2_EEOT_OT0_（idx 左值）→ hash_map::insert
    //   +0x14f _ZNSt4pairIKiP6CSkillEC1IiRS2_EEOT_OT0_ （get_group() 右值，无中间变量）
    //          → multimap::insert；make_pair 会多出转换临时（第三次 call）。
    if (it == m_skillList[jobType].end())
    {
        m_skillList[jobType].insert(
            __gnu_cxx::hash_map<int, CSkill*>::value_type(idx, skill));
        m_groupSkillMap[jobType].insert(
            std::multimap<int, CSkill*>::value_type(skill->get_group(), skill));
        return;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0xfa4, 5);
    trace("[%s][%d]_( itr != m_SkillList[job_type].end() )",
          __PRETTY_FUNCTION__, 0xfa4);
}

// [b28d-solo-CListTables N82 NEAR] ORIG 0x08351c1e：138=138 + p167 17/17 CLEAR
// （== / mmEi / ppEi 全对齐），残差仅槽号整体深移 0x10（ORIG it/range -0x4c/-0x48
// vs 我们 -0x3c/-0x38）；char[0x10] 死局部撑帧实测 sub 0x58→0x78 且槽号不平移 ⇒ 封顶。
int CSkillList::get_ran_groupskill_index(int jobType, int group, int skillIdx)
{
    // [b28d-solo-CListTables N(N82): 138=138 + p167 调用点 CLEAR（17/17，== / mmEi /
    // ppEi 全对齐），残差仅槽号整体深移 0x10（ORIG it/range -0x4c/-0x48 vs 我们
    // -0x3c/-0x38）；char[0x10] 死局部撑帧实测 sub 0x58→0x78 且槽号不平移 ⇒ 封顶。]
    int count = m_groupSkillMap[jobType].count(group);
    if (count == 0)
    {
        return -1;
    }
    int rand = get_rand_int(count);
    // [b28d-solo-CListTables] ORIG 0x08351c1e：range 声明为 const_iterator 对
    // ⇒ equal_range 结果走 pair 转换构造（ORIG call ...C1ISt17_Rb_tree_iterator…）；
    // for 内后置 it++（ppEi 带临时槽）。
    std::pair<std::multimap<int, CSkill*>::const_iterator,
              std::multimap<int, CSkill*>::const_iterator> range =
        m_groupSkillMap[jobType].equal_range(group);
    std::multimap<int, CSkill*>::const_iterator it = range.first;
    for (int i = 0; i < rand; ++i)
    {
        it++;
    }
    CSkill* skill = it->second;
    if (skill->get_index() == skillIdx)
    {
        // [b28d-solo-CListTables] ORIG 0x08351cea：== 正极性（ORIG 调
        // operator==，非 !=）+ then 为 it-- 分支 + 两处后置 it--（mmEi）。
        it++;
        if (it == range.second)
        {
            it--;
            if (it == range.first)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xfde, 5);
                trace("[%s][%d]_( i == res.first )",
                      __PRETTY_FUNCTION__,
                      0xfde);
            }
            else
            {
                it--;
                skill = it->second;
            }
        }
        else
        {
            skill = it->second;
        }
    }
    return skill->get_index();
}

// [b28d-solo-CListTables N(P192/N82) DIFF] ORIG 0x08351e18：163/180 + p167 有
// MISSING value_type C1<int&,vector&>、EXTRA make_pair+2×~pair+转换 ctor。
// value_type 直构实测（std::map<...>::value_type(it->m_index, vec)）虽对齐该构造
// 实例，但连带 identical -2/ae -5（他函数符号集变化）⇒ 回退保持 make_pair；
// 帧 0x50 vs 0x60 + 槽整体深 0x10 属 N82 ⇒ 封顶。
void CSkillList::set_post_learning_skill(int jobType, CSkill* skill)
{
    // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x08351e62/0x08351e94：find/insert
    // 的 map key 取 `it->+0x00`（ptEv 结果直传，_pre_skill::m_index），旧实现误用
    // m_jobType(+0x04)（get_post_learning_skill 亦以 skillIdx 为 key 互证）。
    // value 为 pair<int,int> C1Ev 后逐字段赋值（-0x38/-0x34）；&& 拆嵌套 if 复现
    // 4.1 的 `setne %al; test` 第二条件物化（P225 推广）。
    if (skill != 0)
    {
        if (skill->m_preSkillList.size() != 0)
        {
            for (std::list<_pre_skill>::iterator it =
                     skill->m_preSkillList.begin();
                 it != skill->m_preSkillList.end(); ++it)
            {
                std::map<int, std::vector<std::pair<int, int> > >::iterator
                    found = m_postLearningMap[jobType].find(it->m_index);
                std::pair<int, int> value;
                value.first = skill->get_index();
                value.second = it->m_jobType;
                if (found == m_postLearningMap[jobType].end())
                {
                    std::vector<std::pair<int, int> > vec;
                    vec.push_back(value);
                    // [b28d-solo-CListTables N(P192) DIFF] ORIG 调 value_type 直构
                    // `pair<const int,vector<pair<int,int>>>::C2IRiRS4_EEOT_OT0_`，
                    // 但本 TU 改 value_type 实测 identical -2 / ae -5（他函数符号集
                    // 变化）⇒ 回退保持 make_pair（batch-25c 结论复证）。
                    m_postLearningMap[jobType].insert(
                        std::make_pair(it->m_index, vec));
                }
                else
                {
                    found->second.push_back(value);
                }
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

// [b28d-solo-CListTables 修复] ORIG 0x08350c26：866/866（gap=0）+ p167 CLEAR(111/111，
// 差集全为 C1/C2·D1/D2 别名）。本轮真修复：①STSkillScript 镜像补 m_needSp(+0x18)/
// m_needSfp(+0x24)（旧实现把 need_sp/need_sfp 循环错指向 +0x44/+0x50 = pre-skill
// 索引表，**读错字段**）；②镜像尾部补 m_vec284/m_290/m_vec294/m_vec2a0（sizeof
// 0x284→0x2ac，ORIG 对象间隔 0x2ac 实证）；③m_stealLevelPenalty/m_stealSPPenalty
// 偏移互换修正（ORIG SetStealLevelPenalty 读 +0x1f4、SetStealSPPenalty 读 +0x1f0）；
// ④m_growType[6] 具名子对象替代裸偏移（ORIG `shl $2; lea; add $0x24; mov -0x364(,%e,4)`）；
// ⑤need_sp/sfp、learned、vec204/210 循环 size 缓存+int 比较、secondGrowType 嵌套 if（P225）、
// m_stealable bool、pre.m_group 删发明写入（P171）、主循环 it 提函数头（ORIG C1Ev 仅一次）、
// 死计数器 -0x78 照抄、push_back(int&&) 临时右值（ORIG 0x0808e1ee 实证）。
// 残差为 N7 寄存器分配/N82 槽位族 ⇒ 语义完成、byte 层封顶。
bool CSkillList::init()  // [P330 int→bool] 与 CDataManager.h 声明同步（ORIG caller xor $1 形）
{
    // [b28d-solo-CListTables] 866/866 + p167 CLEAR(111/111) ⇒ gap=0。
    STSkillScript script;
    CSkill* skill = 0;
    sync_script::GetInstanceSyncScript()->truncate_skill_index();
    sync_script::GetInstanceSyncScript()->trancate_skill_info();
    // [b28d-solo-CListTables] ORIG 0x08350c40/0x08350c61：-0x78 与 path(-0x74) 两处
    // 清零；-0x78 在内层循环尾每轮 +1（0x083517f4）且全函数无读取点 ⇒ 原码遗留死
    // 计数器（P171 照抄死写入）。
    int scriptCount = 0;
    // [b28d-solo-CListTables] ORIG 0x08350cbc 存 c_str() 指针于 -0x74 并 `cmpl $0`
    // ⇒ path 是 char*（旧实现用 int 存指针 + cast，实参装载序与 ORIG 相反）。
    const char* path = 0;
    // [b28d-solo-CListTables P309] ORIG 深区槽序：it(-0xa8) 深于 pre(-0xa4..-0x9c)
    // ⇒ 4.4 函数头声明「先声明浅」，pre 声明在 it 之前即可复现（pre 为 POD，
    // 提头无构造副作用；循环内仅赋值）。
    _pre_skill pre;
    // [b28d-solo-CListTables] ORIG 0x08350c78：主循环 map 迭代器 C1Ev 只在函数头
    // 执行一次（循环外声明，-0xa8）；循环体内仅 begin() 赋值。
    std::map<int, std::string>::iterator it;
    int job;
    for (job = 0; job <= 10; ++job)
    {
        // [b28d-solo-CListTables] ORIG：循环体内 begin() 赋值 + 尾后置 it++（ppEi，P28）。
        it = g_staSkillScriptFileList[job].begin();
        while (it != g_staSkillScriptFileList[job].end())
        {
            path = it->second.c_str();
            if (path == 0)
            {
                break;
            }
            if (!ImportSkillScript(&script, path, (ENUM_CHARACTERJOB)job, 0, -1))
            {
                printf("Skill Script Error: %s\n", path);
                return 0;
            }
            if (!sync_script::GetInstanceSyncScript()
                     ->insert_skill_index(&script, job))
            {
                printf("insert_skill_index Error: %s\n", path);
                return 0;
            }
            if (sync_script::GetInstanceSyncScript()->IsEnable())
            {
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&script, job, 0))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
                STSkillScript s2;
                // [b28d-solo-CListTables] ORIG 0x08350e20/0x08350e6f：Import 与
                // insert_skill_info 各自独立 printf（两条消息串不同）。
                if (!ImportSkillScript(&s2, path, (ENUM_CHARACTERJOB)job, 4, -1))
                {
                    printf("Skill Script Error: %s\n", path);
                    return 0;
                }
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s2, job, 4))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
                STSkillScript s3;
                if (!ImportSkillScript(&s3, path, (ENUM_CHARACTERJOB)job, 6, -1))
                {
                    printf("Skill Script Error: %s\n", path);
                    return 0;
                }
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s3, job, 6))
                {
                    printf("insert_skill_info Error: %s\n", path);
                    return 0;
                }
                STSkillScript s4;
                if (!ImportSkillScript(&s4, path, (ENUM_CHARACTERJOB)job, 11, -1))
                {
                    printf("Skill Script Error: %s\n", path);
                    return 0;
                }
                if (!sync_script::GetInstanceSyncScript()
                         ->insert_skill_info(&s4, job, 11))
                {
                    printf("insert_skill_info Error: %s\n", path);
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
            // [b28d-solo-CListTables] ORIG 0x08351179 `movzbl; movzbl %al` 直传
            // （无 setne）⇒ m_stealable 为 bool 字段、实参直读。
            skill->SetStealable(script.m_stealable);
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
            // [b28d-solo-CListTables] ORIG 0x08351307 `cmpl $0x1; setbe` ⇒ unsigned。
            for (unsigned int i = 0; i <= 1; ++i)
            {
                // [b28d-solo-CListTables P225 推广] ORIG 两条件各自 `setX; test; je`
                // （0x083512aa/0x083512c7）⇒ 拆嵌套 if，`&&` 短路会被 4.4 折成直跳。
                if (!script.m_secondGrowType.empty())
                {
                    if ((unsigned int)i < script.m_secondGrowType.size())
                    {
                        skill->set_fitness_second_growtype(
                            i + 1, script.m_secondGrowType[i]);
                    }
                }
            }
            for (int i = 0; i <= 5; ++i)
            {
                for (int j = 0; j <= 1; ++j)
                {
                    skill->set_second_growtype_fitness(
                        i, j + 1, script.m_growType[i].m_0[j]);
                }
            }
            for (int i = 0; i <= 5; ++i)
            {
                for (int j = 0; j <= 1; ++j)
                {
                    skill->set_second_growtype_max_level(
                        i, j + 1, script.m_growType[i].m_8[j]);
                }
            }
            // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x083513de/0x0835143b：
            // need_sp 读 script+0x18、need_sfp 读 script+0x24（旧实现错读 +0x44/+0x50）。
            // 循环形：size 先缓存具名局部（-0x6c/-0x68），比较 `setl`（signed）。
            int needSpCount = script.m_needSp.size();
            for (int i = 0; i < needSpCount; ++i)
            {
                skill->add_need_sp(script.m_needSp.at(i));
            }
            int needSfpCount = script.m_needSfp.size();
            for (int i = 0; i < needSfpCount; ++i)
            {
                skill->add_need_sfp(script.m_needSfp.at(i));
            }
            int preSkillCount = script.m_vec44.size();
            for (int i = 0; i < preSkillCount; ++i)
            {
                pre.m_index = script.m_vec44.at(i);
                pre.m_jobType = script.m_vec50.at(i);
                // [P171 照抄] ORIG 0x083514fd 直读栈槽 -0x9c（= pre+0x08）且全函数
                // 无写入点 ⇒ 原码未给 m_group 赋值，不得补写（旧实现多一条 store）。
                skill->add_learned_skill(pre);
            }
            int guildGspCount = script.m_vec204.size();
            for (int i = 0; i < guildGspCount; ++i)
            {
                // [b28d-solo-CListTables] ORIG 0x08111126 = push_back(int const&)
                // （nm 实证）；实参为 at() 返回的 int&（`mov %eax,0x4(%esp)`），
                // 旧实现用 static_cast<int&&> 会多一次解引用+临时槽。
                skill->m_guildSkillNeedGSP.push_back(script.m_vec204.at(i));
            }
            int guildGoldCount = script.m_vec210.size();
            for (int i = 0; i < guildGoldCount; ++i)
            {
                skill->m_guildSkillNeedGold.push_back(script.m_vec210.at(i));
            }
            skill->m_needGuildLevel = script.m_field3c;
            skill->m_needGuildLevelAdd = script.m_field40;
            skill->m_field104 = script.m_field14;
            for (unsigned int i = 0; i < script.m_fitnessTable.size(); ++i)
            {
                std::vector<int> vec;
                for (unsigned int j = 0;
                     j < script.m_fitnessTable[i].size(); ++j)
                {
                    // [b28d-solo-CListTables] ORIG 0x083516a3 调 0x0808e1ee =
                    // vector<int>::push_back(int&&)（nm 实证）：实参是 ROUND 截断的
                    // **临时右值**（先 `mov %eax,-0x94` 再传 &-0x94）⇒ 原码内联
                    // 临时表达式，具名 int 局部会改选 push_back(int const&) 并丢
                    // 3 个弱实例符号（AE 账 -3）。
                    vec.push_back((int)script.m_fitnessTable[i].at(j));
                }
                skill->m_field108.push_back(vec);
            }
            skill->m_isSpecialSkill = script.m_field260;
            skill->m_keyCommandList = script.m_keyCmd;
            // [b28d-solo-CListTables] CSkill.h:136 具名字段（ORIG 0x08351779
            // `movzbl -0xe8; mov %dl,0x130` 单条 disp store）。
            skill->m_isCommandCustomizing = script.m_field280;
            skill->setSkillName(script.m_name.c_str());
            skill->m_pvpSkillGrade = script.m_field38;
            insert_skill(job, skill);
            it++;
            scriptCount++;
        }
    }
    for (int i = 0; i <= 10; ++i)
    {
        // [b28d-solo-CListTables] ORIG：尾部循环迭代器先 C1Ev 后 begin() 赋值
        // （P50）+ 条件区每轮重调 end()（sret -0x8c，无具名 end 缓存）+ 后置
        // ++it（ppEi，P28）。
        __gnu_cxx::hash_map<int, CSkill*>::iterator it;
        it = m_skillList[i].begin();
        while (it != m_skillList[i].end())
        {
            skill = it->second;
            skill->set_prelearned_skill();
            set_post_learning_skill(i, skill);
            it++;
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
bool QuestList::Check_Quest(Quest* quest, stSelectQuestParam& param,
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

// [b28d-solo-CListTables AE] ORIG 0x08355304：113=113——①quest=0 声明提到
// out.clear() 之前（ORIG `movl $0x0,-0xc` 先于 clear）；②`return !out.empty()`
// 拆成 `if (out.empty()) return false; return true;`（P56，消 4 条 bool 物化）；
// ③questIdx 全头部声明（P309/P212，it/questIdx 槽序对齐）。
bool QuestList::select_mail_quest(std::list<int>& out,
                                  const WongWork::CQuestClear& clear,
                                  const stSelectQuestParam& param)
{
    // [b28d-solo-CListTables] ORIG 0x08355304：quest 函数头清零（-0xc）、迭代器为
    // const_iterator（begin() 结果走 C1<Pi> 转换构造）、每条件独立 guard
    // （`xor $1; test; jne fail`，P133/P168a 不可合并 && 短路）。
    Quest* quest = 0;
    int questIdx;
    out.clear();
    for (std::vector<int>::const_iterator it = m_vecD4.begin();
         it != m_vecD4.end(); ++it)
    {
        questIdx = *it;
        quest = find_quest(questIdx);
        if (quest == 0)
            continue;
        if (!quest->check_possible(param))
            continue;
        if (!check_clear(questIdx, clear))
            continue;
        if (!check_ahead_quest(questIdx, clear))
            continue;
        if (!check_anti_quest(questIdx, clear))
            continue;
        out.push_back(questIdx);
    }
    if (out.empty())
        return false;
    return true;
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

// [b28d-solo-CListTables 修复] ORIG 0x0835556c：131/131（gap=0）+ p167 CLEAR(16/16)。
// 已修：range 提到函数头（C1Ev 在循环外）、三守卫独立 goto + 尾标签串（P223 多岛，
// ORIG +0x136/+0x139/+0x13c 三 nop 岛复现）、flag 守卫改嵌套 `if (!flag) {...}`
// 复现 ORIG `movzbl -0x3c; xor $1; test %al; je`（旧 `if (flag) goto DO_PUSH` 被
// 4.4 折成 `cmpb $0; jne`）。残差仅 it/range/param 槽位差（N82 族）⇒ 封顶。
// [round1-L29 2026-09-10 修复] 判据⑩：`int npcIdx;` 前置声明（无初值→浅槽 -0x24）+ 循环内改
//   赋值 `npcIdx = *it;`（保留 it→range→npcIdx 调用序）⇒ AE 128/128 ndiff=0（此前 NEAR）。
//   注：把 cit 也提到头部（全头部声明）反劣 131 vs 128，已回退。
void QuestList::allowable_questlist_as_npc(
    std::list<int>& out, const std::multimap<int, int>& npcList,
    stSelectQuestParam& param, const WongWork::CQuestClear& clear, bool flag)
{
    // [b28d-solo-CListTables] ORIG 0x0835556c：it 先 C1Ev 后 begin() 赋值（P50）
    // + 尾后置 it++（ppEi）；range 先 C1Ev（-0x2c，函数头）再
    // `range = equal_range(...)`（op=(&&) 赋值，同 GetDailyQuestList）。
    std::list<int>::iterator it;
    int npcIdx;
    Quest* quest = 0;
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        npcIdx = *it;
        range = npcList.equal_range(npcIdx);
        for (std::multimap<int, int>::const_iterator cit = range.first;
             cit != range.second; ++cit)
        {
            param.m_questIdx = cit->second;
            quest = find_quest(param.m_questIdx);
            // [b28d-solo-CListTables] ORIG 0x083556a2/0x083556a5/0x083556a8：三失败
            // 出口各带 nop 中继岛（+0x136/+0x139/+0x13c）⇒ P223 多岛形：独立否定
            // goto + 尾标签串（标签序 = 守卫序）。
            if (quest == 0)
                goto SKIP_NULL_QUEST;
            if (!Check_Quest(quest, param, clear))
                goto SKIP_CHECKQ_FAIL;
            // [b28d-solo-CListTables P168a/P194] ORIG 0x083556.. `movzbl -0x3c;
            // xor $1; test %al; je`（bool 形参取反物化）⇒ 写嵌套 `if (!flag)`，
            // 旧 `if (flag) goto DO_PUSH;` 被 4.4 折成 `cmpb $0; jne`（2 条）。
            if (!flag)
            {
                if (!check_clear(param.m_questIdx, clear))
                    goto SKIP_CLEAR_FAIL;
            }
        DO_PUSH:
            out.push_back(param.m_questIdx);
        SKIP_NULL_QUEST:;
        SKIP_CHECKQ_FAIL:;
        SKIP_CLEAR_FAIL:;
        }
        it++;
    }
}

// [b28d-solo-CListTables AE] ORIG 0x0835589e：106=106——①双守卫独立否定 goto +
// 尾标签串（P223，ORIG +0xe0/+0xe3 两 nop 岛复现）；②npcIdx/questIdx 全头部声明
// 按 ORIG 槽浅→深（P212/P309，range/cit 槽序对齐）。
void QuestList::GetDailyQuestList(std::list<int>& out)
{
    // [b28d-solo-CListTables] ORIG 0x0835589e：it 先 C1Ev 默认构造（-0x24）再
    // `it = begin()`（sret -0x3c 拷贝）、quest 函数头清零（-0xc，P171 死初始化）、
    // range 先 C1Ev（-0x34）再 `range = equal_range(...)`（临时 -0x1c + op=）、
    // cit 初始化式（-0x38）、it++/cit++ 均后置（ppEi，临时 -0x10/-0x14）。
    std::list<int>::iterator it;
    Quest* quest = 0;
    // [b28d-solo-CListTables P212/P309] ORIG 槽深序 quest(-0xc)/it(-0x24)/
    // npcIdx(-0x28)/questIdx(-0x2c)/range(-0x34)/cit(-0x38) ⇒ 全头部声明、
    // 声明序按 ORIG 浅→深；块内仅赋值。
    int npcIdx;
    int questIdx;
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        npcIdx = *it;
        range = m_npcQuestList7.equal_range(npcIdx);
        std::multimap<int, int>::const_iterator cit = range.first;
        while (cit != range.second)
        {
            questIdx = cit->second;
            quest = find_quest(questIdx);
            // [b28d-solo-CListTables] ORIG 0x0835597e/0x08355981：两守卫落点不同
            // （+0xe0 nop;jmp / +0xe3 nop）⇒ P223 多岛形：独立否定 goto + 尾标签串。
            if (quest == 0)
                goto SKIP_NULL_QUEST;
            if (quest->m_field8 != 3)
                goto SKIP_NOT_DAILY;
            out.push_back(questIdx);
        SKIP_NULL_QUEST:;
        SKIP_NOT_DAILY:;
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
// 改为真实 `item->GetItemType() == 8`。
// [b28d-solo-CListTables 修复 + §6.2 N 封顶] ORIG 0x08353d98：991/996（gap=5）
// + p167 CLEAR(122/122)。本轮真修复：①QuestScript 镜像 m_mapIdx2 前缺 4B pad
// ⇒ m_questDataType/+0x178、m_questData/+0x180、m_dailySchedule/+0x2dc 全体少 4
// （**读错字段**；ORIG 0x0835408a/0x08354193/0x08353e9b 实证）；②镜像 sizeof
// 0x30c（帧 0x76c + script@-0x734 实证）；③函数头构造/清零序 it C1Ev → script
// C1Ev → quest=0 → mit C1Ev → item=0；④尾部循环 q 循环外清零；⑤补 ORIG 死局部
// -0x422(int)/-0x41e(short)（P171 照抄）；⑥enemyType int+显式 unsigned 比较复现
// 两段寄存器比较（P310）。残差 6 条 = logFormat 变参 size/questDataType 的寄存器
// 分配（N7：ORIG 直载 %edx，4.4 恒多 `mov %eax,%edx`）+ N87 `and $0x1f` +
// N6 位折叠（4.4 sar+and$1 vs 4.1 shl+and$0x40e）⇒ 语义完成、byte 层封顶。
// [b28d-solo-CListTables 归零] importQuestScript 定义/声明 int→bool 已由
// SweepSTQuestScript 落盘、本 TU 声明同步 ⇒ 0x1357 调用点由 sete 物化改为
// ORIG 的 `call; xor $0x1,%eax; test %al,%al; je`（991/997→991/996）。
bool QuestList::load_list()
{
    // [b28d-solo-CListTables] ORIG 0x08353da4..0x08353dd8 函数头构造/清零序：
    // it C1Ev(-0x40c) → script C1Ev(-0x734) → quest=0(-0x410) → mit C1Ev(-0x414)
    // → item=0(-0x40) → truncate 调用 ⇒ 声明序照此排列（4.4 按声明序发射）。
    std::map<int, std::string>::iterator it;
    QuestScript script;
    Quest* quest = 0;
    std::vector<MonsterKillItem>::iterator mit;
    CItem* item = 0;
    sync_script::GetInstanceSyncScript()->truncate_quest_category();
    it = g_questScriptFileList.begin();
    while (it != g_questScriptFileList.end())
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
        // [b28d-solo-CListTables] ORIG 用 size()（p167: MISSING vector<_DailySchedule>::size）。
        if (script.m_dailySchedule.size() != 0)
        {
            // ORIG 0x8353eb1：lea script+0x2dc 直传 AddDailSchedule
            // （vector<_DailySchedule> const&，ORIG 0x8125c3c），无逐项转换。
            GlobalData::s_DailyScheduleManager->AddDailSchedule(
                script.m_dailySchedule);
        }
        if (!script.m_monsterKillList.empty())
        {
            // [b28d-solo-CListTables] ORIG 用非 const iterator + deEv（P167：it-> 改 (*it).）
            // + 函数头已 C1Ev、此处仅赋值 + 尾后置 mit++（ppEi，P28/P50）。
            mit = script.m_monsterKillList.begin();
            while (mit != script.m_monsterKillList.end())
            {
                item = G_CDataManager()->find_item((*mit).m_itemIdx);
                if (item == 0)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x136f,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Exist\n",
                        path, (*mit).m_itemIdx);
                    return 0;
                }
                if (!(item->is_stackable() && item->GetItemType() == 8))
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x1375,
                        "Quest Script Error: %s - Monster Kill Item %d is not "
                        "Quest Item\n",
                        path, (*mit).m_itemIdx);
                    return 0;
                }
                mit++;
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
            // [b28d-solo-CListTables] ORIG 用 const_iterator（p167: C1<Pi> 转换构造 + deEv
            // + ppEi + ne(const,const)）。
            std::vector<int>::const_iterator vit = script.m_questData.begin();
            std::vector<int>::const_iterator vend = script.m_questData.end();
            // [P171/P232 照抄] ORIG 0x083541ec/0x083541f6 在循环前写
            // -0x422(int)/-0x41e(short) 两个死局部（全函数无读取点）。
            int deadQuestData = 0;
            short deadQuestData2 = 0;
            while (vit != vend)
            {
                int v1 = *vit++;
                int v2 = *vit++;
                int v3 = *vit++;
                if (script.m_questDataType == 6)
                {
                    // [b28d-solo-CListTables P310] ORIG 0x083542eb `mov -0x28,%eax;
                    // cmp $0xa,%eax; ja` 两段寄存器比较（unsigned 局部被 4.4 折成
                    // mem 直比）⇒ 局部声明 int + 比较处显式 (unsigned int)。
                    int enemyType = *vit++;
                    if ((unsigned int)enemyType > 10 ||
                        ((1 << (enemyType & 0x1f)) & 0x40e) == 0)
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
            // [b28d-solo-CListTables] ORIG 0x0835440a.. 每处重调
            // script.m_questData.size()（10 次调用，无 size 具名缓存）。
            if (script.m_questDataSubType == 4)
            {
                if (script.m_questData.size() != 2)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13c4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType,
                        (int)script.m_questData.size());
                    return 0;
                }
            }
            else if (script.m_questDataSubType == 6)
            {
                if (script.m_questData.size() != 2 &&
                    script.m_questData.size() != 3)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13cc,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType,
                        (int)script.m_questData.size());
                    return 0;
                }
            }
            else if (script.m_questDataSubType < 4 ||
                     script.m_questDataSubType == 5 ||
                     (script.m_questDataSubType > 6 &&
                      script.m_questDataSubType < 0xc))
            {
                if (script.m_questData.size() != 3)
                {
                    LogManager::logFormat(
                        1, "data_manager.cpp", __PRETTY_FUNCTION__,
                        0x13d4,
                        "Quest Script Error: %s - Quest Data Type is %d, But "
                        "Argumemt Size is Wrong(%d)\n",
                        path, script.m_questDataType,
                        (int)script.m_questData.size());
                    return 0;
                }
            }
            else if (script.m_questData.size() != 4)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__,
                    0x13dc,
                    "Quest Script Error: %s - Quest Data Type is %d, But "
                    "Argumemt Size is Wrong(%d)\n",
                    path, script.m_questDataType,
                    (int)script.m_questData.size());
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
        // [b28d-solo-CListTables] ORIG 0x08354d..：直接按值传 script（调用点仅 1 次
        // C1ERKS_ + 1 次 D1Ev），无具名 script2 副本。
        quest->set_quest(script);
        insert_NPC(script.m_npcIdx);
        m_questMap.insert(std::map<int, Quest*>::value_type(script.m_index, quest));
        if (script.m_questType == 4)
        {
            m_npcQuestList2.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 5)
        {
            m_npcQuestList1.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 0)
        {
            m_npcQuestList0.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 2)
        {
            m_npcQuestList4.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 6)
        {
            m_npcQuestList5.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 3)
        {
            m_npcQuestList7.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 1)
        {
            m_npcQuestList3.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        else if (script.m_questType == 8)
        {
            m_npcQuestList6.insert(
                std::multimap<int, int>::value_type(script.m_npcIdx, script.m_index));
        }
        if (script.m_mailQuestFlag != -1)
        {
            m_vecD4.push_back(script.m_index);
        }
        it++;
    }
    if (!m_vecD4.empty())
    {
        // [b28d-solo-CListTables] ORIG 0x08354c82 `movl $0x0,-0x1c` 循环外清零 +
        // 循环内 `q = find_quest()`（0x08354cde）⇒ q 声明在循环外。
        Quest* q = 0;
        // [b28d-solo-CListTables] ORIG 用 const_iterator（p167: C1IPi 共 2 处）。
        for (std::vector<int>::const_iterator it = m_vecD4.begin();
             it != m_vecD4.end(); ++it)
        {
            int questIdx = *it;
            q = find_quest(questIdx);
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
