bool CGuildServerProxy::Parsing(int len)
{
    return m_tcp.Parsing_ServerPacket(len);
