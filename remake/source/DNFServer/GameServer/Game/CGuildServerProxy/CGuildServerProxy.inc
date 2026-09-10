CGuildServerProxy::CGuildServerProxy(std::string ip, int udpPort, int tcpPort)
    : m_ip(ip), m_udpPort(udpPort), m_guildInfoMap(), m_tcpPort(tcpPort),
      m_tcp(), m_udp()
{
