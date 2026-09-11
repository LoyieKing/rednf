int PvP_Room::change_team(int seat, int team)
{
    lock();
    P_TEAMS(seat) = team;
    m_relayMgr.OnChangeTeam(seat, team);
    unlock();
}