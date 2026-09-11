// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "InitScriptProducers_4/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* map);

// ---- STScriptFileList（布局权威：STScriptFileList.cpp 同名本地类，
//      sizeof 0x18 = std::map<int,std::string>；ORIG ctor 0x088bacf8）----


// [L32 GI 调序] ORIG `_GLOBAL__I_DIFFICULTY_OF_DUNGEON_BONUS`
//   （CU=RDARMonsterAbilityFormula.cpp）⇒ 本 TU 首个强符号须为
//   DIFFICULTY_OF_DUNGEON_BONUS（静态初始化 float 表，无构造序影响；P327）。
//   ORIG D 0x93f4a00 [0x104]=13×5，.data 初值为首元 1.0f 其余 0。
float DIFFICULTY_OF_DUNGEON_BONUS[13][5]  = { { 1.0f } };

// ===================== 地图脚本域（InitMapScript）=====================

// ORIG B 0x94fe2a0 [0x100]
char g_szMapScriptBaseDirectory[0x100];
// [b14 归属迁移·Main 批准] B 0x94fe3a0 的 g_stMapScriptFileList **定义**已迁至
// OnlyOrigBatch14.cpp（ORIG 该 TU static_init 0x089e81d2 构造它 + atexit D1
// 0x80f1352，属 P186/P128）；本 TU 仅按 extern 引用（InitMapScript 消费）。
extern STScriptFileList g_stMapScriptFileList;      // 权威型（NpcScript.h，sizeof 0x18）

#include "InitScriptProducers_4/InitMapScript.inc"


// ORIG B 0x94ff2a0 [0x100] / B 0x94ff3a0 [0x18]
char g_szPassiveObjectScriptBaseDirectory[0x100];
STScriptFileList g_stPassiveObjectScriptFileList;   // 权威型（NpcScript.h，sizeof 0x18）

#include "InitScriptProducers_4/InitPassiveObjectScript.inc"


#include "InitScriptProducers_4/InitPassiveObjectScript_1.inc"


// ===================== 地下城脚本域（InitDungeonScript）=============

// ORIG B 0x94f6da0 [0x100] / B 0x94f6ea0 [0x18] / B 0x9500480 [0x18]
char g_szDungeonScriptBaseDirectory[0x100];
STScriptFileList g_stDungeonScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）
std::map<int, std::string> g_questScriptFileList;       // ORIG 0x9500480（CListTables 引用）

#include "InitScriptProducers_4/InitDungeonScript.inc"


// ===================== 技能脚本域（InitSkillScript）=================

// ORIG B 0x9500720 [0x100] / B 0x9500820 [11 × 0x18]
// 数组长度 11 的证据：ORIG __static_initialization_and_destruction_0ii
// （0x08a9b073..0x08a097）以 esi=0xa 递减构造 11 个元素（步进 0x18），
// __tcf_0 析构上界 0x9500928 = 0x9500820 + 11*0x18。
char g_szSkillScriptBaseDirectory[0x100];
STScriptFileList g_staSkillScriptFileList[11];   // 权威型（NpcScript.h，元素 0x18）

#include "InitScriptProducers_4/InitSkillScript.inc"


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

#include "InitScriptProducers_4/InitEquipmentScript.inc"


// ORIG B 0x94fdb00 [0x100] / B 0x94fdc00 [0x18]
char g_szStackableScriptBaseDirectory[0x100];
STScriptFileList g_stStackableScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）

#include "InitScriptProducers_4/InitStackableScript.inc"


// ===================== 角色脚本域（InitCharacterScript）=============

// ORIG B 0x94fe840 [0x100] / B 0x94fe940 [0x18]
char g_szCharacterScriptBaseDirectory[0x100];
STScriptFileList g_stCharacterScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）
// ORIG B 0x94fe960 [0x320] / B 0x94fec80 [0x320]：静态成员定义（声明见头）。
int STCharacterScript::naExpTable[0xc8];
int STCharacterScript::pvpPrices_[0xc8];

#include "InitScriptProducers_4/InitCharacterScript.inc"


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
// ORIG D 0x93f4d20 [4]：脚本包安全开关，初值 1（objdump -s -j .data 原字节
// `01000000`；紧邻 ORIG 的 _ZL16s_bUseScriptPack d 0x93f4d24 = `01`，两者同段
// 相邻 ⇒ 同 TU 数据域）。ORIG 全库 `objdump -d` 无任何以 0x93f4d20 为操作数的
// 指令 ⇒ 纯数据符号对账项（无消费点），值逐字节直读、非推断。
int g_xUseScriptPackSecurity = 1;

// ORIG B 0x94ff160 [0x100] / B 0x94ff260 [0x18]
char g_szMonsterScriptBaseDirectory[0x100];
STScriptFileList g_stMonsterScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）

#include "InitScriptProducers_4/InitMonsterScript.inc"

#include "InitScriptProducers_4/InitMonsterScript_1.inc"

