bool CUser::SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                   unsigned char error)  // b28d-lane5：ORIG 0x0867be6f 保存 4 参版返回值
{
    PacketGuard packet;
    return SendCmdPeerErrorPacket(cmd, type, error, packet);
