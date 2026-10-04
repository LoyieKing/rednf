// [b28d-lane2] verdict=DIFF ndiff=32（ORIG 0x084dad96）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=60（ORIG 0x84dad96）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
// [b28d-PoolInterDisp] verdict=NEAR ndiff=2（ORIG 0x84dad96）：memoLen 单变量复用（ORIG
// 84dae22/84dae61 同槽 -0xc 两次写实证）消 4B 多占槽 ⇒ 全族槽号对齐，60→2；残 = N22
// get_charac_no(int) cmp 方向（六形封顶，同 PowerWarProcessInfo）。
int Inter_NoticeGuildMemo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_NoticeGuildMemoPktView* pPkt = reinterpret_cast<Inter_NoticeGuildMemoPktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：ORIG `mov 0xe(%eax),%ebx;
    // cmp %eax,%ebx` 靠「字段读内联进比较」复现；具名（register）局部必产反向 dst。
    if (pPkt->m_characNo != user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0xc5);
    // ORIG 两段 memoLen 共用槽 -0xc（84dae22/84dae61 同址写）⇒ 单变量跨两段复用；
    // 旧双声明多占一槽，使 pPkt/packet 整体下移 4（N82 假性帧差主项）。
    int memoLen = (int)strlen(pPkt->m_memo1);              // memo 串 1
    packet.put_int(memoLen);
    packet.put_str(pPkt->m_memo1, memoLen);
    memoLen = (int)strlen(pPkt->m_memo2);                  // memo 串 2（重入同槽）
    packet.put_int(memoLen);
    packet.put_str(pPkt->m_memo2, memoLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}