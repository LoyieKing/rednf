int Inter_HadesDisConnect::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    Timer_HadesHeartBeat::registNextTimer();
    return 0;
}