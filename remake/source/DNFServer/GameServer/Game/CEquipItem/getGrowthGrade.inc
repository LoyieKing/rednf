int CEquipItem::getGrowthGrade(int level) const
{
    if (m_growthFlag != 0)
    {
        return getLevelLinearAbility(m_growthA, m_growthB, m_growthC,
                                     m_growthD, level);
    }
    return m_grade;
