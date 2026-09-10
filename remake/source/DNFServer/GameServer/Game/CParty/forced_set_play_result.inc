void CParty::forced_set_play_result()
{
    if ((unsigned char)m_state == 3)                    // +0x6a
        return;
    PacketGuard packet;
    packet.put_header(0, 0x22);
    packet.put_byte(0);
    packet.put_int(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.finalize(true);
    send_to_party(packet);
