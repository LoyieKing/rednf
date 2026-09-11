// SendNotiSetDurability(int)  ORIG T 0x082f8540：广播耐久度比率包。
void CAutoMarketConditionsControl::SendNotiSetDurability(int value)
{
    PacketGuard packet;
    MakeNotiSetDurability(packet, value);
    G_GameWorld()->send_all(packet);
}