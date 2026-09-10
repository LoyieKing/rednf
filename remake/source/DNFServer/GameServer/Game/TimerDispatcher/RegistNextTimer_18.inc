bool Timer_TenMinuteSchedule::RegistNextTimer(int type)
{
    // ORIG 0x086396db：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），非 const 防常量传播，
    // 0x10(%esp)=mov -0xc(%ebp),%edx 装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8e,
                                nSec, type, 0);
    return 1;
