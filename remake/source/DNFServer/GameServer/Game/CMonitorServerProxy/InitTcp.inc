bool CMonitorServerProxy::InitTcp()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = m_tcp.CreateConnectionSocket(m_tcp.m_sock, m_ip.c_str(),
                                               m_tcpPort);
        while (ret != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x49b, 0)
                ("CMonitorServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4a0, 9, true, true)
        ("[!] Connect To Monitor Server ... ");
    return 1;
