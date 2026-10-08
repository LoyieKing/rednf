// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CListTables/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
    // [b24FIX · 与权威定义对齐] 权威 CDungeon.h 的 STDungeonScript 末端为
    //   m_map4e4(+0x4e4,0x18) / m_4fc / m_500 / m_504 / m_505 / m_508 / m_50c(+0x50c)
    //   ⇒ **sizeof = 0x510**；旧镜像只铺到 +0x50a（sizeof 0x50C），比权威**短 4 字节**。
    // 后果（实机 A/B 实证）：load_list 的局部 script 按镜像 0x50C 分配，而
    //   ImportDungeonScript（按权威 0x510 编译）收尾写 m_50c(+0x50c) 时越界 4 字节，
    //   恰好落在紧邻其上的迭代器 it —— script 基址 -0x548 + 0x50c = **-0x3c = it 槽位**
    //   ⇒ it 的节点指针被写坏，循环尾 `it++`(_Rb_tree_increment) 走野指针 SIGSEGV
    //   （坏值呈数据样：MALLOC_PERTURB 下为 ASCII "7287"）。
    // 修法：按 ORIG load_list 帧实证（ORIG script 基 -0x54c、it -0x3c、帧 0x56c）把镜像
    //   补到与权威等长：0x324+0x1e9 = 0x50D → 对齐后 sizeof 0x510 ✓。
    char m_pad324[0x1e9];                         // +0x324..0x50d（对齐后 sizeof 0x510）
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
    // [DNF-FIX 2026-10-08 c31] ORIG load_monsters 0x0834fcb1 读 -0x1914 / 0x0834fce1 读 -0x1910，
    //   script@-0x1d00 ⇒ +0x3ec/+0x3f0（= STMonsterScript.h m_levelByDiff[2] @+0x3ec）。
    char m_pad3b8[0x34];                      // +0x3b8
    int m_minLevel;                           // +0x3ec
    int m_maxLevel;                           // +0x3f0
    // [b28d-solo-CListTables] 补足权威 sizeof（STMonsterScript.h 尾部注释 0x1cc4；
    // ORIG load_monsters 帧 0x1d1c / script 槽 -0x1d0c 实证），此前截断到 0x3f0
    // 导致栈帧仅 0x44c。
    char m_pad3f4[0x1cc4 - 0x3f4];            // +0x3f4..0x1cc4
};

struct MonsterKillItem
{
    char m_pad[0x10];
    int m_itemIdx;    // +0x10
    // [AttackC 2026-09-11 尺寸修正] ORIG `_ZN9__gnu_cxx17__normal_iteratorIP15MonsterKillItem
    //   St6vectorIS1_SaIS1_EEEppEi`（0x082b6eae 段）`lea 0x20(%eax),%ecx` ⇒ 元素 stride
    //   **0x20**；旧定义 sizeof=0x14 使本 TU 全部 vector<MonsterKillItem> 迭代/析构步长少
    //   0xc（越界读）。+0x10（find_item 读点）偏移不变，仅补尾 0xc 字节。
    char m_pad14[0xc]; // +0x14..+0x1f（sizeof 0x20）
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

#include "CListTables/get_rand_int.inc"

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

#include "CListTables/PvPMapInfo.inc"


// ===================== CDungeonList =====================

#include "CListTables/CDungeonList.inc"


#include "CListTables/dtor_CDungeonList.inc"


#include "CListTables/destroy.inc"


#include "CListTables/find_dungeon.inc"


#include "CListTables/get_dungeon_at_index.inc"


#include "CListTables/getDungeonIndexList.inc"


#include "CListTables/getDungeonIndexList_1.inc"


#include "CListTables/insert_dungeon.inc"


#include "CListTables/load_list.inc"


// ===================== CMapList =====================

#include "CListTables/CMapList.inc"


#include "CListTables/dtor_CMapList.inc"


#include "CListTables/destroy_1.inc"


#include "CListTables/find_map.inc"


#include "CListTables/insert_map.inc"


#include "CListTables/LoadPvPSinglePlayableMap.inc"


#include "CListTables/GetPvpMapInfo.inc"


#include "CListTables/GetMapList.inc"


#include "CListTables/GetPVPMapCount.inc"


// ===================== CMonsterManager =====================

#include "CListTables/CMonsterManager.inc"


#include "CListTables/dtor_CMonsterManager.inc"


#include "CListTables/destroy_2.inc"


#include "CListTables/find_monster.inc"


#include "CListTables/get_levelrandom_mob.inc"


#include "CListTables/insert_monster.inc"


#include "CListTables/insert_monster_1.inc"


#include "CListTables/load_monsters.inc"


// ===================== CSkillList =====================

#include "CListTables/CSkillList.inc"


#include "CListTables/dtor_CSkillList.inc"


#include "CListTables/destroy_3.inc"


#include "CListTables/find_skill.inc"


#include "CListTables/insert_skill.inc"


#include "CListTables/get_ran_groupskill_index.inc"


#include "CListTables/set_post_learning_skill.inc"


#include "CListTables/get_post_learning_skill.inc"


#include "CListTables/getSkillList.inc"


#include "CListTables/init.inc"


// ===================== QuestList =====================

#include "CListTables/QuestList.inc"


#include "CListTables/dtor_QuestList.inc"


#include "CListTables/destroy_4.inc"


#include "CListTables/find_quest.inc"


#include "CListTables/getQuestNPCList.inc"


#include "CListTables/insert_NPC.inc"


#include "CListTables/Check_Quest.inc"


#include "CListTables/check_ahead_quest.inc"


#include "CListTables/check_anti_quest.inc"


#include "CListTables/check_clear.inc"


#include "CListTables/select_mail_quest.inc"


#include "CListTables/select_quest.inc"


#include "CListTables/allowable_questlist_as_npc.inc"


#include "CListTables/GetDailyQuestList.inc"


#include "CListTables/GetTrainingQuestList.inc"


#include "CListTables/load_list_1.inc"

