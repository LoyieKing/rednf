int Inter_LoadTicket::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x1a70;
    if (user->get_state() == 0)
        return 0x1a72;
    // ORIG 0x084c515a：pPkt 在 state 检查后才物化 -0x10；count 无槽（每次重载 +0x8）
    Inter_LoadTicketPktView* pPkt = reinterpret_cast<Inter_LoadTicketPktView*>(data);
    if (pPkt->m_4 == 1)
        return 0x1a79;
    if (pPkt->m_8 > 99)
        return 0x1a7e;
    // ORIG cond 重载 n 且方向 n>i（cmp MEM,%eax; setg）；体走数组视图折叠寻址
    for (int i = 0; pPkt->m_8 > i; ++i)
        user->AddTicket(pPkt->m_ids[i], false);
    user->EnableCharacInfo((CACHE_SECTION)4);
    return 0;
}