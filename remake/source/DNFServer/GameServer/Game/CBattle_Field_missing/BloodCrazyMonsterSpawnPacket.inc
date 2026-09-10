void CBattle_Field::BloodCrazyMonsterSpawnPacket()
{
    PacketGuard packet;
    packet.put_header(0, 0xd5);
    packet.put_int(m_bloodSpawnCntCur);
    packet.put_byte((int)(signed char)m_bloodCrazyType);
    packet.finalize(true);
    m_party->send_to_party(packet);
