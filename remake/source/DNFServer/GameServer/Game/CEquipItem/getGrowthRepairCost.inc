int CEquipItem::getGrowthRepairCost(int level) const
{
    if (m_growthFlag != 0)
    {
        return getLevelLinearAbility(m_growthA, m_growthB, m_growthE,
                                     m_growthF, level);
    }
    return m_repairCost;
