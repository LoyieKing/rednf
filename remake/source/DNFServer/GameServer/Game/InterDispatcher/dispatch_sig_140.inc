int Inter_LoadCleanPadPointReq::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_LoadCleanPadPoint::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__,
        0x371d, 0)("Get CleanPad point Request");
    return 0;
