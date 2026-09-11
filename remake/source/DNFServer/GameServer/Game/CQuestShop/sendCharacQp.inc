void CQuestShop::sendCharacQp(CUser* user, char flag)
{
    PacketGuard packet;
    packet.put_header(0, 0x15c);
    packet.put_int(getQp());
    packet.put_short(getInitCount());
    packet.put_byte(static_cast<int>(flag));
    packet.finalize(true);
    user->Send(packet);
}