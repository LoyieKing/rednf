bool CHadesServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_ServerPacket(recvLen);
}