void CPCRoomServerProxy::Disconnect()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return;
    }
    else
    {
        m_tcp.disconnect();
        m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
    }
