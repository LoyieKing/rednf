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