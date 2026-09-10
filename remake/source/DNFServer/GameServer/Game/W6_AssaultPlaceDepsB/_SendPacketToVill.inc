void CAssaultPlace::_SendPacketToVill(PacketGuard& packet)
{
    if (m_pos.m_x != 0)                         // +0x118 vill id 非零才发
        G_GameWorld()->send_to_vill(m_pos.m_x, packet);
