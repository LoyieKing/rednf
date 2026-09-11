// ============================================================================
// _sendInventoryUpgradePacket —— ORIG 0x083275be (T)
// 组 0x42 背包扩容包：header(0,0x42) / short(type) / short(get_inventory_capacity()).
// ============================================================================
void CCeraShop::_sendInventoryUpgradePacket(CUser* user, int type)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    packet.put_short(user->getCurCharacInvenR()->get_inventory_capacity());  // ORIG 无具名槽（P0）
    packet.finalize(true);
    user->Send(packet);
}