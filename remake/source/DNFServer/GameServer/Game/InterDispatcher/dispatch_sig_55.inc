int Inter_MonitorNoticeChangeGuildMemberGrade::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG +0x4c：包基址槽 -0xc 的写入点在 get_state 守卫之后（声明随之下移）。
    Inter_MonitorNoticeChangeGuildMemberGradePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeChangeGuildMemberGradePktView*>(data);
    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0x8c);
    packet.put_byte(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}