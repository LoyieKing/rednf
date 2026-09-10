bool TimerLightServerNotice::RegistNextTimer(int idx)
{
    // ORIG：localtime_r 返回值 pt 具名存槽 -0x18，字段读写全经 pt 指针
    // （`mov -0x18,%eax; mov %edx,0x8(%eax)` 形），非 cur 折叠；curSec 槽 -0x1c。
    // G_CDataManager()+0xa4 的 vector operator[] 按 P48 每用点重取（tm_hour/tm_min
    // 各自独立 operator[] + G_CDataManager 调用），禁缓存 notice 引用。
    time_t curSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm cur;
    tm* pt = localtime_r(&curSec, &cur);
    int prevHour = pt->tm_hour;
    // P48：两处各自主调用 G_CDataManager()+0xa4 operator[]（ORIG 双调用）。
    pt->tm_hour = G_CDataManager()->m_serverParameter.m_light3c[idx].m_hour;
    pt->tm_min = G_CDataManager()->m_serverParameter.m_light3c[idx].m_min;
    pt->tm_sec = 0;
    int next = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = static_cast<int>(next - curSec);
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x68), delay, idx, 0);
    return 1;
