// ORIG 0x8125ff4：按 _DailySchedule 时间窗与 CEventManager::GetRepeatEvent
// 判定条目应处状态；返回 1=进入（或进行中），0=不进入。
// [b28d-solo-CDailyScheduleManager] 已收：两处 GetRepeatEvent 实参经 m_eventIdE（enum 视图）
// 复现 ORIG `mov (%eax),%eax; mov %eax,%edx` 两段（P254）。
// 残差 1 条 mov 判 N18b 封顶：ORIG `mov 0x14(%eax),%eax; mov %eax,%edx; mov -0x18(%ebp),%eax;
// cmp %eax,%edx`，4.4 对成员==局部比较恒折 `mov 0x14(%eax),%edx` 单段；字段 int/unsigned/
// enum 视图 × 局部 int/unsigned/long/enum 十二形实测均不翻（P248 三档试尽）。
// [N5Free211to230.N5D 2026-09-11] 残差唯一 = 4.1 独有 regmove，判 **N18a** 封顶：
//   ORIG 0x08126150..0x08126158（4 条）/ 4.4 现形 0x381（3 条）= gap +1。
//   本波新增证伪记录（/tmp/r5/n5/p_sched4.cpp、p_sched5.cpp，7 形矩阵 + enum 字段）：
//   F_uint|F_int × L_uint|L_int|L_long、反向写法、`(int)` cast、真实 enum 字段（SDE）
//   全部产出 `mov (%eax),%eax; cmp %eax,-N(%ebp)`（mem 直比，**比现形还少 1 条**）
//   ⇒ 把 m_wday 改 enum（P254 档）在本「成员==局部」比较位是**负收益**，勿再试；
//   P254 三例（JobKey/ExpertJobKey/InfoDaily）均为 map 键构造/实参位，不适用于比较位。
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
    // [N5D] 上述②为唯一残差（ORIG 136 / 现形 135）；7 形类型矩阵探针全败 ⇒ N18a 封顶，
    // 不再在本位引入 (int)/enum/unsigned 变换（T5：类型不是排序旋钮）。

    if (result == 0)
        out = 0;
    else
        out = static_cast<unsigned int>(end - cur);

    return result;
}