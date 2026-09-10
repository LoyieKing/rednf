void CAssaultPlace::_SendUserState()
{
    PacketGuard packet;
    _MakeUserState(&packet);
    G_GameWorld()->send_all(packet);
