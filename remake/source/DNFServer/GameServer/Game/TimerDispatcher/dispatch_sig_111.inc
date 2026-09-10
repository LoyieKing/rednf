int TimerVillageMonsterRegen::dispatch_sig(int p1, int p2, unsigned int p3)
{
    GlobalData::s_villageMonsterMgr->OnRegenVillageMonster(p2,
                                                           static_cast<int>(p3));
    return 1;
