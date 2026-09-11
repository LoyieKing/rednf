// [b28d-lane2] verdict=DIFF ndiff=128（ORIG 0x084d422a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-15。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=117（ORIG 0x84d422a）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_AutoPunishRuleBlackIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    GlobalData::g_autoPunishRuleBlackIPMgr->reset();
    Inter_AutoPunishRuleBlackIPListView* pPkt =
        reinterpret_cast<Inter_AutoPunishRuleBlackIPListView*>(data);

    // P227/P261：totalCount 无具名槽（ORIG 084d4244 直读 data+0 入 %ebx 保活，084d43d7 循环处重读）。
    cMyTrace(__PRETTY_FUNCTION__,
        0x3561, 0)("Black IP Load Start (TotalCount: %d)",
                   (int)pPkt->m_totalCount);

    for (unsigned int i = 0; i < pPkt->m_totalCount; ++i)   // cmp; seta
    {
        char ipStr[16] = { '\0' };
        in_addr netIp;
        // P0/P261：ORIG 084d42b5 先取 ip 入 %ebx、084d42cf `or %ebx,%eax`（ip 为左操作数）；
        //   无 addrPart/netIp 具名临时（结果直落 netIp 槽），无 lea 取址往返。
        netIp.s_addr = pPkt->m_entries[i].m_ip
                       | htonl((unsigned int)pPkt->m_entries[i].m_startIp);
        strncpy(ipStr, inet_ntoa(netIp), 0x10);

        // [K3 2026-09-11] P0/极性：ORIG `084d4330 call addBlackIP; xor $0x1,%eax; test %al,%al;
        //   je`（`!bool` 形，无 added 具名槽、无 setne/cmp）⇒ 去槽 + 取反形。
        if (!GlobalData::g_autoPunishRuleBlackIPMgr->addBlackIP(
                (unsigned long)pPkt->m_entries[i].m_ip,
                pPkt->m_entries[i].m_startIp,
                pPkt->m_entries[i].m_maxIp))
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x356d,
                "Black IP Load fail(StartIP:%s -> %d)", ipStr,
                (int)pPkt->m_entries[i].m_maxIp);
        }
        // P227：maxIp 无具名槽（ORIG 084d4387 直读 data+i*8+9 → %ebx → 实参）。
        cMyTrace(__PRETTY_FUNCTION__,
            0x356e, 0)("Black IP Load (StartIP:%s -> %d)", ipStr,
                       (int)pPkt->m_entries[i].m_maxIp);
    }

    // P227：getBlackIPCount 结果直内联（ORIG 084d43f8 `mov %eax,%ebx` 跨 ctor 保活，无 finalCount 槽）。
    cMyTrace(__PRETTY_FUNCTION__,
        0x3570, 0)("Black IP Load End (TotalCount: %d)",
                   GlobalData::g_autoPunishRuleBlackIPMgr->getBlackIPCount());
    return 0;
}