// SendChangeTradeState（本 TU 实现，ORIG 0x852f0a0）
void SendChangeTradeState(PacketGuard& packet, CUser* user, CUser* target,
                          ENUM_TRADESTATE state)
{
    // ORIG 0x852f0a0：清空 → 头部(0, 0x11) → unique_id(short) → state(byte)
    // → finalize(true) → 双方各 Send 一次。
    packet.clear();
    packet.put_header(0, 0x11);
    packet.put_short(user->get_unique_id());
    packet.put_byte(state);
    packet.finalize(true);
    user->Send(packet);
    target->Send(packet);
}