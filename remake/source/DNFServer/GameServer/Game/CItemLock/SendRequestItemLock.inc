// ORIG 0x8541ea0：包 0x10f = put_header(1,0x110 族第 1 条)、byte(1)、
// byte(space)、short(slot)。注：ORIG put_header 第二参 0x10f。
void CItemLock::SendRequestItemLock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x10f);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}