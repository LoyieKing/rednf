bool advancealtar::Timer_StageControl::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    advancealtar::StageControl* stage = G_CGameManager()->getAdvanceAltar(p1);
    if (stage == NULL) {
        return 0;
    }
    stage->onTimer(static_cast<TIMER_MESSAGE>(p2),
                   static_cast<advancealtar::StageEndType::T>(p3));
    return 1;
}