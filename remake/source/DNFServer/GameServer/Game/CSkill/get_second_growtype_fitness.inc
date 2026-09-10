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
