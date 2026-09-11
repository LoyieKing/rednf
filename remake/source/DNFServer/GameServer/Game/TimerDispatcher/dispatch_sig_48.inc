bool TimerScheduleOneHour::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x56, 0xe10, 0, 0);
    return 1;
}