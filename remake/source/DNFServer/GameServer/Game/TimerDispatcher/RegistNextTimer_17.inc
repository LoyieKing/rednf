void Timer_Schedule_MidNight::RegistNextTimer()
{
    // ORIG 0x0863a9b4：getCurSec 结果寄存器内 add 后单落槽 -0x28(%ebp)。
    time_t sec = (time_t)(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    struct tm v;                                 // ORIG v@-0x24（比 sec 近端）
    struct tm* pt = localtime_r(&sec, &v);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    int next = (int)mktime(pt);
    int delta = next - GlobalData::s_systemTime_.getCurSec();
    // [b28c-31] ORIG 0x0863aa1c：sec=-0x28 / cMyTrace=-0x24（标量深、聚合浅）；
    // 具名 trace 4.4 反置（trace=-0x28/sec=-0x18，N82附）⇒ 试全表达式临时链（P221）。
    cMyTrace(__PRETTY_FUNCTION__, 0x1626, 0)(
        "RegistNextTimer Schedule_MidNight delta %d\n", delta);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x9a,
                                delta, 0, 0);
