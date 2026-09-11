// ORIG T _ZN32TimerUpdateCompoundEmblemStatics15registNextTimerEv @0x0863939a
// [b28d-30] 复核 DIFF：localtime_r 返回指针视图/槽差（P43/N82 族），ORIG 0x0863939a。
// [b28d-solo-TimerDispatcher] 判词 N1 封顶：74/73。修法：①P43 指针访问已修；
// ②§4.3 具名化 `*(const int*)((const char*)G_CEnvironment() + 0x1b0)` →
// `G_CEnvironment()->m_serverEnvir.m_channelNo`（CEnvironment.h:132，ORIG
// `mov 0x1b0(%eax),%ecx` 单条 disp 已匹配）。残差 = `%0x1e` 常量除法展开
// lea+shl+sub vs add+shl+sub（源形不可择）⇒ N1。
// [TimerDispatcher-2 2026-09-09 复核] 同 TimerDungeonDataStatistic::RegistNextTimer：
// N86b/N1（ORIG-only lea/mov vs OURS-only add 各 1）⇒ 封顶，无语义缺口。
// [TimerDispatcher-3 2026-09-10 判据①] 同族：唯一 jg 目标差 1 = lea↔add 单条
// （探针 r1..r6 六形恒 add），分支语义等价。
int TimerUpdateCompoundEmblemStatics::registNextTimer()
{
    // [TimerDispatcher-4 2026-09-11] 同 TimerDungeonDataStatistic：唯一真残差 =
    // `%0x1e` 余数段 lea↔add（N86b/N1，22 形探针封底）；p167：6=6 CLEAR。
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
}