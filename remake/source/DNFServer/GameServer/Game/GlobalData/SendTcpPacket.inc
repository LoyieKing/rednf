void CPCRoomServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace trace(__FUNCTION__, 0x138, 0);
        trace("cannot send packet because PCRoom Server is disconnected.");
        return;
    }
    m_tcp.send_packet(data, len);
