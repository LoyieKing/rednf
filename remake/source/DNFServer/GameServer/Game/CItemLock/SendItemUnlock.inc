// ORIG 0x85420e0：包 0xfc = put_header(0,0xfc)、byte(space)、short(slot)。
void CItemLock::SendItemUnlock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfc);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}