int PvpUserTable::GetTeamUserCount(int team)
{
    if (!IsValidTeam(team))
    {
        return -1;
    }
    int count = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (m_users[team][i].user != 0)
        {
            ++count;
        }
    }
    return count;
