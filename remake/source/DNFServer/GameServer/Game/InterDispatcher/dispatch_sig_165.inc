int Inter_BlackIPMonitorPartLoadReq::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_BlackIPMonitorPartLoad::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__,
        0x3f63, 0)("Get BlackIPMonitor Request Part Load");
    return 0;
