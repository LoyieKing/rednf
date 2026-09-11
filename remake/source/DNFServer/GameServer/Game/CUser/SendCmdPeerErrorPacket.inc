// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bd84 | _ZN5CUser22SendCmdPeerErrorPacketE14ENUM_CMDPACKET22ENUM_PEER_REQUEST_TYPEh
bool CUser::SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                   unsigned char error)  // b28d-lane5：ORIG 0x0867be6f 保存 4 参版返回值
{
    PacketGuard packet;
    return SendCmdPeerErrorPacket(cmd, type, error, packet);
}