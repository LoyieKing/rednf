int BlueMarble::sendBlueMarbleRoomInfo() const
{
    PacketGuard packet;
    packet.put_header(0, 0x1ae);
    packet.put_short(getMemoryPoolIndex());
    packet.put_byte(getWaitUserCount());
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        packet.put_byte(i);
        packet.put_short((unsigned short)m_users[i].getUser()->get_unique_id());
    }
    packet.finalize(true);
    sendToBlueMarble(packet);
}