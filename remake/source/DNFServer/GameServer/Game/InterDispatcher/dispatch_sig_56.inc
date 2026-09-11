// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084cc650）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=2（ORIG 0x84cc650）：N22 有参 callee 封顶（同上）。
int Inter_GuildNoticeGuildMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_GuildNoticeGuildMessagePktView* pPkt = reinterpret_cast<Inter_GuildNoticeGuildMessagePktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：ORIG +0x2f 一步
    // `mov 0xe(%eax),%ebx; cmp %eax,%ebx` 只在字段读**内联进比较**（无具名局部）时复现；
    // 用 register/普通具名局部必产 `cmp %ebx,%eax`（P0：无槽往返 ⇒ 原码无该局部）。
    if (pPkt->m_e != user->get_charac_no(-1))         // recv charId @ msg+0xe
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0x8d);
    // ORIG：strlen 只调一次，结果落 -0xc 供 put_int/put_str 复用。
    const int msgLen = strlen(pPkt->m_12);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}