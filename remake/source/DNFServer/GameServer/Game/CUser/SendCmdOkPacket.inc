// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bea0 | _ZN5CUser15SendCmdOkPacketE14ENUM_CMDPACKET
bool CUser::SendCmdOkPacket(ENUM_CMDPACKET cmd)  // b28d-lane5：ORIG 0x0867bf10 `call Send; mov %eax,%ebx`
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}