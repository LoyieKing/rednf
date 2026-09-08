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
    switch (uiGroupNo)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        if (count > 2)
        {
            uiGroupNo = 0;
        }
        else
        {
            uiGroupNo = 1;
        }
        break;
    case 4:
        uiGroupNo = 2;
        break;
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
    CSkill* found;
    int i;
    _pre_skill* pSkill;
    it = m_preSkillList.begin();
    for (i = 0; i < count; ++i)
    {
        pSkill = &*it;
        found = G_CDataManager()->m_skillList->find_skill(
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
// ORIG 0x83504ec _ZNK6CSkill11IsStealableEii：体内 `xor $0x1,%eax; test %al,%al` bool 物化形
bool CSkill::IsStealable(int firstGrowType, int secondGrowType) const
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
    // b28d-37 [P194/X 族封顶，ORIG 0x83?]：ORIG `jge` 直跳 vs 4.4 `setl+test+je` 物化（int 比较物化不可翻）；107/115。
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
    // b28d-37 [真语义 bug 修复，ORIG 0x08350700]：maxLevel 须取 min((level-m_levelLimit)+1, maxLevel)（旧实现恒无操作致上限偏大）；残 cmov（P230① -march=i486 可消，待路由）+ N82 帧。
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
    // b28d-37 [真语义 bug 修正，ORIG 0x08350785]：ORIG 为
    // `eax=v+1; edx=maxLevel; cmp %edx,%eax; jle L; mov %edx,%eax; L: mov %eax,-0x10`
    // ⇒ maxLevel = min((level-m_levelLimit)+1, maxLevel)（v+1<=maxLevel 时上限缩小）。
    // 旧实现 `if (v+1 > keep) maxLevel = keep;` 恒为无操作（keep==maxLevel），
    // 导致返回上限偏大（v+1<=maxLevel 时未缩小）。改三元 min 形。
    // [N 封顶] 残差：4.4 三元产 `cmovle`（ORIG 为 jle 分支），P230① -march=i486
    // 可消 cmov 族但本波不改路由 ⇒ 待 Main 统一路由变更后复测；另帧 0x20 vs
    // ORIG 0x38 属 N82 槽分区。78/77。
    maxLevel = ((level - m_levelLimit) + 1 < maxLevel)
                   ? (level - m_levelLimit) + 1
                   : maxLevel;
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
