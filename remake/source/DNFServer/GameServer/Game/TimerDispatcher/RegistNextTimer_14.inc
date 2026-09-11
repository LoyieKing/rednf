void Timer_GenerateCaptchaData::RegistNextTimer(int sec)
{
    // ORIG 0x863b2ba：G_TimerQueue()->InsertTimer((OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
    // sec, 0, 0)。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
                                sec, 0, 0);
}