bool CMission_move_channel::isClearable(MissionInfo& info, bool&) const
{
    if (info.m_streak == 0x10) return 1;
    if (m_targetChannel == (int)info.m_count) return 1;
    if ((m_targetChannel == 10) && (info.m_count == 3)) return 1;
    return 0;
