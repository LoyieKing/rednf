void CAssaultPlace::_SendUserUdpIpPort()
{
    PacketGuard packet;
    _MakeUserUdpIpPort(&packet);
    SendPacket(packet);
