int CMission_winning_streak::Update(
    MissionInfo& info, const MissionClearCondition_Parameter& param) const
{
    if (param.m_victory & 1)
    {
        --info.m_streak;
        if (info.m_streak < 1)
        {
            ++info.m_count;
            info.m_streak = (short)m_streak;
        }
    }
    else
    {
        info.m_streak = (short)m_streak;
    }
    return 1;
}