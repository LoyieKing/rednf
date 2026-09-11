// ORIG 0x085503fe
void CLogGameChannel::SetNextLogTimer()
{
    // InsertTimer(OBJ_TYPE_2, 0, TIMER_MESSAGE_0x58, period*60, 0, 0)
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x58, m_period * 0x3c, 0, 0);
}