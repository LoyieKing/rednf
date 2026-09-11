// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptDepsA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
    // _Z15ScanTypeOrFloatRbRSsS_b T 0x088c2e01（定义 DNFLexWrapperHelpers.cpp）
// [bigfix-enum 待同步] ORIG [job] 0x08a0cfcd store 为 LHS-first ⇒ callee 返回
// 与 m_job 同型（enum）。本 TU 已按权威声明 NpcScript.h:74 保持 int；全链
// enum 化（SyncScriptDeps.cpp 定义 + 各 TU 镜像声明）已上报 Main，届时本处
// 的 static_cast 可删、[job] store 翻正。
#include "ScriptDepsA/getCharacterJob.inc"
    // _Z15getCharacterJobPKc T 0x088be57e（定义 SyncScriptDeps.cpp）
#include "ScriptDepsA/createLotteryInfo.inc"
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

// ORIG static d 0x093f5480 [0x44]＝17 个 .rodata 字面量指针（逐一解析实证；
// 串址 0x08e1ddd3 起，逐项与下方旧定义序列一致）。
// [SymActStatus·2026-09-11 跨 TU 合并] ORIG 仅此一份（local `_ZL16ACTIVESTATUS_TAG`；
// 引用点 0x08a0fa5d / 0x08a0fdc2 / 0x08a221fb / 0x08a22e1f 四函数同属单一 TU）。
// 我方因 TU 拆分曾在本文件与 STMonsterScript.cpp 各定义一份同内容拷贝
// ⇒ 唯一定义（外部链接、.data 段 d / size 0x44）迁至 STMonsterScript.cpp
// （与同域 PARAMETER_CATEGORY_NAME 并列），本文件只保留 extern 引用；
// 消费点 `token == ACTIVESTATUS_TAG[i]` 形不变。
extern const char* ACTIVESTATUS_TAG[17];

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
{
    // [b28d-solo-ScriptDepsA 判词] AE（Main 已落路由）：dnf_game_cxx.sh i486 组
    // 纳入本 TU 后，fucompp/fnstsw %ax/sahf 两向比较与 ORIG 0x08a3df64 全等。
    // 双后端读数：默认 707/17 → i486 708/16，本件 DIFF→AE，零回归。
    if (v >= 0.0f) return static_cast<int>(v + 0.5f);
    return static_cast<int>(v - 0.5f);
}

#include "ScriptDepsA/getElement.inc"
    // _Z10getElementPKc T 0x088bfe5b：strcmp 阶梯，未命中 4。唯一定义在
    // CreatureScript.cpp（声明见 CreatureScript.h），本 TU 仅调用。

// ORIG getItemRarity T 0x088bfd97（报告全文转写）：未命中 6（RARITY_MAX）。
#include "ScriptDepsA/getItemRarity.inc"
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
#include "ScriptDepsA/importActiveStatusTable.inc"
{
    // [b28d-solo-ScriptDepsA 判词] N82/N115 封顶（§6.2：237/235，gap=2，
    // p167 31/31 CLEAR）：本波修三处 level-rate 的具名 float 槽往返
    // （fstps→槽→读回→存全局，P252）。残差=帧 0x60 vs 0x50（ORIG 多 3 个
    // dead 槽 -0x30..-0x38）+ 全帧槽号平移 + 4.1/4.4 bool while 归一化方向
    // 相反（ORIG `xor $1;test;jne` vs 4.4 `test;jne`，探针 g1/g2 实证不可翻）
    // + power 段索引地址计算时刻差（P75）。
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0x162b,
            "ERROR : %s \306\304\300\317\300\273\040\303\243\300\273\274\366\040"
            "\276\370\275\300\264\317\264\331.", path);   // EUC-KR 字面量（ORIG 0x08e103d8）
        return false;
    }

    std::string token;
    for (;;)
    {
        // [P292/P283] ORIG 循环头 `call; xor $0x1,%eax; test %al,%al; jne exit`
        // ⇒ 原码是 for(;;)+`if (!ScanType(...)) break;`（bool 返回的取反形），
        // 非 while(ScanType(...))（后者 4.4 出无 xor 的 `test;jne body`）。
        if (!ScanType(token, true))
            break;
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
                ACTIVESTATUS_POWER1.m_pair[i].m_lo = ScanFloat(static_cast<bool*>(0));
                ACTIVESTATUS_POWER1.m_flat[i * 2 + 1] = ScanFloat(static_cast<bool*>(0));
            }
        }
        else if (token == "[active status power 2]")
        {
            for (int i = 0; i < 18; ++i)
            {
                ACTIVESTATUS_POWER2.m_pair[i].m_lo = ScanFloat(static_cast<bool*>(0));
                ACTIVESTATUS_POWER2.m_flat[i * 2 + 1] = ScanFloat(static_cast<bool*>(0));
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
            // [b28d-solo-ScriptDepsA] ORIG 0x089071dc 段：fstps 先落具名 float
            // 槽(-0x14) 再读回存全局（P252 槽往返形）。
            const float rate = 0.01f * static_cast<float>(v);
            ACTIVESTATUS_LEVEL_RATE_FOR_PROB = rate;
        }
        else if (token == "[active status level rate for power]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            const float rate = 0.01f * static_cast<float>(v);
            ACTIVESTATUS_LEVEL_RATE_FOR_POWER = rate;
        }
        else if (token == "[active status level absolute rate]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            const float rate = 0.01f * static_cast<float>(v);
            ACTIVESTATUS_LEVEL_ABSOLUTE_RATE = rate;
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
#include "ScriptDepsA/importDisjointScript.inc"
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：542/529，gap=13，
    // p167 65/65 CLEAR）：本波修 [additional result] 的 push_back 右值内联实参
    // （ORIG 0x088e5198 EOi）+ [avatar disjoint info] 终止守卫改 `==`+break
    // （P133）。残差=巨件帧/槽平移 + map/vector 模板实例内联差。
    if (!loadRDARScriptFile("", path))
        return false;

    script->clear();

    std::string token;
    std::string str;
    int value = 0;
    bool ok = false;

    // [P292/P283] ORIG 循环头 `xor $0x1,%eax; test %al,%al; jne/je` ⇒ 原码
    // 统一 for(;;)+`if (!cond) break;`。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[cube index]")
        {
            ScanStr(&str);
            const int elem = getElement(str.c_str());
            // [P183] ORIG 0x088e4e3d/0x088e4e73 两处各自重算
            // `3*elem*4 + script`（add/add/shl/add 链）⇒ 禁引用缓存，逐点重取。
            script->m_cubeIndexVecs[elem].clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_cubeIndexVecs[elem].push_back(value);
            }
        }
        else if (token == "[legacy info]")
        {
            const int idx = ScanInt(static_cast<bool*>(0));
            std::vector<int> counts;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                counts.push_back(value);
            }
            // [P171/P137] ORIG 0x088e4ef6 仅 `sub $0x1,%eax; cmp $0x3,%eax; jg`
            // 单一有符号上界（无下界守卫，idx=0 照抄越界行为）⇒ 写 idx-1 <= 3。
            if (idx - 1 <= 3)
                createLotteryInfo(&script->m_legacyInfo[idx - 1], &counts);
        }
        else if (token == "[skill info]")
        {
            script->m_skills.clear();
            script->m_skillSelect = ScanInt(static_cast<bool*>(0));
            DisjointSkillScript entry;
            for (;;)
            {
                if (!ScanStr(&str))
                    break;
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
                    // [b28d-solo-ScriptDepsA] ORIG 0x088e5198 为 push_backEOi
                    //（右值）：源码把 ScanInt 调用内联进实参（4.1 为右值建临时槽
                    // -0x48 并传址）。
                    script->m_additional[g].m_items.push_back(
                        ScanInt(static_cast<bool*>(0)));
                }
            }
        }
        else if (token == "[additional result const]")
        {
            for (int g = 0; g < 6; ++g)
            {
                script->m_additionalWeight[g] = ScanFloat(static_cast<bool*>(0));
                // [P47/P222] ORIG 0x088e5232 为 `jne <非零>; jp <非零>`（== 语义，
                // 零分支直落）；写 `!=` 时 4.4 反产 `jp; je`（非零直落）⇒ 翻正极性。
                if (script->m_additionalWeight[g] == 0.0f)
                {
                    script->m_additionalRate[g] = 0.0f;
                    script->m_additionalConst[g].m_item = 0;
                    script->m_additionalConst[g].m_count = 0;
                }
                else
                {
                    script->m_additionalRate[g] = ScanFloat(static_cast<bool*>(0));
                    script->m_additionalConst[g].m_item = ScanInt(static_cast<bool*>(0));
                    script->m_additionalConst[g].m_count = ScanInt(static_cast<bool*>(0));
                }
            }
        }
        else if (token == "[avatar disjoint info]")
        {
            // [b28d-solo-ScriptDepsA] ORIG 0x088e50xx 用 eq（`==`+break）而非
            // 短路 `!=`（p167：MISSING eq / EXTRA ne）。
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/avatar disjoint info]")
                    break;
                const int job = getCharacterJob(token.c_str());
                if (job == 0xb)
                    return false;
                for (int grade = 1; grade <= 3; ++grade)
                {
                    for (int slot = 0; slot < 5; ++slot)
                    {
                        // [P0/P171] ORIG 0x088e5397 段：ScanInt 结果入 %edi 直存
                        //（无具名局部）；索引为 [job][grade][slot]（grade 1..3 直接
                        // 使用，基址 +0xd4 = 0x088e53cc `0x4(%ecx,%eax,4)` 实证）。
                        script->m_avatarDisjoint[job][grade][slot] = ScanInt(&ok);
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
                    // [P171] ORIG 0x088e5d05 段（clear 同址式 0x08375d05）：
                    // 索引 = 10*grade + parts + 0x110，grade 1..3 直接用 ⇒ 基址 +0x444。
                    script->m_avatarPartsDisjoint[grade][parts] = ScanInt(&ok);
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
#include "ScriptDepsA/getCompoundAvatarPartsType.inc"
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
#include "ScriptDepsA/getCompoundRareAvatarType.inc"
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
#include "ScriptDepsA/setCompoundAvatarData.inc"
{
    // [b28d-solo-ScriptDepsA 判词] AE（p167 16/16 CLEAR）：修 (1) m_b/m_c/m_d
    // 字段 unsigned 化（CCompoundAvatar.h，ORIG pair converting ctor `C1IRiRj`
    // 实证）；(2) 三处 make_pair→pair 直构（P192）+ item/weight 去 const
    // （`IRKiRj` 是错误变体）；(3) 删 idx 具名槽（P0，每次重读 0xc(%ebp)）、
    // ok 去预清、count 改 int（循环尾 setl）。
    bool ok;

    const int countA = ScanInt(&ok);
    if (!ok)
        return false;
    avatar->m_optsA[static_cast<int>(equip)].reserve(countA);
    for (int i = 0; i < countA; ++i)
    {
        // [b28d-solo-ScriptDepsA] ORIG pair ctor `C1IRiRj` 第一参非 const
        //（IRKiRj 是 const int& 变体，符号不符）⇒ item/weight 去 const。
        int item = ScanInt(&ok);
        if (!ok)
            return false;
        int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_b[static_cast<int>(equip)] += weight;
        // [b28d-solo-ScriptDepsA] P192：ORIG 0x08902d84 直构 pair（converting
        // ctor `C1IRiRj`，第二参 m_b[idx] 为 unsigned 左值），非 make_pair。
        avatar->m_optsA[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item, avatar->m_b[static_cast<int>(equip)]));
    }

    const int countC = ScanInt(&ok);
    if (!ok)
        return false;
    avatar->m_optsC[static_cast<int>(equip)].reserve(countC);
    for (int i = 0; i < countC; ++i)
    {
        int item = ScanInt(&ok);
        if (!ok)
            return false;
        int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_d[static_cast<int>(equip)] += weight;
        avatar->m_optsC[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item, avatar->m_d[static_cast<int>(equip)]));
    }

    while (true)
    {
        int item = ScanInt(&ok);
        if (!ok)
            break;
        int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_c[static_cast<int>(equip)] += weight;
        avatar->m_optsB[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item, avatar->m_c[static_cast<int>(equip)]));
    }
    return true;
}

// ORIG setCompoundRareAvatarData T 0x08902e78（报告全文转写）：开列式稀有选项，
// 权重累计入 m_cube.m_field0c[e]，pair 入 m_cube.m_vecs[e]。type==0x16 → ret0。
// [b28d-48] 已闭合（NDIFF=0 全等）：m_field0c 由 int[10] 改 unsigned int[10]
//（CCompoundAvatar.h；双证据：ORIG 0x08902e78 加法段 `mov 0x18(%eax,%ecx,4),%ecx;
// mov -0xc(%ebp),%eax; add %eax,%ecx` 仅字段 unsigned+RHS int 时 4.4 可复现（P274），
// 且 ORIG push_back 的 pair converting ctor 符号 `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`
// 第二参即 unsigned int&）。sizeof/偏移/mangled 不变。
#include "ScriptDepsA/setCompoundRareAvatarData.inc"
{
    // [b24ab] P48/P0：ORIG 直读 0xc(%ebp)（param_2），无 idx 具名槽；ken禁
    // bool 预清（ScanInt 恒写 ok）。loop 条件式 `(item=ScanInt(&ok)), ok`。
    if (static_cast<int>(equip) == 0x16)
        return false;
    bool ok;
    int item;
    while (true)
    {
        item = ScanInt(&ok);
        if (!ok)
            break;
        const int weight = ScanInt(&ok);
        if (!ok)
            return false;
        // [b28c-lane24] ORIG 0x08902e78 段：写索引(edx)先、读索引(ecx)后；
        // 显式双下标表达式 + 字段 unsigned 化后逐条全等（b28d-48 闭合）。
        avatar->m_cube.m_field0c[static_cast<int>(equip)] =
            avatar->m_cube.m_field0c[static_cast<int>(equip)] + weight;
        // [b24ab] ORIG `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`（converting ctor,
        // int&/unsigned&）+ push_back(EOS1_) ⇒ 直构 pair 非 make_pair（P192）
        avatar->m_cube.m_vecs[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item,
                                avatar->m_cube.m_field0c[static_cast<int>(equip)]));
    }
    return true;
}

// ORIG importCompoundAvatarTableCore T 0x089031d2（报告 ## 汇编 + ## 反编译 C
// 全文转写）。table 实为 11×tagCompoundAvatarTable 数组首址（job*0xc 定位）。
#include "ScriptDepsA/importCompoundAvatarTableCore.inc"
                                           tagCompoundAvatarTable* table)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：808/830，gap=-22，
    // p167 88/88 CLEAR）：本波修 [ancient bind cube] 终止守卫 `==`+break（P133）
    // + 4 处 tail/cube push_back 直构 pair（P192）+ tagCompoundAvatar 改用
    // value-init 临时作 resize 实参（消 C1ERKS 参数副本，ORIG 0x0890334e 形）。
    // 残差=巨件解析树帧/槽平移 + vector/map 模板实例内联差。
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
            break;                                     // P164 配套：EOF 落点移到循环外

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
            // [b28d-solo-ScriptDepsA] ORIG 0x0890334e 段：rep stos 0x308B 前置清零
            // + 就地 C1Ev（value-init 临时）+ 直接作 resize 第二实参（无具名 tmp
            // ⇒ 无 C1ERKS 参数副本）。
            curTable->m_avatars.resize(grade + 1, tagCompoundAvatar());
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
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0xfd4;
                avatar->m_tailA.push_back(std::pair<int, int>(item, weight));
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
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0x1016;
                avatar->m_tailB.push_back(std::pair<int, int>(item, weight));
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
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0x102d;
                avatar->m_tailC.push_back(std::pair<int, int>(item, weight));
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
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0x1044;
                avatar->m_cube.m_vec.push_back(std::pair<int, int>(item, weight));
                if (weight != 1) return 0x1049;
            }
        }
        else if (token == "[ancient bind cube]")
        {
            if (avatar == 0) return 0x104f;
            while (ScanType(token, true))
            {
                if (token == "[/ancient bind cube]")
                    break;
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
    return 0;

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
                            const char* listPath, std::map<int, int>* unused);

// _Z29importCompoundAvatarTableCorePKcS0_P22tagCompoundAvatarTable 0x089031d2 T。
#include "ScriptDepsA/importCompoundAvatarTableCore_1.inc"
                                  tagCompoundAvatarTable* table);

#include "ScriptDepsA/importCompoundAvatarTable.inc"
                              tagCompoundAvatarTable* table)
{
    STScriptFileList list;
    // [b28d-solo-ScriptDepsA 判词] AE（§6.2 全等）：修 (1) loadRDARScriptListFile
    // 返回类型 bool（P292，Main 已同步 STScriptFileList.cpp）；(2) ORIG 0x08903edb
    // `test %al,%al; je <0x107b 岛>`（无 xor）⇒ 正极性包裹（P214①/P257）；
    // (3) begin/end 缓存顺序 it(-0x24)→e(-0x28)（循环体 operator-> 实证）。
    if (loadRDARScriptListFile(&list, dir, path, 0))
    {
        if (list.m_fileMap.size() != 11)
            return 0x106e;

        // ORIG 0x08903f02 先 begin（it 落 -0x24，循环体 operator-> 实证）、
        // 0x08903f17 后 end（e 落 -0x28）；条件 operator!=(it,e)，增量 operator++。
        std::map<int, std::string>::iterator it = list.m_fileMap.begin();
        std::map<int, std::string>::iterator e = list.m_fileMap.end();
        while (it != e)
        {
            if (importCompoundAvatarTableCore(dir, it->second.c_str(), table) != 0)
                return 0x1075;
            ++it;
        }
        return 0;
    }
    return 0x107b;
}


// ============================ 生物公共脚本域 ============================

// ORIG MODULE_NAME_TAG（B 0x0949b2c0 [0x88] = std::string[17][2]，全库唯一一份、
// 外部链接）定义在 CreatureScript.cpp（CreatureScript.h:36 extern 声明），本 TU 只引用。
// [SymActStatus·2026-09-11 评估落地] 原匿名 namespace 的 ModuleTagEntry/
// ModuleTagTableInit/`s_moduleNameTag`（我方发射 9 条符号：`_ZN12_GLOBAL__N_114ModuleTagEntry
// C1/C2/D1/D2`、`…18ModuleTagTableInit C1/C2/D1/D2`、`_ZN12_GLOBAL__N_1L15s_moduleNameTagE`），
// ORIG 全表 0 命中 ⇒ 是本表的第 2 份拷贝，已删除并改引用 ORIG 真表。
// 引用点对齐：ORIG 0x088c2248（getModuleTypeFromTag 内）`add $0x949b2c0,%eax`。
// 内容一致性实证：ORIG static_init 0x088c31b0 / 0x088c3201 逐项构造 0x8e08a19="[dungeon]"
// 与 0x8e08d4b="[/dungeon]"（17 组）——与本 TU 原拷贝内容逐项相同。
// ⚠ 依赖（已在报告登记）：CreatureScript.cpp 侧 MODULE_NAME_TAG 当前无初始化（默认空串），
// 须补 17 组 {"[dungeon]","[/dungeon]"} 才能与本消费点对齐 ORIG 运行期行为（属 OurOnly33 域）。
extern std::string MODULE_NAME_TAG[17][2];

// ORIG getModuleTypeFromTag T 0x088c2233（报告全文转写）：步长 8 遍历 17 组，
// 与 open 标签比较；未命中 -1。
#include "ScriptDepsA/getModuleTypeFromTag.inc"
{
    for (int i = 0; i < 17; ++i)
    {
        if (tag == MODULE_NAME_TAG[i][0])
            return i;
    }
    return -1;
}

// ORIG addCreatureModuleRate T 0x08a38a08（报告全文转写）：已有时乘旧值后覆盖写入。
#include "ScriptDepsA/addCreatureModuleRate.inc"
{
    std::map<ENUM_MODULE_TYPE, float>::iterator it =
        g_creatureModuleDamageRateMap.find(type);
    // b28c-l23：ORIG @0x08a38a4x 命中块为 `mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp);
    // flds 0xc(%ebp); fmuls -0xc(%ebp); fstps 0xc(%ebp)` ⇒ old 经**槽往返**后再乘
    // （N85 附「4.4 float 纯拷贝恒 mov 直搬」的突破口 = 具名 float 局部，
    // 与 P211/P218 同族的"值先物化"开关）；直写 `rate * it->second` 时 4.4 出
    // `flds 0x4(%eax); flds 0xc(%ebp); fmulp`（少 2 条且装载序反）。
    if (it != g_creatureModuleDamageRateMap.end())
    {
        float old = it->second;
        rate = rate * old;
    }
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
#include "ScriptDepsA/importCreatureCommonScript.inc"
                                std::vector<int>* damageTable)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：415/405，gap=10，
    // p167 69/69 CLEAR）：本波补 [module damage percent] 的具名临时 string
    // （allocator+str.c_str()+SsC1EPKcRKSaIcE 三件套，ORIG 0x08a3866x）。
    // 残差=巨件帧/槽平移 + hash_map/vector 模板实例内联差。
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, path))
        return false;

    script->clear();
    damageTable->clear();

    std::string token;
    std::string str;
    int value = 0;
    bool ok = true;

    // [P292/P283] ORIG 各循环头均 `xor $0x1,%eax; test %al,%al; jne/je`（bool
    // 取反形）⇒ 原码统一为 for(;;)+`if (!cond) break;`，非 while(cond)。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
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
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_damage.push_back(value);
                damageTable->push_back(value);
            }
        }
        else if (token == "[mana]")
        {
            script->m_mana.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_mana.push_back(value);
            }
        }
        else if (token == "[module damage percent]")
        {
            g_creatureModuleDamageRateMap.clear();
            for (;;)
            {
                // ORIG 0x08a387f6/0x08a38814 两处独立 `xor $0x1` 守卫 ⇒ 两条独立
                // break，非 `ScanStr && …` 短路合并（P133）。
                if (!ScanStr(&str))
                    break;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                // [b28d-solo-ScriptDepsA] ORIG 0x08a3866x 段：allocator+str.c_str()
                // +string(const char*,const allocator&) 三件套 ⇒ 具名临时 string
                // 由 c_str() 复制构造（P130 家族）。
                std::string moduleName(str.c_str());
                const ENUM_MODULE_TYPE type =
                    static_cast<ENUM_MODULE_TYPE>(getModuleTypeFromTag(moduleName));
                // [P252/P0] ORIG 0x08a38890 段 `fstps -0x10(%ebp); mov -0x10(%ebp),%eax;
                // mov %eax,0x4(%esp)` ⇒ 原码有具名 float 局部（槽往返），非内联实参。
                const float rate = static_cast<float>(value) / 100.0f;
                addCreatureModuleRate(type, rate);
            }
        }
        else if (token == "[assault creature damage penalty]")
        {
            g_creatureIndexDamageRateMap.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                const int idx = value;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                // [P252/P0] ORIG 0x08a38944 段 `fstps -0xc(%ebp)` 先算存槽，
                // operator[] 之后 `mov -0xc(%ebp),%edx; mov %edx,(%eax)` 读回 ⇒
                // 原码具名 float 局部，且先于 map 查找求值。
                const float rate = static_cast<float>(value) / 100.0f;
                g_creatureIndexDamageRateMap[idx] = rate;
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

// ORIG `CATEGORY_STRING` D 0x093f53c0 [0xb0] = `const char*[44]`（.data 初值实证：
// 44 个指针 0x08e1dc01/09/11/…/dda9/ddb0/ddb8/ddbf/ddc6 → .rodata 串，与
// initObjectCategory 的 map 键逐一**同序同名**）；ORIG 全库 0 引用（死数据）
// ⇒ 纯符号轴补发射，零消费点、零 codegen 影响。
// [Main·PoolH 数据符号普查 2026-09-11] 段/大小与 ORIG 对齐（.data / 0xb0）。
const char* CATEGORY_STRING[44] = {
    "[human]", "[beast]", "[plant]", "[insect]", "[machine]", "[organic]",
    "[hybrid]", "[undead]", "[devil]", "[spirit]", "[dragon]", "[angel]",
    "[melee combat]", "[range combat]", "[close-passive]",
    "[close-carefully]", "[full-superarmor]", "[close-tough]",
    "[close-manace]", "[oneside move]", "[human armor]", "[fort armor]",
    "[divine armor]", "[goblin]", "[tau]", "[lugaru]", "[fixture]", "[stay]",
    "[melee]", "[missile]", "[energy]", "[physical]", "[magical]", "[bullet]",
    "[cannon]", "[explosion]", "[inflame]", "[non inflame]", "[light]",
    "[dark]", "[water]", "[fire]", "[fort]", "[unbeatable]"
};

// ORIG initObjectCategory T 0x08a0aed8：map[literal]=枚举 逐条插入，恒返 1。
// 字面量地址逐一解析自 .rodata（0x08e1dc01..0x08e1ddc6）。
#include "ScriptDepsA/initObjectCategory.inc"
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
#include "ScriptDepsA/initObjectScriptBase.inc"
{
    // ORIG `xor $1,%eax; test %al; je`：initObjectCategory 返回 bool，`!` 翻转直测寄存器
    // （P198② 判定内联直测）；return-0 作 fall-through、return-1 作岛。直写透传会丢此形。
    if (!initObjectCategory())
        return 0;
    return 1;
}

// ORIG getObjectCategory T 0x08a3ca1b（报告全文转写）：按值收参；查不到 0x2c。
// b28c-l23 根因（P96/P50 + P220 极性 + P132 operator*）：ORIG 只发射 `map::find` /
// `map::end` / `_Rb_tree_iterator...neEERKS4_` / `...deEv`（**operator\*，非 operator->**）
// 各一次（30 条），find 结果直接构造进 -0x10（无拷贝 ctor）⇒ ①原码用**非 const
// iterator**；②判据是正极 `if (it != end()) return (*it).second;` 让查表走
// fall-through（负极 `==` 形 4.4 恒产 eq + 块序反）；③取值写 `(*it).second`
// 而非 `it->second`（后者发 ptEv）。
#include "ScriptDepsA/getObjectCategory.inc"
{
    std::map<std::string, ENUM_CATEGORY, IgnoreCaseCompare>::iterator it =
        s_objCategoryString.find(name);
    if (it != s_objCategoryString.end())
        return (*it).second;
    return 0x2c;
}

// ============================ 强化脚本域 ============================

// ORIG stInvestAmplifyOption_t C1 W 0x0822aea6（定义 CItemAmplifier.cpp）——仅声明引用。

// ORIG importAmplifyItem T 0x0890a6b0（报告 ## 汇编 + ## 反编译 C 全文转写）：
// 返回错误行号，0=成功。[option mapping table] 建 map<string,ENUM_AMPLIFY_TYPE>；
// [option data] 查表得类型，{type,(short)(sum),(short)value} 入 m_options；
// 三个 hash_map 投资段 key=ScanInt、value={查表类型, 第二个 ScanInt}。
#include "ScriptDepsA/importAmplifyItem.inc"
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：737/766，gap=-29）：
    // 本波修 4 处 const_iterator（P96 反向，MISSING const C1ERK/eq/ptE 全消）
    // + 3 处 invest key 去 const（ORIG make_pair `IRmR...`）+ 3 处 pair 直构。
    // 残差：p167 尚余 EXTRA map<string,ENUM_AMPLIFY_TYPE> C1/D1×3 + ScanInt×1
    // （ORIG 该三处 map 生命周期/读序与 4.4 折叠不同；语义等价，非缺失调用）；
    // 其余为巨件帧/槽平移 + 模板实例内联差。
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
            break;                                     // P164 配套：EOF 落点移到循环外

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
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
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
            // [b28d-solo-ScriptDepsA] ORIG：本段与「未匹配任何段名」的链尾
            // 默认路径**共用同一段代码**（反编译 C 的 do-while 尾部 ScanInt
            // 实证）⇒ 单一 ScanInt 调用点（p167 EXTRA ScanInt×1 的来源）。
            goto parse_equip_level;
        }
        else if (token == "[purify material]")
        {
            const int item = ScanInt(&ok);
            if (!ok) return 0x1d98;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1d9b;
            out.m_purifyMatItem[0] = item;
            out.m_purifyMatCount[0] = count;
        }
        else if (token == "[purify only material]")
        {
            const int item = ScanInt(&ok);
            if (!ok) return 0x1da8;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1dab;
            out.m_purifyMatItem[1] = item;
            out.m_purifyMatCount[1] = count;
        }
        else if (token == "[invest option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;      // ORIG ctor 0x0822aea6
                if (!ScanStr(&str))
                    break;
                // [b28d-solo-ScriptDepsA] P96 反向：ORIG 用 const_iterator
                //（p167 MISSING const C1ERK/eq/ptE，EXTRA 非 const 版）。
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1dbd;
                entry.m_amount = it->second;
                // [b28d-solo-ScriptDepsA] ORIG make_pair 实例为
                // `IRmR23stInvestAmplifyOption_tE`（unsigned long& 非 const）。
                unsigned long key = static_cast<unsigned long>(ScanInt(&ok));
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
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1dd8;
                entry.m_amount = it->second;
                unsigned long key = static_cast<unsigned long>(ScanInt(&ok));
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
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1df4;
                entry.m_amount = it->second;
                unsigned long key = static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dfa;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1dfe;
                entry.m_value = v;
                out.m_investMap[2].insert(std::make_pair(key, entry));
            }
        }
        else
        {
        parse_equip_level:
            // 链尾合并默认路径（与 [equip level const] 同一代码）。
            out.m_field3c = ScanInt(&ok);
            if (!ok)
                return 0x1d92;
        }
    }
    return 0;

}

// ============================ 武器信息 / 角色脚本域 ============================

// ORIG WeaponInfo::clear W 0x08a3cd8e（objdump 转写，无独立报告）。
#include "ScriptDepsA/clear.inc"
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
#include "ScriptDepsA/WeaponInfo.inc"

// ORIG STRDCharacterStatus::Clear W 0x08367642（objdump 全文转写，无独立报告）：
// +0x52 字节与 +0x53 dword 不在 [HP MAX]/[weight] 等导入写入区（仅清零）。
#include "ScriptDepsA/Clear.inc"
{
    m_hpMax = 0;
    m_mpMax = 0;
    m_physAttack = 0;
    m_physDefense = 0;
    m_magAttack = 0;
    m_magDefense = 0;
    int i = 0;
    for (i = 0; i < 4; ++i)   // ORIG 双 init（声明=0 + for 独立 init，同槽 -0x4）
        m_elemResist[i] = 0;
    for (i = 0; i < 17; ++i)
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
    m_field53 = 0.0f;   // b28c-28：float 位模式零 ⇒ ORIG `mov $0,%edx; mov %edx,disp`
}

// ORIG C1 W 0x0836778c：vector C1 + clear()。
#include "ScriptDepsA/CharacterEtcStats.inc"
    : m_category()
{
    clear();
}

// ORIG CharacterEtcStats::clear W 0x08367752（objdump 转写）：vector clear +
// +0xc..+0x18 四个 int 清零；+0x4..+0xb 未触。
#include "ScriptDepsA/clear_1.inc"
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
{
    clear();
}

// ORIG _ComboInfo::clear W 0x0836785c（objdump 转写）。
#include "ScriptDepsA/clear_2.inc"
{
    m_mapA.clear();
    m_mapB.clear();
    m_vec.clear();
}

// ORIG STSecondGrowTypeData::clear W 0x08367a28（objdump 转写）：双状态块 +
// 双技能表 + 双 etc 域。
#include "ScriptDepsA/clear_3.inc"
{
    // [b28d-solo-ScriptDepsA 判词] N4/N18a 封顶（35/36，唯一残差 2 条）：
    // ORIG 首元素地址 `imul $0x57; add 0x8(%ebp),%eax`（基址内存加），4.4 恒
    // `mov 0x8(%ebp),%edx; imul; lea (%edx,%eax,1)`（先载 this 再 lea）。
    // 本波探针复验 6 形（单成员指针算术/数组下标/`*ptr`/reinterpret_cast/
    // 数组引用/具名指针缓存）全部产 lea 或加槽，无源形可产 `add mem` ⇒ 封顶。
    for (int i = 0; i < 2; ++i)
    {
        // ORIG imul $0x57 直接寻址 ⇒ 数组下标形（CCharacter.cpp 同款先例）
        m_statusA[i].Clear();
        m_skills[i].clear();
        m_etc[i].clear();
    }
}

// ORIG STCharacterScript::Clear W 0x08367a98（objdump 0x3a5 字节全文转写）。
#include "ScriptDepsA/Clear_1.inc"
{
    // [round1 判词更新] 旧 N82「帧/槽平移封顶」判词废弃。本波按 ORIG 逐条复刻：
    //  ① 循环变量 i/j 提函数头（判据⑩ 槽序反转律：i 先声明占深槽 -0x10、
    //     j 后声明占浅槽 -0xc，与 ORIG 0x08367abb/0x08367b34 一致）；
    //     `i = 0` 初始化语句 + `for (i = 0; …)` 双清零复现 ORIG 0x08367abb/0x08367ac2
    //     两条 `movl $0x0,-0x10(%ebp)`（P191 配套）。
    //  ② i<=6 循环改连续数组 `m_status[i]`（m_initialStatus +0x24 与
    //     m_growStatus[6] +0x7b 同型连续，stride 0x57）：复现 ORIG 0x08367b40
    //     `imul $0x57; add $0x20; add this; add $4` 统一寻址，删除 i==0 三元分支。
    //  ③ m_attackMotion.clear() 移到 ORIG 真实位置（+0xb44，m_motionBuff 之后、
    //     m_etcMotion 之前）；旧码错放在 i<=6 循环之后。
    //  ④ m_jumpSpeed 留在尾部（ORIG 0x08367e06 段）——保持源码原语句序。
    m_job = static_cast<ENUM_CHARACTERJOB>(0xb);   // ORIG 0x08367ab2 movl $0xb
    m_width = 0;
    m_height = 0;
    int i = 0;
    int j;
    for (i = 0; i <= 5; ++i)
    {
        m_growtypeName[i] = "";
        for (j = 0; j <= 1; ++j)
            m_awakeningName[i][j] = "";
    }
    for (i = 0; i <= 6; ++i)
    {
        m_status[i].Clear();
        m_skillList[i].clear();
        m_etcStats[i].clear();
        m_secondGrow[i].clear();
    }
    m_motionWaiting = "";
    m_motionMove = "";
    m_motionSit = "";
    for (i = 0; i <= 1; ++i)                 // +0xae0（ORIG i<=1 循环）
        m_motionDamage[i] = "";
    m_motionDown = "";
    m_motionOverturn = "";
    m_motionJump = "";
    m_motionJumpattack = "";
    m_motionRest = "";
    for (i = 0; i <= 3; ++i)                 // +0xb04/+0xb14（ORIG i<=3 同循环）
    {
        m_throwMotion1[i] = "";
        m_throwMotion2[i] = "";
    }
    m_motionDash = "";
    m_motionDashattack = "";
    m_motionGetitem = "";
    m_motionBuff = "";
    m_attackMotion.clear();                  // +0xb44（ORIG 0x08367d6b 段）
    m_etcMotion.clear();                     // +0xb50
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
{
    // [b28d-solo-ScriptDepsA·大件波 BigSkillChar] AE 口径 ndiff 2728→**101**：
    //  ① [attack action]/[action list] 收尾具名 `bool endTag` 改纯短路 if——ORIG
    //     4.1 将 endTag 驻 %eax（`mov $1/$0; test %al`），4.4 具名 bool 必落 -0x29
    //     栈槽并把 cur* 指针组压深 4、全 deep 区 +4 平移（≈4300 行 diff 主源）；
    //     真值表等价（A&&B→break；C&&D→break；否则 return false）。
    //  ② [body image path] 的 `s_bodyImagePath[static_cast<ENUM>(m_job)]` rvalue
    //     经 reinterpret 左值引用直绑 &m_job（字段偏移 0）——ORIG `mov %eax,0x4`
    //     直传实证；消 -0x4c 临时槽（第二平移源）。map 键型保持 ENUM_CHARACTERJOB
    //     （ORIG W 实例 _ZNSt3mapI17ENUM_CHARACTERJOBSs...4find/ix 实证）。
    //  ③ [category] 块 `curEtc->m_category.clear()` 移至 t/isType 声明前（ORIG
    //     clear 先于 string ctor 序，消 hunk 错位）。
    // 语义核对：imm/字段访问常量组差集仅剩 `mov (%eax),%eax; mov %eax,%edx` 型
    // 4.1 寄存器复制（×4）与一处 imm:0 物化 ⇒ token 链/字段写/循环界无缺失。
    // 剩余 101 构成：ORIG 函数头 41 条 `jmp <T>` else-if 回跳岛（4.4 jump-
    // threading 消除，N121 封顶）+ 上述寄存器复制 + `test/je`↔`xor/test/jne`
    // 守卫极性（P292/N122）+ EH dtor epilogue 合并差（p167 332/332 CLEAR 已证）。
    if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory, path))
        return false;

    script->Clear();
    script->m_scriptName = path;
    getDirectoryPath(script->m_scriptPath, "%s%s",
                     g_szCharacterScriptBaseDirectory, path);

    // ---- ORIG 函数级槽（0x08a0ce80..0x08a0ced4）：token/str/值暂存/指针游标
    // 全部函数作用域共享；growtype N / awakening 分支为 5 指针直赋
    // （0x08a0d155.. 逐例 `mov 0x8(%ebp),%eax; add $K; mov %eax,-0x48/-0x44/
    // -0x40/-0x3c/-0x38`），不合并、不算下标。 ----
    std::string token;       // ORIG -0xcc
    std::string str;         // ORIG -0xd0（job 名 / blood 串复用）
    int value = 0;           // ORIG -0xd4（ScanInt(int*) 值暂存：width/jump/
                             // skill 第二列/hit int 共用）
    float fvalue = 0.0f;     // ORIG -0xd8（ScanFloat/ScanTypeOrFloat 共用暂存）
    bool ok = false;         // ORIG -0xd9（ScanInt(int*)/ScanTypeOr* 的 got）

    STRDCharacterStatus* curStatus = script->m_status;             // -0x48
    std::vector<std::pair<int, int> >* curSkills = &script->m_skillList[0]; // -0x44
    CharacterEtcStats* curEtc = &script->m_etcStats[0];                    // -0x40
    // ORIG 0x08a0ced7/ce de：curSecond/curAwaken 初值 0；仅 [initial value]/
    // [growtype N] 落位（初值分支不重置 curAwaken）。[awakening 1/2/name] 先
    // 于二者出现属畸形输入（ORIG 空指针解引用），忠实保留。
    STCharacterScript::STSecondGrowTypeData* curSecond = 0;                // -0x3c
    std::string* curAwaken = 0;                                            // -0x38

    for (;;)
    {
        // [bigfix] ORIG 0x08a0cf6e..0x08a0cf78：`call ScanType; xor $0x1,%eax;
        // test %al,%al; jne <exit>`（P164：外层 for(;;)+否定 break 复现 4.1
        // 头部条件块的 xor 物化；while(ScanType) 4.4 恒产 `test;je` 无 xor）。
        if (!ScanType(token, true))
            break;

        if (token == "[job]")
        {
            if (!ScanStr(&str))
                return false;
            // [bigfix-enum] ORIG 0x08a0cfcd `mov 0x8(%ebp),%edx; mov %eax,(%edx)`
            // LHS-first ⇒ 目标字段与 callee 返回同型（enum，P211/P254）。
            script->m_job = static_cast<ENUM_CHARACTERJOB>(getCharacterJob(str.c_str()));
        }
        else if (token == "[growtype name]")
        {
            // ORIG 0x08a0d020 `cmpl $5; setle` ⇒ i <= 5（P2）
            for (int i = 0; i <= 5; ++i)
                if (!ScanStr(&script->m_growtypeName[i]))
                    return false;
        }
        else if (token == "[width]")
        {
            // ORIG ScanInt(int*) + 值槽 -0xd4 中转（非直接 &field）
            if (ScanInt(&value) == false)
                return false;
            script->m_width = value;
            if (ScanInt(&value) == false)
                return false;
            script->m_height = value;
        }
        else if (token == "[jump speed]")
        {
            if (ScanInt(&value) == false)
                return false;
            script->m_jumpSpeed = value;
        }
        else if (token == "[initial value]")
        {
            curStatus = script->m_status;
            curSkills = &script->m_skillList[0];
            curEtc = &script->m_etcStats[0];
            curSecond = &script->m_secondGrow[0];
        }
        else if (token == "[growtype 1]")
        {
            curStatus = &script->m_status[1];
            curSkills = &script->m_skillList[1];
            curEtc = &script->m_etcStats[1];
            curSecond = &script->m_secondGrow[1];
            curAwaken = &script->m_awakeningName[0][0];
        }
        else if (token == "[growtype 2]")
        {
            curStatus = &script->m_status[2];
            curSkills = &script->m_skillList[2];
            curEtc = &script->m_etcStats[2];
            curSecond = &script->m_secondGrow[2];
            curAwaken = &script->m_awakeningName[1][0];
        }
        else if (token == "[growtype 3]")
        {
            curStatus = &script->m_status[3];
            curSkills = &script->m_skillList[3];
            curEtc = &script->m_etcStats[3];
            curSecond = &script->m_secondGrow[3];
            curAwaken = &script->m_awakeningName[2][0];
        }
        else if (token == "[growtype 4]")
        {
            curStatus = &script->m_status[4];
            curSkills = &script->m_skillList[4];
            curEtc = &script->m_etcStats[4];
            curSecond = &script->m_secondGrow[4];
            curAwaken = &script->m_awakeningName[3][0];
        }
        else if (token == "[growtype 5]")
        {
            curStatus = &script->m_status[5];
            curSkills = &script->m_skillList[5];
            curEtc = &script->m_etcStats[5];
            curSecond = &script->m_secondGrow[5];
            curAwaken = &script->m_awakeningName[4][0];
        }
        else if (token == "[growtype 6]")
        {
            curStatus = &script->m_status[6];
            curSkills = &script->m_skillList[6];
            curEtc = &script->m_etcStats[6];
            curSecond = &script->m_secondGrow[6];
            curAwaken = &script->m_awakeningName[5][0];
        }
        else if (token == "[awakening 1]")
        {
            curStatus = curSecond->m_statusA;           // +0x00
            curSkills = &curSecond->m_skills[0];         // +0xb0
            curEtc = &curSecond->m_etc[0];               // +0xc8
        }
        else if (token == "[awakening 2]")
        {
            curStatus = &curSecond->m_statusA[1];           // +0x57
            curSkills = &curSecond->m_skills[1];         // +0xbc
            curEtc = &curSecond->m_etc[1];               // +0xe4
        }
        else if (token == "[awakening name]")
        {
            if (!ScanStr(curAwaken))
                return false;
            if (!ScanStr(curAwaken + 1))
                return false;
        }
        else if (token == "[awakening skill]")
        {
            std::string t;                // ORIG -0xe0
            bool isType = false;          // ORIG -0xe1
            int v;                        // ORIG -0xe8（ScanTypeOrInt 返回值）
            curSkills->clear();
            for (;;)
            {
                v = ScanTypeOrInt(isType, t, ok, true);
                if (ok != true)
                    return false;
                if (isType)
                {
                    if (t == "[/awakening skill]")
                        break;
                    return false;
                }
                if (ScanInt(&value) == false)
                    return false;
                // [b28d-solo-ScriptDepsA] ORIG pair converting ctor `C1IRiS2_`
                //（int&,int&）⇒ 直构（P192）。
                curSkills->push_back(std::pair<int, int>(v, value));
            }
        }
        else if (token == "[HP MAX]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_hpMax = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[MP MAX]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_mpMax = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[physical attack]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_physAttack =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[physical defense]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_physDefense =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[magical attack]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_magAttack =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[magical defense]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_magDefense =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[fire resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[0] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[water resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[1] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[dark resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[2] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[light resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[3] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[inventory limit]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_inventoryLimit = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[HP regen speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_hpRegen =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[MP regen speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_mpRegen =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[move speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_moveSpeed = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[attack speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_attackSpeed =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[cast speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_castSpeed =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[hit recovery]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_hitRecovery =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[jump power]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_jumpPower =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[weight]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_weight = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[skill]")
        {
            std::string t;                // ORIG -0xf0
            bool isType = false;          // ORIG -0xf1
            int v;                        // ORIG -0xf8
            curSkills->clear();
            for (;;)
            {
                v = ScanTypeOrInt(isType, t, ok, true);
                if (ok != true)
                    return false;
                if (isType)
                {
                    if (t == "[/skill]")
                        break;
                    return false;
                }
                if (ScanInt(&value) == false)
                    return false;
                curSkills->push_back(std::pair<int, int>(v, value));
            }
        }
        else if (token == "[category]")
        {
            curEtc->m_category.clear();
            // ORIG 0x08a0dd??：clear（`mov -0x40,%eax; call`）先于 t/isType 的
            // string ctor + movb $0（-0xf8/-0xf9）——ORIG 把 clear 写在声明前。
            std::string t;                // ORIG -0xf8
            bool isType = false;          // ORIG -0xf9（折入 t 槽）
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    // [b28d-solo-ScriptDepsA] P133：ORIG 0x08a0de4f 用否定式
                    // 守卫（`_ZStne`）；p167 MISSING ne / EXTRA eq。
                    if (t != "[/category]")
                        return false;
                    break;
                }
                const int cat = getObjectCategory(t);   // 传值 → 拷贝临时
                if (cat != 0x2c)
                    // ORIG 0x08a0dde0：push_backEOi（右值临时 -0xb4）。
                    curEtc->m_category.push_back(static_cast<int>(cat));
            }
        }
        else if (token == "[module damage rate]")
        {
            // ORIG 0x08a0df61 `cmpl $3; setle` ⇒ i <= 3（P2）
            for (int i = 0; i <= 3; ++i)
            {
                if (!ScanFloat(&fvalue)) return false;
                curEtc->m_moduleDamageRate[i] = fvalue;
            }
        }
        else if (token == "[waiting motion]")   { if (!ScanStr(&script->m_motionWaiting)) return false; }
        else if (token == "[move motion]")      { if (!ScanStr(&script->m_motionMove)) return false; }
        else if (token == "[sit motion]")       { if (!ScanStr(&script->m_motionSit)) return false; }
        else if (token == "[damage motion 1]")  { if (!ScanStr(&script->m_motionDamage[0])) return false; }
        else if (token == "[damage motion 2]")  { if (!ScanStr(&script->m_motionDamage[1])) return false; }
        else if (token == "[down motion]")      { if (!ScanStr(&script->m_motionDown)) return false; }
        else if (token == "[overturn motion]")  { if (!ScanStr(&script->m_motionOverturn)) return false; }
        else if (token == "[jump motion]")      { if (!ScanStr(&script->m_motionJump)) return false; }
        else if (token == "[jumpattack motion]"){ if (!ScanStr(&script->m_motionJumpattack)) return false; }
        else if (token == "[rest motion]")      { if (!ScanStr(&script->m_motionRest)) return false; }
        else if (token == "[throw motion 1-1]") { if (!ScanStr(&script->m_throwMotion1[0])) return false; }
        else if (token == "[throw motion 1-2]") { if (!ScanStr(&script->m_throwMotion2[0])) return false; }
        else if (token == "[throw motion 2-1]") { if (!ScanStr(&script->m_throwMotion1[1])) return false; }
        else if (token == "[throw motion 2-2]") { if (!ScanStr(&script->m_throwMotion2[1])) return false; }
        else if (token == "[throw motion 3-1]") { if (!ScanStr(&script->m_throwMotion1[2])) return false; }
        else if (token == "[throw motion 3-2]") { if (!ScanStr(&script->m_throwMotion2[2])) return false; }
        // ORIG 0x08a0e399/0x08a0e3de：4-1/4-2 在 3-2 之后、dash 之前。
        else if (token == "[throw motion 4-1]") { if (!ScanStr(&script->m_throwMotion1[3])) return false; }
        else if (token == "[throw motion 4-2]") { if (!ScanStr(&script->m_throwMotion2[3])) return false; }
        else if (token == "[dash motion]")      { if (!ScanStr(&script->m_motionDash)) return false; }
        else if (token == "[dashattack motion]"){ if (!ScanStr(&script->m_motionDashattack)) return false; }
        else if (token == "[getitem motion]")   { if (!ScanStr(&script->m_motionGetitem)) return false; }
        else if (token == "[buff motion]")      { if (!ScanStr(&script->m_motionBuff)) return false; }
        else if (token == "[back motion]")      { if (!ScanStr(&script->m_motionBack)) return false; }
        else if (token == "[simple rest motion]")  { if (!ScanStr(&script->m_motionSimpleRest)) return false; }
        else if (token == "[simple move motion]")  { if (!ScanStr(&script->m_motionSimpleMove)) return false; }
        // ORIG 链序：attack motion(0x08a0e598) → ghost motion(0x08a0e69d) →
        // etc motion(0x08a0e7a2)；各块 t/isType 独立槽。
        else if (token == "[attack motion]")
        {
            std::string t;                // ORIG -0x100
            bool isType = false;          // ORIG -0x101
            script->m_attackMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/attack motion]")
                        break;
                    return false;
                }
                script->m_attackMotion.push_back(t);
            }
        }
        else if (token == "[ghost motion]")
        {
            std::string t;                // ORIG -0x108
            bool isType = false;          // ORIG -0x109
            script->m_ghostMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/ghost motion]")
                        break;
                    return false;
                }
                script->m_ghostMotion.push_back(t);
            }
        }
        else if (token == "[etc motion]")
        {
            std::string t;                // ORIG -0x110
            bool isType = false;          // ORIG -0x111
            script->m_etcMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/etc motion]")
                        break;
                    return false;
                }
                script->m_etcMotion.push_back(t);
            }
        }
        else if (token == "[waiting action]")     { if (!ScanStr(&script->m_actionSet.m_waiting)) return false; }
        else if (token == "[move action]")        { if (!ScanStr(&script->m_actionSet.m_move)) return false; }
        else if (token == "[dash action]")        { if (!ScanStr(&script->m_actionSet.m_dash)) return false; }
        else if (token == "[sit action]")         { if (!ScanStr(&script->m_actionSet.m_sit)) return false; }
        else if (token == "[damage action 1]")    { if (!ScanStr(&script->m_actionSet.m_damage[0])) return false; }
        else if (token == "[damage action 2]")    { if (!ScanStr(&script->m_actionSet.m_damage[1])) return false; }
        else if (token == "[down action]")        { if (!ScanStr(&script->m_actionSet.m_down)) return false; }
        else if (token == "[overturn action]")    { if (!ScanStr(&script->m_actionSet.m_overturn)) return false; }
        else if (token == "[jump action]")        { if (!ScanStr(&script->m_actionSet.m_jump)) return false; }
        else if (token == "[jumpattack action]")  { if (!ScanStr(&script->m_actionSet.m_jumpattack)) return false; }
        else if (token == "[attack action]" || token == "[action list]")
        {
            std::string t;                // ORIG -0x11c
            bool isType = false;          // ORIG -0x11d
            script->m_actionSet.m_attackList.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    // ORIG 0x08a0ec0d..：先 (token==[attack action] &&
                    // t==[/attack action])，否则再 (token==[action list] &&
                    // t==[/action list])，皆不中 → 失败。ORIG 具名 endTag 由
                    // 4.1 驻 %eax（mov $1/$0 物化，无栈槽）；4.4 具名 bool 必落
                    // -0x29 并把指针组压深 4 字节（§N82 主源，实测）。等价改写：
                    // 条件直接进 if（短路跳转形，无物化槽），命中 break / 失败
                    // return false，与 “a||b → break” 真值表一致。
                    if (token == "[attack action]" &&
                        t == "[/attack action]")
                        break;
                    if (token == "[action list]" &&
                        t == "[/action list]")
                        break;
                    return false;
                }
                script->m_actionSet.m_attackList.push_back(t);
            }
        }
        else if (token == "[etc action]")
        {
            std::string t;                // ORIG -0x124
            bool isType = false;          // ORIG -0x125
            script->m_actionSet.m_etc.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/etc action]")
                        break;
                    return false;
                }
                script->m_actionSet.m_etc.push_back(t);
            }
        }
        else if (token == "[last action]")      { if (!ScanStr(&script->m_lastAction)) return false; }
        else if (token == "[proc action]")      { if (!ScanStr(&script->m_procAction)) return false; }
        else if (token == "[jumpattack info]")  { if (!ScanStr(&script->m_jumpattackInfo)) return false; }
        else if (token == "[dashattack info]")  { if (!ScanStr(&script->m_dashattackInfo)) return false; }
        else if (token == "[attack info]")
        {
            std::string t;                // ORIG -0x12c
            bool isType = false;          // ORIG -0x12d
            script->m_attackInfo.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/attack info]")
                        break;
                    return false;
                }
                script->m_attackInfo.push_back(t);
            }
        }
        else if (token == "[etc attack info]")
        {
            std::string t;                // ORIG -0x130
            bool isType = false;          // ORIG -0x131
            script->m_etcAttackInfo.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/etc attack info]")
                        break;
                    return false;
                }
                script->m_etcAttackInfo.push_back(t);
            }
        }
        else if (token == "[weapon wav]")
        {
            std::string t;                // ORIG -0x138
            bool isType = false;          // ORIG -0x139
            WeaponInfo info;              // ORIG -0x190（t 后构造、先析构）
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    // ORIG 0x08a0f169 `jne 08a0f194`：push_back(info) 块在
                    // !isType 块之后；负向早返形（P222）实测我方 INSERT +7
                    //（break 重排），回退维持正向形（块序差 N 封顶）。
                    if (t == "[/weapon wav]")
                    {
                        script->m_weaponWav.push_back(info);
                        break;
                    }
                    return false;
                }
                info.m_wavs.push_back(t);
            }
        }
        else if (token == "[weapon hit info]")
        {
            std::string t;                // ORIG -0x140
            bool isType = false;          // ORIG -0x141
            unsigned int idx = 0;
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/weapon hit info]")
                        break;
                    return false;
                }
                if (script->m_weaponWav.size() <= idx)
                {
                    // [b28d-solo-ScriptDepsA] ORIG 0x08a3eae6 段：value-init 临时
                    // 就地构造进 resize 按值形参（无 C1ERKS 参数副本，D1 少一枚）。
                    script->m_weaponWav.resize(idx + 1, WeaponInfo());
                }
                WeaponInfo* w = &script->m_weaponWav[idx];
                if (t == "[blow]") w->m_hitType = 0;
                else if (t == "[cut]") w->m_hitType = 1;
                else if (t == "[burn]") w->m_hitType = 2;
                else if (t == "[etc]") w->m_hitType = 3;
                else return false;
                if (!ScanStr(&str)) return false;
                if (str == "[no blood]") w->m_blood = 0;
                else if (str == "[blood]") w->m_blood = 1;
                else return false;
                if (ScanInt(&value) == false) return false;
                w->m_field14 = value;
                if (!ScanFloat(&fvalue)) return false;
                w->m_rate18 = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_rate1c = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_rate20 = fvalue;
                ++idx;
            }
        }
        else if (token == "[weapon skill info]")
        {
            std::string t;                // ORIG -0x148
            bool isType = false;          // ORIG -0x149
            unsigned int idx = 0;
            while (true)
            {
                fvalue = ScanTypeOrFloat(isType, t, ok, true);
                if (ok != true) return false;
                if (isType)
                {
                    if (t == "[/weapon skill info]") break;
                    return false;
                }
                if (script->m_weaponWav.size() <= idx)
                {
                    script->m_weaponWav.resize(idx + 1, WeaponInfo());
                }
                WeaponInfo* w = &script->m_weaponWav[idx];
                w->m_skillRates[0] = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_skillRates[1] = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_skillRates[2] = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_skillRates[3] = fvalue;
                ++idx;
            }
        }
        else if (token == "[body image path]")
        {
            std::string t;                // ORIG -0x150
            // ORIG `xor $0x1,%eax; test; je` = 否定守卫（!ScanStr→失败）。
            // m_job 为 ENUM_CHARACTERJOB（P254 实证 0x08a0f951 中转），直接作
            // map 键，无需 reinterpret 视图。
            if (!ScanStr(&t))
                return false;
            s_bodyImagePath[script->m_job] = t;
        }
        else if (token == "[upgrade weapon attack power rate]")
        {
            // [bigfix] ORIG 0x08a0f946 `mov -0x18(%ebp),%eax; cmp $0x5,%eax; ja`
            // 两段寄存器形 ⇒ 局部声明 int + 显式无符号比较（P310）。
            int col = 0;                  // ORIG -0x18
            float rate = 0.0f;            // ORIG -0x14（ScanTypeOrFloat 返回）
            float val = 0.0f;             // ORIG -0x154
            std::string t;                // ORIG -0x158
            bool isType = false;          // ORIG -0x159
            while (true)
            {
                rate = ScanTypeOrFloat(isType, t, ok, true);
                if (ok != true) return false;
                if (isType)
                {
                    if (t == "[/upgrade weapon attack power rate]") break;
                    return false;
                }
                if (!ScanFloat(&val)) return false;
                if ((unsigned int)col <= 5)  // ORIG 0x08a0f949 `cmp $5; ja`（无符号）
                {
                    upgradeWeaponAttackPowerRate_[script->m_job][col][0] = rate;
                    upgradeWeaponAttackPowerRate_[script->m_job][col][1] = val;
                }
                col = col + 1;
            }
        }
        else if (token == "[weapon durability decrease rate]")
        {
            // ORIG 0x08a0fa41 `cmpl $5; setle` ⇒ i <= 5
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&fvalue)) return false;
                g_weaponDurabilityDecreaseRate_[script->m_job][i] = fvalue;
            }
        }
        else
        {
            // ORIG 0x08a0fa51..0x08a0fac9：i = 0..0x10（`cmpl $0x10; setle`）
            // 线性比 17 项 ACTIVESTATUS_TAG，命中即 ScanFloat+×10 写
            // curStatus->m_activeResist[i]（+0x18+2i），**无 break** 续扫。
            for (int i = 0; i <= 0x10; ++i)
            {
                if (token == ACTIVESTATUS_TAG[i])
                {
                    if (!ScanFloat(&fvalue)) return false;
                    curStatus->m_activeResist[i] =
                        static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
                }
            }
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
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：1279/1225，gap=54，
    // p167 181/181 CLEAR）：本波修文件迭代器先声明后赋值（P314/P50 复活，
    // 复现 normal_iterator C1Ev）+ [equipment ani script] 终止守卫改 `==`+break
    // （P133）。
    // 附：STEquipmentAniScript::C1ERKS_（433/423）与 D1（332/365）为本 TU 使用点
    // 发射的编译器隐式 W 符号（无显式定义可挂判词）——ORIG 0x0898xxxx 段对
    // 连续 string 成员用 `mov $0x3,%ebx` 倒计数块搬移循环（前 10 个逐个
    // SsC1ERKSs、其余走循环），4.4 恒逐个展开 ⇒ N90 数组成员块搬移分块启发式
    // + N91 EH cleanup 跨版本差，判词随本函数。
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
        // [b28d-solo-ScriptDepsA] ORIG 用 `section == "[/equipment ani script]"`
        // +break（p167 MISSING eq / EXTRA ne）。
        // [P292/P283] ORIG 0x089c06d2 `call; xor $0x1,%eax; test %al,%al; jne`
        // ⇒ for(;;)+`if (!ScanStr(...)) break;`。
        for (;;)
        {
            if (!ScanStr(&name))
                break;
            if (section == "[/equipment ani script]")
                break;
            files.push_back(name);
        }
    }

    // [b28d-solo-ScriptDepsA] P314/P50 复活：ORIG 0x089c0698 段先默认构造
    // iterator（C1Ev）再赋值 begin()。
    std::vector<std::string>::iterator fit;
    for (fit = files.begin(); fit != files.end(); ++fit)
    {
        if (!loadRDARScriptFile("", fit->c_str()))
            return false;

        std::string tag;                 // ORIG -0x370
        std::string val;                 // ORIG -0x374
        STEquipmentAniScript entry;      // ORIG -0x3f4（C1 0x0898ca36）

        // [P292/P283] ORIG 0x089c079c/0x089c07ed `call; xor $0x1,%eax; test;
        // jne/je` ⇒ for(;;)+`if (!ScanType(...)) break;`。
        for (;;)
        {
            if (!ScanType(tag, true))
                break;
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
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[LAYER]", 0x3142));
                    }
                    else
                    {
                        entry.m_field0 = ScanInt(static_cast<bool*>(0));
                        break;                     // 0x089c0bd9
                    }
                    /* fall through（0x089c0bc5 → 0x089c0bde） */
                case 0x3145:                       // [waiting motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[waiting motion]", 0x3145));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str04 = val;
                        break;
                    }
                    /* fall through（→ 0x089c0c69） */
                case 0x3149:                       // [move motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[move motion]", 0x3149));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str08 = val;
                        break;
                    }
                    /* fall through */
                case 0x314d:                       // [sit motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[sit motion]", 0x314d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str0c = val;
                        break;
                    }
                    /* fall through */
                case 0x3151:                       // [damage motion 1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[damage motion 1]", 0x3151));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr10[0] = val;
                        break;
                    }
                    /* fall through */
                case 0x3155:                       // [damage motion 2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[damage motion 2]", 0x3155));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr10[1] = val;
                        break;
                    }
                    /* fall through */
                case 0x3159:                       // [down motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[down motion]", 0x3159));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str18 = val;
                        break;
                    }
                    /* fall through */
                case 0x315d:                       // [overturn motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[overturn motion]", 0x315d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str1c = val;
                        break;
                    }
                    /* fall through */
                case 0x3161:                       // [jump motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[jump motion]", 0x3161));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str20 = val;
                        break;
                    }
                    /* fall through */
                case 0x3165:                       // [jumpattack motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[jumpattack motion]", 0x3165));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str24 = val;
                        break;
                    }
                    /* fall through */
                case 0x3169:                       // [rest motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[rest motion]", 0x3169));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str28 = val;
                        break;
                    }
                    /* fall through */
                case 0x316d:                       // [throw motion 1-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 1-1]", 0x316d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[0] = val;
                        break;
                    }
                    /* fall through（0x089c11a7 → 0x089c11d7） */
                case 0x3171:                       // [throw motion 1-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 1-2]", 0x3171));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[0] = val;
                        break;
                    }
                    /* fall through（0x089c1232 → 0x089c1262） */
                case 0x3175:                       // [throw motion 2-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 2-1]", 0x3175));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[1] = val;
                        break;
                    }
                    /* fall through */
                case 0x3179:                       // [throw motion 2-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 2-2]", 0x3179));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[1] = val;
                        break;
                    }
                    /* fall through */
                case 0x317d:                       // [throw motion 3-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 3-1]", 0x317d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[2] = val;
                        break;
                    }
                    /* fall through */
                case 0x3181:                       // [throw motion 3-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 3-2]", 0x3181));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[2] = val;
                        break;
                    }
                    /* fall through */
                case 0x3185:                       // [throw motion 4-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 4-1]", 0x3185));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[3] = val;
                        break;
                    }
                    /* fall through */
                case 0x3189:                       // [throw motion 4-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 4-2]", 0x3189));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[3] = val;
                        break;
                    }
                    /* fall through */
                case 0x318d:                       // [dash motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[dash motion]", 0x318d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str4c = val;
                        break;
                    }
                    /* fall through */
                case 0x3191:                       // [dashattack motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[dashattack motion]", 0x3191));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str50 = val;
                        break;
                    }
                    /* fall through */
                case 0x3195:                       // [getitem motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[getitem motion]", 0x3195));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str54 = val;
                        break;
                    }
                    /* fall through */
                case 0x3199:                       // [buff motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[buff motion]", 0x3199));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str58 = val;
                        break;
                    }
                    /* fall through */
                case 0x319d:                       // [back motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[back motion]", 0x319d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str5c = val;
                        break;
                    }
                    /* fall through（0x089c182b → 0x089c185b） */
                case 0x31a1:                       // [attack motion]（多行）
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[attack motion]", 0x31a1));
                    }
                    else
                    {
                        while (ScanStr(&val))
                            entry.m_vec64.push_back(std::string(val.c_str()));
                        break;
                    }
                    /* fall through（0x089c18ba → 0x089c1980） */
                case 0x31a7:                       // [etc motion]（多行）
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[etc motion]", 0x31a7));
                    }
                    else
                    {
                        while (ScanStr(&val))
                            entry.m_vec70.push_back(std::string(val.c_str()));
                        break;
                    }
                    /* fall through（注册链尾 → 公共尾 0x089c1aa0） */
                default:
                    break;                     // 未命中（id=-1）静默
                }

                // [P292] ORIG 0x089c1ab0 `movzbl __inited; xor $0x1,%eax;
                // test %al,%al; je <跳出>` ⇒ 原码是 `if (!__inited) { …; continue; }`
                // 后接裸 break，非 `if (__inited) break;`（后者无 xor 物化）。
                if (!__inited)
                {
                    __init_lock.leave();       // 0x089c1ab5
                    __inited = true;           // 0x089c1ac1
                    continue;                  // → guard 重查（0x089c07f8）
                }
                break;                         // 解析完成 → 下一 ScanType
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
#include "ScriptDepsA/getUpgradeWeaponAttackPowerRate.inc"
                                      unsigned int level)
{
    if (type > 5 || level > 1)
        return 0.0f;
    return upgradeWeaponAttackPowerRate_[static_cast<int>(job)][type][level];
}
