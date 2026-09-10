bool CMonitorServerProxy::InitUdp()
{
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_MonitorServerProxy.cpp", __PRETTY_FUNCTION__,
                              0x485,
                              "[CMonitorServerProxy::Init] Can\'t init udp client socket");
        return 0;
    }
    return 1;
