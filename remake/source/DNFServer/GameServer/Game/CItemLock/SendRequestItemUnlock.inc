void CItemLock::SendRequestItemUnlock(CUser* user, int space, int slot,
                                      int remainSec)
{
    PacketGuard packet;
    packet.put_header(1, 0x110);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
