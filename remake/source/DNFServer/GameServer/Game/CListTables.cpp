// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CListTables/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

// ===================== CDungeonList =====================

#include "CListTables/CDungeonList.inc"
}

#include "CListTables/dtor_CDungeonList.inc"
}

#include "CListTables/destroy.inc"
}

#include "CListTables/find_dungeon.inc"
}

#include "CListTables/get_dungeon_at_index.inc"
}

#include "CListTables/getDungeonIndexList.inc"
}

// ---------------------------------------------------------------------------
// CDataManager::getDungeonIndexList()
// ORIG T 0x08365434 sz 0x15（_ZN12CDataManager19getDungeonIndexListEv 非 const）。
// b28 BM4/Main 归位：原定义在 UserPartyLink4.cpp，ORIG 与 CDungeonList::getDungeonIndexList
// 地址相邻（0x0836542c/0x08365434 同 TU）且 callee 定义先见使 4.4 outgoing 精确继承
// （消 0x18 保守帧 → ORIG sub $0x4）。this+0x00 = m_dungeonList，透传返回值。
// （CDungeonList.cpp 为镜像 TU 不入构建，定义落本 TU 才是真实发射点。）
// ---------------------------------------------------------------------------
#include "CListTables/getDungeonIndexList_1.inc"
}

// [b28d-solo-CListTables 修复] ORIG 0x0834d6cc：69/69（gap=0）。已修：it 先 C1Ev
// 再 `it = find(idx)`（P50）、insert 改 value_type 直构 `pair<const int,CDungeon*>(
// dungeon->get_index(), dungeon)`（ORIG 0x0838a848 C2IiRS2_EEOT_OT0_；旧 make_pair
// 多出 make_pair + 转换 ctor 两个 EXTRA call，p167 现 CLEAR）。
// [round1-L29 2026-09-10 修复] ①end 改内联 `if (it == m_dungeonMap.end())`（ORIG 的 end 无具名槽）；
//   ②`int idx;` 前置声明（先声明浅槽 -0x2c）+ 赋值语句后置到 it 声明之后（保 C1Ev→get_index 调用序）
//   ⇒ 69=69 IDENTICAL（此前 NEAR）。
#include "CListTables/insert_dungeon.inc"
}

// [b28d-solo-CListTables 修复] ORIG 0x0834d376：178/178（gap=0）。已修：①it 先
// C1Ev 再 `it = begin()` + 尾部后置 it++（P50/P28）；②ImportDungeonScript 声明
// int→bool（ORIG 0x0834d408 `call; xor $1; test %al`，定义侧 STDungeonScript.cpp
// 已 bool）；③cMyTrace 改全表达式临时链（P227，size() 结果入 %ebx 跨 ctor 保活）。
// 残差仅帧 0x55c vs 0x56c + script/it/cMyTrace 槽位差（N82 族）⇒ 封顶。
#include "CListTables/load_list.inc"
}

// ===================== CMapList =====================

#include "CListTables/CMapList.inc"
}

#include "CListTables/dtor_CMapList.inc"
}

// [b28d-solo-CListTables AE] ORIG 0x0834f08c：真语义 bug 修复——尾循环三组 clear
// 第一组旧写 m_mapList[i+1]（漏清 index 0、多清 index 15），ORIG 目标为
// this+i*0x14+0x14/+0x154/+0x294（Ghidra C 同步）= m_mapList[i] ⇒ 修正后 82=82 AE。
#include "CListTables/destroy_1.inc"
}

#include "CListTables/find_map.inc"
}

// [b28d-solo-CListTables AE] ORIG 0x0834f18e：五处修复——①m_mapList[channel+1]→
// [channel]（ORIG 0x0834f357 目标 this+0x14*channel+0x14，真语义 bug）；②循环迭代器
// 改 const_iterator（P96，ORIG 有 C1<const PVPMAP_DATA*> 转换构造）；③get_map_type()
// 两次内联调用（p167 MISSING 1）；④mapPvpMapInfo_[count] = info 结构赋值复现单
// operator[] + 自比较 + memcpy(5)；⑤(*it).m_mapIdx 替 it->（deEv vs ptEv）。
// p167 25/25 CLEAR ⇒ 202=202 AE。
#include "CListTables/insert_map.inc"
}

// [b28d-32 N89/N82: STL 内联/槽位差（74=74 但装载序多块不同） | ORIG 0x0834f4ba]
#include "CListTables/LoadPvPSinglePlayableMap.inc"
}

// [b28d-solo-CListTables AE] ORIG 0x0834f5c8：`info = it->second;` 结构赋值即复现
// ORIG 的单次 ptEv + 寄存器内比较直跳 + memcpy(5)（4.4 对 5 字节 POD 赋值同形）；
// 旧 `if (&info != &it->second) memcpy(&info,&it->second,5)` 两次 ptEv + setne
// 物化（P197④ 镜像）⇒ 38=38 AE。
#include "CListTables/GetPvpMapInfo.inc"
}

#include "CListTables/GetMapList.inc"
}

#include "CListTables/GetPVPMapCount.inc"
}

// ===================== CMonsterManager =====================

#include "CListTables/CMonsterManager.inc"
}

#include "CListTables/dtor_CMonsterManager.inc"
}

#include "CListTables/destroy_2.inc"
}

#include "CListTables/find_monster.inc"
}

// [b28d-32 AE: const_iterator 对 + 后置 it++ + 具名 result 复现 ORIG，51=51 逐条全等 | ORIG 0x0834ff20]
#include "CListTables/get_levelrandom_mob.inc"
}

#include "CListTables/insert_monster.inc"
}

// [b28d-solo-CListTables N98/N93 NEAR] ORIG 0x0834fdee：54=54 助记符全等，仅
// it/end 槽互换（ORIG it -0x2c/end -0x24 vs 4.4 it -0x24/end -0x2c）。it 为先
// 声明后赋值（P50 复活，ORIG 单次 C1Ev），end 为初始化式直构；全头部声明多出
// 第二个 C1Ev（+5 条），交换声明序又改执行序 ⇒ 4.4 按首次使用序分配不可翻，封顶。
// [round1-L29 2026-09-10 修复] end 改内联 `if (it == m_monsterMap.end())`（ORIG end 无具名槽，
//   it/end 两具名对象槽互换消失）⇒ 54=54 IDENTICAL（此前 NEAR）。
#include "CListTables/insert_monster_1.inc"
}

// [b28d-solo-CListTables N(N82) DIFF] ORIG 0x0834fa84：215/215（gap=0）+ p167
// CLEAR(34/34，差集全为 C1/C2·D1/D2 别名）。已修：truncate 负形早退复现
// `xor $1; test; je`（P168a）、drops 下标 unsigned→int 复现 `setg`（P196）。
// 残差 8 处槽位差（unusedIt -0x38 vs -0x3c、STMonsterScript 基址 ±4、path==0
// 分支 je/jne 布局）：sizeof 0x1cc0/0x1cc4 两探针均不可同时对齐 ⇒ N82 槽位族封顶。
// [round1-L29 2026-09-10 部分修复] 判据⑩：`unusedIt` 移到 it 声明之后（后声明者深）⇒
//   -0x38(5)/-0x3c(1) 与 ORIG 逐值对齐。剩 `script` 基址 -0x1918 vs ORIG -0x1910（±8）+ 1 条 gap
//   ⇒ N126/N82 族；script 声明前移试验反劣（ndiff 15→22，已回退）。
// [round5-A29 修复 + C05① 边界复核，2026-09-10] 已按 **P325 共享尾出口**把 `path==0` 的
//   `return 1;` 改 `goto ret_true;` 并与循环后 `return 1` 共享尾标签 ⇒ 复现 ORIG
//   `je <共享 ret-true 块>`（旧形 4.4 内联 `mov $1,%ebx; jmp` +2 条），**DIFF→NEAR**
//   （norm 214=214）。C05①「双层嵌套」仅当 ORIG **两个检查共享同一尾块**时才需要；本件
//   fail 块各自独立（truncate 失败 → `puts`+return 0；循环内两处 printf+return 0 各属
//   独立块）⇒ 单尾标签足够，**不套两层**。D08 闸门：我方岛数−ORIG 岛数 = 0 < 2 ⇒ 不试。
#include "CListTables/load_monsters.inc"
}

// ===================== CSkillList =====================

#include "CListTables/CSkillList.inc"
}

#include "CListTables/dtor_CSkillList.inc"
}

// [b28d-32 N(P223/N89): ORIG 尾岛与 STL 调用形差（71/98） | ORIG 0x08350b42]
#include "CListTables/destroy_3.inc"
}

#include "CListTables/find_skill.inc"
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
#include "CListTables/insert_skill.inc"
}

// [b28d-solo-CListTables N82 NEAR] ORIG 0x08351c1e：138=138 + p167 17/17 CLEAR
// （== / mmEi / ppEi 全对齐），残差仅槽号整体深移 0x10（ORIG it/range -0x4c/-0x48
// vs 我们 -0x3c/-0x38）；char[0x10] 死局部撑帧实测 sub 0x58→0x78 且槽号不平移 ⇒ 封顶。
#include "CListTables/get_ran_groupskill_index.inc"
}

// [b28d-solo-CListTables N(P192/N82) DIFF] ORIG 0x08351e18：163/180 + p167 有
// MISSING value_type C1<int&,vector&>、EXTRA make_pair+2×~pair+转换 ctor。
// value_type 直构实测（std::map<...>::value_type(it->m_index, vec)）虽对齐该构造
// 实例，但连带 identical -2/ae -5（他函数符号集变化）⇒ 回退保持 make_pair；
// 帧 0x50 vs 0x60 + 槽整体深 0x10 属 N82 ⇒ 封顶。
#include "CListTables/set_post_learning_skill.inc"
}

#include "CListTables/get_post_learning_skill.inc"
}

#include "CListTables/getSkillList.inc"
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
#include "CListTables/init.inc"
}

// ===================== QuestList =====================

#include "CListTables/QuestList.inc"
}

#include "CListTables/dtor_QuestList.inc"
}

// [b28d-32 N(P223/N89): ORIG 空块岛与迭代器/STL 形态差（81/93） | ORIG 0x08353c86]
#include "CListTables/destroy_4.inc"
}

#include "CListTables/find_quest.inc"
}

#include "CListTables/getQuestNPCList.inc"
}

// [b28d-32 AE: P50 先声明后赋值（C1Ev）+ 后置 it++ + 双 nop 岛，61=61 逐条全等 | ORIG 0x08354f02]
#include "CListTables/insert_NPC.inc"
}

// [b28d-32 N(P74/P75): ORIG char 成员单字节拷贝 vs 4.4 折 dword（+0x15..17 padding 合并）+ 实参装载序（79/74）；本 TU 镜像已裁为 ORIG 7 字段 | ORIG 0x08355474]
#include "CListTables/Check_Quest.inc"
}

// [b28d-32 N(P75/N93): 实参装载序与槽位差（94/86） | ORIG 0x08354fc8]
#include "CListTables/check_ahead_quest.inc"
}

// [b28d-32 N(P75/N93): 栈拷贝/装载序与分支块序差（88/79） | ORIG 0x0835511e]
#include "CListTables/check_anti_quest.inc"
}

// [b28d-32 AE: P223 正向 if-early-return 消 goto 尾对齐 nop，31=31 逐条全等 | ORIG 0x08355284]
#include "CListTables/check_clear.inc"
}

// [b28d-solo-CListTables AE] ORIG 0x08355304：113=113——①quest=0 声明提到
// out.clear() 之前（ORIG `movl $0x0,-0xc` 先于 clear）；②`return !out.empty()`
// 拆成 `if (out.empty()) return false; return true;`（P56，消 4 条 bool 物化）；
// ③questIdx 全头部声明（P309/P212，it/questIdx 槽序对齐）。
#include "CListTables/select_mail_quest.inc"
}

#include "CListTables/select_quest.inc"
}

// [b28d-solo-CListTables 修复] ORIG 0x0835556c：131/131（gap=0）+ p167 CLEAR(16/16)。
// 已修：range 提到函数头（C1Ev 在循环外）、三守卫独立 goto + 尾标签串（P223 多岛，
// ORIG +0x136/+0x139/+0x13c 三 nop 岛复现）、flag 守卫改嵌套 `if (!flag) {...}`
// 复现 ORIG `movzbl -0x3c; xor $1; test %al; je`（旧 `if (flag) goto DO_PUSH` 被
// 4.4 折成 `cmpb $0; jne`）。残差仅 it/range/param 槽位差（N82 族）⇒ 封顶。
// [round1-L29 2026-09-10 修复] 判据⑩：`int npcIdx;` 前置声明（无初值→浅槽 -0x24）+ 循环内改
//   赋值 `npcIdx = *it;`（保留 it→range→npcIdx 调用序）⇒ AE 128/128 ndiff=0（此前 NEAR）。
//   注：把 cit 也提到头部（全头部声明）反劣 131 vs 128，已回退。
#include "CListTables/allowable_questlist_as_npc.inc"
}

// [b28d-solo-CListTables AE] ORIG 0x0835589e：106=106——①双守卫独立否定 goto +
// 尾标签串（P223，ORIG +0xe0/+0xe3 两 nop 岛复现）；②npcIdx/questIdx 全头部声明
// 按 ORIG 槽浅→深（P212/P309，range/cit 槽序对齐）。
#include "CListTables/GetDailyQuestList.inc"
}

#include "CListTables/GetTrainingQuestList.inc"
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
#include "CListTables/load_list_1.inc"
}
