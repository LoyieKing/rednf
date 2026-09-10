void CUser::sendCharacLinkMessage()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x122);
    packet.finalize(true);
    Send(packet);
