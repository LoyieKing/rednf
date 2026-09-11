// ---------------------------------------------------------------------------
// ORIG T 0x82e7918 (size 0x6d)：_SendUserInfo() —— UserInfo(USERINFO==1) 广播
// ---------------------------------------------------------------------------
void CAssaultPlace::_SendUserInfo()
{
    PacketGuard packet;
    _MakeUserInfo(&packet, (ENUM_USERINFO)1);
    SendPacket(packet);
}