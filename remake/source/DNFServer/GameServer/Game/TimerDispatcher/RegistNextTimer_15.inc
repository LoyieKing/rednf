// ORIG T 0x0863b87e sz 0x45（_ZN33Timer_NoticeEmergencyPatchMessage15RegistNextTimerEv）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0xa2, 0x3c, 0, 0)。
void Timer_NoticeEmergencyPatchMessage::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xa2,
                                0x3c, 0, 0);
}