void TeamInfoThird::refresh()
{
    int deadSlot = 0;  // ORIG -0x10 死槽（0x85624e5 movl $0x0 初始化后未读）
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user != 0)
            m_players[i].m_rating = m_players[i].m_user->GetRating();
        else
            m_players[i].m_rating = 0.0f;
    }
    m_field00 = 0;
    m_field04 = 2;
