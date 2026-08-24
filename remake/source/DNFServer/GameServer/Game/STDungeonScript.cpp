// ============================================================================
// df_game_r 还原 —— STDungeonScript 类方法（G5 副本脚本表）
// ORIG：Clear 0x0836abc6 / ctor 0x0836b5e2 / dtor 0x0836bd5c；
// 类布局见 CDungeon.h（0x50a）。ImportDungeonScript 0x088e5faa /
// exportDungeonScript 0x088f0a98 / ImportAdvanceAltarDungeonTag 0x088f20a4
// 为脚本解析/导出（语义简化实现）。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CDungeon.h"
#include "DNFLexWrapper.h"

STDungeonScript::STDungeonScript()
{
    Clear();
}

STDungeonScript::~STDungeonScript()
{
}

void STDungeonScript::Clear()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_str10.clear();
    m_14 = 0;
    m_18 = 0;
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
    m_str28.clear();
    m_2c = 0;
    m_str30.clear();
    m_34 = 0;
    m_vec38.clear();
    m_44 = 0;
    m_vec48.clear();
    m_str54.clear();
    m_str58.clear();
    m_str74.clear();
    m_78 = 0;
    m_88 = 0;
    m_vec98.clear();
    m_a4 = 0;
    m_a8 = 0;
    m_ac = 0;
    m_b0 = 0;
    m_vecc8.clear();
    m_vecd4.clear();
    m_vece0.clear();
    m_vecec.clear();
    m_140 = 0;
    m_141 = 0;
    m_142 = 0;
    m_vec144.clear();
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;
    m_159 = 0;
    m_vec15c.clear();
    m_mazeList170.clear();
    m_str17c.clear();
    m_statMap290.clear();
    m_28f = 0;
    m_2a8 = 0;
    m_2a9 = 0;
    m_2aa = 0;
    m_2ab = 0;
    m_2ac = 0;
    m_2bc[0] = 0;
    m_2bc[1] = 0;
    m_2bc[2] = 0;
    m_vec2c8.clear();
    m_dimension308.clear();
    m_314 = 0;
    m_315 = 0;
    m_316 = 0;
    m_317 = 0;
    m_dailySchedule.clear();
    m_324 = 0;
    m_325 = 0;
    m_338 = 0;
    m_33c = 0;
    m_map340.clear();
    m_358 = 0;
}

bool ImportDungeonScript(STDungeonScript* script, const char* path)
{
    if (!loadRDARScriptFile("Script/DungeonScript", path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    int value = 0;
    if (!ScanType(line, true))
    {
        return false;
    }
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_0 = value;
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_4 = value;
    return true;
}

bool exportDungeonScript(STDungeonScript* script, const char* path)
{
    FILE* file = fopen(path, "w");
    if (!file)
    {
        return false;
    }
    fprintf(file, "%d\n%d\n", script->m_0, script->m_4);
    fclose(file);
    return true;
}

// ---- advancealtar 字符串枚举解析器 ----
// ORIG: StageType::getStageType 0x88990ba / StageDifficulty::getDifficulty
// 0x8899038 / RewardItemType::getRewardItemType 0x889acaa。
bool advancealtar::StageType::getStageType(const std::string& name, T& out)
{
    if (name == "normal")
    {
        out = STAGE_NORMAL;
    }
    else if (name == "survival")
    {
        out = STAGE_SURVIVAL;
    }
    else
    {
        // [推断] ORIG 未识别时写 0 并仍返回 true（见报告反编译）。
        out = STAGE_NORMAL;
    }
    return true;
}

bool advancealtar::StageDifficulty::getDifficulty(const std::string& name,
                                                   T& out)
{
    if (name == "easy")
    {
        out = DIFF_EASY;
    }
    else if (name == "medium")
    {
        out = DIFF_MEDIUM;
    }
    else if (name == "hard")
    {
        out = DIFF_HARD;
    }
    else
    {
        return false;
    }
    return true;
}

bool advancealtar::RewardItemType::getRewardItemType(const std::string& name,
                                                     T& out)
{
    if (name == "normal")
    {
        out = REWARD_NORMAL;
    }
    else if (name == "event")
    {
        out = REWARD_EVENT;
    }
    else
    {
        return false;
    }
    return true;
}

static bool importAltarRewardItem(advancealtar::_RewardItem& item)
{
    // ORIG 内联段：ScanStr→getRewardItemType→ScanInt(+0x04)→ScanInt(+0x08)
    bool ok = false;
    std::string token;
    if (!ScanStr(&token))
    {
        return false;
    }
    advancealtar::RewardItemType::T type;
    if (!advancealtar::RewardItemType::getRewardItemType(token, type))
    {
        return false;
    }
    item.m_type = static_cast<int>(type);
    item.m_value1 = ScanInt(&ok);
    if (!ok)
    {
        return false;
    }
    item.m_value2 = ScanInt(&ok);
    return ok;
}

// ImportAdvanceAltarDungeonTag ORIG 0x088f20a4（536 insn）：
// 按 str 给出的 tag 分派解析；脚本流已由调用方 loadRDARScriptFile 打开
// （path 参数在 ORIG 中未使用）。返回 false=解析失败，true=成功/结束。
bool ImportAdvanceAltarDungeonTag(STDungeonScript* script, std::string str,
                                  const char* path)
{
    (void)path;  // ORIG 未使用该参数
    if (str == "[defense dungeon]")
    {
        // ScanInt(bool*) 返回值非 0 → m_2ab 置 true
        bool ok = false;
        script->m_2ab = (ScanInt(&ok) != 0);
    }
    else if (str == "[advance altar type]")
    {
        std::string token;
        if (!ScanStr(&token))
        {
            return false;
        }
        advancealtar::StageType::T type;
        if (!advancealtar::StageType::getStageType(token, type))
        {
            return false;
        }
        script->m_2ac = static_cast<int>(type);
    }
    else if (str == "[advance altar map]")
    {
        // 循环：难度名 + mapId(m_2b0[d]) + 第二个 int(m_2bc[d])；
        // 任一解析失败 ORIG 直接返回 false（+0x17a/+0x1a9 ebx=0）
        while (true)
        {
            std::string token;
            if (!ScanStr(&token))
            {
                return false;
            }
            advancealtar::StageDifficulty::T diff;
            if (!advancealtar::StageDifficulty::getDifficulty(token, diff))
            {
                return false;
            }
            bool ok = false;
            script->m_2b0[diff] = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_2bc[diff] = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
    }
    else if (str == "[advance altar clear reward]")
    {
        // 循环：[easy]/[medium]/[hard] 选择 ClearRewardData 的三个 vector，
        // 逐项导入 _RewardItem 直到 [/advance altar clear reward] 或解析失败
        while (true)
        {
            std::string token;
            if (!ScanType(token, true))
            {
                break;
            }
            int idx = -1;
            if (token == "[easy]")
            {
                idx = 0;
            }
            else if (token == "[medium]")
            {
                idx = 1;
            }
            else if (token == "[hard]")
            {
                idx = 2;
            }
            else
            {
                // [/advance altar clear reward] 或其它 tag → 本节结束
                break;
            }
            std::vector<advancealtar::_RewardItem>& vec =
                (idx == 0) ? script->m_clearReward2d4.m_vec0
                           : (idx == 1) ? script->m_clearReward2d4.m_vecc
                                        : script->m_clearReward2d4.m_vec18;
            while (true)
            {
                advancealtar::_RewardItem item;
                if (!importAltarRewardItem(item))
                {
                    break;
                }
                vec.push_back(item);
            }
        }
    }
    else if (str == "[advance altar survival map]")
    {
        while (true)
        {
            bool ok = false;
            int mapId = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec2c8.push_back(mapId);
        }
    }
    else if (str == "[advance altar survival clear reward]")
    {
        // 多个 [type]…[/type] 块 → SurvivalRewardData::m_vec
        while (true)
        {
            std::string token;
            if (!ScanType(token, true))
            {
                break;
            }
            if (token != "[type]")
            {
                break;
            }
            advancealtar::RoundRewardData rrd;
            rrd.m_0 = 0;
            rrd.m_4 = 0;
            while (true)
            {
                std::string sub;
                if (!ScanType(sub, true))
                {
                    break;
                }
                if (sub == "[/type]")
                {
                    script->m_survival2f8.m_vec.push_back(rrd);
                    break;
                }
                bool ok = false;
                if (sub == "[round]")
                {
                    rrd.m_0 = ScanInt(&ok);   // 轮次
                    if (!ok)
                    {
                        break;
                    }
                    rrd.m_4 = ScanInt(&ok);   // 奖励参数
                    if (!ok)
                    {
                        break;
                    }
                }
                else if (sub == "[list]")
                {
                    // 逐项 _RewardItem；ORIG 仅接受 type 0/1（getRewardItemType
                    // 只会产生这两个值），失败即结束本 list
                    while (true)
                    {
                        advancealtar::_RewardItem item;
                        if (!importAltarRewardItem(item))
                        {
                            break;
                        }
                        if (item.m_type == 0 || item.m_type == 1)
                        {
                            rrd.m_vec8.push_back(item);
                        }
                    }
                }
                // 其它子 tag：忽略并继续（推断）
            }
        }
    }
    // 其它 tag：ORIG 直接返回成功（未识别 tag 忽略）
    return true;
}
