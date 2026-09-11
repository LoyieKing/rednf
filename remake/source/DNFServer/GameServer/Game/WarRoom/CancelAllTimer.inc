void WarRoom::CancelAllTimer()
{
    for (int i = 0x40; i <= 0x47; ++i)
        GenTimerKey((TIMER_MESSAGE)i);
}