int TimerVillageMonsterUpdateRate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // OnRun/OnRegen/OnStart 为非静态成员（CVillageMonsterMgr.h），经全局管理器实例调用。
    GlobalData::s_villageMonsterMgr->OnRunVillageMonster();
    return 1;
