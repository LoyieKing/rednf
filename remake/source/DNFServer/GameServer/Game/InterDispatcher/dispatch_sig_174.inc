int Inter_LoadPowerWarStatueRanker::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadPowerWarStatueRankerPktView* pPkt =
        reinterpret_cast<Inter_LoadPowerWarStatueRankerPktView*>(data);
    GlobalData::s_power_manager->SetPowerWarUserRankingInfo(
        reinterpret_cast<unsigned int*>(pPkt->m_charNos));
    return 0;
