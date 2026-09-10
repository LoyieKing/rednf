void CParty::send_to_party_cmd_error(ENUM_CMDPACKET cmd, unsigned char error)
{
    PacketGuard packet;
    packet.put_header(1, cmd);
    packet.put_byte(0);
    packet.put_byte(error);
    packet.finalize(true);
    send_to_party(packet);
