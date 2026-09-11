bool CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error)  // b28d-lane5
{
    PacketGuard packet;
    return SendCmdErrorPacket(cmd, error, packet);
}