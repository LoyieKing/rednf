void CBattle_Field::BloodRoundIntervalPacket(short round, int interval)
{
    PacketGuard packet;
    packet.put_header(0, 0xd7);
    packet.put_byte((int)round + 1);
    packet.put_int(interval);
    packet.finalize(true);
    m_party->send_to_party(packet);
