bool TimerNatTypeUser::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    G_GameWorld()->UpdateNatTypeUser();
    RegistNextTimer();
    return 1;
}