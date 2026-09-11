// ORIG T 0x08633a04 (_ZN17TimerFatigueReset15RegistNextTimerEv)
// ORIG 非静态成员但不使用 this（调用点 0x086339d4 传 this）；返回 bool。
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组跨版本差（N7/N82 族），ORIG 0x086339d4。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：ORIG -0x4c(%ebp)→%ebx 缓存 vs 4.4 槽化
// + cMyTrace 槽 -0x38↔-0x48 + 帧 0x8c↔leave（95/94）。
bool TimerFatigueReset::RegistNextTimer()
{
    // ORIG 0x08633a17-0x08633a1c：`call getCurSec; mov %eax,-0x4c` 单段直存，
    // curSec 槽与 tmv(-0x78..-0x49) 高端相邻——试先声明取得更外槽位
    time_t curSec = GlobalData::s_systemTime_.getCurSec();   // -0x4c
    tm timeInfo;                                             // local_7c -0x78

    tm* pt = localtime_r(&curSec, &timeInfo);                // ORIG sret 形 pt 槽 -0x28

    const int hour = pt->tm_hour;                            // -0x24（08633a33 直读）
    // ORIG 0x08633a40/0x45/0x47：三字段各 `mov -0x28,%eax` 重读 + 单段直落 callee-saved
    // （无栈槽）⇒ P231/P307 纯 register（非 asm 绑定）。
    register int sec = pt->tm_sec;
    register int min = pt->tm_min;
    register int hr = pt->tm_hour;

    // ORIG 0x08633a71-0x08633a79：参数装载 edi→0x10/min→0xc/hr→0x8（右→左）
    cMyTrace(__PRETTY_FUNCTION__, 0x572, 0)(
        "Reset all user's fatigue!(%02d:%02d:%02d)", hr, min, sec);

    // env+0x37c 写回 tm_hour；min/sec 清零（ORIG 0x8(%eax)/0x4(%eax)/(%eax) 槽序）。
    pt->tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // env+0x37c
    pt->tm_min = 0;
    pt->tm_sec = 0;

    int nextMidnight = static_cast<int>(mktime(pt));         // -0x20
    if (G_CEnvironment()->m_serverEnvir.m_fatigueTime <= hour)
        nextMidnight += 0x15180;                             // ORIG addl 原地 -0x20

    const int delta = nextMidnight - static_cast<int>(curSec);  // -0x1c

    cMyTrace(__PRETTY_FUNCTION__, 0x587, 0)(
        "DELTA %d , CHECK_T %d , CUR_T %d\n", delta, nextMidnight, static_cast<int>(curSec));

    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2,
                                0,
                                (TIMER_MESSAGE)0x50,
                                delta,
                                0,
                                0);
    return 1;
}