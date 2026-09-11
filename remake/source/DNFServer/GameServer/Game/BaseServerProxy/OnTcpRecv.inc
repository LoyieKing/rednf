bool BaseServerProxy::OnTcpRecv()
{
    int recvLen = m_tcpHandler.recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x16f, 0)
                ("%s occur Error at parsing receive packet", GetServerName().c_str());
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "BaseServerProxy.cpp", __PRETTY_FUNCTION__,
                              0x17a, "%s maybe Die!!!", GetServerName().c_str());
        Disconnect();
        return 0;
    }
    return 1;
}