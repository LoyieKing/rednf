void advancealtar::Timer_StageControl::RegistTimerClearReward(int stageIdx,
                                                             advancealtar::StageEndType::T endType)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa8, kClearRewardMs, 0xa8, endType);
