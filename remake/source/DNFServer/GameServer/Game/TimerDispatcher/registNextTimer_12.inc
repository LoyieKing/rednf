// ORIG T 0x08638294 _ZN24TimerCheckPowerWarAbuser15registNextTimerEl
void TimerCheckPowerWarAbuser::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x80, static_cast<int>(sec), 0, 0);
}