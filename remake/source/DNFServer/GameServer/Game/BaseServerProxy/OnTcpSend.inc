int BaseServerProxy::OnTcpSend()
{
    int sendRet = m_tcpHandler.send_packet();
    if (sendRet < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x18f, 0)
            ("%s OnSend() Socket[%d] Error(%s)", GetServerName().c_str(),
             GetTcpHandler()->get_socket(), strerror(*__errno_location()));
        cMyTrace(__PRETTY_FUNCTION__, 0x192, 0)
            ("Disconnect to %s (send_packet() returns minus value)", GetServerName().c_str());
        Disconnect();
        return -1;
    }
    return m_tcpHandler.m_recvOffset;
}