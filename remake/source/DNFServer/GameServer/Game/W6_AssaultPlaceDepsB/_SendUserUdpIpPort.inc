// ORIG T 0x82e7a5e _SendUserUdpIpPort()
void CAssaultPlace::_SendUserUdpIpPort()
{
    PacketGuard packet;
    _MakeUserUdpIpPort(&packet);
    SendPacket(packet);
}