bool CMission_combo_clear::isClearable(MissionInfo& info, bool&) const
{
    return (info.m_count == 0) && (info.m_streak == 0);
