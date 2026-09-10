int Inter_AchieveGuildAttendance::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AchieveGuildAttendancePktView* pPkt = reinterpret_cast<Inter_AchieveGuildAttendancePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG 0x084e4214 `mov 0xa(%eax),%ebx`：register 直落 callee-saved（cmp 方向 N22）。
    register int charIdMsg =                           // recv charId @ msg+0xa
        pPkt->m_a;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    // ORIG 0x084e425b/0x084e4270：m_12/m_16 各自从 -0xc reload（PG 生命周期内无缓存）。
    PacketGuard packet;
    packet.put_header(0, 0x134);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_16);
    packet.finalize(true);
    user->Send(packet);
    return 0;
