// ORIG T 0x82e79f6：_MakeUserState 后 G_GameWorld 全服广播。
void CAssaultPlace::_SendUserState()
{
    PacketGuard packet;
    _MakeUserState(&packet);
    G_GameWorld()->send_all(packet);
}