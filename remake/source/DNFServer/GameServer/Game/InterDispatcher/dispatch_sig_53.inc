// [b28d-lane2] verdict=DIFF ndiff=102（ORIG 0x084cc35c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=95（ORIG 0x84cc35c）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_MonitorSetSubGuildMasterReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_MonitorSetSubGuildMasterReplyPktView* pPkt = reinterpret_cast<Inter_MonitorSetSubGuildMasterReplyPktView*>(data);
    if (pPkt->m_12 !=        // msg+0x12 characNo
        user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x81);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(pPkt->m_a);
        const int msgLen = (int)strlen(pPkt->m_17);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_17, msgLen);
        packet.put_byte(
            (int)pPkt->m_16);

        // ORIG 084cc473/084cc480：newGrade/oldGrade 落 **4 字节**槽（`mov %eax,-0x40`、
        //   `mov %eax,-0x3c`）⇒ 原码局部类型是 unsigned int（旧形 unsigned char 只发
        //   `mov %al,<槽>`，且多出槽往返）；初值带 `(unsigned char)` cast
        //   （ORIG 084cc470/084cc47d `movzbl %al,%eax` 二次截断）。
        unsigned int newGrade = (unsigned char)pPkt->m_16;   // msg+0x16
        unsigned int oldGrade = (unsigned char)pPkt->m_35;   // msg+0x35
        // ORIG 084cc486/084cc48c：grade→%edi、characNo→%esi 均在寄存器（无槽）⇒ 原码内联实参；
        //   ORIG 084cc4a9 receiver `mov 0xc(%ebp),%eax; add $0x79700,%eax` 直算亦无槽 ⇒ hist 无具名（P0）。
        user->m_historyLog.GuildChangeGrade(
            (unsigned int)user->get_charac_guildkey(),
            static_cast<const char*>(user->GetGuildDBInfo()),
            (unsigned int)pPkt->m_12,
            pPkt->m_36,
            oldGrade, newGrade);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}