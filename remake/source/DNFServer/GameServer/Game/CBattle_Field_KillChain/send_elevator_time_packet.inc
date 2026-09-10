void CBattle_Field::send_elevator_time_packet(char state)
{
    PacketGuard packet;
    packet.put_header(0, 0xfd);
    packet.put_byte(static_cast<int>(static_cast<char>(m_elevatorTimer)));
    packet.put_byte(static_cast<int>(state));
    packet.finalize(true);
    m_party->send_to_party(packet);
