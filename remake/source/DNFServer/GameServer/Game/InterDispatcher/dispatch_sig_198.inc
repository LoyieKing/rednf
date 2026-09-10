int Inter_ReloadPowerWarRanker::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    cMyTrace(__PRETTY_FUNCTION__,
        0x482b, 0)("Receive Packet_Request_Reload_Power_War_Ranker");
    GlobalData::s_power_manager->ClearMVPInfo();
    GlobalData::s_power_manager->LoadRankerInfo();
    return 0;
