int CSkill::get_fitness_second_growtype(int secondGrowType) const
{
    if (secondGrowType < 1 || 2 < secondGrowType)
    {
        return -1;
    }
    return m_fitnessSecondGrowType[secondGrowType - 1];
