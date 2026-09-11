// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084cc23c）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=2（ORIG 0x84cc23c）：N22 有参 callee 封顶（同上）。
int Inter_MonitorGuildMemberNamingReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084cc262：pPkt 物化在 null 岛之后（技巧13：声明位置=落栈时机）。
    Inter_MonitorGuildMemberNamingReplyPktView* pPkt = reinterpret_cast<Inter_MonitorGuildMemberNamingReplyPktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：ORIG 0x084cc26b 字段值直落
    // %ebx 跨 call 存活 + `cmp %eax,%ebx` 只在字段读**内联进比较**（无具名局部）时复现；
    // 具名（register/非 register）局部必产 `cmp %ebx,%eax`（P0：无槽往返 ⇒ 原码无该局部）。
    if (pPkt->m_12 != user->get_charac_no(-1))
        return 0;

    // ORIG 0x084cc2ba/0x084cc2ec：m_a 两处独立 reload，无具名槽（技巧37）。
    PacketGuard packet;
    packet.put_header(1, 0x80);
    if (pPkt->m_a == 0)
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.put_byte(pPkt->m_a);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}