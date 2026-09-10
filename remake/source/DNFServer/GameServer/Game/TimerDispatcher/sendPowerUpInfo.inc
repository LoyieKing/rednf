void TimerGeneratePowerUp::sendPowerUpInfo(int team1Count, int team2Count)
{
    PacketGuard packet;
    packet.put_header(0, 0xdd);
    packet.put_int(team1Count);
    packet.put_int(team2Count);
    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
