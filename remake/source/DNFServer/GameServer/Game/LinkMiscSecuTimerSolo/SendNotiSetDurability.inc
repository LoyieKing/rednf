void CAutoMarketConditionsControl::SendNotiSetDurability(int value)
{
    PacketGuard packet;
    MakeNotiSetDurability(packet, value);
    G_GameWorld()->send_all(packet);
