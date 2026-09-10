void Timer_DailySchedule::RegistNextTimer()
{
    // ORIG 0x812c940：msg=0x91，间隔 60s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x91, 0x3c, 0, 0);
