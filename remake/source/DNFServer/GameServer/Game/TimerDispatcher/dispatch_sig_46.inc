// ORIG T 0x0863575c (_ZN32TimerStatisticsDetailChannelInfo12dispatch_sigEiij)：
// G_GameWorld()->updateDetailChannelServiceInfo()；RegistNextTimer()；返回 1。
bool TimerStatisticsDetailChannelInfo::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    G_GameWorld()->updateDetailChannelServiceInfo();
    RegistNextTimer();
    return 1;
}