// ORIG T 0x0812f956 _ZN12advancealtar18Timer_StageControl28RegistTimerEPLPReturnVillageEi
void advancealtar::Timer_StageControl::RegistTimerEPLPReturnVillage(int stageIdx)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xaa, kEPLPReturnVillageMs, 0xaa, 0);
}