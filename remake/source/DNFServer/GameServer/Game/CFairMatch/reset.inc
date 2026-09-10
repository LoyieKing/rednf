void TeamInfoThird::reset()
{
    for (int i = 0; i < 4; ++i)
        m_players[i].reset();
    m_field00 = 0;
    m_field04 = 2;
