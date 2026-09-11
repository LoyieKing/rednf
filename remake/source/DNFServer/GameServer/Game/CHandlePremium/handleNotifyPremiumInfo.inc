// ---- handleNotifyPremiumInfo —— ORIG 0x086af8b4 (T) ----
// 组 0x42 高级会员通知包：header(0,0x42)/short(2)/byte(type)/int(remainSec)。
void CHandlePremium::handleNotifyPremiumInfo(CUser* user, int type, int remainSec)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(2);
    packet.put_byte(type);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}