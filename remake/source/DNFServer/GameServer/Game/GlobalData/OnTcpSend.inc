// ORIG T 0x08471b32：m_tcp.send_packet()<0 → 报错+Disconnect 返 -1；
// 否则返 m_recvOffset（+0x18）。
int CPCRoomServerProxy::OnTcpSend()
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace(__FUNCTION__, 0x16d, 0)
            ("PCRoom OnSend() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.m_recvOffset;
}