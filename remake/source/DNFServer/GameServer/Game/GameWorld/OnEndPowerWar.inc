void GameWorld::OnEndPowerWar(PacketGuard& packet)
{
    int var0 = 0;
    std::map<unsigned short, CUser*>::iterator it;
    int var1 = 0;
    packet.put_byte(GlobalData::s_power_manager->GetWinnerSide());
    packet.put_short(0);
