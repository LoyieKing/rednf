void advancealtar::Timer_StageControl::RegistTimerCheckFinsh(int stageIdx)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa7, kCheckFinishMs, 0xa7, 0);
