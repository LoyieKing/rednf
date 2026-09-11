// ORIG T 0x0863847e (_ZN28TimerGeneratePowerUpNotiTime15registNextTimerEl)
void TimerGeneratePowerUpNotiTime::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x7d,
                                sec, 0, 0);
}