bool BaseServerProxy::InitTcp(bool bSetNonBlock)
{
    if (m_ip.length() == 0 || m_port == 0)
    {
        return 0;
    }
    if (m_tcpHandler.m_sock < 0)
    {
        int ret;
        if (bSetNonBlock)
        {
            if ((ret = m_tcpHandler.CreateConnectionSocket(m_tcpHandler.m_sock, m_ip.c_str(),
                                                           m_port)) != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x134, 0)
                    ("%s CreateConnectionSocket() error(%d:%d:%s)",
                     GetServerName().c_str(), ret, *__errno_location(),
                     strerror(*__errno_location()));
                return 0;
            }
        }
        else
        {
            if ((ret = m_tcpHandler.CreateConnectionBlockSocket(m_tcpHandler.m_sock,
                                                                m_ip.c_str(), m_port)) != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x13d, 0)
                    ("%s CreateConnectionSocket() error(%d:%d:%s)",
                     GetServerName().c_str(), ret, *__errno_location(),
                     strerror(*__errno_location()));
                return 0;
            }
        }
    }
    return 1;
