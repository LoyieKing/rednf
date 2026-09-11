// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步 | ORIG 0x0867bfae | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKEThR11PacketGuard
bool CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PacketGuard& packet)
{
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}