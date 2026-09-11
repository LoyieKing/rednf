// ORIG T 0x084719de：连接有效（ip 非空且端口非 0）时 disconnect + 清 socketState。
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
}