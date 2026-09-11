// ORIG T 0x82e6d3a：m_pos.x/y 非零才向所在区域广播。
void CAssaultPlace::_SendPacketToArea(PacketGuard& packet)
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet);
}