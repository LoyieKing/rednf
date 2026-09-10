void CAssaultPlace::_SendUserInfo()
{
    PacketGuard packet;
    _MakeUserInfo(&packet, (ENUM_USERINFO)1);
    SendPacket(packet);
