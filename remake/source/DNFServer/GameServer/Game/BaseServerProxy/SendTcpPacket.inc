bool BaseServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x15d, 0)
            ("cannot send packet because %s is disconnected", GetServerName().c_str());
        return 0;
    }
    if (m_tcpHandler.send_packet(data, len) < 0)
    {
        return 0;
    }
    return 1;
}