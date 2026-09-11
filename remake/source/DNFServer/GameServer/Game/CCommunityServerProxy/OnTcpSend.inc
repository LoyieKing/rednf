// ORIG T 0x0846c8c4：send_packet()<0 → 报错+Disconnect 返 -1；否则返 m_recvOffset。
int CCommunityServerProxy::OnTcpSend()
{
    if (send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x101, 0)
            ("CommunityServer OnSend() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        Disconnect();
        return -1;
    }
    return m_recvOffset;
}