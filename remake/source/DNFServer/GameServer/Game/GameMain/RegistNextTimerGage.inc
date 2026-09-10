void advancealtar::Timer_MainTick::RegistNextTimerGage()
{
    // ORIG 0x812f7b8：毫秒级 InsertTimerInMilisecond，msg=0xa6，1000ms
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)8, 0,
                                            (TIMER_MESSAGE)0xa6, 0x3e8, 0, 0);
