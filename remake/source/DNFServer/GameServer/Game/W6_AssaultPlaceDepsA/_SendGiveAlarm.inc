// ---------------------------------------------------------------------------
// ORIG T 0x82e7ac4 (size 0x65)：_SendGiveAlarm() —— MakeGiveAlarm → SendPacket
// ---------------------------------------------------------------------------
void CAssaultPlace::_SendGiveAlarm()
{
    PacketGuard packet;
    _MakeGiveAlarm(&packet);
    SendPacket(packet);
}