// ORIG T 0x0863a7c2 _ZN20Timer_HadesHeartBeat15registNextTimerEv
void Timer_HadesHeartBeat::registNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x96, 0x3c, 0, 0);
}