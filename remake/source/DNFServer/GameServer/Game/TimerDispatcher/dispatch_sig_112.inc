int TimerStartVillageAttacked::dispatch_sig(int /*p1*/, int /*p2*/,
                                            unsigned int /*p3*/)
{
    GlobalData::s_villageMonsterMgr->OnStartVillageAttacked();
    return 1;
