bool TimerStatisticsLog::RegistNextTimer()
{
    // ORIG：delay 物化到局部槽 -0xc(%ebp) 再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x52,
                                delay, 0, 0);
    return 1;
