BaseServerProxy::BaseServerProxy(char* ip, int port, char* name)
    : m_ip(ip), m_port(port), m_serverName(name)
{
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
}