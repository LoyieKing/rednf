int BlueMarble::getWaitUserCount() const
{
    int count = 0;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            ++count;
        }
    }
    return count;
}