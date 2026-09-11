// ===========================================================================
// Timer_AssaultGhostTimer::RegisterNextTimer —— T 0x0863b242（无报告：objdump 转写）
// user 非空时：G_TimerQueue()->InsertTimer(OBJ_TYPE 0, user->GetUID(),
//   TIMER_MESSAGE 0xa0(调度槽 160), sec, 0, characNo)。
// ===========================================================================
void Timer_AssaultGhostTimer::RegisterNextTimer(CUser* user, int sec,
                                                unsigned int characNo)
{
    if (user == 0)
        return;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0xa0, sec, (unsigned int)0,
                                characNo);
}