void PvpUserTable::SetWinTeam(int team)
{
    m_winTeam = team;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (i == team)
                m_users[i][j].flag = 1;
            else
                m_users[i][j].flag = 0;
        }
    }
