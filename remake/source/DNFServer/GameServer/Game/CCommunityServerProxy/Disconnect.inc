void CCommunityServerProxy::Disconnect()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return;
    }
    else
    {
        disconnect();
        m_udpPort = (ENUM_SOCKET_STATE)0;   // 断开置状态 0
    }
