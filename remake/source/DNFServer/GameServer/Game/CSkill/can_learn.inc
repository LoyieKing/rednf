bool CSkill::can_learn(int level, int firstGrowType, int secondGrowType,
                       int targetLevel) const
{
    // b28d-37 [P194/X 族封顶，ORIG 0x83?]：ORIG `jge` 直跳 vs 4.4 `setl+test+je` 物化（int 比较物化不可翻）；107/115。
    int maxLevel;
    if (secondGrowType < 1)
    {
        if (targetLevel < 1 || get_type_max_level(firstGrowType) < targetLevel)
        {
            return false;
        }
        if (get_type_max_level(firstGrowType) <
            (level - m_levelLimit) + 1)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
        else
        {
            maxLevel = (level - m_levelLimit) + 1;
        }
    }
    else
    {
        maxLevel = get_second_growtype_max_level(firstGrowType,
                                                 secondGrowType);
        if (maxLevel == 0)
        {
            maxLevel = get_type_max_level(firstGrowType);
        }
        if (targetLevel < 1 || maxLevel < targetLevel)
        {
            return false;
        }
        if ((level - m_levelLimit) + 1 <= maxLevel)
            maxLevel = (level - m_levelLimit) + 1;
        else
            maxLevel = maxLevel;
    }
    if (maxLevel < targetLevel)
    {
        return false;
    }
    int needLevel = m_levelLimit + m_fieldC8 * (targetLevel - 1);
    return needLevel <= level;
