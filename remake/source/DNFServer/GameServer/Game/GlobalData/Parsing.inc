// ORIG T 0x08471c5a：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
bool CPCRoomServerProxy::Parsing(int recvLen)
{
    return m_tcp.Parsing_ServerPacket(recvLen);
}