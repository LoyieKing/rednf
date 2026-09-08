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
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* map);

// ---- STScriptFileList（布局权威：STScriptFileList.cpp 同名本地类，
//      sizeof 0x18 = std::map<int,std::string>；ORIG ctor 0x088bacf8）----


// ===================== 地图脚本域（InitMapScript）=====================

// ORIG B 0x94fe2a0 [0x100] / B 0x94fe3a0 [0x18]
char g_szMapScriptBaseDirectory[0x100];
std::map<int, std::string> g_stMapScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）

// ORIG 0x089dce98：列表登记后直接透传 loadRDARScriptListFile 结果。
bool InitMapScript(const char* dir, const char* list)
{
    strncpy(g_szMapScriptBaseDirectory, dir, 0x100);
    // P49 禁 TU helper（原 AppendDirSeparator 属发明符号）：按 ORIG 汇编内联
    // strlen×3 + memcpy 2B "\\"，无空串防御（ORIG len==0 读 buf[-1] 原始 UB，P140）。
    if (g_szMapScriptBaseDirectory[strlen(g_szMapScriptBaseDirectory) - 1] != '\\' &&
        g_szMapScriptBaseDirectory[strlen(g_szMapScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szMapScriptBaseDirectory + strlen(g_szMapScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stMapScriptFileList),
                                g_szMapScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

// ORIG B 0x94ff2a0 [0x100] / B 0x94ff3a0 [0x18]
char g_szPassiveObjectScriptBaseDirectory[0x100];
std::map<int, std::string> g_stPassiveObjectScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）

// ORIG 0x08a29ead：与 InitMapScript 完全同构（不同全局）。
bool InitPassiveObjectScript(const char* dir, const char* list)
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szPassiveObjectScriptBaseDirectory + strlen(g_szPassiveObjectScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stPassiveObjectScriptFileList),
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
bool InitPassiveObjectScript(const char* dir, const char* list, std::map<int, int>* map)
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szPassiveObjectScriptBaseDirectory + strlen(g_szPassiveObjectScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stPassiveObjectScriptFileList),
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
std::map<int, std::string> g_stDungeonScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）
std::map<int, std::string> g_questScriptFileList;       // ORIG 0x9500480（CListTables 引用）

// ORIG 0x088e5e3c：签名 char*（调用点传字符串字面量，ORIG mangled PcS_）。
// [b28c-L23 N(P209 附 e19 / P75)] 残 12 行 = memcpy(base+strlen(base),"\\",2) 段的
// 跨版本实参装载时刻（4.1 先压 size/src、4.4 先算 dest）+ 由此连锁的 4 处 jmp
// 目标偏移。strncpy/strlen×3/双条件 `!='\\' && !='/'` 与 loadRDARScriptListFile
// 调用体均逐条对齐。语义与 ORIG 一致。
bool InitDungeonScript(char* dir, char* list)
{
    strncpy(g_szDungeonScriptBaseDirectory, dir, 0x100);
    if (g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '\\' &&
        g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szDungeonScriptBaseDirectory + strlen(g_szDungeonScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stDungeonScriptFileList),
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
std::map<int, std::string> g_staSkillScriptFileList[11];   // 元素布局同 STScriptFileList

// ORIG 0x08a97438：先整表装入局部 STScriptFileList，再按条目分Job 装载；
// idx > 10 的条目跳过；任一分表装载失败即整体失败。
bool InitSkillScript(const char* dir, const char* list)
{
    strncpy(g_szSkillScriptBaseDirectory, dir, 0x100);
    if (g_szSkillScriptBaseDirectory[strlen(g_szSkillScriptBaseDirectory) - 1] != '\\' &&
        g_szSkillScriptBaseDirectory[strlen(g_szSkillScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szSkillScriptBaseDirectory + strlen(g_szSkillScriptBaseDirectory), "\\", 2);
    }

    STScriptFileList files;
    if (!loadRDARScriptListFile(&files, g_szSkillScriptBaseDirectory, list, 0))
        return false;

    for (std::map<int, std::string>::const_iterator it = files.m_fileMap.begin();
         it != files.m_fileMap.end(); ++it)
    {
        const int idx = it->first;
        if (idx > 10)                       // ORIG cmp $0xa,setg → continue
            continue;
        // ORIG 目标元素地址 = 0x9500820 + idx*0x18
        if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_staSkillScriptFileList[idx]),
                                    g_szSkillScriptBaseDirectory,
                                    it->second.c_str(), 0))
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
std::map<int, std::string> g_stEquipmentScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）

// ORIG 0x08991601：列表登记 → pricetable.tbl 清零回填 → 头 3 个价格系数 →
// "[repair cost]" 可选段 → 30 行×100 列主表。
bool InitEquipmentScript(const char* dir, const char* list,
                         const char* priceTable)
{
    strncpy(g_szEquipmentScriptBaseDirectory, dir, 0x100);
    if (g_szEquipmentScriptBaseDirectory[strlen(g_szEquipmentScriptBaseDirectory) - 1] != '\\' &&
        g_szEquipmentScriptBaseDirectory[strlen(g_szEquipmentScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szEquipmentScriptBaseDirectory + strlen(g_szEquipmentScriptBaseDirectory), "\\", 2);
    }

    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stEquipmentScriptFileList),
                                g_szEquipmentScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_szEquipmentScriptBaseDirectory, priceTable))
        return false;

    for (int i = 0; i <= 29; ++i)           // ORIG cmpl $0x1d,setle
        for (int j = 0; j <= 99; ++j)       // ORIG cmpl $0x63,setle
            STEquipmentScript::priceTable_[i][j] = 0;

    std::string token;
    ScanType(token, true);                  // 首标记（结果不校验）
    for (int i = 0; i <= 2; ++i)            // ORIG cmpl $0x2,setle
        itemPriceRates_[i] = ScanInt((bool*)0);

    ScanType(token, true);                  // ORIG 0x8e1602b "[repair cost]"
    if (token == "[repair cost]")
        itemRepairCost_ = ScanFloat((bool*)0);

    bool ok = false;
    for (int i = 0; i <= 29; ++i)           // ScanType 失败即正常收尾
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
std::map<int, std::string> g_stStackableScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）

// ORIG 0x089ae72e：列表登记 → pricetable.tbl 清零回填 30×100 主表。
bool InitStackableScript(const char* dir, const char* list,
                         const char* priceTable)
{
    strncpy(g_szStackableScriptBaseDirectory, dir, 0x100);
    if (g_szStackableScriptBaseDirectory[strlen(g_szStackableScriptBaseDirectory) - 1] != '\\' &&
        g_szStackableScriptBaseDirectory[strlen(g_szStackableScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szStackableScriptBaseDirectory + strlen(g_szStackableScriptBaseDirectory), "\\", 2);
    }

    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stStackableScriptFileList),
                                g_szStackableScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, priceTable))
        return false;

    for (int i = 0; i <= 29; ++i)
        for (int j = 0; j <= 99; ++j)
            STStackableScript::priceTable_[i][j] = 0;

    std::string token;
    bool ok = false;
    for (int i = 0; i <= 29; ++i)
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
std::map<int, std::string> g_stCharacterScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）
// ORIG B 0x94fe960 [0x320] / B 0x94fec80 [0x320]：静态成员定义（声明见头）。
int STCharacterScript::naExpTable[0xc8];
int STCharacterScript::pvpPrices_[0xc8];

// ORIG 0x08a0cc3c：Character.lst 登记 → ExpTable.tbl 回填 naExpTable[200] →
// 第 4 参非 NULL 时再加载对应 pvp 价格表回填 pvpPrices_[200]。
// （DataManagerInitDepends.h 第 4 参注释作 unused，与 ORIG 0x14(%ebp) 分支
//  不符——真实语义为可选 pvp 价格表路径，此处按 ORIG 实现。）
bool InitCharacterScript(const char* dir, const char* list,
                         const char* expTable, const char* pvpPriceTable)
{
    strncpy(g_szCharacterScriptBaseDirectory, dir, 0x100);
    if (g_szCharacterScriptBaseDirectory[strlen(g_szCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_szCharacterScriptBaseDirectory[strlen(g_szCharacterScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_szCharacterScriptBaseDirectory + strlen(g_szCharacterScriptBaseDirectory), "\\", 2);
    }

    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stCharacterScriptFileList),
                                g_szCharacterScriptBaseDirectory, list, 0))
        return 0;

    for (int i = 0; i <= 199; ++i)          // ORIG cmpl $0xc7,setle
        STCharacterScript::naExpTable[i] = 0;

    if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory, expTable))
        return 0;
    for (int i = 0; i <= 199; ++i)
        STCharacterScript::naExpTable[i] = ScanInt((bool*)0);

    if (pvpPriceTable != 0)                 // ORIG cmpl $0x0,0x14(%ebp)
    {
        for (int i = 0; i <= 199; ++i)
            STCharacterScript::pvpPrices_[i] = 0;
        if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory,
                                pvpPriceTable))
            return 0;
        for (int i = 0; i <= 199; ++i)
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
    int m_value[FIELD_COUNT];               // +0x00..+0x5c，按解析顺序填充

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
            m_value[(i + 12) + 1] = 0;
            m_value[(i + 16) + 1] = 0;
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
    DungeonPartyBalance() { clear(); }
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
float DIFFICULTY_OF_DUNGEON_BONUS[13][5]  = { { 1.0f } };
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
std::map<int, std::string> g_stMonsterScriptFileList;   // 布局同 STScriptFileList（0x18，双视图）

// ORIG 0x08a11311（0x1516 字节）：
//   list==NULL → 跳过目录/列表/基础参数表，直入 APC 加成表段；
//   否则登记 Monster.lst → resize(5) 基础参数向量 → 五张 *BaseParameter.tbl
//   逐行 MonsterBaseParameter 回填 → MonsterApcDifficultyBonus.tbl 三张
//   13 行浮点加成表 → 标签化难度/平衡/减速/血量调整段。
bool InitMonsterScript(const char* dir, const char* list, const char* apcBonus)
{
    if (list != 0)                          // ORIG cmpl $0x0,0xc(%ebp)
    {
        strncpy(g_szMonsterScriptBaseDirectory, dir, 0x100);
        // ORIG 0x8a11342..0x8a113a0：本 TU 仅此两处**内联**分隔符判定
        // （strlen×2 短路 + strcat 内建展开 strlen+memcpy 2B），
        // 其余 *Script 段为真实 AppendDirSeparator 调用——勿用 helper。
        {
            char* b = g_szMonsterScriptBaseDirectory;
            if (b[strlen(b) - 1] != '\\' && b[strlen(b) - 1] != '/')
                strcat(b, "/");
        }

        if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stMonsterScriptFileList),
                                    g_szMonsterScriptBaseDirectory, list, 0))
            return false;

        // ORIG 0x8a113d9: 局部空向量 → resize(5, empty)（0x94ff278）。
        {
            std::vector<MonsterBaseParameter> empty;
            g_monsterBaseParameters.resize(5, empty);
        }

        for (int tableIdx = 0; tableIdx <= 4; ++tableIdx)
        {
            // ORIG 0x93f54c4[tableIdx] 文件名，经全局基目录打开。
            if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory,
                                    MONSTER_BASIS_PARAMETER_TABLE_NAMES[tableIdx]))
                return false;

            bool ok = false;
            for (int row = 0;; ++row)       // ORIG -0x74(%ebp)
            {
                MonsterBaseParameter param;             // ORIG -0x26c(%ebp)
                param.m_value[0] = ScanInt(&ok);        // 首列驱动循环
                if (!ok)                                // ORIG 0x8a114a8 jne
                    break;
                // ORIG 4.1 逐列展开（0x8a114b5..0x8a116a0，无 k 循环）
                param.m_value[1] = ScanInt((bool*)0);
                param.m_value[2] = ScanInt((bool*)0);
                param.m_value[3] = ScanInt((bool*)0);
                param.m_value[4] = ScanInt((bool*)0);
                param.m_value[5] = ScanInt((bool*)0);
                param.m_value[6] = ScanInt((bool*)0);
                param.m_value[7] = ScanInt((bool*)0);
                param.m_value[8] = ScanInt((bool*)0);
                param.m_value[9] = ScanInt((bool*)0);
                param.m_value[10] = ScanInt((bool*)0);
                param.m_value[11] = ScanInt((bool*)0);
                param.m_value[12] = ScanInt((bool*)0);
                param.m_value[13] = ScanInt((bool*)0);
                param.m_value[14] = ScanInt((bool*)0);
                param.m_value[15] = ScanInt((bool*)0);
                param.m_value[16] = ScanInt((bool*)0);
                param.m_value[17] = ScanInt((bool*)0);
                param.m_value[18] = ScanInt((bool*)0);
                param.m_value[19] = ScanInt((bool*)0);
                param.m_value[20] = ScanInt((bool*)0);
                param.m_value[21] = ScanInt((bool*)0);         // +0x54
                param.m_value[22] = ScanInt((bool*)0);         // +0x58
                param.m_value[23] = ScanInt((bool*)0);         // +0x5c

                // ORIG P157：operator[] 每用点独立取（size×2/push_back/resize/
                // 赋值共 5 次），勿建 vec 引用缓存。
                if (row == static_cast<int>(g_monsterBaseParameters[tableIdx].size()))
                    g_monsterBaseParameters[tableIdx].push_back(param);   // ORIG 0x8a11670
                else if (row > static_cast<int>(g_monsterBaseParameters[tableIdx].size()))
                {
                    // ORIG 0x8a116a1: 默认元素 resize(row+1) 后再赋值。
                    MonsterBaseParameter def;
                    g_monsterBaseParameters[tableIdx].resize(row + 1, def);
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

    bool ok = false;
    // 难度加成主表：13 行 × 5 列（缺项报错行号 0x7fc，末行/末列豁免）。
    for (int r = 0; r <= 12; ++r)
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
        if (!ok)
            break;
    }
    // 死塔加成：13 行 × 4 列（行号 0x818）。
    for (int r = 0; r <= 12; ++r)
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
        if (!ok)
            break;
    }
    // 血条系统加成：13 行 × 4 列写入 5 列数组前 4 列（行号 0x833）。
    for (int r = 0; r <= 12; ++r)
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
        if (!ok)
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

        int difficulty = 0;
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
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.clear();       // ORIG 0x8a11e46
            NAMED_MONSTER_HOLD_BALANCE_TABLE.clear();
            BOSS_MONSTER_HOLD_BALANCE_TABLE.clear();
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8); // 各 reserve 200
            NAMED_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);
            BOSS_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);

            std::string sub;
            while (ScanType(sub, true))     // ORIG 0x8a12046
            {
                if (sub == "[/mesmerization damage balance]")   // 0x8e1ed4c
                    break;
                if (sub == "[normal]")      // 0x8e1ed6c
                {
                    for (int i = 0; i <= 199; ++i)          // cmpl $0xc7,setle
                    {
                        DungeonHoldBalance h;
                        h.m_a = ScanInt((bool*)0);
                        h.m_b = ScanInt((bool*)0);
                        NORMAL_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[boss]")   // 0x8e1ed75
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        DungeonHoldBalance h;
                        h.m_a = ScanInt((bool*)0);
                        h.m_b = ScanInt((bool*)0);
                        BOSS_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[named]")  // 0x8e1ed7c
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        DungeonHoldBalance h;
                        h.m_a = ScanInt((bool*)0);
                        h.m_b = ScanInt((bool*)0);
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
            // 首次进入锁内插入四条 tag→id（id 为 ORIG 源内常量 0x932/0x935/
            // 0x949/0x95d），置 inited 后重派发同一 token 进入真实解析分支。
            static std::map<const char*, int, less_TCHAR> lookup;
            static SpinLock lock;
            static bool inited = false;

            std::string sub;
            for (;;)
            {
                if (!ScanType(sub, true))   // ORIG 0x8a12649
                    break;                  // 段结束 → 下一主标签

                if (!inited)
                {
                    lock.enter();                       // ORIG 0x8a122a2
                    if (!inited)
                    {
                        // ORIG 插入链 0x8a12320/0x8a12386/0x8a1246a/0x8a1254e
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit slowdown]", 0x932));
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal]", 0x935));
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss]", 0x949));
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[named]", 0x95d));
                        lock.leave();                   // ORIG 0x8a1262a
                        inited = true;
                    }
                    else
                    {
                        lock.leave();
                    }
                }

                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    lookup.find(sub.c_str());           // ORIG 0x8a12218
                const int id = (it != lookup.end()) ? it->second : -1;

                if (id == 0 || id == 0x932)             // [/limit slowdown] 终止标记
                    continue;
                if (id == 0x935)                        // [normal]
                {
                    NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.clear();   // 0x9494f8c
                    for (int i = 0; i <= 199; ++i)
                    {
                        ScanInt((bool*)0);                     // 首列索引（弃）
                        stLimitSlowDown e;
                        e.m_a = ScanInt((bool*)0);
                        e.m_b = ScanInt((bool*)0);
                        NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                    }
                    continue;
                }
                if (id == 0x949)                        // [boss]
                {
                    BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.clear();     // 0x9494fa4
                    for (int i = 0; i <= 199; ++i)
                    {
                        ScanInt((bool*)0);
                        stLimitSlowDown e;
                        e.m_a = ScanInt((bool*)0);
                        e.m_b = ScanInt((bool*)0);
                        BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                    }
                    continue;
                }
                if (id == 0x95d)                        // [named]
                {
                    NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.clear();    // 0x9494f98
                    for (int i = 0; i <= 199; ++i)
                    {
                        ScanInt((bool*)0);
                        stLimitSlowDown e;
                        e.m_a = ScanInt((bool*)0);
                        e.m_b = ScanInt((bool*)0);
                        NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                    }
                    continue;
                }
                // id==-1（未知子标签）：ORIG 直落 0x8a1261c 继续。
            }
            continue;
        }
        else if (token == "[adjust hp gauge]")              // 0x8e1eda7
        {
            // 注：ORIG 0x8a126cc 在段首构造了一个 DungeonHoldBalance 局部对象，
            // 之后从未使用（纯编译产物），无可观行为，故不复刻。
            std::string sub;
            while (ScanType(sub, true))     // ORIG 0x8a1279a
            {
                if (sub == "[/adjust hp gauge]")                // 0x8e1edb9
                    break;
                if (sub == "[normal]")
                {
                    float v = ScanFloat((bool*)0);
                    NORMAL_MONSTER_GAUGE_ADJUST = v;            // 0x93f4d10
                }
                else if (sub == "[boss]")
                {
                    float v = ScanFloat((bool*)0);
                    BOSS_MONSTER_GAUGE_ADJUST = v;              // 0x93f4d18
                }
                else if (sub == "[named]")
                {
                    float v = ScanFloat((bool*)0);
                    NAMED_MONSTER_GAUGE_ADJUST = v;             // 0x93f4d14
                }
            }
            continue;
        }
        else
        {
            continue;                       // 未识别标签：ORIG 0x8a126b0 je
        }

        // ---- 难度组读取器（ORIG 0x8a11ba7）：每 consume 1 个弃置 float +
        //      最多 13 个 float 为一组；组不满或读失败则该难度段结束。
        for (;;)
        {
            ScanFloat(&ok);                 // 组头 float（fstp %st(0) 弃置）
            if (!ok)
                break;                      // → 下一主标签
            std::vector<float> values;      // ORIG -0x168 局部向量
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
    }
}
// ORIG 0x08a3b0ad（0x1515 字节）：带 map 形参重载
// _Z17InitMonsterScriptPKcS0_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEES0_。
// 与 3 参数版（0x08a11311）归一化 diff 仅一处逻辑差异：loadRDARScriptListFile
// 第 4 参装载 0x10(%ebp)（map）而非 movl $0；其余指令序列、跳转结构、
// 全局地址逐条相同（两版函数级 static __lookup/__init_lock/__inited 各自独立：
// ORIG 0x94fff80/0x94fff98/0x94fff9c vs 0x94ff8a8/0x94ff8b8/0x94ff8d4）。
bool InitMonsterScript(const char* dir, const char* list, std::map<int, int>* map,
                       const char* apcBonus)
{
    if (list != 0)                          // ORIG cmpl $0x0,0xc(%ebp)
    {
        strncpy(g_szMonsterScriptBaseDirectory, dir, 0x100);
        // ORIG 0x8a11342..0x8a113a0：本 TU 仅此两处**内联**分隔符判定
        // （strlen×2 短路 + strcat 内建展开 strlen+memcpy 2B），
        // 其余 *Script 段为真实 AppendDirSeparator 调用——勿用 helper。
        {
            char* b = g_szMonsterScriptBaseDirectory;
            if (b[strlen(b) - 1] != '\\' && b[strlen(b) - 1] != '/')
                strcat(b, "/");
        }

        if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_stMonsterScriptFileList),
                                    g_szMonsterScriptBaseDirectory, list, map))
            return false;

        // ORIG 0x8a113d9: 局部空向量 → resize(5, empty)（0x94ff278）。
        {
            std::vector<MonsterBaseParameter> empty;
            g_monsterBaseParameters.resize(5, empty);
        }

        for (int tableIdx = 0; tableIdx <= 4; ++tableIdx)
        {
            // ORIG 0x93f54c4[tableIdx] 文件名，经全局基目录打开。
            if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory,
                                    MONSTER_BASIS_PARAMETER_TABLE_NAMES[tableIdx]))
                return false;

            bool ok = false;
            for (int row = 0;; ++row)       // ORIG -0x74(%ebp)
            {
                MonsterBaseParameter param;             // ORIG -0x26c(%ebp)
                param.m_value[0] = ScanInt(&ok);        // 首列驱动循环
                if (!ok)                                // ORIG 0x8a114a8 jne
                    break;
                // ORIG 4.1 逐列展开（0x8a114b5..0x8a116a0，无 k 循环）
                param.m_value[1] = ScanInt((bool*)0);
                param.m_value[2] = ScanInt((bool*)0);
                param.m_value[3] = ScanInt((bool*)0);
                param.m_value[4] = ScanInt((bool*)0);
                param.m_value[5] = ScanInt((bool*)0);
                param.m_value[6] = ScanInt((bool*)0);
                param.m_value[7] = ScanInt((bool*)0);
                param.m_value[8] = ScanInt((bool*)0);
                param.m_value[9] = ScanInt((bool*)0);
                param.m_value[10] = ScanInt((bool*)0);
                param.m_value[11] = ScanInt((bool*)0);
                param.m_value[12] = ScanInt((bool*)0);
                param.m_value[13] = ScanInt((bool*)0);
                param.m_value[14] = ScanInt((bool*)0);
                param.m_value[15] = ScanInt((bool*)0);
                param.m_value[16] = ScanInt((bool*)0);
                param.m_value[17] = ScanInt((bool*)0);
                param.m_value[18] = ScanInt((bool*)0);
                param.m_value[19] = ScanInt((bool*)0);
                param.m_value[20] = ScanInt((bool*)0);
                param.m_value[21] = ScanInt((bool*)0);         // +0x54
                param.m_value[22] = ScanInt((bool*)0);         // +0x58
                param.m_value[23] = ScanInt((bool*)0);         // +0x5c

                // ORIG P157：operator[] 每用点独立取（size×2/push_back/resize/
                // 赋值共 5 次），勿建 vec 引用缓存。
                if (row == static_cast<int>(g_monsterBaseParameters[tableIdx].size()))
                    g_monsterBaseParameters[tableIdx].push_back(param);   // ORIG 0x8a11670
                else if (row > static_cast<int>(g_monsterBaseParameters[tableIdx].size()))
                {
                    // ORIG 0x8a116a1: 默认元素 resize(row+1) 后再赋值。
                    MonsterBaseParameter def;
                    g_monsterBaseParameters[tableIdx].resize(row + 1, def);
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

    bool ok = false;
    // 难度加成主表：13 行 × 5 列（缺项报错行号 0x7fc，末行/末列豁免）。
    for (int r = 0; r <= 12; ++r)
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
        if (!ok)
            break;
    }
    // 死塔加成：13 行 × 4 列（行号 0x818）。
    for (int r = 0; r <= 12; ++r)
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
        if (!ok)
            break;
    }
    // 血条系统加成：13 行 × 4 列写入 5 列数组前 4 列（行号 0x833）。
    for (int r = 0; r <= 12; ++r)
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
        if (!ok)
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

        int difficulty = 0;
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
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.clear();       // ORIG 0x8a11e46
            NAMED_MONSTER_HOLD_BALANCE_TABLE.clear();
            BOSS_MONSTER_HOLD_BALANCE_TABLE.clear();
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8); // 各 reserve 200
            NAMED_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);
            BOSS_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);

            std::string sub;
            while (ScanType(sub, true))     // ORIG 0x8a12046
            {
                if (sub == "[/mesmerization damage balance]")   // 0x8e1ed4c
                    break;
                if (sub == "[normal]")      // 0x8e1ed6c
                {
                    for (int i = 0; i <= 199; ++i)          // cmpl $0xc7,setle
                    {
                        DungeonHoldBalance h;
                        h.m_a = ScanInt((bool*)0);
                        h.m_b = ScanInt((bool*)0);
                        NORMAL_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[boss]")   // 0x8e1ed75
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        DungeonHoldBalance h;
                        h.m_a = ScanInt((bool*)0);
                        h.m_b = ScanInt((bool*)0);
                        BOSS_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[named]")  // 0x8e1ed7c
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        DungeonHoldBalance h;
                        h.m_a = ScanInt((bool*)0);
                        h.m_b = ScanInt((bool*)0);
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
            // 首次进入锁内插入四条 tag→id（id 为 ORIG 源内常量 0x932/0x935/
            // 0x949/0x95d），置 inited 后重派发同一 token 进入真实解析分支。
            static std::map<const char*, int, less_TCHAR> lookup;
            static SpinLock lock;
            static bool inited = false;

            std::string sub;
            for (;;)
            {
                if (!ScanType(sub, true))   // ORIG 0x8a12649
                    break;                  // 段结束 → 下一主标签

                if (!inited)
                {
                    lock.enter();                       // ORIG 0x8a122a2
                    if (!inited)
                    {
                        // ORIG 插入链 0x8a12320/0x8a12386/0x8a1246a/0x8a1254e
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit slowdown]", 0x932));
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal]", 0x935));
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss]", 0x949));
                        lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[named]", 0x95d));
                        lock.leave();                   // ORIG 0x8a1262a
                        inited = true;
                    }
                    else
                    {
                        lock.leave();
                    }
                }

                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    lookup.find(sub.c_str());           // ORIG 0x8a12218
                const int id = (it != lookup.end()) ? it->second : -1;

                if (id == 0 || id == 0x932)             // [/limit slowdown] 终止标记
                    continue;
                if (id == 0x935)                        // [normal]
                {
                    NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.clear();   // 0x9494f8c
                    for (int i = 0; i <= 199; ++i)
                    {
                        ScanInt((bool*)0);                     // 首列索引（弃）
                        stLimitSlowDown e;
                        e.m_a = ScanInt((bool*)0);
                        e.m_b = ScanInt((bool*)0);
                        NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                    }
                    continue;
                }
                if (id == 0x949)                        // [boss]
                {
                    BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.clear();     // 0x9494fa4
                    for (int i = 0; i <= 199; ++i)
                    {
                        ScanInt((bool*)0);
                        stLimitSlowDown e;
                        e.m_a = ScanInt((bool*)0);
                        e.m_b = ScanInt((bool*)0);
                        BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                    }
                    continue;
                }
                if (id == 0x95d)                        // [named]
                {
                    NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.clear();    // 0x9494f98
                    for (int i = 0; i <= 199; ++i)
                    {
                        ScanInt((bool*)0);
                        stLimitSlowDown e;
                        e.m_a = ScanInt((bool*)0);
                        e.m_b = ScanInt((bool*)0);
                        NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                    }
                    continue;
                }
                // id==-1（未知子标签）：ORIG 直落 0x8a1261c 继续。
            }
            continue;
        }
        else if (token == "[adjust hp gauge]")              // 0x8e1eda7
        {
            // 注：ORIG 0x8a126cc 在段首构造了一个 DungeonHoldBalance 局部对象，
            // 之后从未使用（纯编译产物），无可观行为，故不复刻。
            std::string sub;
            while (ScanType(sub, true))     // ORIG 0x8a1279a
            {
                if (sub == "[/adjust hp gauge]")                // 0x8e1edb9
                    break;
                if (sub == "[normal]")
                {
                    float v = ScanFloat((bool*)0);
                    NORMAL_MONSTER_GAUGE_ADJUST = v;            // 0x93f4d10
                }
                else if (sub == "[boss]")
                {
                    float v = ScanFloat((bool*)0);
                    BOSS_MONSTER_GAUGE_ADJUST = v;              // 0x93f4d18
                }
                else if (sub == "[named]")
                {
                    float v = ScanFloat((bool*)0);
                    NAMED_MONSTER_GAUGE_ADJUST = v;             // 0x93f4d14
                }
            }
            continue;
        }
        else
        {
            continue;                       // 未识别标签：ORIG 0x8a126b0 je
        }

        // ---- 难度组读取器（ORIG 0x8a11ba7）：每 consume 1 个弃置 float +
        //      最多 13 个 float 为一组；组不满或读失败则该难度段结束。
        for (;;)
        {
            ScanFloat(&ok);                 // 组头 float（fstp %st(0) 弃置）
            if (!ok)
                break;                      // → 下一主标签
            std::vector<float> values;      // ORIG -0x168 局部向量
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
    }
}
