// ORIG T 0x08636348 (_ZN17TimerQueueSizeLog15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x61, 0x3c, 0, 0)；返回 1。
bool TimerQueueSizeLog::RegistNextTimer()
{
    // ORIG：delay 物化到局部槽 -0xc(%ebp) 再两步入栈。
    int delay = 0x3c;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x61, delay, 0, 0);
    return 1;
}