// ORIG T 0x82e7986 _SendUserMiniInfo()
void CAssaultPlace::_SendUserMiniInfo()
{
    PacketGuard packet;
    _MakeUserInfo(&packet, static_cast<ENUM_USERINFO>(0));
    G_GameWorld()->send_all(packet);
}