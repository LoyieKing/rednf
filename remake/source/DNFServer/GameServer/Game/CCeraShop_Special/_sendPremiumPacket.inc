// ============================================================================
// _sendPremiumPacket —— ORIG 0x083277dc (T)
// 组 0x42 高级会员购买通知包：header(0,0x42)/short(param2)/byte(param3)/int(param4)。
// ============================================================================
void CCeraShop::_sendPremiumPacket(CUser* user, int a, int type, int remainSec)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(a);
    packet.put_byte(type);          // ORIG 全宽装载（mov 0x14(%ebp)），禁 uchar cast（生 movzbl）
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}