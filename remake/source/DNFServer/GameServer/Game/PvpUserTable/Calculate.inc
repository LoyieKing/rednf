int PvpUserTable::Calculate()
{
    memset((char*)this, 0, 0x100);
    memset(m_teamPoint, 0, sizeof(m_teamPoint));
    CheckDan();

    for (int i = 0; i < 8; ++i)
    {
        SetWinTeam(i);
        for (int j = 0; j < 8; ++j)
        {
            if (i == j)
                goto skip_vs;
            GetTeamVsTeamPoint(i, j);
        skip_vs:;
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        m_minPoint[i] = 0.0f;
        for (int j = 0; j < 8; ++j)
        {
            m_minPoint[i] = std::min(m_minPoint[i], m_teamVsTeam[j][i]);
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            m_users[i][j].user = 0;
        }
    }
    return 1;
