// ORIG T 0x086397ee (_ZN25Timer_SecretShopStatistic15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x90, 0x3c, 0, 0)。
void Timer_SecretShopStatistic::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x90, 0x3c, 0, 0);
}