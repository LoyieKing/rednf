int Inter_MonitorNoticeMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorNoticeMessagePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMessagePktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x6e);
    packet.put_byte(1);
    packet.put_byte(1);
    if (pPkt->m_len == '\0')
        return 0x2e82;                                 // 无公告内容，包废弃

    packet.put_int((unsigned int)(unsigned char)pPkt->m_len);
    packet.put_str(pPkt->m_msg,
                   (unsigned int)(unsigned char)pPkt->m_len);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}