int TimerQueryCounter::RegistNextTimer()
{
    // ORIG：delay 先物化到局部槽 -0xc(%ebp)（movl $0x708,-0xc），再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x59,
                                delay, 0, 0);
    return 1;
