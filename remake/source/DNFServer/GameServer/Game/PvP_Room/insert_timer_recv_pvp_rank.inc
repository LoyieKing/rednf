// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::insert_timer_recv_pvp_rank（原 TU 第 2876 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N7] ORIG 0x085dc340：gen_timer_key→%ebx / m_index→%esi（0x085dc36b/085dc375），4.4 反配（同 VoteLackUser 判例）；裸 register 实测无效 ⇒ N7 封顶。
// [b28d-solo-PvP_Room] N7-N109: ORIG 0x085dc340 key->ebx/m_index->esi，4.4 反配
int PvP_Room::insert_timer_recv_pvp_rank()
{
    // 残余 4 行 N7 互换：gen_timer_key 结果与 m_index 的 callee-saved 缓存
    // ORIG 用 ebx/esi、4.4 反序（同 VoteLackUser 判例），封顶。
    if (m_isInsertTimerRecvPvpRank != 0)
    {
        return 1;
    }
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, m_index, (TIMER_MESSAGE)0x3c, 10000LL,
        (int)gen_timer_key((TIMER_MESSAGE)0x3c), 0);
    m_isInsertTimerRecvPvpRank = 1;
    return 1;
}
