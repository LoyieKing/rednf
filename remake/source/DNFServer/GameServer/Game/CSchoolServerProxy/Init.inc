bool CSchoolServerProxy::Init()
{
    if (m_udpHandler.InitClientSocket() == -1) {
        LogManager::logFormat(1, "DF_SchoolServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0x3c,
                              "[CSchoolServerProxy::Init] Can't init udp client socket\n");
        return false;
    }
    return true;
