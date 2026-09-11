// [b28d-lane2] verdict=DIFF ndiff=156（ORIG 0x084cd5ca）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84cd5ca）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [b28d-SweepInterDispatcher2] verdict=AE（ORIG 0x084cd5ca，176=176，TU 121/1331/5/133）：真修三处——
//   ① 删 `const unsigned char type` 具名局部，m_a 守卫拆两段独立（P133/P45：`if(m_a==2)return 0; if(m_a==3||m_a==4)return 0;`）；
//   ② m_b/m_c char→unsigned char 消 `movsbl`（ORIG `movzbl %al`）；③ m_11 int→unsigned int：比较点出 `mov %eax,%edx` 中转（P211/P265）
//   且 set_charac_memberkey 实参单段 `mov 0x11(%eax),%edx`（P265 unsigned 恒单段）。
int Inter_MonitorNoticeMemberEnterReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeMemberEnterReplyPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberEnterReplyPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_11)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x28d9,
            "MEMBER : Inter_MonitorNoticeMemberEnterReply::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_11);
        return 0;
    }

    if (pPkt->m_a == '\x02')
        return 0;
    if (pPkt->m_a == '\x03' || pPkt->m_a == '\x04')
        return 0;

    if (pPkt->m_b == '\x01' ||
        pPkt->m_b == '\x02')
    {
        if (pPkt->m_b == '\x01' &&
            pPkt->m_c == '\x01')
        {
            user->set_uppermember_exp_level(
                pPkt->m_34);
            user->set_connect_upper_member(
                pPkt->m_35);
            CParty* local_10 = (CParty*)user->GetParty();
            if (local_10 != 0)
                local_10->send_party_realtime_info();
        }
        user->set_charac_memberkey(
            pPkt->m_11);
    }

    PacketGuard packet;
    packet.put_header(1, 0x50);
    packet.put_byte(1);
    packet.put_byte(
        (unsigned int)pPkt->m_b);
    packet.put_byte(
        (unsigned int)pPkt->m_15);
    const int local_14 = (int)strlen(pPkt->m_16);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_16, local_14);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}