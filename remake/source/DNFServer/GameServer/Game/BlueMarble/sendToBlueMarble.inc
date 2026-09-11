void BlueMarble::sendToBlueMarble(PacketGuard& guard) const
{
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        m_users[i].getUser()->Send(guard);
    }
}