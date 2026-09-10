bool TimerInsertUserPlayTime::RegistNextTimer()
{
    // ORIG 0x08636：curSec 落槽 -0x1c（time_t 直传 &）；localtime_r 返回值具名 pt
    // 槽 -0x18；prevHour -0x14 / next -0x10；全部字段读写经 pt 指针
    // （`mov -0x18,%eax; movl $0x4,0x8(%eax)` 形，ORIG 0x08636xxx），非 cur 折叠。
    time_t curSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm cur;
    tm* pt = localtime_r(&curSec, &cur);
    int prevHour = pt->tm_hour;
    pt->tm_hour = 4;
    pt->tm_min = 0x1e;
    pt->tm_sec = 0;
    int next = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = static_cast<int>(next - curSec);
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x69), delay, 0, 0);
    return 1;
