void CParty::insert_time_card_select_right_msg(int param)
{
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x13, param,
        gen_timer_key((TIMER_MESSAGE)0x13), 0);
}