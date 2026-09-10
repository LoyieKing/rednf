void CItemLock::SendItemUnlock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfc);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
