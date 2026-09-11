void CSecretShop::SendSecretShopItemList(CUser* user, std::vector<SALE_INFO>& list)
{
    int count = (int)list.size();
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x115);
    ((InterfacePacketBuf*)&packet)->put_int(count);
    for (int i = 0; i < count; ++i) {
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_itemIdx);
        ((InterfacePacketBuf*)&packet)->put_byte(list[i].m_rule);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_price);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_limit);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_material);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_materialCount);
        ((InterfacePacketBuf*)&packet)->put_byte(list[i].m_field18);
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}