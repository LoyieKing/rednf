// ORIG T 0x0846c9ec：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
bool CCommunityServerProxy::Parsing(int recvLen)
{
    return Parsing_ServerPacket(recvLen);
}