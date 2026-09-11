// ORIG T 0x08635cd0 (_ZN16TimerNatTypeUser15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5b, 0xe10, 0, 0)；返回 1。
int TimerNatTypeUser::RegistNextTimer()
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用非 const 局部变量复现该栈槽暂存（const 会被 4.4 折叠为立即数）。
    int delay = 0xe10;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5b, delay, 0, 0);
    return 1;
}