void CCoinRefillEvent::registNextRefillTimer() const
{
    // [b28d-59 判词] 封 N82/P231（ORIG 0x0832c1fe）：93/93 条、P167 调用点 9/9 CLEAR
    // ⇒ 语义完成；残差=帧 0x94 vs 0xa8 + ORIG 用 %ebx 跨调用保活（callee-saved）。
    // byte 层封顶。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    tm cur;
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    tm next = cur;
    next.tm_hour = getNextEventTime(cur);
    if (next.tm_hour < cur.tm_hour)
    {
        next.tm_mday = next.tm_mday + 1;
    }
    next.tm_min = 0;
    next.tm_sec = 0;
    time_t nextSec = mktime(&next);
    if (nextSec - static_cast<time_t>(curSec) >= 0)
    {
        int delay = static_cast<int>(nextSec) - curSec;
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                    static_cast<TIMER_MESSAGE>(0x65), delay, 0, 0);
        char* timeStr = ctime(&nextSec);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x88, 0);
        trace("Next coin refill time is %s", timeStr);
    }
