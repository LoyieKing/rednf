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
float ScanTypeOrFloat(bool& isType, std::string& s, bool& isFloat, bool consume);
    // _Z15ScanTypeOrFloatRbRSsS_b T 0x088c2e01（定义 DNFLexWrapperHelpers.cpp）
int getCharacterJob(const char* name);
    // _Z15getCharacterJobPKc T 0x088be57e（定义 SyncScriptDeps.cpp）
void createLotteryInfo(LotteryInfo* info, std::vector<int>* counts);
    // _Z17createLotteryInfoP11LotteryInfoPSt6vectorIiSaIiEE T 0x089bab64（定义 CStackableItem.cpp）
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
float ACTIVESTATUS_POWER1[36];                      // B 0x093f5900 [0x90]
float ACTIVESTATUS_POWER2[36];                      // B 0x093f59a0 [0x90]
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
int GetRoundInt(float v)
{
    if (v >= 0.0f) return static_cast<int>(v + 0.5f);
    return static_cast<int>(v - 0.5f);
}

int getElement(const char* tag);
    // _Z10getElementPKc T 0x088bfe5b：strcmp 阶梯，未命中 4。唯一定义在
    // CreatureScript.cpp（声明见 CreatureScript.h），本 TU 仅调用。

// ORIG getItemRarity T 0x088bfd97（报告全文转写）：未命中 6（RARITY_MAX）。
int getItemRarity(const char* name)
{
    if (std::strcmp(name, "common") == 0) return 0;
    if (std::strcmp(name, "uncommon") == 0) return 1;
    if (std::strcmp(name, "rare") == 0) return 2;
    if (std::strcmp(name, "unique") == 0) return 3;
    if (std::strcmp(name, "epic") == 0) return 4;
    if (std::strcmp(name, "chronicle") == 0) return 5;
    return 6;
}

// ============================ importActiveStatusTable ============================

// ORIG importActiveStatusTable T 0x08906f88（报告 ## 汇编 + ## 反编译 C 全文转写）：
// loadRDARScriptFile("", path) 失败 → logFormat 后 ret0；ScanType 主循环按段名分派；
// level-rate 三段为 ScanInt 结果乘 0.01f（_DAT_08e122ec=0.01f 实证）后存 float。
bool importActiveStatusTable(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0x162b,
            "ERROR : %s \306\304\300\317\300\273\040\303\243\300\273\274\366\040"
            "\276\370\275\300\264\317\264\331.", path);   // EUC-KR 字面量（ORIG 0x08e103d8）
        return false;
    }

    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[active status max level]")
        {
            ACTIVESTATUS_MAXLEVEL = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[active status duration]")
        {
            for (int i = 0; i < 18; ++i)
                ACTIVESTATUS_DURATION[i] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[active status power 1]")
        {
            for (int i = 0; i < 18; ++i)
            {
                ACTIVESTATUS_POWER1[i * 2] = ScanFloat(static_cast<bool*>(0));
                ACTIVESTATUS_POWER1[i * 2 + 1] = ScanFloat(static_cast<bool*>(0));
            }
        }
        else if (token == "[active status power 2]")
        {
            for (int i = 0; i < 18; ++i)
            {
                ACTIVESTATUS_POWER2[i * 2] = ScanFloat(static_cast<bool*>(0));
                ACTIVESTATUS_POWER2[i * 2 + 1] = ScanFloat(static_cast<bool*>(0));
            }
        }
        else if (token == "[active status etc parameters]")
        {
            for (int i = 0; i < 18; ++i)
                for (int j = 0; j < 8; ++j)
                    ACTIVESTATUS_ETC_PARAMETERS[i][j] = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[active status level rate for prob]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            ACTIVESTATUS_LEVEL_RATE_FOR_PROB = 0.01f * static_cast<float>(v);
        }
        else if (token == "[active status level rate for power]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            ACTIVESTATUS_LEVEL_RATE_FOR_POWER = 0.01f * static_cast<float>(v);
        }
        else if (token == "[active status level absolute rate]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            ACTIVESTATUS_LEVEL_ABSOLUTE_RATE = 0.01f * static_cast<float>(v);
        }
        else if (token == "[lightning time per number]")
        {
            LIGHTNING_TIME_PER_NUMBER = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[active status user to user damage rate]")
        {
            ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[dead tower active status user to user damage rate]")
        {
            ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER =
                ScanFloat(static_cast<bool*>(0));
        }
    }
    return true;
}

// ============================ importDisjointScript ============================

// ORIG importDisjointScript T 0x088e4d88（报告全文转写）。依赖已由既有 TU 提供：
// DisjointScript::clear(CtorProducers_2.cpp)/getCharacterJob(SyncScriptDeps.cpp)/
// createLotteryInfo(CStackableItem.cpp)/DisjointSkillScript 生命周期(CtorProducers_2.cpp)。
bool importDisjointScript(DisjointScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script->clear();

    std::string token;
    std::string str;
    int value = 0;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[cube index]")
        {
            ScanStr(&str);
            const int elem = getElement(str.c_str());
            std::vector<int>& vec = script->m_cubeIndexVecs[elem];
            vec.clear();
            while ((value = ScanInt(&ok), ok))
                vec.push_back(value);
        }
        else if (token == "[legacy info]")
        {
            const int idx = ScanInt(static_cast<bool*>(0));
            std::vector<int> counts;
            while ((value = ScanInt(&ok), ok))
                counts.push_back(value);
            if (idx >= 1 && idx <= 4)
                createLotteryInfo(&script->m_legacyInfo[idx - 1], &counts);
        }
        else if (token == "[skill info]")
        {
            script->m_skills.clear();
            script->m_skillSelect = ScanInt(static_cast<bool*>(0));
            DisjointSkillScript entry;
            while (ScanStr(&str))
            {
                entry.m_field00 = getCharacterJob(str.c_str());
                entry.m_field04 = ScanInt(static_cast<bool*>(0));
                entry.m_field08 = ScanInt(static_cast<bool*>(0));
                ScanStr(&str);
                entry.m_field0c = getElement(str.c_str());
                script->m_skills.push_back(entry);
            }
        }
        else if (token == "[skill prob]")
        {
            script->m_fieldCC = ScanFloat(static_cast<bool*>(0));
            script->m_skillProbD0 = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[cube creation const]")
        {
            script->m_creationConst = ScanInt(static_cast<bool*>(0));
            for (int i = 0; i < 6; ++i)
                script->m_creationRate[i] = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[additional result]")
        {
            for (int g = 0; g < 6; ++g)
            {
                // m_additional 为 DisjointScript 匿名嵌套结构（P146 保持 ._381 mangled），
                // 不可具名引用，直接成员访问。
                script->m_additional[g].m_count = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_additional[g].m_items.clear();
                script->m_additional[g].m_items.reserve(
                    script->m_additional[g].m_count);
                for (int j = 0; j < script->m_additional[g].m_count; ++j)
                {
                    const int item = ScanInt(static_cast<bool*>(0));
                    script->m_additional[g].m_items.push_back(item);
                }
            }
        }
        else if (token == "[additional result const]")
        {
            for (int g = 0; g < 6; ++g)
            {
                script->m_additionalWeight[g] = ScanFloat(static_cast<bool*>(0));
                if (script->m_additionalWeight[g] != 0.0f)
                {
                    script->m_additionalRate[g] = ScanFloat(static_cast<bool*>(0));
                    script->m_additionalConst[g].m_item = ScanInt(static_cast<bool*>(0));
                    script->m_additionalConst[g].m_count = ScanInt(static_cast<bool*>(0));
                }
                else
                {
                    script->m_additionalRate[g] = 0.0f;
                    script->m_additionalConst[g].m_item = 0;
                    script->m_additionalConst[g].m_count = 0;
                }
            }
        }
        else if (token == "[avatar disjoint info]")
        {
            while (ScanType(token, true) && token != "[/avatar disjoint info]")
            {
                const int job = getCharacterJob(token.c_str());
                if (job == 0xb)
                    return false;
                for (int grade = 1; grade <= 3; ++grade)
                {
                    for (int slot = 0; slot < 5; ++slot)
                    {
                        const int v = ScanInt(&ok);
                        script->m_avatarDisjoint[job][grade - 1][slot] = v;
                        if (!ok)
                            return false;
                    }
                }
                ScanInt(&ok);
            }
        }
        else if (token == "[avatar disjoint for parts info]")
        {
            for (int grade = 1; grade <= 3; ++grade)
            {
                for (int parts = 0; parts < 10; ++parts)
                {
                    const int v = ScanInt(&ok);
                    script->m_avatarPartsDisjoint[grade - 1][parts] = v;
                    if (!ok)
                        return false;
                }
            }
            ScanInt(&ok);
        }
    }
    return true;
}

// ============================ 头像合成域 ============================

// ORIG getCompoundAvatarPartsType T 0x0890308a（报告全文转写）：未命中 0x16。
int getCompoundAvatarPartsType(const std::string& name)
{
    if (name == "hat") return 0;
    if (name == "hair") return 1;
    if (name == "face") return 2;
    if (name == "neck") return 6;
    if (name == "coat") return 3;
    if (name == "pants") return 4;
    if (name == "belt") return 7;
    if (name == "shoes") return 5;
    if (name == "skin") return 8;
    if (name == "aurora") return 9;
    return 0x16;
}

// ORIG getCompoundRareAvatarType T 0x08902f42（报告全文转写）：未命中 0x16。
int getCompoundRareAvatarType(const std::string& token)
{
    if (token == "[hat rare avatar]") return 0;
    if (token == "[hair rare avatar]") return 1;
    if (token == "[face rare avatar]") return 2;
    if (token == "[neck rare avatar]") return 6;
    if (token == "[coat rare avatar]") return 3;
    if (token == "[pants rare avatar]") return 4;
    if (token == "[belt rare avatar]") return 7;
    if (token == "[shoes rare avatar]") return 5;
    if (token == "[skin rare avatar]") return 8;
    if (token == "[aurora rare avatar]") return 9;
    return 0x16;
}

// ORIG setCompoundAvatarData T 0x08902b8e（报告全文转写）：三组选项
// （计数式 optsA→m_b 累计权、计数式 optsC→m_d 累计权、开列式 optsB→m_c 累计权），
// pair={itemIdx, 截至当前的累计权重}。任一 ScanInt 失败 → ret0。
bool setCompoundAvatarData(tagCompoundAvatar* avatar, ENUM_EQUIPMENTTYPE equip)
{
    const int idx = static_cast<int>(equip);
    bool ok = false;

    const unsigned int countA = static_cast<unsigned int>(ScanInt(&ok));
    if (!ok)
        return false;
    avatar->m_optsA[idx].reserve(countA);
    for (unsigned int i = 0; i < countA; ++i)
    {
        const int item = ScanInt(&ok);
        if (!ok)
            return false;
        const int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_b[idx] += weight;
        avatar->m_optsA[idx].push_back(std::make_pair(item, avatar->m_b[idx]));
    }

    const unsigned int countC = static_cast<unsigned int>(ScanInt(&ok));
    if (!ok)
        return false;
    avatar->m_optsC[idx].reserve(countC);
    for (unsigned int i = 0; i < countC; ++i)
    {
        const int item = ScanInt(&ok);
        if (!ok)
            return false;
        const int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_d[idx] += weight;
        avatar->m_optsC[idx].push_back(std::make_pair(item, avatar->m_d[idx]));
    }

    while (true)
    {
        const int item = ScanInt(&ok);
        if (!ok)
            break;
        const int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_c[idx] += weight;
        avatar->m_optsB[idx].push_back(std::make_pair(item, avatar->m_c[idx]));
    }
    return true;
}

// ORIG setCompoundRareAvatarData T 0x08902e78（报告全文转写）：开列式稀有选项，
// 权重累计入 m_cube.m_field0c[e]，pair 入 m_cube.m_vecs[e]。type==0x16 → ret0。
bool setCompoundRareAvatarData(tagCompoundAvatar* avatar, ENUM_EQUIPMENTTYPE equip)
{
    const int idx = static_cast<int>(equip);
    if (idx == 0x16)
        return false;

    bool ok = false;
    while (true)
    {
        const int item = ScanInt(&ok);
        if (!ok)
            break;
        const int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_cube.m_field0c[idx] += weight;
        avatar->m_cube.m_vecs[idx].push_back(
            std::make_pair(item, avatar->m_cube.m_field0c[idx]));
    }
    return true;
}

// ORIG importCompoundAvatarTableCore T 0x089031d2（报告 ## 汇编 + ## 反编译 C
// 全文转写）。table 实为 11×tagCompoundAvatarTable 数组首址（job*0xc 定位）。
int importCompoundAvatarTableCore(const char* dir, const char* path,
                                           tagCompoundAvatarTable* table)
{
    if (!loadRDARScriptFile(dir, path))
        return 0xf1e;

    tagCompoundAvatarTable* curTable = 0;
    tagCompoundAvatar* avatar = 0;
    int grade = 0;
    bool ok = false;
    std::string token;
    std::string str;

    while (true)
    {
        if (!ScanType(token, true))
            return 0;

        if (token == "[character type]")
        {
            if (!ScanStr(&str))
                return 0xf2d;
            const int job = getCharacterJob(str.c_str());
            if (job == 0xb)
                return 0xf32;
            curTable = table + job;                       // base + job*0xc
        }
        else if (token == "[grade]")
        {
            grade = ScanInt(&ok);
            if (!ok)
                return 0xf37;
            if (curTable == 0)
                return 0xf39;
            tagCompoundAvatar tmp;                        // memset+ctor 由默认构造覆盖
            curTable->m_avatars.resize(grade + 1, tmp);
            avatar = &curTable->m_avatars[grade];
            avatar->Clean();
            avatar->m_idx = grade;
        }
        else if (token == "[rare rate]")
        {
            while (ScanStr(&str))
            {
                const int rate = ScanInt(&ok);
                if (!ok)
                    return 0xf48;
                const int parts = getCompoundAvatarPartsType(str);
                if (parts == 0x16 || avatar == 0)
                    return 0xf4c;
                avatar->m_a[parts] = rate;
            }
        }
        else if (token == "[upper grade]")
        {
            if (avatar == 0)
                return 0xf53;
            avatar->m_flag2dc = ScanInt(&ok);
            if (!ok)
                return 0xf56;
        }
        else if (token == "[upper rare rate]")
        {
            if (avatar == 0)
                return 0xf5b;
            while (ScanStr(&str))
            {
                const int rate = ScanInt(&ok);
                if (!ok)
                    return 0xf63;
                const int parts = getCompoundAvatarPartsType(str);
                if (parts == 0x16)
                    return 0xf67;
                avatar->m_e[parts] = rate;
            }
        }
        else if (token == "[hat avatar]")
        {
            if (avatar == 0) return 0xf6e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(0)))
                return 0xf71;
        }
        else if (token == "[hair avatar]")
        {
            if (avatar == 0) return 0xf76;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(1)))
                return 0xf79;
        }
        else if (token == "[face avatar]")
        {
            if (avatar == 0) return 0xf7e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(2)))
                return 0xf81;
        }
        else if (token == "[neck avatar]")
        {
            if (avatar == 0) return 0xf86;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(6)))
                return 0xf89;
        }
        else if (token == "[coat avatar]")
        {
            if (avatar == 0) return 0xf8e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(3)))
                return 0xf91;
        }
        else if (token == "[pants avatar]")
        {
            if (avatar == 0) return 0xf96;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(4)))
                return 0xf99;
        }
        else if (token == "[belt avatar]")
        {
            if (avatar == 0) return 0xf9e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(7)))
                return 0xfa1;
        }
        else if (token == "[shoes avatar]")
        {
            if (avatar == 0) return 0xfa6;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(5)))
                return 0xfa9;
        }
        else if (token == "[skin avatar]")
        {
            if (avatar == 0) return 0xfae;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(8)))
                return 0xfb1;
        }
        else if (token == "[aurora avatar]")
        {
            if (avatar == 0) return 0xfb7;
            do
            {
                if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(9)))
                    return 0xfba;
            } while (false);
        }
        else if (token == "[material]")
        {
            if (avatar == 0) return 0xfca;
            const int count = ScanInt(&ok);
            if (!ok) return 0xfcd;
            for (int i = 0; i < count; ++i)
            {
                const int item = ScanInt(&ok);
                if (!ok) break;
                const int weight = ScanInt(&ok);
                if (!ok) return 0xfd4;
                avatar->m_tailA.push_back(std::make_pair(item, weight));
                if (weight != 1) return 0xfd9;
            }
        }
        else if (token == "[material_upper]")
        {
            if (avatar == 0) return 0x100c;
            const int count = ScanInt(&ok);
            if (!ok) return 0x100f;
            for (int i = 0; i < count; ++i)
            {
                const int item = ScanInt(&ok);
                if (!ok) break;
                const int weight = ScanInt(&ok);
                if (!ok) return 0x1016;
                avatar->m_tailB.push_back(std::make_pair(item, weight));
                if (weight != 1) return 0x101b;
            }
        }
        else if (token == "[material_lower]")
        {
            if (avatar == 0) return 0x1023;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1026;
            for (int i = 0; i < count; ++i)
            {
                const int item = ScanInt(&ok);
                if (!ok) break;
                const int weight = ScanInt(&ok);
                if (!ok) return 0x102d;
                avatar->m_tailC.push_back(std::make_pair(item, weight));
                if (weight != 1) return 0x1032;
            }
        }
        else if (token == "[material_ancient]")
        {
            if (avatar == 0) return 0x103a;
            const int count = ScanInt(&ok);
            if (!ok) return 0x103d;
            for (int i = 0; i < count; ++i)
            {
                const int item = ScanInt(&ok);
                if (!ok) break;
                const int weight = ScanInt(&ok);
                if (!ok) return 0x1044;
                avatar->m_cube.m_vec.push_back(std::make_pair(item, weight));
                if (weight != 1) return 0x1049;
            }
        }
        else if (token == "[ancient bind cube]")
        {
            if (avatar == 0) return 0x104f;
            while (ScanType(token, true) && token != "[/ancient bind cube]")
            {
                if (token == "[hat rare avatar]" || token == "[hair rare avatar]" ||
                    token == "[face rare avatar]" || token == "[neck rare avatar]" ||
                    token == "[coat rare avatar]" || token == "[pants rare avatar]" ||
                    token == "[belt rare avatar]" || token == "[shoes rare avatar]")
                {
                    if (!setCompoundRareAvatarData(
                            avatar,
                            static_cast<ENUM_EQUIPMENTTYPE>(
                                getCompoundRareAvatarType(token))))
                        return 0x105d;
                }
            }
        }
    }
}

// ORIG importCompoundAvatarTable T 0x08903ea7（报告全文转写）：
// loadRDARScriptListFile 取 11 个子文件名（≠11 → 0x106e），逐个 Core 导入
// （失败 → 0x1075），全部成功 ret0。
// ============================ 头像合成域（声明修正） ============================

// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii... 0x088bbd77 T
//（定义 STScriptFileList.cpp；第 4 参 ORIG 未使用）。
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* unused);

// _Z29importCompoundAvatarTableCorePKcS0_P22tagCompoundAvatarTable 0x089031d2 T。
int importCompoundAvatarTableCore(const char* dir, const char* path,
                                  tagCompoundAvatarTable* table);

int importCompoundAvatarTable(const char* dir, const char* path,
                              tagCompoundAvatarTable* table)
{
    STScriptFileList list;
    if (!loadRDARScriptListFile(&list, dir, path, 0))
        return 0x107b;
    if (list.m_fileMap.size() != 11)
        return 0x106e;

    for (std::map<int, std::string>::iterator it = list.m_fileMap.begin();
         it != list.m_fileMap.end(); ++it)
    {
        if (importCompoundAvatarTableCore(dir, it->second.c_str(), table) != 0)
            return 0x1075;
    }
    return 0;
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
int getModuleTypeFromTag(std::string& tag)
{
    for (int i = 0; i < 17; ++i)
    {
        if (tag == s_moduleNameTag.entries[i].open)
            return i;
    }
    return -1;
}

// ORIG addCreatureModuleRate T 0x08a38a08（报告全文转写）：已有时乘旧值后覆盖写入。
void addCreatureModuleRate(ENUM_MODULE_TYPE type, float rate)
{
    std::map<ENUM_MODULE_TYPE, float>::iterator it =
        g_creatureModuleDamageRateMap.find(type);
    if (it != g_creatureModuleDamageRateMap.end())
        rate = rate * it->second;
    g_creatureModuleDamageRateMap[type] = rate;
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
bool importCreatureCommonScript(CreatureCommonScript* script, const char* path,
                                std::vector<int>* damageTable)
{
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, path))
        return false;

    script->clear();
    damageTable->clear();

    std::string token;
    std::string str;
    int value = 0;
    bool ok = true;

    while (ScanType(token, true))
    {
        if (token == "[attack constant]")
        {
            script->m_attackConstant = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[response delay]")
        {
            script->m_responseDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[second per fill point spend]")
        {
            script->m_secondPerFillPointSpend = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[second per fill point charge]")
        {
            script->m_secondPerFillPointCharge = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[MAX fill point]")
        {
            script->m_maxFillPoint = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[fill point to eat feed]")
        {
            script->m_fillPointToEatFeed = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[evolution fade in delay]")
        {
            script->m_evolutionFadeInDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[evolution fade out delay]")
        {
            script->m_evolutionFadeOutDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[evolution delay]")
        {
            script->m_evolutionDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[dying delay]")
        {
            script->m_dyingDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[ambient in room list]")
        {
            script->m_ambientRoomList[0] = ScanInt(static_cast<bool*>(0));
            script->m_ambientRoomList[1] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[ambient in main game]")
        {
            script->m_ambientMainGame[0] = ScanInt(static_cast<bool*>(0));
            script->m_ambientMainGame[1] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[ambient in pvp]")
        {
            script->m_ambientPvp[0] = ScanInt(static_cast<bool*>(0));
            script->m_ambientPvp[1] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[damage]")
        {
            script->m_damage.clear();
            while ((value = ScanInt(&ok), ok))
            {
                script->m_damage.push_back(value);
                damageTable->push_back(value);
            }
        }
        else if (token == "[mana]")
        {
            script->m_mana.clear();
            while ((value = ScanInt(&ok), ok))
                script->m_mana.push_back(value);
        }
        else if (token == "[module damage percent]")
        {
            g_creatureModuleDamageRateMap.clear();
            while (ScanStr(&str) && (value = ScanInt(&ok), ok))
            {
                const ENUM_MODULE_TYPE type =
                    static_cast<ENUM_MODULE_TYPE>(getModuleTypeFromTag(str));
                addCreatureModuleRate(type, static_cast<float>(value) / 100.0f);
            }
        }
        else if (token == "[assault creature damage penalty]")
        {
            g_creatureIndexDamageRateMap.clear();
            while ((value = ScanInt(&ok), ok))
            {
                const int idx = value;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                g_creatureIndexDamageRateMap[idx] = static_cast<float>(value) / 100.0f;
            }
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2701,
                "script loading failed. | file <%s> tag <%s>",
                path, token.data());
            return false;
        }
    }
    return true;
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
bool initObjectCategory()
{
    s_objCategoryString["[human]"] = ENUM_CATEGORY_HUMAN;
    s_objCategoryString["[beast]"] = ENUM_CATEGORY_BEAST;
    s_objCategoryString["[plant]"] = ENUM_CATEGORY_PLANT;
    s_objCategoryString["[insect]"] = ENUM_CATEGORY_INSECT;
    s_objCategoryString["[machine]"] = ENUM_CATEGORY_MACHINE;
    s_objCategoryString["[organic]"] = ENUM_CATEGORY_ORGANIC;
    s_objCategoryString["[hybrid]"] = ENUM_CATEGORY_HYBRID;
    s_objCategoryString["[undead]"] = ENUM_CATEGORY_UNDEAD;
    s_objCategoryString["[devil]"] = ENUM_CATEGORY_DEVIL;
    s_objCategoryString["[spirit]"] = ENUM_CATEGORY_SPIRIT;
    s_objCategoryString["[dragon]"] = ENUM_CATEGORY_DRAGON;
    s_objCategoryString["[angel]"] = ENUM_CATEGORY_ANGEL;
    s_objCategoryString["[melee combat]"] = ENUM_CATEGORY_MELEE_COMBAT;
    s_objCategoryString["[range combat]"] = ENUM_CATEGORY_RANGE_COMBAT;
    s_objCategoryString["[close-passive]"] = ENUM_CATEGORY_CLOSE_PASSIVE;
    s_objCategoryString["[close-carefully]"] = ENUM_CATEGORY_CLOSE_CAREFULLY;
    s_objCategoryString["[full-superarmor]"] = ENUM_CATEGORY_FULL_SUPERARMOR;
    s_objCategoryString["[close-tough]"] = ENUM_CATEGORY_CLOSE_TOUGH;
    s_objCategoryString["[oneside move]"] = ENUM_CATEGORY_ONESIDE_MOVE;
    s_objCategoryString["[close-manace]"] = ENUM_CATEGORY_CLOSE_MANACE;
    s_objCategoryString["[human armor]"] = ENUM_CATEGORY_HUMAN_ARMOR;
    s_objCategoryString["[fort armor]"] = ENUM_CATEGORY_FORT_ARMOR;
    s_objCategoryString["[divine armor]"] = ENUM_CATEGORY_DIVINE_ARMOR;
    s_objCategoryString["[goblin]"] = ENUM_CATEGORY_GOBLIN;
    s_objCategoryString["[tau]"] = ENUM_CATEGORY_TAU;
    s_objCategoryString["[lugaru]"] = ENUM_CATEGORY_LUGARU;
    s_objCategoryString["[fixture]"] = ENUM_CATEGORY_FIXTURE;
    s_objCategoryString["[stay]"] = ENUM_CATEGORY_STAY;
    s_objCategoryString["[melee]"] = ENUM_CATEGORY_MELEE;
    s_objCategoryString["[missile]"] = ENUM_CATEGORY_MISSILE;
    s_objCategoryString["[energy]"] = ENUM_CATEGORY_ENERGY;
    s_objCategoryString["[physical]"] = ENUM_CATEGORY_PHYSICAL;
    s_objCategoryString["[magical]"] = ENUM_CATEGORY_MAGICAL;
    s_objCategoryString["[bullet]"] = ENUM_CATEGORY_BULLET;
    s_objCategoryString["[cannon]"] = ENUM_CATEGORY_CANNON;
    s_objCategoryString["[explosion]"] = ENUM_CATEGORY_EXPLOSION;
    s_objCategoryString["[inflame]"] = ENUM_CATEGORY_INFLAME;
    s_objCategoryString["[non inflame]"] = ENUM_CATEGORY_NON_INFLAME;
    s_objCategoryString["[light]"] = ENUM_CATEGORY_LIGHT;
    s_objCategoryString["[dark]"] = ENUM_CATEGORY_DARK;
    s_objCategoryString["[water]"] = ENUM_CATEGORY_WATER;
    s_objCategoryString["[fire]"] = ENUM_CATEGORY_FIRE;
    s_objCategoryString["[fort]"] = ENUM_CATEGORY_FORT;
    s_objCategoryString["[unbeatable]"] = ENUM_CATEGORY_UNBEATABLE;
    return true;
}

// ORIG initObjectScriptBase T 0x08a0aeb8：initObjectCategory 的布尔透传包装。
bool initObjectScriptBase()
{
    return initObjectCategory();
}

// ORIG getObjectCategory T 0x08a3ca1b（报告全文转写）：按值收参；查不到 0x2c。
int getObjectCategory(std::string name)
{
    std::map<std::string, ENUM_CATEGORY, IgnoreCaseCompare>::const_iterator it =
        s_objCategoryString.find(name);
    if (it == s_objCategoryString.end())
        return 0x2c;
    return it->second;
}

// ============================ 强化脚本域 ============================

// ORIG stInvestAmplifyOption_t C1 W 0x0822aea6（定义 CItemAmplifier.cpp）——仅声明引用。

// ORIG importAmplifyItem T 0x0890a6b0（报告 ## 汇编 + ## 反编译 C 全文转写）：
// 返回错误行号，0=成功。[option mapping table] 建 map<string,ENUM_AMPLIFY_TYPE>；
// [option data] 查表得类型，{type,(short)(sum),(short)value} 入 m_options；
// 三个 hash_map 投资段 key=ScanInt、value={查表类型, 第二个 ScanInt}。
int importAmplifyItem(const char* path, stAmplifyItemScript_t& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x1d21;

    typedef std::map<std::string, ENUM_AMPLIFY_TYPE> AmplifyTypeMap;
    AmplifyTypeMap typeMap;
    std::string token;
    std::string str;
    int value = 0;
    bool ok = false;

    while (true)
    {
        if (!ScanType(token, true))
            return 0;

        if (token == "[option mapping table]")
        {
            while (ScanStr(&str))
            {
                value = ScanInt(&ok);
                if (!ok)
                    return 0x1d35;
                if (!(typeMap.insert(
                          std::make_pair(str,
                                         static_cast<ENUM_AMPLIFY_TYPE>(value)))
                          .second))
                    return 0x1d39;
            }
        }
        else if (token == "[amplification weight by level]")
        {
            out.m_weightByLevel = ScanFloat(3, &ok);
            if (!ok)
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                    0x1d40, "ERROR [amplification weight by level]");
        }
        else if (token == "[amplification rate by rarity]")
        {
            for (int i = 0;; ++i)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                if (i > 5)
                    return 0x1d4d;
                out.m_rarityProb[i] = value;
            }
        }
        else if (token == "[option data]")
        {
            int sum = 0;
            while (ScanStr(&str))
            {
                AmplifyTypeMap::iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1d5e;
                const ENUM_AMPLIFY_TYPE type = it->second;
                const int prob = ScanInt(&ok);
                if (!ok)
                    return 0x1d64;
                const short probSum = static_cast<short>(prob + sum);
                const unsigned short val =
                    static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                    return 0x1d69;
                sum += prob;
                stAmplifyOptionInfo_t info;
                info.m_type = type;
                info.m_prob = static_cast<unsigned short>(probSum);
                info.m_value = val;
                out.m_options.push_back(info);
            }
        }
        else if (token == "[rarity weight]")
        {
            while (ScanStr(&str))
            {
                const float weight = ScanFloat(&ok);
                if (!ok)
                    return 0x1d78;
                const int rarity = getItemRarity(str.c_str());
                if (rarity == 6)
                    return 0x1d7b;
                out.m_rarityRate[rarity] = weight;
            }
        }
        else if (token == "[bonus weight by equipment type]")
        {
            for (int eq = 10; eq <= 21; ++eq)
            {
                out.m_bonusWeightByEquipType[eq - 10] = ScanFloat(4, &ok);
                if (!ok)
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0x1d89, "ERROR : [bonus weight by equipment type]");
            }
        }
        else if (token == "[equip level const]")
        {
            // ORIG：本段与「未匹配任何段名」的链尾默认路径共用同一段代码
            // （反编译 C 的 do-while 尾部 ScanInt 实证）；默认分支见链尾。
            out.m_field3c = ScanInt(&ok);
            if (!ok)
                return 0x1d92;
        }
        else if (token == "[purify material]")
        {
            const int item = ScanInt(&ok);
            if (!ok) return 0x1d98;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1d9b;
            out.m_purifyMatItem = item;
            out.m_purifyMatCount = count;
        }
        else if (token == "[purify only material]")
        {
            const int item = ScanInt(&ok);
            if (!ok) return 0x1da8;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1dab;
            out.m_purifyOnlyMatItem = item;
            out.m_purifyOnlyMatCount = count;
        }
        else if (token == "[invest option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;      // ORIG ctor 0x0822aea6
                if (!ScanStr(&str))
                    break;
                AmplifyTypeMap::iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1dbd;
                entry.m_amount = it->second;
                const unsigned long key =
                    static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dc3;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1dc7;
                entry.m_value = v;
                out.m_investMap[0].insert(
                    std::make_pair(key, entry));
            }
        }
        else if (token == "[reinvest option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;
                if (!ScanStr(&str))
                    break;
                AmplifyTypeMap::iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1dd8;
                entry.m_amount = it->second;
                const unsigned long key =
                    static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dde;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1de2;
                entry.m_value = v;
                out.m_investMap[1].insert(std::make_pair(key, entry));
            }
        }
        else if (token == "[random invest upgrade option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;
                if (!ScanStr(&str))
                    break;
                AmplifyTypeMap::iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1df4;
                entry.m_amount = it->second;
                const unsigned long key =
                    static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dfa;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1dfe;
                entry.m_value = v;
                out.m_investMap[2].insert(std::make_pair(key, entry));
            }
        }
        else
        {
            // 链尾合并默认路径（与 [equip level const] 同一代码）。
            out.m_field3c = ScanInt(&ok);
            if (!ok)
                return 0x1d92;
        }
    }
}

// ============================ 武器信息 / 角色脚本域 ============================

// ORIG WeaponInfo::clear W 0x08a3cd8e（objdump 转写，无独立报告）。
void WeaponInfo::clear()
{
    m_wavs.clear();
    m_hitType = 5;
    m_blood = 0;
    m_field14 = 0;
    m_rate18 = 1.0f;
    m_rate1c = 1.0f;
    m_rate20 = 1.0f;
    for (int i = 0; i < 4; ++i)
        m_skillRates[i] = 1.0f;
}

// ORIG WeaponInfo C1 W 0x08a3ce08：vector 构造后 clear()。D1 W 0x083dc13a 平凡
//（成员逆序析构由编译器生成）；copy 为逐成员语义（ORIG C1ERKS_ W 0x08a3dfca）。
WeaponInfo::WeaponInfo() { clear(); }
WeaponInfo::~WeaponInfo() {}

// ORIG STRDCharacterStatus::Clear W 0x08367642（objdump 全文转写，无独立报告）：
// +0x52 字节与 +0x53 dword 不在 [HP MAX]/[weight] 等导入写入区（仅清零）。
void STRDCharacterStatus::Clear()
{
    m_hpMax = 0;
    m_mpMax = 0;
    m_physAttack = 0;
    m_physDefense = 0;
    m_magAttack = 0;
    m_magDefense = 0;
    for (int i = 0; i < 4; ++i)
        m_elemResist[i] = 0;
    for (int i = 0; i < 17; ++i)
        m_activeResist[i] = 0;
    m_inventoryLimit = 0;
    m_hpRegen = 0;
    m_mpRegen = 0;
    m_moveSpeed = 0;
    m_attackSpeed = 0;
    m_castSpeed = 0;
    m_hitRecovery = 0;
    m_jumpPower = 0;
    m_weight = 0;
    m_field52 = 0;
    m_field53 = 0;
}

// ORIG C1 W 0x0836778c：vector C1 + clear()。
CharacterEtcStats::CharacterEtcStats()
    : m_category()
{
    clear();
}

// ORIG CharacterEtcStats::clear W 0x08367752（objdump 转写）：vector clear +
// +0xc..+0x18 四个 int 清零；+0x4..+0xb 未触。
void CharacterEtcStats::clear()
{
    m_category.clear();
    for (int i = 0; i < 4; ++i)
        m_moduleDamageRate[i] = 0.0f;
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
        m_elemResist[i] = s.m_elemResist[i];
    for (int i = 0; i < 17; ++i)
        m_activeResist[i] = s.m_activeResist[i];
    m_inventoryLimit = s.m_inventoryLimit;
    m_hpRegen = s.m_hpRegen;
    m_mpRegen = s.m_mpRegen;
    m_moveSpeed = s.m_moveSpeed;
    m_attackSpeed = s.m_attackSpeed;
    m_castSpeed = s.m_castSpeed;
    m_hitRecovery = s.m_hitRecovery;
    m_jumpPower = s.m_jumpPower;
    m_weight = s.m_weight;
    m_field52 = s.m_field52;
    m_field53 = s.m_field53;
    return *this;
}

// ORIG _ComboInfo C1（W）：逐成员构造后调 clear()。
_ComboInfo::_ComboInfo()
{
    clear();
}

// ORIG _ComboInfo::clear W 0x0836785c（objdump 转写）。
void _ComboInfo::clear()
{
    m_mapA.clear();
    m_mapB.clear();
    m_vec.clear();
}

// ORIG STSecondGrowTypeData::clear W 0x08367a28（objdump 转写）：双状态块 +
// 双技能表 + 双 etc 域。
void STCharacterScript::STSecondGrowTypeData::clear()
{
    for (int i = 0; i < 2; ++i)
    {
        (i == 0 ? m_statusA : m_statusB).Clear();
        m_skills[i].clear();
        m_etc[i].clear();
    }
}

// ORIG STCharacterScript::Clear W 0x08367a98（objdump 0x3a5 字节全文转写）。
void STCharacterScript::Clear()
{
    m_job = 0xb;
    m_width = 0;
    m_height = 0;
    for (int i = 0; i <= 6; ++i)
    {
        if (i < 6)
        {
            m_growtypeName[i] = "";
            m_awakeningName[i][0] = "";
            m_awakeningName[i][1] = "";
        }
        (i == 0 ? m_initialStatus : m_growStatus[i - 1]).Clear();
        m_skillList[i].clear();
        m_etcStats[i].clear();
        m_secondGrow[i].clear();
    }
    // 注意：ORIG Clear 不触 +0xb38 ghostMotion 与 +0xb5c ActiveActionFileSet。
    m_attackMotion.clear();                  // +0xb44
    m_motionWaiting = "";
    m_motionMove = "";
    m_motionSit = "";
    m_motionDamage1 = "";                    // +0xae0
    m_motionDamage2 = "";                    // +0xae4
    m_motionDown = "";
    m_motionOverturn = "";
    m_motionJump = "";
    m_motionJumpattack = "";
    m_motionRest = "";
    m_throwMotion11 = "";                    // +0xb04..+0xb10（throw N-1）
    m_throwMotion21 = "";
    m_throwMotion31 = "";
    m_throwMotion41 = "";
    m_throwMotion12 = "";                    // +0xb14..+0xb20（throw N-2）
    m_throwMotion22 = "";
    m_throwMotion32 = "";
    m_throwMotion42 = "";
    m_motionDash = "";
    m_motionDashattack = "";
    m_motionGetitem = "";
    m_motionBuff = "";
    m_etcMotion.clear();
    m_jumpattackInfo = "";
    m_dashattackInfo = "";
    m_attackInfo.clear();
    m_etcAttackInfo.clear();
    m_weaponWav.clear();
    m_scriptPath = "";
    m_jumpSpeed = 100;
    m_fieldbd4 = 0;
    m_fieldbd8 = 1.0f;
    m_combo.clear();
}

// ============================ ImportCharacterScript ============================

// ORIG static d 0x094ff618 [0x18]：job → 身体贴图路径（原文件级静态；本 TU
// 以文件级静态等价建模）。
static std::map<ENUM_CHARACTERJOB, std::string> s_bodyImagePath;

// ORIG ImportCharacterScript T 0x08a0ce06（报告 ## 汇编 + ## 反编译 C 全文转写）。
// 返回：1=解析至 EOF；0=任一段失败。
bool ImportCharacterScript(STCharacterScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory, path))
        return false;

    script->Clear();
    script->m_scriptName = path;
    getDirectoryPath(script->m_scriptPath, "%s%s",
                     g_szCharacterScriptBaseDirectory, path);

    STRDCharacterStatus* curStatus = &script->m_initialStatus;
    std::vector<std::pair<int, int> >* curSkills = &script->m_skillList[0];
    CharacterEtcStats* curEtc = &script->m_etcStats[0];
    // ORIG 汇编 +0x00d1/0x00d8：local_40(二转块)/local_3c(觉醒名) 初值为 0，
    // 仅 [initial value]/[growtype N] 分支落位；此前出现 [awakening 1/2/name]
    // 属畸形输入（ORIG 空指针解引用）。
    STCharacterScript::STSecondGrowTypeData* curSecond = 0;
    std::string* curAwakenName = 0;

    std::string token;
    std::string str;
    std::string listTok;
    int value = 0;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[job]")
        {
            if (!ScanStr(&str)) return false;
            script->m_job = getCharacterJob(str.c_str());
        }
        else if (token == "[growtype name]")
        {
            for (int i = 0; i < 6; ++i)
                if (!ScanStr(&script->m_growtypeName[i])) return false;
        }
        else if (token == "[width]")
        {
            if (!ScanInt(&script->m_width)) return false;
            if (!ScanInt(&script->m_height)) return false;
        }
        else if (token == "[jump speed]")
        {
            if (!ScanInt(&script->m_jumpSpeed)) return false;
        }
        else if (token == "[initial value]")
        {
            curStatus = &script->m_initialStatus;
            curSkills = &script->m_skillList[0];
            curEtc = &script->m_etcStats[0];
            curSecond = &script->m_secondGrow[0];
        }
        else if (token == "[growtype 1]" || token == "[growtype 2]" ||
                 token == "[growtype 3]" || token == "[growtype 4]" ||
                 token == "[growtype 5]" || token == "[growtype 6]")
        {
            const int n = token[10] - '1';
            curStatus = &script->m_growStatus[n];
            curSkills = &script->m_skillList[n + 1];
            curEtc = &script->m_etcStats[n + 1];
            curSecond = &script->m_secondGrow[n + 1];
            curAwakenName = &script->m_awakeningName[n][0];
        }
        else if (token == "[awakening 1]" || token == "[awakening 2]")
        {
            if (token == "[awakening 1]")
            {
                curStatus = &curSecond->m_statusA;
                curSkills = &curSecond->m_skills[0];
                curEtc = &curSecond->m_etc[0];
            }
            else
            {
                curStatus = &curSecond->m_statusB;
                curSkills = &curSecond->m_skills[1];
                curEtc = &curSecond->m_etc[1];
            }
        }
        else if (token == "[awakening name]")
        {
            if (!ScanStr(curAwakenName) || !ScanStr(curAwakenName + 1))
                return false;
        }
        else if (token == "[awakening skill]")
        {
            curSkills->clear();
            while (true)
            {
                bool isType = false, isInt = false;
                value = ScanTypeOrInt(isType, listTok, isInt, true);
                if (!isInt) return false;
                if (isType)
                {
                    if (listTok != "[/awakening skill]") return false;
                    break;
                }
                const int left = value;
                value = ScanInt(static_cast<bool*>(0));
                curSkills->push_back(std::make_pair(left, value));
            }
        }
        else if (token == "[skill]")
        {
            curSkills->clear();
            while (true)
            {
                bool isType = false, isInt = false;
                value = ScanTypeOrInt(isType, listTok, isInt, true);
                if (!isInt) return false;
                if (isType)
                {
                    if (listTok != "[/skill]") return false;
                    break;
                }
                const int left = value;
                value = ScanInt(static_cast<bool*>(0));
                curSkills->push_back(std::make_pair(left, value));
            }
        }
        else if (token == "[HP MAX]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_hpMax = GetRoundInt(10.0f * v);
        }
        else if (token == "[MP MAX]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_mpMax = GetRoundInt(10.0f * v);
        }
        else if (token == "[physical attack]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_physAttack =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[physical defense]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_physDefense =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[magical attack]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_magAttack =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[magical defense]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_magDefense =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[fire resistance]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_elemResist[0] =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[water resistance]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_elemResist[1] =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[dark resistance]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_elemResist[2] =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[light resistance]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_elemResist[3] =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[inventory limit]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_inventoryLimit = GetRoundInt(10.0f * v);
        }
        else if (token == "[HP regen speed]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_hpRegen =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[MP regen speed]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_mpRegen =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[move speed]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_moveSpeed = GetRoundInt(10.0f * v);
        }
        else if (token == "[attack speed]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_attackSpeed =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[cast speed]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_castSpeed =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[hit recovery]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_hitRecovery =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[jump power]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_jumpPower =
                static_cast<unsigned short>(GetRoundInt(10.0f * v));
        }
        else if (token == "[weight]")
        {
            float v = 0.0f;
            if (!ScanFloat(&v)) return false;
            curStatus->m_weight = GetRoundInt(10.0f * v);
        }
        else if (token == "[category]")
        {
            curEtc->m_category.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (!(listTok != "[/category]")) break;
                    return false;
                }
                const int cat = getObjectCategory(listTok);
                if (cat != 0x2c)
                    curEtc->m_category.push_back(cat);
            }
        }
        else if (token == "[module damage rate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                float v = 0.0f;
                if (!ScanFloat(&v)) return false;
                curEtc->m_moduleDamageRate[i] = v;
            }
        }
        else if (token == "[waiting motion]")   { if (!ScanStr(&script->m_motionWaiting)) return false; }
        else if (token == "[move motion]")      { if (!ScanStr(&script->m_motionMove)) return false; }
        else if (token == "[sit motion]")       { if (!ScanStr(&script->m_motionSit)) return false; }
        else if (token == "[damage motion 1]")  { if (!ScanStr(&script->m_motionDamage1)) return false; }
        else if (token == "[damage motion 2]")  { if (!ScanStr(&script->m_motionDamage2)) return false; }
        else if (token == "[down motion]")      { if (!ScanStr(&script->m_motionDown)) return false; }
        else if (token == "[overturn motion]")  { if (!ScanStr(&script->m_motionOverturn)) return false; }
        else if (token == "[jump motion]")      { if (!ScanStr(&script->m_motionJump)) return false; }
        else if (token == "[jumpattack motion]"){ if (!ScanStr(&script->m_motionJumpattack)) return false; }
        else if (token == "[rest motion]")      { if (!ScanStr(&script->m_motionRest)) return false; }
        else if (token == "[throw motion 1-1]") { if (!ScanStr(&script->m_throwMotion11)) return false; }
        else if (token == "[throw motion 2-1]") { if (!ScanStr(&script->m_throwMotion21)) return false; }
        else if (token == "[throw motion 3-1]") { if (!ScanStr(&script->m_throwMotion31)) return false; }
        else if (token == "[throw motion 4-1]") { if (!ScanStr(&script->m_throwMotion41)) return false; }
        else if (token == "[throw motion 1-2]") { if (!ScanStr(&script->m_throwMotion12)) return false; }
        else if (token == "[throw motion 2-2]") { if (!ScanStr(&script->m_throwMotion22)) return false; }
        else if (token == "[throw motion 3-2]") { if (!ScanStr(&script->m_throwMotion32)) return false; }
        else if (token == "[throw motion 4-2]") { if (!ScanStr(&script->m_throwMotion42)) return false; }
        else if (token == "[dash motion]")      { if (!ScanStr(&script->m_motionDash)) return false; }
        else if (token == "[dashattack motion]"){ if (!ScanStr(&script->m_motionDashattack)) return false; }
        else if (token == "[getitem motion]")   { if (!ScanStr(&script->m_motionGetitem)) return false; }
        else if (token == "[buff motion]")      { if (!ScanStr(&script->m_motionBuff)) return false; }
        else if (token == "[back motion]")      { if (!ScanStr(&script->m_motionBack)) return false; }
        else if (token == "[simple rest motion]")  { if (!ScanStr(&script->m_motionSimpleRest)) return false; }
        else if (token == "[simple move motion]")  { if (!ScanStr(&script->m_motionSimpleMove)) return false; }
        else if (token == "[ghost motion]")
        {
            script->m_ghostMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/ghost motion]") break;
                    return false;
                }
                script->m_ghostMotion.push_back(listTok);
            }
        }
        else if (token == "[attack motion]")
        {
            script->m_attackMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/attack motion]") break;
                    return false;
                }
                script->m_attackMotion.push_back(listTok);
            }
        }
        else if (token == "[etc motion]")
        {
            script->m_etcMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/etc motion]") break;
                    return false;
                }
                script->m_etcMotion.push_back(listTok);
            }
        }
        else if (token == "[waiting action]")     { if (!ScanStr(&script->m_actionSet.m_waiting)) return false; }
        else if (token == "[move action]")        { if (!ScanStr(&script->m_actionSet.m_move)) return false; }
        else if (token == "[dash action]")        { if (!ScanStr(&script->m_actionSet.m_dash)) return false; }
        else if (token == "[sit action]")         { if (!ScanStr(&script->m_actionSet.m_sit)) return false; }
        else if (token == "[damage action 1]")    { if (!ScanStr(&script->m_actionSet.m_damage1)) return false; }
        else if (token == "[damage action 2]")    { if (!ScanStr(&script->m_actionSet.m_damage2)) return false; }
        else if (token == "[down action]")        { if (!ScanStr(&script->m_actionSet.m_down)) return false; }
        else if (token == "[overturn action]")    { if (!ScanStr(&script->m_actionSet.m_overturn)) return false; }
        else if (token == "[jump action]")        { if (!ScanStr(&script->m_actionSet.m_jump)) return false; }
        else if (token == "[jumpattack action]")  { if (!ScanStr(&script->m_actionSet.m_jumpattack)) return false; }
        else if (token == "[attack action]" || token == "[action list]")
        {
            script->m_actionSet.m_attackList.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    const bool endAttack =
                        token == "[attack action]" && listTok == "[/attack action]";
                    const bool endList =
                        token == "[action list]" && listTok == "[/action list]";
                    if (endAttack || endList) break;
                    return false;
                }
                script->m_actionSet.m_attackList.push_back(listTok);
            }
        }
        else if (token == "[etc action]")
        {
            script->m_actionSet.m_etc.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/etc action]") break;
                    return false;
                }
                script->m_actionSet.m_etc.push_back(listTok);
            }
        }
        else if (token == "[last action]")      { if (!ScanStr(&script->m_lastAction)) return false; }
        else if (token == "[proc action]")      { if (!ScanStr(&script->m_procAction)) return false; }
        else if (token == "[jumpattack info]")  { if (!ScanStr(&script->m_jumpattackInfo)) return false; }
        else if (token == "[dashattack info]")  { if (!ScanStr(&script->m_dashattackInfo)) return false; }
        else if (token == "[attack info]")
        {
            script->m_attackInfo.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/attack info]") break;
                    return false;
                }
                script->m_attackInfo.push_back(listTok);
            }
        }
        else if (token == "[etc attack info]")
        {
            script->m_etcAttackInfo.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/etc attack info]") break;
                    return false;
                }
                script->m_etcAttackInfo.push_back(listTok);
            }
        }
        else if (token == "[weapon wav]")
        {
            WeaponInfo info;
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/weapon wav]")
                    {
                        script->m_weaponWav.push_back(info);
                        break;
                    }
                    return false;
                }
                info.m_wavs.push_back(listTok);
            }
        }
        else if (token == "[weapon hit info]")
        {
            unsigned int idx = 0;
            for (;;)
            {
                if (!ScanTypeOrStr(ok, listTok, true)) return false;  // EOF→ORIG 0
                if (ok)
                {
                    if (listTok == "[/weapon hit info]") break;
                    return false;
                }
                int hitType;
                if (listTok == "[blow]") hitType = 0;
                else if (listTok == "[cut]") hitType = 1;
                else if (listTok == "[burn]") hitType = 2;
                else if (listTok == "[etc]") hitType = 3;
                else return false;
                if (script->m_weaponWav.size() <= idx)
                {
                    const WeaponInfo tmp;
                    script->m_weaponWav.resize(idx + 1, tmp);
                }
                WeaponInfo& w = script->m_weaponWav[idx];
                w.m_hitType = hitType;
                if (!ScanStr(&str)) return false;
                if (str == "[no blood]") w.m_blood = 0;
                else if (str == "[blood]") w.m_blood = 1;
                else return false;
                if (!ScanInt(&w.m_field14)) return false;
                if (!ScanFloat(&w.m_rate18)) return false;
                if (!ScanFloat(&w.m_rate1c)) return false;
                if (!ScanFloat(&w.m_rate20)) return false;
                ++idx;
            }
        }
        else if (token == "[weapon skill info]")
        {
            unsigned int idx = 0;
            while (true)
            {
                bool isType = false, isFloat = false;
                const float head = ScanTypeOrFloat(isType, listTok, isFloat, true);
                if (!isFloat) return false;
                if (isType)
                {
                    if (listTok == "[/weapon skill info]") break;
                    return false;
                }
                if (script->m_weaponWav.size() <= idx)
                {
                    const WeaponInfo tmp;
                    script->m_weaponWav.resize(idx + 1, tmp);
                }
                WeaponInfo& w = script->m_weaponWav[idx];
                w.m_skillRates[0] = head;
                if (!ScanFloat(&w.m_skillRates[1])) return false;
                if (!ScanFloat(&w.m_skillRates[2])) return false;
                if (!ScanFloat(&w.m_skillRates[3])) return false;
                ++idx;
            }
        }
        else if (token == "[body image path]")
        {
            if (ScanStr(&str))
                s_bodyImagePath[
                    static_cast<ENUM_CHARACTERJOB>(script->m_job)] = str;
            else
                return false;
        }
        else if (token == "[upgrade weapon attack power rate]")
        {
            int col = 0;
            while (true)
            {
                bool isType = false, isFloat = false;
                const float rate =
                    ScanTypeOrFloat(isType, listTok, isFloat, true);
                if (!isFloat) return false;
                if (isType)
                {
                    if (listTok == "[/upgrade weapon attack power rate]") break;
                    return false;
                }
                float val = 0.0f;
                if (!ScanFloat(&val)) return false;
                if (col < 6)
                {
                    upgradeWeaponAttackPowerRate_[script->m_job][col][0] = rate;
                    upgradeWeaponAttackPowerRate_[script->m_job][col][1] = val;
                }
                ++col;
            }
        }
        else if (token == "[weapon durability decrease rate]")
        {
            for (int i = 0; i < 6; ++i)
            {
                float v = 0.0f;
                if (!ScanFloat(&v)) return false;
                g_weaponDurabilityDecreaseRate_[script->m_job][i] = v;
            }
        }
        else
        {
            // ACTIVESTATUS_TAG 段（17 抗性 → m_activeResist[i]，×10 取整）；
            // ORIG 无 default 错误路径，未知标签静默忽略。
            bool matched = false;
            for (int i = 0; i < 17; ++i)
            {
                if (token == ACTIVESTATUS_TAG[i])
                {
                    float v = 0.0f;
                    if (!ScanFloat(&v)) return false;
                    curStatus->m_activeResist[i] =
                        static_cast<unsigned short>(GetRoundInt(10.0f * v));
                    matched = true;
                    break;
                }
            }
            (void)matched;
        }
    }
    return true;
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
int ImportEquipmentAniScript(std::map<std::string, STEquipmentAniScript>* table)
{
    if (!loadRDARScriptFile("", "Etc/equipmentAniScript.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptItem.cpp",
                              __FUNCTION__, 0x3116,
                              "CRITICAL ERROR : EquipmentAniScript!");
        return false;
    }

    std::string section;                 // ORIG -0x358（0x089c0655 先构造）
    // 子文件名收集。ORIG vector 与 section 同区构造（0x089c0663，先于首次
    // ScanType）；循环终止比较对象是 section token（0x089c06d9 对
    // [/equipment ani script] 比较外层 section，恒假），实际以 ScanStr 失败
    // 收尾；忠实保留该比较形态。
    std::vector<std::string> files;      // ORIG -0x364
    if (!ScanType(section, true))
        return false;
    if (section == "[equipment ani script]")
    {
        std::string name;                // ORIG -0x36c（block 作用域构造/析构）
        while (ScanStr(&name) && section != "[/equipment ani script]")
            files.push_back(name);
    }

    for (std::vector<std::string>::iterator fit = files.begin();
         fit != files.end(); ++fit)
    {
        if (!loadRDARScriptFile("", fit->c_str()))
            return false;

        std::string tag;                 // ORIG -0x370
        std::string val;                 // ORIG -0x374
        STEquipmentAniScript entry;      // ORIG -0x3f4（C1 0x0898ca36）

        while (ScanType(tag, true))
        {
            static AniLookupMap __lookup;    // 0x089c07f8 guard 区
            static SpinLock __init_lock;     // 0x089c0872 guard 区
            static bool __inited = false;    // 0x094fde04

            // ORIG 建表完成/锁内已建时 jmp 0x089c07f8（guard 检查区）=
            // 内层 for(;;) 重派发循环头。
            for (;;)
            {
                int id;
                if (__inited)
                {
                    const char* key = tag.c_str();   // 0x089c08fb
                    AniLookupMap::const_iterator it = __lookup.find(key);
                    id = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();             // 0x089c09bb
                    if (__inited)
                    {
                        __init_lock.leave();         // 0x089c09d2
                        continue;                    // jmp 0x089c07f8
                    }
                    id = 0;                          // 0x089c09dd
                }

                switch (id)
                {
                case 0:
                case 0x3142:                       // [LAYER]
                    if (__inited)
                    {
                        entry.m_field0 = ScanInt(static_cast<bool*>(0));
                        break;                     // 0x089c0bd9
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[LAYER]", 0x3142));
                    /* fall through（0x089c0bc5 → 0x089c0bde） */
                case 0x3145:                       // [waiting motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str04 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[waiting motion]", 0x3145));
                    /* fall through（→ 0x089c0c69） */
                case 0x3149:                       // [move motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str08 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[move motion]", 0x3149));
                    /* fall through */
                case 0x314d:                       // [sit motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str0c = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[sit motion]", 0x314d));
                    /* fall through */
                case 0x3151:                       // [damage motion 1]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str10 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[damage motion 1]", 0x3151));
                    /* fall through */
                case 0x3155:                       // [damage motion 2]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str14 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[damage motion 2]", 0x3155));
                    /* fall through */
                case 0x3159:                       // [down motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str18 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[down motion]", 0x3159));
                    /* fall through */
                case 0x315d:                       // [overturn motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str1c = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[overturn motion]", 0x315d));
                    /* fall through */
                case 0x3161:                       // [jump motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str20 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[jump motion]", 0x3161));
                    /* fall through */
                case 0x3165:                       // [jumpattack motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str24 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[jumpattack motion]", 0x3165));
                    /* fall through */
                case 0x3169:                       // [rest motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str28 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[rest motion]", 0x3169));
                    /* fall through */
                case 0x316d:                       // [throw motion 1-1]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str2c = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 1-1]", 0x316d));
                    /* fall through（0x089c11a7 → 0x089c11d7） */
                case 0x3171:                       // [throw motion 1-2]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str3c = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 1-2]", 0x3171));
                    /* fall through（0x089c1232 → 0x089c1262） */
                case 0x3175:                       // [throw motion 2-1]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str30 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 2-1]", 0x3175));
                    /* fall through */
                case 0x3179:                       // [throw motion 2-2]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str40 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 2-2]", 0x3179));
                    /* fall through */
                case 0x317d:                       // [throw motion 3-1]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str34 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 3-1]", 0x317d));
                    /* fall through */
                case 0x3181:                       // [throw motion 3-2]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str44 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 3-2]", 0x3181));
                    /* fall through */
                case 0x3185:                       // [throw motion 4-1]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str38 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 4-1]", 0x3185));
                    /* fall through */
                case 0x3189:                       // [throw motion 4-2]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str48 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[throw motion 4-2]", 0x3189));
                    /* fall through */
                case 0x318d:                       // [dash motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str4c = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[dash motion]", 0x318d));
                    /* fall through */
                case 0x3191:                       // [dashattack motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str50 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[dashattack motion]", 0x3191));
                    /* fall through */
                case 0x3195:                       // [getitem motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str54 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[getitem motion]", 0x3195));
                    /* fall through */
                case 0x3199:                       // [buff motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str58 = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[buff motion]", 0x3199));
                    /* fall through */
                case 0x319d:                       // [back motion]
                    if (__inited)
                    {
                        ScanStr(&val);
                        entry.m_str5c = val;
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[back motion]", 0x319d));
                    /* fall through（0x089c182b → 0x089c185b） */
                case 0x31a1:                       // [attack motion]（多行）
                    if (__inited)
                    {
                        while (ScanStr(&val))
                            entry.m_vec64.push_back(std::string(val.c_str()));
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[attack motion]", 0x31a1));
                    /* fall through（0x089c18ba → 0x089c1980） */
                case 0x31a7:                       // [etc motion]（多行）
                    if (__inited)
                    {
                        while (ScanStr(&val))
                            entry.m_vec70.push_back(std::string(val.c_str()));
                        break;
                    }
                    __lookup.insert(
                        AniLookupMap::value_type("[etc motion]", 0x31a7));
                    /* fall through（注册链尾 → 公共尾 0x089c1aa0） */
                default:
                    break;                     // 未命中（id=-1）静默
                }

                if (__inited)
                    break;                     // 解析完成 → 下一 ScanType
                __init_lock.leave();           // 0x089c1ab5
                __inited = true;               // 0x089c1ac1
                // continue → guard 重查（0x089c07f8）
            }
        }

        table->insert(std::make_pair(fit->c_str(), entry));
    }
    return true;
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
float getUpgradeWeaponAttackPowerRate(ENUM_CHARACTERJOB job, unsigned int type,
                                      unsigned int level)
{
    if (type > 5 || level > 1)
        return 0.0f;
    return upgradeWeaponAttackPowerRate_[static_cast<int>(job)][type][level];
}
