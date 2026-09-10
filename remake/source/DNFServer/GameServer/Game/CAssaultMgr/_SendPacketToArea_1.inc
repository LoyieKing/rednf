void CAssaultPlace::_SendPacketToArea(PacketGuard& packet,
                                      std::vector<CUser*>& users)
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet, users);
