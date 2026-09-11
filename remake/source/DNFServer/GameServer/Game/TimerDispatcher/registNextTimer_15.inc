// ORIG _ZN23Timer_ReturnUserTimeout15registNextTimerEiij @0x0863a706：
//   now = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, (TIMER_MESSAGE)0x95,
//                               (int)(p3 - now), p2, 0)
void Timer_ReturnUserTimeout::registNextTimer(int p1, int p2, unsigned int p3)
{
    // [b28d-30] ORIG 0x0863ae10：getCurSec 返回值无槽往返（call 后直接
    // `mov 0x10(%ebp),%edx; mov %edx,%ecx; sub %eax,%ecx` 三步链，delta 落具名槽）
    // ⇒ P0 表达式直用：删 cur 槽、无显式 cast（探针 /tmp/probe30_c.cpp r1
    // 逐条复现 ORIG；旧「表达式形反劣」判词源于带 cast 的 r3 形，作废）。
    int delta = p3 - GlobalData::s_systemTime_.getCurSec();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x95, delta,
                                p2, 0);
}