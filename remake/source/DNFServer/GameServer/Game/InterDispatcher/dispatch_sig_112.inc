int Inter_LoadDeathTowerBestRecord::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadDeathTowerBestRecordPktView* pPkt = reinterpret_cast<Inter_LoadDeathTowerBestRecordPktView*>(data);
