// ORIG T 0x08471a32：socketState==CONNECTED(2) 时经 m_tcp.send_packet 下发；
// 否则 cMyTrace 报 disconnected。
void CPCRoomServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace trace(__FUNCTION__, 0x138, 0);
        // [K8GlobalData 2026-09-11 · §6.5 串盲区] ORIG .rodata 0x8c6bd9c =
        // 'cannot send packet because PCRoom Server is disconnected'（**无句点**），
        // 0x08471a32 段 `movl $0x8c6bd9c,0x4(%esp)` 直证（另 0x8c6cc7c='SendTcpPacket'
        // 证明此处用 __FUNCTION__ 正确）。
        trace("cannot send packet because PCRoom Server is disconnected");
        return;
    }
    m_tcp.send_packet(data, len);
}