void CRelayBattleMgr::OnChangeTeam(int seat, int team)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    m_battles[seat].m_team = (char)team;
