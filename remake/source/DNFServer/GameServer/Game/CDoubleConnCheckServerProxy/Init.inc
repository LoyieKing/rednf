bool CDoubleConnCheckServerProxy::Init()
{
    if (m_udpHandler.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_DoubleConnCheckServerProxy.cpp",
                              __FUNCTION__, 0x78,
                              "[CDoubleConnCheckServerProxy::Init] Can\'t init udp client socket\n");
        return false;
    }
    return true;
