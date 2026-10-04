// [b28d-lane2] verdict=DIFF ndiff=45（ORIG 0x084e16fe）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=6（ORIG 0x84e16fe）：cmp 方向经 int 同型化
// 已翻正（-1）；余 entry 地址形 N4/P30（ORIG 三段 add 0x10/base/0x6 vs 4.4 add 0x16+lea）。
int Inter_PvPBuddyConnList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b28c-30] N22 封顶：ORIG 0x084e1709 `cmp %eax,%ebx`（ebx = 局部字段值）vs
    // 4.4 `cmp %ebx,%eax`（call 结果在 %eax）；cmp 两向不可翻（dst=局部 vs
    // dst=call 结果）。
    // [P5·2026-09-11] N4a（/tmp/P5/n4/RESULT.md d2 实证）：**指针型局部槽 → int 型局部槽**。
    //   指针槽（+ 逐处 reinterpret_cast<int>(pPkt)）令 4.4 把 `(i*0x29 + 0x10) + base + 6`
    //   合成 `lea 0x10(%eax),%edx; mov -0x10(%ebp),%eax; lea (%edx,%eax,1),%eax`（唯一残差
    //   block）；改 int 槽后逐步复现 ORIG 084e17a2 `add $0x10,%eax; add -0x10(%ebp),%eax;
    //   add $0x6,%eax` 三段就地 add（槽位不变；比较段/put_header/put_int/循环尾全同）。
    //   字段访问统一经 reinterpret_cast<V*>(base) 读，与旧形逐指令同码。
    int base = reinterpret_cast<int>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG 一步 `mov 0xa(%eax),%ebx` 直落 callee-saved（具名槽会顶深 pPkt，技巧2）。
    register int charIdMsg =                           // recv charId @ msg+0xa
        reinterpret_cast<Inter_PvPBuddyConnListPktView*>(base)->m_a;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    // ORIG：m_12 每处独立 reload（无 count 槽；put_int + 循环界各一次）。
    PacketGuard packet;
    packet.put_header(0, 0x10f);
    packet.put_int((int)reinterpret_cast<Inter_PvPBuddyConnListPktView*>(base)->m_12);
    for (int i = 0; i < (int)reinterpret_cast<Inter_PvPBuddyConnListPktView*>(base)->m_12; ++i)
        // 求值序 = ORIG：i 项先算（eax），再 add $0x10 / add base 槽 / add $0x6
        // （base 置于两常量之间阻折叠 ⇒ 三段就地 add，N4a）。
        packet.put_binary(
            reinterpret_cast<Inter_PvPBuddyConnListPktView*>(base)->m_recs[i].m_data,
            0x29);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}