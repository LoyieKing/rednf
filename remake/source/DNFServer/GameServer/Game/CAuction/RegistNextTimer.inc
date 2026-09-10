void Timer_CheckAuctionReady::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                static_cast<TIMER_MESSAGE>(0x98), 0x3c, 0, 0);
