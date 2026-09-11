// ==== Dispatcher_UseDye::notifyUpdatedInventoryItemInfo (ORIG 0x081e70e6) ====
void Dispatcher_UseDye::notifyUpdatedInventoryItemInfo(CUser& user, short slot) const
{
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user.getCurCharacInvenR()->MakeItemPacket(
        static_cast<INVEN_TYPE>(1), slot, packet);
    packet.finalize(true);
    user.Send(packet);
}