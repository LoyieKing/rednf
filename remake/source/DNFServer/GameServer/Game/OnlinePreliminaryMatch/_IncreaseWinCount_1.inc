void COnlinePreliminary::_IncreaseWinCount(ENUM_SCHOOL_MATCH_TEAM team)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[(int)team].m_seats[i].m_flag != 0 &&
            m_teams[(int)team].m_seats[i].m_user != 0)
        {
            _IncreaseWinCount(m_teams[(int)team].m_seats[i].m_user);
            break;
        }
    }
