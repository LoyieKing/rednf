void IBuyRule::SendSecretShopBuyItem(CUser* user, int slot, Inven_Item& item,
                                     int a, int b, int c)
{
    // [b28d-solo-CSecretShop] P183/P0：ORIG 0x085fb566 每处调用都重算
    // `lea -0x14(%ebp),%eax`（无 buf/inven 具名局部，无槽往返）⇒ 全表达式内联，
    // 不缓存 InterfacePacketBuf*/CInventory*。PacketGuard 槽保持 -0x14。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, 0x129);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_int(
        ((CInventory*)user->getCurCharacInvenR())->get_money());
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    ((InterfacePacketBuf*)&packet)->put_int(item.m_addInfo);
    ((InterfacePacketBuf*)&packet)->put_int(item.m_addInfo2);
    ((InterfacePacketBuf*)&packet)->put_byte(item.m_amp.getAbilityType() & 0xff);
    ((InterfacePacketBuf*)&packet)->put_short(item.m_amp.getAbilityValue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_int(a);
    ((InterfacePacketBuf*)&packet)->put_int(b);
    ((InterfacePacketBuf*)&packet)->put_int(c);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}