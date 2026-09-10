CCommunityServerProxy::CCommunityServerProxy(char* ip, int port)
    : CNetwork<100000, 100000>(), m_ip(ip)
{
    m_tcpPort = port;
    m_udpPort = (ENUM_SOCKET_STATE)0;   // +0x30df8 实为 socket 状态（GDataC6 enum 化），初值 disconnected
