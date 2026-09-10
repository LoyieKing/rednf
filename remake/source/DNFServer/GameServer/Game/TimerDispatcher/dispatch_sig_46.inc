int TimerStatisticsDetailChannelInfo::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->updateDetailChannelServiceInfo();
    RegistNextTimer();
    return 1;
