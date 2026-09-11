bool TimerStatisticLevel::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    G_GameWorld()->UpdateStatistic4ChannelUserLev();
    // b28c-28（P0 清绑 + P52 内联实参）：ORIG 0x086358ac 为
    // `push %ebx; sub $0x24` + `call CalcNextTime; mov %eax,%ebx; …;
    // call getCurSec; sub %eax,%ebx; …; mov %ebx,0x10(%esp)`——
    // delay 无栈槽、经 callee-saved 跨 getCurSec/G_TimerQueue 两 call 保活。
    // 具名局部（含旧 register 绑定）一律落槽 ⇒ 改实参位内联求值。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x55,
        CalcNextTime() - static_cast<int>(GlobalData::s_systemTime_.getCurSec()),
        0, 0);
    return 1;
}