// ============================================================================
// CharacManageScript.cpp —— charac_manage.etc 脚本宿主查询方法（ORIG 专属批次）
//
// 覆盖 5 个 ORIG 符号（均经 docs/class_func_reports/CharacManageScript/ 报告
// ## 汇编 + ## 反编译 C 完整读取后转写）：
//   GetExpBonus            T 0x088e0f58（GetExpBonus.md）
//   GetGoldBonus           T 0x088e0fce（GetGoldBonus.md）
//   GetLevel               T 0x088e0ed4（GetLevel.md）
//   GetPoint               T 0x088e0e0a（GetPoint.md）
//   GetLevelUpRequirePoint T 0x088e1114（GetLevelUpRequirePoint.md）
//
// 此前实现分别暂居 CPartyDepsD2.cpp（GetExpBonus/GetGoldBonus，PartyDepsD2
// 批次依赖闭环）与 CUser.cpp（GetLevel/GetPoint/GetLevelUpRequirePoint，
// LinkMisc5 批次），本批次归位至本 TU。
// ============================================================================

#include "CharacManageScript.h"

// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus —— ORIG T 0x088e0f58
// m_expBonus(+0x18)[level] 命中 → fildl 值后 flds _DAT_08e0c554(=0.1f，
// rodata 8e0c554: 3d cc cc cd) fdivrp → 值/0.1f；未命中 fldz。
// （GetGoldBonus 同构，map 为 m_goldBonus +0x30。）
// ---------------------------------------------------------------------------
float CharacManageScript::GetExpBonus(int level)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus —— ORIG T 0x088e0fce（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
float CharacManageScript::GetGoldBonus(int level)
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetLevel —— ORIG T 0x088e0ed4
// point 为 0 返 0；自高向低找首个不高于 point 的门槛，等级取其下标加一，
// 最后 clamp 到 m_manageLevelMax。
// ---------------------------------------------------------------------------
int CharacManageScript::GetLevel(int point)
{
    if (point == 0)
        return 0;

    int level = 0;
    unsigned int cursor = m_manageLevelPoint.size() - 1;
    while ((int)cursor >= 0)
    {
        if (m_manageLevelPoint.operator[](cursor) <= point)
        {
            level = (int)(cursor + 1);
            break;
        }
        --cursor;
    }

    if (m_manageLevelMax < level)
        return m_manageLevelMax;
    return level;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetPoint —— ORIG T 0x088e0e0a
// Σ 每行区间 [start..end] 内 lv 不超过 level 的 bonus。
// ---------------------------------------------------------------------------
int CharacManageScript::GetPoint(int level)
{
    int total = 0;
    for (std::vector<ManagePointBonus>::iterator it = m_pointBonus.begin();
         it != m_pointBonus.end(); it++)
    {
        ManagePointBonus& bonus = *it;
        for (int lv = it->m_field00; lv <= it->m_field04; ++lv)
        {
            if (level >= lv)
                total += it->m_field08;
        }
    }
    return total;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetLevelUpRequirePoint —— ORIG T 0x088e1114
// max 为 0 或越界返 0；否则取第 max 个门槛值。
// ---------------------------------------------------------------------------
int CharacManageScript::GetLevelUpRequirePoint(unsigned int maxLevel)
{
    if ((maxLevel == 0) || (maxLevel > m_manageLevelPoint.size()))
        return 0;
    return m_manageLevelPoint[maxLevel - 1];
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus(int, int*) —— ORIG T 0x088e1044
// 命中 m_expBonus[level] 写 *out，未命中不动 *out（void 返回）。
// ---------------------------------------------------------------------------
void CharacManageScript::GetExpBonus(int level, int* out)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        *out = it->second;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus(int, int*) —— ORIG T 0x088e10ac（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
void CharacManageScript::GetGoldBonus(int level, int* out)
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        *out = it->second;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetMinimumPointCharacterLevel —— ORIG T 0x088e1266
// m_pointBonus 空 → 0x28；否则取首行 m_field00。
// ---------------------------------------------------------------------------
int CharacManageScript::GetMinimumPointCharacterLevel()
{
    if (m_pointBonus.empty())
        return 0x28;
    return m_pointBonus[0].m_field00;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetName —— ORIG T 0x088e1166
// m_basicExplain[key] 命中 → 拷贝 string，未命中 → 空串（std::string 返回值）。
// ---------------------------------------------------------------------------
std::string CharacManageScript::GetName(unsigned int key)
{
    std::map<int, std::string>::iterator it = m_basicExplain.find((int)key);
    if (it != m_basicExplain.end())
        return it->second;
    return std::string();
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetOptionValue —— ORIG T 0x088e11fc
// m_manageOption[type] 命中 → second，未命中 → 0。
// ---------------------------------------------------------------------------
int CharacManageScript::GetOptionValue(int type)
{
    std::map<int, int>::iterator it = m_manageOption.find(type);
    if (it != m_manageOption.end())
        return it->second;
    return 0;
}
