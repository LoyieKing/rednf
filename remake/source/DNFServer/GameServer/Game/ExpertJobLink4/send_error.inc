// send_error（报告 send_error.md，ORIG T 0x085d1e62）：
// header(0,0xcb)/byte(0)/byte(err)/finalize/Send。
void CDisjointer::send_error(CUser* user, int err)
{
    PacketGuard packet;
    packet.put_header(0, 0xcb);
    packet.put_byte(0);
    packet.put_byte(err);
    packet.finalize(true);
    user->Send(packet);
}