int TimerCloseAssault::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863767c: GetInstanceAssaultMgr() -> OnTimeoutCloseAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCloseAssault(partyIdx, p2);
    return 1;
