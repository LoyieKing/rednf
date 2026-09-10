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
