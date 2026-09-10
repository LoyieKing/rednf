void TimerScheduleTenSecond::registNextTimer(unsigned int sec)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x57, (int)sec, 0, 0);
