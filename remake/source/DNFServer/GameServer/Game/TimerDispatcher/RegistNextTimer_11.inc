// ORIG T 0x0863977e _ZN33Timer_UpdateOnlinePreliminaryTeam15RegistNextTimerEi
bool Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer(int team)
{
    // ORIG 0x0863977e：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），0x10(%esp) 二级装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8f, nSec, team, 0);
    return true;
}