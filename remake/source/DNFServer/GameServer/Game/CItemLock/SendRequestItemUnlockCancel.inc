// ORIG 0x8542026：包 0x111 = put_header(1,0x111)、byte(1)、byte(space)、
// short(slot)。
void CItemLock::SendRequestItemUnlockCancel(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x111);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}