void CParty::insert_time_card_select_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x10, param,
        gen_timer_key((TIMER_MESSAGE)0x10), 0);
}