void TimerRefreshPowerWarProcessInfo::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, 0,
                                (TIMER_MESSAGE)0x7b, static_cast<int>(sec), 0, 0);
