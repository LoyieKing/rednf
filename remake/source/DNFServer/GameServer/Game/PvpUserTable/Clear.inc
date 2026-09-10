void PvpUserTable::Clear()
{
    for (int team = 0; team < 8; ++team)
    {
        for (int idx = 0; idx < 8; ++idx)
        {
            m_users[idx][team].user = 0;
            m_users[idx][team].flag = 0;
        }
    }
    m_winTeam = -1;
