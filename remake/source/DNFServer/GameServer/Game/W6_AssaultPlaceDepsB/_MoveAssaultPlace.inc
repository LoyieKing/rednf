void CAssaultPlace::_MoveAssaultPlace()
{
    PacketGuard packet;
    _MakeMoveAssaultPlace(&packet);
    SendPacket(packet);
