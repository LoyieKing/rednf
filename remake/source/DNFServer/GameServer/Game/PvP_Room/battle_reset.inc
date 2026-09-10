// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::battle_reset（原 TU 第 3034 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

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
}
