// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c110 | _ZN5CUser23SendCmdErrorPacket2ByteE14ENUM_CMDPACKETt
bool CUser::SendCmdErrorPacket2Byte(ENUM_CMDPACKET cmd, unsigned short error)  // b28d-lane5
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}