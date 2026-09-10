void CBattle_Field::sendUltimateRandomDifficulty(int a, int b, int c)
{
    PacketGuard packet;
    packet.put_header(0, 0x126);
    packet.put_byte(a);
    packet.put_byte(b);
    packet.put_short(c);
    packet.finalize(true);
    m_party->send_to_party(packet);
