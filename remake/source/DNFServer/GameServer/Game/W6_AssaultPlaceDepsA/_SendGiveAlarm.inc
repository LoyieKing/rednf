void CAssaultPlace::_SendGiveAlarm()
{
    PacketGuard packet;
    _MakeGiveAlarm(&packet);
    SendPacket(packet);
