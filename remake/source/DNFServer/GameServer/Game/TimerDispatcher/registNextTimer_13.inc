int TimerUpdateCompoundEmblemStatics::registNextTimer()
{
    const time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm t;
    // [b28d-solo-TimerDispatcher] P43：ORIG 经 localtime_r 返回值指针访问
    // （`mov -0x18(%ebp),%eax; mov 0x8(%eax),%eax` / mktime(p)），非直接读栈上 tm。
    tm* p = localtime_r(&now, &t);
    const int curHour = p->tm_hour;
    p->tm_hour = 5;
    p->tm_min = 0;
    p->tm_sec = 0;
    // §4.3 具名化：+0x1b0 = CEnvironment::m_serverEnvir(+0x198).m_channelNo(+0x18)
    // （CEnvironment.h:132 权威声明；ORIG `mov 0x1b0(%eax),%ecx` 单条 disp）。
    p->tm_min = G_CEnvironment()->m_serverEnvir.m_channelNo % 0x1e;
    int target = static_cast<int>(mktime(p));
    if (p->tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(now);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x89,
                                delay, 0, 0);
    return 1;
