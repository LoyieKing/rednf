void CBattle_Field::BloodRoundMaxTimePacket(short round, int a, int b)
{
    PacketGuard packet;
    packet.put_header(0, 0xd6);
    packet.put_byte(0);
    packet.put_byte((int)round + 1);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    m_party->send_to_party(packet);
