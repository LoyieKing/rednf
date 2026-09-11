int BlueMarble::sendUpdateItemInfo(int seat, int idx) const
{
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    // ORIG 0x080d7exx：getCurCharacInvenR 结果免槽直入 this（%eax 跨 3 条 mov 保活，
    // P0 右值内联 + 范式37）。
    ((CInventory*)m_users[seat].getUser()->getCurCharacInvenR())
        ->MakeItemPacket(INVEN_TYPE_EQUIP, idx, packet);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
}