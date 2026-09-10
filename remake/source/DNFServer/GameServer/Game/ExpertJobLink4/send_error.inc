void CDisjointer::send_error(CUser* user, int err)
{
    PacketGuard packet;
    packet.put_header(0, 0xcb);
    packet.put_byte(0);
    packet.put_byte(err);
    packet.finalize(true);
    user->Send(packet);
