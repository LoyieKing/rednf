void BaseServerProxy::Disconnect()
{
    cMyTrace(__PRETTY_FUNCTION__, 0x148, 0)
        ("BaseServerProxy::Disconnect(), ServerName:%s,SocketState:%d,IP:%s,Port:%d,Socket:%d",
         GetServerName().c_str(), m_socketState, m_ip.c_str(), m_port,
         m_tcpHandler.get_socket());
    if (m_ip.length() == 0 || m_port == 0)
    {
        return;
    }
    m_tcpHandler.disconnect();
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
    OnDisconnect();
