int PvP_Room::battle_reset()
{
    lock();
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        P_WIN(local_14) = 0;
        P_TEAMWIN(local_14) = 0;
        P_SEATSTATE(local_14) = 0;
        if (P_SEATS(local_14) != 0)
        {
            P_SEATS(local_14)->getHackAnalyzer()->reportHackInfo();
            P_SEATS(local_14)->getHackAnalyzer()->resetHackInfo();
        }
    }
    memset(m_field5dc, 0, 0xc);
    m_field5d8 = 0;
    m_state = 1;
    m_recvFlagB4.Clear();
    m_recvFlagBC.Clear();
    m_userTable.Clear();
    m_fieldC0 = 0;
    m_relayMgr.Reset();
    m_battleEndFlag = 0;
    m_rankRequested = 0;
    m_isInsertTimerRecvPvpRank = 0;
    m_isEndPvpBattle = 0;
    for (int local_10 = 0x31; local_10 < 0x3f; local_10 = local_10 + 1)
    {
        // ORIG 085d657d: cmpl $0x3e / je 085d6597(空块 nop) / call / jmp 085d6598
        // 4.1 保留的空 skip 块落点在 call 之后、++ 之前，用 goto 钉住块序。
        if (local_10 == 0x3e)
            goto SKIP_GEN_TIMER_KEY;
        gen_timer_key((TIMER_MESSAGE)local_10);
SKIP_GEN_TIMER_KEY:
        ;
    }
    unlock();
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}