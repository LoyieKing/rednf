bool TimerDungeonDataStatistic::RegistNextTimer()
{
    // ORIG 0x086368xx：curSec 单 time_t 槽（免 int→long 拷贝）；localtime_r 返回
    // pt 落槽 -0x18，各字段经 `mov -0x18,%eax` 重读 + 指针 disp 读写（技巧37 反用：
    // 具名指针槽正是 ORIG 形）
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    tm t;
    tm* pt = localtime_r(&curSec, &t);
    const int curHour = pt->tm_hour;
    pt->tm_hour = 5;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    // ORIG 0x086368a7-0x086368e1：chn 全程 %ecx、x=chn/30（0x88888889+sar4 魔数）
    // 存 %ebx（sign 修正后）、2x=%edx；`shl $4; sub %edx` =30x；
    // tm_min = chn-30x = chn%30（P196：旧 /15*45 形在 chn≥15 分叉系误判）。
    // P0：ORIG chn/x/x2 全程寄存器无槽（086368a7 `mov 0x1b0(%eax),%ecx` 直入
    // 魔数除法）⇒ 表达式内联为 `% 0x1e`（旧三个具名局部多 3 槽 + 全帧平移）。
    pt->tm_min = G_CEnvironment()->m_serverEnvir.m_channelNo % 0x1e;  // +0x1b0
    int target = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(curSec);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66,
                                delay, 0, 0);
    return 1;
