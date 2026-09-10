void CHandlePremium::handleNotifyPremiumInfo(CUser* user, int type, int remainSec)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(2);
    packet.put_byte(type);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
