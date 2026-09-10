void TimerUpdatePvPGrade::registNextTimer(long sec)
{
    if (G_CEnvironment()->get_channel_no() != 1)
        return;
    tm timeInfo;
    localtime_r(&sec, &timeInfo);
    timeInfo.tm_hour = 2;
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 0;
    sec = static_cast<long>(mktime(&timeInfo));   // ORIG mov %eax,0x8(%ebp) 回写参数槽
    // b28c-28（P0 清绑 + P52 内联实参）：ORIG 0x08637449 段 =
    // `mov 0x8(%ebp),%ebx; call getCurSec; mov %ebx,%edx; sub %eax,%edx;
    // mov %edx,%eax; test; jle` → then 臂 `mov 0x8(%ebp),%ebx; call getCurSec;
    // sub %eax,%ebx` / else 臂 `mov $0x1,%ebx`，delay 无槽、经 %ebx 跨
    // G_TimerQueue call ⇒ 三目整体内联到 InsertTimer 实参位（两臂各自重调
    // getCurSec，P48 无 CSE）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x6c,
        sec - static_cast<long>(GlobalData::s_systemTime_.getCurSec()) > 0
            ? static_cast<int>(sec -
                               static_cast<long>(GlobalData::s_systemTime_.getCurSec()))
            : 1,
        0, 0);
