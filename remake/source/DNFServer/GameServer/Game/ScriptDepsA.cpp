// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScriptDepsA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ScriptDepsA.cpp —— 脚本导入域 A 组（专属批次 ScriptDepsA）
//
// 依据 docs/class_func_reports/_lt_global_gt_/ 下对应报告的 ## 汇编 与
// ## 反编译 C 全文，以及无报告符号的 ORIG objdump 反汇编逐段转写：
//   importActiveStatusTable        T 0x08906f88
//   importDisjointScript           T 0x088e4d88
//   importCompoundAvatarTable      T 0x08903ea7 / Core T 0x089031d2
//   importAmplifyItem              T 0x0890a6b0
//   importCreatureCommonScript     T 0x08a383f6
//   ImportCharacterScript          T 0x08a0ce06
//   ImportEquipmentAniScript       T 0x089c05f8
// 及其直接叶子依赖（全部 ORIG nm 真实符号）：
//   getElement T 0x088bfe5b（唯一定义 CreatureScript.cpp）/ getItemRarity T 0x088bfd97 /
//   GetRoundInt W 0x08a3df64 / getCompoundAvatarPartsType T 0x0890308a /
//   setCompoundAvatarData T 0x08902b8e / setCompoundRareAvatarData T 0x08902e78 /
//   getCompoundRareAvatarType T 0x08902f42 / getObjectCategory T 0x08a3ca1b /
//   initObjectCategory T 0x08a0aed8 / initObjectScriptBase T 0x08a0aeb8 /
//   IgnoreCaseCompare::operator() W 0x08a3df2e /
//   getModuleTypeFromTag T 0x088c2233 / addCreatureModuleRate T 0x08a38a08 /
//   STCharacterScript::{C1 W 0x08368348, Clear W 0x08367a98} /
//   STRDCharacterStatus::Clear W 0x08367642 / CharacterEtcStats::clear W 0x08367752 /
//   ActiveActionFileSet C1 W 0x08367eee（隐式逐成员）/ _ComboInfo::clear W 0x0836785c /
//   STCharacterScript::STSecondGrowTypeData::clear W 0x08367a28 /
//   WeaponInfo {C1 W 0x08a3ce08, clear W 0x08a3cd8e, D1 W 0x083dc13a}
// 与全局表：
//   ACTIVESTATUS_MAXLEVEL D 0x0936e484 等 ACTIVESTATUS_* 域、
//   ACTIVESTATUS_TAG(static) d 0x093f5480[17]、g_weaponDurabilityDecreaseRate_
//   D 0x093f52a0[11][6]、upgradeWeaponAttackPowerRate_(static) d 0x093f5080[11][6][2]、
//   g_szCharacterScriptBaseDirectory B 0x094fe840[0x100]（唯一定义 InitScriptProducers_4.cpp）、
//   g_creatureModuleDamageRateMap B 0x094ff12c / g_creatureIndexDamageRateMap B 0x094ff144。
// 无独立报告的函数（GetRoundInt/WeaponInfo/子结构 clear/initObjectCategory/
// MODULE_NAME_TAG 初始化/s_objCategoryString 内容）均按 ORIG objdump 转写并在
// 注释标注地址；MODULE_NAME_TAG 的 17 组开/闭标签与 s_objCategoryString 的
// 44 条目内容取自 _Z41__static_initialization_and_destruction_0ii(0x088c3055)
// 与 initObjectCategory(0x08a0aed8) 的立即数字面量。
// ============================================================================
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "DNFLexWrapper.h"         // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/
                                   // ScanStr/ScanTypeOrInt/ScanTypeOrStr/getDirectoryPath
#include "LogManager.h"            // LogManager::logFormat
#include "STCharacterScript.h"     // STCharacterScript 完整布局 + 子结构（本批次补全）
#include "STRDCharacterStatusClient.h"  // STRDCharacterStatusClient（双向 operator=）
#include "DisJoint.h"              // DisjointScript 完整布局
#include "CCompoundAvatar.h"       // tagCompoundAvatar/tagCompoundAvatarTable
#include "CItemAmplifier.h"        // stAmplifyItemScript_t/ENUM_RARITY/ENUM_AMPLIFY_TYPE
#include "STEquipmentScript.h"     // ENUM_EQUIPMENTTYPE
#include "CEnvironment.h"          // less_TCHAR / SpinLock（ImportEquipmentAniScript
                                   // 函数级静态 __lookup 三件套，ORIG 0x089c07f8 区）
#include "STStackableScript.h"     // ENUM_MODULE_TYPE
#include "Quest.h"                 // ENUM_CHARACTERJOB

// ---- 本 TU 内提供或外部提供的叶子声明 ----
#include "ScriptDepsA/ScanTypeOrFloat.inc"
#include "ScriptDepsA/getCharacterJob.inc"
#include "ScriptDepsA/createLotteryInfo.inc"
extern char g_creatureScriptBaseDirectory[];   // B 0x094ff4e0（定义 CreatureScript.cpp）

// ============================ 全局表 ============================

extern char g_szCharacterScriptBaseDirectory[0x100];
    // ORIG B 0x094fe840 [0x100]：角色脚本基目录。唯一定义在 InitScriptProducers_4.cpp
    // （InitCharacterScript 回填，与 g_szStackableScriptBaseDirectory 同域）；
    // 本 TU 仅 ImportCharacterScript 读取。

// ---- ACTIVESTATUS 域（importActiveStatusTable 写入目标；初值取自 .data）----
int ACTIVESTATUS_MAXLEVEL = 150;                    // D 0x0936e484（初值实证）
int LIGHTNING_TIME_PER_NUMBER = 2000;               // D 0x0936e488
float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE = 1.0f;             // D 0x0936e48c
float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER = 1.0f; // D 0x0936e490
int ACTIVESTATUS_DURATION[18];                      // B 0x093f58a0 [0x48]
// [P291/P235 具名双视角] ORIG 4.1 对 `POWER[i*2]` 写侧发单条 scale-8
// `fstps ACTIVESTATUS_POWER1(,%ebx,8)`；4.4 对 float[36] 下标 i*2 恒折
// `add %eax,%eax` + scale-4（每段 +1 条 lea）。m_pair 以 8 字节元素下标
// 即折叠为单条 scale-8；奇槽继续走 m_flat[i*2+1] 保持 ORIG 的两段寻址形。
// 布局逐字节相同（0x90 字节，符号名不参与 mangling）；消费侧
// W9_SDCCommon.cpp 持同布局 TU-local 视图。
union SDCActiveStatusPowerView
{
    float m_flat[36];
    struct { float m_lo; float m_hi; } m_pair[18];
};
SDCActiveStatusPowerView ACTIVESTATUS_POWER1;       // B 0x093f5900 [0x90]
SDCActiveStatusPowerView ACTIVESTATUS_POWER2;       // B 0x093f59a0 [0x90]
float ACTIVESTATUS_ETC_PARAMETERS[18][8];           // B 0x093f5a40 [0x240]
float ACTIVESTATUS_LEVEL_RATE_FOR_PROB;             // B 0x093f5c80
float ACTIVESTATUS_LEVEL_RATE_FOR_POWER;            // B 0x093f5c84
float ACTIVESTATUS_LEVEL_ABSOLUTE_RATE;             // B 0x093f5c88

// ORIG static d 0x093f5480 [0x44]＝17 个 .rodata 字面量指针（逐一解析实证）。
static const char* const ACTIVESTATUS_TAG[17] =
{
    "[slow resistance]",             // 0x08e1ddd3
    "[freeze resistance]",           // 0x08e1dde5
    "[poison resistance]",           // 0x08e1ddf9
    "[stun resistance]",             // 0x08e1de0d
    "[curse resistance]",            // 0x08e1de1f
    "[blind resistance]",            // 0x08e1de32
    "[lightning resistance]",        // 0x08e1de45
    "[stone resistance]",            // 0x08e1de5c
    "[sleep resistance]",            // 0x08e1de6f
    "[burn resistance]",             // 0x08e1de82
    "[weapon break resistance]",     // 0x08e1de94
    "[bleeding resistance]",         // 0x08e1deae
    "[piercing resistance]",         // 0x08e1dec4
    "[stuck resistance]",            // 0x08e1deda
    "[confuse resistance]",          // 0x08e1deed
    "[hold resistance]",             // 0x08e1df02
    "[armor break resistance]",      // 0x08e1df14
};

// ORIG static d 0x093f5080 [0x210]＝11 职业 × 6 组 × {rate, value}（.data 初值实证）。
static float upgradeWeaponAttackPowerRate_[11][6][2] =
{
    { {0.95f,1.15f},{0.95f,1.05f},{1.10f,0.95f},{1.20f,0.90f},{1.00f,1.00f},{0.93f,0.90f} },
    { {0.95f,1.15f},{1.20f,0.90f},{1.00f,1.00f},{1.05f,0.95f},{1.00f,1.00f},{0.95f,1.00f} },
    { {0.87f,0.77f},{0.64f,0.94f},{1.00f,0.85f},{1.06f,0.64f},{0.76f,0.85f},{1.00f,1.00f} },
    { {1.20f,0.85f},{1.08f,0.90f},{0.90f,1.10f},{0.95f,1.20f},{1.00f,1.10f},{1.00f,1.00f} },
    { {1.00f,0.95f},{0.90f,1.15f},{1.05f,0.90f},{1.05f,1.05f},{1.20f,0.85f},{1.00f,1.00f} },
    { {0.87f,0.77f},{0.64f,0.94f},{1.00f,0.85f},{1.06f,0.64f},{0.76f,0.85f},{1.00f,1.00f} },
    { {0.95f,1.15f},{1.20f,0.90f},{1.00f,1.00f},{1.05f,0.95f},{1.00f,1.00f},{0.95f,1.00f} },
    { {1.20f,0.85f},{1.08f,0.90f},{0.90f,1.10f},{0.95f,1.20f},{1.00f,1.10f},{1.00f,1.00f} },
    { {0.95f,1.15f},{0.95f,1.05f},{1.10f,0.95f},{1.20f,0.90f},{1.00f,1.00f},{0.93f,0.90f} },
    { {1.20f,0.85f},{1.08f,0.90f},{0.90f,1.10f},{0.95f,1.20f},{1.00f,1.10f},{1.00f,1.00f} },
    { {0.00f,0.00f},{0.00f,0.00f},{0.00f,0.00f},{0.00f,0.00f},{0.00f,0.00f},{0.00f,0.00f} },
};

// ORIG D 0x093f52a0 [0x108]＝11 职业 × 6（.data 初值实证：仅 job0 首项 1.0f）。
float g_weaponDurabilityDecreaseRate_[11][6] =
{
    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
};

// ORIG B 0x094ff12c / 0x094ff144 [0x18]（生物模块伤害率表）。
std::map<ENUM_MODULE_TYPE, float> g_creatureModuleDamageRateMap;
std::map<int, float> g_creatureIndexDamageRateMap;

// ============================ 小工具 ============================

// ORIG GetRoundInt W 0x08a3df64：v>=0 ? (int)(v+0.5f) : (int)(v-0.5f)。
#include "ScriptDepsA/GetRoundInt.inc"
}

#include "ScriptDepsA/getElement.inc"
#include "ScriptDepsA/getItemRarity.inc"
}

// ============================ importActiveStatusTable ============================

// ORIG importActiveStatusTable T 0x08906f88（报告 ## 汇编 + ## 反编译 C 全文转写）：
// loadRDARScriptFile("", path) 失败 → logFormat 后 ret0；ScanType 主循环按段名分派；
// level-rate 三段为 ScanInt 结果乘 0.01f（_DAT_08e122ec=0.01f 实证）后存 float。
#include "ScriptDepsA/importActiveStatusTable.inc"
}

// ============================ importDisjointScript ============================

// ORIG importDisjointScript T 0x088e4d88（报告全文转写）。依赖已由既有 TU 提供：
// DisjointScript::clear(CtorProducers_2.cpp)/getCharacterJob(SyncScriptDeps.cpp)/
// createLotteryInfo(CStackableItem.cpp)/DisjointSkillScript 生命周期(CtorProducers_2.cpp)。
#include "ScriptDepsA/importDisjointScript.inc"
}

// ============================ 头像合成域 ============================

// ORIG getCompoundAvatarPartsType T 0x0890308a（报告全文转写）：未命中 0x16。
#include "ScriptDepsA/getCompoundAvatarPartsType.inc"
}

// ORIG getCompoundRareAvatarType T 0x08902f42（报告全文转写）：未命中 0x16。
#include "ScriptDepsA/getCompoundRareAvatarType.inc"
}

// ORIG setCompoundAvatarData T 0x08902b8e（报告全文转写）：三组选项
// （计数式 optsA→m_b 累计权、计数式 optsC→m_d 累计权、开列式 optsB→m_c 累计权），
// pair={itemIdx, 截至当前的累计权重}。任一 ScanInt 失败 → ret0。
#include "ScriptDepsA/setCompoundAvatarData.inc"
}

// ORIG setCompoundRareAvatarData T 0x08902e78（报告全文转写）：开列式稀有选项，
// 权重累计入 m_cube.m_field0c[e]，pair 入 m_cube.m_vecs[e]。type==0x16 → ret0。
// [b28d-48] 已闭合（NDIFF=0 全等）：m_field0c 由 int[10] 改 unsigned int[10]
//（CCompoundAvatar.h；双证据：ORIG 0x08902e78 加法段 `mov 0x18(%eax,%ecx,4),%ecx;
// mov -0xc(%ebp),%eax; add %eax,%ecx` 仅字段 unsigned+RHS int 时 4.4 可复现（P274），
// 且 ORIG push_back 的 pair converting ctor 符号 `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`
// 第二参即 unsigned int&）。sizeof/偏移/mangled 不变。
#include "ScriptDepsA/setCompoundRareAvatarData.inc"
}

// ORIG importCompoundAvatarTableCore T 0x089031d2（报告 ## 汇编 + ## 反编译 C
// 全文转写）。table 实为 11×tagCompoundAvatarTable 数组首址（job*0xc 定位）。
#include "ScriptDepsA/importCompoundAvatarTableCore.inc"

}

// ORIG importCompoundAvatarTable T 0x08903ea7（报告全文转写）：
// loadRDARScriptListFile 取 11 个子文件名（≠11 → 0x106e），逐个 Core 导入
// （失败 → 0x1075），全部成功 ret0。
// ============================ 头像合成域（声明修正） ============================

// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii... 0x088bbd77 T
//（定义 STScriptFileList.cpp；第 4 参 ORIG 未使用）。
// [b28d-solo-ScriptDepsA] P292：ORIG 0x08903edb 调用点 `test %al,%al; je`（无
// sete/setne）⇒ 该函数返回 bool；已上报 Main 同步 STScriptFileList.cpp 定义。
#include "ScriptDepsA/loadRDARScriptListFile.inc"
#include "ScriptDepsA/importCompoundAvatarTableCore_1.inc"

#include "ScriptDepsA/importCompoundAvatarTable.inc"
}


// ============================ 生物公共脚本域 ============================

// ORIG MODULE_NAME_TAG（B 0x0949b2c0 [0x88]）初始化实证（__static_initialization_
// and_destruction_0ii 0x088c3055 内循环）：17 组全部为 {"[dungeon]","[/dungeon]"}。
namespace {

struct ModuleTagEntry
{
    std::string open;    // +0x00（getModuleTypeFromTag 比对目标）
    std::string close;   // +0x04
};

struct ModuleTagTableInit
{
    ModuleTagEntry entries[17];
    ModuleTagTableInit()
    {
        for (int i = 0; i < 17; ++i)
        {
            entries[i].open = "[dungeon]";
            entries[i].close = "[/dungeon]";
        }
    }
};

const ModuleTagTableInit s_moduleNameTag;

}  // namespace

// ORIG getModuleTypeFromTag T 0x088c2233（报告全文转写）：步长 8 遍历 17 组，
// 与 open 标签比较；未命中 -1。
#include "ScriptDepsA/getModuleTypeFromTag.inc"
}

// ORIG addCreatureModuleRate T 0x08a38a08（报告全文转写）：已有时乘旧值后覆盖写入。
#include "ScriptDepsA/addCreatureModuleRate.inc"
}

// CreatureCommonScript 本地布局视图（与 InitEnvHelpers.cpp 本地定义同布局；
// clear() 仅声明——唯一强定义在 InitEnvHelpers.cpp，避免重复符号）。
class CreatureCommonScript
{
public:
    void clear();                        // ORIG W 0x08096a2c（定义 InitEnvHelpers.cpp）

    float m_attackConstant;              // +0x00（[attack constant]）
    int m_responseDelay;                 // +0x04
    int m_evolutionFadeInDelay;          // +0x08
    int m_evolutionFadeOutDelay;         // +0x0c
    int m_evolutionDelay;                // +0x10
    int m_dyingDelay;                    // +0x14
    std::vector<int> m_damage;           // +0x18
    std::vector<int> m_mana;             // +0x24
    int m_secondPerFillPointSpend;       // +0x30
    int m_secondPerFillPointCharge;      // +0x34
    int m_maxFillPoint;                  // +0x38
    int m_fillPointToEatFeed;            // +0x3c
    int m_ambientRoomList[2];            // +0x40
    int m_ambientMainGame[2];            // +0x48
    int m_ambientPvp[2];                 // +0x50
};

// ORIG importCreatureCommonScript T 0x08a383f6（报告 ## 汇编 + ## 反编译 C 全文
// 转写）。伤害表同步进 damageTable；module/assault 段除数为 _DAT_08e23930=100.0f。
#include "ScriptDepsA/importCreatureCommonScript.inc"
}

// ============================ 物件类别域 ============================

// ---- ENUM_CATEGORY / IgnoreCaseCompare / s_objCategoryString ----
// 取值与内容自 initObjectCategory(0x08a0aed8) 立即数全量转写（44 条，默认 0x2c）。

// ORIG IgnoreCaseCompare::operator() W 0x08a3df2e：strcasecmp(a,b)>0。
struct IgnoreCaseCompare
{
    bool operator()(const std::string& a, const std::string& b)
    {
        return strcasecmp(a.c_str(), b.c_str()) > 0;
    }
};

// ORIG static b 0x094ff630 [0x18]。
static std::map<std::string, ENUM_CATEGORY, IgnoreCaseCompare> s_objCategoryString;

// ORIG initObjectCategory T 0x08a0aed8：map[literal]=枚举 逐条插入，恒返 1。
// 字面量地址逐一解析自 .rodata（0x08e1dc01..0x08e1ddc6）。
#include "ScriptDepsA/initObjectCategory.inc"
}

// ORIG initObjectScriptBase T 0x08a0aeb8：initObjectCategory 的布尔透传包装。
#include "ScriptDepsA/initObjectScriptBase.inc"
}

// ORIG getObjectCategory T 0x08a3ca1b（报告全文转写）：按值收参；查不到 0x2c。
// b28c-l23 根因（P96/P50 + P220 极性 + P132 operator*）：ORIG 只发射 `map::find` /
// `map::end` / `_Rb_tree_iterator...neEERKS4_` / `...deEv`（**operator\*，非 operator->**）
// 各一次（30 条），find 结果直接构造进 -0x10（无拷贝 ctor）⇒ ①原码用**非 const
// iterator**；②判据是正极 `if (it != end()) return (*it).second;` 让查表走
// fall-through（负极 `==` 形 4.4 恒产 eq + 块序反）；③取值写 `(*it).second`
// 而非 `it->second`（后者发 ptEv）。
#include "ScriptDepsA/getObjectCategory.inc"
}

// ============================ 强化脚本域 ============================

// ORIG stInvestAmplifyOption_t C1 W 0x0822aea6（定义 CItemAmplifier.cpp）——仅声明引用。

// ORIG importAmplifyItem T 0x0890a6b0（报告 ## 汇编 + ## 反编译 C 全文转写）：
// 返回错误行号，0=成功。[option mapping table] 建 map<string,ENUM_AMPLIFY_TYPE>；
// [option data] 查表得类型，{type,(short)(sum),(short)value} 入 m_options；
// 三个 hash_map 投资段 key=ScanInt、value={查表类型, 第二个 ScanInt}。
#include "ScriptDepsA/importAmplifyItem.inc"

}

// ============================ 武器信息 / 角色脚本域 ============================

// ORIG WeaponInfo::clear W 0x08a3cd8e（objdump 转写，无独立报告）。
#include "ScriptDepsA/clear.inc"
}

// ORIG WeaponInfo C1 W 0x08a3ce08：vector 构造后 clear()。D1 W 0x083dc13a 平凡
//（成员逆序析构由编译器生成）；copy 为逐成员语义（ORIG C1ERKS_ W 0x08a3dfca）。
#include "ScriptDepsA/WeaponInfo.inc"
#include "ScriptDepsA/Clear.inc"
}

// ORIG C1 W 0x0836778c：vector C1 + clear()。
#include "ScriptDepsA/CharacterEtcStats.inc"
}

// ORIG CharacterEtcStats::clear W 0x08367752（objdump 转写）：vector clear +
// +0xc..+0x18 四个 int 清零；+0x4..+0xb 未触。
#include "ScriptDepsA/clear_1.inc"
}

// ORIG T 0x08a0c942：Client（0x98 展开块）→ 本 packed 块截断回填。
STRDCharacterStatus& STRDCharacterStatus::operator=(
    const STRDCharacterStatusClient& c)
{
    m_hpMax = c.m_hpMax;
    m_mpMax = c.m_mpMax;
    m_physAttack = (unsigned short)c.m_physAttack;
    m_physDefense = (unsigned short)c.m_physDefense;
    m_magAttack = (unsigned short)c.m_magAttack;
    m_magDefense = (unsigned short)c.m_magDefense;
    for (int i = 0; i < 4; ++i)
        m_elemResist[i] = (unsigned short)c.m_elemResist[i];
    for (int i = 0; i < 17; ++i)
        m_activeResist[i] = (unsigned short)c.m_activeResist[i];
    m_inventoryLimit = c.m_inventoryLimit;
    m_hpRegen = (unsigned short)c.m_hpRegen;
    m_mpRegen = (unsigned short)c.m_mpRegen;
    m_moveSpeed = c.m_moveSpeed;
    m_attackSpeed = (unsigned short)c.m_attackSpeed;
    m_castSpeed = (unsigned short)c.m_castSpeed;
    m_hitRecovery = (unsigned short)c.m_hitRecovery;
    m_jumpPower = (unsigned short)c.m_jumpPower;
    m_weight = c.m_weight;
    m_field52 = (unsigned char)c.m_field52;
    m_field53 = c.m_field53;
    return *this;
}

// ORIG T 0x08a0cab8：packed 块 → Client 展开（符号/零扩展按 ORIG movswl/movzwl）。
STRDCharacterStatusClient& STRDCharacterStatusClient::operator=(
    const STRDCharacterStatus& s)
{
    m_hpMax = s.m_hpMax;
    m_mpMax = s.m_mpMax;
    m_physAttack = s.m_physAttack;
    m_physDefense = s.m_physDefense;
    m_magAttack = s.m_magAttack;
    m_magDefense = s.m_magDefense;
    for (int i = 0; i < 4; ++i)
        m_elemResist[i] = (short)s.m_elemResist[i];   // ORIG movzwl+movswl 符号扩展
    for (int i = 0; i < 17; ++i)
        m_activeResist[i] = (short)s.m_activeResist[i];
    m_inventoryLimit = s.m_inventoryLimit;
    m_hpRegen = (short)s.m_hpRegen;
    m_mpRegen = (short)s.m_mpRegen;
    m_moveSpeed = s.m_moveSpeed;
    m_attackSpeed = s.m_attackSpeed;
    m_castSpeed = s.m_castSpeed;
    m_hitRecovery = (short)s.m_hitRecovery;
    m_jumpPower = (short)s.m_jumpPower;
    m_weight = s.m_weight;
    m_field52 = s.m_field52;
    m_field53 = s.m_field53;
    return *this;
}

// ORIG _ComboInfo C1（W）：逐成员构造后调 clear()。
#include "ScriptDepsA/_ComboInfo.inc"
}

// ORIG _ComboInfo::clear W 0x0836785c（objdump 转写）。
#include "ScriptDepsA/clear_2.inc"
}

// ORIG STSecondGrowTypeData::clear W 0x08367a28（objdump 转写）：双状态块 +
// 双技能表 + 双 etc 域。
#include "ScriptDepsA/clear_3.inc"
}

// ORIG STCharacterScript::Clear W 0x08367a98（objdump 0x3a5 字节全文转写）。
#include "ScriptDepsA/Clear_1.inc"
}

// ============================ ImportCharacterScript ============================

// ORIG static d 0x094ff618 [0x18]：job → 身体贴图路径（原文件级静态；本 TU
// 以文件级静态等价建模）。ORIG 实例化实证 map<ENUM_CHARACTERJOB,Ss>（W
// _ZNSt3mapI17ENUM_CHARACTERJOBSs...4find/ix @08a4101c/08a3ebec）。
// [body image path] 块 ORIG `mov 0x8(%ebp),%eax; mov %eax,0x4(%esp)` =
// &m_job（字段偏移 0，地址即 script 值）直绑 const ENUM& 形参，**无临时槽**；
// m_job 已按 ORIG 证据具型化为 ENUM_CHARACTERJOB（见下 [bigfix-enum]），
// 键直接取 script->m_job，无 reinterpret 视图。
static std::map<ENUM_CHARACTERJOB, std::string> s_bodyImagePath;

// ORIG ImportCharacterScript T 0x08a0ce06（报告 ## 汇编 + ## 反编译 C 全文转写）。
// 返回：1=解析至 EOF；0=任一段失败。
// [bigfix 大件语义修复波] 归一化 2818/2771（gap +47）→ **2818/2820（gap −2）**：
//  ① 循环头 `while (ScanType(...))` → `for (;;) { if (!ScanType(...)) break; }`：
//     复现 ORIG 0x08a0cf6e..0x08a0cf78 `call ScanType; xor $0x1,%eax; test %al,%al;
//     jne` 的取反物化（P164；while 形 4.4 恒 `test;je` 少 1 条），并令 4.4 为
//     39 个 else-if 成功出口各发一条回跳 jmp —— 一次消掉 ORIG 函数头
//     0x08a0cee8..0x08a0cf5c 的 39 个 N121 delete 岛（−39）。
//  ② [upgrade weapon attack power rate] 的 col 由 unsigned 改 int + 显式
//     `(unsigned int)col <= 5`：复现 ORIG 0x08a0f946 `mov -0x18(%ebp),%eax;
//     cmp $0x5,%eax; ja` 两段寄存器比较（P310；unsigned 局部 4.4 恒折
//     `cmpl $5,-0x18(%ebp)` 单条）。
//  ③ 符号性方向修正（T2 直证）：STRDCharacterStatus::m_hpMax/m_mpMax/m_moveSpeed
//     int→unsigned int，STRDCharacterStatusClient 侧同名三字段 unsigned→int。
//     ORIG exportCharacterStatusScript 0x08a0fb41/0x08a0fb89/0x08a0feaa 对
//     Status 侧 +0x0/+0x4/+0x42 发 `mov $0x0,%edx; … fildll`（零扩展 ⇒ unsigned），
//     而 +0x3a/+0x4e 发 `fildl`（int）；双向 op= 0x08a0c942/0x08a0cab8 两侧均
//     `mov %eax,%edx` 中转 ⇒ 两侧异型。修后 [HP MAX]/[MP MAX]/[move speed]
//     0x08a0d5b8/0x08a0d613/0x08a0da6b 的 RHS-first + edx 中转 store 翻正
//     （P211/P248 档③），inventory limit/weight 维持 LHS-first。同宽 4B，
//     sizeof(STRDCharacterStatus)=0x57 / Client 0x98 / 全偏移 / mangled 不变。
//  ④ m_job int→ENUM_CHARACTERJOB（enum 内联走既有 guard DNF_ENUM_CHARACTERJOB_
//     DEFINED，与 JobKey.h 幂等）：复现 ORIG 0x08a0f951/0x08a0f970/0x08a0fa1f 读
//     m_job 的 `mov (%eax),%eax; mov %eax,%edx`（P254 enum 中转；int 字段 4.4
//     恒折单段）。
// 残余（gap −2，我方多 2 条）：
//  ① [job] 0x08a0cfcd store：ORIG LHS-first（`mov 0x8(%ebp),%edx; mov %eax,(%edx)`）
//     ⇒ getCharacterJob 返回类型须为 ENUM_CHARACTERJOB 才能同型直存；现权威声明
//     NpcScript.h:74 为 int，本处 static_cast 触发 4.4 转换中转（多 1 条）。
//     全链 enum 化（定义 SyncScriptDeps.cpp + 各 TU 镜像声明）已上报 Main。
//  ② [weapon wav] 0x08a0f169 块序差（ORIG `jne` 落 push_back 块；负向早返形
//     实测 INSERT +7 反劣，回退）⇒ 1 条，N 封顶。
//  ③ difflib 错位伪影 6 条（我方等价指令在别处）；p167 332/332 仅 C1↔C2/D1↔D2
//     同址别名差（P93 豁免）⇒ 语义缺口 0。
#include "ScriptDepsA/ImportCharacterScript.inc"
}

// ============================ ImportEquipmentAniScript ============================

// 标签分派表类型：ORIG 函数级静态 __lookup = map<char const*, int, less_TCHAR>
// （_ZZ24ImportEquipmentAniScript...E8__lookup B 0x094fddd8）。
typedef std::map<const char*, int, less_TCHAR> AniLookupMap;

// ORIG ImportEquipmentAniScript T 0x089c05f8（报告 ## 汇编 + ## 反编译 C 双段
// 全文转写）。主文件列子文件名 → 逐个 loadRDARScriptFile 解析 →
// insert(make_pair(文件名 c_str(), 条目))（0x089c1acc 段：c_str →
// make_pair<char const*,STEquipmentAniScript&> → pair 转换 → map::insert）。
// 标签分派 = 函数级静态三件套（ORIG：__lookup map<char const*,int,less_TCHAR>
// guard 构造 0x089c07f8 / __init_lock SpinLock 0x089c0872 / __inited
// 0x094fde04）：未建表时持锁以 id=0 进入 switch，case 0 与 case 0x3142 合并
// （0x089c0a21 `test %eax,%eax; je 0x089c0b6a`），沿 case 排布逐例
// fall-through 注册 (tag,id) 共 26 例（0x089c0b6a LAYER → 0x089c0bde waiting
// → 0x089c0c69 move → 0x089c0cf4 sit → 0x089c0d7f dmg1 → 0x089c0e0a dmg2 →
// 0x089c0e95 down → 0x089c0f20 overturn → 0x089c0fab jump → 0x089c1036
// jumpattack → 0x089c10c1 rest → 0x089c114c throw1-1 → 0x089c11d7 throw1-2 →
// 0x089c1262 throw2-1 → 0x089c12ed throw2-2 → 0x089c1378 throw3-1 →
// 0x089c1403 throw3-2 → 0x089c148e throw4-1 → 0x089c1519 throw4-2 →
// 0x089c15a4 dash → 0x089c162f dashattack → 0x089c16ba getitem →
// 0x089c1745 buff → 0x089c17d0 back → 0x089c185b attack → 0x089c1980 etc），
// 链尾公共块 0x089c1aa0 释放锁、置 __inited 并 jmp 0x089c07f8 用当前 token
// 重查派发。字段映射（parse 分支 `lea -0x3f4; add $off` 实证）：
// throw n-1 → +0x2c+4*(n-1)、throw n-2 → +0x3c+4*(n-1)；attack/etc motion
// 为多行 vector（0x089c18bf/0x089c19e4 循环 string(val.c_str()) 临时 +
// push_back）。ORIG 尾 `mov %ebx,%eax` 返回 0/1（P159 判据 → int）。
// 残余 DIFF 归因（N 类豁免，语义全量恢复后 1223/1279）：
//  ① 主循环与内层 while 的 4.1 “xor$1;test;jne exit-island(nop;jmp)” 镜像
//     布局（0x089c0687..0693 等）= 负例“空 else nop;jmp 岛 vs
//     jump-threading”；② 子文件收集/attack/etc 循环的 EH landing pad 与
//     4.4 合并差异（0x089c0721/1b52 区，负例“new-expression landing-pad /
//     EH 双份 dtor epilogue vs merge”）；③ 4.1 vector::begin() 以 sret 返回
//     迭代器（0x089c0745 -0x3fc/-0x3f8 双槽）而 4.4 寄存器直返，帧 0x41c vs
//     0x40c、上方槽整体 -4 平移（P31/P112 结构性伪影）；④ 其余为 .o 重定位
//     地址显示差（链接 AE 口径消除）。
#include "ScriptDepsA/ImportEquipmentAniScript.inc"
}


// ============================================================================
// OnlyOrigBatch11 追加：getObjectCategoryString / getBodyImagePath /
// getUpgradeWeaponAttackPowerRate（ORIG 同属脚本域；依赖本 TU static 表）
// ============================================================================

// ORIG T 0x08a3ca81：遍历 s_objCategoryString（IgnoreCaseCompare map）找
// value==cat 的首项返回 key；未命中返回空串（ORIG 0x8e1dc00）。
std::string getObjectCategoryString(ENUM_CATEGORY cat)
{
    std::map<std::string, ENUM_CATEGORY, IgnoreCaseCompare>::iterator it =
        s_objCategoryString.begin();
    for (; it != s_objCategoryString.end(); ++it)
    {
        if ((*it).second == cat)   // ORIG 用 operator*（de）
            return (*it).first;
    }
    return std::string("");
}

// ORIG T 0x08a397f8：s_bodyImagePath.find(job)；命中返回 second.c_str()，
// 未命中返回 0。
const char* getBodyImagePath(ENUM_CHARACTERJOB job)
{
    std::map<ENUM_CHARACTERJOB, std::string>::iterator it =
        s_bodyImagePath.find(job);
    if (it != s_bodyImagePath.end())
        return it->second.c_str();
    return 0;
}

// ORIG T 0x08a39866：索引 = 12*job + 2*type + level（[job][type][level]）；
// type>5 或 level>1 返回 0；否则返回表项 float。
#include "ScriptDepsA/getUpgradeWeaponAttackPowerRate.inc"
}
