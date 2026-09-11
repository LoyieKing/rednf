// [b28 判词 N22] 唯一残差 `cmp %eax,%ebx`↔`cmp %ebx,%eax`（dst 方向），负例在册。
// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084e1832）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=AE（收口：字段/局部 int 同型化翻转 cmp 方向 → 归零）。
int Inter_ReqAddPvPBuddy::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG +0x10：包基址槽在首条 call 之前即物化（-0x10）。
    Inter_ReqAddPvPBuddyPktView* pPkt = reinterpret_cast<Inter_ReqAddPvPBuddyPktView*>(data);
    if (user->get_state() <= 2 || user->get_state() == 7)
        return 0;
    // ORIG +0x27 一步 `mov 0xa(%eax),%ebx` ⇒ register 局部。
    register int charIdMsg =                           // recv charId @ msg+0xa
        pPkt->m_a;
    if (charIdMsg != user->getCurCharacNo())   // ORIG `cmp %eax,%ebx` 同上
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0x10c);
    packet.put_byte((int)pPkt->m_12);   // movsbl
    // ORIG：strlen 单次调用，结果落 -0xc 复用。
    const int nameLen = strlen(pPkt->m_13);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_13, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}