int TimerConnectP2PAfterAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863775c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2PAfterAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutConnectP2PAfterAssault(partyIdx, p2);
    return 1;
