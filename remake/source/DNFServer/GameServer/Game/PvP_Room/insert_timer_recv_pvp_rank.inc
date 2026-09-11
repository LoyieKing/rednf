// [b28d-7 N7] ORIG 0x085dc340：gen_timer_key→%ebx / m_index→%esi（0x085dc36b/085dc375），4.4 反配（同 VoteLackUser 判例）；裸 register 实测无效 ⇒ N7 封顶。
// [b28d-solo-PvP_Room] N7-N109: ORIG 0x085dc340 key->ebx/m_index->esi，4.4 反配
// [G8InvenPvP 2026-09-10 · 第 6 形实测失败] `({ int idx = m_index; idx; })`
// 语句表达式形（在 insert_timer_recv_pvp_end_result 有效的形）在本件**反劣**：
// 帧 0x20→0x30、指令 36→38、ndiff 4→8（idx 落 -0xc 槽 + key 仍留 %esi，
// ORIG 0x085dc340 `mov %eax,%ebx` 未复现）⇒ 回退原内联形，维持 N7/N109 封顶。
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