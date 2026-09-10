bool CStatisticServerProxy::Init()
{
    if (m_port == 0)
        return 1;
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_StatisticServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0x267,
                              "[CStatisticServerProxy::Init] Can't init udp client socket\n");
        return 0;
    }
    return 1;
