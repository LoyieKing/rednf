bool CPCRoomServerProxy::OnTcpRecv()
{
    int recvLen = m_tcp.recv_packet();
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
        LogManager::logFormat(1, "DF_PCRoomServerProxy.cpp",
                              __FUNCTION__, 0x155,
                              "PCRoom Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
