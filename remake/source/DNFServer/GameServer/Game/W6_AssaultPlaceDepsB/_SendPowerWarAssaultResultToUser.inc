// ORIG T 0x82ec808 _SendPowerWarAssaultResultToUser(CUser*, int)
void CAssaultPlace::_SendPowerWarAssaultResultToUser(CUser* user, int point)
{
    PacketGuard packet;
    packet.put_header(0, 200);
    packet.put_byte(point);
    packet.finalize(true);
    user->Send(packet);
}