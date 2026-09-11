// ---- 主逻辑（ORIG strong）----
// ORIG 0x83504ec _ZNK6CSkill11IsStealableEii：体内 `xor $0x1,%eax; test %al,%al` bool 物化形
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
}