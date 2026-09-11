// ORIG T 0x82e8300 _MoveAssaultPlace()
void CAssaultPlace::_MoveAssaultPlace()
{
    PacketGuard packet;
    _MakeMoveAssaultPlace(&packet);
    SendPacket(packet);
}