void TimerDetectDisconnect::registNextTimer()
{
    int interval = 10;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x6d, interval, 0, 0);
