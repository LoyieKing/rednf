// ORIG T 0x086384c2 (_ZN28TimerGeneratePowerUpNotiTime14send_noti_timeEi)
void TimerGeneratePowerUpNotiTime::send_noti_time(int sec)
{
    PacketGuard packet;
    packet.put_header(0, 0xdc);
    packet.put_int(sec);
    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}