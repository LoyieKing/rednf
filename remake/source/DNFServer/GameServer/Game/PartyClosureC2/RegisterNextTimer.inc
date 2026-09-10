void Timer_AssaultGhostTimer::RegisterNextTimer(CUser* user, int sec,
                                                unsigned int characNo)
{
    if (user == 0)
        return;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0xa0, sec, (unsigned int)0,
                                characNo);
