// ============================================================================
// df_game_r 还原 —— CExpertJobList（专家职业脚本列表，G5 批次）。
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
// ============================================================================
namespace expert_job
{
bool get_disjoint_additional_items(
    std::multimap<int, stItemSelectRate>& m, int key,
    std::vector<stItemSelectRate>& out)
{
    if (m.find(key) == m.end())
        return false;
    for (std::multimap<int, stItemSelectRate>::iterator it = m.lower_bound(key);
         it != m.upper_bound(key); ++it)
    {
        out.push_back(it->second);
    }
    return true;
}
}

// ============================================================================
// STExpertJobScript::Clear —— 清空全部容器并清零标量。
// 由 ImportExpertJobScript 在加载前调用（脚本为 new 分配，容器已默认构造，
// 此处保证重复加载语义一致）。
// ============================================================================
void STExpertJobScript::Clear()
{
    m_expVec.clear();
    m_growSkillVec.clear();
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
    m_disjointResultMap.clear();
    m_baseConst = 0;
    m_additionalConst = 0;
    m_clearQuests.clear();
    m_giveupCost.clear();
    m_compoundRateVec.clear();
    m_recipeInfoMap.clear();
    m_recipeIndexMap.clear();
    m_recipeLevelMap.clear();
    m_recipeItemMap.clear();
    m_enduranceRepairCost.clear();
    m_enduranceReduce1 = 0;
    m_enduranceReduce2 = 0;
    m_enduranceInit = 0;
    m_upgradeCost.clear();
    m_gainExp1 = 0;
    m_gainExp2 = 0;
    m_limitDisjointGold = 0;
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
// ============================================================================
bool ImportExpertJobScript(STExpertJobScript* script, const char* path)
{
    bool ok;
    if (loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
    {
        script->Clear();
        std::string section;
        std::multimap<int, stItemSelectRate> additional;  // [additional result]
        std::multimap<int, stItemSelectRate> bigwin;      // [big win result]

        while (ScanType(section, true))
        {
            if (section == "[expertness exp]")
            {
                while (true)
                {
                    int exp = ScanInt(&ok);
                    if (!ok) break;
                    int level = ScanInt(&ok);
                    if (!ok) return false;
                    std::string name;
                    ScanStr(&name);
                    ExpertJobExp e;
                    e.m_exp = exp;
                    e.m_level = level;
                    e.m_name = name;
                    script->m_expVec.push_back(e);
                }
            }
            else if (section == "[skill]")
            {
                script->m_growSkillVec.clear();
                std::pair<int, int> p;
                while (p.first = ScanInt(&ok), ok)
                {
                    p.second = ScanInt(&ok);
                    if (!ok) return false;
                    script->m_growSkillVec.push_back(p);
                }
            }
            else if (section == "[additional result]")
            {
                additional.clear();
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
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
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
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
                script->m_baseConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[additional const]")
            {
                script->m_additionalConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[disjoint result]")
            {
                script->m_disjointResultMap.clear();
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
                    script->m_disjointResultMap.insert(std::make_pair(grade, res));
                }
            }
            else if (section == "[clear quests]")
            {
                script->m_clearQuests.clear();
                for (int v = ScanInt(&ok); ok; v = ScanInt(&ok))
                    script->m_clearQuests.push_back(v);
            }
            else if (section == "[giveup cost]")
            {
                script->m_giveupCost.clear();
                for (int v = ScanInt(&ok); ok; v = ScanInt(&ok))
                {
                    script->m_giveupCost.push_back(v);
                    if (script->m_giveupCost.size() > 9)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                }
            }
            else if (section == "[compound rate]")
            {
                script->m_compoundRateVec.clear();
                stCompoundRate r;
                while (r.m_level = ScanInt(&ok), ok)
                {
                    r.m_successRate = ScanInt(&ok); if (!ok) return false;
                    r.m_expMin      = ScanInt(&ok); if (!ok) return false;
                    r.m_expMax      = ScanInt(&ok); if (!ok) return false;
                    script->m_compoundRateVec.push_back(r);
                }
            }
            else if (section == "[items]")
            {
                script->m_recipeInfoMap.clear();
                script->m_recipeIndexMap.clear();
                script->m_recipeLevelMap.clear();
                script->m_recipeItemMap.clear();
                for (int idx = ScanInt(&ok); ok; idx = ScanInt(&ok))
                {
                    int itemIdx = ScanInt(&ok);
                    if (!ok) return false;
                    unsigned long v3 = ScanInt(&ok);
                    if (!ok) return false;
                    unsigned short v4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (0xef < idx)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                    stRecipeInfo info;
                    info.m_field8 = v4;
                    script->m_recipeInfoMap.insert(std::make_pair(idx, info));
                    script->m_recipeIndexMap.insert(
                        std::make_pair((unsigned long)itemIdx, (int)v3));
                    script->m_recipeLevelMap.insert(
                        std::make_pair(itemIdx, (int)v3));
                    script->m_recipeItemMap.insert(
                        std::make_pair(v3, (unsigned long)v4));
                }
            }
            else if (section == "[endurance repair cost]")
            {
                script->m_enduranceRepairCost.clear();
                stEnduranceRepairCost r;
                while (r.m_field0 = ScanInt(&ok), ok)
                {
                    r.m_field4 = ScanInt(&ok); if (!ok) return false;
                    script->m_enduranceRepairCost.push_back(r);
                }
            }
            else if (section == "[endurance reduce]")
            {
                script->m_enduranceReduce1 = ScanInt(&ok);
                if (!ok) return false;
                script->m_enduranceReduce2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[endurance initial value]")
            {
                script->m_enduranceInit = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[upgrade cost]")
            {
                script->m_upgradeCost.clear();
                for (int idx = ScanInt(&ok); ok; idx = ScanInt(&ok))
                {
                    if (idx - 1 != (int)script->m_upgradeCost.size())
                        return false;
                    int v = ScanInt(&ok);
                    if (!ok) return false;
                    script->m_upgradeCost.push_back(v);
                }
            }
            else if (section == "[potion info]")
            {
                // 读取 (itemIndex, value) 对，按 itemIndex 写入两个全局炼金术值；
                // 非法 itemIndex 返回 false。
                for (;;)
                {
                    int idx = ScanInt(&ok);
                    if (!ok) break;
                    int val = ScanInt(&ok);
                    if (!ok) return false;
                    if (idx == 0x27ac52 || idx == 0x27ac59)
                    {
                        g_alchemistInfo[0] = val;
                    }
                    else if (idx == 0x98981a || idx == 0x1f8e ||
                             idx == 0x98988d || idx == 0x2920913e ||
                             idx == 0x989885)
                    {
                        g_alchemistInfo[1] = val;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (section == "[gain exp]")
            {
                script->m_gainExp1 = ScanInt(&ok);
                if (!ok) return false;
                script->m_gainExp2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[limit disjoint gold]")
            {
                script->m_limitDisjointGold = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[portable disjoint info]")
            {
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_portableDisjointMap.insert(
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
                script->m_selfDisjoin.m_field0 = ScanInt(&ok);
                if (!ok) return false;
                script->m_selfDisjoin.m_field4 = ScanInt(&ok);
                if (!ok) return false;
                script->m_selfDisjoin.m_field8 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[shop doll info]")
            {
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    unsigned char val = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_shopDollMap.insert(
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
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_unsealScrollMap.insert(
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
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    stMonsterCardBindRate rate;
                    rate.m_field0 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    rate.m_field2 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_monsterCardBindRateMap.insert(
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
                    res.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script->m_specialBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field8;
                }
                for (int i = 0; i < 4; ++i)
                    script->m_specialBindInfo.m_count[i] = cnt[i];
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
                    res.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script->m_commonBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field8;
                }
                for (int i = 0; i < 4; ++i)
                    script->m_commonBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[alchemist extraction info]")
            {
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_alchemistExtractInfo.insert(
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
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_enchanterExtractInfo.insert(
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
                for (int key = ScanInt(&ok); ok; key = ScanInt(&ok))
                {
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script->m_dollExtractInfo.insert(
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
                    res.m_field8 = (unsigned short)ScanInt(&ok);
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
                    script->m_alchemistResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field8;
                }
                for (int i = 0; i < 3; ++i)
                    script->m_alchemistResultInfo.m_count[i] = cnt[i];
            }
            else if (section == "[enchanter extraction result item]")
            {
                int total = 0;
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    total += (int)res.m_field8;
                    script->m_enchanterResultInfo.m_vec.push_back(res);
                }
                script->m_enchanterResultInfo.m_count = total;
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
                    res.m_field8 = (unsigned short)ScanInt(&ok);
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
                    script->m_dollResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field8;
                }
                for (int i = 0; i < 3; ++i)
                    script->m_dollResultInfo.m_count[i] = cnt[i];
            }
        }
        return true;
    }
    return false;
}

// ============================================================================
// ImportExpertJobEtcScript（ORIG 0x896ae21）。
// 加载专家职业「.etc」脚本：填充等级上限 map 与分解机大奖概率 map。
// ============================================================================
bool ImportExpertJobEtcScript(STExpertJobEtcScript& script, const char* path)
{
    bool ok;
    if (loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
    {
        script.Clear();
        std::string section;
        while (ScanType(section, true))
        {
            if (section == "[expertjob level limit]")
            {
                while (true)
                {
                    unsigned char level = (unsigned char)ScanInt(&ok);
                    if (!ok) break;
                    short needLevel = (short)ScanInt(&ok);
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
                    unsigned char key = (unsigned char)ScanInt(&ok);
                    if (!ok) break;
                    unsigned char val = (unsigned char)ScanInt(&ok);
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
    return false;
}

// ============================================================================
// 构造（ORIG 0x896e312，键到 g_expertJobScriptBaseDirectory）
// ============================================================================
CExpertJobList::CExpertJobList()
    : m_jobMap(), m_etc() {}

CExpertJobList::~CExpertJobList() {}

// ============================================================================
// init（ORIG 0x896d8ec）
// ============================================================================
bool CExpertJobList::init()
{
    // ORIG 0x896d8ec：无空指针检查，直接遍历 g_expertJobScriptFileList 对象。
    const char* path = 0;   // ORIG -0x1c(%ebp)：循环内最后 path，ETC 失败时作 %s 参数
    for (std::map<int, std::string>::const_iterator it =
             g_expertJobScriptFileList.m_fileMap.begin();
         it != g_expertJobScriptFileList.m_fileMap.end(); ++it)
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        STExpertJobScript* script = new STExpertJobScript();
        if (!ImportExpertJobScript(script, path))
        {
            printf("expert job Script Error: %s", path);
            delete script;
            return false;
        }
        int job = g_expertJobScriptFileList.FindIndexByFullScan(path);
    }
    if (!ImportExpertJobEtcScript(m_etc, "expertjob.etc"))
    {
        printf("ExpertJob ETC Script Error : %s", path);
        return false;
    }
    return true;
}

// ============================================================================
// destroy（ORIG 0x896dac6）
// ============================================================================
void CExpertJobList::destroy()
{
    for (std::map<int, STExpertJobScript*>::iterator it = m_jobMap.begin();
         it != m_jobMap.end(); ++it)
    {
        STExpertJobScript* script = it->second;
        if (script != 0)
            delete script;
    }
    m_jobMap.clear();
}

// ============================================================================
// get（ORIG W 0x0822b5f2 调用链）
// ============================================================================
STExpertJobScript* CExpertJobList::get(int job)
{
    std::map<int, STExpertJobScript*>::iterator it = m_jobMap.find(job);
    return it == m_jobMap.end() ? 0 : it->second;
}

// ============================================================================
// STExpertJobEtcScript::isExpertJobLimitLevel（ORIG W 0x0849f47c）
// 遍历 m_levelMap（map<uchar,short>），任一 mapped short == level 返回 true。
// 供 CExpertJob::IncreaseExpertJobExp 判断当前等级是否为边界上限。
// ============================================================================
bool STExpertJobEtcScript::isExpertJobLimitLevel(int level)
{
    for (std::map<unsigned char, short>::iterator it = m_levelMap.begin();
         it != m_levelMap.end(); ++it)
    {
        if ((int)it->second == level)
            return true;
    }
    return false;
}
