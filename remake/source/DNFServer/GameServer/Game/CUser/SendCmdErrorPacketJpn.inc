bool CUser::SendCmdErrorPacketJpn(ENUM_CMDPACKET cmd, unsigned short error)  // b28d-lane5
{
    PacketGuard packet;
    ARAD::DISPATCHER::make_cmd_packetheader_jpn(packet, cmd, false);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
