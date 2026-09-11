void Timer_EventManagement::RegistNextTimer(int intervalMs)
{
    // ORIG 0x863abd4：msg=0x9c，间隔=参数
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x9c, intervalMs, 0, 0);
}