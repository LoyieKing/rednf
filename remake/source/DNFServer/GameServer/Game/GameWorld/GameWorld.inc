GameWorld::GameWorld()
    : m_mutex(), m_obsCrash(), m_idMap(), m_shusia(), m_pvp(), m_natType(),
      m_UserInWorld(), m_sessionList(), m_loginUserMap()
{
    m_pVill = NULL;
    m_obsCrash.reset();
    for (int i = 0; i <= 10; i++)
    {
        m_channelUserCount[i] = 0;
    }
    m_userCount = 0;
    m_hangameUserCount = m_userCount;
