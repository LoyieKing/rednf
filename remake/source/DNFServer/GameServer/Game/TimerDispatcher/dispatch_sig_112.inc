// ORIG 0x8639042：村战开启定时器（报告 TimerStartVillageAttacked/dispatch_sig.md）。
bool TimerStartVillageAttacked::dispatch_sig(int /*p1*/, int /*p2*/,
                                            memberIdentificationNumber_t /*p3*/)
{
    GlobalData::s_villageMonsterMgr->OnStartVillageAttacked();
    return 1;
}