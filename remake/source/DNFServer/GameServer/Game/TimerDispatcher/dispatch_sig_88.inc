int TimerCompleteLoadAfterAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86376ec: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoadAfterAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCompleteLoadAfterAssault(partyIdx, p2);
    return 1;
