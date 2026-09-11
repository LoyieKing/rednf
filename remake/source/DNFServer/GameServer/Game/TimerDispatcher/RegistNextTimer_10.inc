// ORIG T 0x08639554 _ZN14Timer_Midnight15RegistNextTimerEv
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组/帧差（N7/N82 族），ORIG 0x08639554。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：push edi/esi/ebx 缓存组 + 槽差
// （-0x3c↔-0x2c 等）（71/72）。
bool Timer_Midnight::RegistNextTimer()
{
    // ORIG 0x08639554：curSec time_t 单槽 -0x3c（免 int→long 拷贝）；pt 槽 -0x28
    // 各点 reload；hourNow 槽 -0x24；sec/min/hour 直落 edi/esi/ebx（绑定，N18a
    // 两段残差）跨 cMyTrace ctor 存活
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    std::tm timePoint;
    std::tm* pt = localtime_r(&nowSec, &timePoint);
    const int hourNow = pt->tm_hour;
    // [b28d-solo] P231/P307 纯 register（非 asm 绑定）：ORIG 三值直落 edi/esi/ebx
    // 跨 cMyTrace ctor 保活无栈槽（086395xx mov (%eax),%edi / 0x4,%esi / 0x8,%ebx）。
    register int secNow = pt->tm_sec;
    register int minNow = pt->tm_min;
    register int hrNow = pt->tm_hour;
    cMyTrace(__PRETTY_FUNCTION__, 0x1293, 0)(
        "Reset all user's fatigue!(%02d:%02d:%02d)", hrNow, minNow, secNow);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    // ORIG 0863964x：`call mktime; mov %eax,-0x20; addl $0x15180,-0x20`（单槽原地自增，
    // 无独立 midnight 局部）⇒ 具名 nextAt + 复合赋值（旧双局部多 1 槽 + 2 insn）。
    int nextAt = static_cast<int>(mktime(pt));
    nextAt += 0x15180;
    const int delay = nextAt - static_cast<int>(nowSec);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8d, delay, 0, 0);
    return true;
}