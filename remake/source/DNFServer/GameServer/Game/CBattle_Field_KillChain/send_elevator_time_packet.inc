// ============================================================================
// send_elevator_time_packet —— ORIG T 0x0830d848。
// 组 0xfd 包：byte(m_elevatorTimer +0x139)、byte(state)；finalize 后 send_to_party。
// ============================================================================
void CBattle_Field::send_elevator_time_packet(char state)
{
    PacketGuard packet;
    packet.put_header(0, 0xfd);
    packet.put_byte(static_cast<int>(static_cast<char>(m_elevatorTimer)));
    packet.put_byte(static_cast<int>(state));
    packet.finalize(true);
    m_party->send_to_party(packet);
}