// [b28d-lane2] verdict=DIFF ndiff=136（ORIG 0x084cd83a）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：`const int msgLen=(int)strlen(...)`
// + 无 cast 实参 ⇒ 4.4 对 `put_str(&pPkt->m_name[0], msgLen)` 发 ORIG 的 lea 形，探针 q7 实证）。
int Inter_MonitorNoticeMemberSecede::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorNoticeMemberSecedePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberSecedePktView*>(data);

    // ORIG：unsigned 字段失配 4 行 spill（同 ReplyGuildLetter）。
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2914,
            "MEMBER : Inter_MonitorNoticeMemberSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCallerCharID(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);
        return 0;
    }

    // ORIG：m_12 各处独立 movzbl reload（无具名槽，槽数决定 pPkt=-0x14/PG=-0x20）。
    if (pPkt->m_secedeType == 1)
    {
        user->set_connect_upper_member(0);
        user->set_uppermember_exp_level(0);
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
            party->send_party_realtime_info();
    }

    PacketGuard packet;
    packet.put_header(1, 0x51);
    if (pPkt->m_secedeType == 1 || pPkt->m_secedeType == 2)
        packet.put_byte(1);
    else
        packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_name);
    packet.put_int(msgLen);
    packet.put_str(&pPkt->m_name[0], msgLen);   // 探针 q7 实证：int 局部 + 无 cast 实参 ⇒ 4.4 发 lea
    if (pPkt->m_secedeType == 1 || pPkt->m_secedeType == 2)
        packet.put_byte(1);
    else
        packet.put_byte((int)pPkt->m_secedeType);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}