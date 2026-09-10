int TimerNatType::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->GetStatisticsNatType()->UpdateNatType();
    RegistNextTimer();
    return 1;
