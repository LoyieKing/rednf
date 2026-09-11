// ORIG T 0x08a9d7a6：m_skillIdx<=level<=m_value 区间判定。
bool TagSkillPenalty::isPenaltyLevel(int level)
{
    if (m_skillIdx <= level && level <= m_value)
        return true;
    return false;
}