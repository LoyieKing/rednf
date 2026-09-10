bool CHadesServerProxy::InitUdp()
{
    if (m_udpHandler.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_HadesServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0xd5,
                              "[CHadesServerProxy::Init] Can't init udp client socket");
        return false;
    }
    return true;
