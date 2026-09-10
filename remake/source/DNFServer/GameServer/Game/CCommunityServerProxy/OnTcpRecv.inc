bool CCommunityServerProxy::OnTcpRecv()
{
    int recvLen = recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "DF_CommunityServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0xed,
                              "Community Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
