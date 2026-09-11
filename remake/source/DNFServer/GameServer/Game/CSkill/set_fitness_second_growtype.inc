void CSkill::set_fitness_second_growtype(int secondGrowType, int value)
{
    if (secondGrowType <= 0)
        return;
    if (secondGrowType > 2)
        return;
    m_fitnessSecondGrowType[secondGrowType - 1] = value;
}