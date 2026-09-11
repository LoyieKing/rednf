bool TimerEndAssault::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x863760c: GetInstanceAssaultMgr() -> OnTimeoutEndAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutEndAssault(partyIdx, p2);
    return 1;
}