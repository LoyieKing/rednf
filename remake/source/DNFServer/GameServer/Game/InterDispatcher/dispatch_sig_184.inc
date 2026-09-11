int Inter_NoticeLoadGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeLoadGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeLoadGuildAgitPktView*>(data);
    user->SetGuildAgitDBInfo(
        *reinterpret_cast<STGuildAgitDBInfo*>((char*)pPkt + 0x16));
    PacketGuard packet;
    packet.put_header(0, 0xbf);
    packet.put_byte(pPkt->m_16);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}