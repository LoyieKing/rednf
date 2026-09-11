void CGuildServerProxy::SendTcpPacket(char* p, int len)
{
    if (GetTcpHandler()->get_socket() == -1)
    {
    }
    else
    {
        m_tcp.send_packet(p, len);
    }
    return;
}