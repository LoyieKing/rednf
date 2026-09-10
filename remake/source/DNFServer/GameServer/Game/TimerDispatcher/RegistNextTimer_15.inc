void Timer_NoticeEmergencyPatchMessage::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xa2,
                                0x3c, 0, 0);
