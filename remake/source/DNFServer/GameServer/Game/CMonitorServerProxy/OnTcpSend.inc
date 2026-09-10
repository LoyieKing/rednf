int CMonitorServerProxy::OnTcpSend()
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6a3, 0)
            ("OnSend() Socket[%d] Error(%s)", GetTcpHandler()->get_socket(),
             strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.GetRemainSendSize();
