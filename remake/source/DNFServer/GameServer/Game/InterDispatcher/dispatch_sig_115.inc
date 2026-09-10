int Inter_MonitorTowerReloadTopRanker::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_DeathTowerLoadTopRanker::makeRequest(true);
    return 0;
