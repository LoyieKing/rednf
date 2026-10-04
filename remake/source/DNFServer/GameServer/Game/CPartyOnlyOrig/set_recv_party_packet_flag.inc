// ORIG T 0x085b3f34（set_recv_party_packet_flag.md）：+0x1dc 结果接收标志区。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：+0x1dc 视图具名成员
// receiver（`CPartyRecvFlagView::m_recvFlag`）复现 ORIG `lea 0x1dc(%eax),%edx`
// —— 4.4 对取址 cast 恒 add（N107），对视图具名成员产 lea。
void CParty::set_recv_party_packet_flag(CUser* user)
{
    int slot = GetMemberSlotNo(const_cast<const CUser*>(user));
    // b28d-45 P223 变体：ORIG 0x085b3f70 体尾 `jmp +0x3c; nop` 岛（4.4 对
    // 正向 if 恒 jump-thread）⇒ 改裸早返形复现。
    if (slot < 0)
    {
        return;
    }
    m_recvFlag.SetRecvFlag(slot);
}