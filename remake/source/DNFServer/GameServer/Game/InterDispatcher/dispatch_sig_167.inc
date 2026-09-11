int Inter_LoadAutoMarketConditionsControl::dispatch_sig(
    CUser* /*user*/, char* data, int /*len*/)
{
    SIG_AUTO_MARKET_CONDITIONS_CONTROL* packet =
        reinterpret_cast<SIG_AUTO_MARKET_CONDITIONS_CONTROL*>(data);
    G_CDataManager()->GetAutoMarketContitionsControl()->LoadDatabase(packet);
    return 0;
}