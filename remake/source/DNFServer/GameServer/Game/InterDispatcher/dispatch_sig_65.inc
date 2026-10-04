// [b28d-lane2] verdict=DIFF ndiff=139（ORIG 0x084ccfda）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84ccfda）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 3→3（147 vs 144，TU 121/1331/5/133 不回退）：真修三处——
//   ① pPkt 声明移到 memberkey 门之后（复现 ORIG 0x084cd061 `mov 0x10(%ebp),%eax; mov %eax,-0x10`）；
//   ② 删 `register int characNo` 具名局部，比较/日志两处直读 `pPkt->m_characNo`（P0）；
//   ③ m_e int→unsigned int（P211/P265 档③，比较点中转）。残余=槽基平移 4 + N7 寄存器角色。
int Inter_MonitorNoticeMemberChatMsg::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2852,
            "MEMBER : Inter_MonitorNoticeMemberChatMsg::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    Inter_MonitorNoticeMemberChatMsgPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberChatMsgPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2859,
            "MEMBER : Inter_MonitorNoticeMemberChatMsg::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte(8);
    packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_msg);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_msg, msgLen);
    packet.put_byte(0);
    // ORIG 084cd1xx：无 nameLen 槽（我方旧形多发 `mov %eax,<槽>; mov <槽>,%eax` 往返），
    //   且读形为 `movzbl 0x30(%eax),%eax; movzbl %al,%eax`（二次零扩展）= (unsigned char) 包裹
    //   ⇒ 原码内联实参。
    packet.put_int((int)(unsigned char)pPkt->m_nameLen);
    packet.put_str(pPkt->m_name, (int)(unsigned char)pPkt->m_nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}