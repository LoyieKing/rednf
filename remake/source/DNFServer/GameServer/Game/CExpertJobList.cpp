// ============================================================================
// df_game_r 还原 —— CExpertJobList（专家职业脚本列表，G5 批次）。
// ---- b28d-61 判词汇总（本 TU 6 件非 identical）----
// 已修 AE：get（P222 正向 if 翻块序）、STExpertJobScript::Clear（ORIG 0x0896dfec
//   调用序重排：0x248 在 0x230 前、0x18/0x30 等落标量清零之后，120/120 对齐）。
// N 封顶（P167 缺口=STL 迭代器/字符串符号，N89）：
//   destroy 0x0822b5f2（MISSING Rb_tree_iterator 符号）、
//   get_disjoint_additional_items 0x085d3cxx（MISSING Rb_tree_iterator）、
//   init 0x0822b3xx（MISSING iterator C1Ev+EXTRA）、
//   ImportExpertJobEtcScript 0x0897xxxx（MISSING string C1/D1）、
//   ImportExpertJobScript 0x089xxxxx（MISSING make_pair+EXTRA 24，N89 大函数）。
// 语义依据 docs/class_func_reports/CExpertJobList/{init,destroy}.md 反汇编。
// init（0x896d8ec）：遍历 g_expertJobScriptFileList，逐条 new STExpertJobScript
//   + ImportExpertJobScript，成功则 FindIndexByFullScan 取 job key 插入 m_jobMap；
//   失败 trace + delete + 返回 false。最后 ImportExpertJobEtcScript(m_etc,"expertjob.etc")。
// destroy（0x896dac6）：遍历 m_jobMap delete 各脚本。
//
// ImportExpertJobScript（0x896b0e4）/ ImportExpertJobEtcScript（0x896ae21）
// 按 docs/class_func_reports/_lt_global_gt_/{ImportExpertJobScript,
// ImportExpertJobEtcScript}.md 反编译忠实转写：函数内访问均形如
// `param_1 + local_58*0x290 + X` 且 local_58 恒为 0，故有效偏移即 X，
// 与 STExpertJobScript.h 权威布局一一对应。
// ============================================================================

#include "CExpertJobList.h"

#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanStr / ScanFloat
#include "LogManager.h"      // LogManager::logFormat
#include <stdio.h>
#include "NpcScript.h"

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_expertJobScriptFileList 0x094f7760 = 0x18 STScriptFileList
// 对象 / g_expertJobScriptBaseDirectory 0x094f7660[0x100]）。
// 由 initExpertJobScript（InitScriptProducers_5.cpp）填充；init() 遍历使用。
STScriptFileList g_expertJobScriptFileList;
char g_expertJobScriptBaseDirectory[0x100];

// [potion info] 段读取并写出的炼金术值表。
// ORIG 为单一全局 g_alchemistInfo（B 0x094f7778，nm 尺寸 8）；[0] 即低半区
// g_alchemistInfo，[1] 即未命名全局 _DAT_094f777c（见 ImportExpertJobScript.md
// [potion info] 反编译）。本 TU 为唯一定义/写点。
int g_alchemistInfo[2];

// ============================================================================
// expert_job::get_disjoint_additional_items（ORIG 0x896d75d）。
// 在 multimap 中按 key 取 equal_range，将每个 mapped stItemSelectRate 压入 out；
// 未找到返回 false，否则返回 true。供 [disjoint result] 校验/填充附加/大奖物品。
// b28d-solo-CExpertJobList 判词（AE）：P222 正向包裹（命中块 fall-through、false 归
// 共享尾 0x0896d829）+ P13 缓存 upper_bound（ORIG 0x0896d7e2 循环初始化区一次求值、
// 条件只比较）；64=64。
// ============================================================================
namespace expert_job
{
bool get_disjoint_additional_items(
    std::multimap<int, stItemSelectRate>& m, int key,
    std::vector<stItemSelectRate>& out)
{
    // b28d-solo [P222 正向包裹；P13：ORIG 循环初始化区一次性求值 lower/upper_bound
    // （-0x14/-0x18 两具名迭代器），条件仅比较不重复调用]。
    std::multimap<int, stItemSelectRate>::iterator found = m.find(key);
    if (found != m.end())
    {
        for (std::multimap<int, stItemSelectRate>::iterator it = m.lower_bound(key),
                 e = m.upper_bound(key); it != e; ++it)
        {
            out.push_back(it->second);
        }
        return true;
    }
    return false;
}
}

// ORIG C1/C2 @0x0896e5cc：全部成员容器构造后尾调 Clear()（与 ORIG 构造序一致）。
STExpertJobScript::STExpertJobScript()
{
    Clear();
}

// ============================================================================
// STExpertJobScript::Clear —— 清空全部容器并清零标量。
// 由 ImportExpertJobScript 在加载前调用（脚本为 new 分配，容器已默认构造，
// 此处保证重复加载语义一致）。
// ============================================================================
void STExpertJobScript::Clear()
{
    // b28d-61 [按 ORIG 0x0896dfec 调用序重排，120/120 逐条对齐]：
    // ORIG 顺序非声明序（0x248 在 0x230 前、0x18/0x30 等在标量清零之后）。
    m_expVec.clear();
    m_growSkillVec.clear();
    m_disjointResultMap.clear();     // 0x1bc
    m_baseConst = 0;                 // 0x1d4
    m_additionalConst = 0;           // 0x1d8
    m_clearQuests.clear();           // 0x1dc
    m_giveupCost.clear();            // 0x1e8
    m_compoundRateVec.clear();       // 0x1f4
    m_recipeInfoMap.clear();         // 0x200
    m_recipeIndexMap.clear();        // 0x218
    m_recipeItemMap.clear();         // 0x248
    m_recipeLevelMap.clear();        // 0x230
    m_enduranceRepairCost.clear();   // 0x260
    m_enduranceReduce1 = 0;          // 0x26c
    m_enduranceReduce2 = 0;          // 0x270
    m_enduranceInit = 0;             // 0x274
    m_upgradeCost.clear();           // 0x278
    m_gainExp1 = 0;                  // 0x284
    m_gainExp2 = 0;                  // 0x288
    m_limitDisjointGold = 0;         // 0x28c
    m_portableDisjointMap.clear();
    m_selfDisjoin.clear();   // ORIG：stSelfDisjoinMaterialInfo::clear @0x0896dbe6
    m_shopDollMap.clear();
    m_unsealScrollMap.clear();
    m_monsterCardBindRateMap.clear();
    m_commonBindInfo.clear();    // ORIG：this+0x84 BindInfo clear
    m_specialBindInfo.clear();   // ORIG：this+0xc4 BindInfo clear
    m_alchemistExtractInfo.clear();
    m_enchanterExtractInfo.clear();
    m_dollExtractInfo.clear();
    m_alchemistResultInfo.clear();
    m_enchanterResultInfo.clear();
    m_dollResultInfo.clear();
}

// ============================================================================
// STExpertJobEtcScript::Clear —— 清空 etc 脚本容器。
// ============================================================================
void STExpertJobEtcScript::Clear()
{
    m_levelMap.clear();
    m_disjointerBigWinRate.clear();
}
// ORIG C1 0x0849f438：构造两个 map 后调 Clear()。
STExpertJobEtcScript::STExpertJobEtcScript()
{
    Clear();
}

// ============================================================================
// ImportExpertJobScript（ORIG 0x896b0e4）。
// 加载指定专家职业脚本，逐节填充 STExpertJobScript 各容器/标量。
//
// b28d-solo-CExpertJobList 判词（N 封顶；§6.2 三件套）：
//   语义完成：p167 调用点差集 CLEAR（ORIG 239 = 本 TU 239，0 missing / 0 extra）；
//   真语义 bug 修 3 族：[items] 三处 make_pair 实参取错字段（ORIG 0x0896bf39/
//   0x0896bfaa/0x0896c01b 实证 (m_field0,idx)/(m_field0,m_field8)/(m_field4,m_field0)
//   + stRecipeInfo +0/+4 字段化）、[expertness exp] 去 string 临时+operator=（改直写
//   e.m_name）、load 判定改 bool 早返形。
//   byte 层残差（ndiff 702 / gap 38 @ ORIG 2194 条）：
//   ① ORIG 0x0896b16b..0x0896b188 的 8 个 `nop;jmp` 循环头中继岛（4.1 if-else 链
//      占位产物，16 条；P223 族多源同标签尾岛）；
//   ② 块内局部槽基差 4（string/multimap 组 -0x224/-0x244/-0x25c vs ORIG
//      -0x228/-0x248/-0x260；int 组 -0x48/-0x44 vs ORIG -0x54/-0x50）——4.4 与 4.1
//      块内槽分配策略差（N82/P212 族；声明序已逐条照 ORIG）；
//   ③ [potion info] 的 `||` 链：4.1 发数值二分比较树、4.4 发线性 cmpl 链（14 条）；
//   ④ 索引化地址计算书写序（imul/add）与 EH 状态机编码差。
// ============================================================================
bool ImportExpertJobScript(STExpertJobScript* script, const char* path)
{
    // b28d-solo [P16/P292：ORIG 0x0896b102 `xor $1; test %al,%al; je` bool 形早返；局部集合照抄
    // ORIG：-0x54 scriptIdx（恒 0，成员访问索引化）/ -0x50 死 int / -0x22c 死 string /
    // -0x22d ok 预清]。
    if (!loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
        return false;
    {
        int scriptIdx = 0;
        (*script).Clear();
        std::string section;
        std::string unusedSection;   // ORIG -0x22c（构造/析构但未使用，P171 照抄）
        int unusedCount = 0;         // ORIG -0x50（死 int，P171 照抄）
        bool ok = false;             // ORIG -0x22d（预清）
        std::multimap<int, stItemSelectRate> additional;  // [additional result]
        std::multimap<int, stItemSelectRate> bigwin;      // [big win result]

        while (ScanType(section, true))
        {
            if (section == "[expertness exp]")
            {
                while (true)
                {
                    // b28d-solo [ORIG 0x0896b1df：先构造 ExpertJobExp，三个字段直写成员
                    // （ScanStr 直接写 e.m_name，无 string 临时+operator=）]。
                    ExpertJobExp e;
                    e.m_exp = ScanInt(&ok);
                    if (!ok) break;
                    e.m_level = ScanInt(&ok);
                    if (!ok) return false;
                    ScanStr(&e.m_name);
                    script[scriptIdx].m_expVec.push_back(e);
                }
            }
            else if (section == "[skill]")
            {
                script[scriptIdx].m_growSkillVec.clear();
                std::pair<int, int> p;
                for (;;)
                {
                    p.first = ScanInt(&ok);
                    if (!ok) break;
                    p.second = ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_growSkillVec.push_back(p);
                }
            }
            else if (section == "[additional result]")
            {
                additional.clear();
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stItemSelectRate rate;
                    rate.m_rateMin   = ScanInt(&ok); if (!ok) return false;
                    rate.m_rateMax   = ScanInt(&ok); if (!ok) return false;
                    rate.m_itemIndex = ScanInt(&ok); if (!ok) return false;
                    rate.m_rate      = ScanInt(&ok); if (!ok) return false;
                    rate.m_float8    = ScanFloat(&ok); if (!ok) return false;
                    additional.insert(std::make_pair(key, rate));
                }
            }
            else if (section == "[big win result]")
            {
                bigwin.clear();
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stItemSelectRate rate;
                    rate.m_rateMin   = ScanInt(&ok); if (!ok) return false;
                    rate.m_rateMax   = ScanInt(&ok); if (!ok) return false;
                    rate.m_itemIndex = ScanInt(&ok); if (!ok) return false;
                    rate.m_rate      = ScanInt(&ok); if (!ok) return false;
                    rate.m_float8    = ScanFloat(&ok); if (!ok) return false;
                    bigwin.insert(std::make_pair(key, rate));
                }
            }
            else if (section == "[base const]")
            {
                script[scriptIdx].m_baseConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[additional const]")
            {
                script[scriptIdx].m_additionalConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[disjoint result]")
            {
                script[scriptIdx].m_disjointResultMap.clear();
                while (true)
                {
                    DisjointMachineGrade grade;
                    grade.m_grade = (unsigned char)ScanInt(&ok);
                    if (!ok) break;
                    grade.m_field1 = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    grade.m_field2 = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    stDisjointResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = ScanFloat(&ok);
                    if (!ok) return false;
                    int addKey = ScanInt(&ok);
                    if (!ok) return false;
                    if (addKey != 0 &&
                        !expert_job::get_disjoint_additional_items(
                            additional, addKey, res.m_additionalItems))
                        return false;
                    int bigKey = ScanInt(&ok);
                    if (!ok) return false;
                    if (bigKey != 0 &&
                        !expert_job::get_disjoint_additional_items(
                            bigwin, bigKey, res.m_bigwinItems))
                        return false;
                    ScanInt(&ok);   // 读取但未建模持久化（local_38c）
                    if (!ok) return false;
                    script[scriptIdx].m_disjointResultMap.insert(std::make_pair(grade, res));
                }
            }
            else if (section == "[clear quests]")
            {
                script[scriptIdx].m_clearQuests.clear();
                int v;
                for (;;)
                {
                    v = ScanInt(&ok);
                    if (!ok) break;
                    script[scriptIdx].m_clearQuests.push_back(v);
                }
            }
            else if (section == "[giveup cost]")
            {
                script[scriptIdx].m_giveupCost.clear();
                int v;
                for (;;)
                {
                    v = ScanInt(&ok);
                    if (!ok) break;
                    script[scriptIdx].m_giveupCost.push_back(v);
                    if (script[scriptIdx].m_giveupCost.size() > 9)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                }
            }
            else if (section == "[compound rate]")
            {
                script[scriptIdx].m_compoundRateVec.clear();
                stCompoundRate r;
                for (;;)
                {
                    r.m_level = ScanInt(&ok);
                    if (!ok) break;
                    r.m_successRate = ScanInt(&ok); if (!ok) return false;
                    r.m_expMin      = ScanInt(&ok); if (!ok) return false;
                    r.m_expMax      = ScanInt(&ok); if (!ok) return false;
                    script[scriptIdx].m_compoundRateVec.push_back(r);
                }
            }
            else if (section == "[items]")
            {
                script[scriptIdx].m_recipeInfoMap.clear();
                script[scriptIdx].m_recipeIndexMap.clear();
                script[scriptIdx].m_recipeLevelMap.clear();
                script[scriptIdx].m_recipeItemMap.clear();
                int idx;
                for (;;)
                {
                    idx = ScanInt(&ok);
                    if (!ok) break;
                    // b28d-solo [ORIG 0x0896beae..0x0896c01b：三个字段直写 info(+0/+4/+8)，
                    // 三处 make_pair 实参 = (m_field0, idx)/(m_field0, m_field8)/
                    // (m_field4, m_field0)（p167 MISSING IRmRiE/IRmRtE/IRmS0_E 实证）]。
                    stRecipeInfo info;
                    info.m_field0 = ScanInt(&ok);
                    if (!ok) return false;
                    info.m_field4 = ScanInt(&ok);
                    if (!ok) return false;
                    info.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (0xef < idx)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                    script[scriptIdx].m_recipeInfoMap.insert(std::make_pair(idx, info));
                    script[scriptIdx].m_recipeIndexMap.insert(
                        std::make_pair(info.m_field0, idx));
                    script[scriptIdx].m_recipeLevelMap.insert(
                        std::make_pair(info.m_field0, info.m_field8));
                    script[scriptIdx].m_recipeItemMap.insert(
                        std::make_pair(info.m_field4, info.m_field0));
                }
            }
            else if (section == "[endurance repair cost]")
            {
                script[scriptIdx].m_enduranceRepairCost.clear();
                stEnduranceRepairCost r;
                for (;;)
                {
                    r.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    r.m_field4 = ScanInt(&ok); if (!ok) return false;
                    script[scriptIdx].m_enduranceRepairCost.push_back(r);
                }
            }
            else if (section == "[endurance reduce]")
            {
                script[scriptIdx].m_enduranceReduce1 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_enduranceReduce2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[endurance initial value]")
            {
                script[scriptIdx].m_enduranceInit = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[upgrade cost]")
            {
                script[scriptIdx].m_upgradeCost.clear();
                int idx;
                for (;;)
                {
                    idx = ScanInt(&ok);
                    if (!ok) break;
                    if (idx - 1 != (int)script[scriptIdx].m_upgradeCost.size())
                        return false;
                    int v = ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_upgradeCost.push_back(v);
                }
            }
            else if (section == "[potion info]")
            {
                // 读取 (itemIndex, value) 对，按 itemIndex 写入两个全局炼金术值；
                // 非法 itemIndex 返回 false。
                // b290-BigExpert [REAL-BUG 修正 + P329/switch 形态]：ORIG
                // 0x0896c35d..0x0896c398 的 switch 判定树实证 0x27ac52 落
                // `mov %eax,0x94f777c`（= g_alchemistInfo[1]），只有 0x27ac59 落
                // `mov %eax,0x94f7778`（=[0]，0x0896c3a4 独此一路）——原码把
                // 0x27ac52 误并入 [0] 组 ⇒ 该药水索引写错全局。改为 switch 形，
                // 同时复现 ORIG「条件一次取入 %eax + 二分判定树」（4.1 对 || 链
                // 发数值二分树、4.4 对 if/else-if 发线性 cmpl 链 ⇒ 24 行 struct 差）。
                for (;;)
                {
                    int idx = ScanInt(&ok);
                    if (!ok) break;
                    int val = ScanInt(&ok);
                    if (!ok) return false;
                    switch (idx)
                    {
                    case 0x27ac59:
                        g_alchemistInfo[0] = val;
                        break;
                    case 0x27ac52:
                    case 0x1f8e:
                    case 0x98981a:
                    case 0x98988d:
                    case 0x989885:
                    case 0x2920913e:
                        g_alchemistInfo[1] = val;
                        break;
                    default:
                        return false;
                    }
                }
            }
            else if (section == "[gain exp]")
            {
                script[scriptIdx].m_gainExp1 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_gainExp2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[limit disjoint gold]")
            {
                script[scriptIdx].m_limitDisjointGold = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[portable disjoint info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_portableDisjointMap.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x211,
                            "import expertjob script - [portable disjoint info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[disjoint self service]")
            {
                script[scriptIdx].m_selfDisjoin.m_field0 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_selfDisjoin.m_field4 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_selfDisjoin.m_field8 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[shop doll info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned char val = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_shopDollMap.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x236,
                            "import expertjob script - [shop doll info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[unseal scroll level limit info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_unsealScrollMap.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x24b,
                            "import expertjob script - [unseal scroll level limit info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[monstercard bind info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stMonsterCardBindRate rate;
                    rate.m_field0 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    rate.m_field2 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_monsterCardBindRateMap.insert(
                            std::make_pair(key, rate)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x266,
                            "import expertjob script - [monstercard bind info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[monstercard bind only]")
            {
                int cnt[4] = { 0, 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    int idx = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_specialBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 4; ++i)
                    script[scriptIdx].m_specialBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[monstercard bind list]")
            {
                int cnt[4] = { 0, 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    int idx = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_commonBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 4; ++i)
                    script[scriptIdx].m_commonBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[alchemist extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_alchemistExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2b1,
                            "import expertjob script - [alchemist extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[enchanter extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_enchanterExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2c4,
                            "import expertjob script - [enchanter extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[doll_controller extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_dollExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2d7,
                            "import expertjob script - [doll_controller extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[alchemist extraction result item]")
            {
                int cnt[3] = { 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    std::string type;
                    if (!ScanStr(&type))
                        return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    int idx;
                    if (type == "food")
                        idx = 0;
                    else if (type == "life")
                        idx = 1;
                    else if (type == "jewel")
                        idx = 2;
                    else
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2f9,
                            "import expertjob script - [alchemist extraction result item] result_type is invalid");
                        return false;
                    }
                    script[scriptIdx].m_alchemistResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 3; ++i)
                    script[scriptIdx].m_alchemistResultInfo.m_count[i] = cnt[i];
            }
            else if (section == "[enchanter extraction result item]")
            {
                int total = 0;
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    total += (int)res.m_field4;
                    script[scriptIdx].m_enchanterResultInfo.m_vec.push_back(res);
                }
                script[scriptIdx].m_enchanterResultInfo.m_count = total;
            }
            else if (section == "[doll_controller extraction result item]")
            {
                int cnt[3] = { 0, 0, 0 };
                while (true)
                {
                    stDollControllerResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field6 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    int idx = 3;
                    if (res.m_field4 == 0 || 0x1e < res.m_field6)
                    {
                        if (res.m_field4 < 0x1f || 0x32 < res.m_field6)
                        {
                            if (res.m_field4 < 0x33 || 0x50 < res.m_field6)
                            {
                                LogManager::logFormat(
                                    0, "../RDARScriptExpertJob.cpp",
                                    __FUNCTION__, 0x33b,
                                    "import expertjob script - [doll_controller extraction result item] result_type is invalid");
                                return false;
                            }
                            idx = 2;
                        }
                        else
                        {
                            idx = 1;
                        }
                    }
                    else
                    {
                        idx = 0;
                    }
                    script[scriptIdx].m_dollResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 3; ++i)
                    script[scriptIdx].m_dollResultInfo.m_count[i] = cnt[i];
            }
        }
        return true;
    }
    return false;
}

// ============================================================================
// ImportExpertJobEtcScript（ORIG 0x896ae21）。
// 加载专家职业「.etc」脚本：填充等级上限 map 与分解机大奖概率 map。
// b28d-solo-CExpertJobList 判词（AE）：P164 for(;;)+头部 if(!ScanType) break（复现
// ORIG cond 块在 body 之前）+ P16/P292 bool 形早返 + P198① ok 预清两句式 + P171 照抄
// ORIG 死局部（-0x30 std::string 仅构造/析构、-0xc int 清零未读）+ 两处预清声明分组
// 在循环体首；189=189。
// ============================================================================
bool ImportExpertJobEtcScript(STExpertJobEtcScript& script, const char* path)
{
    // b28d-solo [P16/P292：ORIG 0x0896ae3c `xor $1; test; je <主体>` 为「bool 形早返」]。
    if (!loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
        return false;
    script.Clear();
    std::string section;
    // b28d-solo [P171 照抄 ORIG 死局部：-0x30 std::string（0x0896ae69 构造、
    // 仅 EH/尾段 0x08707be0 析构，全程未使用）与 -0xc int（0x0896ae6e 清零后未读）]。
    std::string unusedSection;
    int unusedLine = 0;
    // b28d-solo [P198①：ORIG 0x0896ae75 `movb $0x0,-0x31` 为 bool 局部预清 ⇒ 两句式]。
    bool ok = false;
    // b28d-solo [P164：ORIG 0x0896ae7c cond 块在 body 之前（4.4 对 while 恒旋转到尾部）
    // ⇒ for(;;) + 头部 `if (!ScanType) break;`，循环外统一 return true]。
    for (;;)
    {
        if (!ScanType(section, true))
            break;
        if (section == "[expertjob level limit]")
        {
            while (true)
            {
                // ORIG 0x0896aeb8/0x0896aebc：循环体首 `movb $0`+`movw $0` 预清 ⇒ 声明=0+赋值。
                unsigned char level = 0;
                short needLevel = 0;
                level = (unsigned char)ScanInt(&ok);
                if (!ok) break;
                needLevel = (short)ScanInt(&ok);
                if (!ok) return false;
                if (!script.m_levelMap.insert(
                        std::make_pair(level, needLevel)).second)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptExpertJob.cpp",
                        __FUNCTION__, 0x4a,
                        "import expertjob etc script - [expertjob level limit] index duplicated");
                    return false;
                }
            }
        }
        else if (section == "[disjointer big win rate]")
        {
            while (true)
            {
                unsigned char key = 0;
                unsigned char val = 0;
                key = (unsigned char)ScanInt(&ok);
                if (!ok) break;
                val = (unsigned char)ScanInt(&ok);
                if (!ok) return false;
                if (!script.m_disjointerBigWinRate.insert(
                        std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptExpertJob.cpp",
                        __FUNCTION__, 0x60,
                        "import expertjob etc script - [disjointer big win rate] index duplicated");
                    return false;
                }
            }
        }
    }
    return true;
}

// ============================================================================
// 构造（ORIG 0x896e312，键到 g_expertJobScriptBaseDirectory）
// ============================================================================
CExpertJobList::CExpertJobList()
    : m_jobMap(), m_etc() {}

// [b28d-Main·检查点21] CExpertJobList/STExpertJobEtcScript 的 dtor 已删（ORIG 无符号）。

// ============================================================================
// init（ORIG 0x896d8ec）
// b28d-solo-CExpertJobList 判词（NEAR / N82附 封顶）：133=133 助记符序列全等；
// 残差仅 job 槽位（ORIG -0x24 vs 本 TU -0x40）与其后 make_pair/pair/insert/end
// 四个临时槽整体差 4——4.4 标量分区排布 vs 4.1 按生命周期穿插（N82附；job 在
// 函数头/循环体首/循环尾三形实测均落 -0x40..-0x48，it/script/begin 临时槽已对齐）。
// ============================================================================
bool CExpertJobList::init()
{
    // ORIG 0x896d8ec：无空指针检查，直接遍历 g_expertJobScriptFileList 对象。
    const char* path = 0;   // ORIG -0x1c(%ebp)：循环内最后 path，ETC 失败时作 %s 参数
    int job;                // ORIG -0x24(%ebp)（4.4 恒排深槽区 -0x40，见 N82附 标量分区）
    // b28d-solo [P96/P50：ORIG 用非 const iterator + 独立 C1Ev 默认构造（-0x44）
    // + begin() 临时槽 -0x4c 后内联拷贝赋值；P28：增量段为 ppEi（后置 it++）]。
    std::map<int, std::string>::iterator it;
    for (it = g_expertJobScriptFileList.m_fileMap.begin();
         it != g_expertJobScriptFileList.m_fileMap.end(); it++)
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        STExpertJobScript* script = new STExpertJobScript();
        if (!ImportExpertJobScript(script, path))
        {
            printf("expert job Script Error: %s\n", path);
            delete script;
            return false;
        }
        job = g_expertJobScriptFileList.FindIndexByFullScan(path);
        m_jobMap.insert(std::make_pair(job, script));   // ORIG 0x896d8ec：find 后 insert（NEW 二进制/源码曾丢失）
    }
    if (!ImportExpertJobEtcScript(m_etc, "expertjob.etc"))
    {
        printf("ExpertJob ETC Script Error : %s\n", path);
        return false;
    }
    return true;
}

// ============================================================================
// destroy（ORIG 0x896dac6）
// b28d-solo-CExpertJobList 判词（AE）：P13 缓存 end()（ORIG 0x0896dada 前一次性求值、
// 条件只比较）+ P141 删手写 delete 判空（ORIG 仅 test/je 自带守卫）；42=42。
// ============================================================================
void CExpertJobList::destroy()
{
    // b28d-solo [P13：ORIG 0x0896dada 前一次性求值 end()（循环条件只比较不重调）]。
    for (std::map<int, STExpertJobScript*>::iterator it = m_jobMap.begin(),
             e = m_jobMap.end(); it != e; ++it)
    {
        STExpertJobScript* script = it->second;
        delete script;
    }
}

// ============================================================================
// get（ORIG W 0x0822b5f2 调用链）
// ============================================================================
STExpertJobScript* CExpertJobList::get(int job)
{
    std::map<int, STExpertJobScript*>::iterator it = m_jobMap.find(job);
    // b28d-61 [P222 翻正，ORIG 0x0822b3bf `je +0x5d`→mov$0 岛、命中块 fall-through]：
    // 旧「否定早返」形 `if (it == end()) return 0; return it->second;` 在 4.4 产
    // 反向块序（je→命中块、mov$0 落 fall-through，diff 8）；改正向形后 32/32 全等。
    if (it != m_jobMap.end())
        return it->second;
    return 0;
}

// ============================================================================
// STExpertJobEtcScript::isExpertJobLimitLevel（ORIG W 0x0849f47c）
// 遍历 m_levelMap（map<uchar,short>），任一 mapped short == level 返回 true。
// 供 CExpertJob::IncreaseExpertJobExp 判断当前等级是否为边界上限。
// ============================================================================
bool STExpertJobEtcScript::isExpertJobLimitLevel(int level)
{
    // [b28c-L29] ORIG W 0x0849f47c：循环尾用后置 it++（ppEi 带 dummy int 实参
    // 0x0 + sret 旧值槽 -0xc），非前置 ++it。
    for (std::map<unsigned char, short>::iterator it = m_levelMap.begin();
         it != m_levelMap.end(); it++)
    {
        if ((int)it->second == level)
            return true;
    }
    return false;
}
