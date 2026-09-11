// [b28d-lane2] verdict=DIFF ndiff=71（ORIG 0x084dcbea）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=60（ORIG 0x84dcbea）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_SetFullIPCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [G2D] verdict=N：与 SetIPCount 同族、同形（ORIG base 走 %ecx + lea vs 我方 add）。
    //   属 O0 寄存器选择差异，无可判定改写，未改。
    Inter_SetFullIPCountPktView* pPkt = reinterpret_cast<Inter_SetFullIPCountPktView*>(data);
    if (pPkt->m_a == 0)
        GlobalData::g_autoPunishRuleHackTypeMgr->ResetFullIpCounterData();
    // §P0：与 SetIPCount 同族，循环尾每轮重读 m_b（count 无栈槽）。
    for (int i = 0; i < (int)(unsigned char)pPkt->m_b; ++i)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setFullIPCounterData(
            (unsigned int)pPkt->m_entries[i].m_c,
            (unsigned int)pPkt->m_entries[i].m_e,
            pPkt->m_entries[i].m_sub.m_ip,
            // [H2·2026-09-11 laneA 形A 负结论·已回滚] 同 SetIPCount（ORIG 084dcc1d/084dcc29，
            //   stride 0x18）：显式 int 域加法形反劣 n 71→72、nd 3→5 ⇒ 判 N。
            reinterpret_cast<Inter_SetFullIPCountSubTail*>(pPkt->m_entries[i].m_sub.m_ip)->m_counter);
    }
    return 0;
}