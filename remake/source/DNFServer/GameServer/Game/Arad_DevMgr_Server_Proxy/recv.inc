int Arad_DevMgr_Server_Proxy::recv()
{
    int size = m_network.recv_packet();
    if (size > 0)
    {
        if (!m_network.Parsing_ServerPacket(size))
        {
            LogManager::logFormat(1, "localjapan/Arad_DevMgr_Server_Proxy.cpp", __FUNCTION__, 0x34, "Monitor Recv Parsing Err!!!");
            return 0;
        }
        return 1;
    }
    if (size <= 0)
    {
        disconnect();
        return 0;
    }
    return 1;
}