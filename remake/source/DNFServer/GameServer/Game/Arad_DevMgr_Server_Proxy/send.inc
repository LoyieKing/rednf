int Arad_DevMgr_Server_Proxy::send(char* data, int len)
{
    if (m_network.send_packet() < 0)
    {
        return -1;
    }
    return m_network.GetRemainSendSize();
}