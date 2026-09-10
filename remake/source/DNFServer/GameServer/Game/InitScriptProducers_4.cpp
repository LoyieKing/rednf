// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InitScriptProducers_4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— InitScriptProducers_4.cpp
// CDataManager::init()（ORIG 0x08358332..0x0835f1d5，docs/class_func_reports/
// CDataManager/init.md）依赖的 8 个大类脚本装载自由函数。逐函数自 ORIG
// objdump 反汇编转写（df_game_r），非固定返回：
//
//   InitCharacterScript    _Z19InitCharacterScriptPKcS0_S0_S0_  T 0x08a0cc3c
//     init.md +0x2740 调用 ("Character/","Character.lst","ExpTable.tbl",NULL)
//   InitMonsterScript      _Z17InitMonsterScriptPKcS0_S0_       T 0x08a11311
//     init.md +0x27b9 调用 ("Monster/","Monster.lst","MonsterApcDifficultyBonus.tbl")
//   InitPassiveObjectScript _Z23InitPassiveObjectScriptPKcS0_   T 0x08a29ead
//     init.md +0x282a 调用 ("PassiveObject/","PassiveObject.lst")
//   InitEquipmentScript    _Z19InitEquipmentScriptPKcS0_S0_     T 0x08991601
//     init.md +0x28a3 调用 ("equipment/","equipment.lst","pricetable.tbl")
//   InitStackableScript    _Z19InitStackableScriptPKcS0_S0_     T 0x089ae72e
//     init.md +0x291c 调用 ("stackable/","stackable.lst","pricetable.tbl")
//   InitMapScript          _Z13InitMapScriptPKcS0_              T 0x089dce98
//     init.md +0x294d 调用 ("Map/","Map.lst")
//   InitDungeonScript      _Z17InitDungeonScriptPcS_            T 0x088e5e3c
//     init.md +0x29be 调用 ("Dungeon/","Dungeon.lst")
//   InitSkillScript        _Z15InitSkillScriptPKcS0_            T 0x08a97438
//     init.md +0x2ac6 调用 ("Skill/","SkillList.lst")
//
// 统一模式：strncpy 基目录全局(0x100) → 末尾非 '\\''/' 时 memcpy 追加 "\\"
// （ORIG 字符串 0x8e1aaaf/0x8e1df2d/0x8e16029 均为 "\"）→ loadRDARScriptListFile
// （STScriptFileList.cpp，ORIG T 0x088bbd77）/loadRDARScriptFile +
// ScanType/ScanInt/ScanFloat 分派回填（DNFLexWrapperHelpers.cpp 真实符号）。
//
// 本 TU 同时提供上述装载器的全局落点（名称逐个取自 df_game_r nm 符号表，
// 地址注释在定义处）：脚本文件列表、基目录缓冲、价格表、APC 难度加成表等。
// MonsterBaseParameter / DungeonPartyBalance / DungeonHoldBalance /
// stLimitSlowDown 为 ORIG DWARF 类型名，本 TU 首次落地定义（已与
// FixMiscStubs.STMonsterImport / STEquipmentImport 协调无 ODR 冲突）。
// ---------------------------------------------------------------------------
// [b22判词·N族/索引伪差集 P93·P154 同族] __tcf_0（<global>）：本 TU 文件作用域带
// 非平凡析构全局（g_staSkillScriptFileList[11] map 数组、各 g_st*ScriptFileList、
// std::vector/std::map 全局）的 GCC 合成 atexit cleanup，无独立可写源码函数体，
// 指令完全由本 TU 全局对象集合与构造序决定。ORIG 二进制内 `__tcf_0` 同名 local 数百枚
// （每含全局对象 TU 一枚），compare 工具按名取到 0x0828f940（WongWork::CDeathTowerRanking
// 域另一 TU，与本 TU 全局无关）→ 逐条 DIFF 系索引伪差集（§6.3 撞名族），非实现缺口。
// 本 .o __tcf_0 已按本 TU 全局集合正确合成析构循环（0x108/0x18 = 11 元素对位
// g_staSkillScriptFileList）。语义等价，不可源码复现，保留判词。
// [b28d-solo-InitScriptProducers_4 N] 同上：__tcf_0 / __static_initialization_and_destruction_0ii /
// __uninitialized_copy<false,MonsterBaseParameter*> 三枚均为编译器合成/模板实例
// （P93 同址别名 + N89 libstdc++ 内联差 + 全局构造序），非本 TU 可写函数体。
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include <string.h>

#include "CEnvironment.h"        // less_TCHAR / SpinLock（SpinLock.cpp 提供）
#include "DNFLexWrapper.h"       // loadRDARScriptFile / ScanType / ScanInt / ScanFloat
#include "LogManager.h"          // LogManager::logFormat
#include "STCharacterScript.h"   // STCharacterScript::naExpTable / pvpPrices_
#include "STEquipmentScript.h"   // STEquipmentScript::priceTable_ 声明
#include "STStackableScript.h"   // STStackableScript::priceTable_ 声明
#include "NpcScript.h"

class STScriptFileList;
// bool 视图声明（P22：ORIG init* 尾 xor $1,%eax;test %al 为 bool 取反形；
// 返回类型不入 mangled，链接 STScriptFileList.cpp 的 int 定义恒 0/1，ABI 兼容）。
#include "InitScriptProducers_4/loadRDARScriptListFile.inc"
extern STScriptFileList g_stMapScriptFileList;      // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x089dce98：列表登记后直接透传 loadRDARScriptListFile 结果。
// [b28d-solo-InitScriptProducers_4 AE] P303 __builtin_strcat + P304 权威型 &g_stMapScriptFileList
// （去 reinterpret_cast 物化）；43/43 逐条同构。
#include "InitScriptProducers_4/InitMapScript.inc"
}

// ORIG B 0x94ff2a0 [0x100] / B 0x94ff3a0 [0x18]
char g_szPassiveObjectScriptBaseDirectory[0x100];
STScriptFileList g_stPassiveObjectScriptFileList;   // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x08a29ead：与 InitMapScript 完全同构（不同全局）。
// [b28d-solo-InitScriptProducers_4 AE] 同 InitMapScript：P303 + P304 权威型；43/43。
#include "InitScriptProducers_4/InitPassiveObjectScript.inc"
}

// ORIG 0x08a3c5c2（0xbb 字节）：与 2 参数版同构，仅把 map 透传
// loadRDARScriptListFile 第 4 参（ORIG 0x8a3c646 mov 0x10(%ebp)，
// 2 参数版同址为 movl $0）。
// [b28d-solo-InitScriptProducers_4 AE] 3 参版同构（第 4 参 map 透传）；44/44。
#include "InitScriptProducers_4/InitPassiveObjectScript_1.inc"
}

// ===================== 地下城脚本域（InitDungeonScript）=============

// ORIG B 0x94f6da0 [0x100] / B 0x94f6ea0 [0x18] / B 0x9500480 [0x18]
char g_szDungeonScriptBaseDirectory[0x100];
STScriptFileList g_stDungeonScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）
std::map<int, std::string> g_questScriptFileList;       // ORIG 0x9500480（CListTables 引用）

// ORIG 0x088e5e3c：签名 char*（调用点传字符串字面量，ORIG mangled PcS_）。
// [b28d-solo-InitScriptProducers_4 AE] 旧 [b28c-L23 N(P209/P75)] 判词作废：P303
// __builtin_strcat 复现 ORIG 9 条内建展开（strlen+memcpy）；43/43 逐条同构。
#include "InitScriptProducers_4/InitDungeonScript.inc"
}

// ===================== 技能脚本域（InitSkillScript）=================

// ORIG B 0x9500720 [0x100] / B 0x9500820 [11 × 0x18]
// 数组长度 11 的证据：ORIG __static_initialization_and_destruction_0ii
// （0x08a9b073..0x08a097）以 esi=0xa 递减构造 11 个元素（步进 0x18），
// __tcf_0 析构上界 0x9500928 = 0x9500820 + 11*0x18。
char g_szSkillScriptBaseDirectory[0x100];
STScriptFileList g_staSkillScriptFileList[11];   // 权威型（NpcScript.h，元素 0x18）

// ORIG 0x08a97438：先整表装入局部 STScriptFileList，再按条目分Job 装载；
// idx > 10 的条目跳过；任一分表装载失败即整体失败。
// [b28d-solo-InitScriptProducers_4 AE] P160：删具名 idx 局部，(*it).first 二次 deEv 直读；129/129。
#include "InitScriptProducers_4/InitSkillScript.inc"
}

// ===================== 装备/可堆叠价格表域 ==========================

// ORIG B 0x94f7bfc [0xc]：pricetable.tbl 头三个 int（装备出售价格系数）。
int itemPriceRates_[3];
// ORIG B 0x94f7c08 [4]："repair cost" 浮点系数。
float itemRepairCost_;
// ORIG B 0x94f7c20 [0x2ee0]：30×100 int 表（static 成员，声明见头）。
int STEquipmentScript::priceTable_[30][100];
// ORIG B 0x94fac20 [0x2ee0]：同构 30×100 int 表（可堆叠物品）。
int STStackableScript::priceTable_[30][100];

// ORIG B 0x94fab00 [0x100] / B 0x94fac00 [0x18]
char g_szEquipmentScriptBaseDirectory[0x100];
STScriptFileList g_stEquipmentScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x08991601：列表登记 → pricetable.tbl 清零回填 → 头 3 个价格系数 →
// "[repair cost]" 可选段 → 30 行×100 列主表。
// [b28d-solo-InitScriptProducers_4 AE] P303 + P160（函数级 i 复用 -0x14）+ P191 首循环双初始化
// + 删死 bool ok；152/152。
#include "InitScriptProducers_4/InitEquipmentScript.inc"
}

// ORIG B 0x94fdb00 [0x100] / B 0x94fdc00 [0x18]
char g_szStackableScriptBaseDirectory[0x100];
STScriptFileList g_stStackableScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x089ae72e：列表登记 → pricetable.tbl 清零回填 30×100 主表。
// [b28d-solo-InitScriptProducers_4 AE] 同 Equipment：i 复用 + 双初始化 + 删死 bool ok；124/124。
#include "InitScriptProducers_4/InitStackableScript.inc"
}

// ===================== 角色脚本域（InitCharacterScript）=============

// ORIG B 0x94fe840 [0x100] / B 0x94fe940 [0x18]
char g_szCharacterScriptBaseDirectory[0x100];
STScriptFileList g_stCharacterScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）
// ORIG B 0x94fe960 [0x320] / B 0x94fec80 [0x320]：静态成员定义（声明见头）。
int STCharacterScript::naExpTable[0xc8];
int STCharacterScript::pvpPrices_[0xc8];

// ORIG 0x08a0cc3c：Character.lst 登记 → ExpTable.tbl 回填 naExpTable[200] →
// 第 4 参非 NULL 时再加载对应 pvp 价格表回填 pvpPrices_[200]。
// （DataManagerInitDepends.h 第 4 参注释作 unused，与 ORIG 0x14(%ebp) 分支
//  不符——真实语义为可选 pvp 价格表路径，此处按 ORIG 实现。）
// [b28d-solo-InitScriptProducers_4 AE] P160/P191：函数级 int i=0 复用 -0xc 槽 + 首循环双初始化；107/107。
#include "InitScriptProducers_4/InitCharacterScript.inc"
}

// ===================== 怪物脚本域（InitMonsterScript）===============

// ---- ORIG 数据类型（DWARF 类型名；本 TU 权威定义）----

// ORIG sizeof 0x60（rep movsl $0x18）；ctor 0x08a3cf2c→clear 0x08a3ce4a 全清零。
struct MonsterBaseParameter
{
    enum { FIELD_COUNT = 24 };
    // [b29-SweepSTMonsterScript2] ORIG 两处循环列组用 base=param+4、idx=i+0xc/i+0x10
    // 寻址（clear `movl $0,0x4(%eax,%edx,4)` @0x08a3ce4a；ImportMonsterBaseTable
    //  `mov %eax,-0xe8(%ebp,%edx,4)` @0x08a3c8a8/0x08a3c8d9）⇒ 原码数组成员自
    //  +0x04 起（首 int 独立）。匿名 union 双视角复现该寻址；sizeof 0x60、全部
    //  偏移、mangled 零变化（POD 拷贝仍 rep movsl 0x18 dwords）。
    union {
        int m_value[FIELD_COUNT];   // +0x00..+0x5c，按解析顺序填充（既有消费点）
        // m_grid[3][i]=m_value[13+i]（this+4+(12+i)*4）、
        // m_grid[4][i]=m_value[17+i]（this+4+(16+i)*4）⇒ 复现 ORIG
        // `mov i,%edx; add $0xc/$0x10,%edx; movl $0,0x4(%eax,%edx,4)` 两步寻址。
        // sizeof 仍 0x60（4 + 5*4*4 + 3*4 = 96），全部偏移/mangled 零变化。
        struct { int m_value0; int m_grid[5][4]; int m_value21, m_value22, m_value23; };
    };

    void clear()
    {
        // [b28c-L29] ORIG W 0x08a3ce4a：前 13 字段逐条独立 store，随后
        // i∈[0,3] 循环写 m_value[13+i]/m_value[17+i]（+0xc/+0x10 偏移寻址），
        // 尾 3 字段独立 store——照抄原码展开形（非单一 24 次循环）。
        m_value[0] = 0;
        m_value[1] = 0;
        m_value[2] = 0;
        m_value[3] = 0;
        m_value[4] = 0;
        m_value[5] = 0;
        m_value[6] = 0;
        m_value[7] = 0;
        m_value[8] = 0;
        m_value[9] = 0;
        m_value[10] = 0;
        m_value[11] = 0;
        m_value[12] = 0;
        for (int i = 0; i <= 3; ++i)
        {
            m_grid[3][i] = 0;   // ORIG disp=+0x04、idx=i+0xc（value[13+i]）
            m_grid[4][i] = 0;   // ORIG disp=+0x04、idx=i+0x10（value[17+i]）
        }
        m_value[21] = 0;
        m_value[22] = 0;
        m_value[23] = 0;
    }
    MonsterBaseParameter() { clear(); }
};

// ORIG sizeof 0xd8：grid[13][4](+0x000..+0xcf) + a(+0xd0) + b(+0xd4)。
// 解析序：两个 int 先入 a/b（栈上 -0x19c/-0x198 = 缓冲 +0xd0/+0xd4），随后
// 13×4 float 入 +0x000 起。
struct DungeonPartyBalance
{
    float m_grid[13][4];                    // +0x00
    float m_a;                              // +0xd0
    float m_b;                              // +0xd4

    void clear()
    {
        for (int r = 0; r < 13; ++r)
            for (int c = 0; c < 4; ++c)
                m_grid[r][c] = 0.0f;
        m_a = 0.0f;
        m_b = 0.0f;
    }
    // ORIG 段内无 C1 调用（0x8a11cf2 直写字段）⇒ 无用户 ctor（POD）。
};

// ORIG ctor 0x08a3deee → clear 0x08a3ded6：两 int 清零；sizeof 8。
struct DungeonHoldBalance
{
    int m_a;                                // +0x00
    int m_b;                                // +0x04

    void clear() { m_a = 0; m_b = 0; }
    DungeonHoldBalance() { clear(); }
};

// ORIG ctor 0x08a3df1a → clear 0x08a3df02：两 int 清零；sizeof 8。
struct stLimitSlowDown
{
    int m_a;                                // +0x00
    int m_b;                                // +0x04

    void clear() { m_a = 0; m_b = 0; }
    stLimitSlowDown() { clear(); }
};

// ---- ORIG 全局落点（nm 名称/地址逐条核对）----

// ORIG D 0x93f54c4 [0x14]：五张基础参数表文件名。
const char* MONSTER_BASIS_PARAMETER_TABLE_NAMES[5] = {
    "CommonMonsterBaseParameter.tbl",       // 0x8e1ebc0
    "NamedMonsterBaseParameter.tbl",        // 0x8e1ebdf
    "BossMonsterBaseParameter.tbl",         // 0x8e1ebfd
    "SummonMonsterBaseParameter.tbl",       // 0x8e1ec1c
    "WarRoomMonsterBaseParameter.tbl",      // 0x8e1ec3c
};
std::vector<std::vector<MonsterBaseParameter> > g_monsterBaseParameters;  // B 0x94ff278

// ORIG D 0x93f4a00 [0x104]=13×5 / D 0x93f4b20 [0x104]=13×5 /
//       D 0x93f4c40 [0xd0]=13×4；.data 初值均为首元 1.0f 其余 0。
// [L32 GI 调序] DIFFICULTY_OF_DUNGEON_BONUS 定义已前移到文件首，此处不再重复定义。
float BLOOD_SYSTEM_BONUS[13][5]           = { { 1.0f } };
float DEATH_TOWER_BONUS[13][4]            = { { 1.0f } };

// ORIG B 0x9494f20 [0x3c] = vector<vector<float>>[5]，下标 [easy]..[hero]。
std::vector<std::vector<float> > MONSTER_DIFFICULTY_BONUS[5];
// ORIG B 0x9494f5c / 0x9494f68 / 0x9494f74 / 0x9494f80
std::vector<DungeonPartyBalance> DUNGEON_PARTY_BALANCE_TABLE;
std::vector<DungeonHoldBalance> NORMAL_MONSTER_HOLD_BALANCE_TABLE;
std::vector<DungeonHoldBalance> NAMED_MONSTER_HOLD_BALANCE_TABLE;
std::vector<DungeonHoldBalance> BOSS_MONSTER_HOLD_BALANCE_TABLE;
// ORIG B 0x9494f8c / 0x9494fa4 / 0x9494f98（MONSER 为 ORIG nm 原拼写）
std::vector<stLimitSlowDown> NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE;
std::vector<stLimitSlowDown> BOSS_MONSER_LIMIT_SLOWDOWN_TABLE;
std::vector<stLimitSlowDown> NAMED_MONSER_LIMIT_SLOWDOWN_TABLE;
// ORIG D 0x93f4d10 / 0x93f4d14 / 0x93f4d18，初值 1.0f
float NORMAL_MONSTER_GAUGE_ADJUST = 1.0f;
float NAMED_MONSTER_GAUGE_ADJUST  = 1.0f;
float BOSS_MONSTER_GAUGE_ADJUST   = 1.0f;

// ORIG B 0x94ff160 [0x100] / B 0x94ff260 [0x18]
char g_szMonsterScriptBaseDirectory[0x100];
STScriptFileList g_stMonsterScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x08a11311（0x1516 字节）：
//   list==NULL → 跳过目录/列表/基础参数表，直入 APC 加成表段；
//   否则登记 Monster.lst → resize(5) 基础参数向量 → 五张 *BaseParameter.tbl
//   逐行 MonsterBaseParameter 回填 → MonsterApcDifficultyBonus.tbl 三张
//   13 行浮点加成表 → 标签化难度/平衡/减速/血量调整段。
// [b28d-solo-InitScriptProducers_4 语义完成] 三件套：①p167 调用点差集 168/168 CLEAR；
// ②帧 0x2ec 与 ORIG 全等；③本 TU 剩余真缺口 = 0。上轮修的真缺口：
//  ①[limit slowdown] 段 P333 状态机还原（ORIG 0x8a120e6..0x8a12678：switch fallthrough
//    逐条 insert + 尾部 leave/置位/goto retry 回跳，原直线 if 链漏 4 处 __inited 检查与回跳）；
//  ②[/limit slowdown] 终止标记应 break（原 continue 会吞掉后续主标签，真语义 bug）；
//  ③ok 形参：limit slowdown 三段 9 处 ScanInt + mesmerization 三段 6 处 ScanInt +
//    gauge 三处 ScanFloat 均须传 &ok（ORIG lea -0x151），原 (bool*)0 不更新 ok；
//  ④解析具名中转 a/b（ORIG -0x38/-0x34、-0x4c/-0x48）与 [adjust hp gauge] 段首
//    float v=1.0f（ORIG -0x1c，三支复用）补回。
// 其余：P303、临时空向量 resize(5,临时)、ORIG 列序 ScanInt(0,3,4,5,6,1,2,7..12,两循环,21..23)、
// 5 连短路难度链、段首单次构造 DungeonHoldBalance/stLimitSlowDown、P177 static 入循环体。
// [本轮 2026-09-09] gap 8→0（raw 1239 vs 1231 → 1239 vs 1239；p167 168/168 CLEAR；
//   帧 0x2ec 全等）：①[limit slowdown] 4 处 `if (inited)` 改负极性
//   `if (!inited) {insert} else {...break}`（P22/P133；ORIG 0x8a12312/0x8a12378/
//   0x8a1245c/0x8a12540 均为 movzbl __inited; xor $1; test; je <else>）；
//   ②MonsterBaseParameter 循环 first(-0x78) 预清零且声明序在 param 之前（P198①/P0；
//   ORIG 0x8a117a6 movl $0,-0x78 在 0x8a117b0 param C1 之前）。
// [本轮 2026-09-10 结构归位 · byte 层封顶] ①row 循环内 first/ok/param 三者上提至
//   tableIdx 体内（ORIG 0x8a1146a first=0 → 0x8a11471 ok=false → 0x8a11478 param C1
//   → 0x8a11486 row=0；C1 全函数仅一次）；②m_value[13+k]/[17+k] 改 m_grid[3][k]/
//   m_grid[4][k]（MonsterBaseParameter 匿名 union 视角，P311）；③三段难度表循环改
//   `int r=0; while(true){…;++r; if(!ok||r==13) break;}`（t11 形，ORIG 0x8a11844..
//   0x8a11875 逐条同构）；④补 ORIG 0x8a117a6 死局部 deadIdx@-0x84（P244 照抄）；
//   ⑤resize 分支的 def 改匿名临时 `resize(row+1, MonsterBaseParameter())`。
//   ⇒ 指令数 1226→1220 与 ORIG 全等；flat（槽号归一）ndiff 528→36；p167 168/168 CLEAR。
// 残余 = N93/N98 槽分配策略差（4.4 给深层嵌套/后声明对象**更深**槽，4.1 相反；探针
//   /tmp/tu4probe/r.cpp t1/t2/t3 三形实证）⇒ param/def/tableIdx/r 等整体错位 4~8B，
//   无源码形可翻（全头部声明会改 ctor 发射点与顺序，P331 明禁）⇒ byte 层 N 封顶。
#include "InitScriptProducers_4/InitMonsterScript.inc"
}
// ORIG 0x08a3b0ad（0x1515 字节）：带 map 形参重载
// _Z17InitMonsterScriptPKcS0_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEES0_。
// 与 3 参数版（0x08a11311）逐条 objdump 比对（本轮）：差异仅
//  ①loadRDARScriptListFile 第 4 参装载 0x10(%ebp)（map）而非 movl $0；
//  ②logFormat 行号本版 0x2b22/0x2b38/0x2b54/0x2b6f（3 参版 0x7e6/0x7fc/0x818/0x833）；
//  ③limit slowdown 段 map value 本版 0x2c6e/0x2c71/0x2c85/0x2c99
//    （3 参版 0x932/0x935/0x949/0x95d）；④__FUNCTION__/static 变量/字符串地址不同；
// 其余指令序列、跳转结构逐条相同（两版函数级 static __lookup/__init_lock/__inited 各自独立：
// ORIG 0x94fff80/0x94fff98/0x94fff9c vs 0x94ff8a8/0x94ff8b8/0x94ff8d4）。
// [本轮 2026-09-09] gap 8→0（raw 1240 vs 1232 → 1240 vs 1240；p167 168/168 CLEAR，
//   余 C1/C2、D1/D2 同址别名 P93 豁免）：真缺口 = 错常量实参——原源码 4 处 map value
//   与 4 处 logFormat 行号误抄 3 参版常量，已按本版 ORIG 修正；另同 3 参版
//   P22/P133 负极性改写（if (!inited)…else…）与 first(-0x78) 预清零（P198①/P0）。
// 注：本函数体内行内注释的 0x8a11xxxx/0x8a12xxxx 地址系上轮自 3 参版复制的同构位置
// 注释；本版真实地址以 objdump 0x08a3b0ad 起为准（仅注释，机器码不受影响）。
// [本轮 2026-09-10 结构归位 · byte 层封顶] 与 3 参版同批：①first/ok/param 上提至
//   tableIdx 体内（ORIG 0x8a3b205 first=0 → ok=false → param C1 → row=0）；②m_grid
//   双视角写；③三段难度表循环改 `while(true){…;++r; if(!ok||r==13) break;}`；
//   ④deadIdx@-0x84；⑤resize 分支 def 改匿名临时；⑥row==size 比较改 unsigned 形。
//   ⇒ 指令数 1227→1221 与 ORIG 全等，flat ndiff → 36；p167 168/168 CLEAR。
//   残余同 3 参版 = N93/N98 槽分配策略差。
#include "InitScriptProducers_4/InitMonsterScript_1.inc"
}
