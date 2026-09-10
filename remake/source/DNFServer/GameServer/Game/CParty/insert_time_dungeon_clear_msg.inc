void CParty::insert_time_dungeon_clear_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0xd, param,
        gen_timer_key((TIMER_MESSAGE)0xd), 0);
