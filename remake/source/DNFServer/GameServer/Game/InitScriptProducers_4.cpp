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
// bool 视图声明（P22：ORIG init* 尾 xor $1,%eax;test %al 为 bool 取反形；
// 返回类型不入 mangled，链接 STScriptFileList.cpp 的 int 定义恒 0/1，ABI 兼容）。
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

// ORIG 0x089dce98：列表登记后直接透传 loadRDARScriptListFile 结果。
// [b28d-solo-InitScriptProducers_4 AE] P303 __builtin_strcat + P304 权威型 &g_stMapScriptFileList
// （去 reinterpret_cast 物化）；43/43 逐条同构。
#include "InitScriptProducers_4/InitMapScript.inc"
{
    strncpy(g_szMapScriptBaseDirectory, dir, 0x100);
    // P49 禁 TU helper（原 AppendDirSeparator 属发明符号）：按 ORIG 汇编内联
    // strlen×3 + memcpy 2B "\\"，无空串防御（ORIG len==0 读 buf[-1] 原始 UB，P140）。
    if (g_szMapScriptBaseDirectory[strlen(g_szMapScriptBaseDirectory) - 1] != '\\' &&
        g_szMapScriptBaseDirectory[strlen(g_szMapScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szMapScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stMapScriptFileList,
                                g_szMapScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

// ORIG B 0x94ff2a0 [0x100] / B 0x94ff3a0 [0x18]
char g_szPassiveObjectScriptBaseDirectory[0x100];
STScriptFileList g_stPassiveObjectScriptFileList;   // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x08a29ead：与 InitMapScript 完全同构（不同全局）。
// [b28d-solo-InitScriptProducers_4 AE] 同 InitMapScript：P303 + P304 权威型；43/43。
#include "InitScriptProducers_4/InitPassiveObjectScript.inc"
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szPassiveObjectScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stPassiveObjectScriptFileList,
                                g_szPassiveObjectScriptBaseDirectory,
                                list, 0))
    {
        return 0;
    }
    return 1;
}

// ORIG 0x08a3c5c2（0xbb 字节）：与 2 参数版同构，仅把 map 透传
// loadRDARScriptListFile 第 4 参（ORIG 0x8a3c646 mov 0x10(%ebp)，
// 2 参数版同址为 movl $0）。
// [b28d-solo-InitScriptProducers_4 AE] 3 参版同构（第 4 参 map 透传）；44/44。
#include "InitScriptProducers_4/InitPassiveObjectScript_1.inc"
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szPassiveObjectScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stPassiveObjectScriptFileList,
                                g_szPassiveObjectScriptBaseDirectory,
                                list, map))
    {
        return 0;
    }
    return 1;
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
{
    strncpy(g_szDungeonScriptBaseDirectory, dir, 0x100);
    if (g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '\\' &&
        g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szDungeonScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stDungeonScriptFileList,
                                g_szDungeonScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
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
{
    strncpy(g_szSkillScriptBaseDirectory, dir, 0x100);
    if (g_szSkillScriptBaseDirectory[strlen(g_szSkillScriptBaseDirectory) - 1] != '\\' &&
        g_szSkillScriptBaseDirectory[strlen(g_szSkillScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szSkillScriptBaseDirectory, "\\");
    }

    STScriptFileList files;
    if (!loadRDARScriptListFile(&files, g_szSkillScriptBaseDirectory, list, 0))
        return false;

    // [b28d-lane16 P167/P28] ORIG 发非 const `_Rb_tree_iterator` 的 deEv/ppEi/ne
    // （p167 MISSING -3 deEv/ppEi/ne，EXTRA +2 const_iterator 转换构造 +ptEv/ppEv）
    // ⇒ 源形必须是非 const iterator + `(*it)` + 后置 `it++`。
    for (std::map<int, std::string>::iterator it = files.m_fileMap.begin();
         it != files.m_fileMap.end(); it++)
    {
        if ((*it).first > 10)               // ORIG cmp $0xa,setg → continue
            continue;
        // ORIG 目标元素地址 = 0x9500820 + (*it).first*0x18（二次 deEv，无具名 idx）
        if (!loadRDARScriptListFile(&g_staSkillScriptFileList[(*it).first],
                                    g_szSkillScriptBaseDirectory,
                                    (*it).second.c_str(), 0))
            return false;
    }
    return true;
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
                         const char* priceTable)
{
    strncpy(g_szEquipmentScriptBaseDirectory, dir, 0x100);
    if (g_szEquipmentScriptBaseDirectory[strlen(g_szEquipmentScriptBaseDirectory) - 1] != '\\' &&
        g_szEquipmentScriptBaseDirectory[strlen(g_szEquipmentScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szEquipmentScriptBaseDirectory, "\\");
    }

    if (!loadRDARScriptListFile(&g_stEquipmentScriptFileList,
                                g_szEquipmentScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_szEquipmentScriptBaseDirectory, priceTable))
        return false;

    // ORIG 三个外层循环复用同一 -0x14 槽（P160）；首循环双初始化（P191）。
    // 两个内层 j 为独立作用域 ⇒ 各自占槽（ORIG -0x10 / -0xc）。
    int i = 0;
    for (i = 0; i <= 29; ++i)               // ORIG cmpl $0x1d,setle
        for (int j = 0; j <= 99; ++j)       // ORIG cmpl $0x63,setle
            STEquipmentScript::priceTable_[i][j] = 0;

    std::string token;
    ScanType(token, true);                  // 首标记（结果不校验）
    for (i = 0; i <= 2; ++i)                // ORIG cmpl $0x2,setle
        itemPriceRates_[i] = ScanInt((bool*)0);

    ScanType(token, true);                  // ORIG 0x8e1602b "[repair cost]"
    if (token == "[repair cost]")
        itemRepairCost_ = ScanFloat((bool*)0);

    for (i = 0; i <= 29; ++i)               // ScanType 失败即正常收尾
    {
        if (!ScanType(token, true))
            break;
        for (int j = 0; j <= 99; ++j)       // ORIG ScanInt(NULL)
            STEquipmentScript::priceTable_[i][j] = ScanInt((bool*)0);
    }
    return true;
}

// ORIG B 0x94fdb00 [0x100] / B 0x94fdc00 [0x18]
char g_szStackableScriptBaseDirectory[0x100];
STScriptFileList g_stStackableScriptFileList;             // 权威型（NpcScript.h，sizeof 0x18）

// ORIG 0x089ae72e：列表登记 → pricetable.tbl 清零回填 30×100 主表。
// [b28d-solo-InitScriptProducers_4 AE] 同 Equipment：i 复用 + 双初始化 + 删死 bool ok；124/124。
#include "InitScriptProducers_4/InitStackableScript.inc"
                         const char* priceTable)
{
    strncpy(g_szStackableScriptBaseDirectory, dir, 0x100);
    if (g_szStackableScriptBaseDirectory[strlen(g_szStackableScriptBaseDirectory) - 1] != '\\' &&
        g_szStackableScriptBaseDirectory[strlen(g_szStackableScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szStackableScriptBaseDirectory, "\\");
    }

    if (!loadRDARScriptListFile(&g_stStackableScriptFileList,
                                g_szStackableScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, priceTable))
        return false;

    // ORIG 两个外层循环复用同一 -0x14 槽（P160）；首循环双初始化（P191）。
    int i = 0;
    for (i = 0; i <= 29; ++i)
        for (int j = 0; j <= 99; ++j)
            STStackableScript::priceTable_[i][j] = 0;

    std::string token;
    for (i = 0; i <= 29; ++i)
    {
        if (!ScanType(token, true))         // 无更多行 → 正常成功返回
            break;
        for (int j = 0; j <= 99; ++j)
            STStackableScript::priceTable_[i][j] = ScanInt((bool*)0);
    }
    return true;
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
                         const char* expTable, const char* pvpPriceTable)
{
    strncpy(g_szCharacterScriptBaseDirectory, dir, 0x100);
    if (g_szCharacterScriptBaseDirectory[strlen(g_szCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_szCharacterScriptBaseDirectory[strlen(g_szCharacterScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szCharacterScriptBaseDirectory, "\\");
    }

    if (!loadRDARScriptListFile(&g_stCharacterScriptFileList,
                                g_szCharacterScriptBaseDirectory, list, 0))
        return 0;

    // ORIG 四个 200 次循环复用同一 -0xc 槽（P160 复用 ORIG 复用的槽）
    int i = 0;                              // P191：ORIG 首循环双初始化两条 movl $0,-0xc
    for (i = 0; i <= 199; ++i)              // ORIG cmpl $0xc7,setle
        STCharacterScript::naExpTable[i] = 0;

    if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory, expTable))
        return 0;
    for (i = 0; i <= 199; ++i)
        STCharacterScript::naExpTable[i] = ScanInt((bool*)0);

    if (pvpPriceTable != 0)                 // ORIG cmpl $0x0,0x14(%ebp)
    {
        for (i = 0; i <= 199; ++i)
            STCharacterScript::pvpPrices_[i] = 0;
        if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory,
                                pvpPriceTable))
            return 0;
        for (i = 0; i <= 199; ++i)
            STCharacterScript::pvpPrices_[i] = ScanInt((bool*)0);
    }
    return 1;
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
{
    if (list != 0)                          // ORIG cmpl $0x0,0xc(%ebp)
    {
        strncpy(g_szMonsterScriptBaseDirectory, dir, 0x100);
        // ORIG 0x8a11342..0x8a113a0：本 TU 仅此两处**内联**分隔符判定
        // （strlen×2 短路 + strcat 内建展开 strlen+memcpy 2B），
        // 其余 *Script 段为真实 AppendDirSeparator 调用——勿用 helper。
        if (g_szMonsterScriptBaseDirectory[strlen(g_szMonsterScriptBaseDirectory) - 1] != '\\' &&
            g_szMonsterScriptBaseDirectory[strlen(g_szMonsterScriptBaseDirectory) - 1] != '/')
        {
            __builtin_strcat(g_szMonsterScriptBaseDirectory, "\\");
        }

        if (!loadRDARScriptListFile(&g_stMonsterScriptFileList,
                                    g_szMonsterScriptBaseDirectory, list, 0))
            return false;

        // ORIG 0x8a113d9: 临时空向量 → resize(5, 临时)（0x94ff278）。
        // 传具名 empty 会多发 vector C1ERKS + 2×D1Ev（ORIG 无）。
        g_monsterBaseParameters.resize(5, std::vector<MonsterBaseParameter>());

        for (int tableIdx = 0; tableIdx <= 4; ++tableIdx)
        {
            // ORIG 0x93f54c4[tableIdx] 文件名，经全局基目录打开。
            if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory,
                                    MONSTER_BASIS_PARAMETER_TABLE_NAMES[tableIdx]))
                return false;

            // [槽布局对齐] ORIG 0x8a1146a(first=0) → 0x8a11471(ok=false) →
            // 0x8a11478(param C1) → 0x8a11486(row=0)：三者声明在 row 循环**外**
            // （C1 全函数仅此一处，row 循环内不再构造）⇒ 每 tableIdx 重置一次。
            int first = 0;                          // ORIG -0x78 具名中转
            bool ok = false;
            MonsterBaseParameter param;             // ORIG -0x26c(%ebp)
            for (int row = 0;; ++row)       // ORIG -0x74(%ebp)
            {
                first = ScanInt(&ok);
                if (!ok)                                // ORIG 0x8a114aa jne
                    break;
                param.m_value[0] = first;
                // ORIG 列序（0x8a114b5..0x8a116a0）：3,4,5,6,1,2,7..12 + 两循环 + 21..23
                param.m_value[3] = ScanInt((bool*)0);
                param.m_value[4] = ScanInt((bool*)0);
                param.m_value[5] = ScanInt((bool*)0);
                param.m_value[6] = ScanInt((bool*)0);
                param.m_value[1] = ScanInt((bool*)0);
                param.m_value[2] = ScanInt((bool*)0);
                param.m_value[7] = ScanInt((bool*)0);
                param.m_value[8] = ScanInt((bool*)0);
                param.m_value[9] = ScanInt((bool*)0);
                param.m_value[10] = ScanInt((bool*)0);
                param.m_value[11] = ScanInt((bool*)0);
                param.m_value[12] = ScanInt((bool*)0);
                for (int k = 0; k <= 3; ++k)            // ORIG -0x70 循环
                    param.m_grid[3][k] = ScanInt((bool*)0);
                for (int k = 0; k <= 3; ++k)            // ORIG -0x6c 循环
                    param.m_grid[4][k] = ScanInt((bool*)0);
                param.m_value[21] = ScanInt((bool*)0);         // +0x54
                param.m_value[22] = ScanInt((bool*)0);         // +0x58
                param.m_value[23] = ScanInt((bool*)0);         // +0x5c

                // ORIG P157：operator[] 每用点独立取（size×2/push_back/resize/
                // 赋值共 5 次），勿建 vec 引用缓存。
                if (static_cast<unsigned int>(row) ==
                    g_monsterBaseParameters[tableIdx].size())
                    g_monsterBaseParameters[tableIdx].push_back(param);   // ORIG 0x8a11670
                // ORIG 0x8a1169a seta（size_t 无符号比较），勿 cast 成 int（会生 setl）。
                else if (static_cast<unsigned int>(row) >
                         g_monsterBaseParameters[tableIdx].size())
                {
                    // ORIG 0x8a116a1: 默认元素 resize(row+1) 后再赋值。
                    g_monsterBaseParameters[tableIdx].resize(row + 1, MonsterBaseParameter());
                }
                g_monsterBaseParameters[tableIdx][row] = param;          // ORIG rep movsl 0x18
            }
        }
    }

    // 注意：apcBonus 用原始 dir 形参打开（ORIG 0x8a11757 直接压 0x8(%ebp)）。
    if (!loadRDARScriptFile(dir, apcBonus))
    {
        LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                              0x7e6, "error : MonsterApcDifficultyBonus.tbl is missing!!");
        return false;
    }

    // ORIG 0x8a117a6/0x8a117b0/0x8a117b7 初始化序：movl $0,-0x84（原码遗留死
    // 局部，全函数从不读）→ movl $0,-0x80（r）→ movb $0,-0x151（ok）；声明序照抄
    // 以复现槽位（P232/P244：ORIG 未使用局部照抄）。
    int r = 0;
    int deadIdx = 0;                            // ORIG -0x84 [推断：原码遗留未使用局部]
    bool ok = false;
    // 难度加成主表：13 行 × 5 列（缺项报错行号 0x7fc，末行/末列豁免）。
    while (true)
    {
        for (int c = 0; c <= 4; ++c)
        {
            DIFFICULTY_OF_DUNGEON_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 4 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x7fc,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }
    // 死塔加成：13 行 × 4 列（行号 0x818）。
    r = 0;
    while (true)
    {
        for (int c = 0; c <= 3; ++c)
        {
            DEATH_TOWER_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 3 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x818,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }
    // 血条系统加成：13 行 × 4 列写入 5 列数组前 4 列（行号 0x833）。
    r = 0;
    while (true)
    {
        for (int c = 0; c <= 3; ++c)
        {
            BLOOD_SYSTEM_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 3 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x833,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }

    for (int k = 0; k <= 4; ++k)            // ORIG 0x8a119fd clear 循环
        MONSTER_DIFFICULTY_BONUS[k].clear();

    // ---- 标签化段（[easy]..[hero]/[dungeon party balance]/... ）----
    std::string token;
    for (;;)                                // ORIG 0x8a11a39 主循环头
    {
        if (!ScanType(token, true))         // ORIG 0x8a127e6 → 成功返回
            return true;

        // ORIG 429-462：5 连短路比较 + bool 物化，块内再逐项定 difficulty。
        if (token == "[easy]" || token == "[medium]" || token == "[hard]" ||
            token == "[ultimate]" || token == "[hero]")
        {
            int difficulty = 0;                 // ORIG -0x58
            if (token == "[easy]")
                difficulty = 0;                 // ORIG 0x8e1eceb
            else if (token == "[medium]")
                difficulty = 1;                 // 0x8e1ecf2
            else if (token == "[hard]")
                difficulty = 2;                 // 0x8e1ecfb
            else if (token == "[ultimate]")
                difficulty = 3;                 // 0x8e1ed02
            else if (token == "[hero]")
                difficulty = 4;                 // 0x8e1ed0d

            // ---- 难度组读取器（ORIG 0x8a11ba7）：每 consume 1 个弃置 float +
            //      最多 13 个 float 为一组；组不满或读失败则该难度段结束。
            for (;;)
            {
                ScanFloat(&ok);                 // 组头 float（fstp %st(0) 弃置）
                if (!ok)
                    break;                      // → 下一主标签
                std::vector<float> values;      // ORIG -0x168 局部向量
                values.clear();                 // ORIG 0x8383ad8
                values.reserve(13);             // ORIG reserve 0xd
                for (int i = 0; i <= 12; ++i)
                {
                    float f = ScanFloat(&ok);
                    if (!ok)
                        break;
                    values.push_back(f);
                }
                if (!ok)                        // 不完整组整体丢弃（ebx=0 路径）
                    break;
                MONSTER_DIFFICULTY_BONUS[difficulty].push_back(values);
            }
            continue;
        }
        else if (token == "[dungeon party balance]")    // 0x8e1ed14
        {
            DUNGEON_PARTY_BALANCE_TABLE.clear();
            for (;;)                        // ORIG 0x8a11cf2
            {
                DungeonPartyBalance entry;  // ORIG 栈复用 -0x26c 缓冲
                entry.m_a = static_cast<float>(ScanInt(&ok));
                if (!ok)
                    break;
                entry.m_b = static_cast<float>(ScanInt(&ok));
                if (!ok)
                    break;
                for (int r = 0; r <= 12; ++r)
                {
                    for (int c = 0; c <= 3; ++c)
                    {
                        entry.m_grid[r][c] = ScanFloat(&ok);
                        if (!ok)
                            break;
                    }
                    if (!ok)
                        break;
                }
                if (!ok)                    // 不完整行丢弃 → 下一个标签
                    break;
                DUNGEON_PARTY_BALANCE_TABLE.push_back(entry);
            }
            continue;
        }
        else if (token == "[mesmerization damage balance]")  // 0x8e1ed2c
        {
            // ORIG 顺序：sub C1(661) → h C1(665) → clear×3(667-671) → reserve×3(674-680)。
            std::string sub;
            int a;                          // ORIG -0x4c（具名中转，P0 槽往返）
            int b = 0;                      // ORIG -0x48（P198① 预清必写）
            DungeonHoldBalance h;           // ORIG 0x8a3deee @0x8a11e31：段首一次，三支复用
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.clear();       // ORIG 0x8a11e46
            NAMED_MONSTER_HOLD_BALANCE_TABLE.clear();
            BOSS_MONSTER_HOLD_BALANCE_TABLE.clear();
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8); // 各 reserve 200
            NAMED_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);
            BOSS_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);

            while (ScanType(sub, true))     // ORIG 0x8a12046
            {
                if (sub == "[/mesmerization damage balance]")   // 0x8e1ed4c
                    break;
                if (sub == "[normal]")      // 0x8e1ed6c
                {
                    for (int i = 0; i <= 199; ++i)          // cmpl $0xc7,setle
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        NORMAL_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[boss]")   // 0x8e1ed75
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        BOSS_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[named]")  // 0x8e1ed7c
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        NAMED_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
            }
            continue;
        }
        else if (token == "[limit slowdown]")               // 0x8e1ed84
        {
            // ORIG 函数级静态：__lookup map<const char*,int,less_TCHAR>
            // （0x94fff80，guard 0x94ff648）+ __init_lock SpinLock
            // （0x94fff98，guard 0x94ff650）+ __inited（0x94fff9c）。
            // ORIG 0x8a120b3..0x8a12678：P333「首次建表、之后查表」状态机形。
            // 段首 sub C1 → a=0(-0x38) → b=0(-0x34) → bStop=false(-0x2d) → e C1；
            // 每轮按 id switch 派发：未建表时各 case 体先查 __inited 再逐条 insert
            // 并 fallthrough（首轮一次插满 4 条），尾部 leave+置位+goto retry 重查；
            // 已建表时执行真实解析。id==0/0x932（[/limit slowdown]）置 bStop 后 break。
            std::string sub;
            int a = 0;                  // ORIG -0x38（三支复用，P198① 预清必写）
            int b = 0;                  // ORIG -0x34
            bool bStop = false;         // ORIG -0x2d
            stLimitSlowDown e;          // ORIG 0x8a3df1a @0x8a120d3：段首一次，三支复用
            int id;
            for (;;)
            {
                if (!ScanType(sub, true))   // ORIG 0x8a12649
                    break;                  // 段结束 → 下一主标签

                // P177：ORIG 的 static 三件套声明在循环体内（guard 每轮执行）。
                static std::map<const char*, int, less_TCHAR> __lookup;
                static SpinLock __init_lock;
                static bool __inited = false;

            retry:
                if (__inited)
                {
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(sub.c_str());           // ORIG 0x8a12218
                    id = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();                       // ORIG 0x8a122a2
                    if (__inited)
                    {
                        __init_lock.leave();                   // ORIG 0x8a122c0
                        goto retry;                     // 回跳重查（ORIG 0x8a122c6）
                    }
                    id = 0;                             // 建表哨兵
                }

                switch (id)
                {
                case 0:
                case 0x932:                             // [/limit slowdown] 终止标记
                    // P22/P133：ORIG 0x8a12312 movzbl __inited; xor $1; test; je <else>
                    // ⇒ 原码为 `if (!inited) {insert} else {bStop; break}` 负极性形。
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit slowdown]", 0x932));
                        // fall through
                    }
                    else
                    {
                        bStop = true;                   // ORIG 0x8a1236f
                        break;
                    }
                case 0x935:                             // [normal]
                    // ORIG 0x8a12378 movzbl __inited; xor $1; test; je <parse>
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal]", 0x935));
                        // fall through
                    }
                    else
                    {
                        NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.clear();   // 0x9494f8c
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);               // 首列索引（弃）
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                case 0x949:                             // [boss]
                    // ORIG 0x8a1245c movzbl __inited; xor $1; test; je <parse>
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss]", 0x949));
                        // fall through
                    }
                    else
                    {
                        BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.clear();     // 0x9494fa4
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                case 0x95d:                             // [named]
                    // ORIG 0x8a12540 movzbl __inited; xor $1; test; je <parse>；
                    // 0x8a1259b jmp <switch 尾>（插入分支 break，不 fallthrough）。
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[named]", 0x95d));
                        break;
                    }
                    else
                    {
                        NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.clear();    // 0x9494f98
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                default:                                // id==-1（未知子标签）
                    break;
                }

                if (!__inited)
                {
                    __init_lock.leave();                       // ORIG 0x8a12631
                    __inited = true;                      // ORIG 0x8a12636
                    goto retry;                         // ORIG 0x8a1263d 回跳重查
                }
                if (bStop)                              // ORIG 0x8a12643 cmpb
                    break;
            }
            continue;
        }
        else if (token == "[adjust hp gauge]")              // 0x8e1eda7
        {
            // ORIG 0x8a126c4 段首 float v = 1.0f（-0x1c，三支复用）；
            // ORIG 0x8a126cc 段首构造 DungeonHoldBalance 局部对象且从未使用
            // （纯编译产物）——P204 照抄：ORIG 无条件发射该 C1 调用。
            std::string sub;
            float v = 1.0f;                 // ORIG -0x1c
            DungeonHoldBalance h;
            (void)h;
            while (ScanType(sub, true))     // ORIG 0x8a1279a
            {
                if (sub == "[/adjust hp gauge]")                // 0x8e1edb9
                    break;
                if (sub == "[normal]")
                {
                    v = ScanFloat(&ok);
                    NORMAL_MONSTER_GAUGE_ADJUST = v;            // 0x93f4d10
                }
                else if (sub == "[boss]")
                {
                    v = ScanFloat(&ok);
                    BOSS_MONSTER_GAUGE_ADJUST = v;              // 0x93f4d18
                }
                else if (sub == "[named]")
                {
                    v = ScanFloat(&ok);
                    NAMED_MONSTER_GAUGE_ADJUST = v;             // 0x93f4d14
                }
            }
            continue;
        }
        else
        {
            continue;                       // 未识别标签：ORIG 0x8a126b0 je
        }
    }
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
                       const char* apcBonus)
{
    if (list != 0)                          // ORIG cmpl $0x0,0xc(%ebp)
    {
        strncpy(g_szMonsterScriptBaseDirectory, dir, 0x100);
        // ORIG 0x8a11342..0x8a113a0：本 TU 仅此两处**内联**分隔符判定
        // （strlen×2 短路 + strcat 内建展开 strlen+memcpy 2B），
        // 其余 *Script 段为真实 AppendDirSeparator 调用——勿用 helper。
        if (g_szMonsterScriptBaseDirectory[strlen(g_szMonsterScriptBaseDirectory) - 1] != '\\' &&
            g_szMonsterScriptBaseDirectory[strlen(g_szMonsterScriptBaseDirectory) - 1] != '/')
        {
            __builtin_strcat(g_szMonsterScriptBaseDirectory, "\\");
        }

        if (!loadRDARScriptListFile(&g_stMonsterScriptFileList,
                                    g_szMonsterScriptBaseDirectory, list, map))
            return false;

        // ORIG 0x8a113d9: 临时空向量 → resize(5, 临时)（0x94ff278）。
        // 传具名 empty 会多发 vector C1ERKS + 2×D1Ev（ORIG 无）。
        g_monsterBaseParameters.resize(5, std::vector<MonsterBaseParameter>());

        for (int tableIdx = 0; tableIdx <= 4; ++tableIdx)
        {
            // ORIG 0x93f54c4[tableIdx] 文件名，经全局基目录打开。
            if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory,
                                    MONSTER_BASIS_PARAMETER_TABLE_NAMES[tableIdx]))
                return false;

            // [槽布局对齐] ORIG 0x8a3b205(first=0) → 0x8a3b20c(ok=false) →
            // param C1 → row=0：三者声明在 row 循环**外**（C1 每 tableIdx 仅一次，
            // row 循环内不再构造）⇒ 与 3 参版同构。
            int first = 0;                          // ORIG -0x78 具名中转
            bool ok = false;
            MonsterBaseParameter param;             // ORIG -0x26c(%ebp)
            for (int row = 0;; ++row)       // ORIG -0x74(%ebp)
            {
                first = ScanInt(&ok);
                if (!ok)                                // ORIG 0x8a3b245 jne
                    break;
                param.m_value[0] = first;
                // ORIG 列序（0x8a114b5..0x8a116a0）：3,4,5,6,1,2,7..12 + 两循环 + 21..23
                param.m_value[3] = ScanInt((bool*)0);
                param.m_value[4] = ScanInt((bool*)0);
                param.m_value[5] = ScanInt((bool*)0);
                param.m_value[6] = ScanInt((bool*)0);
                param.m_value[1] = ScanInt((bool*)0);
                param.m_value[2] = ScanInt((bool*)0);
                param.m_value[7] = ScanInt((bool*)0);
                param.m_value[8] = ScanInt((bool*)0);
                param.m_value[9] = ScanInt((bool*)0);
                param.m_value[10] = ScanInt((bool*)0);
                param.m_value[11] = ScanInt((bool*)0);
                param.m_value[12] = ScanInt((bool*)0);
                for (int k = 0; k <= 3; ++k)            // ORIG -0x70 循环
                    param.m_grid[3][k] = ScanInt((bool*)0);
                for (int k = 0; k <= 3; ++k)            // ORIG -0x6c 循环
                    param.m_grid[4][k] = ScanInt((bool*)0);
                param.m_value[21] = ScanInt((bool*)0);         // +0x54
                param.m_value[22] = ScanInt((bool*)0);         // +0x58
                param.m_value[23] = ScanInt((bool*)0);         // +0x5c

                // ORIG P157：operator[] 每用点独立取（size×2/push_back/resize/
                // 赋值共 5 次），勿建 vec 引用缓存。
                if (static_cast<unsigned int>(row) ==
                    g_monsterBaseParameters[tableIdx].size())
                    g_monsterBaseParameters[tableIdx].push_back(param);   // ORIG 0x8a11670
                // ORIG 0x8a1169a seta（size_t 无符号比较），勿 cast 成 int（会生 setl）。
                else if (static_cast<unsigned int>(row) >
                         g_monsterBaseParameters[tableIdx].size())
                {
                    // ORIG 0x8a116a1: 默认元素 resize(row+1) 后再赋值。
                    g_monsterBaseParameters[tableIdx].resize(row + 1, MonsterBaseParameter());
                }
                g_monsterBaseParameters[tableIdx][row] = param;          // ORIG rep movsl 0x18
            }
        }
    }

    // 注意：apcBonus 用原始 dir 形参打开（ORIG 0x8a11757 直接压 0x8(%ebp)）。
    if (!loadRDARScriptFile(dir, apcBonus))
    {
        LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                              0x2b22, "error : MonsterApcDifficultyBonus.tbl is missing!!");
        return false;
    }

    // 同 3 参版：[ORIG 初始化序] movl $0,-0x84（原码遗留死局部）→ movl $0,-0x80(r)
    // → movb $0,-0x151(ok)。声明序照抄以复现槽位。
    int deadIdx = 0;                            // ORIG -0x84 [推断：原码遗留未使用局部]
    int r = 0;
    bool ok = false;
    // 难度加成主表：13 行 × 5 列（缺项报错行号 0x7fc，末行/末列豁免）。
    while (true)
    {
        for (int c = 0; c <= 4; ++c)
        {
            DIFFICULTY_OF_DUNGEON_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 4 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2b38,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }
    // 死塔加成：13 行 × 4 列（行号 0x818）。
    r = 0;
    while (true)
    {
        for (int c = 0; c <= 3; ++c)
        {
            DEATH_TOWER_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 3 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2b54,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }
    // 血条系统加成：13 行 × 4 列写入 5 列数组前 4 列（行号 0x833）。
    r = 0;
    while (true)
    {
        for (int c = 0; c <= 3; ++c)
        {
            BLOOD_SYSTEM_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 3 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2b6f,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }

    for (int k = 0; k <= 4; ++k)            // ORIG 0x8a119fd clear 循环
        MONSTER_DIFFICULTY_BONUS[k].clear();

    // ---- 标签化段（[easy]..[hero]/[dungeon party balance]/... ）----
    std::string token;
    for (;;)                                // ORIG 0x8a11a39 主循环头
    {
        if (!ScanType(token, true))         // ORIG 0x8a127e6 → 成功返回
            return true;

        // ORIG 429-462：5 连短路比较 + bool 物化，块内再逐项定 difficulty。
        if (token == "[easy]" || token == "[medium]" || token == "[hard]" ||
            token == "[ultimate]" || token == "[hero]")
        {
            int difficulty = 0;                 // ORIG -0x58
            if (token == "[easy]")
                difficulty = 0;                 // ORIG 0x8e1eceb
            else if (token == "[medium]")
                difficulty = 1;                 // 0x8e1ecf2
            else if (token == "[hard]")
                difficulty = 2;                 // 0x8e1ecfb
            else if (token == "[ultimate]")
                difficulty = 3;                 // 0x8e1ed02
            else if (token == "[hero]")
                difficulty = 4;                 // 0x8e1ed0d

            // ---- 难度组读取器（ORIG 0x8a11ba7）：每 consume 1 个弃置 float +
            //      最多 13 个 float 为一组；组不满或读失败则该难度段结束。
            for (;;)
            {
                ScanFloat(&ok);                 // 组头 float（fstp %st(0) 弃置）
                if (!ok)
                    break;                      // → 下一主标签
                std::vector<float> values;      // ORIG -0x168 局部向量
                values.clear();                 // ORIG 0x8383ad8
                values.reserve(13);             // ORIG reserve 0xd
                for (int i = 0; i <= 12; ++i)
                {
                    float f = ScanFloat(&ok);
                    if (!ok)
                        break;
                    values.push_back(f);
                }
                if (!ok)                        // 不完整组整体丢弃（ebx=0 路径）
                    break;
                MONSTER_DIFFICULTY_BONUS[difficulty].push_back(values);
            }
            continue;
        }
        else if (token == "[dungeon party balance]")    // 0x8e1ed14
        {
            DUNGEON_PARTY_BALANCE_TABLE.clear();
            for (;;)                        // ORIG 0x8a11cf2
            {
                DungeonPartyBalance entry;  // ORIG 栈复用 -0x26c 缓冲
                entry.m_a = static_cast<float>(ScanInt(&ok));
                if (!ok)
                    break;
                entry.m_b = static_cast<float>(ScanInt(&ok));
                if (!ok)
                    break;
                for (int r = 0; r <= 12; ++r)
                {
                    for (int c = 0; c <= 3; ++c)
                    {
                        entry.m_grid[r][c] = ScanFloat(&ok);
                        if (!ok)
                            break;
                    }
                    if (!ok)
                        break;
                }
                if (!ok)                    // 不完整行丢弃 → 下一个标签
                    break;
                DUNGEON_PARTY_BALANCE_TABLE.push_back(entry);
            }
            continue;
        }
        else if (token == "[mesmerization damage balance]")  // 0x8e1ed2c
        {
            // ORIG 顺序：sub C1(661) → h C1(665) → clear×3(667-671) → reserve×3(674-680)。
            std::string sub;
            int a;                          // ORIG -0x4c（具名中转，P0 槽往返）
            int b = 0;                      // ORIG -0x48（P198① 预清必写）
            DungeonHoldBalance h;           // ORIG 0x8a3deee @0x8a3bbd5：段首一次，三支复用
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.clear();       // ORIG 0x8a11e46
            NAMED_MONSTER_HOLD_BALANCE_TABLE.clear();
            BOSS_MONSTER_HOLD_BALANCE_TABLE.clear();
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8); // 各 reserve 200
            NAMED_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);
            BOSS_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);

            while (ScanType(sub, true))     // ORIG 0x8a12046
            {
                if (sub == "[/mesmerization damage balance]")   // 0x8e1ed4c
                    break;
                if (sub == "[normal]")      // 0x8e1ed6c
                {
                    for (int i = 0; i <= 199; ++i)          // cmpl $0xc7,setle
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        NORMAL_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[boss]")   // 0x8e1ed75
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        BOSS_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[named]")  // 0x8e1ed7c
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        NAMED_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
            }
            continue;
        }
        else if (token == "[limit slowdown]")               // 0x8e1ed84
        {
            // ORIG 函数级静态：__lookup map<const char*,int,less_TCHAR>
            // （0x94ff8a8，guard 0x94ff8b0）+ __init_lock SpinLock
            // （0x94ff8b8，guard 0x94ff8b0 复用）+ __inited（0x94ff8d4）。
            // ORIG 0x8a3be4e..0x8a3c413：P333「首次建表、之后查表」状态机形。
            // 段首 sub C1 → a=0(-0x38) → b=0(-0x34) → bStop=false(-0x2d) → e C1；
            // 每轮按 id switch 派发：未建表时各 case 体先查 __inited 再逐条 insert
            // 并 fallthrough（首轮一次插满 4 条），尾部 leave+置位+goto retry 重查；
            // 已建表时执行真实解析。id==0/0x932（[/limit slowdown]）置 bStop 后 break。
            std::string sub;
            int a = 0;                  // ORIG -0x38（三支复用，P198① 预清必写）
            int b = 0;                  // ORIG -0x34
            bool bStop = false;         // ORIG -0x2d
            stLimitSlowDown e;          // ORIG 0x8a3df1a：段首一次，三支复用
            int id;
            for (;;)
            {
                if (!ScanType(sub, true))   // ORIG 0x8a3c3f5
                    break;                  // 段结束 → 下一主标签

                // P177：ORIG 的 static 三件套声明在循环体内（guard 每轮执行）。
                static std::map<const char*, int, less_TCHAR> __lookup;
                static SpinLock __init_lock;
                static bool __inited = false;

            retry:
                if (__inited)
                {
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(sub.c_str());
                    id = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto retry;                     // 回跳重查
                    }
                    id = 0;                             // 建表哨兵
                }

                switch (id)
                {
                case 0:
                case 0x2c6e:                            // [/limit slowdown] 终止标记
                    // P22/P133：ORIG 0x8a3c0ad movzbl __inited; xor $1; test; je 8a3c10a
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit slowdown]", 0x2c6e));
                        // fall through
                    }
                    else
                    {
                        bStop = true;
                        break;
                    }
                case 0x2c71:                            // [normal]
                    // ORIG 0x8a3c113 movzbl __inited; xor $1; test; je 8a3c173
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal]", 0x2c71));
                        // fall through
                    }
                    else
                    {
                        NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.clear();
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);               // 首列索引（弃）
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                case 0x2c85:                            // [boss]
                    // ORIG 0x8a3c1f7 movzbl __inited; xor $1; test; je 8a3c257
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss]", 0x2c85));
                        // fall through
                    }
                    else
                    {
                        BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.clear();
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                case 0x2c99:                            // [named]
                    // ORIG 0x8a3c2db movzbl __inited; xor $1; test; je 8a3c338
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[named]", 0x2c99));
                        break;
                    }
                    else
                    {
                        NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.clear();
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                default:                                // id==-1（未知子标签）
                    break;
                }

                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    goto retry;                         // 回跳重查
                }
                if (bStop)
                    break;
            }
            continue;
        }
        else if (token == "[adjust hp gauge]")              // 0x8e1eda7
        {
            // ORIG 段首 float v = 1.0f（-0x1c，三支复用）；
            // ORIG 段首构造 DungeonHoldBalance 局部对象且从未使用
            // （纯编译产物）——P204 照抄：ORIG 无条件发射该 C1 调用。
            std::string sub;
            float v = 1.0f;                 // ORIG -0x1c
            DungeonHoldBalance h;
            (void)h;
            while (ScanType(sub, true))     // ORIG 0x8a1279a
            {
                if (sub == "[/adjust hp gauge]")                // 0x8e1edb9
                    break;
                if (sub == "[normal]")
                {
                    v = ScanFloat(&ok);
                    NORMAL_MONSTER_GAUGE_ADJUST = v;            // 0x93f4d10
                }
                else if (sub == "[boss]")
                {
                    v = ScanFloat(&ok);
                    BOSS_MONSTER_GAUGE_ADJUST = v;              // 0x93f4d18
                }
                else if (sub == "[named]")
                {
                    v = ScanFloat(&ok);
                    NAMED_MONSTER_GAUGE_ADJUST = v;             // 0x93f4d14
                }
            }
            continue;
        }
        else
        {
            continue;                       // 未识别标签：ORIG 0x8a126b0 je
        }
    }
}
