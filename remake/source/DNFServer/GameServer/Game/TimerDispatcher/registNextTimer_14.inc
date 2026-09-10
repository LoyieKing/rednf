void Timer_PcRoomAuthRetry::registNextTimer(long sec, int p1, int p2,
                                            unsigned int p3)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x92, static_cast<int>(sec),
                                p2, p3);
