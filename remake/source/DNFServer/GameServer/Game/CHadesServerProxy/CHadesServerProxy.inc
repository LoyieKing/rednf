CHadesServerProxy::CHadesServerProxy(char* ip, int tcpPort, int udpPort)
    : BaseServerProxy(ip, tcpPort, "HadesSvr"), m_udpPort(udpPort), m_connSec(0)
{
    HadesSvrInit();
