int Inter_NpcLimitBuyItemInfoAll::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                     // ORIG 两独立守卫（P133）
        return 0;
    if (user->get_state() == 0)
        return 0;

    Inter_NpcLimitBuyItemInfoAllPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoAllPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(pPkt->m_16);   // msg+0x16 count
    // ORIG 0x84e62de..0x84e63a9 每处 `lea (%ecx,%eax,1)` 的 ecx = -0x10 具名槽（pPkt），
    // 非形参 0x10(%ebp)（P30/N4 族可消）。
