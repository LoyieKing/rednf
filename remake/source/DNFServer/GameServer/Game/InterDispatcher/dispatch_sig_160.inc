int Inter_LoadBreakAwayPreventSystem::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* packet =
        reinterpret_cast<SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER*>(data);
    GlobalData::s_BreakAwaySys->loadDbData(packet);
    return 0;
