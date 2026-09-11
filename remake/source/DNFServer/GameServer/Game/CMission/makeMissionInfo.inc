bool CMission::makeMissionInfo(MissionInfo& info, const CUser& user) const
{
    info.m_kind = (short)GetMissionKind();
    info.m_index = (short)GetMissionIndex();
    info.m_count = 0;
    info.m_streak = 0;
    _makeTotalMissionInfo(info, user);
    return 1;
}