int Inter_NoticeLoadGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeLoadGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeLoadGuildAgitPktView*>(data);
    user->SetGuildAgitDBInfo(
        pPkt->m_agit);
    PacketGuard packet;
    packet.put_header(0, 0xbf);
    packet.put_byte(static_cast<unsigned char>(pPkt->m_agit.m_pad[0]));
    packet.finalize(true);
    user->Send(packet);
    return 0;
}