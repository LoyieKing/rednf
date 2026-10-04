// [b28d-lane2] verdict=DIFF ndiff=68（ORIG 0x084dcb18）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=60（ORIG 0x84dcb18）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_SetIPCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [G2D] verdict=N：与 ORIG 仅差 3 行（ORIG 084dcb58 段 base 走 %ecx 后
    //   `lea (%ecx,%eax,1),%eax`，我方为 `add -0x20(%ebp),%eax`）；同一表达式在 O0 下的
    //   寄存器选择差异，无源码可判定的等价改写，未改（不留 hack）。
    Inter_SetIPCountPktView* pPkt = reinterpret_cast<Inter_SetIPCountPktView*>(data);
    if (pPkt->m_ok == 0)
        GlobalData::g_autoPunishRuleHackTypeMgr->ResetIpCounterData();
    // §P0：ORIG 084dcbc5 循环尾 `movzbl 0xb(%eax),%eax; movzbl %al,%eax; cmp -0x1c(%ebp),%eax`
    //   = 每轮重读 m_b（count 无栈槽）⇒ 不设具名 count，条件内联表达式。
    for (int i = 0; i < (int)(unsigned char)pPkt->m_count; ++i)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setIPCounterData(
            (unsigned int)pPkt->m_entries[i].m_c,
            (unsigned int)pPkt->m_entries[i].m_e,
            pPkt->m_entries[i].m_sub.m_ip,
            // [H2·2026-09-11 laneA 形A 负结论·已回滚] ORIG 084dcb4b/084dcb58 走
            //   `mov -0x20(%ebp),%ecx; lea (%ecx,%eax,1),%eax`（base 进寄存器）；本件的
            //   4.4 恒折 `add -0x20(%ebp),%eax`（少 1 条，nd=3）。试过「显式 int 域加法 +
            //   独立 +0x10 指针」形 ⇒ 反劣为 `mov %eax,%edx; mov -0x20(%ebp),%eax;
            //   lea (%edx,%eax,1),%eax`（n 71→72、nd 3→5）⇒ 回滚，判 N（P30/N4 寄存器选择族）。
            reinterpret_cast<Inter_SetIPCountSubTail*>(pPkt->m_entries[i].m_sub.m_ip)->m_counter);
    }
    return 0;
}