// ============================================================================
// _sendCargoUpgradePacket —— ORIG 0x08327726 (T)
// 组 0x42 仓库扩容包：header(0,0x42) / short(type) / short(GetCapacity)。
// ============================================================================
void CCeraShop::_sendCargoUpgradePacket(CUser* user, int type)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    packet.put_short(user->getCurCharacCargoR()->GetCapacity());  // ORIG 无具名槽（P0 右值内联直传）
    packet.finalize(true);
    user->Send(packet);
}

