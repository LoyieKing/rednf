int BlueMarble::join(CUser* user)
{
    if (user == 0)
    {
        return 0x17;
    }
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            m_users[i].setUser(user);
            m_users[i].getUser()->setBlueMarbleIndex((short)getMemoryPoolIndex());
            m_users[i].getUser()->setMoveSpace((UserSpace::T)1);
            break;
        }
    }
    G_GameWorld()->gotoBlueMarble(user);
    sendBlueMarbleRoomInfo();
    return 0;
}