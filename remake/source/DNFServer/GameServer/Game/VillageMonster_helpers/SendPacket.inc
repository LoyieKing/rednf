void CVillageMonsterArea::SendPacket(PacketGuard& packet)
{
    G_GameWorld()->send_to_area(m_field28, m_field2c, packet);
