// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x08692824 | _ZN5CUser21SendCmdErrorPacketJpnE14ENUM_CMDPACKETt
bool CUser::SendCmdErrorPacketJpn(ENUM_CMDPACKET cmd, unsigned short error)  // b28d-lane5
{
    PacketGuard packet;
    ARAD::DISPATCHER::make_cmd_packetheader_jpn(packet, cmd, false);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}