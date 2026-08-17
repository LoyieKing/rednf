// df_game_r 技能定义类 CSkill 还原（G1 玩家域第一批）。
// 逐函数对照 docs/class_func_reports/CSkill.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
#include <string.h>

#include "CSkill.h"
#include "CDataManager.h"

// ---- STSecondGrowTypeFitness（ORIG 0x083748da / 0x083748ee）----
void STSecondGrowTypeFitness::clear()
{
    memset(m_fitness, 0, sizeof(m_fitness));
    memset(m_maxLevel, 0, sizeof(m_maxLevel));
}

STSecondGrowTypeFitness::STSecondGrowTypeFitness()
{
    clear();
}

// ---- 构造 / 析构（ORIG 0x08374944 / 0x08379fd8）----
CSkill::CSkill()
{
    memset(m_fitnessSecondGrowType, 0, sizeof(m_fitnessSecondGrowType));
    m_isSpecialSkill = 0;
    m_keyCommandList.clear();
    m_isCommandCustomizing = 1;
    memset(m_skillName, 0, sizeof(m_skillName));
    for (int i = 0; i <= 5; ++i)
    {
        m_fitGrowType[i] = -1;
    }
}

CSkill::~CSkill()
{
}

// ORIG 0x83507e8：组号重排（flag → 3；0..3 按 count 改 1/0；4 → 2）
void CSkill::reform_ui_group_no(int& uiGroupNo, bool flag, int count)
{
    if (flag)
    {
        uiGroupNo = 3;
        return;
    }
    if (uiGroupNo < 0)
    {
        return;
    }
    if (uiGroupNo <= 3)
    {
        if (count <= 2)
        {
            uiGroupNo = 1;
        }
        else
        {
            uiGroupNo = 0;
        }
    }
    else if (uiGroupNo == 4)
    {
        uiGroupNo = 2;
    }
}

// ---- 简单访问器（ORIG weak inline，单条指令形态）----
int CSkill::get_index() const
{
    return m_index;
}

int CSkill::get_group() const
{
    return m_group;
}

void CSkill::set_index(int idx)
{
    m_index = idx;
}

void CSkill::set_group(int group)
{
    m_group = group;
}

void CSkill::set_job_type(int job)
{
    m_jobType = job;
}

void CSkill::set_skill_type(char type)
{
    m_skillType = type;
}

void CSkill::set_level_limit(int limit)
{
    m_levelLimit = limit;
}

void CSkill::SetStealable(bool flag)
{
    m_stealable = flag;
}

void CSkill::SetStealLevelPenalty(int value)
{
    m_stealLevelPenalty = value;
}

void CSkill::SetStealSPPenalty(int value)
{
    m_stealSPPenalty = value;
}

int CSkill::GetStealLevelPenalty() const
{
    return m_stealLevelPenalty;
}

int CSkill::GetStealSPPenalty() const
{
    return m_stealSPPenalty;
}

void CSkill::set_fitGrowType(int idx, int value)
{
    m_fitGrowType[idx] = value;
}

void CSkill::set_fitness(int idx, int value)
{
    m_fitness[idx] = value;
}

int CSkill::get_fitness(int idx) const
{
    return m_fitness[idx];
}

void CSkill::set_type_max_level(int idx, int value)
{
    m_typeMaxLevel[idx] = value;
}

int CSkill::get_type_max_level(int idx) const
{
    return m_typeMaxLevel[idx];
}

void CSkill::set_fitness_second_growtype(int secondGrowType, int value)
{
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_fitnessSecondGrowType[secondGrowType - 1] = value;
}

int CSkill::get_fitness_second_growtype(int secondGrowType) const
{
    if (secondGrowType < 1 || 2 < secondGrowType)
    {
        return -1;
    }
    return m_fitnessSecondGrowType[secondGrowType - 1];
}

void CSkill::set_second_growtype_fitness(int firstGrowType, int secondGrowType,
                                         int value)
{
    if (firstGrowType < 0)
        return;
    if (firstGrowType > 5)
        return;
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_secondGrowTypeFitness[firstGrowType].m_fitness[secondGrowType - 1] =
        value;
}

int CSkill::get_second_growtype_fitness(int firstGrowType,
                                        int secondGrowType) const
{
    if (firstGrowType < 0 || 6 < firstGrowType || secondGrowType < 0 ||
        2 < secondGrowType)
    {
        return -1;
    }
    if (firstGrowType >= 0 && firstGrowType <= 6 && secondGrowType == 0)
    {
        return 0;
    }
    return m_secondGrowTypeFitness[firstGrowType].m_fitness[secondGrowType - 1];
}

void CSkill::set_second_growtype_max_level(int firstGrowType,
                                           int secondGrowType, int value)
{
    if (firstGrowType < 0)
        return;
    if (firstGrowType > 5)
        return;
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_secondGrowTypeFitness[firstGrowType].m_maxLevel[secondGrowType - 1] =
        value;
}

int CSkill::get_second_growtype_max_level(int firstGrowType,
                                          int secondGrowType) const
{
    if (firstGrowType < 0 || 5 < firstGrowType || secondGrowType < 1 ||
        2 < secondGrowType)
    {
        return -1;
    }
    if (firstGrowType >= 0 && firstGrowType <= 6 && secondGrowType == 0)
    {
        return 0;
    }
    return m_secondGrowTypeFitness[firstGrowType].m_maxLevel[secondGrowType - 1];
}

void CSkill::set_prelearned_skill()
{
    std::list<_pre_skill>::iterator it;
    int count = (int)m_preSkillList.size();
    it = m_preSkillList.begin();
    for (int i = 0; i < count; ++i)
    {
        _pre_skill* pSkill = &*it;
        CSkill* found = G_CDataManager()->m_skillList->find_skill(
            m_jobType, pSkill->m_index);
        if (found != 0)
        {
            pSkill->m_group = found->get_group();
        }
        it++;
    }
}

void CSkill::add_need_sp(int value)
{
    m_needSP.push_back(value);
}

void CSkill::add_need_sfp(int value)
{
    m_needSFP.push_back(value);
}

void CSkill::add_learned_skill(_pre_skill skill)
{
    m_preSkillList.push_back(skill);
}

void CSkill::setSkillName(const char* name)
{
    strncpy(m_skillName, name, 0x64);
}

const char* CSkill::getSkillName() const
{
    return m_skillName;
}

bool CSkill::is_active_skill() const
{
    return m_skillType == 0;
}

bool CSkill::IsSpecialSkill() const
{
    if (m_isSpecialSkill == 0)
    {
        return false;
    }
    return true;
}

char CSkill::isCommandCustomizing() const
{
    return m_isCommandCustomizing;
}

// ---- 主逻辑（ORIG strong）----
int CSkill::IsStealable(int firstGrowType, int secondGrowType) const
{
    if (m_stealable == 0)
    {
        return 0;
    }
    for (int i = 0; i <= 5; ++i)
    {
        if (m_fitGrowType[i] == firstGrowType)
        {
            return 0;
        }
    }
    if (0 < secondGrowType)
    {
        for (int i = 0; i <= 1; ++i)
        {
            if (m_fitnessSecondGrowType[i] == secondGrowType)
            {
                return 0;
            }
        }
    }
    return 1;
}

bool CSkill::can_learn(int level, int firstGrowType, int secondGrowType,
                       int targetLevel) const
{
    int maxLevel;
    if (secondGrowType < 1)
    {
        if (targetLevel < 1 || get_type_max_level(firstGrowType) < targetLevel)
        {
            return false;
        }
        if (get_type_max_level(firstGrowType) <
            (level - m_levelLimit) + 1)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
        else
        {
            maxLevel = (level - m_levelLimit) + 1;
        }
    }
    else
    {
        maxLevel = get_second_growtype_max_level(firstGrowType,
                                                 secondGrowType);
        if (maxLevel == 0)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
        if (targetLevel < 1 || maxLevel < targetLevel)
        {
            return false;
        }
        if ((level - m_levelLimit) + 1 <= maxLevel)
            maxLevel = (level - m_levelLimit) + 1;
        else
            maxLevel = maxLevel;
    }
    if (maxLevel < targetLevel)
    {
        return false;
    }
    int needLevel = m_levelLimit + m_fieldC8 * (targetLevel - 1);
    return needLevel <= level;
}

int CSkill::getMaxSkillLevel(int level, int firstGrowType,
                             int secondGrowType) const
{
    int maxLevel;
    if (secondGrowType < 1)
    {
        maxLevel = get_type_max_level(firstGrowType);
    }
    else
    {
        maxLevel = get_second_growtype_max_level(firstGrowType,
                                                 secondGrowType);
        if (maxLevel == 0)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
    }
    if (maxLevel == 0)
    {
        return -1;
    }
    if ((level - m_levelLimit) + 1 <= maxLevel)
        maxLevel = (level - m_levelLimit) + 1;
    else
        maxLevel = maxLevel;
    int q = (level - m_levelLimit) / m_fieldC8 + 1;
    if ((unsigned)(q < maxLevel ? q : maxLevel) >> 31)
    {
        return 0;
    }
    return q < maxLevel ? q : maxLevel;
}

int CSkill::get_need_sp_for_level(int level, int targetLevel) const
{
    std::list<int>::const_iterator it;
    int count = (int)m_needSP.size();
    if (count < targetLevel)
    {
        return -1;
    }
    if (level < 0)
    {
        return -1;
    }
    int total = 0;
    it = m_needSP.begin();
    for (int i = 1; i < targetLevel; ++i)
    {
        if (i > level && i < targetLevel)
        {
            total += *it;
        }
        it++;
    }
    total += *it;
    return total;
}

int CSkill::get_need_sfp_for_level(int level, int targetLevel) const
{
    std::list<int>::const_iterator it;
    int count = (int)m_needSFP.size();
    if (count < targetLevel)
    {
        return -1;
    }
    if (level < 0)
    {
        return -1;
    }
    int total = 0;
    it = m_needSFP.begin();
    for (int i = 1; i < targetLevel; ++i)
    {
        if (i > level && i < targetLevel)
        {
            total += *it;
        }
        it++;
    }
    total += *it;
    return total;
}

int CSkill::get_need_sp_sfp_for_level(int level, int targetLevel) const
{
    if (IsSpecialSkill())
    {
        return get_need_sfp_for_level(level, targetLevel);
    }
    return get_need_sp_for_level(level, targetLevel);
}

int CSkill::get_guildSkillNeedGSP(int count) const
{
    if ((unsigned)m_guildSkillNeedGSP.size() < (unsigned)count)
    {
        return -1;
    }
    return m_guildSkillNeedGSP.at(count - 1);
}

int CSkill::get_guildSkillNeedGold(int count) const
{
    if ((unsigned)m_guildSkillNeedGold.size() < (unsigned)count)
    {
        return -1;
    }
    return m_guildSkillNeedGold.at(count - 1);
}

int CSkill::get_needGuildLevel(int count) const
{
    return m_needGuildLevel + m_needGuildLevelAdd * (count - 1);
}
