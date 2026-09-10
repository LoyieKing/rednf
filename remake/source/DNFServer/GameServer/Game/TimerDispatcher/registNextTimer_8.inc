void TimerConnectMonitorServer::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x7a, static_cast<int>(sec),
                                p2, 0);
