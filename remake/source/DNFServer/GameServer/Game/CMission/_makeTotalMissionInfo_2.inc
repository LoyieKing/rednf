void CMission_winning_streak::_makeTotalMissionInfo(MissionInfo& info,
                                                    const CUser&) const
{
    info.m_streak = (short)m_streak;
}