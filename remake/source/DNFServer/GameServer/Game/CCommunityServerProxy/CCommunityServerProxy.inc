// ORIG T 0x0846c5c8：基类构造 + string(ip)；m_tcpPort/m_udpPort 在函数体赋值
//（报告汇编：string 构造后先写 +0x30e00 再清 +0x30df8）。
CCommunityServerProxy::CCommunityServerProxy(char* ip, int port)
    : CNetwork<100000, 100000>(), m_ip(ip)
{
    m_tcpPort = port;
    m_udpPort = (ENUM_SOCKET_STATE)0;   // +0x30df8 实为 socket 状态（GDataC6 enum 化），初值 disconnected
}