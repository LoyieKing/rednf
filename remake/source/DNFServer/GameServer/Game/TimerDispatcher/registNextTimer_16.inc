void Timer_HadesHeartBeat::registNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x96, 0x3c, 0, 0);
