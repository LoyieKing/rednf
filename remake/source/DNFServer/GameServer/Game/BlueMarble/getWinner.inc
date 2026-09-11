int BlueMarble::getWinner() const
{
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        if (m_users[i].getGrade() == 0)
        {
            return i;
        }
    }
    return -1;
}