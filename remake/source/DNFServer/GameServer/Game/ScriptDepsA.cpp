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

#include "ScriptDepsA/ScanTypeOrFloat.inc"
#include "ScriptDepsA/getCharacterJob.inc"
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
extern float g_weaponDurabilityDecreaseRate_[11][6];   // [SplitC batch5] 定义已迁出至 RDARScriptObject.cpp

// ORIG B 0x094ff12c / 0x094ff144 [0x18]（生物模块伤害率表）。
std::map<ENUM_MODULE_TYPE, float> g_creatureModuleDamageRateMap;
std::map<int, float> g_creatureIndexDamageRateMap;

// ============================ 小工具 ============================

#include "ScriptDepsA/GetRoundInt.inc"


#include "ScriptDepsA/getElement.inc"
    // _Z10getElementPKc T 0x088bfe5b：strcmp 阶梯，未命中 4。唯一定义在
    // CreatureScript.cpp（声明见 CreatureScript.h），本 TU 仅调用。

#include "ScriptDepsA/getItemRarity.inc"


// ============================ importActiveStatusTable ============================

#include "ScriptDepsA/importActiveStatusTable.inc"


// ============================ importDisjointScript ============================

// [SplitC 2026-09-11 batch4 · TU 迁移] importDisjointScript 已迁出至 RDARScriptDisjoint.cpp（ORIG GI 键对齐）。



// ============================ 头像合成域 ============================

#include "ScriptDepsA/getCompoundAvatarPartsType.inc"


#include "ScriptDepsA/getCompoundRareAvatarType.inc"


#include "ScriptDepsA/setCompoundAvatarData.inc"


#include "ScriptDepsA/setCompoundRareAvatarData.inc"


#include "ScriptDepsA/importCompoundAvatarTableCore.inc"


// ORIG importCompoundAvatarTable T 0x08903ea7（报告全文转写）：
// loadRDARScriptListFile 取 11 个子文件名（≠11 → 0x106e），逐个 Core 导入
// （失败 → 0x1075），全部成功 ret0。
// ============================ 头像合成域（声明修正） ============================

#include "ScriptDepsA/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);

#include "ScriptDepsA/importCompoundAvatarTableCore_1.inc"
                                  tagCompoundAvatarTable* table);

#include "ScriptDepsA/importCompoundAvatarTable.inc"



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

#include "ScriptDepsA/getModuleTypeFromTag.inc"


#include "ScriptDepsA/addCreatureModuleRate.inc"


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

#include "ScriptDepsA/importCreatureCommonScript.inc"


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

#include "ScriptDepsA/initObjectCategory.inc"


#include "ScriptDepsA/initObjectScriptBase.inc"


#include "ScriptDepsA/getObjectCategory.inc"


// ============================ 强化脚本域 ============================

// ORIG stInvestAmplifyOption_t C1 W 0x0822aea6（定义 CItemAmplifier.cpp）——仅声明引用。

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
            // ★ ORIG 0x0890ac19 实证：本段**内联**自己的 ScanInt/0x1d92 尾巴，
            //   成功后 `je 0x890a70e`（= 0x890a712 循环头），与「未匹配任何段名」
            //   的链尾**不是**同一段代码。旧转写把二者合并成 `goto parse_equip_level`
            //   ⇒ 未识别标签也会吞一个 ScanInt 并在 !ok 时 return 0x1d92，
            //   实测 Etc/AmplifyItem.etc：ORIG ret=0，我方 ret=7570(0x1d92)。
            //   代码形状照 ORIG：先存 +0x3c，再判 ok。
            out.m_field3c = ScanInt(&ok);
            if (!ok)
            {
                return 0x1d92;
            }
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
            // ORIG 0x0890b0c7 `je 0x890a711`：最后一个标签比较失败后直接落到
            // 0x890a712 = `while (ScanType(line,true))` 循环头 ⇒ **未识别标签
            // 被静默忽略**（不 ScanInt、不报错）。此处不得再补任何扫描/返回。
        }
    }
    return 0;

}

// ============================ 武器信息 / 角色脚本域 ============================

#include "ScriptDepsA/clear.inc"


#include "ScriptDepsA/WeaponInfo.inc"

#include "ScriptDepsA/Clear.inc"


#include "ScriptDepsA/CharacterEtcStats.inc"


#include "ScriptDepsA/clear_1.inc"


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

#include "ScriptDepsA/_ComboInfo.inc"


#include "ScriptDepsA/clear_2.inc"


#include "ScriptDepsA/clear_3.inc"


#include "ScriptDepsA/Clear_1.inc"


// ============================ ImportCharacterScript ============================

// ORIG static d 0x094ff618 [0x18]：job → 身体贴图路径（原文件级静态；本 TU
// 以文件级静态等价建模）。ORIG 实例化实证 map<ENUM_CHARACTERJOB,Ss>（W
// _ZNSt3mapI17ENUM_CHARACTERJOBSs...4find/ix @08a4101c/08a3ebec）。
// [body image path] 块 ORIG `mov 0x8(%ebp),%eax; mov %eax,0x4(%esp)` =
// &m_job（字段偏移 0，地址即 script 值）直绑 const ENUM& 形参，**无临时槽**；
// m_job 已按 ORIG 证据具型化为 ENUM_CHARACTERJOB（见下 [bigfix-enum]），
// 键直接取 script->m_job，无 reinterpret 视图。
static std::map<ENUM_CHARACTERJOB, std::string> s_bodyImagePath;

#include "ScriptDepsA/ImportCharacterScript.inc"


// ============================ ImportEquipmentAniScript ============================

// 标签分派表类型：ORIG 函数级静态 __lookup = map<char const*, int, less_TCHAR>
// （_ZZ24ImportEquipmentAniScript...E8__lookup B 0x094fddd8）。
typedef std::map<const char*, int, less_TCHAR> AniLookupMap;

#include "ScriptDepsA/ImportEquipmentAniScript.inc"



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

#include "ScriptDepsA/getUpgradeWeaponAttackPowerRate.inc"

