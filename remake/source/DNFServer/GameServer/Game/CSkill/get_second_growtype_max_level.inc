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
