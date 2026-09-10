void Inter_MonitorServerEventStart::SendEventNotiPacket(bool noti,
                                                        short eventIdx,
                                                        short interval,
                                                        CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x45);
    packet.put_byte(noti);
    packet.put_short((int)eventIdx);
    packet.put_short((int)interval);
    packet.finalize(true);
    if (user == 0)
        G_GameWorld()->send_all(packet);
    else
        user->Send(packet);
