// ORIG T 0x0846c770：连接有效（ip 非空且端口非 0）时 disconnect + 清 udpPort。
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
}