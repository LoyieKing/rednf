// ----------------------------------------------------------------------------
// 5. ORIG T 0x086886ce sz 0xf5。
// 通知消息包：put_header(0,0xc)/put_byte(type)/put_short(0)/put_byte(0)/
// put_int(strlen(msg))/put_str(msg,strlen(msg))/finalize(true)，Send 后析构
// （strlen 与 put_str 均按 ORIG 各调一次）。
// ----------------------------------------------------------------------------
void CUser::SendNotiPacketMessage(const char* msg, ENUM_MESSAGE_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(static_cast<int>(type));
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(strlen(msg)));
    packet.put_str(msg, static_cast<int>(strlen(msg)));
    packet.finalize(true);
    Send(packet);
}