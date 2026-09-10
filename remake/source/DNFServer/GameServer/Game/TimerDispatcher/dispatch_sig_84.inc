int TimerCompleteLoadAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863752c: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoad(p1,p2), return 1
    // ORIG：p1 先物化到 callee-saved %ebx（跨 GetInstance 调用存活），mgr 不落槽。
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCompleteLoad(partyIdx, p2);
    return 1;
