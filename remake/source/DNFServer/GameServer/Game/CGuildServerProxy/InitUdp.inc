bool CGuildServerProxy::InitUdp()
{
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0x4d4,
                              "[CGuildServerProxy::Init] Can\'t init udp client socket\n");
        return 0;
    }
    return 1;
