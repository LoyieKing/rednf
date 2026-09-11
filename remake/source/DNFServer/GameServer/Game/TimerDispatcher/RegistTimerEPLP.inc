// ORIG T 0x0812f8ec _ZN12advancealtar18Timer_StageControl15RegistTimerEPLPEiNS_12StageEndType1TE
void advancealtar::Timer_StageControl::RegistTimerEPLP(int stageIdx,
                                                       advancealtar::StageEndType::T endType)
{
    long ms = kFailEPLPMs;      // ORIG 先置 0x3e8；endType==3 分支同值再赋 0x3e8（语义无差异）
    if (endType == static_cast<advancealtar::StageEndType::T>(3)) {
        ms = kClearEPLPMs;
    }
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa9, ms, 0xa9, endType);
}