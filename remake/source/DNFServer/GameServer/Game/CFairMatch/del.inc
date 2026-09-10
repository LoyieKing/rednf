int TeamInfoThird::del(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user == user)
        {
            m_players[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
