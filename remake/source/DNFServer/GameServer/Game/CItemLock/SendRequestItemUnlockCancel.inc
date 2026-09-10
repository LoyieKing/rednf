void CItemLock::SendRequestItemUnlockCancel(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x111);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
