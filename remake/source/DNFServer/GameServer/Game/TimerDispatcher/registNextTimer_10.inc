// ORIG T 0x08638b5c (_ZN20TimerGeneratePowerUp15registNextTimerEl)
void TimerGeneratePowerUp::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x7c, static_cast<int>(sec), 0, 0);
}