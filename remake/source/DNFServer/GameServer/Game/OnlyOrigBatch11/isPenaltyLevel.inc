bool TagSkillPenalty::isPenaltyLevel(int level)
{
    if (m_skillIdx <= level && level <= m_value)
        return true;
    return false;
