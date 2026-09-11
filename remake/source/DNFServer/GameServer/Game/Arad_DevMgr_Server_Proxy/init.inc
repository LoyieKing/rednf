int Arad_DevMgr_Server_Proxy::init()
{
    if (m_network.m_sock < 0)
    {
        m_ip = "127.0.0.1";
        m_port = 30000;
        if (CNetwork<6144, 100000>::CreateConnectionSocket(m_network.m_sock, m_ip.c_str(), m_port) == 0)
        {
            return 0;
        }
    }
    return 1;
}