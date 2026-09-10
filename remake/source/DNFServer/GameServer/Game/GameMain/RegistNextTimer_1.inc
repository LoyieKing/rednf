void Timer_OneADayItemShop::RegistNextTimer()
{
    // ORIG 0x863a93c：msg=0x99，间隔 60s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x99, 0x3c, 0, 0);
