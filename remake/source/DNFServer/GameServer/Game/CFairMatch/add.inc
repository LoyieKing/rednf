int TeamInfoThird::add(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!m_players[i].m_flag)
        {
            m_players[i].m_flag = true;
            m_players[i].m_user = user;
            m_players[i].m_kind.m_job = (unsigned char)user->get_charac_job();
            m_players[i].m_kind.m_grow = user->getCurCharacGrowType();
            m_players[i].m_pvpGrade = user->get_pvp_grade();
            m_players[i].m_rating = user->GetRating();
            return 1;
        }
    }
    return 0;
