// ============================================================================
// df_game_r 还原 —— RandomOptionScript（G5 random_option 域）
// 逐函数对照 docs/class_func_reports/RandomOptionScript.md：
//   clear 0x0837261e / ctor 0x08372904 / dtor 0x0837c9e8
//   getPrefixGrade 0x08a73a80 / getPrefix 0x08a73956 / 
//   getOptionModificationCost 0x08a738a0 / importRandomOptionScript 0x08a70a90
// ============================================================================

#include "RandomOptionScript.h"

#include <string.h>

extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" int sub_ScanInt(int* out) asm("_Z7ScanIntPi");

void OPTION_VALUE_RATIO::clear()
{
    m_vec0.clear();
    m_c = 0;
}

RandomOptionGive::RandomOptionGive()
{
    clear();
}

RandomOptionGive::RandomOptionGive(const RandomOptionGive& other)
    : m_vec0(other.m_vec0)
{
    m_c = other.m_c;
}

RandomOptionGive::~RandomOptionGive()
{
}

void RandomOptionGive::clear()
{
    m_vec0.clear();
    m_c = 0;
}

RandomOptionScript::RandomOptionScript()
{
    clear();
}

RandomOptionScript::~RandomOptionScript()
{
}

void RandomOptionScript::clear()
{
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_map48.clear();
    m_map60.clear();
    m_map78.clear();
    m_map90.clear();
    m_veca8.clear();
    m_mapb4.clear();
    m_mapcc.clear();
    m_ratioE4.clear();
    m_mapf4.clear();
    m_costVec10c.clear();
    m_map118.clear();
    m_map130.clear();
    m_map160.clear();
    m_map178.clear();
    m_map190.clear();
    m_prefixNormal.clear();
    m_prefixPremium.clear();
    m_choosePrefix1d8.clear();
    m_str1e4.clear();
    m_map1e8.clear();
    m_set200.clear();
    m_set218.clear();
}

int RandomOptionScript::getPrefixGrade(int level) const
{
    for (unsigned int i = 0; i < m_choosePrefix1d8.size(); ++i)
    {
        const ChoosePrefix& cp = m_choosePrefix1d8[i];
        if (level >= cp.m_min && level < cp.m_max)
        {
            return cp.m_grade - 1;
        }
    }
    return 0;
}

std::string RandomOptionScript::getPrefix(int prefix, bool flag)
{
    std::string out;
    const std::map<int, std::vector<std::string> >& map =
        flag ? m_prefixPremium : m_prefixNormal;
    int grade = getPrefixGrade(prefix);
    if (flag)
    {
        grade += 3;
    }
    std::map<int, std::vector<std::string> >::const_iterator it =
        map.find(grade);
    if (it != map.end())
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            if (grade == (int)i)
            {
                out = it->second[i];
                return out;
            }
        }
    }
    return out;
}

int RandomOptionScript::getOptionModificationCost(short level, int rarity) const
{
    for (std::vector<OPTION_MODIFICATION_COST>::const_iterator it =
             m_costVec10c.begin();
         it != m_costVec10c.end(); ++it)
    {
        if (level <= it->m_level)
        {
            if (rarity == 2)
            {
                return it->m_cost2;
            }
            if (rarity == 3)
            {
                return it->m_cost3;
            }
        }
    }
    return -1;
}

// ===================== importRandomOptionScript =====================
// ORIG 0x08a70a90（0x2e10 字节脚本解析器）。语义还原：加载脚本后按段名
// 填充随机选项主表（前缀/费用/等级区间）；深层映射表从简。

bool importRandomOptionScript(RandomOptionScript& script, const char* path)
{
    if (!sub_loadRDARScriptFile("Script/RandomOptionScript", path))
    {
        return false;
    }
    script.clear();

    std::string line;
    int value = 0;
    while (sub_ScanType(line, true))
    {
        if (line == "[option modification cost]")
        {
            OPTION_MODIFICATION_COST cost;
            if (!sub_ScanInt(&value))
            {
                break;
            }
            cost.m_level = (short)value;
            if (!sub_ScanInt(&value))
            {
                break;
            }
            cost.m_cost2 = value;
            if (!sub_ScanInt(&value))
            {
                break;
            }
            cost.m_cost3 = value;
            script.m_costVec10c.push_back(cost);
        }
        else if (line == "[choose prefix]")
        {
            ChoosePrefix cp;
            if (!sub_ScanInt(&value))
            {
                break;
            }
            cp.m_min = value;
            if (!sub_ScanInt(&value))
            {
                break;
            }
            cp.m_max = value;
            if (!sub_ScanInt(&value))
            {
                break;
            }
            cp.m_grade = value + 1;
            script.m_choosePrefix1d8.push_back(cp);
        }
        else if (line == "[prefix normal]")
        {
            int grade = 0;
            std::vector<std::string> names;
            while (sub_ScanType(line, true))
            {
                if (line[0] == '[')
                {
                    script.m_prefixNormal[grade] = names;
                    break;
                }
                names.push_back(line);
            }
        }
    }
    return true;
}
