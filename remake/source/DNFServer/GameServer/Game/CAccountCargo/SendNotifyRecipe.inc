void CAccountCargo::SendNotifyRecipe(CUser* user, int slot, bool flag)
{
    PacketGuard packet;
    // ORIG：无 buf 槽，每调用点重新 lea packet 地址（P0 右值直传）。
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    if (flag) {
        ((InterfacePacketBuf*)&packet)->put_short(2);
        ((CInventory*)user->getCurCharacInvenR())->MakeItemPacket(INVEN_TYPE_EQUIP, slot, packet);
    } else {
        ((InterfacePacketBuf*)&packet)->put_short(1);
    }
    ((CInventory*)user->getCurCharacInvenR())->MakeItemPacket(INVEN_TYPE_EQUIP, 0, packet);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}