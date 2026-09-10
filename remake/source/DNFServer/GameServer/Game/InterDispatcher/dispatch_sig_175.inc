int Inter_LoadPowerWarStatueInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_LOAD_POWER_WAR_STATUE_INFO* packet =
        reinterpret_cast<SIG_LOAD_POWER_WAR_STATUE_INFO*>(data);
    GlobalData::s_power_manager->SetPowerWarUserStatueInfo(packet);
    return 0;
