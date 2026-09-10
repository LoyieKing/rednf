void CParty::SendCardSelectComplete()
{
    PacketGuard packet;
    packet.put_header(0, 0x43);
    packet.finalize(true);
    send_to_party(packet);
