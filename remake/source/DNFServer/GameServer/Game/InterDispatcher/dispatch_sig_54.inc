int Inter_MonitorNoticeGuildMemberInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    // ORIG 0x084cc78f：包基址在 get_state 检查之后才物化入栈槽 -0xc(%ebp)
    Inter_MonitorNoticeGuildMemberInfoPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildMemberInfoPktView*>(data);
    if (pPkt->m_characNo !=       // msg+0x0e 角色号校验
        user->get_charac_no(-1))
        return 0;

    user->setGuildMemberGrade(                       // msg+0x27 成员等级
        pPkt->m_memberGrade);

    PacketGuard packet;
    packet.put_header(0, 0x7f);
    packet.put_byte(                                 // movzbl 零扩展
        static_cast<int>(pPkt->m_memberGrade));
    packet.finalize(true);
    user->Send(packet);

    user->send_skill_info();

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\x01');
    packet.finalize(true);
    user->Send(packet);
    return 0;
}