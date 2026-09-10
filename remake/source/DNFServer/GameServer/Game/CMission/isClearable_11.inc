bool CMission_within::isClearable(MissionInfo& info, bool& clear) const
{
    short count = 0;
    for (int i = 0; i < m_round; i++)
    {
        if (((int)info.m_count >> i) & 1) count++;
    }
    if (count >= m_clear) return 1;
    if ((m_round - (int)info.m_streak) + count < m_clear)
    {
        info.m_count = 0;
        info.m_streak = 0;
        clear = true;
    }
    return 0;
