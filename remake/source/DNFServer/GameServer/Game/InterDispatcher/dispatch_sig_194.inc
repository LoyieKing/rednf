int Inter_SetIPCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SetIPCountPktView* pPkt = reinterpret_cast<Inter_SetIPCountPktView*>(data);
    if (pPkt->m_a == 0)
        GlobalData::g_autoPunishRuleHackTypeMgr->ResetIpCounterData();
    // §P0：ORIG 084dcbc5 循环尾 `movzbl 0xb(%eax),%eax; movzbl %al,%eax; cmp -0x1c(%ebp),%eax`
    //   = 每轮重读 m_b（count 无栈槽）⇒ 不设具名 count，条件内联表达式。
    for (int i = 0; i < (int)(unsigned char)pPkt->m_b; ++i)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setIPCounterData(
            (unsigned int)pPkt->m_entries[i].m_c,
            (unsigned int)pPkt->m_entries[i].m_e,
            pPkt->m_entries[i].m_sub.m_ip,
            reinterpret_cast<Inter_SetIPCountSubTail*>(pPkt->m_entries[i].m_sub.m_ip)->m_counter);
    }
    return 0;
