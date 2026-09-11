// ORIG T 0x0863ab4c：static，全部常量实参
void Timer_Send_Ontime_Reward::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x9b, 10, 0, 0);
}