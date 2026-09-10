void CCommunityServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_udpPort != 2)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd4, 0);
        trace("cannot send packet because Community Server is disconnected.");
        return;
    }
    send_packet(data, len);
