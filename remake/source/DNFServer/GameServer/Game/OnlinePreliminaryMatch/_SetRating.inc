void COnlinePreliminary::_SetRating(ENUM_SCHOOL_MATCH_TEAM team, float v)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[(int)team].m_seats[i].m_flag != 0 &&
            m_teams[(int)team].m_seats[i].m_user != 0)
        {
            _SetRating(m_teams[(int)team].m_seats[i].m_user, v);
            break;
        }
    }
