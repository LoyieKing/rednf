int CDailyScheduleManager::CheckScheduleState(const _DailySchedule& entry,
                                              unsigned int& out, bool flag) const
{
    (void)flag;

    std::tm tm;
    const std::time_t cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tm);
    const int wday = tm.tm_wday;

    int result = 0;

    if (entry.m_eventId != 0)
    {
        // ORIG 0x812603b：事件管理器为空或 GetRepeatEvent 为空 → 物化布尔后
        // out=0 返回 0；否则再调 GetRepeatEvent 经 vptr+0x34 判活（IsEventing）。
        if (GlobalData::s_event_manager == 0 ||
            GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventIdE) == 0)
        {
            out = 0;
            return 0;
        }
        if (!GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventIdE)->IsEventing(0))
        {
            out = 0;
            return 0;
        }
    }

    // 起止时刻（当日 h/m/s）基于当前日期求纪元秒，复用同一 tm（ORIG 行为）。
    tm.tm_hour = entry.m_startHour;
    tm.tm_min = entry.m_startMin;
    tm.tm_sec = entry.m_startSec2;
    const std::time_t start = mktime(&tm);

    tm.tm_hour = entry.m_endHour;
    tm.tm_min = entry.m_endMin;
    tm.tm_sec = entry.m_endSec2;
    const std::time_t end = mktime(&tm);

    if (entry.m_startSec <= cur && cur <= entry.m_endSec &&
        (entry.m_wday == wday || entry.m_wday == 7) &&
        cur >= start && cur <= end)
    {
        result = 1;
    }
    // 残差 ndiff 10（136→132）定性：
    // ①N18a×2：ORIG GetRepeatEvent 实参 `mov (%eax),%eax; mov %eax,%edx`
    // 双段 regmove，4.4 恒折 `mov (%eax),%edx` 单段（两处 call 前同形）；
    // ②N18b：`wday == entry.m_wday` ORIG 出 m_wday→edx、wday→eax 双物化
    // `cmp %eax,%edx`，4.4 恒折 `cmp -0x18(%ebp),%eax`（正反向写法均试）。
    // `cur >= start` 方向翻转已收（cmp start,cur+jg → cmp cur/start+jl 对齐）。

    if (result == 0)
        out = 0;
    else
        out = static_cast<unsigned int>(end - cur);

    return result;
