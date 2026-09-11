// ORIG T 0x08635c2c (_ZN12TimerNatType12dispatch_sigEiij)：
// G_GameWorld()->GetStatisticsNatType()->UpdateNatType()；RegistNextTimer()；返回 1。
bool TimerNatType::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    G_GameWorld()->GetStatisticsNatType()->UpdateNatType();
    RegistNextTimer();
    return 1;
}