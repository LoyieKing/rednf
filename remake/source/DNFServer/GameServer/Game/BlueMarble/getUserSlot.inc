int BlueMarble::getUserSlot(CUser* user) const
{
    if (user == 0)
    {
        return -1;
    }
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == user)
        {
            return i;
        }
    }
    return -1;
}