// ============================================================================
// insert_time_*（ORIG 0x85b2114..0x85b22e1）：gen_timer_key 后向 TimerQueue
// 注册 OBJ_TYPE_1 定时器，param1=队伍索引，sec=调用方参数，param3=0。
// ============================================================================
void CParty::insert_time_dungeon_clear_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0xd, param,
        gen_timer_key((TIMER_MESSAGE)0xd), 0);
}