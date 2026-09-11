bool CGuildServerProxy::OnTcpRecv()
{
    int n = m_tcp.recv_packet();
    if (n > 0)
    {
        if (Parsing(n) == false)
        {
            return 0;
        }
        return 1;
    }
    if (n < 0)
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0x71f,
                              "Guild Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}