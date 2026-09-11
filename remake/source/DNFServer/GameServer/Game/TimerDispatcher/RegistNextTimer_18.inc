// ORIG T 0x086396d2 sz 0x4f（_ZN23Timer_TenMinuteSchedule15RegistNextTimerEi）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x8e, 600, param_1, 0)；返回 1。
bool Timer_TenMinuteSchedule::RegistNextTimer(int type)
{
    // ORIG 0x086396db：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），非 const 防常量传播，
    // 0x10(%esp)=mov -0xc(%ebp),%edx 装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8e,
                                nSec, type, 0);
    return 1;
}