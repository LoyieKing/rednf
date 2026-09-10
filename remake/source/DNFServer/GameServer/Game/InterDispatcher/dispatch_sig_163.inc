int Inter_BlackIPMonitorReq::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_BlackIPMonitor::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__, 0x3f28, 0)("Get BlackIPMonitor Request");
    return 0;
