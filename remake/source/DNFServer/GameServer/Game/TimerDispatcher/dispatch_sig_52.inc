int TimerNatTypeUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->UpdateNatTypeUser();
    RegistNextTimer();
    return 1;
