int TimerNatType::RegistNextTimer()
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用局部变量复现该栈槽暂存（同 TimerFatigueReset 先例）。
    int delay = 0x15180;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5a, delay, 0, 0);
    return 1;
