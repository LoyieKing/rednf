int PvpUserTable::GetLeavePenalty(int team)
{
    if (!IsValidTeam(team))
    {
        return -1;
    }
    return (int)(m_minPoint[team] * 1000.0f);
