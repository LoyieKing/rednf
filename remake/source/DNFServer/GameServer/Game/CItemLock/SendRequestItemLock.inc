void CItemLock::SendRequestItemLock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x10f);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
