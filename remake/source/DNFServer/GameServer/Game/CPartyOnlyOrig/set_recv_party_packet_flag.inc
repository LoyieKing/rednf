void CParty::set_recv_party_packet_flag(CUser* user)
{
    int slot = GetMemberSlotNo(const_cast<const CUser*>(user));
    // b28d-45 P223 变体：ORIG 0x085b3f70 体尾 `jmp +0x3c; nop` 岛（4.4 对
    // 正向 if 恒 jump-thread）⇒ 改裸早返形复现。
    if (slot < 0)
    {
        return;
    }
    reinterpret_cast<CPartyRecvFlagView*>(this)->m_recvFlag.SetRecvFlag(slot);
