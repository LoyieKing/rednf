// ORIG T 0x0846c7c4：连接态(+0x30df8)==2 时经 CNetwork::send_packet 下发；
// 否则 cMyTrace 报 disconnected（字符串 .rodata 直读转写）。
void CCommunityServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_udpPort != 2)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd4, 0);
        // [N5C_EventChar 2026-09-11] §6.5 串盲区真错（strall 审计发现）：ORIG .rodata
        // 串无句号，我方多一个 '.' ⇒ 运行期日志文本与 ORIG 不一致且 AE 口径不可见（此处修正）。
        trace("cannot send packet because Community Server is disconnected");
        return;
    }
    send_packet(data, len);
}