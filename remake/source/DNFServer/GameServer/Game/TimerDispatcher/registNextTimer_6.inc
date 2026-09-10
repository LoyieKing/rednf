void TimerConnectP2PAfterAssaultTimeout::registNextTimer(long sec,
                                                         int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x72, (int)sec,
                                (unsigned int)p2, 0);
